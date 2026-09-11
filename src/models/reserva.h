#ifndef SMARTLIBRARY_RESERVA_H
#define SMARTLIBRARY_RESERVA_H

typedef enum {
    RESERVA_ATIVA,
    RESERVA_ATENDIDA,
    RESERVA_CANCELADA,
    RESERVA_EXPIRADA
} StatusReserva;

typedef struct {
    int id;
    int usuario_id;
    int livro_id;
    char data_reserva[20];
    char data_expiracao[20];
    StatusReserva status;
} Reserva;

const char *status_reserva_to_string(StatusReserva status);

#endif
