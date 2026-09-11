# Roteiro De Apresentacao

Este roteiro foi preparado para demonstrar o SmartLibrary em uma avaliacao academica, cobrindo os requisitos principais e as melhorias implementadas nas fases finais.

## 1. Abertura

Apresente o objetivo do sistema: uma biblioteca academica em C17 com PostgreSQL como banco transacional e MongoDB para eventos, logs e auditoria.

Destaques iniciais:

- aplicacao de terminal compilada com Makefile;
- arquitetura em camadas;
- banco relacional com constraints e indexes;
- banco documental para observabilidade;
- testes integrados automatizados.

## 2. Preparacao Do Ambiente

Mostre os arquivos principais:

- `README.md`;
- `.env.example`;
- `database/postgresql/001_schema.sql`;
- `database/mongodb/validators.js`;
- `tests/run_phase9_tests.ps1`.

Comando de validacao recomendado:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

Explique que o teste recria bancos isolados `smartlibrary_test` no PostgreSQL e MongoDB.

## 3. Login E Perfis

Demonstre que a aplicacao exige login antes do menu principal.

Explique os perfis:

- `ADMINISTRADOR`: acesso completo;
- `BIBLIOTECARIO`: circulacao, reservas e relatorios;
- `USUARIO`: self checkout.

Mostre que o primeiro acesso cria um administrador apenas quando nao existe nenhum usuario cadastrado. A senha fica armazenada como hash BCrypt usando `pgcrypto`.

## 4. Cadastros Administrativos

Demonstre o menu de cadastros:

- criar;
- listar;
- buscar;
- alterar;
- excluir ou desativar;
- gerenciar vinculos de livro.

Pontos importantes para falar:

- CPF e e-mail possuem validacao;
- livros podem ser associados a autores e generos;
- busca de livros encontra resultados por titulo, ISBN, editora, autor ou genero;
- exclusoes respeitam vinculos e historico.

## 5. Circulacao

Demonstre um emprestimo e uma devolucao.

Explique as regras:

- usuario precisa estar ativo e desbloqueado;
- exemplar precisa estar disponivel;
- limite de exemplares e renovacoes vem da tabela `configuracao`;
- operacoes criticas usam transacao.

## 6. Reservas

Demonstre criacao, listagem, cancelamento ou atendimento de reserva.

Pontos para destacar:

- reserva tem status controlado;
- circulacao respeita reserva de outro usuario;
- reservas expiradas podem ser tratadas pelo fluxo da aplicacao.

## 7. Self Checkout

Demonstre identificacao por CPF e operacoes por codigo de barras ou RFID.

Explique que os eventos gerados nesse fluxo indicam origem `SELF_CHECKOUT`, permitindo relatorios e auditoria por canal.

## 8. NoSQL

Mostre o menu NoSQL:

- eventos recentes;
- logs recentes;
- auditoria recente;
- resumo por tipo.

Destaque que a auditoria de cadastros grava snapshots reais `antes` e `depois` como documentos MongoDB.

## 9. Relatorios

Demonstre relatorios operacionais:

- acervo;
- atrasos;
- ranking de livros;
- origens de emprestimo;
- reservas ativas;
- usuarios com pendencias.

## 10. Fechamento

Finalize mostrando o teste integrado passando e o historico de commits no GitHub.

Mensagem final sugerida:

O SmartLibrary atende ao escopo original e inclui evolucoes de qualidade: CRUD completo, auditoria profissional, login/perfis, regras reforcadas, testes integrados e documentacao de apoio para avaliacao.