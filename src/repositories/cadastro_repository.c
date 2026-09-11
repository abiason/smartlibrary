#include "cadastro_repository.h"

#include <stdio.h>
#include <stdlib.h>

static int exec_command_ok(PGconn *conn, const char *sql, int count, const char *const *params) {
    PGresult *result = PQexecParams(conn, sql, count, NULL, params, NULL, NULL, 0);
    ExecStatusType status = PQresultStatus(result);
    if (status != PGRES_COMMAND_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(conn));
        PQclear(result);
        return 0;
    }

    if (atoi(PQcmdTuples(result)) == 0) {
        PQclear(result);
        return 0;
    }

    PQclear(result);
    return 1;
}

static int exec_insert(PGconn *conn, const char *sql, int count, const char *const *params) {
    PGresult *result = PQexecParams(conn, sql, count, NULL, params, NULL, NULL, 0);
    ExecStatusType status = PQresultStatus(result);
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

static PGresult *exec_select_params(PGconn *conn, const char *sql, int count, const char *const *params) {
    PGresult *result = PQexecParams(conn, sql, count, NULL, params, NULL, NULL, 0);
    if (PQresultStatus(result) != PGRES_TUPLES_OK) {
        fprintf(stderr, "[ERRO] PostgreSQL: %s", PQerrorMessage(conn));
        PQclear(result);
        return NULL;
    }

    return result;
}

static int scalar_int(PGconn *conn, const char *sql, int count, const char *const *params) {
    PGresult *result = exec_select_params(conn, sql, count, params);
    int value = 0;
    if (result != NULL && PQntuples(result) > 0) {
        value = atoi(PQgetvalue(result, 0, 0));
    }
    if (result != NULL) {
        PQclear(result);
    }
    return value;
}

static void set_text(char *buffer, int size, const char *text) {
    if (buffer != NULL && size > 0) {
        snprintf(buffer, (size_t)size, "%s", text != NULL ? text : "");
    }
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

PGresult *cadastro_repository_buscar_usuarios(PGconn *conn, const char *termo) {
    const char *params[1] = { termo };
    return exec_select_params(conn,
        "SELECT u.id_usuario, u.nome, u.cpf, COALESCE(u.email, ''), p.nome AS perfil, u.ativo, u.bloqueado "
        "FROM usuario u INNER JOIN perfil p ON p.id_perfil = u.id_perfil "
        "WHERE u.id_usuario::text = $1 OR u.cpf = $1 OR u.nome ILIKE '%' || $1 || '%' "
        "ORDER BY u.id_usuario",
        1, params);
}

int cadastro_repository_atualizar_usuario(PGconn *conn, const Usuario *usuario) {
    char id[16];
    char perfil_id[16];
    const char *params[7];

    snprintf(id, sizeof(id), "%d", usuario->id);
    snprintf(perfil_id, sizeof(perfil_id), "%d", usuario->perfil_id);
    params[0] = id;
    params[1] = usuario->nome;
    params[2] = usuario->email;
    params[3] = usuario->telefone;
    params[4] = perfil_id;
    params[5] = usuario->ativo ? "true" : "false";
    params[6] = usuario->bloqueado ? "true" : "false";

    return exec_command_ok(conn,
        "UPDATE usuario SET nome = $2, email = NULLIF($3, ''), telefone = NULLIF($4, ''), "
        "id_perfil = $5::integer, ativo = $6::boolean, bloqueado = $7::boolean "
        "WHERE id_usuario = $1::integer",
        7, params);
}

int cadastro_repository_remover_ou_desativar_usuario(PGconn *conn, int usuario_id, char *acao, int acao_size) {
    char id[16];
    const char *params[1];
    snprintf(id, sizeof(id), "%d", usuario_id);
    params[0] = id;

    if (scalar_int(conn, "SELECT COUNT(*) FROM emprestimo WHERE id_usuario = $1::integer", 1, params) > 0 ||
        scalar_int(conn, "SELECT COUNT(*) FROM reserva WHERE id_usuario = $1::integer", 1, params) > 0) {
        if (!exec_command_ok(conn, "UPDATE usuario SET ativo = false WHERE id_usuario = $1::integer", 1, params)) {
            return 0;
        }
        set_text(acao, acao_size, "DESATIVACAO");
        return 1;
    }

    if (!exec_command_ok(conn, "DELETE FROM usuario WHERE id_usuario = $1::integer", 1, params)) {
        return 0;
    }
    set_text(acao, acao_size, "EXCLUSAO");
    return 1;
}

int cadastro_repository_criar_autor(PGconn *conn, const Autor *autor) {
    const char *params[2] = { autor->nome, autor->nacionalidade };
    return exec_insert(conn, "INSERT INTO autor (nome, nacionalidade) VALUES ($1, NULLIF($2, ''))", 2, params);
}

PGresult *cadastro_repository_listar_autores(PGconn *conn) {
    return exec_select(conn, "SELECT id_autor, nome, COALESCE(nacionalidade, '') AS nacionalidade FROM autor ORDER BY id_autor");
}

PGresult *cadastro_repository_buscar_autores(PGconn *conn, const char *termo) {
    const char *params[1] = { termo };
    return exec_select_params(conn, "SELECT id_autor, nome, COALESCE(nacionalidade, '') AS nacionalidade FROM autor WHERE id_autor::text = $1 OR nome ILIKE '%' || $1 || '%' ORDER BY id_autor", 1, params);
}

int cadastro_repository_atualizar_autor(PGconn *conn, const Autor *autor) {
    char id[16];
    const char *params[3];
    snprintf(id, sizeof(id), "%d", autor->id);
    params[0] = id;
    params[1] = autor->nome;
    params[2] = autor->nacionalidade;
    return exec_command_ok(conn, "UPDATE autor SET nome = $2, nacionalidade = NULLIF($3, '') WHERE id_autor = $1::integer", 3, params);
}

int cadastro_repository_excluir_autor(PGconn *conn, int autor_id, char *motivo, int motivo_size) {
    char id[16];
    const char *params[1];
    snprintf(id, sizeof(id), "%d", autor_id);
    params[0] = id;
    if (scalar_int(conn, "SELECT COUNT(*) FROM livro_autor WHERE id_autor = $1::integer", 1, params) > 0) {
        set_text(motivo, motivo_size, "Autor vinculado a livro.");
        return 0;
    }
    if (!exec_command_ok(conn, "DELETE FROM autor WHERE id_autor = $1::integer", 1, params)) {
        set_text(motivo, motivo_size, "Autor nao encontrado.");
        return 0;
    }
    set_text(motivo, motivo_size, "Autor excluido.");
    return 1;
}

int cadastro_repository_criar_editora(PGconn *conn, const Editora *editora) {
    const char *params[3] = { editora->nome, editora->cidade, editora->pais };
    return exec_insert(conn, "INSERT INTO editora (nome, cidade, pais) VALUES ($1, NULLIF($2, ''), NULLIF($3, ''))", 3, params);
}

PGresult *cadastro_repository_listar_editoras(PGconn *conn) {
    return exec_select(conn, "SELECT id_editora, nome, COALESCE(cidade, '') AS cidade, COALESCE(pais, '') AS pais FROM editora ORDER BY id_editora");
}

PGresult *cadastro_repository_buscar_editoras(PGconn *conn, const char *termo) {
    const char *params[1] = { termo };
    return exec_select_params(conn, "SELECT id_editora, nome, COALESCE(cidade, '') AS cidade, COALESCE(pais, '') AS pais FROM editora WHERE id_editora::text = $1 OR nome ILIKE '%' || $1 || '%' ORDER BY id_editora", 1, params);
}

int cadastro_repository_atualizar_editora(PGconn *conn, const Editora *editora) {
    char id[16];
    const char *params[4];
    snprintf(id, sizeof(id), "%d", editora->id);
    params[0] = id;
    params[1] = editora->nome;
    params[2] = editora->cidade;
    params[3] = editora->pais;
    return exec_command_ok(conn, "UPDATE editora SET nome = $2, cidade = NULLIF($3, ''), pais = NULLIF($4, '') WHERE id_editora = $1::integer", 4, params);
}

int cadastro_repository_excluir_editora(PGconn *conn, int editora_id, char *motivo, int motivo_size) {
    char id[16];
    const char *params[1];
    snprintf(id, sizeof(id), "%d", editora_id);
    params[0] = id;
    if (scalar_int(conn, "SELECT COUNT(*) FROM livro WHERE id_editora = $1::integer", 1, params) > 0) {
        set_text(motivo, motivo_size, "Editora vinculada a livro.");
        return 0;
    }
    if (!exec_command_ok(conn, "DELETE FROM editora WHERE id_editora = $1::integer", 1, params)) {
        set_text(motivo, motivo_size, "Editora nao encontrada.");
        return 0;
    }
    set_text(motivo, motivo_size, "Editora excluida.");
    return 1;
}

int cadastro_repository_criar_genero(PGconn *conn, const Genero *genero) {
    const char *params[2] = { genero->nome, genero->descricao };
    return exec_insert(conn, "INSERT INTO genero (nome, descricao) VALUES ($1, NULLIF($2, ''))", 2, params);
}

PGresult *cadastro_repository_listar_generos(PGconn *conn) {
    return exec_select(conn, "SELECT id_genero, nome, COALESCE(descricao, '') AS descricao FROM genero ORDER BY id_genero");
}

PGresult *cadastro_repository_buscar_generos(PGconn *conn, const char *termo) {
    const char *params[1] = { termo };
    return exec_select_params(conn, "SELECT id_genero, nome, COALESCE(descricao, '') AS descricao FROM genero WHERE id_genero::text = $1 OR nome ILIKE '%' || $1 || '%' ORDER BY id_genero", 1, params);
}

int cadastro_repository_atualizar_genero(PGconn *conn, const Genero *genero) {
    char id[16];
    const char *params[3];
    snprintf(id, sizeof(id), "%d", genero->id);
    params[0] = id;
    params[1] = genero->nome;
    params[2] = genero->descricao;
    return exec_command_ok(conn, "UPDATE genero SET nome = $2, descricao = NULLIF($3, '') WHERE id_genero = $1::integer", 3, params);
}

int cadastro_repository_excluir_genero(PGconn *conn, int genero_id, char *motivo, int motivo_size) {
    char id[16];
    const char *params[1];
    snprintf(id, sizeof(id), "%d", genero_id);
    params[0] = id;
    if (scalar_int(conn, "SELECT COUNT(*) FROM livro_genero WHERE id_genero = $1::integer", 1, params) > 0) {
        set_text(motivo, motivo_size, "Genero vinculado a livro.");
        return 0;
    }
    if (!exec_command_ok(conn, "DELETE FROM genero WHERE id_genero = $1::integer", 1, params)) {
        set_text(motivo, motivo_size, "Genero nao encontrado.");
        return 0;
    }
    set_text(motivo, motivo_size, "Genero excluido.");
    return 1;
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
        "SELECT l.id_livro, COALESCE(l.isbn, ''), l.titulo, COALESCE(e.nome, ''), COALESCE(l.ano_publicacao::text, '') "
        "FROM livro l LEFT JOIN editora e ON e.id_editora = l.id_editora ORDER BY l.id_livro");
}

PGresult *cadastro_repository_buscar_livros(PGconn *conn, const char *termo) {
    const char *params[1] = { termo };
    return exec_select_params(conn,
        "SELECT l.id_livro, COALESCE(l.isbn, '') AS isbn, l.titulo, COALESCE(e.nome, '') AS editora, COALESCE(l.ano_publicacao::text, '') AS ano "
        "FROM livro l LEFT JOIN editora e ON e.id_editora = l.id_editora "
        "WHERE l.id_livro::text = $1 OR l.isbn = $1 OR l.titulo ILIKE '%' || $1 || '%' "
        "ORDER BY l.id_livro",
        1, params);
}

int cadastro_repository_atualizar_livro(PGconn *conn, const Livro *livro) {
    char id[16];
    char ano_publicacao[16];
    char edicao[16];
    char editora_id[16];
    const char *params[9];

    snprintf(id, sizeof(id), "%d", livro->id);
    snprintf(ano_publicacao, sizeof(ano_publicacao), "%d", livro->ano_publicacao);
    snprintf(edicao, sizeof(edicao), "%d", livro->edicao);
    snprintf(editora_id, sizeof(editora_id), "%d", livro->editora_id);

    params[0] = id;
    params[1] = livro->isbn;
    params[2] = livro->titulo;
    params[3] = livro->subtitulo;
    params[4] = livro->ano_publicacao > 0 ? ano_publicacao : "";
    params[5] = livro->edicao > 0 ? edicao : "";
    params[6] = livro->editora_id > 0 ? editora_id : "";
    params[7] = livro->idioma;
    params[8] = livro->descricao;

    return exec_command_ok(conn,
        "UPDATE livro SET isbn = NULLIF($2, ''), titulo = $3, subtitulo = NULLIF($4, ''), "
        "ano_publicacao = NULLIF($5, '')::integer, edicao = NULLIF($6, '')::integer, "
        "id_editora = NULLIF($7, '')::integer, idioma = NULLIF($8, ''), descricao = NULLIF($9, '') "
        "WHERE id_livro = $1::integer",
        9, params);
}

int cadastro_repository_excluir_livro(PGconn *conn, int livro_id, char *motivo, int motivo_size) {
    char id[16];
    const char *params[1];
    snprintf(id, sizeof(id), "%d", livro_id);
    params[0] = id;
    if (scalar_int(conn, "SELECT COUNT(*) FROM exemplar WHERE id_livro = $1::integer", 1, params) > 0 ||
        scalar_int(conn, "SELECT COUNT(*) FROM reserva WHERE id_livro = $1::integer", 1, params) > 0) {
        set_text(motivo, motivo_size, "Livro possui exemplares ou reservas vinculadas.");
        return 0;
    }
    if (!exec_command_ok(conn, "DELETE FROM livro WHERE id_livro = $1::integer", 1, params)) {
        set_text(motivo, motivo_size, "Livro nao encontrado.");
        return 0;
    }
    set_text(motivo, motivo_size, "Livro excluido.");
    return 1;
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

PGresult *cadastro_repository_buscar_exemplares(PGconn *conn, const char *termo) {
    const char *params[1] = { termo };
    return exec_select_params(conn,
        "SELECT e.id_exemplar, e.codigo_barras, COALESCE(e.rfid, '') AS rfid, e.status, l.titulo "
        "FROM exemplar e INNER JOIN livro l ON l.id_livro = e.id_livro "
        "WHERE e.id_exemplar::text = $1 OR e.codigo_barras = $1 OR e.rfid = $1 OR l.titulo ILIKE '%' || $1 || '%' "
        "ORDER BY e.id_exemplar",
        1, params);
}

int cadastro_repository_atualizar_exemplar(PGconn *conn, const Exemplar *exemplar) {
    char id[16];
    const char *params[5];
    snprintf(id, sizeof(id), "%d", exemplar->id);
    params[0] = id;
    params[1] = exemplar->codigo_barras;
    params[2] = exemplar->rfid;
    params[3] = status_exemplar_to_string(exemplar->status);
    params[4] = exemplar->localizacao;
    return exec_command_ok(conn,
        "UPDATE exemplar SET codigo_barras = $2, rfid = NULLIF($3, ''), status = $4, localizacao = NULLIF($5, '') "
        "WHERE id_exemplar = $1::integer",
        5, params);
}

int cadastro_repository_remover_ou_inativar_exemplar(PGconn *conn, int exemplar_id, char *acao, int acao_size) {
    char id[16];
    const char *params[1];
    snprintf(id, sizeof(id), "%d", exemplar_id);
    params[0] = id;

    if (scalar_int(conn, "SELECT COUNT(*) FROM emprestimo_item WHERE id_exemplar = $1::integer", 1, params) > 0) {
        if (!exec_command_ok(conn, "UPDATE exemplar SET status = 'MANUTENCAO' WHERE id_exemplar = $1::integer", 1, params)) {
            return 0;
        }
        set_text(acao, acao_size, "ALTERACAO");
        return 1;
    }

    if (!exec_command_ok(conn, "DELETE FROM exemplar WHERE id_exemplar = $1::integer", 1, params)) {
        return 0;
    }
    set_text(acao, acao_size, "EXCLUSAO");
    return 1;
}
