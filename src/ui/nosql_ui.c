#include "nosql_ui.h"

#include "events/event_service.h"
#include "ui/console_ui.h"
#include "utils/input.h"

#include <stdio.h>

static int read_limit(void) {
    int limite = 10;
    if (!ui_read_int("Limite", &limite) || limite <= 0) {
        limite = 10;
    }
    return limite;
}

static void print_menu(void) {
    ui_header("NOSQL - EVENTOS, LOGS E AUDITORIA", "Menu");
    ui_menu_item(1, "Listar eventos recentes");
    ui_menu_item(2, "Listar logs recentes");
    ui_menu_item(3, "Listar auditoria recente");
    ui_menu_item(4, "Resumo de eventos por tipo");
    ui_menu_back("Voltar");
    putchar('\n');
}

void nosql_ui_run(MongoConnection *mongo) {
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
                event_service_listar_eventos_recentes(mongo, read_limit());
                break;
            case 2:
                event_service_listar_logs_recentes(mongo, read_limit());
                break;
            case 3:
                event_service_listar_auditoria_recente(mongo, read_limit());
                break;
            case 4:
                event_service_resumir_eventos_por_tipo(mongo);
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


