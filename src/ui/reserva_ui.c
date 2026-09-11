#include "reserva_ui.h"

#include "services/reserva_service.h"
#include "utils/input.h"

#include <stdio.h>

static void criar_reserva(PostgresConnection *postgres, MongoConnection *mongo) {
    int usuario_id = 0;
    int livro_id = 0;
    input_read_int("ID do usuario: ", &usuario_id);
    input_read_int("ID do livro: ", &livro_id);
    reserva_service_criar(postgres, mongo, usuario_id, livro_id);
}

static void cancelar_reserva(PostgresConnection *postgres, MongoConnection *mongo) {
    int reserva_id = 0;
    input_read_int("ID da reserva: ", &reserva_id);
    reserva_service_cancelar(postgres, mongo, reserva_id);
}

static void atender_reserva(PostgresConnection *postgres, MongoConnection *mongo) {
    int reserva_id = 0;
    input_read_int("ID da reserva: ", &reserva_id);
    reserva_service_atender(postgres, mongo, reserva_id);
}

static void print_menu(void) {
    printf("\n===================================\n");
    printf("RESERVAS\n");
    printf("===================================\n");
    printf("1 - Criar reserva\n");
    printf("2 - Cancelar reserva\n");
    printf("3 - Atender reserva\n");
    printf("4 - Expirar reservas vencidas\n");
    printf("5 - Listar reservas\n");
    printf("0 - Voltar\n");
}

void reserva_ui_run(PostgresConnection *postgres, MongoConnection *mongo) {
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
                criar_reserva(postgres, mongo);
                break;
            case 2:
                cancelar_reserva(postgres, mongo);
                break;
            case 3:
                atender_reserva(postgres, mongo);
                break;
            case 4:
                reserva_service_expirar_vencidas(postgres, mongo);
                break;
            case 5:
                reserva_service_listar(postgres);
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


