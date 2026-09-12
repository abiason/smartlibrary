#include "reserva_service.h"

#include "events/event_service.h"
#include "repositories/reserva_repository.h"
#include "ui/console_ui.h"

#include <libpq-fe.h>
#include <stdio.h>

static int has_postgres(PostgresConnection *postgres) {
    return postgres != NULL && postgres->conn != NULL;
}

int reserva_service_criar(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, int livro_id) {
    char motivo[256];
    char entidade_id[64];

    if (!has_postgres(postgres) || usuario_id <= 0 || livro_id <= 0) {
        fprintf(stderr, "[ERRO] Informe usuario e livro validos.\n");
        event_service_registrar_log(mongo, "WARN", "reserva_service", "", "Tentativa de reserva com dados invalidos.");
        return 0;
    }
    if (!reserva_repository_criar(postgres->conn, usuario_id, livro_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar_log(mongo, "WARN", "reserva_service", "", motivo);
        return 0;
    }

    snprintf(entidade_id, sizeof(entidade_id), "usuario=%d;livro=%d", usuario_id, livro_id);
    event_service_registrar_origem(mongo, "RESERVA_REALIZADA", "BALCAO", usuario_id, 0, 0, "");
    event_service_registrar_auditoria(mongo, "reserva", entidade_id, "CRIACAO", usuario_id, "", "status=ATIVA");
    printf("[OK] %s\n", motivo);
    return 1;
}

int reserva_service_cancelar(PostgresConnection *postgres, MongoConnection *mongo, int reserva_id) {
    char motivo[256];
    char entidade_id[64];

    if (!has_postgres(postgres) || reserva_id <= 0) {
        fprintf(stderr, "[ERRO] Informe reserva valida.\n");
        event_service_registrar_log(mongo, "WARN", "reserva_service", "", "Tentativa de cancelamento com reserva invalida.");
        return 0;
    }
    if (!reserva_repository_cancelar(postgres->conn, reserva_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar_log(mongo, "WARN", "reserva_service", "", motivo);
        return 0;
    }

    snprintf(entidade_id, sizeof(entidade_id), "%d", reserva_id);
    event_service_registrar_origem(mongo, "RESERVA_CANCELADA", "BALCAO", 0, 0, 0, "");
    event_service_registrar_auditoria(mongo, "reserva", entidade_id, "ALTERACAO", 0, "status=ATIVA", "status=CANCELADA");
    printf("[OK] %s\n", motivo);
    return 1;
}

int reserva_service_atender(PostgresConnection *postgres, MongoConnection *mongo, int reserva_id) {
    char motivo[256];
    char entidade_id[64];

    if (!has_postgres(postgres) || reserva_id <= 0) {
        fprintf(stderr, "[ERRO] Informe reserva valida.\n");
        event_service_registrar_log(mongo, "WARN", "reserva_service", "", "Tentativa de atendimento com reserva invalida.");
        return 0;
    }
    if (!reserva_repository_atender(postgres->conn, reserva_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar_log(mongo, "WARN", "reserva_service", "", motivo);
        return 0;
    }

    snprintf(entidade_id, sizeof(entidade_id), "%d", reserva_id);
    event_service_registrar_origem(mongo, "RESERVA_ATENDIDA", "BALCAO", 0, 0, 0, "");
    event_service_registrar_auditoria(mongo, "reserva", entidade_id, "ALTERACAO", 0, "status=ATIVA", "status=ATENDIDA");
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
        event_service_registrar_origem(mongo, "RESERVA_EXPIRADA", "SISTEMA", 0, 0, 0, "");
        event_service_registrar_auditoria(mongo, "reserva", "vencidas", "ALTERACAO", 0, "status=ATIVA", "status=EXPIRADA");
    }
    printf("[OK] Reservas expiradas: %d\n", affected);
}

void reserva_service_listar(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        ui_print_pgresult_table(reserva_repository_listar(postgres->conn), "Nenhuma reserva encontrada.");
    }
}

