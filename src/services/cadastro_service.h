#ifndef SMARTLIBRARY_CADASTRO_SERVICE_H
#define SMARTLIBRARY_CADASTRO_SERVICE_H

#include "database/mongodb.h"
#include "database/postgres.h"
#include "models/autor.h"
#include "models/editora.h"
#include "models/exemplar.h"
#include "models/genero.h"
#include "models/livro.h"
#include "models/usuario.h"

int cadastro_service_criar_usuario(PostgresConnection *postgres, const Usuario *usuario);
void cadastro_service_listar_usuarios(PostgresConnection *postgres);
void cadastro_service_buscar_usuarios(PostgresConnection *postgres, const char *termo);
int cadastro_service_atualizar_usuario(PostgresConnection *postgres, MongoConnection *mongo, const Usuario *usuario, int operador_id);
int cadastro_service_remover_ou_desativar_usuario(PostgresConnection *postgres, MongoConnection *mongo, int usuario_id, int operador_id);

int cadastro_service_criar_autor(PostgresConnection *postgres, const Autor *autor);
void cadastro_service_listar_autores(PostgresConnection *postgres);
void cadastro_service_buscar_autores(PostgresConnection *postgres, const char *termo);
int cadastro_service_atualizar_autor(PostgresConnection *postgres, MongoConnection *mongo, const Autor *autor, int operador_id);
int cadastro_service_excluir_autor(PostgresConnection *postgres, MongoConnection *mongo, int autor_id, int operador_id);

int cadastro_service_criar_editora(PostgresConnection *postgres, const Editora *editora);
void cadastro_service_listar_editoras(PostgresConnection *postgres);
void cadastro_service_buscar_editoras(PostgresConnection *postgres, const char *termo);
int cadastro_service_atualizar_editora(PostgresConnection *postgres, MongoConnection *mongo, const Editora *editora, int operador_id);
int cadastro_service_excluir_editora(PostgresConnection *postgres, MongoConnection *mongo, int editora_id, int operador_id);

int cadastro_service_criar_genero(PostgresConnection *postgres, const Genero *genero);
void cadastro_service_listar_generos(PostgresConnection *postgres);
void cadastro_service_buscar_generos(PostgresConnection *postgres, const char *termo);
int cadastro_service_atualizar_genero(PostgresConnection *postgres, MongoConnection *mongo, const Genero *genero, int operador_id);
int cadastro_service_excluir_genero(PostgresConnection *postgres, MongoConnection *mongo, int genero_id, int operador_id);

int cadastro_service_criar_livro(PostgresConnection *postgres, const Livro *livro);
void cadastro_service_listar_livros(PostgresConnection *postgres);
void cadastro_service_buscar_livros(PostgresConnection *postgres, const char *termo);
int cadastro_service_atualizar_livro(PostgresConnection *postgres, MongoConnection *mongo, const Livro *livro, int operador_id);
int cadastro_service_excluir_livro(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int operador_id);
int cadastro_service_vincular_livro_autor(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int autor_id, int operador_id);
int cadastro_service_desvincular_livro_autor(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int autor_id, int operador_id);
int cadastro_service_vincular_livro_genero(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int genero_id, int operador_id);
int cadastro_service_desvincular_livro_genero(PostgresConnection *postgres, MongoConnection *mongo, int livro_id, int genero_id, int operador_id);

int cadastro_service_criar_exemplar(PostgresConnection *postgres, const Exemplar *exemplar);
void cadastro_service_listar_exemplares(PostgresConnection *postgres);
void cadastro_service_buscar_exemplares(PostgresConnection *postgres, const char *termo);
int cadastro_service_atualizar_exemplar(PostgresConnection *postgres, MongoConnection *mongo, const Exemplar *exemplar, int operador_id);
int cadastro_service_remover_ou_inativar_exemplar(PostgresConnection *postgres, MongoConnection *mongo, int exemplar_id, int operador_id);

#endif
