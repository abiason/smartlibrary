# SmartLibrary

Sistema academico de biblioteca com Self Checkout, desenvolvido em C17 com PostgreSQL e MongoDB.

## Escopo Atual

Esta entrega implementa as Fases 1, 2 e 3 da especificacao:

- estrutura inicial do projeto;
- configuracao por variaveis de ambiente;
- conexao PostgreSQL com libpq;
- conexao MongoDB com MongoDB C Driver;
- build com Makefile;
- documentacao inicial em Markdown e Mermaid;
- schema relacional PostgreSQL;
- constraints, indexes e seed inicial;
- modelo documental MongoDB com indexes e validators;
- structs C das entidades principais;
- dicionario de dados e diagramas C4 Componentes/MER;
- menu administrativo de cadastros;
- repositories com SQL parametrizado para cadastros;
- services com validacoes basicas;
- criacao e listagem de usuarios, autores, editoras, generos, livros e exemplares.

Emprestimos, reservas, Self Checkout completo e relatorios pertencem a fases futuras.

## Dependencias

- GCC com suporte a C17.
- pkg-config.
- PostgreSQL libpq.
- MongoDB C Driver.
- GNU Make.

No ambiente validado, os pacotes detectados foram:

```sh
pkg-config --cflags --libs libpq
pkg-config --cflags --libs mongoc2
```

No MSYS2 UCRT64 desta maquina, `mingw32-make` esta disponivel no lugar de `make`.

## Configuracao

Use as variaveis descritas em `.env.example`:

```sh
POSTGRES_HOST=localhost
POSTGRES_PORT=5432
POSTGRES_DB=smartlibrary
POSTGRES_USER=postgres
POSTGRES_PASSWORD=
MONGODB_URI=mongodb://localhost:27017
MONGODB_DATABASE=smartlibrary
```

## Build

```sh
mingw32-make
```

Em ambientes onde `make` esteja disponivel:

```sh
make
```

## Execucao

```sh
build/smartlibrary.exe
```

ou:

```sh
mingw32-make run
```

## Estrutura

```text
smartlibrary/
|-- src/
|   |-- main.c
|   |-- config/
|   |   |-- config.c
|   |   `-- config.h
|   |-- database/
|   |   |-- postgres.c
|   |   |-- postgres.h
|   |   |-- mongodb.c
|   |   `-- mongodb.h
|   |-- repositories/
|   |   `-- cadastro_repository.c / cadastro_repository.h
|   |-- services/
|   |   `-- cadastro_service.c / cadastro_service.h
|   |-- ui/
|   |   `-- cadastro_ui.c / cadastro_ui.h
|   |-- utils/
|   |   `-- input.c / input.h
|   `-- models/
|       |-- usuario.c / usuario.h
|       |-- autor.c / autor.h
|       |-- editora.c / editora.h
|       |-- genero.c / genero.h
|       |-- livro.c / livro.h
|       |-- exemplar.c / exemplar.h
|       |-- emprestimo.c / emprestimo.h
|       `-- reserva.c / reserva.h
|-- database/
|   |-- postgresql/
|   |   |-- 001_schema.sql
|   |   |-- 002_constraints.sql
|   |   |-- 003_indexes.sql
|   |   `-- 004_seed.sql
|   `-- mongodb/
|       |-- indexes.js
|       `-- validators.js
|-- docs/
|   |-- arquitetura.md
|   |-- modelo_relacional.md
|   |-- modelo_nosql.md
|   |-- dicionario_dados.md
|   |-- cadastros.md
|   `-- diagrams/
|       |-- c4_contexto.md
|       |-- c4_containers.md
|       |-- c4_componentes.md
|       |-- modelo_er.md
|       `-- arquitetura_geral.md
|-- .env.example
|-- .gitignore
|-- Makefile
`-- README.md
```
