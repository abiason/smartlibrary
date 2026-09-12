#include "cadastro_service.h"

#include "events/event_service.h"
#include "repositories/cadastro_repository.h"
#include "ui/console_ui.h"

#include <libpq-fe.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

static int has_text(const char *value) {
    return value != NULL && value[0] != '\0';
}

static int validar_cpf(const char *cpf) {
    char digits[12];
    int count = 0;
    int all_equal = 1;
    int sum = 0;
    int first_digit;
    int second_digit;

    if (cpf == NULL) {
        return 0;
    }

    for (const char *p = cpf; *p != '\0'; p++) {
        if (isdigit((unsigned char)*p)) {
            if (count >= 11) {
                return 0;
            }
            digits[count++] = *p;
        } else if (*p != '.' && *p != '-') {
            return 0;
        }
    }

    if (count != 11) {
        return 0;
    }
    digits[11] = '\0';

    for (int i = 1; i < 11; i++) {
        if (digits[i] != digits[0]) {
            all_equal = 0;
            break;
        }
    }
    if (all_equal) {
        return 0;
    }

    for (int i = 0; i < 9; i++) {
        sum += (digits[i] - '0') * (10 - i);
    }
    first_digit = 11 - (sum % 11);
    if (first_digit >= 10) {
        first_digit = 0;
    }
    if (first_digit != digits[9] - '0') {
        return 0;
    }

    sum = 0;
    for (int i = 0; i < 10; i++) {
        sum += (digits[i] - '0') * (11 - i);
    }
    second_digit = 11 - (sum % 11);
    if (second_digit >= 10) {
        second_digit = 0;
    }

    return second_digit == digits[10] - '0';
}

static int validar_email_opcional(const char *email) {
    const char *at;
    const char *dot;

    if (!has_text(email)) {
        return 1;
    }

    at = strchr(email, '@');
    if (at == NULL || at == email || strchr(at + 1, '@') != NULL) {
        return 0;
    }

    dot = strchr(at + 1, '.');
    return dot != NULL && dot[1] != '\0';
}

static int validar_livro(const Livro *livro) {
    if (livro == NULL || !has_text(livro->titulo)) {
        return 0;
    }
    if (livro->ano_publicacao != 0 && (livro->ano_publicacao < 1450 || livro->ano_publicacao > 2100)) {
        return 0;
    }
    if (livro->edicao < 0) {
        return 0;
    }
    return 1;
}
static int has_postgres(PostgresConnection *postgres) {
    return postgres != NULL && postgres->conn != NULL;
}

#define AUDIT_SNAPSHOT_SIZE 4096

static void read_audit_snapshot(PostgresConnection *postgres, const char *entidade, int id, char *buffer, int buffer_size) {
    if (!has_postgres(postgres) || !cadastro_repository_obter_snapshot(postgres->conn, entidade, id, buffer, buffer_size)) {
        if (buffer != NULL && buffer_size > 0) {
            snprintf(buffer, (size_t)buffer_size, "null");
        }
    }
}

static void audit_int_id(MongoConnection *mongo, const char *entidade, int id, const char *acao, int operador_id, const char *antes, const char *depois) {
    char id_text[32];
    snprintf(id_text, sizeof(id_text), "%d", id);
    event_service_registrar_auditoria_json(mongo, entidade, id_text, acao, operador_id, antes, depois);
}

int cadastro_service_criar_usuario(PostgresConnection *postgres, const Usuario *usuario) {
    if (!has_postgres(postgres) || usuario == NULL || !has_text(usuario->nome) || !has_text(usuario->cpf) || !has_text(usuario->senha_hash) || usuario->perfil_id <= 0) {
        fprintf(stderr, "[ERRO] Usuario invalido. Informe nome, CPF, senha e perfil valido.\n");
        return 0;
    }
    if (!validar_cpf(usuario->cpf)) {
        fprintf(stderr, "[ERRO] CPF invalido. Informe um CPF valido com 11 digitos.\n");
        return 0;
    }
    if (!validar_email_opcional(usuario->email)) {
        fprintf(stderr, "[ERRO] E-mail invalido.\n");
        return 0;
    }

    return cadastro_repository_criar_usuario(postgres->conn, usuario);
}

void cadastro_service_listar_usuarios(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        ui_print_pgresult_table(cadastro_repository_listar_usuarios(postgres->conn), "Nenhum registro encontrado.");
    }
}

void cadastro_service_buscar_usuarios(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        ui_print_pgresult_table(cadastro_repository_buscar_usuarios(postgres->conn, termo), "Nenhum registro encontrado.");
    }
}

