#ifndef SMARTLIBRARY_SELF_CHECKOUT_SERVICE_H
#define SMARTLIBRARY_SELF_CHECKOUT_SERVICE_H

#include "database/mongodb.h"
#include "database/postgres.h"

int self_checkout_service_identificar_usuario(PostgresConnection *postgres, MongoConnection *mongo, const char *cpf, int *usuario_id, char *nome, int nome_size);
void self_checkout_service_listar_emprestimos(PostgresConnection *postgres, int usuario_id);
void self_checkout_service_listar_reservas(PostgresConnection *postgres, int usuario_id);
void self_checkout_service_pesquisar_livros(PostgresConnection *postgres, const char *termo);
int self_checkout_service_realizar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, const char *codigo_barras);
int self_checkout_service_realizar_devolucao(PostgresConnection *postgres, MongoConnection *mongo, const char *codigo_barras);
int self_checkout_service_renovar_item(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, int emprestimo_item_id);

#endif
