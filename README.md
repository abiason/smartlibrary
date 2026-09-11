# SmartLibrary

Sistema academico de biblioteca com Self Checkout, desenvolvido em C17 com PostgreSQL e MongoDB.

## Escopo Atual

Esta entrega implementa somente a Fase 1 da especificacao:

- estrutura inicial do projeto;
- configuracao por variaveis de ambiente;
- conexao PostgreSQL com libpq;
- conexao MongoDB com MongoDB C Driver;
- build com Makefile;
- documentacao inicial em Markdown e Mermaid.

Cadastros, emprestimos, reservas, Self Checkout completo e relatorios pertencem a fases futuras.

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
|   `-- database/
|       |-- postgres.c
|       |-- postgres.h
|       |-- mongodb.c
|       `-- mongodb.h
|-- docs/
|   |-- arquitetura.md
|   `-- diagrams/
|       |-- c4_contexto.md
|       |-- c4_containers.md
|       `-- arquitetura_geral.md
|-- .env.example
|-- .gitignore
|-- Makefile
`-- README.md
```
