#include "exemplar.h"

const char *status_exemplar_to_string(StatusExemplar status) {
    switch (status) {
        case EXEMPLAR_DISPONIVEL:
            return "DISPONIVEL";
        case EXEMPLAR_EMPRESTADO:
            return "EMPRESTADO";
        case EXEMPLAR_RESERVADO:
            return "RESERVADO";
        case EXEMPLAR_MANUTENCAO:
            return "MANUTENCAO";
        case EXEMPLAR_EXTRAVIADO:
            return "EXTRAVIADO";
        default:
            return "DESCONHECIDO";
    }
}
