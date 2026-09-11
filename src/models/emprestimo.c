#include "emprestimo.h"

const char *status_emprestimo_to_string(StatusEmprestimo status) {
    switch (status) {
        case EMPRESTIMO_ABERTO:
            return "ABERTO";
        case EMPRESTIMO_ENCERRADO:
            return "ENCERRADO";
        case EMPRESTIMO_CANCELADO:
            return "CANCELADO";
        default:
            return "DESCONHECIDO";
    }
}

const char *origem_emprestimo_to_string(OrigemEmprestimo origem) {
    switch (origem) {
        case ORIGEM_BALCAO:
            return "BALCAO";
        case ORIGEM_SELF_CHECKOUT:
            return "SELF_CHECKOUT";
        default:
            return "DESCONHECIDA";
    }
}
