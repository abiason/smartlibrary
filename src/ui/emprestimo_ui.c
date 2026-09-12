#include "emprestimo_ui.h"

#include "services/emprestimo_service.h"
#include "ui/console_ui.h"
#include "utils/input.h"

#include <stdio.h>

static void realizar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo) {
    int usuario_id = 0;
    char codigo_barras[51];

    input_read_int("ID do usuario: ", &usuario_id);
    input_read_line("Codigo de barras do exemplar: ", codigo_barras, sizeof(codigo_barras));
    emprestimo_service_realizar_emprestimo(postgres, mongo, usuario_id, codigo_barras);
}

static void realizar_devolucao(PostgresConnection *postgres, MongoConnection *mongo) {
    char codigo_barras[51];

    input_read_line("Codigo de barras do exemplar: ", codigo_barras, sizeof(codigo_barras));
    emprestimo_service_realizar_devolucao(postgres, mongo, codigo_barras);
}

static void renovar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo) {
    int emprestimo_item_id = 0;

    emprestimo_service_listar_abertos(postgres);
    input_read_int("ID do item de emprestimo: ", &emprestimo_item_id);
    emprestimo_service_renovar_item(postgres, mongo, emprestimo_item_id);
}

static void print_menu(void) {
    ui_header("CIRCULACAO", "Menu");
    ui_menu_item(1, "Realizar emprestimo");
    ui_menu_item(2, "Realizar devolucao");
    ui_menu_item(3, "Renovar emprestimo");
    ui_menu_item(4, "Listar emprestimos abertos");
    ui_menu_back("Voltar");
    putchar('\n');
}

void emprestimo_ui_run(PostgresConnection *postgres, MongoConnection *mongo) {
    int option = -1;

    while (option != 0) {
        ui_clear();
        print_menu();
        if (!input_read_int("Escolha uma opcao: ", &option)) {
            ui_error("Opcao invalida.");
            input_wait_enter();
            continue;
        }

        switch (option) {
            case 1:
                realizar_emprestimo(postgres, mongo);
                break;
            case 2:
                realizar_devolucao(postgres, mongo);
                break;
            case 3:
                renovar_emprestimo(postgres, mongo);
                break;
            case 4:
                emprestimo_service_listar_abertos(postgres);
                break;
            case 0:
                ui_info("Voltando ao menu principal.");
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


