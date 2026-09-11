#ifndef SMARTLIBRARY_SELF_CHECKOUT_UI_H
#define SMARTLIBRARY_SELF_CHECKOUT_UI_H

#include "database/mongodb.h"
#include "database/postgres.h"

void self_checkout_ui_run(PostgresConnection *postgres, MongoConnection *mongo);

#endif
