#include "self_checkout_service.h"

#include "events/event_service.h"
#include "repositories/self_checkout_repository.h"
#include "services/emprestimo_service.h"
#include "ui/console_ui.h"

#include <libpq-fe.h>
#include <stdio.h>

static int has_postgres(PostgresConnection *postgres) {
    return postgres != NULL && postgres->conn != NULL;
}

static int has_text(const char *value) {
    return value != NULL && value[0] != '\0';
}

int self_checkout_service_identificar_usuario(PostgresConnection *postgres, MongoConnection *mongo, const char *cpf, int *usuario_id, char *nome, int nome_size) {
    char motivo[256];

    if (!has_postgres(postgres) || !has_text(cpf) || usuario_id == NULL) {
        fprintf(stderr, "[ERRO] CPF invalido.\n");
        event_service_registrar_log(mongo, "WARN", "self_checkout_service", "SELF-DEFAULT", "CPF invalido no self checkout.");
        return 0;
    }

    if (!self_checkout_repository_identificar_usuario(postgres->conn, cpf, usuario_id, nome, nome_size, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        event_service_registrar_origem(mongo, "USUARIO_BLOQUEADO", "SELF_CHECKOUT", 0, 0, 0, "");
        event_service_registrar_log(mongo, "WARN", "self_checkout_service", "SELF-DEFAULT", motivo);
        return 0;
    }

    event_service_registrar_origem(mongo, "USUARIO_IDENTIFICADO", "SELF_CHECKOUT", *usuario_id, 0, 0, "");
    printf("[OK] %s Bem-vindo, %s.\n", motivo, nome);
    return 1;
}

void self_checkout_service_listar_emprestimos(PostgresConnection *postgres, int usuario_id) {
    if (has_postgres(postgres)) {
        ui_print_pgresult_table(self_checkout_repository_listar_emprestimos_usuario(postgres->conn, usuario_id), "Nenhum emprestimo aberto encontrado.");
    }
}

void self_checkout_service_listar_reservas(PostgresConnection *postgres, int usuario_id) {
    if (has_postgres(postgres)) {
        ui_print_pgresult_table(self_checkout_repository_listar_reservas_usuario(postgres->conn, usuario_id), "Nenhuma reserva encontrada.");
    }
}

void self_checkout_service_pesquisar_livros(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        ui_print_pgresult_table(self_checkout_repository_pesquisar_livros(postgres->conn, termo), "Nenhum livro encontrado.");
    }
}

int self_checkout_service_realizar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, const char *codigo_barras) {
    return emprestimo_service_realizar_emprestimo_origem(postgres, mongo, usuario_id, codigo_barras, "SELF_CHECKOUT");
}

int self_checkout_service_realizar_devolucao(PostgresConnection *postgres, MongoConnection *mongo, const char *codigo_barras) {
    return emprestimo_service_realizar_devolucao_origem(postgres, mongo, codigo_barras, "SELF_CHECKOUT");
}

int self_checkout_service_renovar_item(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, int emprestimo_item_id) {
    if (!has_postgres(postgres) || !self_checkout_repository_item_pertence_usuario(postgres->conn, usuario_id, emprestimo_item_id)) {
        printf("[ERRO] Item de emprestimo nao pertence ao usuario identificado.\n");
        event_service_registrar_log(mongo, "WARN", "self_checkout_service", "SELF-DEFAULT", "Renovacao negada: item nao pertence ao usuario identificado.");
        return 0;
    }

    return emprestimo_service_renovar_item_origem(postgres, mongo, emprestimo_item_id, "SELF_CHECKOUT");
}
