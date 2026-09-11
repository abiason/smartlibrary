#include "relatorio_repository.h"

#include <stdio.h>

static PGresult *exec_select(PGconn *conn, const char *sql, int count, const char *const *params) {
    PGresult *result = PQexecParams(conn, sql, count, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(conn));
        PQclear(result);
        return NULL;
    }
    return result;
}

PGresult *relatorio_repository_acervo_por_status(PGconn *conn) {
    return exec_select(conn,
        "SELECT l.id_livro, l.titulo, COALESCE(l.isbn, '') AS isbn, "
        "COUNT(e.id_exemplar) AS total_exemplares, "
        "SUM(CASE WHEN e.status = 'DISPONIVEL' THEN 1 ELSE 0 END) AS disponiveis, "
        "SUM(CASE WHEN e.status = 'EMPRESTADO' THEN 1 ELSE 0 END) AS emprestados, "
        "SUM(CASE WHEN e.status = 'RESERVADO' THEN 1 ELSE 0 END) AS reservados, "
        "SUM(CASE WHEN e.status IN ('MANUTENCAO', 'EXTRAVIADO') THEN 1 ELSE 0 END) AS indisponiveis "
        "FROM livro l LEFT JOIN exemplar e ON e.id_livro = l.id_livro "
        "GROUP BY l.id_livro, l.titulo, l.isbn "
        "ORDER BY l.titulo",
        0, NULL);
}

PGresult *relatorio_repository_emprestimos_atrasados(PGconn *conn) {
    return exec_select(conn,
        "SELECT i.id_emprestimo_item, e.id_emprestimo, u.nome AS usuario, COALESCE(u.email, '') AS email, "
        "ex.codigo_barras, l.titulo, e.data_prevista_devolucao, "
        "(CURRENT_DATE - e.data_prevista_devolucao) AS dias_atraso "
        "FROM emprestimo e "
        "JOIN emprestimo_item i ON i.id_emprestimo = e.id_emprestimo "
        "JOIN usuario u ON u.id_usuario = e.id_usuario "
        "JOIN exemplar ex ON ex.id_exemplar = i.id_exemplar "
        "JOIN livro l ON l.id_livro = ex.id_livro "
        "WHERE e.status = 'ABERTO' AND i.data_devolucao IS NULL AND e.data_prevista_devolucao < CURRENT_DATE "
        "ORDER BY dias_atraso DESC, u.nome",
        0, NULL);
}

PGresult *relatorio_repository_livros_mais_emprestados(PGconn *conn, int limite) {
    char limite_text[16];
    const char *params[1];

    snprintf(limite_text, sizeof(limite_text), "%d", limite > 0 ? limite : 10);
    params[0] = limite_text;

    return exec_select(conn,
        "SELECT l.id_livro, l.titulo, COALESCE(l.isbn, '') AS isbn, COUNT(i.id_emprestimo_item) AS total_emprestimos "
        "FROM livro l "
        "JOIN exemplar ex ON ex.id_livro = l.id_livro "
        "JOIN emprestimo_item i ON i.id_exemplar = ex.id_exemplar "
        "GROUP BY l.id_livro, l.titulo, l.isbn "
        "ORDER BY total_emprestimos DESC, l.titulo "
        "LIMIT $1::integer",
        1, params);
}

PGresult *relatorio_repository_emprestimos_por_origem(PGconn *conn) {
    return exec_select(conn,
        "SELECT origem, COUNT(*) AS total, "
        "SUM(CASE WHEN status = 'ABERTO' THEN 1 ELSE 0 END) AS abertos, "
        "SUM(CASE WHEN status = 'ENCERRADO' THEN 1 ELSE 0 END) AS encerrados, "
        "SUM(CASE WHEN status = 'CANCELADO' THEN 1 ELSE 0 END) AS cancelados "
        "FROM emprestimo GROUP BY origem ORDER BY origem",
        0, NULL);
}

PGresult *relatorio_repository_reservas_ativas(PGconn *conn) {
    return exec_select(conn,
        "SELECT r.id_reserva, u.nome AS usuario, l.titulo AS livro, r.data_reserva, "
        "COALESCE(r.data_expiracao::text, '') AS data_expiracao "
        "FROM reserva r "
        "JOIN usuario u ON u.id_usuario = r.id_usuario "
        "JOIN livro l ON l.id_livro = r.id_livro "
        "WHERE r.status = 'ATIVA' "
        "ORDER BY r.data_reserva, r.id_reserva",
        0, NULL);
}

PGresult *relatorio_repository_usuarios_com_pendencias(PGconn *conn) {
    return exec_select(conn,
        "SELECT u.id_usuario, u.nome, u.cpf, COUNT(i.id_emprestimo_item) AS itens_atrasados, "
        "MIN(e.data_prevista_devolucao) AS atraso_mais_antigo "
        "FROM usuario u "
        "JOIN emprestimo e ON e.id_usuario = u.id_usuario "
        "JOIN emprestimo_item i ON i.id_emprestimo = e.id_emprestimo "
        "WHERE e.status = 'ABERTO' AND i.data_devolucao IS NULL AND e.data_prevista_devolucao < CURRENT_DATE "
        "GROUP BY u.id_usuario, u.nome, u.cpf "
        "ORDER BY itens_atrasados DESC, atraso_mais_antigo",
        0, NULL);
}
