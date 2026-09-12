# Evidencias De Testes

A Fase 15 consolida a cobertura automatizada em uma matriz de evidencias. O teste integrado principal continua centralizado em `tests/run_phase9_tests.ps1`, executado pelo alvo `mingw32-make test`.

## Matriz De Cobertura

| Area | Validacao Automatizada |
| --- | --- |
| Build | Compilacao completa pelo Makefile com C17 e warnings habilitados |
| Schema PostgreSQL | Recriacao do banco `smartlibrary_test` e aplicacao dos scripts SQL |
| Setup local | Validacao do script `scripts/setup_local.ps1` para banco principal, MongoDB e build |
| Roteiro 5 minutos | Documento `docs/roteiro_5_minutos.md` para demonstracao cronometrada |
| MongoDB | Recriacao do banco `smartlibrary_test` e validacao de eventos, logs e auditoria |
| Login, senha e primeiro acesso | Autenticacao com BCrypt, troca de senha e recuperacao quando nao ha administrador ativo |
| Permissoes | Bloqueio de bibliotecario ao acessar cadastros administrativos |
| Cadastros V2 | Busca, alteracao, desativacao e bloqueio de exclusao vinculada |
| Regras Fase 14 | CPF invalido rejeitado, exemplar emprestado protegido e vinculos bibliograficos persistidos |
| Circulacao | Emprestimo e devolucao com estado final do exemplar validado |
| Relatorios | Acesso aos relatorios operacionais pela aplicacao |
| NoSQL | Consulta ao menu NoSQL e conferencia de documentos persistidos |
| Auditoria profissional | Verificacao de `antes`, `depois` e `usuarioId` do operador autenticado no MongoDB |

## Bases Usadas

- PostgreSQL: `smartlibrary_test`.
- MongoDB: `smartlibrary_test`.

As bases sao recriadas a cada execucao para garantir repetibilidade.

## Como Interpretar

Quando o comando abaixo termina com codigo zero, a entrega esta funcional no ambiente local validado:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

Falhas interrompem o script com mensagem `ASSERTION FAILED`, indicando a regra que deixou de ser atendida.