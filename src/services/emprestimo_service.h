#ifndef SMARTLIBRARY_EMPRESTIMO_SERVICE_H
#define SMARTLIBRARY_EMPRESTIMO_SERVICE_H

#include "database/mongodb.h"
#include "database/postgres.h"

int emprestimo_service_realizar_emprestimo(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, const char *codigo_barras);
int emprestimo_service_realizar_emprestimo_origem(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, const char *codigo_barras, const char *origem);
int emprestimo_service_realizar_devolucao(PostgresConnection *postgres, MongoConnection *mongo, const char *codigo_barras);
int emprestimo_service_realizar_devolucao_origem(PostgresConnection *postgres, MongoConnection *mongo, const char *codigo_barras, const char *origem);
int emprestimo_service_renovar_item(PostgresConnection *postgres, MongoConnection *mongo, int emprestimo_item_id);
int emprestimo_service_renovar_item_origem(PostgresConnection *postgres, MongoConnection *mongo, int emprestimo_item_id, const char *origem);
void emprestimo_service_listar_abertos(PostgresConnection *postgres);

#endif
