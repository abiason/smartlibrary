#include "event_service.h"

#include <bson/bson.h>
#include <mongoc/mongoc.h>
#include <stdio.h>
#include <time.h>

static int has_mongo(MongoConnection *mongo) {
    return mongo != NULL && mongo->database != NULL;
}

static const char *text_or_empty(const char *value) {
    return value != NULL ? value : "";
}

static void print_bson_document(const bson_t *doc) {
    char *json = bson_as_canonical_extended_json(doc, NULL);
    if (json != NULL) {
        printf("%s\n", json);
        bson_free(json);
    }
}

static void listar_documentos(MongoConnection *mongo, const char *collection_name, const bson_t *query, const bson_t *opts, const char *empty_message) {
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    const bson_t *doc;
    bson_error_t error;
    int count = 0;

    if (!has_mongo(mongo) || collection_name == NULL) {
        printf("[ERRO] MongoDB indisponivel.\n");
        return;
    }

    collection = mongoc_database_get_collection(mongo->database, collection_name);
    cursor = mongoc_collection_find_with_opts(collection, query, opts, NULL);

    while (mongoc_cursor_next(cursor, &doc)) {
        print_bson_document(doc);
        count++;
    }

    if (mongoc_cursor_error(cursor, &error)) {
        fprintf(stderr, "[WARN] MongoDB: falha ao consultar %s: %s\n", collection_name, error.message);
    } else if (count == 0) {
        printf("[INFO] %s\n", empty_message);
    }

    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
}

