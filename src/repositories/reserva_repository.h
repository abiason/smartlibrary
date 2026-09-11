#ifndef SMARTLIBRARY_RESERVA_REPOSITORY_H
#define SMARTLIBRARY_RESERVA_REPOSITORY_H

#include <libpq-fe.h>

int reserva_repository_criar(PGconn *conn, int usuario_id, int livro_id, char *motivo, int motivo_size);
int reserva_repository_cancelar(PGconn *conn, int reserva_id, char *motivo, int motivo_size);
int reserva_repository_atender(PGconn *conn, int reserva_id, char *motivo, int motivo_size);
int reserva_repository_expirar_vencidas(PGconn *conn);
PGresult *reserva_repository_listar(PGconn *conn);

#endif
