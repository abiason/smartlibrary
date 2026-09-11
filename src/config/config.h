#ifndef SMARTLIBRARY_CONFIG_H
#define SMARTLIBRARY_CONFIG_H

#include <stddef.h>

typedef struct {
    char postgres_host[128];
    char postgres_port[16];
    char postgres_db[128];
    char postgres_user[128];
    char postgres_password[128];
    char mongodb_uri[256];
    char mongodb_database[128];
} SmartLibraryConfig;

int config_load(SmartLibraryConfig *config);
void config_print_summary(const SmartLibraryConfig *config);

#endif
