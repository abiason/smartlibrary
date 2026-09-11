#include "emprestimo_repository.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_EMPRESTIMOS_ATIVOS 5
#define MAX_RENOVACOES 2
#define PRAZO_DIAS 7

static void set_motivo(char *motivo, int motivo_size, const char *texto) {
    if (motivo != NULL && motivo_size > 0) {
        snprintf(motivo, (size_t)motivo_size, "%s", texto);
    }
}

static int exec_command(PGconn *conn, const char *sql) {
    PGresult *result = PQexec(conn, sql);
    ExecStatusType status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }
    PQclear(result);
    return 1;
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

static int begin(PGconn *conn) {
    return exec_command(conn, "BEGIN");
}

static int commit(PGconn *conn) {
    return exec_command(conn, "COMMIT");
}

static void rollback(PGconn *conn) {
    PGresult *result = PQexec(conn, "ROLLBACK");
    PQclear(result);
}

static int usuario_pode_emprestar(PGconn *conn, int usuario_id, char *motivo, int motivo_size) {
    char usuario_id_text[16];
    const char *params[1];
    PGresult *result;
    int emprestimos_ativos;

    snprintf(usuario_id_text, sizeof(usuario_id_text), "%d", usuario_id);
    params[0] = usuario_id_text;

    result = exec_params(conn, "SELECT ativo, bloqueado FROM usuario WHERE id_usuario = $1::integer", 1, params);
    if (result == NULL || PQntuples(result) == 0) {
        if (result != NULL) {
            PQclear(result);
        }
        set_motivo(motivo, motivo_size, "Usuario nao encontrado.");
        return 0;
    }

    if (strcmp(PQgetvalue(result, 0, 0), "t") != 0) {
        PQclear(result);
        set_motivo(motivo, motivo_size, "Usuario inativo.");
        return 0;
    }

    if (strcmp(PQgetvalue(result, 0, 1), "t") == 0) {
        PQclear(result);
        set_motivo(motivo, motivo_size, "Usuario bloqueado.");
        return 0;
    }
    PQclear(result);

    result = exec_params(conn,
        "SELECT COUNT(*) FROM emprestimo e "
        "JOIN emprestimo_item i ON i.id_emprestimo = e.id_emprestimo "
        "WHERE e.id_usuario = $1::integer AND e.status = 'ABERTO' AND i.data_devolucao IS NULL "
        "AND e.data_prevista_devolucao < CURRENT_DATE",
        1, params);
    if (scalar_int(result) > 0) {
        PQclear(result);
        set_motivo(motivo, motivo_size, "Usuario possui emprestimo atrasado.");
        return 0;
    }
    PQclear(result);

    result = exec_params(conn,
        "SELECT COUNT(*) FROM emprestimo e "
        "JOIN emprestimo_item i ON i.id_emprestimo = e.id_emprestimo "
        "WHERE e.id_usuario = $1::integer AND e.status = 'ABERTO' AND i.data_devolucao IS NULL",
        1, params);
    emprestimos_ativos = scalar_int(result);
    PQclear(result);

    if (emprestimos_ativos >= MAX_EMPRESTIMOS_ATIVOS) {
        set_motivo(motivo, motivo_size, "Usuario atingiu o limite de exemplares simultaneos.");
        return 0;
    }

    return 1;
}
int emprestimo_repository_realizar_emprestimo(PGconn *conn, int usuario_id, const char *codigo_barras, const char *origem, int *emprestimo_id, int *exemplar_id, char *motivo, int motivo_size) {
    char usuario_id_text[16];
    const char *params_codigo[1];
    const char *params_insert[2];
    const char *params_item[2];
    PGresult *result;
    int livro_id;

    if (!begin(conn)) {
        set_motivo(motivo, motivo_size, "Falha ao iniciar transacao.");
        return 0;
    }

    snprintf(usuario_id_text, sizeof(usuario_id_text), "%d", usuario_id);
    params_codigo[0] = codigo_barras;

    if (!usuario_pode_emprestar(conn, usuario_id, motivo, motivo_size)) {
        rollback(conn);
        return 0;
    }

    result = exec_params(conn,
        "SELECT id_exemplar, id_livro, status FROM exemplar WHERE codigo_barras = $1 OR rfid = $1 FOR UPDATE",
        1, params_codigo);
    if (result == NULL || PQntuples(result) == 0) {
        if (result != NULL) {
            PQclear(result);
        }
        rollback(conn);
        set_motivo(motivo, motivo_size, "Exemplar nao encontrado.");
        return 0;
    }

    *exemplar_id = atoi(PQgetvalue(result, 0, 0));
    livro_id = atoi(PQgetvalue(result, 0, 1));
    if (strcmp(PQgetvalue(result, 0, 2), "DISPONIVEL") != 0) {
        PQclear(result);
        rollback(conn);
        set_motivo(motivo, motivo_size, "Exemplar indisponivel.");
        return 0;
    }
    PQclear(result);

    char livro_id_text[16];
    const char *params_reserva[2];
    snprintf(livro_id_text, sizeof(livro_id_text), "%d", livro_id);
    params_reserva[0] = livro_id_text;
    params_reserva[1] = usuario_id_text;
    result = exec_params(conn,
        "SELECT COUNT(*) FROM reserva WHERE id_livro = $1::integer AND status = 'ATIVA' AND id_usuario <> $2::integer",
        2, params_reserva);
    if (scalar_int(result) > 0) {
        PQclear(result);
        rollback(conn);
        set_motivo(motivo, motivo_size, "Livro reservado para outro usuario.");
        return 0;
    }
    PQclear(result);

    params_insert[0] = usuario_id_text;
    params_insert[1] = origem != NULL && origem[0] != '\0' ? origem : "BALCAO";
    result = exec_params(conn,
        "INSERT INTO emprestimo (id_usuario, data_prevista_devolucao, status, origem) "
        "VALUES ($1::integer, CURRENT_DATE + INTERVAL '7 days', 'ABERTO', $2) RETURNING id_emprestimo",
        2, params_insert);
    if (result == NULL || PQntuples(result) == 0) {
        if (result != NULL) {
            PQclear(result);
        }
        rollback(conn);
        set_motivo(motivo, motivo_size, "Falha ao criar emprestimo.");
        return 0;
    }

    *emprestimo_id = atoi(PQgetvalue(result, 0, 0));
    PQclear(result);

    char emprestimo_id_text[16];
    char exemplar_id_text[16];
    snprintf(emprestimo_id_text, sizeof(emprestimo_id_text), "%d", *emprestimo_id);
    snprintf(exemplar_id_text, sizeof(exemplar_id_text), "%d", *exemplar_id);
    params_item[0] = emprestimo_id_text;
    params_item[1] = exemplar_id_text;

    result = exec_params(conn,
        "INSERT INTO emprestimo_item (id_emprestimo, id_exemplar) VALUES ($1::integer, $2::integer)",
        2, params_item);
    if (result == NULL) {
        rollback(conn);
        set_motivo(motivo, motivo_size, "Falha ao criar item de emprestimo.");
        return 0;
    }
    PQclear(result);

    result = exec_params(conn,
        "UPDATE exemplar SET status = 'EMPRESTADO' WHERE id_exemplar = $1::integer",
        1, &params_item[1]);
    if (result == NULL) {
        rollback(conn);
        set_motivo(motivo, motivo_size, "Falha ao atualizar exemplar.");
        return 0;
    }
    PQclear(result);

    result = exec_params(conn,
        "UPDATE reserva SET status = 'ATENDIDA' "
        "WHERE id_livro = $1::integer AND id_usuario = $2::integer AND status = 'ATIVA'",
        2, params_reserva);
    if (result == NULL) {
        rollback(conn);
        set_motivo(motivo, motivo_size, "Falha ao atender reserva do usuario.");
        return 0;
    }
    PQclear(result);

    if (!commit(conn)) {
        set_motivo(motivo, motivo_size, "Falha ao confirmar transacao.");
        return 0;
    }

    set_motivo(motivo, motivo_size, "Emprestimo realizado.");
    return 1;
}
int emprestimo_repository_realizar_devolucao(PGconn *conn, const char *codigo_barras, int *usuario_id, int *emprestimo_id, int *exemplar_id, char *motivo, int motivo_size) {
    const char *params_codigo[1] = { codigo_barras };
    PGresult *result;

    if (!begin(conn)) {
        set_motivo(motivo, motivo_size, "Falha ao iniciar transacao.");
        return 0;
    }

    result = exec_params(conn,
        "SELECT id_exemplar, status FROM exemplar WHERE codigo_barras = $1 OR rfid = $1 FOR UPDATE",
        1, params_codigo);
    if (result == NULL || PQntuples(result) == 0) {
        if (result != NULL) {
            PQclear(result);
        }
        rollback(conn);
        set_motivo(motivo, motivo_size, "Exemplar nao encontrado.");
        return 0;
    }

    *exemplar_id = atoi(PQgetvalue(result, 0, 0));
    if (strcmp(PQgetvalue(result, 0, 1), "EMPRESTADO") != 0) {
        PQclear(result);
        rollback(conn);
        set_motivo(motivo, motivo_size, "Exemplar nao esta emprestado.");
        return 0;
    }
    PQclear(result);

    char exemplar_id_text[16];
    const char *params_exemplar[1];
    snprintf(exemplar_id_text, sizeof(exemplar_id_text), "%d", *exemplar_id);
    params_exemplar[0] = exemplar_id_text;

    result = exec_params(conn,
        "SELECT e.id_emprestimo, e.id_usuario FROM emprestimo e "
        "JOIN emprestimo_item i ON i.id_emprestimo = e.id_emprestimo "
        "WHERE i.id_exemplar = $1::integer AND e.status = 'ABERTO' AND i.data_devolucao IS NULL "
        "FOR UPDATE OF i, e",
        1, params_exemplar);
    if (result == NULL || PQntuples(result) == 0) {
        if (result != NULL) {
            PQclear(result);
        }
        rollback(conn);
        set_motivo(motivo, motivo_size, "Emprestimo aberto nao encontrado para o exemplar.");
        return 0;
    }

    *emprestimo_id = atoi(PQgetvalue(result, 0, 0));
    *usuario_id = atoi(PQgetvalue(result, 0, 1));
    PQclear(result);

    result = exec_params(conn,
        "UPDATE emprestimo_item SET data_devolucao = CURRENT_TIMESTAMP "
        "WHERE id_exemplar = $1::integer AND data_devolucao IS NULL",
        1, params_exemplar);
    if (result == NULL) {
        rollback(conn);
        set_motivo(motivo, motivo_size, "Falha ao registrar devolucao.");
        return 0;
    }
    PQclear(result);

    result = exec_params(conn,
        "UPDATE exemplar SET status = 'DISPONIVEL' WHERE id_exemplar = $1::integer",
        1, params_exemplar);
    if (result == NULL) {
        rollback(conn);
        set_motivo(motivo, motivo_size, "Falha ao liberar exemplar.");
        return 0;
    }
    PQclear(result);

    char emprestimo_id_text[16];
    const char *params_emprestimo[1];
    snprintf(emprestimo_id_text, sizeof(emprestimo_id_text), "%d", *emprestimo_id);
    params_emprestimo[0] = emprestimo_id_text;

    result = exec_params(conn,
        "UPDATE emprestimo SET status = 'ENCERRADO', data_encerramento = CURRENT_TIMESTAMP "
        "WHERE id_emprestimo = $1::integer AND NOT EXISTS ("
        "SELECT 1 FROM emprestimo_item WHERE id_emprestimo = $1::integer AND data_devolucao IS NULL)",
        1, params_emprestimo);
    if (result == NULL) {
        rollback(conn);
        set_motivo(motivo, motivo_size, "Falha ao encerrar emprestimo.");
        return 0;
    }
    PQclear(result);

    if (!commit(conn)) {
        set_motivo(motivo, motivo_size, "Falha ao confirmar transacao.");
        return 0;
    }

    set_motivo(motivo, motivo_size, "Devolucao realizada.");
    return 1;
}
int emprestimo_repository_renovar_item(PGconn *conn, int emprestimo_item_id, int *usuario_id, int *emprestimo_id, int *exemplar_id, char *codigo_barras, int codigo_size, char *motivo, int motivo_size) {
    char item_id_text[16];
    const char *params_item_id[1];
    PGresult *result;
    int livro_id;
    int renovacoes;

    if (!begin(conn)) {
        set_motivo(motivo, motivo_size, "Falha ao iniciar transacao.");
        return 0;
    }

    snprintf(item_id_text, sizeof(item_id_text), "%d", emprestimo_item_id);
    params_item_id[0] = item_id_text;

    result = exec_params(conn,
        "SELECT i.id_emprestimo, e.id_usuario, i.id_exemplar, ex.id_livro, ex.codigo_barras, i.quantidade_renovacoes "
        "FROM emprestimo_item i "
        "JOIN emprestimo e ON e.id_emprestimo = i.id_emprestimo "
        "JOIN exemplar ex ON ex.id_exemplar = i.id_exemplar "
        "WHERE i.id_emprestimo_item = $1::integer AND e.status = 'ABERTO' AND i.data_devolucao IS NULL "
        "FOR UPDATE OF i, e, ex",
        1, params_item_id);
    if (result == NULL || PQntuples(result) == 0) {
        if (result != NULL) {
            PQclear(result);
        }
        rollback(conn);
        set_motivo(motivo, motivo_size, "Item de emprestimo aberto nao encontrado.");
        return 0;
    }

    *emprestimo_id = atoi(PQgetvalue(result, 0, 0));
    *usuario_id = atoi(PQgetvalue(result, 0, 1));
    *exemplar_id = atoi(PQgetvalue(result, 0, 2));
    livro_id = atoi(PQgetvalue(result, 0, 3));
    snprintf(codigo_barras, (size_t)codigo_size, "%s", PQgetvalue(result, 0, 4));
    renovacoes = atoi(PQgetvalue(result, 0, 5));
    PQclear(result);

    if (renovacoes >= MAX_RENOVACOES) {
        rollback(conn);
        set_motivo(motivo, motivo_size, "Limite de renovacoes atingido.");
        return 0;
    }

    char livro_id_text[16];
    char usuario_id_text[16];
    const char *params_reserva[2];
    snprintf(livro_id_text, sizeof(livro_id_text), "%d", livro_id);
    snprintf(usuario_id_text, sizeof(usuario_id_text), "%d", *usuario_id);
    params_reserva[0] = livro_id_text;
    params_reserva[1] = usuario_id_text;

    result = exec_params(conn,
        "SELECT COUNT(*) FROM reserva WHERE id_livro = $1::integer AND status = 'ATIVA' AND id_usuario <> $2::integer",
        2, params_reserva);
    if (scalar_int(result) > 0) {
        PQclear(result);
        rollback(conn);
        set_motivo(motivo, motivo_size, "Livro reservado para outro usuario.");
        return 0;
    }
    PQclear(result);

    result = exec_params(conn,
        "UPDATE emprestimo_item SET quantidade_renovacoes = quantidade_renovacoes + 1 "
        "WHERE id_emprestimo_item = $1::integer",
        1, params_item_id);
    if (result == NULL) {
        rollback(conn);
        set_motivo(motivo, motivo_size, "Falha ao atualizar renovacoes.");
        return 0;
    }
    PQclear(result);

    char emprestimo_id_text[16];
    const char *params_emprestimo[1];
    snprintf(emprestimo_id_text, sizeof(emprestimo_id_text), "%d", *emprestimo_id);
    params_emprestimo[0] = emprestimo_id_text;

    result = exec_params(conn,
        "UPDATE emprestimo SET data_prevista_devolucao = CURRENT_DATE + INTERVAL '7 days' "
        "WHERE id_emprestimo = $1::integer",
        1, params_emprestimo);
    if (result == NULL) {
        rollback(conn);
        set_motivo(motivo, motivo_size, "Falha ao atualizar prazo.");
        return 0;
    }
    PQclear(result);

    if (!commit(conn)) {
        set_motivo(motivo, motivo_size, "Falha ao confirmar transacao.");
        return 0;
    }

    set_motivo(motivo, motivo_size, "Renovacao realizada.");
    return 1;
}

PGresult *emprestimo_repository_listar_emprestimos_abertos(PGconn *conn) {
    return exec_params(conn,
        "SELECT i.id_emprestimo_item, e.id_emprestimo, u.nome AS usuario, ex.codigo_barras, l.titulo, "
        "e.data_prevista_devolucao, i.quantidade_renovacoes "
        "FROM emprestimo e "
        "JOIN usuario u ON u.id_usuario = e.id_usuario "
        "JOIN emprestimo_item i ON i.id_emprestimo = e.id_emprestimo "
        "JOIN exemplar ex ON ex.id_exemplar = i.id_exemplar "
        "JOIN livro l ON l.id_livro = ex.id_livro "
        "WHERE e.status = 'ABERTO' AND i.data_devolucao IS NULL "
        "ORDER BY e.id_emprestimo, i.id_emprestimo_item",
        0, NULL);
}





