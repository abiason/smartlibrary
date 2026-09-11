#ifndef SMARTLIBRARY_RESERVA_UI_H
#define SMARTLIBRARY_RESERVA_UI_H

#include "database/mongodb.h"
#include "database/postgres.h"

void reserva_ui_run(PostgresConnection *postgres, MongoConnection *mongo);

#endif
