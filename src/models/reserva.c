#include "reserva.h"

const char *status_reserva_to_string(StatusReserva status) {
    switch (status) {
        case RESERVA_ATIVA:
            return "ATIVA";
        case RESERVA_ATENDIDA:
            return "ATENDIDA";
        case RESERVA_CANCELADA:
            return "CANCELADA";
        case RESERVA_EXPIRADA:
            return "EXPIRADA";
        default:
            return "DESCONHECIDA";
    }
}
