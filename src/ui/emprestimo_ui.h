#ifndef SMARTLIBRARY_EMPRESTIMO_UI_H
#define SMARTLIBRARY_EMPRESTIMO_UI_H

#include "database/mongodb.h"
#include "database/postgres.h"

void emprestimo_ui_run(PostgresConnection *postgres, MongoConnection *mongo);

#endif
