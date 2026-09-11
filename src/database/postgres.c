#include "postgres.h"

#include <stdio.h>

int postgres_connect(PostgresConnection *connection, const SmartLibraryConfig *config) {
    char conninfo[768];

    if (connection == NULL || config == NULL) {
        return 0;
    }

    connection->conn = NULL;

    snprintf(
        conninfo,
        sizeof(conninfo),
        "host=%s port=%s dbname=%s user=%s password=%s",
        config->postgres_host,
        config->postgres_port,
        config->postgres_db,
        config->postgres_user,
        config->postgres_password
    );

    connection->conn = PQconnectdb(conninfo);
    if (PQstatus(connection->conn) != CONNECTION_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(connection->conn));
        PQfinish(connection->conn);
        connection->conn = NULL;
        return 0;
    }

    return 1;
}

void postgres_disconnect(PostgresConnection *connection) {
    if (connection != NULL && connection->conn != NULL) {
        PQfinish(connection->conn);
        connection->conn = NULL;
    }
}
