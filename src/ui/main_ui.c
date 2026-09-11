#include "main_ui.h"

#include "auth/auth_service.h"
#include "ui/cadastro_ui.h"
#include "ui/emprestimo_ui.h"
#include "ui/nosql_ui.h"
#include "ui/relatorio_ui.h"
#include "ui/reserva_ui.h"
#include "ui/self_checkout_ui.h"
#include "utils/input.h"

#include <stdio.h>

static int can_access_option(const AuthSession *session, int option) {
    if (auth_service_is_admin(session)) {
        return 1;
    }

    if (auth_service_is_bibliotecario(session)) {
        return option == 2 || option == 3 || option == 6;
    }

    if (auth_service_is_usuario(session)) {
        return option == 4;
    }

    return 0;
}

static void print_menu(const AuthSession *session) {
    printf("\n===================================\n");
    printf("SMART LIBRARY - MENU PRINCIPAL\n");
    printf("===================================\n");
    if (session != NULL && session->authenticated) {
        printf("Operador: %s (%s)\n", session->nome, session->perfil_nome);
        printf("-----------------------------------\n");
    }
    printf("1 - Cadastros administrativos\n");
    printf("2 - Circulacao\n");
    printf("3 - Reservas\n");
    printf("4 - Self Checkout\n");
    printf("5 - NoSQL: eventos, logs e auditoria\n");
    printf("6 - Relatorios\n");
    printf("0 - Sair\n");
}

void main_ui_run(PostgresConnection *postgres, MongoConnection *mongo, const AuthSession *session) {
    int option = -1;

    while (option != 0) {
        input_clear_screen();
        print_menu(session);
        if (!input_read_int("Opcao: ", &option)) {
            printf("[ERRO] Opcao invalida.\n");
            input_wait_enter();
            continue;
        }

        if (option != 0 && !can_access_option(session, option)) {
            printf("[ERRO] Acesso negado para o perfil atual.\n");
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
            case 0:
                printf("[INFO] Encerrando aplicacao.\n");
                break;
            default:
                printf("[ERRO] Opcao invalida.\n");
                break;
        }

        if (option != 0) {
            input_wait_enter();
        }
    }
}