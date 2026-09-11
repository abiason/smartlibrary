#ifndef SMARTLIBRARY_EMPRESTIMO_REPOSITORY_H
#define SMARTLIBRARY_EMPRESTIMO_REPOSITORY_H

#include <libpq-fe.h>

int emprestimo_repository_realizar_emprestimo(PGconn *conn, int usuario_id, const char *codigo_barras, int *emprestimo_id, int *exemplar_id, char *motivo, int motivo_size);
int emprestimo_repository_realizar_devolucao(PGconn *conn, const char *codigo_barras, int *usuario_id, int *emprestimo_id, int *exemplar_id, char *motivo, int motivo_size);
int emprestimo_repository_renovar_item(PGconn *conn, int emprestimo_item_id, int *usuario_id, int *emprestimo_id, int *exemplar_id, char *codigo_barras, int codigo_size, char *motivo, int motivo_size);
PGresult *emprestimo_repository_listar_emprestimos_abertos(PGconn *conn);

#endif