int cadastro_service_atualizar_usuario(PostgresConnection *postgres, MongoConnection *mongo, const Usuario *usuario, int operador_id) {
    if (!has_postgres(postgres) || usuario == NULL || usuario->id <= 0 || !has_text(usuario->nome) || usuario->perfil_id <= 0) {
        fprintf(stderr, "[ERRO] Usuario invalido para alteracao.\n");
        return 0;
    }
    if (!validar_email_opcional(usuario->email)) {
        fprintf(stderr, "[ERRO] E-mail invalido.\n");
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
    audit_int_id(mongo, "usuario", usuario->id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Usuario atualizado.\n");
    return 1;
}

int cadastro_service_remover_ou_desativar_usuario(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, int operador_id) {
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
    audit_int_id(mongo, "usuario", usuario_id, acao, operador_id, antes, depois);
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
        ui_print_pgresult_table(cadastro_repository_listar_autores(postgres->conn), "Nenhum registro encontrado.");
    }
}

void cadastro_service_buscar_autores(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        ui_print_pgresult_table(cadastro_repository_buscar_autores(postgres->conn, termo), "Nenhum registro encontrado.");
    }
}

int cadastro_service_atualizar_autor(PostgresConnection *postgres, MongoConnection *mongo, const Autor *autor, int operador_id) {
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
    audit_int_id(mongo, "autor", autor->id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Autor atualizado.\n");
    return 1;
}

int cadastro_service_excluir_autor(PostgresConnection *postgres, MongoConnection *mongo, int autor_id, int operador_id) {
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
    audit_int_id(mongo, "autor", autor_id, "EXCLUSAO", operador_id, antes, depois);
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
        ui_print_pgresult_table(cadastro_repository_listar_editoras(postgres->conn), "Nenhum registro encontrado.");
    }
}

void cadastro_service_buscar_editoras(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        ui_print_pgresult_table(cadastro_repository_buscar_editoras(postgres->conn, termo), "Nenhum registro encontrado.");
    }
}

int cadastro_service_atualizar_editora(PostgresConnection *postgres, MongoConnection *mongo, const Editora *editora, int operador_id) {
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
    audit_int_id(mongo, "editora", editora->id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Editora atualizada.\n");
    return 1;
}

int cadastro_service_excluir_editora(PostgresConnection *postgres, MongoConnection *mongo, int editora_id, int operador_id) {
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
    audit_int_id(mongo, "editora", editora_id, "EXCLUSAO", operador_id, antes, depois);
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
        ui_print_pgresult_table(cadastro_repository_listar_generos(postgres->conn), "Nenhum registro encontrado.");
    }
}

void cadastro_service_buscar_generos(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        ui_print_pgresult_table(cadastro_repository_buscar_generos(postgres->conn, termo), "Nenhum registro encontrado.");
    }
}

int cadastro_service_atualizar_genero(PostgresConnection *postgres, MongoConnection *mongo, const Genero *genero, int operador_id) {
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
    audit_int_id(mongo, "genero", genero->id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Genero atualizado.\n");
    return 1;
}

int cadastro_service_excluir_genero(PostgresConnection *postgres, MongoConnection *mongo, int genero_id, int operador_id) {
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
    audit_int_id(mongo, "genero", genero_id, "EXCLUSAO", operador_id, antes, depois);
    printf("[OK] %s\n", motivo);
    return 1;
}

int cadastro_service_criar_livro(PostgresConnection *postgres, const Livro *livro) {
    if (!has_postgres(postgres) || !validar_livro(livro)) {
        fprintf(stderr, "[ERRO] Livro invalido. Informe titulo, ano entre 1450 e 2100 e edicao nao negativa.\n");
        return 0;
    }
    return cadastro_repository_criar_livro(postgres->conn, livro);
}

void cadastro_service_listar_livros(PostgresConnection *postgres) {
    if (has_postgres(postgres)) {
        ui_print_pgresult_table(cadastro_repository_listar_livros(postgres->conn), "Nenhum registro encontrado.");
    }
}

void cadastro_service_buscar_livros(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        ui_print_pgresult_table(cadastro_repository_buscar_livros(postgres->conn, termo), "Nenhum registro encontrado.");
    }
}

