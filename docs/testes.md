# Testes

A Fase 9 adiciona uma rotina automatizada de validacao integrada para o SmartLibrary.

## Estrategia

Os testes usam uma base isolada para evitar dependencia dos dados criados manualmente durante o desenvolvimento:

- PostgreSQL: `smartlibrary_test`;
- MongoDB: `smartlibrary_test`.

A rotina executa:

- build da aplicacao pelo Makefile;
- recriacao do banco relacional de teste;
- aplicacao dos scripts `001_schema.sql`, `002_constraints.sql`, `003_indexes.sql` e `004_seed.sql`;
- insercao de fixtures controladas;
- execucao da aplicacao com entradas simuladas;
- validacao de emprestimo e devolucao;
- validacao de busca, alteracao e exclusao/desativacao de cadastros;
- validacao dos relatorios pela UI;
- validacao de eventos, logs e auditoria no MongoDB.

## Como Executar

Defina a senha do PostgreSQL somente na sessao do terminal e rode o alvo de teste:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

O script nao grava senha em arquivo e nao altera o banco principal `smartlibrary`.

## Arquivos

- `tests/run_phase9_tests.ps1`: orquestra ambiente, fixtures e asserts.
- `Makefile`: adiciona o alvo `test`.

## Cobertura Atual

A validacao automatizada cobre:

- compilacao completa com flags C17;
- aplicacao do schema PostgreSQL do zero;
- fluxo de emprestimo e devolucao pelo balcao;
- busca de usuario por CPF;
- alteracao e desativacao segura de usuario com historico;
- bloqueio de exclusao de livro vinculado;
- persistencia de origem `BALCAO`;
- retorno do exemplar para `DISPONIVEL` apos devolucao;
- consultas dos menus de relatorios e NoSQL;
- eventos MongoDB;
- auditoria MongoDB de circulacao e cadastros;
- logs MongoDB.

Testes unitarios granulares em C podem ser adicionados em fase posterior se o projeto ganhar uma biblioteca de testes dedicada.
