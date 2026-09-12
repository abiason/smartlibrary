# Entrega Final

Este documento consolida a entrega completa do SmartLibrary conforme o plano incremental expandido para 20 fases, incluindo a V5 como ultima melhoria planejada.

## Objetivo

Desenvolver um sistema academico de biblioteca em C17 com persistencia relacional em PostgreSQL, persistencia documental em MongoDB, interface em terminal polida, Self Checkout, relatorios e testes automatizados.

## Fases Entregues

| Fase | Entrega | Status |
| --- | --- | --- |
| 1 | Fundacao do projeto, configuracao, conexoes e estrutura inicial | Concluida |
| 2 | Modelagem relacional/documental, scripts SQL/MongoDB, structs e diagramas | Concluida |
| 3 | Cadastros administrativos | Concluida |
| 4 | Emprestimos, devolucoes e renovacoes transacionais | Concluida |
| 5 | Reservas e integracao com circulacao | Concluida |
| 6 | Self Checkout | Concluida |
| 7 | Eventos, logs, auditoria e consultas NoSQL | Concluida |
| 8 | Relatorios operacionais | Concluida |
| 9 | Testes automatizados integrados | Concluida |
| 10 | Documentacao final | Concluida |
| 11 | Cadastros V2: busca, alteracao, exclusao/desativacao | Concluida |
| 12 | Auditoria profissional com snapshots antes/depois | Concluida |
| 13 | Login, hash BCrypt e permissoes por perfil | Concluida |
| 14 | Regras de negocio reforcadas e vinculos bibliograficos | Concluida |
| 15 | Roteiro, checklist e evidencias finais de avaliacao | Concluida |
| 16 | Base visual reutilizavel da interface console | Concluida |
| 17 | Padronizacao dos menus da aplicacao | Concluida |
| 18 | Padronizacao de formularios e mensagens | Concluida |
| 19 | Melhoria de tabelas e listagens | Concluida |
| 20 | Fechamento da V5 e documentacao final | Concluida |

## Funcionalidades Principais

- Cadastro, listagem, busca, alteracao e exclusao/desativacao de usuarios, autores, editoras, generos, livros e exemplares.
- Controle transacional de emprestimo, devolucao e renovacao.
- Regras de bloqueio para usuario inativo, usuario bloqueado, atraso, limite de exemplares, exemplar indisponivel e reserva de outro usuario.
- Reserva com criacao, cancelamento, atendimento e expiracao.
- Self Checkout com identificacao por CPF, emprestimo/devolucao por codigo de barras ou RFID, renovacao, consulta de emprestimos, consulta de reservas e pesquisa de livros.
- Registro NoSQL de eventos, logs e auditoria.
- Consultas NoSQL pela propria aplicacao.
- Relatorios de acervo, atrasos, livros mais emprestados, origens de emprestimo, reservas ativas e usuarios com pendencias.
- Testes integrados em bancos isolados.
- Login obrigatorio, senhas com hash BCrypt, troca de senha, recuperacao sem administrador ativo e controle por perfil.
- Auditoria antes/depois real no MongoDB, incluindo o operador autenticado nas alteracoes administrativas.
- Setup local automatizado para preparar PostgreSQL, MongoDB e build.
- Roteiro de apresentacao, roteiro relampago de 5 minutos, checklist de avaliacao e matriz de evidencias.
- Interface console V5 com cabecalhos, menus, prompts, mensagens e tabelas padronizadas.
- Video de apresentacao V4 versionado no repositorio.
- Diagrama resumido de arquitetura em Mermaid.

## Persistencia

PostgreSQL e a fonte da verdade para dados operacionais. MongoDB complementa a aplicacao com observabilidade e historico documental.

Operacoes relacionais criticas usam transacoes explicitas e bloqueios com `SELECT ... FOR UPDATE` quando ha risco de concorrencia sobre exemplares e itens de emprestimo.

## Validacao

A validacao automatizada principal e feita com:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

Essa rotina recompila o projeto se necessario, recria bancos de teste, aplica migrations, insere fixtures, executa a aplicacao com entradas simuladas e valida PostgreSQL/MongoDB.

## Observacoes

O sistema foi construido como aplicacao academica em terminal, com persistencia relacional/documental, seguranca por perfil, regras reforcadas, testes integrados, documentacao de avaliacao e polimento final da interface console. A consolidacao funcional esta registrada em `docs/especificacao_v3.md`, a evolucao de qualidade em `docs/especificacao_v4.md` e a melhoria final de experiencia em `docs/especificacao_v5.md`.

## Especificacoes

A especificacao consolidada final esta em `docs/especificacao_v3.md`.

A especificacao V4 esta em `docs/especificacao_v4.md` e compila melhorias recomendadas para uma versao posterior: auditoria com operador real, troca de senha, primeiro acesso mais claro, script de setup local e roteiro de demonstracao em 5 minutos. Os itens V4 de auditoria com operador real, troca de senha, primeiro acesso/recuperacao sem administrador ativo, setup local automatizado e roteiro de demonstracao em 5 minutos ja estao implementados e validados.

Hoje, a especificacao V5 esta em `docs/especificacao_v5.md` e documenta a ultima melhoria planejada: polimento visual da interface console, com base reutilizavel, menus padronizados, formularios alinhados, mensagens consistentes e tabelas mais legiveis.

## Fechamento V4

A V4 foi concluida como camada final de polimento: todos os itens previstos foram implementados ou materializados em documentacao, com validacao por setup local, suite integrada e referencias atualizadas no README e guias do projeto.

## Fechamento V5

A V5 encerra o ciclo de melhorias do SmartLibrary com foco na experiencia de uso no terminal. Foram adicionados helpers reutilizaveis em `src/ui/console_ui.c`, aplicados menus e formularios padronizados e centralizada a renderizacao de tabelas. A aplicacao continua 100% console e C17, mas com apresentacao mais consistente e profissional.
