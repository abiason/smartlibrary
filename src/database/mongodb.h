#ifndef SMARTLIBRARY_MONGODB_H
#define SMARTLIBRARY_MONGODB_H

#include "../config/config.h"

#include <mongoc/mongoc.h>

typedef struct {
    mongoc_client_t *client;
    mongoc_database_t *database;
} MongoConnection;

void mongodb_driver_init(void);
void mongodb_driver_cleanup(void);
int mongodb_connect(MongoConnection *connection, const SmartLibraryConfig *config);
void mongodb_disconnect(MongoConnection *connection);

#endif
