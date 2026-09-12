#ifndef SMARTLIBRARY_AUTH_SERVICE_H
#define SMARTLIBRARY_AUTH_SERVICE_H

#include "database/postgres.h"

#include <stdbool.h>

typedef struct {
    bool authenticated;
    int usuario_id;
    int perfil_id;
    char nome[151];
    char cpf[15];
    char perfil_nome[51];
} AuthSession;

int auth_service_autenticar(PostgresConnection *postgres, const char *cpf, const char *senha, AuthSession *session);
int auth_service_contar_usuarios(PostgresConnection *postgres);
int auth_service_criar_admin_inicial(PostgresConnection *postgres, const char *nome, const char *cpf, const char *email, const char *senha);
int auth_service_trocar_senha(PostgresConnection *postgres, int usuario_id, const char *senha_atual, const char *nova_senha);
int auth_service_is_admin(const AuthSession *session);
int auth_service_is_bibliotecario(const AuthSession *session);
int auth_service_is_usuario(const AuthSession *session);

#endif