int cadastro_service_atualizar_livro(PostgresConnection *postgres, MongoConnection *mongo, const Livro *livro, int operador_id) {
    if (!has_postgres(postgres) || livro == NULL || livro->id <= 0 || !validar_livro(livro)) {
        fprintf(stderr, "[ERRO] Livro invalido para alteracao. Verifique titulo, ano e edicao.\n");
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
    audit_int_id(mongo, "livro", livro->id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Livro atualizado.\n");
    return 1;
}

int cadastro_service_excluir_livro(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int operador_id) {
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
    audit_int_id(mongo, "livro", livro_id, "EXCLUSAO", operador_id, antes, depois);
    printf("[OK] %s\n", motivo);
    return 1;
}

int cadastro_service_vincular_livro_autor(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int autor_id, int operador_id) {
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    if (!has_postgres(postgres) || livro_id <= 0 || autor_id <= 0) {
        fprintf(stderr, "[ERRO] Livro ou autor invalido para vinculacao.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro_id, antes, sizeof(antes));
    if (!cadastro_repository_vincular_livro_autor(postgres->conn, livro_id, autor_id)) {
        printf("[ERRO] Vinculo livro/autor nao criado. Verifique IDs ou vinculo existente.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro_id, depois, sizeof(depois));
    audit_int_id(mongo, "livro", livro_id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Autor vinculado ao livro.\n");
    return 1;
}

int cadastro_service_desvincular_livro_autor(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int autor_id, int operador_id) {
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    if (!has_postgres(postgres) || livro_id <= 0 || autor_id <= 0) {
        fprintf(stderr, "[ERRO] Livro ou autor invalido para desvinculacao.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro_id, antes, sizeof(antes));
    if (!cadastro_repository_desvincular_livro_autor(postgres->conn, livro_id, autor_id)) {
        printf("[ERRO] Vinculo livro/autor nao removido.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro_id, depois, sizeof(depois));
    audit_int_id(mongo, "livro", livro_id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Autor desvinculado do livro.\n");
    return 1;
}

int cadastro_service_vincular_livro_genero(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int genero_id, int operador_id) {
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    if (!has_postgres(postgres) || livro_id <= 0 || genero_id <= 0) {
        fprintf(stderr, "[ERRO] Livro ou genero invalido para vinculacao.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro_id, antes, sizeof(antes));
    if (!cadastro_repository_vincular_livro_genero(postgres->conn, livro_id, genero_id)) {
        printf("[ERRO] Vinculo livro/genero nao criado. Verifique IDs ou vinculo existente.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro_id, depois, sizeof(depois));
    audit_int_id(mongo, "livro", livro_id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Genero vinculado ao livro.\n");
    return 1;
}

int cadastro_service_desvincular_livro_genero(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int genero_id, int operador_id) {
    char antes[AUDIT_SNAPSHOT_SIZE];
    char depois[AUDIT_SNAPSHOT_SIZE];
    if (!has_postgres(postgres) || livro_id <= 0 || genero_id <= 0) {
        fprintf(stderr, "[ERRO] Livro ou genero invalido para desvinculacao.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro_id, antes, sizeof(antes));
    if (!cadastro_repository_desvincular_livro_genero(postgres->conn, livro_id, genero_id)) {
        printf("[ERRO] Vinculo livro/genero nao removido.\n");
        return 0;
    }
    read_audit_snapshot(postgres, "livro", livro_id, depois, sizeof(depois));
    audit_int_id(mongo, "livro", livro_id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Genero desvinculado do livro.\n");
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
        ui_print_pgresult_table(cadastro_repository_listar_exemplares(postgres->conn), "Nenhum registro encontrado.");
    }
}

void cadastro_service_buscar_exemplares(PostgresConnection *postgres, const char *termo) {
    if (has_postgres(postgres) && has_text(termo)) {
        ui_print_pgresult_table(cadastro_repository_buscar_exemplares(postgres->conn, termo), "Nenhum registro encontrado.");
    }
}

int cadastro_service_atualizar_exemplar(PostgresConnection *postgres, MongoConnection *mongo, const Exemplar *exemplar, int operador_id) {
    if (!has_postgres(postgres) || exemplar == NULL || exemplar->id <= 0 || !has_text(exemplar->codigo_barras)) {
        fprintf(stderr, "[ERRO] Exemplar invalido para alteracao.\n");
        return 0;
    }
    if (cadastro_repository_exemplar_tem_emprestimo_aberto(postgres->conn, exemplar->id)) {
        printf("[ERRO] Exemplar com emprestimo aberto nao pode ser alterado manualmente. Faca a devolucao primeiro.\n");
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
    audit_int_id(mongo, "exemplar", exemplar->id, "ALTERACAO", operador_id, antes, depois);
    printf("[OK] Exemplar atualizado.\n");
    return 1;
}

int cadastro_service_remover_ou_inativar_exemplar(PostgresConnection *postgres, MongoConnection *mongo, int exemplar_id, int operador_id) {
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
    audit_int_id(mongo, "exemplar", exemplar_id, acao, operador_id, antes, depois);
    printf("[OK] Exemplar %s.\n", acao[0] == 'E' ? "excluido" : "marcado como MANUTENCAO");
    return 1;
}
