#include "cadastro_repository.h"

#include <stdio.h>

static int exec_insert(PGconn *conn, const char *sql, int count, const char *const *params) {
    PGresult *result;
    ExecStatusType status;

    result = PQexecParams(conn, sql, count, NULL, params, NULL, NULL, 0);
    status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }

    PQclear(result);
    return 1;
}

static PGresult *exec_select(PGconn *conn, const char *sql) {
    PGresult *result = PQexecParams(conn, sql, 0, NULL, NULL, NULL, NULL, 0);
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(conn));
        PQclear(result);
        return NULL;
    }

    return result;
}

int cadastro_repository_criar_usuario(PGconn *conn, const Usuario *usuario) {
    char perfil_id[16];
    const char *params[8];

    snprintf(perfil_id, sizeof(perfil_id), "%d", usuario->perfil_id);
    params[0] = usuario->nome;
    params[1] = usuario->cpf;
    params[2] = usuario->email;
    params[3] = usuario->telefone;
    params[4] = usuario->senha_hash;
    params[5] = perfil_id;
    params[6] = usuario->ativo ? "true" : "false";
    params[7] = usuario->bloqueado ? "true" : "false";

    return exec_insert(conn,
        "INSERT INTO usuario (nome, cpf, email, telefone, senha_hash, id_perfil, ativo, bloqueado) "
        "VALUES ($1, $2, NULLIF($3, ''), NULLIF($4, ''), $5, $6::integer, $7::boolean, $8::boolean)",
        8, params);
}

PGresult *cadastro_repository_listar_usuarios(PGconn *conn) {
    return exec_select(conn,
        "SELECT u.id_usuario, u.nome, u.cpf, COALESCE(u.email, ''), p.nome AS perfil, u.ativo, u.bloqueado "
        "FROM usuario u INNER JOIN perfil p ON p.id_perfil = u.id_perfil ORDER BY u.id_usuario");
}

int cadastro_repository_criar_autor(PGconn *conn, const Autor *autor) {
    const char *params[2] = { autor->nome, autor->nacionalidade };
    return exec_insert(conn,
        "INSERT INTO autor (nome, nacionalidade) VALUES ($1, NULLIF($2, ''))",
        2, params);
}

PGresult *cadastro_repository_listar_autores(PGconn *conn) {
    return exec_select(conn,
        "SELECT id_autor, nome, COALESCE(nacionalidade, '') AS nacionalidade FROM autor ORDER BY id_autor");
}

int cadastro_repository_criar_editora(PGconn *conn, const Editora *editora) {
    const char *params[3] = { editora->nome, editora->cidade, editora->pais };
    return exec_insert(conn,
        "INSERT INTO editora (nome, cidade, pais) VALUES ($1, NULLIF($2, ''), NULLIF($3, ''))",
        3, params);
}

PGresult *cadastro_repository_listar_editoras(PGconn *conn) {
    return exec_select(conn,
        "SELECT id_editora, nome, COALESCE(cidade, '') AS cidade, COALESCE(pais, '') AS pais FROM editora ORDER BY id_editora");
}

int cadastro_repository_criar_genero(PGconn *conn, const Genero *genero) {
    const char *params[2] = { genero->nome, genero->descricao };
    return exec_insert(conn,
        "INSERT INTO genero (nome, descricao) VALUES ($1, NULLIF($2, ''))",
        2, params);
}

PGresult *cadastro_repository_listar_generos(PGconn *conn) {
    return exec_select(conn,
        "SELECT id_genero, nome, COALESCE(descricao, '') AS descricao FROM genero ORDER BY id_genero");
}
int cadastro_repository_criar_livro(PGconn *conn, const Livro *livro) {
    char ano_publicacao[16];
    char edicao[16];
    char editora_id[16];
    const char *params[8];

    snprintf(ano_publicacao, sizeof(ano_publicacao), "%d", livro->ano_publicacao);
    snprintf(edicao, sizeof(edicao), "%d", livro->edicao);
    snprintf(editora_id, sizeof(editora_id), "%d", livro->editora_id);

    params[0] = livro->isbn;
    params[1] = livro->titulo;
    params[2] = livro->subtitulo;
    params[3] = livro->ano_publicacao > 0 ? ano_publicacao : "";
    params[4] = livro->edicao > 0 ? edicao : "";
    params[5] = livro->editora_id > 0 ? editora_id : "";
    params[6] = livro->idioma;
    params[7] = livro->descricao;

    return exec_insert(conn,
        "INSERT INTO livro (isbn, titulo, subtitulo, ano_publicacao, edicao, id_editora, idioma, descricao) "
        "VALUES (NULLIF($1, ''), $2, NULLIF($3, ''), NULLIF($4, '')::integer, "
        "NULLIF($5, '')::integer, NULLIF($6, '')::integer, NULLIF($7, ''), NULLIF($8, ''))",
        8, params);
}

PGresult *cadastro_repository_listar_livros(PGconn *conn) {
    return exec_select(conn,
        "SELECT l.id_livro, COALESCE(l.isbn, ''), l.titulo, COALESCE(e.nome, ''), "
        "COALESCE(l.ano_publicacao::text, '') "
        "FROM livro l LEFT JOIN editora e ON e.id_editora = l.id_editora ORDER BY l.id_livro");
}

int cadastro_repository_criar_exemplar(PGconn *conn, const Exemplar *exemplar) {
    char livro_id[16];
    const char *params[5];

    snprintf(livro_id, sizeof(livro_id), "%d", exemplar->livro_id);
    params[0] = livro_id;
    params[1] = exemplar->codigo_barras;
    params[2] = exemplar->rfid;
    params[3] = status_exemplar_to_string(exemplar->status);
    params[4] = exemplar->localizacao;

    return exec_insert(conn,
        "INSERT INTO exemplar (id_livro, codigo_barras, rfid, status, localizacao) "
        "VALUES ($1::integer, $2, NULLIF($3, ''), $4, NULLIF($5, ''))",
        5, params);
}

PGresult *cadastro_repository_listar_exemplares(PGconn *conn) {
    return exec_select(conn,
        "SELECT e.id_exemplar, e.codigo_barras, COALESCE(e.rfid, ''), e.status, l.titulo "
        "FROM exemplar e INNER JOIN livro l ON l.id_livro = e.id_livro ORDER BY e.id_exemplar");
}

