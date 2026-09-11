#ifndef SMARTLIBRARY_SELF_CHECKOUT_REPOSITORY_H
#define SMARTLIBRARY_SELF_CHECKOUT_REPOSITORY_H

#include <libpq-fe.h>

int self_checkout_repository_identificar_usuario(PGconn *conn, const char *cpf, int *usuario_id, char *nome, int nome_size, char *motivo, int motivo_size);
PGresult *self_checkout_repository_listar_emprestimos_usuario(PGconn *conn, int usuario_id);
PGresult *self_checkout_repository_listar_reservas_usuario(PGconn *conn, int usuario_id);
PGresult *self_checkout_repository_pesquisar_livros(PGconn *conn, const char *termo);
int self_checkout_repository_item_pertence_usuario(PGconn *conn, int usuario_id, int emprestimo_item_id);

#endif

