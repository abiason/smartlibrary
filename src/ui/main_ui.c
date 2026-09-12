#include "main_ui.h"

#include "auth/auth_service.h"
#include "events/event_service.h"
#include "ui/cadastro_ui.h"
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
    printf("7 - Trocar senha\n");
    printf("0 - Sair\n");
}

static void trocar_senha(PostgresConnection *postgres, MongoConnection *mongo, const AuthSession *session) {
    char senha_atual[256];
    char nova_senha[256];
    char confirmacao[256];

    if (session == NULL || !session->authenticated) {
        printf("[ERRO] Sessao invalida. Faca login novamente.\n");
        return;
    }

    input_read_line("Senha atual: ", senha_atual, sizeof(senha_atual));
    input_read_line("Nova senha: ", nova_senha, sizeof(nova_senha));
    input_read_line("Confirme a nova senha: ", confirmacao, sizeof(confirmacao));

    if (strcmp(nova_senha, confirmacao) != 0) {
        printf("[ERRO] Confirmacao diferente da nova senha.\n");
        event_service_registrar_log(mongo, "WARN", "auth", "", "Troca de senha recusada por confirmacao divergente.");
        return;
    }

    if (!auth_service_trocar_senha(postgres, session->usuario_id, senha_atual, nova_senha)) {
        printf("[ERRO] Senha nao alterada. Verifique a senha atual e o tamanho minimo da nova senha.\n");
        event_service_registrar_log(mongo, "WARN", "auth", "", "Troca de senha recusada.");
        return;
    }

    printf("[OK] Senha alterada com sucesso.\n");
    event_service_registrar_log(mongo, "INFO", "auth", "", "Senha alterada com sucesso.");
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
            case 7:
                trocar_senha(postgres, mongo, session);
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