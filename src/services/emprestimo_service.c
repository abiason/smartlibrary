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

int emprestimo_service_realizar_emprestimo_origem(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, const char *codigo_barras, const char *origem) {
    int emprestimo_id = 0;
    int exemplar_id = 0;
    char motivo[256];
    char entidade_id[32];

    if (!has_postgres(postgres) || usuario_id <= 0 || !has_text(codigo_barras)) {
        fprintf(stderr, "[ERRO] Informe usuario e codigo de barras validos.\n");
        event_service_registrar_log(mongo, "WARN", "emprestimo_service", "", "Tentativa de emprestimo com dados invalidos.");
        return 0;
    }

    if (!emprestimo_repository_realizar_emprestimo(postgres->conn, usuario_id, codigo_barras, &emprestimo_id, &exemplar_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar_origem(mongo, "EMPRESTIMO_NEGADO", origem, usuario_id, emprestimo_id, exemplar_id, codigo_barras);
        event_service_registrar_log(mongo, "WARN", "emprestimo_service", "", motivo);
        return 0;
    }

    snprintf(entidade_id, sizeof(entidade_id), "%d", emprestimo_id);
    event_service_registrar_origem(mongo, "EMPRESTIMO_REALIZADO", origem, usuario_id, emprestimo_id, exemplar_id, codigo_barras);
    event_service_registrar_auditoria(mongo, "emprestimo", entidade_id, "CRIACAO", usuario_id, "", "status=ABERTO");
    printf("[OK] %s ID do emprestimo: %d\n", motivo, emprestimo_id);
    return 1;
}

int emprestimo_service_realizar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, const char *codigo_barras) {
    return emprestimo_service_realizar_emprestimo_origem(postgres, mongo, usuario_id, codigo_barras, "BALCAO");
}

int emprestimo_service_realizar_devolucao_origem(PostgresConnection *postgres, MongoConnection *mongo, const char *codigo_barras, const char *origem) {
    int usuario_id = 0;
    int emprestimo_id = 0;
    int exemplar_id = 0;
    char motivo[256];
    char entidade_id[32];

    if (!has_postgres(postgres) || !has_text(codigo_barras)) {
        fprintf(stderr, "[ERRO] Informe codigo de barras valido.\n");
        event_service_registrar_log(mongo, "WARN", "emprestimo_service", "", "Tentativa de devolucao com codigo invalido.");
        return 0;
    }

    if (!emprestimo_repository_realizar_devolucao(postgres->conn, codigo_barras, &usuario_id, &emprestimo_id, &exemplar_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar_origem(mongo, "DEVOLUCAO_NEGADA", origem, usuario_id, emprestimo_id, exemplar_id, codigo_barras);
        event_service_registrar_log(mongo, "WARN", "emprestimo_service", "", motivo);
        return 0;
    }

    snprintf(entidade_id, sizeof(entidade_id), "%d", emprestimo_id);
    event_service_registrar_origem(mongo, "DEVOLUCAO_REALIZADA", origem, usuario_id, emprestimo_id, exemplar_id, codigo_barras);
    event_service_registrar_auditoria(mongo, "emprestimo", entidade_id, "ALTERACAO", usuario_id, "status=ABERTO", "item devolvido");
    printf("[OK] %s\n", motivo);
    return 1;
}

int emprestimo_service_realizar_devolucao(PostgresConnection *postgres, MongoConnection *mongo, const char *codigo_barras) {
    return emprestimo_service_realizar_devolucao_origem(postgres, mongo, codigo_barras, "BALCAO");
}

int emprestimo_service_renovar_item_origem(PostgresConnection *postgres, MongoConnection *mongo, int emprestimo_item_id, const char *origem) {
    int usuario_id = 0;
    int emprestimo_id = 0;
    int exemplar_id = 0;
    char codigo_barras[51];
    char motivo[256];
    char entidade_id[32];

    codigo_barras[0] = '\0';
    if (!has_postgres(postgres) || emprestimo_item_id <= 0) {
        fprintf(stderr, "[ERRO] Informe item de emprestimo valido.\n");
        event_service_registrar_log(mongo, "WARN", "emprestimo_service", "", "Tentativa de renovacao com item invalido.");
        return 0;
    }

    if (!emprestimo_repository_renovar_item(postgres->conn, emprestimo_item_id, &usuario_id, &emprestimo_id, &exemplar_id, codigo_barras, sizeof(codigo_barras), motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar_origem(mongo, "RENOVACAO_NEGADA", origem, usuario_id, emprestimo_id, exemplar_id, codigo_barras);
        event_service_registrar_log(mongo, "WARN", "emprestimo_service", "", motivo);
        return 0;
    }

    snprintf(entidade_id, sizeof(entidade_id), "%d", emprestimo_item_id);
    event_service_registrar_origem(mongo, "RENOVACAO_REALIZADA", origem, usuario_id, emprestimo_id, exemplar_id, codigo_barras);
    event_service_registrar_auditoria(mongo, "emprestimo_item", entidade_id, "ALTERACAO", usuario_id, "renovacao anterior", "renovacao incrementada");
    printf("[OK] %s\n", motivo);
    return 1;
}

int emprestimo_service_renovar_item(PostgresConnection *postgres, MongoConnection *mongo, int emprestimo_item_id) {
    return emprestimo_service_renovar_item_origem(postgres, mongo, emprestimo_item_id, "BALCAO");
}

void emprestimo_service_listar_abertos(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(emprestimo_repository_listar_emprestimos_abertos(postgres->conn));
    }
}
