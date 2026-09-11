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
int cadastro_repository_criar_autor(PGconn *conn, const Autor *autor);
PGresult *cadastro_repository_listar_autores(PGconn *conn);
int cadastro_repository_criar_editora(PGconn *conn, const Editora *editora);
PGresult *cadastro_repository_listar_editoras(PGconn *conn);
int cadastro_repository_criar_genero(PGconn *conn, const Genero *genero);
PGresult *cadastro_repository_listar_generos(PGconn *conn);
int cadastro_repository_criar_livro(PGconn *conn, const Livro *livro);
PGresult *cadastro_repository_listar_livros(PGconn *conn);
int cadastro_repository_criar_exemplar(PGconn *conn, const Exemplar *exemplar);
PGresult *cadastro_repository_listar_exemplares(PGconn *conn);

#endif
