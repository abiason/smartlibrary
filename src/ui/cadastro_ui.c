#include "cadastro_ui.h"

#include "models/autor.h"
#include "models/editora.h"
#include "models/exemplar.h"
#include "models/genero.h"
#include "models/livro.h"
#include "models/usuario.h"
#include "services/cadastro_service.h"
#include "ui/console_ui.h"
#include "utils/input.h"

#include <stdbool.h>
#include <stdio.h>

static void print_result(int ok) {
    if (ok) {
        ui_success("Operacao concluida.");
    } else {
        ui_error("Operacao nao concluida.");
    }
}

static StatusExemplar read_status_exemplar(void) {
    int status = 1;
    ui_header("CADASTROS", "Status do Exemplar");
    ui_menu_item(1, "DISPONIVEL");
    ui_menu_item(2, "EMPRESTADO");
    ui_menu_item(3, "RESERVADO");
    ui_menu_item(4, "MANUTENCAO");
    ui_menu_item(5, "EXTRAVIADO");
    putchar('\n');
    input_read_int("Status: ", &status);

    switch (status) {
        case 2: return EXEMPLAR_EMPRESTADO;
        case 3: return EXEMPLAR_RESERVADO;
        case 4: return EXEMPLAR_MANUTENCAO;
        case 5: return EXEMPLAR_EXTRAVIADO;
        default: return EXEMPLAR_DISPONIVEL;
    }
}

static int escolher_entidade(void) {
    int entity = 0;
    ui_header("CADASTROS", "Escolha da Entidade");
    ui_menu_item(1, "Usuario");
    ui_menu_item(2, "Autor");
    ui_menu_item(3, "Editora");
    ui_menu_item(4, "Genero");
    ui_menu_item(5, "Livro");
    ui_menu_item(6, "Exemplar");
    putchar('\n');
    input_read_int("Entidade: ", &entity);
    return entity;
}

