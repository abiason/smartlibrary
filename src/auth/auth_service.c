#include "auth_service.h"

#include <libpq-fe.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int has_text(const char *value) {
    return value != NULL && value[0] != '\0';
}

static int has_postgres(PostgresConnection *postgres) {
    return postgres != NULL && postgres->conn != NULL;
}

static void copy_text(char *dest, size_t size, const char *value) {
    if (dest != NULL && size > 0) {
        snprintf(dest, size, "%s", value != NULL ? value : "");
    }
}

int auth_service_autenticar(PostgresConnection *postgres, const char *cpf, const char *senha, AuthSession *session) {
    const char *params[2];
    PGresult *result;

    if (session != NULL) {
        memset(session, 0, sizeof(*session));
    }

    if (!has_postgres(postgres) || !has_text(cpf) || !has_text(senha) || session == NULL) {
        return 0;
    }

    params[0] = cpf;
    params[1] = senha;
    result = PQexecParams(postgres->conn,
        "SELECT u.id_usuario, u.nome, u.cpf, u.id_perfil, p.nome AS perfil "
        "FROM usuario u INNER JOIN perfil p ON p.id_perfil = u.id_perfil "
        "WHERE u.cpf = $1 AND u.senha_hash = crypt($2, u.senha_hash) AND u.ativo = true AND u.bloqueado = false",
        2, NULL, params, NULL, NULL, 0);

    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(postgres->conn));
        PQclear(result);
        return 0;
    }

    if (PQntuples(result) == 0) {
        PQclear(result);
        return 0;
    }

    session->authenticated = true;
    session->usuario_id = atoi(PQgetvalue(result, 0, 0));
    copy_text(session->nome, sizeof(session->nome), PQgetvalue(result, 0, 1));
    copy_text(session->cpf, sizeof(session->cpf), PQgetvalue(result, 0, 2));
    session->perfil_id = atoi(PQgetvalue(result, 0, 3));
    copy_text(session->perfil_nome, sizeof(session->perfil_nome), PQgetvalue(result, 0, 4));

    PQclear(result);
    return 1;
}


int auth_service_contar_usuarios(PostgresConnection *postgres) {
    PGresult *result;
    int count = 0;

    if (!has_postgres(postgres)) {
        return -1;
    }

    result = PQexec(postgres->conn, "SELECT COUNT(*) FROM usuario");
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(postgres->conn));
        PQclear(result);
        return -1;
    }

    count = atoi(PQgetvalue(result, 0, 0));
    PQclear(result);
    return count;
}

int auth_service_criar_admin_inicial(PostgresConnection *postgres, const char *nome, const char *cpf, const char *email, const char *senha) {
    const char *params[4];
    PGresult *result;
    int ok;

    if (!has_postgres(postgres) || !has_text(nome) || !has_text(cpf) || !has_text(senha)) {
        return 0;
    }

    params[0] = nome;
    params[1] = cpf;
    params[2] = email != NULL ? email : "";
    params[3] = senha;

    result = PQexecParams(postgres->conn,
        "INSERT INTO usuario (nome, cpf, email, telefone, senha_hash, id_perfil, ativo, bloqueado) "
        "SELECT $1, $2, NULLIF($3, ''), '', crypt($4, gen_salt('bf')), p.id_perfil, true, false "
        "FROM perfil p WHERE p.nome = 'ADMINISTRADOR'",
        4, NULL, params, NULL, NULL, 0);

    ok = PQresultStatus(result) == PGRES_COMMAND_OK && atoi(PQcmdTuples(result)) == 1;
    if (!ok) {
        fprintf(stderr, "[ERRO] PostgreSQL: administrador inicial nao criado: %s", PQerrorMessage(postgres->conn));
    }

    PQclear(result);
    return ok;
}
int auth_service_is_admin(const AuthSession *session) {
    return session != NULL && session->authenticated && strcmp(session->perfil_nome, "ADMINISTRADOR") == 0;
}

int auth_service_is_bibliotecario(const AuthSession *session) {
    return session != NULL && session->authenticated && strcmp(session->perfil_nome, "BIBLIOTECARIO") == 0;
}

int auth_service_is_usuario(const AuthSession *session) {
    return session != NULL && session->authenticated && strcmp(session->perfil_nome, "USUARIO") == 0;
}
