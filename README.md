# SmartLibrary

Sistema academico de biblioteca com Self Checkout, desenvolvido em C17 com PostgreSQL e MongoDB.

## Escopo Atual

Esta entrega implementa as Fases 1 a 9 da especificacao:

- estrutura inicial do projeto;
- configuracao por variaveis de ambiente;
- conexao PostgreSQL com libpq;
- conexao MongoDB com MongoDB C Driver;
- build com Makefile;
- documentacao em Markdown e Mermaid;
- schema relacional PostgreSQL, constraints, indexes e seed inicial;
- modelo documental MongoDB com indexes e validators;
- structs C das entidades principais;
- dicionario de dados e diagramas C4/MER;
- menu administrativo de cadastros;
- criacao e listagem de usuarios, autores, editoras, generos, livros e exemplares;
- emprestimos, devolucoes e renovacoes transacionais;
- bloqueio de exemplar com `SELECT ... FOR UPDATE`;
- reservas com criacao, cancelamento, atendimento, expiracao e listagem;
- Self Checkout com identificacao por CPF, emprestimo, devolucao, renovacao, consultas e pesquisa;
- eventos MongoDB por origem apos operacoes confirmadas;
- logs operacionais, auditoria e consultas NoSQL pela aplicacao;
- relatorios operacionais de acervo, atrasos, ranking de livros, origens de emprestimo, reservas e pendencias;
- testes automatizados integrados com banco isolado para PostgreSQL e MongoDB.

Documentacao final pertence a fase futura.

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

## Testes

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

O alvo `test` recria os bancos `smartlibrary_test` no PostgreSQL e MongoDB, executa fixtures controladas e valida fluxo, relatorios e documentos NoSQL.

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
|   |-- database/
|   |-- repositories/
|   |   |-- cadastro_repository.c / cadastro_repository.h
|   |   |-- emprestimo_repository.c / emprestimo_repository.h
|   |   |-- reserva_repository.c / reserva_repository.h
|   |   |-- self_checkout_repository.c / self_checkout_repository.h
|   |   `-- relatorio_repository.c / relatorio_repository.h
|   |-- services/
|   |   |-- cadastro_service.c / cadastro_service.h
|   |   |-- emprestimo_service.c / emprestimo_service.h
|   |   |-- reserva_service.c / reserva_service.h
|   |   |-- self_checkout_service.c / self_checkout_service.h
|   |   `-- relatorio_service.c / relatorio_service.h
|   |-- events/
|   |   `-- event_service.c / event_service.h
|   |-- ui/
|   |   |-- main_ui.c / main_ui.h
|   |   |-- cadastro_ui.c / cadastro_ui.h
|   |   |-- emprestimo_ui.c / emprestimo_ui.h
|   |   |-- reserva_ui.c / reserva_ui.h
|   |   |-- self_checkout_ui.c / self_checkout_ui.h
|   |   |-- nosql_ui.c / nosql_ui.h
|   |   `-- relatorio_ui.c / relatorio_ui.h
|   |-- utils/
|   `-- models/
|-- database/
|   |-- postgresql/
|   `-- mongodb/
|-- tests/
|   `-- run_phase9_tests.ps1
|-- docs/
|   |-- arquitetura.md
|   |-- modelo_relacional.md
|   |-- modelo_nosql.md
|   |-- dicionario_dados.md
|   |-- cadastros.md
|   |-- emprestimos.md
|   |-- reservas.md
|   |-- self_checkout.md
|   |-- relatorios.md
|   |-- testes.md
|   `-- diagrams/
|-- .env.example
|-- .gitignore
|-- Makefile
`-- README.md
```
