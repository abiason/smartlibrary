#ifndef SMARTLIBRARY_POSTGRES_H
#define SMARTLIBRARY_POSTGRES_H

#include "../config/config.h"

#include <libpq-fe.h>

typedef struct {
    PGconn *conn;
} PostgresConnection;

int postgres_connect(PostgresConnection *connection, const SmartLibraryConfig *config);
void postgres_disconnect(PostgresConnection *connection);

#endif
