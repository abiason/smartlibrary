#include "auth/auth_service.h"
#include "config/config.h"
#include "database/mongodb.h"
#include "database/postgres.h"
#include "events/event_service.h"
#include "ui/console_ui.h"
#include "ui/main_ui.h"
#include "utils/input.h"

#include <stdio.h>

static int garantir_admin_ativo(PostgresConnection *postgres, MongoConnection *mongo) {
    char nome[151];
    char cpf[15];
    char email[151];
    char senha[256];
    int total_usuarios = auth_service_contar_usuarios(postgres);
    int total_admins_ativos = auth_service_contar_admins_ativos(postgres);

    if (total_usuarios < 0 || total_admins_ativos < 0) {
        return 0;
    }

    if (total_admins_ativos > 0) {
        return 1;
    }

    ui_clear();
    ui_header("SMARTLIBRARY", "Primeiro Acesso");
    if (total_usuarios == 0) {
        ui_info("Nenhum usuario encontrado. Crie o administrador inicial.");
    } else {
        ui_warning("Nenhum administrador ativo encontrado. Crie um administrador de recuperacao.");
    }
    ui_prompt_label("Nome");
    input_read_line("", nome, sizeof(nome));
    ui_prompt_label("CPF");
    input_read_line("", cpf, sizeof(cpf));
    ui_prompt_label("E-mail");
    input_read_line("", email, sizeof(email));
    ui_prompt_label("Senha");
    input_read_line("", senha, sizeof(senha));

    if (!auth_service_criar_admin_inicial(postgres, nome, cpf, email, senha)) {
        ui_error("Administrador inicial nao foi criado.");
        event_service_registrar_log(mongo, "ERROR", "auth", "", "Falha ao criar administrador inicial.");
        input_wait_enter();
        return 0;
    }

    ui_success("Administrador ativo criado. Use essas credenciais para entrar.");
    event_service_registrar_log(mongo, "INFO", "auth", "", "Administrador ativo criado no primeiro acesso ou recuperacao.");
    input_wait_enter();
    return 1;
}

static int autenticar_operador(PostgresConnection *postgres, MongoConnection *mongo, AuthSession *session) {
    char cpf[15];
    char senha[256];

    for (int tentativa = 1; tentativa <= 3; tentativa++) {
        char contexto[64];

        snprintf(contexto, sizeof(contexto), "Login - tentativa %d de 3", tentativa);
        ui_clear();
        ui_header("SMARTLIBRARY", contexto);
        ui_prompt_label("CPF");
        input_read_line("", cpf, sizeof(cpf));
        ui_prompt_label("Senha");
        input_read_line("", senha, sizeof(senha));

        if (auth_service_autenticar(postgres, cpf, senha, session)) {
            char mensagem[256];

            snprintf(mensagem, sizeof(mensagem), "Login realizado: %s (%s).", session->nome, session->perfil_nome);
            ui_success(mensagem);
            event_service_registrar_log(mongo, "INFO", "auth", "", "Login realizado com sucesso.");
            input_wait_enter();
            return 1;
        }

        ui_error("Login recusado. Verifique CPF, senha e situacao do usuario.");
        event_service_registrar_log(mongo, "WARN", "auth", "", "Tentativa de login recusada.");
        input_wait_enter();
    }

    return 0;
}

int main(void) {
    SmartLibraryConfig config;
    PostgresConnection postgres = {0};
    MongoConnection mongo = {0};
    AuthSession session = {0};
    int exit_code = 0;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    ui_header("SMARTLIBRARY", "Inicializacao do Sistema");
    ui_info("Inicializando sistema...");

    if (!config_load(&config)) {
        ui_error("Falha ao carregar configuracao.");
        return 1;
    }

    ui_success("Configuracao carregada.");
    config_print_summary(&config);

    mongodb_driver_init();

    if (!postgres_connect(&postgres, &config)) {
        exit_code = 1;
        goto cleanup;
    }
    ui_success("PostgreSQL conectado.");

    if (!mongodb_connect(&mongo, &config)) {
        exit_code = 1;
        goto cleanup;
    }
    ui_success("MongoDB conectado.");

    ui_info("Sistema inicializado com sucesso.");
    event_service_registrar_log(&mongo, "INFO", "main", "", "Sistema inicializado com sucesso.");

    if (!garantir_admin_ativo(&postgres, &mongo)) {
        exit_code = 1;
        goto cleanup;
    }

    if (!autenticar_operador(&postgres, &mongo, &session)) {
        ui_error("Acesso negado apos 3 tentativas.");
        exit_code = 1;
        goto cleanup;
    }

    main_ui_run(&postgres, &mongo, &session);

cleanup:
    event_service_registrar_log(&mongo, "INFO", "main", "", "Sistema em encerramento.");
    ui_info("Encerrando conexoes...");
    mongodb_disconnect(&mongo);
    postgres_disconnect(&postgres);
    mongodb_driver_cleanup();

    if (exit_code == 0) {
        ui_success("Sistema finalizado.");
    } else {
        ui_error("Sistema finalizado com falhas.");
    }

    return exit_code;
}
