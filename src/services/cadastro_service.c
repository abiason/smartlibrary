#include "cadastro_service.h"

#include "events/event_service.h"
#include "repositories/cadastro_repository.h"

#include <libpq-fe.h>
#include <stdio.h>

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

#define AUDIT_SNAPSHOT_SIZE 4096

static void read_audit_snapshot(PostgresConnection *postgres, const char *entidade, int id, char *buffer, int buffer_size) {
    if (!has_postgres(postgres) || !cadastro_repository_obter_snapshot(postgres->conn, entidade, id, buffer, buffer_size)) {
        if (buffer != NULL && buffer_size > 0) {
            snprintf(buffer, (size_t)buffer_size, "null");
        }
    }
}

static void audit_int_id(MongoConnection *mongo, const char *entidade, int id, const char *acao, const char *antes, const char *depois) {
    char id_text[32];
    snprintf(id_text, sizeof(id_text), "%d", id);
    event_service_registrar_auditoria_json(mongo, entidade, id_text, acao, 0, antes, depois);
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

void cadastro_service_buscar_usuarios(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        print_rows(cadastro_repository_buscar_usuarios(postgres->conn, termo));
    }
}

int cadastro_service_atualizar_usuario(PostgresConnection *postgres, MongoConnection *mongo, const Usuario *usuario) {
    if (!has_postgres(postgres) || usuario == NULL || usuario->id <= 0 || !has_text(usuario->nome) || usuario->perfil_id <= 0) {
        fprintf(stderr, "[ERRO] Usuario invalido para alteracao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "usuario", usuario->id, antes, sizeof(antes));
    if (!cadastro_repository_atualizar_usuario(postgres->conn, usuario)) {
        printf("[ERRO] Usuario nao atualizado. Verifique ID, perfil e campos unicos.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "usuario", usuario->id, depois, sizeof(depois));
    audit_int_id(mongo, "usuario", usuario->id, "ALTERACAO", antes, depois);
    printf("[OK] Usuario atualizado.\n");
    return 1;
}

int cadastro_service_remover_ou_desativar_usuario(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id) {
    char acao[32];
    if (!has_postgres(postgres) || usuario_id <= 0) {
        fprintf(stderr, "[ERRO] Usuario invalido para remocao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "usuario", usuario_id, antes, sizeof(antes));
    if (!cadastro_repository_remover_ou_desativar_usuario(postgres->conn, usuario_id, acao, sizeof(acao))) {
        printf("[ERRO] Usuario nao encontrado ou nao removido.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "usuario", usuario_id, depois, sizeof(depois));
    audit_int_id(mongo, "usuario", usuario_id, acao, antes, depois);
    printf("[OK] Usuario %s.\n", acao[0] == 'E' ? "excluido" : "desativado");
    return 1;
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

void cadastro_service_buscar_autores(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        print_rows(cadastro_repository_buscar_autores(postgres->conn, termo));
    }
}

int cadastro_service_atualizar_autor(PostgresConnection *postgres, MongoConnection *mongo, const Autor *autor) {
    if (!has_postgres(postgres) || autor == NULL || autor->id <= 0 || !has_text(autor->nome)) {
        fprintf(stderr, "[ERRO] Autor invalido para alteracao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "autor", autor->id, antes, sizeof(antes));
    if (!cadastro_repository_atualizar_autor(postgres->conn, autor)) {
        printf("[ERRO] Autor nao atualizado.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "autor", autor->id, depois, sizeof(depois));
    audit_int_id(mongo, "autor", autor->id, "ALTERACAO", antes, depois);
    printf("[OK] Autor atualizado.\n");
    return 1;
}

int cadastro_service_excluir_autor(PostgresConnection *postgres, MongoConnection *mongo, int autor_id) {
    char motivo[128];
    if (!has_postgres(postgres) || autor_id <= 0) {
        fprintf(stderr, "[ERRO] Autor invalido para exclusao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "autor", autor_id, antes, sizeof(antes));
    if (!cadastro_repository_excluir_autor(postgres->conn, autor_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        return 0;
    }
    read_audit_snapshot(postgres, "autor", autor_id, depois, sizeof(depois));
    audit_int_id(mongo, "autor", autor_id, "EXCLUSAO", antes, depois);
    printf("[OK] %s\n", motivo);
    return 1;
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

void cadastro_service_buscar_editoras(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        print_rows(cadastro_repository_buscar_editoras(postgres->conn, termo));
    }
}

int cadastro_service_atualizar_editora(PostgresConnection *postgres, MongoConnection *mongo, const Editora *editora) {
    if (!has_postgres(postgres) || editora == NULL || editora->id <= 0 || !has_text(editora->nome)) {
        fprintf(stderr, "[ERRO] Editora invalida para alteracao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "editora", editora->id, antes, sizeof(antes));
    if (!cadastro_repository_atualizar_editora(postgres->conn, editora)) {
        printf("[ERRO] Editora nao atualizada.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "editora", editora->id, depois, sizeof(depois));
    audit_int_id(mongo, "editora", editora->id, "ALTERACAO", antes, depois);
    printf("[OK] Editora atualizada.\n");
    return 1;
}

int cadastro_service_excluir_editora(PostgresConnection *postgres, MongoConnection *mongo, int editora_id) {
    char motivo[128];
    if (!has_postgres(postgres) || editora_id <= 0) {
        fprintf(stderr, "[ERRO] Editora invalida para exclusao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "editora", editora_id, antes, sizeof(antes));
    if (!cadastro_repository_excluir_editora(postgres->conn, editora_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        return 0;
    }
    read_audit_snapshot(postgres, "editora", editora_id, depois, sizeof(depois));
    audit_int_id(mongo, "editora", editora_id, "EXCLUSAO", antes, depois);
    printf("[OK] %s\n", motivo);
    return 1;
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

void cadastro_service_buscar_generos(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        print_rows(cadastro_repository_buscar_generos(postgres->conn, termo));
    }
}

int cadastro_service_atualizar_genero(PostgresConnection *postgres, MongoConnection *mongo, const Genero *genero) {
    if (!has_postgres(postgres) || genero == NULL || genero->id <= 0 || !has_text(genero->nome)) {
        fprintf(stderr, "[ERRO] Genero invalido para alteracao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "genero", genero->id, antes, sizeof(antes));
    if (!cadastro_repository_atualizar_genero(postgres->conn, genero)) {
        printf("[ERRO] Genero nao atualizado.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "genero", genero->id, depois, sizeof(depois));
    audit_int_id(mongo, "genero", genero->id, "ALTERACAO", antes, depois);
    printf("[OK] Genero atualizado.\n");
    return 1;
}

int cadastro_service_excluir_genero(PostgresConnection *postgres, MongoConnection *mongo, int genero_id) {
    char motivo[128];
    if (!has_postgres(postgres) || genero_id <= 0) {
        fprintf(stderr, "[ERRO] Genero invalido para exclusao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "genero", genero_id, antes, sizeof(antes));
    if (!cadastro_repository_excluir_genero(postgres->conn, genero_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        return 0;
    }
    read_audit_snapshot(postgres, "genero", genero_id, depois, sizeof(depois));
    audit_int_id(mongo, "genero", genero_id, "EXCLUSAO", antes, depois);
    printf("[OK] %s\n", motivo);
    return 1;
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

void cadastro_service_buscar_livros(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        print_rows(cadastro_repository_buscar_livros(postgres->conn, termo));
    }
}

int cadastro_service_atualizar_livro(PostgresConnection *postgres, MongoConnection *mongo, const Livro *livro) {
    if (!has_postgres(postgres) || livro == NULL || livro->id <= 0 || !has_text(livro->titulo)) {
        fprintf(stderr, "[ERRO] Livro invalido para alteracao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "livro", livro->id, antes, sizeof(antes));
    if (!cadastro_repository_atualizar_livro(postgres->conn, livro)) {
        printf("[ERRO] Livro nao atualizado. Verifique ID, editora e campos unicos.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro->id, depois, sizeof(depois));
    audit_int_id(mongo, "livro", livro->id, "ALTERACAO", antes, depois);
    printf("[OK] Livro atualizado.\n");
    return 1;
}

int cadastro_service_excluir_livro(PostgresConnection *postgres, MongoConnection *mongo, int livro_id) {
    char motivo[128];
    if (!has_postgres(postgres) || livro_id <= 0) {
        fprintf(stderr, "[ERRO] Livro invalido para exclusao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "livro", livro_id, antes, sizeof(antes));
    if (!cadastro_repository_excluir_livro(postgres->conn, livro_id, motivo, sizeof(motivo))) {
        printf("[ERRO] %s\n", motivo);
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro_id, depois, sizeof(depois));
    audit_int_id(mongo, "livro", livro_id, "EXCLUSAO", antes, depois);
    printf("[OK] %s\n", motivo);
    return 1;
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

void cadastro_service_buscar_exemplares(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        print_rows(cadastro_repository_buscar_exemplares(postgres->conn, termo));
    }
}

int cadastro_service_atualizar_exemplar(PostgresConnection *postgres, MongoConnection *mongo, const Exemplar *exemplar) {
    if (!has_postgres(postgres) || exemplar == NULL || exemplar->id <= 0 || !has_text(exemplar->codigo_barras)) {
        fprintf(stderr, "[ERRO] Exemplar invalido para alteracao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "exemplar", exemplar->id, antes, sizeof(antes));
    if (!cadastro_repository_atualizar_exemplar(postgres->conn, exemplar)) {
        printf("[ERRO] Exemplar nao atualizado. Verifique ID e campos unicos.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "exemplar", exemplar->id, depois, sizeof(depois));
    audit_int_id(mongo, "exemplar", exemplar->id, "ALTERACAO", antes, depois);
    printf("[OK] Exemplar atualizado.\n");
    return 1;
}

int cadastro_service_remover_ou_inativar_exemplar(PostgresConnection *postgres, MongoConnection *mongo, int exemplar_id) {
    char acao[32];
    if (!has_postgres(postgres) || exemplar_id <= 0) {
        fprintf(stderr, "[ERRO] Exemplar invalido para remocao.\n");
        return 0;
    }
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    read_audit_snapshot(postgres, "exemplar", exemplar_id, antes, sizeof(antes));
    if (!cadastro_repository_remover_ou_inativar_exemplar(postgres->conn, exemplar_id, acao, sizeof(acao))) {
        printf("[ERRO] Exemplar nao encontrado ou nao removido.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "exemplar", exemplar_id, depois, sizeof(depois));
    audit_int_id(mongo, "exemplar", exemplar_id, acao, antes, depois);
    printf("[OK] Exemplar %s.\n", acao[0] == 'E' ? "excluido" : "marcado como MANUTENCAO");
    return 1;
}
