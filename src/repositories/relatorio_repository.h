#ifndef SMARTLIBRARY_RELATORIO_REPOSITORY_H
#define SMARTLIBRARY_RELATORIO_REPOSITORY_H

#include <libpq-fe.h>

PGresult *relatorio_repository_acervo_por_status(PGconn *conn);
PGresult *relatorio_repository_emprestimos_atrasados(PGconn *conn);
PGresult *relatorio_repository_livros_mais_emprestados(PGconn *conn, int limite);
PGresult *relatorio_repository_emprestimos_por_origem(PGconn *conn);
PGresult *relatorio_repository_reservas_ativas(PGconn *conn);
PGresult *relatorio_repository_usuarios_com_pendencias(PGconn *conn);

#endif
