#ifndef SMARTLIBRARY_MAIN_UI_H
#define SMARTLIBRARY_MAIN_UI_H

#include "database/mongodb.h"
#include "database/postgres.h"
#include "auth/auth_service.h"

void main_ui_run(PostgresConnection *postgres, MongoConnection *mongo, const AuthSession *session);

#endif
