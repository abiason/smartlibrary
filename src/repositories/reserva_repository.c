#include "reserva_repository.h"

#include <stdio.h>
#include <stdlib.h>

static void set_motivo(char *motivo, int motivo_size, const char *texto) {
    if (motivo != NULL && motivo_size > 0) {
        snprintf(motivo, (size_t)motivo_size, "%s", texto);
    }
}

static PGresult *exec_params(PGconn *conn, const char *sql, int count, const char *const *params) {
    PGresult *result = PQexecParams(conn, sql, count, NULL, params, NULL, NULL, 0);
    ExecStatusType status = PQresultStatus(result);
    if (status != PGRES_TUPLES_OK && status != PGRES_COMMAND_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(conn));
        PQclear(result);
        return NULL;
    }
    return result;
}

static int scalar_int(PGresult *result) {
    int value = 0;
    if (result != NULL && PQntuples(result) > 0) {
        value = atoi(PQgetvalue(result, 0, 0));
    }
    return value;
}

int reserva_repository_criar(PGconn *conn, int usuario_id, int livro_id, char *motivo, int motivo_size) {
    char usuario_id_text[16];
    char livro_id_text[16];
    const char *params[2];
    PGresult *result;

    snprintf(usuario_id_text, sizeof(usuario_id_text), "%d", usuario_id);
    snprintf(livro_id_text, sizeof(livro_id_text), "%d", livro_id);
    params[0] = usuario_id_text;
    params[1] = livro_id_text;

    result = exec_params(conn, "SELECT COUNT(*) FROM usuario WHERE id_usuario = $1::integer AND ativo = true AND bloqueado = false", 1, params);
    if (scalar_int(result) == 0) {
        PQclear(result);
        set_motivo(motivo, motivo_size, "Usuario inexistente, inativo ou bloqueado.");
        return 0;
    }
    PQclear(result);

    result = exec_params(conn, "SELECT COUNT(*) FROM livro WHERE id_livro = $1::integer", 1, &params[1]);
    if (scalar_int(result) == 0) {
        PQclear(result);
        set_motivo(motivo, motivo_size, "Livro nao encontrado.");
        return 0;
    }
    PQclear(result);

    result = exec_params(conn, "SELECT COUNT(*) FROM reserva WHERE id_usuario = $1::integer AND id_livro = $2::integer AND status = 'ATIVA'", 2, params);
    if (scalar_int(result) > 0) {
        PQclear(result);
        set_motivo(motivo, motivo_size, "Usuario ja possui reserva ativa para este livro.");
        return 0;
    }
    PQclear(result);

    result = exec_params(conn, "INSERT INTO reserva (id_usuario, id_livro, data_expiracao, status) VALUES ($1::integer, $2::integer, CURRENT_TIMESTAMP + INTERVAL '3 days', 'ATIVA')", 2, params);
    if (result == NULL) {
        set_motivo(motivo, motivo_size, "Falha ao criar reserva.");
        return 0;
    }
    PQclear(result);
    set_motivo(motivo, motivo_size, "Reserva criada.");
    return 1;
}

int reserva_repository_cancelar(PGconn *conn, int reserva_id, char *motivo, int motivo_size) {
    char reserva_id_text[16];
    const char *params[1];
    PGresult *result;

    snprintf(reserva_id_text, sizeof(reserva_id_text), "%d", reserva_id);
    params[0] = reserva_id_text;
    result = exec_params(conn, "UPDATE reserva SET status = 'CANCELADA' WHERE id_reserva = $1::integer AND status = 'ATIVA'", 1, params);
    if (result == NULL) {
        set_motivo(motivo, motivo_size, "Falha ao cancelar reserva.");
        return 0;
    }
    if (atoi(PQcmdTuples(result)) == 0) {
        PQclear(result);
        set_motivo(motivo, motivo_size, "Reserva ativa nao encontrada.");
        return 0;
    }
    PQclear(result);
    set_motivo(motivo, motivo_size, "Reserva cancelada.");
    return 1;
}
int reserva_repository_atender(PGconn *conn, int reserva_id, char *motivo, int motivo_size) {
    char reserva_id_text[16];
    const char *params[1];
    PGresult *result;

    snprintf(reserva_id_text, sizeof(reserva_id_text), "%d", reserva_id);
    params[0] = reserva_id_text;
    result = exec_params(conn, "UPDATE reserva SET status = 'ATENDIDA' WHERE id_reserva = $1::integer AND status = 'ATIVA'", 1, params);
    if (result == NULL) {
        set_motivo(motivo, motivo_size, "Falha ao atender reserva.");
        return 0;
    }
    if (atoi(PQcmdTuples(result)) == 0) {
        PQclear(result);
        set_motivo(motivo, motivo_size, "Reserva ativa nao encontrada.");
        return 0;
    }
    PQclear(result);
    set_motivo(motivo, motivo_size, "Reserva atendida.");
    return 1;
}

int reserva_repository_expirar_vencidas(PGconn *conn) {
    PGresult *result = exec_params(conn,
        "UPDATE reserva SET status = 'EXPIRADA' WHERE status = 'ATIVA' AND data_expiracao IS NOT NULL AND data_expiracao < CURRENT_TIMESTAMP",
        0, NULL);
    int affected = 0;
    if (result != NULL) {
        affected = atoi(PQcmdTuples(result));
        PQclear(result);
    }
    return affected;
}

PGresult *reserva_repository_listar(PGconn *conn) {
    return exec_params(conn,
        "SELECT r.id_reserva, u.nome AS usuario, l.titulo AS livro, r.data_reserva, COALESCE(r.data_expiracao::text, '') AS data_expiracao, r.status "
        "FROM reserva r "
        "JOIN usuario u ON u.id_usuario = r.id_usuario "
        "JOIN livro l ON l.id_livro = r.id_livro "
        "ORDER BY r.id_reserva",
        0, NULL);
}
