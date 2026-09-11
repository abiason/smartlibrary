#include "config/config.h"
#include "database/mongodb.h"
#include "database/postgres.h"
#include "ui/cadastro_ui.h"

#include <stdio.h>

int main(void) {
    SmartLibraryConfig config;
    PostgresConnection postgres = {0};
    MongoConnection mongo = {0};
    int exit_code = 0;

    setvbuf(stdout, NULL, _IONBF, 0);
    setvbuf(stderr, NULL, _IONBF, 0);

    printf("===================================\n");
    printf("SMART LIBRARY SYSTEM\n");
    printf("===================================\n");
    printf("[INFO] Inicializando sistema...\n");

    if (!config_load(&config)) {
        fprintf(stderr, "[ERRO] Falha ao carregar configuracao.\n");
        return 1;
    }

    printf("[OK] Configuracao carregada.\n");
    config_print_summary(&config);

    mongodb_driver_init();

    if (!postgres_connect(&postgres, &config)) {
        exit_code = 1;
        goto cleanup;
    }
    printf("[OK] PostgreSQL conectado.\n");

    if (!mongodb_connect(&mongo, &config)) {
        exit_code = 1;
        goto cleanup;
    }
    printf("[OK] MongoDB conectado.\n");

    printf("[INFO] Sistema inicializado com sucesso.\n");
    cadastro_ui_run(&postgres);

cleanup:
    printf("[INFO] Encerrando conexoes...\n");
    mongodb_disconnect(&mongo);
    postgres_disconnect(&postgres);
    mongodb_driver_cleanup();

    if (exit_code == 0) {
        printf("[OK] Sistema finalizado.\n");
    } else {
        fprintf(stderr, "[ERRO] Sistema finalizado com falhas.\n");
    }

    return exit_code;
}


