#ifndef SMARTLIBRARY_CADASTRO_SERVICE_H
#define SMARTLIBRARY_CADASTRO_SERVICE_H

#include "database/postgres.h"
#include "models/autor.h"
#include "models/editora.h"
#include "models/exemplar.h"
#include "models/genero.h"
#include "models/livro.h"
#include "models/usuario.h"

int cadastro_service_criar_usuario(PostgresConnection *postgres, const Usuario *usuario);
void cadastro_service_listar_usuarios(PostgresConnection *postgres);
int cadastro_service_criar_autor(PostgresConnection *postgres, const Autor *autor);
void cadastro_service_listar_autores(PostgresConnection *postgres);
int cadastro_service_criar_editora(PostgresConnection *postgres, const Editora *editora);
void cadastro_service_listar_editoras(PostgresConnection *postgres);
int cadastro_service_criar_genero(PostgresConnection *postgres, const Genero *genero);
void cadastro_service_listar_generos(PostgresConnection *postgres);
int cadastro_service_criar_livro(PostgresConnection *postgres, const Livro *livro);
void cadastro_service_listar_livros(PostgresConnection *postgres);
int cadastro_service_criar_exemplar(PostgresConnection *postgres, const Exemplar *exemplar);
void cadastro_service_listar_exemplares(PostgresConnection *postgres);

#endif
