#ifndef SMARTLIBRARY_LIVRO_H
#define SMARTLIBRARY_LIVRO_H

typedef struct {
    int id;
    char isbn[21];
    char titulo[201];
    char subtitulo[201];
    int ano_publicacao;
    int edicao;
    int editora_id;
    char idioma[51];
    char descricao[1024];
} Livro;

#endif
