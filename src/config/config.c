#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void copy_env_or_default(char *dest, size_t dest_size, const char *name, const char *fallback) {
    const char *value = getenv(name);
    if (value == NULL || value[0] == '\0') {
        value = fallback;
    }

    snprintf(dest, dest_size, "%s", value);
}

int config_load(SmartLibraryConfig *config) {
    if (config == NULL) {
        return 0;
    }

    copy_env_or_default(config->postgres_host, sizeof(config->postgres_host), "POSTGRES_HOST", "localhost");
    copy_env_or_default(config->postgres_port, sizeof(config->postgres_port), "POSTGRES_PORT", "5432");
    copy_env_or_default(config->postgres_db, sizeof(config->postgres_db), "POSTGRES_DB", "smartlibrary");
    copy_env_or_default(config->postgres_user, sizeof(config->postgres_user), "POSTGRES_USER", "postgres");
    copy_env_or_default(config->postgres_password, sizeof(config->postgres_password), "POSTGRES_PASSWORD", "");
    copy_env_or_default(config->mongodb_uri, sizeof(config->mongodb_uri), "MONGODB_URI", "mongodb://localhost:27017");
    copy_env_or_default(config->mongodb_database, sizeof(config->mongodb_database), "MONGODB_DATABASE", "smartlibrary");

    return 1;
}

void config_print_summary(const SmartLibraryConfig *config) {
    if (config == NULL) {
        return;
    }

    printf("[INFO] PostgreSQL: %s:%s/%s\n", config->postgres_host, config->postgres_port, config->postgres_db);
    printf("[INFO] MongoDB: %s database=%s\n", config->mongodb_uri, config->mongodb_database);
}
