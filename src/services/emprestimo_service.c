#include "emprestimo_service.h"

#include "events/event_service.h"
#include "repositories/emprestimo_repository.h"

#include <libpq-fe.h>
#include <stdio.h>

static int has_text(const char *value) {
    return value != NULL && value[0] != '\0';
}

static int has_postgres(PostgresConnection *postgres) {
    return postgres != NULL && postgres->conn != NULL;
}

static void print_rows(PGresult *result) {
    int rows;
    int cols;

    if (result == NULL) {
        return;
    }

    rows = PQntuples(result);
    cols = PQnfields(result);
    if (rows == 0) {
        printf("[INFO] Nenhum emprestimo aberto encontrado.\n");
        PQclear(result);
        return;
    }

    for (int col = 0; col < cols; col++) {
        printf("%-24s", PQfname(result, col));
    }
    printf("\n");
    for (int col = 0; col < cols; col++) {
        printf("------------------------");
    }
    printf("\n");

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            printf("%-24s", PQgetvalue(result, row, col));
        }
        printf("\n");
    }

    PQclear(result);
}

int emprestimo_service_realizar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, const char *codigo_barras) {
    int emprestimo_id = 0;
    int exemplar_id = 0;
    char motivo[256];

    if (!has_postgres(postgres) || usuario_id <= 0 || !has_text(codigo_barras)) {
        fprintf(stderr, "[ERRO] Informe usuario e codigo de barras validos.\n");
        return 0;
    }

    if (!emprestimo_repository_realizar_emprestimo(postgres->conn, usuario_id, codigo_barras, &emprestimo_id, &exemplar_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar(mongo, "EMPRESTIMO_NEGADO", usuario_id, emprestimo_id, exemplar_id, codigo_barras);
        return 0;
    }

    event_service_registrar(mongo, "EMPRESTIMO_REALIZADO", usuario_id, emprestimo_id, exemplar_id, codigo_barras);
    printf("[OK] %s ID do emprestimo: %d\n", motivo, emprestimo_id);
    return 1;
}

int emprestimo_service_realizar_devolucao(PostgresConnection *postgres, MongoConnection *mongo, const char *codigo_barras) {
    int usuario_id = 0;
    int emprestimo_id = 0;
    int exemplar_id = 0;
    char motivo[256];

    if (!has_postgres(postgres) || !has_text(codigo_barras)) {
        fprintf(stderr, "[ERRO] Informe codigo de barras valido.\n");
        return 0;
    }

    if (!emprestimo_repository_realizar_devolucao(postgres->conn, codigo_barras, &usuario_id, &emprestimo_id, &exemplar_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar(mongo, "DEVOLUCAO_NEGADA", usuario_id, emprestimo_id, exemplar_id, codigo_barras);
        return 0;
    }

    event_service_registrar(mongo, "DEVOLUCAO_REALIZADA", usuario_id, emprestimo_id, exemplar_id, codigo_barras);
    printf("[OK] %s\n", motivo);
    return 1;
}
int emprestimo_service_renovar_item(PostgresConnection *postgres, MongoConnection *mongo, int emprestimo_item_id) {
    int usuario_id = 0;
    int emprestimo_id = 0;
    int exemplar_id = 0;
    char codigo_barras[51];
    char motivo[256];

    if (!has_postgres(postgres) || emprestimo_item_id <= 0) {
        fprintf(stderr, "[ERRO] Informe item de emprestimo valido.\n");
        return 0;
    }

    if (!emprestimo_repository_renovar_item(postgres->conn, emprestimo_item_id, &usuario_id, &emprestimo_id, &exemplar_id, codigo_barras, sizeof(codigo_barras), motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar(mongo, "RENOVACAO_NEGADA", usuario_id, emprestimo_id, exemplar_id, codigo_barras);
        return 0;
    }

    event_service_registrar(mongo, "RENOVACAO_REALIZADA", usuario_id, emprestimo_id, exemplar_id, codigo_barras);
    printf("[OK] %s\n", motivo);
    return 1;
}

void emprestimo_service_listar_abertos(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(emprestimo_repository_listar_emprestimos_abertos(postgres->conn));
    }
}
