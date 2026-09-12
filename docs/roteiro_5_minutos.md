# Roteiro De Demonstracao Em 5 Minutos

Este roteiro foi feito para uma apresentacao curta do SmartLibrary, priorizando os pontos que mais demonstram maturidade tecnica: setup automatizado, login/perfis, CRUD completo, auditoria MongoDB, regras de negocio e testes integrados.

## Antes De Comecar

Deixe o ambiente preparado:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
PowerShell -ExecutionPolicy Bypass -File scripts/setup_local.ps1
```

Para demonstrar validacao automatizada:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

Para abrir o sistema:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
.\build\smartlibrary.exe
```

## 0:00 A 0:40 - Abertura

Fale em uma frase:

O SmartLibrary e uma aplicacao academica em C17 para gestao de biblioteca, com PostgreSQL como fonte transacional, MongoDB para eventos/logs/auditoria, login com BCrypt, controle de perfis e testes integrados.

Mostre rapidamente:

- `README.md`;
- `scripts/setup_local.ps1`;
- `database/postgresql/`;
- `database/mongodb/`;
- `tests/run_phase9_tests.ps1`.

## 0:40 A 1:20 - Setup E Testes

Mostre o comando de setup automatizado e explique:

- valida dependencias locais;
- cria o banco principal quando necessario;
- aplica schema PostgreSQL;
- aplica validators e indexes MongoDB;
- compila o projeto;
- nao grava senha real em arquivo.

Depois mostre `mingw32-make test` passando. Destaque que a suite usa bancos isolados `smartlibrary_test`, entao nao interfere no banco principal.

## 1:20 A 2:00 - Login, Perfis E Senha

Abra a aplicacao e faca login com um usuario administrador.

Destaque:

- o sistema nao abre o menu sem autenticar;
- senha e validada com hash BCrypt via `pgcrypto`;
- `ADMINISTRADOR`, `BIBLIOTECARIO` e `USUARIO` possuem permissoes diferentes;
- existe recuperacao quando nao ha administrador ativo;
- a opcao `7 - Trocar senha` valida senha atual e confirmacao.

## 2:00 A 3:00 - Cadastros E Regras

Entre em `1 - Cadastros administrativos`.

Demonstre ou explique rapidamente:

- criar/listar/buscar/alterar/excluir ou desativar cadastro;
- validacao de CPF e e-mail;
- associacao de livro com autor e genero;
- bloqueio de exclusao quando ha vinculos ou historico;
- protecao contra alterar exemplar emprestado manualmente.

Ponto forte para falar:

As operacoes administrativas relevantes geram auditoria com snapshot `antes` e `depois`, incluindo o `usuarioId` do operador autenticado.

## 3:00 A 3:50 - Circulacao, Reservas E Self Checkout

Mostre os menus principais:

- `2 - Circulacao` para emprestimo, devolucao e renovacao;
- `3 - Reservas` para criar, cancelar, atender e expirar reservas;
- `4 - Self Checkout` para atendimento por CPF, codigo de barras ou RFID.

Destaque:

- emprestimos usam transacao;
- regras de bloqueio protegem usuario inativo, atraso, limite e reserva de outro usuario;
- eventos registram origem `BALCAO` ou `SELF_CHECKOUT`.

## 3:50 A 4:35 - MongoDB E Relatorios

Entre em `5 - NoSQL: eventos, logs e auditoria`.

Mostre:

- eventos recentes;
- logs recentes;
- auditoria recente;
- resumo por tipo.

Depois entre em `6 - Relatorios` e cite:

- acervo;
- atrasos;
- ranking de livros;
- origens de emprestimo;
- reservas ativas;
- usuarios com pendencias.

## 4:35 A 5:00 - Fechamento

Feche com esta mensagem:

O projeto entrega mais que o CRUD basico: tem arquitetura em camadas, PostgreSQL, MongoDB, autenticacao com BCrypt, permissoes, regras de negocio, auditoria rastreavel, setup automatizado, testes integrados e documentacao de avaliacao.

Mostre o GitHub ou o ultimo commit para reforcar que a entrega esta versionada.

## Checklist Relampago

- Setup automatizado mostrado.
- Login demonstrado.
- Perfil/permissao explicado.
- CRUD administrativo citado ou demonstrado.
- Auditoria `antes`/`depois` com operador real destacada.
- Circulacao/reservas/self checkout citados.
- NoSQL e relatorios mostrados.
- `mingw32-make test` passando exibido.