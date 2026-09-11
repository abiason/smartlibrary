#ifndef SMARTLIBRARY_RELATORIO_SERVICE_H
#define SMARTLIBRARY_RELATORIO_SERVICE_H

#include "database/postgres.h"

void relatorio_service_acervo_por_status(PostgresConnection *postgres);
void relatorio_service_emprestimos_atrasados(PostgresConnection *postgres);
void relatorio_service_livros_mais_emprestados(PostgresConnection *postgres, int limite);
void relatorio_service_emprestimos_por_origem(PostgresConnection *postgres);
void relatorio_service_reservas_ativas(PostgresConnection *postgres);
void relatorio_service_usuarios_com_pendencias(PostgresConnection *postgres);

#endif
