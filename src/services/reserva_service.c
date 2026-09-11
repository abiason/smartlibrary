#include "reserva_service.h"

#include "events/event_service.h"
#include "repositories/reserva_repository.h"

#include <libpq-fe.h>
#include <stdio.h>

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
        printf("[INFO] Nenhuma reserva encontrada.\n");
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

int reserva_service_criar(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, int livro_id) {
    char motivo[256];
    if (!has_postgres(postgres) || usuario_id <= 0 || livro_id <= 0) {
        fprintf(stderr, "[ERRO] Informe usuario e livro validos.\n");
        return 0;
    }
    if (!reserva_repository_criar(postgres->conn, usuario_id, livro_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        return 0;
    }
    event_service_registrar(mongo, "RESERVA_REALIZADA", usuario_id, 0, 0, "");
    printf("[OK] %s\n", motivo);
    return 1;
}

int reserva_service_cancelar(PostgresConnection *postgres, MongoConnection *mongo, int reserva_id) {
    char motivo[256];
    if (!has_postgres(postgres) || reserva_id <= 0) {
        fprintf(stderr, "[ERRO] Informe reserva valida.\n");
        return 0;
    }
    if (!reserva_repository_cancelar(postgres->conn, reserva_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        return 0;
    }
    event_service_registrar(mongo, "RESERVA_CANCELADA", 0, 0, 0, "");
    printf("[OK] %s\n", motivo);
    return 1;
}

int reserva_service_atender(PostgresConnection *postgres, MongoConnection *mongo, int reserva_id) {
    char motivo[256];
    if (!has_postgres(postgres) || reserva_id <= 0) {
        fprintf(stderr, "[ERRO] Informe reserva valida.\n");
        return 0;
    }
    if (!reserva_repository_atender(postgres->conn, reserva_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        return 0;
    }
    event_service_registrar(mongo, "RESERVA_ATENDIDA", 0, 0, 0, "");
    printf("[OK] %s\n", motivo);
    return 1;
}

void reserva_service_expirar_vencidas(PostgresConnection *postgres, MongoConnection *mongo) {
    int affected;
    if (!has_postgres(postgres)) {
        return;
    }
    affected = reserva_repository_expirar_vencidas(postgres->conn);
    if (affected > 0) {
        event_service_registrar(mongo, "RESERVA_EXPIRADA", 0, 0, 0, "");
    }
    printf("[OK] Reservas expiradas: %d\n", affected);
}

void reserva_service_listar(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(reserva_repository_listar(postgres->conn));
    }
}
