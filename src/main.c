#include "auth/auth_service.h"
#include "config/config.h"
#include "database/mongodb.h"
#include "database/postgres.h"
#include "events/event_service.h"
#include "ui/main_ui.h"
#include "utils/input.h"

#include <stdio.h>

static int criar_admin_inicial_se_necessario(PostgresConnection *postgres, MongoConnection *mongo) {
    char nome[151];
    char cpf[15];
    char email[151];
    char senha[256];
    int total_usuarios = auth_service_contar_usuarios(postgres);

    if (total_usuarios < 0) {
        return 0;
    }

    if (total_usuarios > 0) {
        return 1;
    }

    input_clear_screen();
    printf("===================================\n");
    printf("SMART LIBRARY - PRIMEIRO ACESSO\n");
    printf("===================================\n");
    printf("Nenhum usuario encontrado. Crie o administrador inicial.\n");
    input_read_line("Nome: ", nome, sizeof(nome));
    input_read_line("CPF: ", cpf, sizeof(cpf));
    input_read_line("E-mail: ", email, sizeof(email));
    input_read_line("Senha: ", senha, sizeof(senha));

    if (!auth_service_criar_admin_inicial(postgres, nome, cpf, email, senha)) {
        printf("[ERRO] Administrador inicial nao foi criado.\n");
        event_service_registrar_log(mongo, "ERROR", "auth", "", "Falha ao criar administrador inicial.");
        input_wait_enter();
        return 0;
    }

    printf("[OK] Administrador inicial criado. Use essas credenciais para entrar.\n");
    event_service_registrar_log(mongo, "INFO", "auth", "", "Administrador inicial criado.");
    input_wait_enter();
    return 1;
}

static int autenticar_operador(PostgresConnection *postgres, MongoConnection *mongo, AuthSession *session) {
    char cpf[15];
    char senha[256];

    for (int tentativa = 1; tentativa <= 3; tentativa++) {
        input_clear_screen();
        printf("===================================\n");
        printf("SMART LIBRARY - LOGIN\n");
        printf("===================================\n");
        input_read_line("CPF: ", cpf, sizeof(cpf));
        input_read_line("Senha: ", senha, sizeof(senha));

        if (auth_service_autenticar(postgres, cpf, senha, session)) {
            printf("[OK] Login realizado: %s (%s).\n", session->nome, session->perfil_nome);
            event_service_registrar_log(mongo, "INFO", "auth", "", "Login realizado com sucesso.");
            input_wait_enter();
            return 1;
        }

        printf("[ERRO] CPF ou senha invalidos, usuario inativo ou bloqueado.\n");
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

    printf("===================================\n");
    printf("SMART LIBRARY SYSTEM\n");
    printf("===================================\n");
    printf("[INFO] Inicializando sistema...\n");

    if (!config_load(&config)) {
        fprintf(stderr, "[ERRO] Falha ao carregar configuracao.\n");
        return 1;
    }

    printf("[OK] Configuracao carregada.\n");
    config_print_summary(&config);

    mongodb_driver_init();

    if (!postgres_connect(&postgres, &config)) {
        exit_code = 1;
        goto cleanup;
    }
    printf("[OK] PostgreSQL conectado.\n");

    if (!mongodb_connect(&mongo, &config)) {
        exit_code = 1;
        goto cleanup;
    }
    printf("[OK] MongoDB conectado.\n");

    printf("[INFO] Sistema inicializado com sucesso.\n");
    event_service_registrar_log(&mongo, "INFO", "main", "", "Sistema inicializado com sucesso.");

    if (!criar_admin_inicial_se_necessario(&postgres, &mongo)) {
        exit_code = 1;
        goto cleanup;
    }

    if (!autenticar_operador(&postgres, &mongo, &session)) {
        fprintf(stderr, "[ERRO] Acesso negado apos 3 tentativas.\n");
        exit_code = 1;
        goto cleanup;
    }

    main_ui_run(&postgres, &mongo, &session);

cleanup:
    event_service_registrar_log(&mongo, "INFO", "main", "", "Sistema em encerramento.");
    printf("[INFO] Encerrando conexoes...\n");
    mongodb_disconnect(&mongo);
    postgres_disconnect(&postgres);
    mongodb_driver_cleanup();

    if (exit_code == 0) {
        printf("[OK] Sistema finalizado.\n");
    } else {
        fprintf(stderr, "[ERRO] Sistema finalizado com falhas.\n");
    }

    return exit_code;
}