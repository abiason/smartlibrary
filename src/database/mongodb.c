#include "mongodb.h"

#include <bson/bson.h>
#include <stdio.h>

void mongodb_driver_init(void) {
    mongoc_init();
}

void mongodb_driver_cleanup(void) {
    mongoc_cleanup();
}

int mongodb_connect(MongoConnection *connection, const SmartLibraryConfig *config) {
    bson_error_t error;
    bson_t *ping = NULL;
    bson_t reply;
    bool ok;

    if (connection == NULL || config == NULL) {
        return 0;
    }

    connection->client = NULL;
    connection->database = NULL;

    connection->client = mongoc_client_new(config->mongodb_uri);
    if (connection->client == NULL) {
        fprintf(stderr, "[ERRO] MongoDB: URI invalida ou cliente nao inicializado.\n");
        return 0;
    }

    connection->database = mongoc_client_get_database(connection->client, config->mongodb_database);
    ping = BCON_NEW("ping", BCON_INT32(1));
    ok = mongoc_client_command_simple(connection->client, "admin", ping, NULL, &reply, &error);

    bson_destroy(ping);
    if (ok) {
        bson_destroy(&reply);
        return 1;
    }

    fprintf(stderr, "[ERRO] MongoDB: %s\n", error.message);
    mongodb_disconnect(connection);
    return 0;
}

void mongodb_disconnect(MongoConnection *connection) {
    if (connection == NULL) {
        return;
    }

    if (connection->database != NULL) {
        mongoc_database_destroy(connection->database);
        connection->database = NULL;
    }

    if (connection->client != NULL) {
        mongoc_client_destroy(connection->client);
        connection->client = NULL;
    }
}
