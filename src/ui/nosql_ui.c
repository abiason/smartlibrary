#include "nosql_ui.h"

#include "events/event_service.h"
#include "utils/input.h"

#include <stdio.h>

static int read_limit(void) {
    int limite = 10;
    if (!input_read_int("Limite de documentos: ", &limite) || limite <= 0) {
        limite = 10;
    }
    return limite;
}

static void print_menu(void) {
    printf("\n===================================\n");
    printf("NOSQL - EVENTOS, LOGS E AUDITORIA\n");
    printf("===================================\n");
    printf("1 - Listar eventos recentes\n");
    printf("2 - Listar logs recentes\n");
    printf("3 - Listar auditoria recente\n");
    printf("4 - Resumo de eventos por tipo\n");
    printf("0 - Voltar\n");
}

void nosql_ui_run(MongoConnection *mongo) {
    int option = -1;

    while (option != 0) {
        input_clear_screen();
        print_menu();
        if (!input_read_int("Opcao: ", &option)) {
            printf("[ERRO] Opcao invalida.\n");
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
                printf("[INFO] Voltando ao menu principal.\n");
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


