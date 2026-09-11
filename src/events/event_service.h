#ifndef SMARTLIBRARY_EVENT_SERVICE_H
#define SMARTLIBRARY_EVENT_SERVICE_H

#include "database/mongodb.h"

int event_service_registrar(MongoConnection *mongo, const char *tipo, int usuario_id, int emprestimo_id, int exemplar_id, const char *codigo_barras);
int event_service_registrar_origem(MongoConnection *mongo, const char *tipo, const char *origem, int usuario_id, int emprestimo_id, int exemplar_id, const char *codigo_barras);
int event_service_registrar_log(MongoConnection *mongo, const char *nivel, const char *componente, const char *terminal_codigo, const char *mensagem);
int event_service_registrar_auditoria(MongoConnection *mongo, const char *entidade, const char *entidade_id, const char *acao, int usuario_id, const char *antes, const char *depois);
int event_service_registrar_auditoria_json(MongoConnection *mongo, const char *entidade, const char *entidade_id, const char *acao, int usuario_id, const char *antes_json, const char *depois_json);
void event_service_listar_eventos_recentes(MongoConnection *mongo, int limite);
void event_service_listar_logs_recentes(MongoConnection *mongo, int limite);
void event_service_listar_auditoria_recente(MongoConnection *mongo, int limite);
void event_service_resumir_eventos_por_tipo(MongoConnection *mongo);

#endif
