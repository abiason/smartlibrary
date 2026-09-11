#include "self_checkout_repository.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void set_text(char *dest, int size, const char *text) {
    if (dest != NULL && size > 0) {
        snprintf(dest, (size_t)size, "%s", text);
    }
}

static PGresult *exec_params(PGconn *conn, const char *sql, int count, const char *const *params) {
    PGresult *result = PQexecParams(conn, sql, count, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(conn));
        PQclear(result);
        return NULL;
    }
    return result;
}

int self_checkout_repository_identificar_usuario(PGconn *conn, const char *cpf, int *usuario_id, char *nome, int nome_size, char *motivo, int motivo_size) {
    const char *params[1] = { cpf };
    PGresult *result = exec_params(conn, "SELECT id_usuario, nome, ativo, bloqueado FROM usuario WHERE cpf = $1", 1, params);

    if (result == NULL || PQntuples(result) == 0) {
        if (result != NULL) {
            PQclear(result);
        }
        set_text(motivo, motivo_size, "Usuario nao encontrado.");
        return 0;
    }

    if (strcmp(PQgetvalue(result, 0, 2), "t") != 0) {
        PQclear(result);
        set_text(motivo, motivo_size, "Usuario inativo.");
        return 0;
    }

    if (strcmp(PQgetvalue(result, 0, 3), "t") == 0) {
        PQclear(result);
        set_text(motivo, motivo_size, "Usuario bloqueado.");
        return 0;
    }

    *usuario_id = atoi(PQgetvalue(result, 0, 0));
    set_text(nome, nome_size, PQgetvalue(result, 0, 1));
    set_text(motivo, motivo_size, "Usuario identificado.");
    PQclear(result);
    return 1;
}

PGresult *self_checkout_repository_listar_emprestimos_usuario(PGconn *conn, int usuario_id) {
    char usuario_id_text[16];
    const char *params[1];
    snprintf(usuario_id_text, sizeof(usuario_id_text), "%d", usuario_id);
    params[0] = usuario_id_text;

    return exec_params(conn,
        "SELECT i.id_emprestimo_item, e.id_emprestimo, ex.codigo_barras, l.titulo, "
        "e.data_prevista_devolucao, i.quantidade_renovacoes, "
        "CASE WHEN e.data_prevista_devolucao < CURRENT_DATE THEN 'SIM' ELSE 'NAO' END AS atrasado "
        "FROM emprestimo e "
        "JOIN emprestimo_item i ON i.id_emprestimo = e.id_emprestimo "
        "JOIN exemplar ex ON ex.id_exemplar = i.id_exemplar "
        "JOIN livro l ON l.id_livro = ex.id_livro "
        "WHERE e.id_usuario = $1::integer AND e.status = 'ABERTO' AND i.data_devolucao IS NULL "
        "ORDER BY e.data_prevista_devolucao, i.id_emprestimo_item",
        1, params);
}

PGresult *self_checkout_repository_listar_reservas_usuario(PGconn *conn, int usuario_id) {
    char usuario_id_text[16];
    const char *params[1];
    snprintf(usuario_id_text, sizeof(usuario_id_text), "%d", usuario_id);
    params[0] = usuario_id_text;

    return exec_params(conn,
        "SELECT r.id_reserva, l.titulo, r.data_reserva, COALESCE(r.data_expiracao::text, '') AS data_expiracao, r.status "
        "FROM reserva r JOIN livro l ON l.id_livro = r.id_livro "
        "WHERE r.id_usuario = $1::integer ORDER BY r.id_reserva DESC",
        1, params);
}

PGresult *self_checkout_repository_pesquisar_livros(PGconn *conn, const char *termo) {
    char pattern[256];
    const char *params[1];
    snprintf(pattern, sizeof(pattern), "%%%s%%", termo);
    params[0] = pattern;

    return exec_params(conn,
        "SELECT l.id_livro, COALESCE(l.isbn, '') AS isbn, l.titulo, COALESCE(ed.nome, '') AS editora, "
        "COUNT(ex.id_exemplar) FILTER (WHERE ex.status = 'DISPONIVEL') AS disponiveis "
        "FROM livro l "
        "LEFT JOIN editora ed ON ed.id_editora = l.id_editora "
        "LEFT JOIN exemplar ex ON ex.id_livro = l.id_livro "
        "WHERE l.titulo ILIKE $1 OR l.isbn ILIKE $1 "
        "GROUP BY l.id_livro, l.isbn, l.titulo, ed.nome "
        "ORDER BY l.titulo",
        1, params);
}

int self_checkout_repository_item_pertence_usuario(PGconn *conn, int usuario_id, int emprestimo_item_id) {
    char usuario_id_text[16];
    char item_id_text[16];
    const char *params[2];
    PGresult *result;
    int belongs = 0;

    snprintf(usuario_id_text, sizeof(usuario_id_text), "%d", usuario_id);
    snprintf(item_id_text, sizeof(item_id_text), "%d", emprestimo_item_id);
    params[0] = usuario_id_text;
    params[1] = item_id_text;

    result = exec_params(conn,
        "SELECT COUNT(*) FROM emprestimo e "
        "JOIN emprestimo_item i ON i.id_emprestimo = e.id_emprestimo "
        "WHERE e.id_usuario = $1::integer AND i.id_emprestimo_item = $2::integer "
        "AND e.status = 'ABERTO' AND i.data_devolucao IS NULL",
        2, params);
    if (result != NULL && PQntuples(result) > 0) {
        belongs = atoi(PQgetvalue(result, 0, 0)) > 0;
        PQclear(result);
    }

    return belongs;
}
