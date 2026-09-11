#ifndef SMARTLIBRARY_RESERVA_SERVICE_H
#define SMARTLIBRARY_RESERVA_SERVICE_H

#include "database/mongodb.h"
#include "database/postgres.h"

int reserva_service_criar(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, int livro_id);
int reserva_service_cancelar(PostgresConnection *postgres, MongoConnection *mongo, int reserva_id);
int reserva_service_atender(PostgresConnection *postgres, MongoConnection *mongo, int reserva_id);
void reserva_service_expirar_vencidas(PostgresConnection *postgres, MongoConnection *mongo);
void reserva_service_listar(PostgresConnection *postgres);

#endif