int event_service_registrar_origem(MongoConnection *mongo, const char *tipo, const char *origem, int usuario_id, int emprestimo_id, int exemplar_id, const char *codigo_barras) {
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_t *doc;
    bool ok;

    if (!has_mongo(mongo) || tipo == NULL) {
        return 0;
    }

    collection = mongoc_database_get_collection(mongo->database, "eventos");
    doc = BCON_NEW(
        "tipo", BCON_UTF8(tipo),
        "dataHora", BCON_DATE_TIME((int64_t)time(NULL) * 1000),
        "usuarioId", BCON_INT32(usuario_id),
        "emprestimoId", BCON_INT32(emprestimo_id),
        "origem", BCON_UTF8(origem != NULL && origem[0] != '\0' ? origem : "SISTEMA"),
        "exemplar", "{",
            "id", BCON_INT32(exemplar_id),
            "codigoBarras", BCON_UTF8(text_or_empty(codigo_barras)),
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

int event_service_registrar(MongoConnection *mongo, const char *tipo, int usuario_id, int emprestimo_id, int exemplar_id, const char *codigo_barras) {
    return event_service_registrar_origem(mongo, tipo, "SISTEMA", usuario_id, emprestimo_id, exemplar_id, codigo_barras);
}

int event_service_registrar_log(MongoConnection *mongo, const char *nivel, const char *componente, const char *terminal_codigo, const char *mensagem) {
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_t *doc;
    bool ok;

    if (!has_mongo(mongo) || nivel == NULL || mensagem == NULL) {
        return 0;
    }

    collection = mongoc_database_get_collection(mongo->database, "logs");
    doc = BCON_NEW(
        "nivel", BCON_UTF8(nivel),
        "dataHora", BCON_DATE_TIME((int64_t)time(NULL) * 1000),
        "componente", BCON_UTF8(text_or_empty(componente)),
        "terminalCodigo", BCON_UTF8(text_or_empty(terminal_codigo)),
        "mensagem", BCON_UTF8(mensagem)
    );

    ok = mongoc_collection_insert_one(collection, doc, NULL, NULL, &error);
    if (!ok) {
        fprintf(stderr, "[WARN] MongoDB: log nao registrado: %s\n", error.message);
    }

    bson_destroy(doc);
    mongoc_collection_destroy(collection);
    return ok ? 1 : 0;
}

int event_service_registrar_auditoria(MongoConnection *mongo, const char *entidade, const char *entidade_id, const char *acao, int usuario_id, const char *antes, const char *depois) {
    mongoc_collection_t *collection;
    bson_error_t error;
    bson_t *doc;
    bool ok;

    if (!has_mongo(mongo) || entidade == NULL || entidade_id == NULL || acao == NULL) {
        return 0;
    }

    collection = mongoc_database_get_collection(mongo->database, "auditoria");
    doc = BCON_NEW(
        "entidade", BCON_UTF8(entidade),
        "entidadeId", BCON_UTF8(entidade_id),
        "acao", BCON_UTF8(acao),
        "usuarioId", BCON_INT32(usuario_id),
        "dataHora", BCON_DATE_TIME((int64_t)time(NULL) * 1000),
        "antes", BCON_UTF8(text_or_empty(antes)),
        "depois", BCON_UTF8(text_or_empty(depois))
    );

    ok = mongoc_collection_insert_one(collection, doc, NULL, NULL, &error);
    if (!ok) {
        fprintf(stderr, "[WARN] MongoDB: auditoria nao registrada: %s\n", error.message);
    }

    bson_destroy(doc);
    mongoc_collection_destroy(collection);
    return ok ? 1 : 0;
}

void event_service_listar_eventos_recentes(MongoConnection *mongo, int limite) {
    bson_t query;
    bson_t *opts;

    bson_init(&query);
    opts = BCON_NEW(
        "sort", "{", "dataHora", BCON_INT32(-1), "}",
        "limit", BCON_INT64(limite > 0 ? limite : 10)
    );

    listar_documentos(mongo, "eventos", &query, opts, "Nenhum evento encontrado.");
    bson_destroy(&query);
    bson_destroy(opts);
}

void event_service_listar_logs_recentes(MongoConnection *mongo, int limite) {
    bson_t query;
    bson_t *opts;

    bson_init(&query);
    opts = BCON_NEW(
        "sort", "{", "dataHora", BCON_INT32(-1), "}",
        "limit", BCON_INT64(limite > 0 ? limite : 10)
    );

    listar_documentos(mongo, "logs", &query, opts, "Nenhum log encontrado.");
    bson_destroy(&query);
    bson_destroy(opts);
}

void event_service_listar_auditoria_recente(MongoConnection *mongo, int limite) {
    bson_t query;
    bson_t *opts;

    bson_init(&query);
    opts = BCON_NEW(
        "sort", "{", "dataHora", BCON_INT32(-1), "}",
        "limit", BCON_INT64(limite > 0 ? limite : 10)
    );

    listar_documentos(mongo, "auditoria", &query, opts, "Nenhum registro de auditoria encontrado.");
    bson_destroy(&query);
    bson_destroy(opts);
}

void event_service_resumir_eventos_por_tipo(MongoConnection *mongo) {
    mongoc_collection_t *collection;
    mongoc_cursor_t *cursor;
    bson_error_t error;
    bson_t *pipeline;
    const bson_t *doc;
    int count = 0;

    if (!has_mongo(mongo)) {
        printf("[ERRO] MongoDB indisponivel.\n");
        return;
    }

    collection = mongoc_database_get_collection(mongo->database, "eventos");
    pipeline = BCON_NEW(
        "pipeline", "[",
            "{", "$group", "{", "_id", BCON_UTF8("$tipo"), "total", "{", "$sum", BCON_INT32(1), "}", "}", "}",
            "{", "$sort", "{", "total", BCON_INT32(-1), "}", "}",
        "]"
    );

    cursor = mongoc_collection_aggregate(collection, MONGOC_QUERY_NONE, pipeline, NULL, NULL);
    while (mongoc_cursor_next(cursor, &doc)) {
        print_bson_document(doc);
        count++;
    }

    if (mongoc_cursor_error(cursor, &error)) {
        fprintf(stderr, "[WARN] MongoDB: falha ao resumir eventos: %s\n", error.message);
    } else if (count == 0) {
        printf("[INFO] Nenhum evento encontrado.\n");
    }

    bson_destroy(pipeline);
    mongoc_cursor_destroy(cursor);
    mongoc_collection_destroy(collection);
}
