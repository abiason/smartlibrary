#ifndef SMARTLIBRARY_EMPRESTIMO_H
#define SMARTLIBRARY_EMPRESTIMO_H

typedef enum {
    EMPRESTIMO_ABERTO,
    EMPRESTIMO_ENCERRADO,
    EMPRESTIMO_CANCELADO
} StatusEmprestimo;

typedef enum {
    ORIGEM_BALCAO,
    ORIGEM_SELF_CHECKOUT
} OrigemEmprestimo;

typedef struct {
    int id;
    int usuario_id;
    char data_emprestimo[20];
    char data_prevista_devolucao[11];
    char data_encerramento[20];
    StatusEmprestimo status;
    OrigemEmprestimo origem;
} Emprestimo;

typedef struct {
    int id;
    int emprestimo_id;
    int exemplar_id;
    char data_devolucao[20];
    int quantidade_renovacoes;
} EmprestimoItem;

const char *status_emprestimo_to_string(StatusEmprestimo status);
const char *origem_emprestimo_to_string(OrigemEmprestimo origem);

#endif