static void criar_usuario(PostgresConnection *postgres) {
    Usuario usuario = {0};
    int ativo = 1;
    int bloqueado = 0;

    input_read_line("Nome: ", usuario.nome, sizeof(usuario.nome));
    input_read_line("CPF: ", usuario.cpf, sizeof(usuario.cpf));
    input_read_line("E-mail: ", usuario.email, sizeof(usuario.email));
    input_read_line("Telefone: ", usuario.telefone, sizeof(usuario.telefone));
    input_read_line("Senha: ", usuario.senha_hash, sizeof(usuario.senha_hash));
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

static void criar(PostgresConnection *postgres) {
    switch (escolher_entidade()) {
        case 1: criar_usuario(postgres); break;
        case 2: criar_autor(postgres); break;
        case 3: criar_editora(postgres); break;
        case 4: criar_genero(postgres); break;
        case 5: criar_livro(postgres); break;
        case 6: criar_exemplar(postgres); break;
        default: printf("[ERRO] Entidade invalida.\n"); break;
    }
}

static void listar(PostgresConnection *postgres) {
    switch (escolher_entidade()) {
        case 1: cadastro_service_listar_usuarios(postgres); break;
        case 2: cadastro_service_listar_autores(postgres); break;
        case 3: cadastro_service_listar_editoras(postgres); break;
        case 4: cadastro_service_listar_generos(postgres); break;
        case 5: cadastro_service_listar_livros(postgres); break;
        case 6: cadastro_service_listar_exemplares(postgres); break;
        default: printf("[ERRO] Entidade invalida.\n"); break;
    }
}

static void buscar(PostgresConnection *postgres) {
    char termo[201];
    int entity = escolher_entidade();
    input_read_line("Termo de busca: ", termo, sizeof(termo));

    switch (entity) {
        case 1: cadastro_service_buscar_usuarios(postgres, termo); break;
        case 2: cadastro_service_buscar_autores(postgres, termo); break;
        case 3: cadastro_service_buscar_editoras(postgres, termo); break;
        case 4: cadastro_service_buscar_generos(postgres, termo); break;
        case 5: cadastro_service_buscar_livros(postgres, termo); break;
        case 6: cadastro_service_buscar_exemplares(postgres, termo); break;
        default: printf("[ERRO] Entidade invalida.\n"); break;
    }
}

static void alterar_usuario(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    Usuario usuario = {0};
    int ativo = 1;
    int bloqueado = 0;
    input_read_int("ID do usuario: ", &usuario.id);
    input_read_line("Novo nome: ", usuario.nome, sizeof(usuario.nome));
    input_read_line("Novo e-mail: ", usuario.email, sizeof(usuario.email));
    input_read_line("Novo telefone: ", usuario.telefone, sizeof(usuario.telefone));
    input_read_int("ID do perfil: ", &usuario.perfil_id);
    input_read_int("Ativo? 1=sim 0=nao: ", &ativo);
    input_read_int("Bloqueado? 1=sim 0=nao: ", &bloqueado);
    usuario.ativo = ativo != 0;
    usuario.bloqueado = bloqueado != 0;
    cadastro_service_atualizar_usuario(postgres, mongo, &usuario, operador_id);
}

static void alterar_autor(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    Autor autor = {0};
    input_read_int("ID do autor: ", &autor.id);
    input_read_line("Novo nome: ", autor.nome, sizeof(autor.nome));
    input_read_line("Nova nacionalidade: ", autor.nacionalidade, sizeof(autor.nacionalidade));
    cadastro_service_atualizar_autor(postgres, mongo, &autor, operador_id);
}

static void alterar_editora(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    Editora editora = {0};
    input_read_int("ID da editora: ", &editora.id);
    input_read_line("Novo nome: ", editora.nome, sizeof(editora.nome));
    input_read_line("Nova cidade: ", editora.cidade, sizeof(editora.cidade));
    input_read_line("Novo pais: ", editora.pais, sizeof(editora.pais));
    cadastro_service_atualizar_editora(postgres, mongo, &editora, operador_id);
}

static void alterar_genero(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    Genero genero = {0};
    input_read_int("ID do genero: ", &genero.id);
    input_read_line("Novo nome: ", genero.nome, sizeof(genero.nome));
    input_read_line("Nova descricao: ", genero.descricao, sizeof(genero.descricao));
    cadastro_service_atualizar_genero(postgres, mongo, &genero, operador_id);
}

static void alterar_livro(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    Livro livro = {0};
    input_read_int("ID do livro: ", &livro.id);
    input_read_line("Novo ISBN: ", livro.isbn, sizeof(livro.isbn));
    input_read_line("Novo titulo: ", livro.titulo, sizeof(livro.titulo));
    input_read_line("Novo subtitulo: ", livro.subtitulo, sizeof(livro.subtitulo));
    input_read_int("Ano de publicacao (0 para vazio): ", &livro.ano_publicacao);
    input_read_int("Edicao (0 para vazio): ", &livro.edicao);
    input_read_int("ID da editora (0 para vazio): ", &livro.editora_id);
    input_read_line("Idioma: ", livro.idioma, sizeof(livro.idioma));
    input_read_line("Descricao: ", livro.descricao, sizeof(livro.descricao));
    cadastro_service_atualizar_livro(postgres, mongo, &livro, operador_id);
}

static void alterar_exemplar(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    Exemplar exemplar = {0};
    input_read_int("ID do exemplar: ", &exemplar.id);
    input_read_line("Novo codigo de barras: ", exemplar.codigo_barras, sizeof(exemplar.codigo_barras));
    input_read_line("Novo RFID: ", exemplar.rfid, sizeof(exemplar.rfid));
    exemplar.status = read_status_exemplar();
    input_read_line("Nova localizacao: ", exemplar.localizacao, sizeof(exemplar.localizacao));
    cadastro_service_atualizar_exemplar(postgres, mongo, &exemplar, operador_id);
}

static void alterar(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    switch (escolher_entidade()) {
        case 1: alterar_usuario(postgres, mongo, operador_id); break;
        case 2: alterar_autor(postgres, mongo, operador_id); break;
        case 3: alterar_editora(postgres, mongo, operador_id); break;
        case 4: alterar_genero(postgres, mongo, operador_id); break;
        case 5: alterar_livro(postgres, mongo, operador_id); break;
        case 6: alterar_exemplar(postgres, mongo, operador_id); break;
        default: printf("[ERRO] Entidade invalida.\n"); break;
    }
}

static void excluir_ou_desativar(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    int id = 0;
    int entity = escolher_entidade();
    input_read_int("ID para excluir/desativar: ", &id);

    switch (entity) {
        case 1: cadastro_service_remover_ou_desativar_usuario(postgres, mongo, id, operador_id); break;
        case 2: cadastro_service_excluir_autor(postgres, mongo, id, operador_id); break;
        case 3: cadastro_service_excluir_editora(postgres, mongo, id, operador_id); break;
        case 4: cadastro_service_excluir_genero(postgres, mongo, id, operador_id); break;
        case 5: cadastro_service_excluir_livro(postgres, mongo, id, operador_id); break;
        case 6: cadastro_service_remover_ou_inativar_exemplar(postgres, mongo, id, operador_id); break;
        default: printf("[ERRO] Entidade invalida.\n"); break;
    }
}

static void gerenciar_vinculos_livro(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    int option = 0;
    int livro_id = 0;
    int related_id = 0;

    printf("\n1 - Vincular autor ao livro\n");
    printf("2 - Desvincular autor do livro\n");
    printf("3 - Vincular genero ao livro\n");
    printf("4 - Desvincular genero do livro\n");
    input_read_int("Opcao: ", &option);
    input_read_int("ID do livro: ", &livro_id);

    switch (option) {
        case 1:
            input_read_int("ID do autor: ", &related_id);
            cadastro_service_vincular_livro_autor(postgres, mongo, livro_id, related_id, operador_id);
            break;
        case 2:
            input_read_int("ID do autor: ", &related_id);
            cadastro_service_desvincular_livro_autor(postgres, mongo, livro_id, related_id, operador_id);
            break;
        case 3:
            input_read_int("ID do genero: ", &related_id);
            cadastro_service_vincular_livro_genero(postgres, mongo, livro_id, related_id, operador_id);
            break;
        case 4:
            input_read_int("ID do genero: ", &related_id);
            cadastro_service_desvincular_livro_genero(postgres, mongo, livro_id, related_id, operador_id);
            break;
        default:
            ui_error("Opcao invalida.");
            break;
    }
}

static void print_menu(void) {
    ui_header("CADASTROS ADMINISTRATIVOS", "Menu");
    ui_menu_item(1, "Criar cadastro");
    ui_menu_item(2, "Listar cadastros");
    ui_menu_item(3, "Buscar cadastro");
    ui_menu_item(4, "Alterar cadastro");
    ui_menu_item(5, "Excluir ou desativar cadastro");
    ui_menu_item(6, "Gerenciar vinculos de livro");
    ui_menu_back("Voltar");
    putchar('\n');
}

void cadastro_ui_run(PostgresConnection *postgres, MongoConnection *mongo, int operador_id) {
    int option = -1;

    while (option != 0) {
        ui_clear();
        print_menu();
        if (!input_read_int("Escolha uma opcao: ", &option)) {
            ui_error("Opcao invalida.");
            input_wait_enter();
            continue;
        }

        switch (option) {
            case 1:
                criar(postgres);
                break;
            case 2:
                listar(postgres);
                break;
            case 3:
                buscar(postgres);
                break;
            case 4:
                alterar(postgres, mongo, operador_id);
                break;
            case 5:
                excluir_ou_desativar(postgres, mongo, operador_id);
                break;
            case 6:
                gerenciar_vinculos_livro(postgres, mongo, operador_id);
                break;
            case 0:
                ui_info("Voltando ao menu principal.");
                break;
            default:
                ui_error("Opcao invalida.");
                break;
        }

        if (option != 0) {
            input_wait_enter();
        }
    }
}
