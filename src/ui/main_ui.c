#include "main_ui.h"

#include "auth/auth_service.h"
#include "events/event_service.h"
#include "ui/cadastro_ui.h"
#include "ui/console_ui.h"
#include "ui/emprestimo_ui.h"
#include "ui/nosql_ui.h"
#include "ui/relatorio_ui.h"
#include "ui/reserva_ui.h"
#include "ui/self_checkout_ui.h"
#include "utils/input.h"

#include <stdio.h>
#include <string.h>

static int can_access_option(const AuthSession *session, int option) {
    if (auth_service_is_admin(session)) {
        return 1;
    }

    if (auth_service_is_bibliotecario(session)) {
        return option == 2 || option == 3 || option == 6 || option == 7;
    }

    if (auth_service_is_usuario(session)) {
        return option == 4 || option == 7;
    }

    return 0;
}

static void print_menu(const AuthSession *session) {
    ui_header("SMARTLIBRARY", "Menu Principal");
    if (session != NULL && session->authenticated) {
        ui_context("Operador", session->nome);
        ui_context("Perfil", session->perfil_nome);
        putchar('\n');
    }
    ui_menu_item(1, "Cadastros administrativos");
    ui_menu_item(2, "Circulacao");
    ui_menu_item(3, "Reservas");
    ui_menu_item(4, "Self Checkout");
    ui_menu_item(5, "Eventos, logs e auditoria");
    ui_menu_item(6, "Relatorios");
    ui_menu_item(7, "Trocar senha");
    ui_menu_back("Sair");
    putchar('\n');
}

static void trocar_senha(PostgresConnection *postgres, MongoConnection *mongo, const AuthSession *session) {
    char senha_atual[256];
    char nova_senha[256];
    char confirmacao[256];

    if (session == NULL || !session->authenticated) {
        ui_error("Sessao invalida. Faca login novamente.");
        return;
    }

    ui_header("SMARTLIBRARY", "Trocar Senha");
    ui_prompt_label("Senha atual");
    input_read_line("", senha_atual, sizeof(senha_atual));
    ui_prompt_label("Nova senha");
    input_read_line("", nova_senha, sizeof(nova_senha));
    ui_prompt_label("Confirmacao");
    input_read_line("", confirmacao, sizeof(confirmacao));

    if (strcmp(nova_senha, confirmacao) != 0) {
        ui_error("Confirmacao diferente da nova senha.");
        event_service_registrar_log(mongo, "WARN", "auth", "", "Troca de senha recusada por confirmacao divergente.");
        return;
    }

    if (!auth_service_trocar_senha(postgres, session->usuario_id, senha_atual, nova_senha)) {
        ui_error("Senha nao alterada. Verifique os dados informados.");
        event_service_registrar_log(mongo, "WARN", "auth", "", "Troca de senha recusada.");
        return;
    }

    ui_success("Senha alterada com sucesso.");
    event_service_registrar_log(mongo, "INFO", "auth", "", "Senha alterada com sucesso.");
}

void main_ui_run(PostgresConnection *postgres, MongoConnection *mongo, const AuthSession *session) {
    int option = -1;

    while (option != 0) {
        ui_clear();
        print_menu(session);
        if (!input_read_int("Escolha uma opcao: ", &option)) {
            ui_error("Opcao invalida.");
            input_wait_enter();
            continue;
        }

        if (option != 0 && !can_access_option(session, option)) {
            ui_error("Acesso negado para o perfil atual.");
            input_wait_enter();
            continue;
        }

        switch (option) {
            case 1:
                cadastro_ui_run(postgres, mongo, session != NULL ? session->usuario_id : 0);
                break;
            case 2:
                emprestimo_ui_run(postgres, mongo);
                break;
            case 3:
                reserva_ui_run(postgres, mongo);
                break;
            case 4:
                self_checkout_ui_run(postgres, mongo);
                break;
            case 5:
                nosql_ui_run(mongo);
                break;
            case 6:
                relatorio_ui_run(postgres);
                break;
            case 7:
                trocar_senha(postgres, mongo, session);
                break;
            case 0:
                ui_info("Encerrando aplicacao.");
                break;
            default:
                ui_error("Opcao invalida.");
                break;
        }

        if (option != 0) {
            input_wait_enter();
        }
    }
}
