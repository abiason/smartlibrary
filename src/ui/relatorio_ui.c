#include "relatorio_ui.h"

#include "services/relatorio_service.h"
#include "utils/input.h"

#include <stdio.h>

static int read_limit(void) {
    int limite = 10;
    if (!input_read_int("Limite de linhas: ", &limite) || limite <= 0) {
        limite = 10;
    }
    return limite;
}

static void print_menu(void) {
    printf("\n===================================\n");
    printf("RELATORIOS\n");
    printf("===================================\n");
    printf("1 - Acervo por status\n");
    printf("2 - Emprestimos atrasados\n");
    printf("3 - Livros mais emprestados\n");
    printf("4 - Emprestimos por origem\n");
    printf("5 - Reservas ativas\n");
    printf("6 - Usuarios com pendencias\n");
    printf("0 - Voltar\n");
}

void relatorio_ui_run(PostgresConnection *postgres) {
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
                relatorio_service_acervo_por_status(postgres);
                break;
            case 2:
                relatorio_service_emprestimos_atrasados(postgres);
                break;
            case 3:
                relatorio_service_livros_mais_emprestados(postgres, read_limit());
                break;
            case 4:
                relatorio_service_emprestimos_por_origem(postgres);
                break;
            case 5:
                relatorio_service_reservas_ativas(postgres);
                break;
            case 6:
                relatorio_service_usuarios_com_pendencias(postgres);
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


