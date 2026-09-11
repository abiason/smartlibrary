#include "event_service.h"

#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <time.h>

int event_service_registrar(MongoConnection *mongo, const char *tipo, int usuario_id, int emprestimo_id, int exemplar_id, const char *codigo_barras) {
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_t *doc;
    bool ok;

    if (mongo == NULL || mongo->database == NULL || tipo == NULL) {
        return 0;
    }

    collection = mongoc_database_get_collection(mongo->database, "eventos");
    doc = BCON_NEW(
        "tipo", BCON_UTF8(tipo),
        "dataHora", BCON_DATE_TIME((int64_t)time(NULL) * 1000),
        "usuarioId", BCON_INT32(usuario_id),
        "emprestimoId", BCON_INT32(emprestimo_id),
        "origem", BCON_UTF8("SISTEMA"),
        "exemplar", "{",
            "id", BCON_INT32(exemplar_id),
            "codigoBarras", BCON_UTF8(codigo_barras != NULL ? codigo_barras : ""),
        "}"
    );

    ok = mongoc_collection_insert_one(collection, doc, NULL, NULL, &error);
    if (!ok) {
        fprintf(stderr, "[WARN] MongoDB: evento nao registrado: %s\n", error.message);
    }

    bson_destroy(doc);
    mongoc_collection_destroy(collection);
    return ok ? 1 : 0;
}



