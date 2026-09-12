# SmartLibrary

Sistema academico de biblioteca com Self Checkout, desenvolvido em C17 com PostgreSQL e MongoDB.

## Visao Geral

O SmartLibrary organiza cadastros de biblioteca, circulacao de exemplares, reservas, autoatendimento, trilha NoSQL, relatorios e testes automatizados. O PostgreSQL e usado como fonte transacional principal e o MongoDB registra eventos, logs e auditoria.

## Escopo Entregue

Esta entrega implementa as Fases 1 a 10 da especificacao, mais as melhorias V2 de cadastros, auditoria profissional, seguranca por perfis, regras de negocio reforcadas e pacote final de avaliacao:

- fundacao do projeto em C17;
- configuracao por variaveis de ambiente;
- conexoes PostgreSQL/libpq e MongoDB/MongoDB C Driver;
- schema relacional, constraints, indexes e seed inicial;
- modelo documental MongoDB com validators e indexes;
- structs das entidades principais;
- menus administrativos para usuarios, autores, editoras, generos, livros e exemplares;
- busca, alteracao, exclusao e desativacao segura de cadastros administrativos;
- auditoria antes/depois real em documentos MongoDB para manutencao de cadastros, registrando o operador autenticado nas alteracoes administrativas;
- login com senha armazenada por hash BCrypt via PostgreSQL pgcrypto;
- troca de senha pelo usuario autenticado, com validacao da senha atual;
- permissoes por perfil para administrador, bibliotecario e usuario;
- validacao de CPF, e-mail, dados editoriais e bloqueios operacionais;
- associacao de livros com autores e generos pela aplicacao;
- emprestimos, devolucoes e renovacoes transacionais;
- reservas com criacao, cancelamento, atendimento, expiracao e listagem;
- Self Checkout com identificacao por CPF, operacoes de circulacao, consultas e pesquisa;
- eventos MongoDB por origem, logs operacionais e auditoria;
- consultas NoSQL pela aplicacao;
- relatorios operacionais;
- testes automatizados integrados;
- documentacao final de arquitetura, execucao, dados, fluxos e fases;
- roteiro de apresentacao, checklist de avaliacao e matriz de evidencias de teste.

## Dependencias

- GCC com suporte a C17.
- pkg-config.
- PostgreSQL com libpq.
- MongoDB Server.
- MongoDB Shell (`mongosh`).
- MongoDB C Driver.
- GNU Make ou `mingw32-make` no Windows/MSYS2.

No ambiente validado, as bibliotecas sao resolvidas por:

```sh
pkg-config --cflags --libs libpq
pkg-config --cflags --libs mongoc2
```

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

Nao versione senha real no repositorio. Defina `POSTGRES_PASSWORD` apenas no ambiente da sessao.

## Banco De Dados

Crie o banco PostgreSQL `smartlibrary` e aplique os scripts nesta ordem:

```powershell
$env:PGPASSWORD='sua_senha'
psql -U postgres -d smartlibrary -f database/postgresql/001_schema.sql
psql -U postgres -d smartlibrary -f database/postgresql/002_constraints.sql
psql -U postgres -d smartlibrary -f database/postgresql/003_indexes.sql
psql -U postgres -d smartlibrary -f database/postgresql/004_seed.sql
```

Aplique os scripts MongoDB:

```powershell
mongosh database/mongodb/validators.js
mongosh database/mongodb/indexes.js
```

## Build

No Windows/MSYS2 validado:

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

O alvo `test` recria bancos isolados chamados `smartlibrary_test` no PostgreSQL e no MongoDB, aplica fixtures controladas e valida build, login/perfis, regras Fase 14, cadastros V2, auditoria antes/depois com operador autenticado, troca de senha, recuperacao sem administrador ativo, fluxo de emprestimo/devolucao, relatorios e documentos NoSQL. A matriz detalhada esta em `docs/evidencias_testes.md`.

## Execucao

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
.\build\smartlibrary.exe
```

Ou, depois de configurar a senha no ambiente:

```sh
mingw32-make run
```

## Menus Principais

- `1 - Cadastros administrativos`
- `2 - Circulacao`
- `3 - Reservas`
- `4 - Self Checkout`
- `5 - NoSQL: eventos, logs e auditoria`
- `6 - Relatorios`

## Documentacao

- [Arquitetura](docs/arquitetura.md)
- [Guia de execucao](docs/guia_execucao.md)
- [Entrega final](docs/entrega_final.md)
- [Especificacao V4 - evolucao planejada](docs/especificacao_v4.md)
- [Especificacao V3](docs/especificacao_v3.md)
- [Especificacao V2](docs/especificacao_v2.md)
- [Resumo das fases](docs/fases.md)
- [Modelo relacional](docs/modelo_relacional.md)
- [Modelo NoSQL](docs/modelo_nosql.md)
- [Dicionario de dados](docs/dicionario_dados.md)
- [Cadastros](docs/cadastros.md)
- [Emprestimos](docs/emprestimos.md)
- [Reservas](docs/reservas.md)
- [Self Checkout](docs/self_checkout.md)
- [Relatorios](docs/relatorios.md)
- [Testes](docs/testes.md)
- [Evidencias de testes](docs/evidencias_testes.md)
- [Roteiro de apresentacao](docs/roteiro_apresentacao.md)
- [Checklist de avaliacao](docs/checklist_avaliacao.md)

## Estrutura

```text
smartlibrary/
|-- src/
|   |-- main.c
|   |-- auth/
|   |-- config/
|   |-- database/
|   |-- repositories/
|   |-- services/
|   |-- events/
|   |-- ui/
|   |-- utils/
|   `-- models/
|-- database/
|   |-- postgresql/
|   `-- mongodb/
|-- tests/
|   `-- run_phase9_tests.ps1
|-- docs/
|   |-- arquitetura.md
|   |-- guia_execucao.md
|   |-- entrega_final.md
|   |-- especificacao_v4.md
|   |-- especificacao_v3.md
|   |-- especificacao_v2.md
|   |-- modelo_relacional.md
|   |-- modelo_nosql.md
|   |-- dicionario_dados.md
|   |-- cadastros.md
|   |-- emprestimos.md
|   |-- reservas.md
|   |-- self_checkout.md
|   |-- relatorios.md
|   |-- testes.md
|   |-- evidencias_testes.md
|   |-- roteiro_apresentacao.md
|   |-- checklist_avaliacao.md
|   `-- diagrams/
|-- .env.example
|-- .gitignore
|-- Makefile
`-- README.md
```
