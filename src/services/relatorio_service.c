#include "relatorio_service.h"

#include "repositories/relatorio_repository.h"

#include <libpq-fe.h>
#include <stdio.h>

static int has_postgres(PostgresConnection *postgres) {
    return postgres != NULL && postgres->conn != NULL;
}

static void print_rows(PGresult *result, const char *empty_message) {
    int rows;
    int cols;

    if (result == NULL) {
        return;
    }

    rows = PQntuples(result);
    cols = PQnfields(result);
    if (rows == 0) {
        printf("[INFO] %s\n", empty_message);
        PQclear(result);
        return;
    }

    for (int col = 0; col < cols; col++) {
        printf("%-30s", PQfname(result, col));
    }
    printf("\n");
    for (int col = 0; col < cols; col++) {
        printf("------------------------------");
    }
    printf("\n");

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            printf("%-30s", PQgetvalue(result, row, col));
        }
        printf("\n");
    }

    PQclear(result);
}

void relatorio_service_acervo_por_status(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(relatorio_repository_acervo_por_status(postgres->conn), "Nenhum livro encontrado no acervo.");
    }
}

void relatorio_service_emprestimos_atrasados(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(relatorio_repository_emprestimos_atrasados(postgres->conn), "Nenhum emprestimo atrasado encontrado.");
    }
}

void relatorio_service_livros_mais_emprestados(PostgresConnection *postgres, int limite) {
    if (has_postgres(postgres)) {
        print_rows(relatorio_repository_livros_mais_emprestados(postgres->conn, limite), "Nenhum emprestimo encontrado para ranking.");
    }
}

void relatorio_service_emprestimos_por_origem(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(relatorio_repository_emprestimos_por_origem(postgres->conn), "Nenhum emprestimo encontrado.");
    }
}

void relatorio_service_reservas_ativas(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(relatorio_repository_reservas_ativas(postgres->conn), "Nenhuma reserva ativa encontrada.");
    }
}

void relatorio_service_usuarios_com_pendencias(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(relatorio_repository_usuarios_com_pendencias(postgres->conn), "Nenhum usuario com pendencia encontrado.");
    }
}

