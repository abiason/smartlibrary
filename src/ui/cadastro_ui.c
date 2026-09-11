#include "cadastro_ui.h"

#include "models/autor.h"
#include "models/editora.h"
#include "models/exemplar.h"
#include "models/genero.h"
#include "models/livro.h"
#include "models/usuario.h"
#include "services/cadastro_service.h"
#include "utils/input.h"

#include <stdbool.h>
#include <stdio.h>

static void print_result(int ok) {
    printf(ok ? "[OK] Cadastro salvo.\n" : "[ERRO] Cadastro nao foi salvo.\n");
}

static void criar_usuario(PostgresConnection *postgres) {
    Usuario usuario = {0};
    int ativo = 1;
    int bloqueado = 0;

    input_read_line("Nome: ", usuario.nome, sizeof(usuario.nome));
    input_read_line("CPF: ", usuario.cpf, sizeof(usuario.cpf));
    input_read_line("E-mail: ", usuario.email, sizeof(usuario.email));
    input_read_line("Telefone: ", usuario.telefone, sizeof(usuario.telefone));
    input_read_line("Senha hash: ", usuario.senha_hash, sizeof(usuario.senha_hash));
    input_read_int("ID do perfil: ", &usuario.perfil_id);
    input_read_int("Ativo? 1=sim 0=nao: ", &ativo);
    input_read_int("Bloqueado? 1=sim 0=nao: ", &bloqueado);
    usuario.ativo = ativo != 0;
    usuario.bloqueado = bloqueado != 0;

    print_result(cadastro_service_criar_usuario(postgres, &usuario));
}

static void criar_autor(PostgresConnection *postgres) {
    Autor autor = {0};
    input_read_line("Nome: ", autor.nome, sizeof(autor.nome));
    input_read_line("Nacionalidade: ", autor.nacionalidade, sizeof(autor.nacionalidade));
    print_result(cadastro_service_criar_autor(postgres, &autor));
}

static void criar_editora(PostgresConnection *postgres) {
    Editora editora = {0};
    input_read_line("Nome: ", editora.nome, sizeof(editora.nome));
    input_read_line("Cidade: ", editora.cidade, sizeof(editora.cidade));
    input_read_line("Pais: ", editora.pais, sizeof(editora.pais));
    print_result(cadastro_service_criar_editora(postgres, &editora));
}

static void criar_genero(PostgresConnection *postgres) {
    Genero genero = {0};
    input_read_line("Nome: ", genero.nome, sizeof(genero.nome));
    input_read_line("Descricao: ", genero.descricao, sizeof(genero.descricao));
    print_result(cadastro_service_criar_genero(postgres, &genero));
}
static void criar_livro(PostgresConnection *postgres) {
    Livro livro = {0};

    input_read_line("ISBN: ", livro.isbn, sizeof(livro.isbn));
    input_read_line("Titulo: ", livro.titulo, sizeof(livro.titulo));
    input_read_line("Subtitulo: ", livro.subtitulo, sizeof(livro.subtitulo));
    input_read_int("Ano de publicacao (0 para vazio): ", &livro.ano_publicacao);
    input_read_int("Edicao (0 para vazio): ", &livro.edicao);
    input_read_int("ID da editora (0 para vazio): ", &livro.editora_id);
    input_read_line("Idioma: ", livro.idioma, sizeof(livro.idioma));
    input_read_line("Descricao: ", livro.descricao, sizeof(livro.descricao));

    print_result(cadastro_service_criar_livro(postgres, &livro));
}

static void criar_exemplar(PostgresConnection *postgres) {
    Exemplar exemplar = {0};

    input_read_int("ID do livro: ", &exemplar.livro_id);
    input_read_line("Codigo de barras: ", exemplar.codigo_barras, sizeof(exemplar.codigo_barras));
    input_read_line("RFID: ", exemplar.rfid, sizeof(exemplar.rfid));
    exemplar.status = EXEMPLAR_DISPONIVEL;
    input_read_line("Localizacao: ", exemplar.localizacao, sizeof(exemplar.localizacao));

    print_result(cadastro_service_criar_exemplar(postgres, &exemplar));
}

static void print_menu(void) {
    printf("\n===================================\n");
    printf("CADASTROS ADMINISTRATIVOS\n");
    printf("===================================\n");
    printf("1  - Cadastrar usuario\n");
    printf("2  - Listar usuarios\n");
    printf("3  - Cadastrar autor\n");
    printf("4  - Listar autores\n");
    printf("5  - Cadastrar editora\n");
    printf("6  - Listar editoras\n");
    printf("7  - Cadastrar genero\n");
    printf("8  - Listar generos\n");
    printf("9  - Cadastrar livro\n");
    printf("10 - Listar livros\n");
    printf("11 - Cadastrar exemplar\n");
    printf("12 - Listar exemplares\n");
    printf("0  - Sair\n");
}

void cadastro_ui_run(PostgresConnection *postgres) {
    int option = -1;

    while (option != 0) {
        input_clear_screen();
        print_menu();
        if (!input_read_int("Opcao: ", &option)) {
            printf("[ERRO] Opcao invalida.\n");
            input_wait_enter();
            continue;
        }

        switch (option) {
            case 1:
                criar_usuario(postgres);
                break;
            case 2:
                cadastro_service_listar_usuarios(postgres);
                break;
            case 3:
                criar_autor(postgres);
                break;
            case 4:
                cadastro_service_listar_autores(postgres);
                break;
            case 5:
                criar_editora(postgres);
                break;
            case 6:
                cadastro_service_listar_editoras(postgres);
                break;
            case 7:
                criar_genero(postgres);
                break;
            case 8:
                cadastro_service_listar_generos(postgres);
                break;
            case 9:
                criar_livro(postgres);
                break;
            case 10:
                cadastro_service_listar_livros(postgres);
                break;
            case 11:
                criar_exemplar(postgres);
                break;
            case 12:
                cadastro_service_listar_exemplares(postgres);
                break;
            case 0:
                printf("[INFO] Saindo dos cadastros.\n");
                break;
            default:
                printf("[ERRO] Opcao invalida.\n");
                break;
        }

        if (option != 0) {
            input_wait_enter();
        }
    }
}


