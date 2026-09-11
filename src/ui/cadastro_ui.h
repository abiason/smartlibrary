#ifndef SMARTLIBRARY_CADASTRO_UI_H
#define SMARTLIBRARY_CADASTRO_UI_H

#include "database/mongodb.h"
#include "database/postgres.h"

void cadastro_ui_run(PostgresConnection *postgres, MongoConnection *mongo);

#endif
