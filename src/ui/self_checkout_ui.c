#include "self_checkout_ui.h"

#include "events/event_service.h"
#include "services/self_checkout_service.h"
#include "utils/input.h"

#include <stdio.h>

static void print_header(void) {
    printf("\n=========================================\n");
    printf("SMART LIBRARY\n");
    printf("SELF CHECKOUT\n");
    printf("=========================================\n");
}

static void print_menu(const char *nome) {
    printf("\nUsuario: %s\n", nome);
    printf("1 - Realizar emprestimo\n");
    printf("2 - Realizar devolucao\n");
    printf("3 - Renovar emprestimo\n");
    printf("4 - Consultar emprestimos\n");
    printf("5 - Consultar reservas\n");
    printf("6 - Pesquisar livros\n");
    printf("0 - Encerrar atendimento\n");
}

static void realizar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id) {
    char codigo_barras[51];
    input_read_line("Codigo de barras ou RFID: ", codigo_barras, sizeof(codigo_barras));
    self_checkout_service_realizar_emprestimo(postgres, mongo, usuario_id, codigo_barras);
}

static void realizar_devolucao(PostgresConnection *postgres, MongoConnection *mongo) {
    char codigo_barras[51];
    input_read_line("Codigo de barras ou RFID: ", codigo_barras, sizeof(codigo_barras));
    self_checkout_service_realizar_devolucao(postgres, mongo, codigo_barras);
}

static void renovar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id) {
    int item_id = 0;
    self_checkout_service_listar_emprestimos(postgres, usuario_id);
    input_read_int("ID do item de emprestimo: ", &item_id);
    self_checkout_service_renovar_item(postgres, mongo, usuario_id, item_id);
}

static void pesquisar_livros(PostgresConnection *postgres) {
    char termo[151];
    input_read_line("Titulo ou ISBN: ", termo, sizeof(termo));
    self_checkout_service_pesquisar_livros(postgres, termo);
}

void self_checkout_ui_run(PostgresConnection *postgres, MongoConnection *mongo) {
    char cpf[15];
    char nome[151];
    int usuario_id = 0;
    int option = -1;

    print_header();
    event_service_registrar_origem(mongo, "SELF_CHECKOUT_INICIADO", "SELF_CHECKOUT", 0, 0, 0, "");
    input_read_line("CPF: ", cpf, sizeof(cpf));

    if (!self_checkout_service_identificar_usuario(postgres, mongo, cpf, &usuario_id, nome, sizeof(nome))) {
        event_service_registrar_origem(mongo, "SELF_CHECKOUT_FINALIZADO", "SELF_CHECKOUT", 0, 0, 0, "");
        return;
    }

    while (option != 0) {
        print_menu(nome);
        if (!input_read_int("Opcao: ", &option)) {
            printf("[ERRO] Opcao invalida.\n");
            continue;
        }

        switch (option) {
            case 1:
                realizar_emprestimo(postgres, mongo, usuario_id);
                break;
            case 2:
                realizar_devolucao(postgres, mongo);
                break;
            case 3:
                renovar_emprestimo(postgres, mongo, usuario_id);
                break;
            case 4:
                self_checkout_service_listar_emprestimos(postgres, usuario_id);
                break;
            case 5:
                self_checkout_service_listar_reservas(postgres, usuario_id);
                break;
            case 6:
                pesquisar_livros(postgres);
                break;
            case 0:
                printf("[INFO] Atendimento encerrado.\n");
                break;
            default:
                printf("[ERRO] Opcao invalida.\n");
                break;
        }
    }

    event_service_registrar_origem(mongo, "SELF_CHECKOUT_FINALIZADO", "SELF_CHECKOUT", usuario_id, 0, 0, "");
}

