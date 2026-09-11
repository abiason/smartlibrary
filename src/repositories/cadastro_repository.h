#ifndef SMARTLIBRARY_CADASTRO_REPOSITORY_H
#define SMARTLIBRARY_CADASTRO_REPOSITORY_H

#include "models/autor.h"
#include "models/editora.h"
#include "models/exemplar.h"
#include "models/genero.h"
#include "models/livro.h"
#include "models/usuario.h"

#include <libpq-fe.h>

int cadastro_repository_criar_usuario(PGconn *conn, const Usuario *usuario);
PGresult *cadastro_repository_listar_usuarios(PGconn *conn);
PGresult *cadastro_repository_buscar_usuarios(PGconn *conn, const char *termo);
int cadastro_repository_atualizar_usuario(PGconn *conn, const Usuario *usuario);
int cadastro_repository_remover_ou_desativar_usuario(PGconn *conn, int usuario_id, char *acao, int acao_size);

int cadastro_repository_criar_autor(PGconn *conn, const Autor *autor);
PGresult *cadastro_repository_listar_autores(PGconn *conn);
PGresult *cadastro_repository_buscar_autores(PGconn *conn, const char *termo);
int cadastro_repository_atualizar_autor(PGconn *conn, const Autor *autor);
int cadastro_repository_excluir_autor(PGconn *conn, int autor_id, char *motivo, int motivo_size);

int cadastro_repository_criar_editora(PGconn *conn, const Editora *editora);
PGresult *cadastro_repository_listar_editoras(PGconn *conn);
PGresult *cadastro_repository_buscar_editoras(PGconn *conn, const char *termo);
int cadastro_repository_atualizar_editora(PGconn *conn, const Editora *editora);
int cadastro_repository_excluir_editora(PGconn *conn, int editora_id, char *motivo, int motivo_size);

int cadastro_repository_criar_genero(PGconn *conn, const Genero *genero);
PGresult *cadastro_repository_listar_generos(PGconn *conn);
PGresult *cadastro_repository_buscar_generos(PGconn *conn, const char *termo);
int cadastro_repository_atualizar_genero(PGconn *conn, const Genero *genero);
int cadastro_repository_excluir_genero(PGconn *conn, int genero_id, char *motivo, int motivo_size);

int cadastro_repository_criar_livro(PGconn *conn, const Livro *livro);
PGresult *cadastro_repository_listar_livros(PGconn *conn);
PGresult *cadastro_repository_buscar_livros(PGconn *conn, const char *termo);
int cadastro_repository_atualizar_livro(PGconn *conn, const Livro *livro);
int cadastro_repository_excluir_livro(PGconn *conn, int livro_id, char *motivo, int motivo_size);

int cadastro_repository_criar_exemplar(PGconn *conn, const Exemplar *exemplar);
PGresult *cadastro_repository_listar_exemplares(PGconn *conn);
PGresult *cadastro_repository_buscar_exemplares(PGconn *conn, const char *termo);
int cadastro_repository_atualizar_exemplar(PGconn *conn, const Exemplar *exemplar);
int cadastro_repository_remover_ou_inativar_exemplar(PGconn *conn, int exemplar_id, char *acao, int acao_size);

#endif
