#ifndef SMARTLIBRARY_MAIN_UI_H
#define SMARTLIBRARY_MAIN_UI_H

#include "database/mongodb.h"
#include "database/postgres.h"

void main_ui_run(PostgresConnection *postgres, MongoConnection *mongo);

#endif
