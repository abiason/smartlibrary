# Entrega Final

Este documento consolida a entrega completa do SmartLibrary conforme o plano incremental expandido para 15 fases.

## Objetivo

Desenvolver um sistema academico de biblioteca em C17 com persistencia relacional em PostgreSQL, persistencia documental em MongoDB, interface em terminal, Self Checkout, relatorios e testes automatizados.

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
- Login obrigatorio, senhas com hash BCrypt e controle por perfil.
- Auditoria antes/depois real no MongoDB.
- Roteiro de apresentacao, checklist de avaliacao e matriz de evidencias.

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

O sistema foi construido como aplicacao academica em terminal, com persistencia relacional/documental, seguranca por perfil, regras reforcadas, testes integrados e documentacao de avaliacao. A consolidacao final esta registrada em `docs/especificacao_v3.md`, refletindo o escopo implementado ate a Fase 15. A evolucao planejada para buscar nota maxima esta organizada em `docs/especificacao_v4.md`.

## Especificacoes

A especificacao consolidada final esta em `docs/especificacao_v3.md`.

A especificacao V4 esta em `docs/especificacao_v4.md` e compila melhorias recomendadas para uma versao posterior: auditoria com operador real, troca de senha, primeiro acesso mais claro, script de setup local e roteiro de demonstracao em 5 minutos.
