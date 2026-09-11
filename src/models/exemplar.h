#ifndef SMARTLIBRARY_EXEMPLAR_H
#define SMARTLIBRARY_EXEMPLAR_H

typedef enum {
    EXEMPLAR_DISPONIVEL,
    EXEMPLAR_EMPRESTADO,
    EXEMPLAR_RESERVADO,
    EXEMPLAR_MANUTENCAO,
    EXEMPLAR_EXTRAVIADO
} StatusExemplar;

typedef struct {
    int id;
    int livro_id;
    char codigo_barras[51];
    char rfid[101];
    StatusExemplar status;
    char localizacao[101];
    char data_aquisicao[11];
} Exemplar;

const char *status_exemplar_to_string(StatusExemplar status);

#endif
