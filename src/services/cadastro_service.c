#include "cadastro_service.h"

#include "repositories/cadastro_repository.h"

#include <libpq-fe.h>
#include <stdio.h>
#include <string.h>

static int has_text(const char *value) {
    return value != NULL && value[0] != '\0';
}

static int has_postgres(PostgresConnection *postgres) {
    return postgres != NULL && postgres->conn != NULL;
}

static void print_rows(PGresult *result) {
    int rows;
    int cols;

    if (result == NULL) {
        return;
    }

    rows = PQntuples(result);
    cols = PQnfields(result);

    if (rows == 0) {
        printf("[INFO] Nenhum registro encontrado.\n");
        PQclear(result);
        return;
    }

    for (int col = 0; col < cols; col++) {
        printf("%-24s", PQfname(result, col));
    }
    printf("\n");

    for (int col = 0; col < cols; col++) {
        printf("------------------------");
    }
    printf("\n");

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            printf("%-24s", PQgetvalue(result, row, col));
        }
        printf("\n");
    }

    PQclear(result);
}

int cadastro_service_criar_usuario(PostgresConnection *postgres, const Usuario *usuario) {
    if (!has_postgres(postgres) || usuario == NULL || !has_text(usuario->nome) || !has_text(usuario->cpf) || !has_text(usuario->senha_hash) || usuario->perfil_id <= 0) {
        fprintf(stderr, "[ERRO] Usuario invalido. Informe nome, CPF, senha_hash e perfil valido.\n");
        return 0;
    }

    return cadastro_repository_criar_usuario(postgres->conn, usuario);
}

void cadastro_service_listar_usuarios(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(cadastro_repository_listar_usuarios(postgres->conn));
    }
}

int cadastro_service_criar_autor(PostgresConnection *postgres, const Autor *autor) {
    if (!has_postgres(postgres) || autor == NULL || !has_text(autor->nome)) {
        fprintf(stderr, "[ERRO] Autor invalido. Informe o nome.\n");
        return 0;
    }

    return cadastro_repository_criar_autor(postgres->conn, autor);
}

void cadastro_service_listar_autores(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(cadastro_repository_listar_autores(postgres->conn));
    }
}

int cadastro_service_criar_editora(PostgresConnection *postgres, const Editora *editora) {
    if (!has_postgres(postgres) || editora == NULL || !has_text(editora->nome)) {
        fprintf(stderr, "[ERRO] Editora invalida. Informe o nome.\n");
        return 0;
    }

    return cadastro_repository_criar_editora(postgres->conn, editora);
}

void cadastro_service_listar_editoras(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(cadastro_repository_listar_editoras(postgres->conn));
    }
}
int cadastro_service_criar_genero(PostgresConnection *postgres, const Genero *genero) {
    if (!has_postgres(postgres) || genero == NULL || !has_text(genero->nome)) {
        fprintf(stderr, "[ERRO] Genero invalido. Informe o nome.\n");
        return 0;
    }

    return cadastro_repository_criar_genero(postgres->conn, genero);
}

void cadastro_service_listar_generos(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(cadastro_repository_listar_generos(postgres->conn));
    }
}

int cadastro_service_criar_livro(PostgresConnection *postgres, const Livro *livro) {
    if (!has_postgres(postgres) || livro == NULL || !has_text(livro->titulo)) {
        fprintf(stderr, "[ERRO] Livro invalido. Informe o titulo.\n");
        return 0;
    }

    return cadastro_repository_criar_livro(postgres->conn, livro);
}

void cadastro_service_listar_livros(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(cadastro_repository_listar_livros(postgres->conn));
    }
}

int cadastro_service_criar_exemplar(PostgresConnection *postgres, const Exemplar *exemplar) {
    if (!has_postgres(postgres) || exemplar == NULL || exemplar->livro_id <= 0 || !has_text(exemplar->codigo_barras)) {
        fprintf(stderr, "[ERRO] Exemplar invalido. Informe livro e codigo de barras.\n");
        return 0;
    }

    return cadastro_repository_criar_exemplar(postgres->conn, exemplar);
}

void cadastro_service_listar_exemplares(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        print_rows(cadastro_repository_listar_exemplares(postgres->conn));
    }
}
