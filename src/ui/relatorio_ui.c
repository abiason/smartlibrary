#include "relatorio_ui.h"

#include "services/relatorio_service.h"
#include "ui/console_ui.h"
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
    ui_header("RELATORIOS", "Menu");
    ui_menu_item(1, "Acervo por status");
    ui_menu_item(2, "Emprestimos atrasados");
    ui_menu_item(3, "Livros mais emprestados");
    ui_menu_item(4, "Emprestimos por origem");
    ui_menu_item(5, "Reservas ativas");
    ui_menu_item(6, "Usuarios com pendencias");
    ui_menu_back("Voltar");
    putchar('\n');
}

void relatorio_ui_run(PostgresConnection *postgres) {
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


