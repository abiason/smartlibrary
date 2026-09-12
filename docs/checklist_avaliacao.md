# Checklist De Avaliacao

Este checklist organiza as evidencias de entrega do SmartLibrary para revisao academica.

| Criterio | Evidencia | Status |
| --- | --- | --- |
| Linguagem C | Codigo-fonte em `src/` compilado com C17 | OK |
| Build automatizado | `Makefile` com alvos `all`, `run`, `test` e `clean` | OK |
| PostgreSQL | Scripts em `database/postgresql/` | OK |
| MongoDB | Validators e indexes em `database/mongodb/` | OK |
| Arquitetura em camadas | Pastas `ui`, `services`, `repositories`, `models`, `database`, `events`, `auth` | OK |
| Cadastros | Usuarios, autores, editoras, generos, livros e exemplares | OK |
| CRUD completo | Criar, listar, buscar, alterar e excluir/desativar | OK |
| Vinculos bibliograficos | `livro_autor` e `livro_genero` pela aplicacao | OK |
| Circulacao | Emprestimo, devolucao e renovacao | OK |
| Reservas | Criacao, cancelamento, atendimento, expiracao e listagem | OK |
| Self Checkout | Identificacao por CPF e operacoes por codigo/RFID | OK |
| Relatorios | Acervo, atrasos, ranking, origem, reservas e pendencias | OK |
| Interface console V5 | Menus, formularios, mensagens e tabelas padronizadas | OK |
| Auditoria NoSQL | Eventos, logs e auditoria no MongoDB | OK |
| Auditoria antes/depois | Snapshots reais em documentos MongoDB | OK |
| Login | Autenticacao obrigatoria antes do menu | OK |
| Senha segura | Hash BCrypt via PostgreSQL `pgcrypto` | OK |
| Permissoes | Controle por `ADMINISTRADOR`, `BIBLIOTECARIO` e `USUARIO` | OK |
| Regras reforcadas | CPF, e-mail, dados editoriais e bloqueio de exemplar emprestado | OK |
| Testes integrados | `mingw32-make test` com bancos isolados | OK |
| Documentacao | README e documentos em `docs/` | OK |
| Versionamento | Commits organizados e push para GitHub | OK |

## Comando De Evidencia

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

Resultado esperado ao final:

```text
[TEST] OK: build, login/perfis, regras Fase 14, cadastros V2, fluxo, relatorios, PostgreSQL e MongoDB validados.
```

## Observacoes Para O Avaliador

- A senha do PostgreSQL nao e versionada.
- A senha de usuarios da aplicacao e armazenada com hash BCrypt.
- Os testes usam bancos `smartlibrary_test`, sem alterar a base principal.
- O primeiro administrador e criado por fluxo de primeiro acesso quando a tabela `usuario` esta vazia.
- A V5 preserva a proposta de aplicacao console em C, mas melhora a organizacao visual das telas.
