#include "self_checkout_ui.h"

#include "events/event_service.h"
#include "services/self_checkout_service.h"
#include "ui/console_ui.h"
#include "utils/input.h"

#include <stdio.h>

static void print_header(void) {
    ui_header("SMARTLIBRARY", "Self Checkout");
}

static void print_menu(const char *nome) {
    ui_header("SMARTLIBRARY", "Self Checkout");
    ui_context("Usuario", nome);
    putchar('\n');
    ui_menu_item(1, "Realizar emprestimo");
    ui_menu_item(2, "Realizar devolucao");
    ui_menu_item(3, "Renovar emprestimo");
    ui_menu_item(4, "Consultar emprestimos");
    ui_menu_item(5, "Consultar reservas");
    ui_menu_item(6, "Pesquisar livros");
    ui_menu_back("Encerrar atendimento");
    putchar('\n');
}

static void realizar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id) {
    char codigo_barras[51];
    ui_read_line("Codigo/RFID", codigo_barras, sizeof(codigo_barras));
    self_checkout_service_realizar_emprestimo(postgres, mongo, usuario_id, codigo_barras);
}

static void realizar_devolucao(PostgresConnection *postgres, MongoConnection *mongo) {
    char codigo_barras[51];
    ui_read_line("Codigo/RFID", codigo_barras, sizeof(codigo_barras));
    self_checkout_service_realizar_devolucao(postgres, mongo, codigo_barras);
}

static void renovar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id) {
    int item_id = 0;
    self_checkout_service_listar_emprestimos(postgres, usuario_id);
    ui_read_int("ID item", &item_id);
    self_checkout_service_renovar_item(postgres, mongo, usuario_id, item_id);
}

static void pesquisar_livros(PostgresConnection *postgres) {
    char termo[151];
    ui_read_line("Titulo/ISBN", termo, sizeof(termo));
    self_checkout_service_pesquisar_livros(postgres, termo);
}

void self_checkout_ui_run(PostgresConnection *postgres, MongoConnection *mongo) {
    char cpf[15];
    char nome[151];
    int usuario_id = 0;
    int option = -1;

    ui_clear();
    print_header();
    event_service_registrar_origem(mongo, "SELF_CHECKOUT_INICIADO", "SELF_CHECKOUT", 0, 0, 0, "");
    ui_read_line("CPF", cpf, sizeof(cpf));

    if (!self_checkout_service_identificar_usuario(postgres, mongo, cpf, &usuario_id, nome, sizeof(nome))) {
        event_service_registrar_origem(mongo, "SELF_CHECKOUT_FINALIZADO", "SELF_CHECKOUT", 0, 0, 0, "");
        return;
    }

    while (option != 0) {
        ui_clear();
        print_menu(nome);
        if (!input_read_int("Escolha uma opcao: ", &option)) {
            ui_error("Opcao invalida.");
            input_wait_enter();
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
                ui_info("Atendimento encerrado.");
                break;
            default:
                ui_error("Opcao invalida.");
                break;
        }

        if (option != 0) {
            input_wait_enter();
        }
    }

    event_service_registrar_origem(mongo, "SELF_CHECKOUT_FINALIZADO", "SELF_CHECKOUT", usuario_id, 0, 0, "");
}




