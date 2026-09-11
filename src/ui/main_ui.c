#include "main_ui.h"

#include "ui/cadastro_ui.h"
#include "ui/emprestimo_ui.h"
#include "utils/input.h"

#include <stdio.h>

static void print_menu(void) {
    printf("\n===================================\n");
    printf("SMART LIBRARY - MENU PRINCIPAL\n");
    printf("===================================\n");
    printf("1 - Cadastros administrativos\n");
    printf("2 - Circulacao\n");
    printf("0 - Sair\n");
}

void main_ui_run(PostgresConnection *postgres, MongoConnection *mongo) {
    int option = -1;

    while (option != 0) {
        print_menu();
        if (!input_read_int("Opcao: ", &option)) {
            printf("[ERRO] Opcao invalida.\n");
            continue;
        }

        switch (option) {
            case 1:
                cadastro_ui_run(postgres);
                break;
            case 2:
                emprestimo_ui_run(postgres, mongo);
                break;
            case 0:
                printf("[INFO] Encerrando aplicacao.\n");
                break;
            default:
                printf("[ERRO] Opcao invalida.\n");
                break;
        }
    }
}
