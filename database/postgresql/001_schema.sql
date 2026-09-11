CREATE TABLE perfil (
    id_perfil SERIAL PRIMARY KEY,
    nome VARCHAR(50) NOT NULL,
    descricao VARCHAR(200)
);

CREATE TABLE usuario (
    id_usuario SERIAL PRIMARY KEY,
    nome VARCHAR(150) NOT NULL,
    cpf VARCHAR(14) NOT NULL,
    email VARCHAR(150),
    telefone VARCHAR(20),
    senha_hash VARCHAR(255) NOT NULL,
    id_perfil INTEGER NOT NULL,
    ativo BOOLEAN NOT NULL DEFAULT TRUE,
    bloqueado BOOLEAN NOT NULL DEFAULT FALSE,
    data_cadastro TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE autor (
    id_autor SERIAL PRIMARY KEY,
    nome VARCHAR(150) NOT NULL,
    nacionalidade VARCHAR(100)
);

CREATE TABLE editora (
    id_editora SERIAL PRIMARY KEY,
    nome VARCHAR(150) NOT NULL,
    cidade VARCHAR(100),
    pais VARCHAR(100)
);

CREATE TABLE genero (
    id_genero SERIAL PRIMARY KEY,
    nome VARCHAR(100) NOT NULL,
    descricao VARCHAR(255)
);

CREATE TABLE livro (
    id_livro SERIAL PRIMARY KEY,
    isbn VARCHAR(20),
    titulo VARCHAR(200) NOT NULL,
    subtitulo VARCHAR(200),
    ano_publicacao INTEGER,
    edicao INTEGER,
    id_editora INTEGER,
    idioma VARCHAR(50),
    descricao TEXT
);

CREATE TABLE livro_autor (
    id_livro INTEGER NOT NULL,
    id_autor INTEGER NOT NULL,
    PRIMARY KEY (id_livro, id_autor)
);

CREATE TABLE livro_genero (
    id_livro INTEGER NOT NULL,
    id_genero INTEGER NOT NULL,
    PRIMARY KEY (id_livro, id_genero)
);

CREATE TABLE exemplar (
    id_exemplar SERIAL PRIMARY KEY,
    id_livro INTEGER NOT NULL,
    codigo_barras VARCHAR(50) NOT NULL,
    rfid VARCHAR(100),
    status VARCHAR(30) NOT NULL DEFAULT 'DISPONIVEL',
    localizacao VARCHAR(100),
    data_aquisicao DATE
);

CREATE TABLE emprestimo (
    id_emprestimo SERIAL PRIMARY KEY,
    id_usuario INTEGER NOT NULL,
    data_emprestimo TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    data_prevista_devolucao DATE NOT NULL,
    data_encerramento TIMESTAMP,
    status VARCHAR(30) NOT NULL DEFAULT 'ABERTO',
    origem VARCHAR(30) NOT NULL
);

CREATE TABLE emprestimo_item (
    id_emprestimo_item SERIAL PRIMARY KEY,
    id_emprestimo INTEGER NOT NULL,
    id_exemplar INTEGER NOT NULL,
    data_devolucao TIMESTAMP,
    quantidade_renovacoes INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE reserva (
    id_reserva SERIAL PRIMARY KEY,
    id_usuario INTEGER NOT NULL,
    id_livro INTEGER NOT NULL,
    data_reserva TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    data_expiracao TIMESTAMP,
    status VARCHAR(30) NOT NULL DEFAULT 'ATIVA'
);

CREATE TABLE terminal_self_checkout (
    id_terminal SERIAL PRIMARY KEY,
    codigo VARCHAR(50) NOT NULL,
    descricao VARCHAR(150),
    localizacao VARCHAR(100),
    ativo BOOLEAN NOT NULL DEFAULT TRUE,
    ultima_comunicacao TIMESTAMP
);

CREATE TABLE configuracao (
    id_configuracao SERIAL PRIMARY KEY,
    chave VARCHAR(100) NOT NULL,
    valor VARCHAR(255) NOT NULL,
    descricao VARCHAR(255)
);
