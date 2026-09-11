#ifndef SMARTLIBRARY_EVENT_SERVICE_H
#define SMARTLIBRARY_EVENT_SERVICE_H

#include "database/mongodb.h"

int event_service_registrar(MongoConnection *mongo, const char *tipo, int usuario_id, int emprestimo_id, int exemplar_id, const char *codigo_barras);

#endif
