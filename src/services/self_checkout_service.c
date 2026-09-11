#include "self_checkout_service.h"

#include "events/event_service.h"
#include "repositories/self_checkout_repository.h"
#include "services/emprestimo_service.h"

#include <libpq-fe.h>
#include <stdio.h>

static int has_postgres(PostgresConnection *postgres) {
    return postgres != NULL && postgres->conn != NULL;
}

static int has_text(const char *value) {
    return value != NULL && value[0] != '\0';
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

int self_checkout_service_identificar_usuario(PostgresConnection *postgres, MongoConnection *mongo, const char *cpf, int *usuario_id, char *nome, int nome_size) {
    char motivo[256];

    if (!has_postgres(postgres) || !has_text(cpf) || usuario_id == NULL) {
        fprintf(stderr, "[ERRO] CPF invalido.\n");
        return 0;
    }

    if (!self_checkout_repository_identificar_usuario(postgres->conn, cpf, usuario_id, nome, nome_size, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar(mongo, "USUARIO_BLOQUEADO", 0, 0, 0, "");
        return 0;
    }

    event_service_registrar(mongo, "USUARIO_IDENTIFICADO", *usuario_id, 0, 0, "");
    printf("[OK] %s Bem-vindo, %s.\n", motivo, nome);
    return 1;
}

void self_checkout_service_listar_emprestimos(PostgresConnection *postgres, int usuario_id) {
    if (has_postgres(postgres)) {
        print_rows(self_checkout_repository_listar_emprestimos_usuario(postgres->conn, usuario_id), "Nenhum emprestimo aberto encontrado.");
    }
}

void self_checkout_service_listar_reservas(PostgresConnection *postgres, int usuario_id) {
    if (has_postgres(postgres)) {
        print_rows(self_checkout_repository_listar_reservas_usuario(postgres->conn, usuario_id), "Nenhuma reserva encontrada.");
    }
}

void self_checkout_service_pesquisar_livros(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        print_rows(self_checkout_repository_pesquisar_livros(postgres->conn, termo), "Nenhum livro encontrado.");
    }
}

int self_checkout_service_realizar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, const char *codigo_barras) {
    return emprestimo_service_realizar_emprestimo(postgres, mongo, usuario_id, codigo_barras);
}

int self_checkout_service_realizar_devolucao(PostgresConnection *postgres, MongoConnection *mongo, const char *codigo_barras) {
    return emprestimo_service_realizar_devolucao(postgres, mongo, codigo_barras);
}

int self_checkout_service_renovar_item(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, int emprestimo_item_id) {
    if (!has_postgres(postgres) || !self_checkout_repository_item_pertence_usuario(postgres->conn, usuario_id, emprestimo_item_id)) {
        printf("[ERRO] Item de emprestimo nao pertence ao usuario identificado.\n");
        return 0;
    }

    return emprestimo_service_renovar_item(postgres, mongo, emprestimo_item_id);
}


