#ifndef SMARTLIBRARY_USUARIO_H
#define SMARTLIBRARY_USUARIO_H

#include <stdbool.h>

typedef struct {
    int id;
    char nome[151];
    char cpf[15];
    char email[151];
    char telefone[21];
    char senha_hash[256];
    int perfil_id;
    bool ativo;
    bool bloqueado;
} Usuario;

#endif
