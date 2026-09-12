# SmartLibrary - Interface Console V5

## Objetivo

A V5 finaliza o ciclo de melhorias do SmartLibrary com foco na experiencia do usuario no terminal. A aplicacao permanece console e escrita em C17, mas passa a ter uma apresentacao mais consistente, organizada e adequada para demonstracao academica.

## O Que Foi Padronizado

- Cabecalhos com moldura ASCII.
- Menus principais e secundarios com itens no formato `[n] Opcao`.
- Rodape de retorno com `[0]`.
- Prompts de formulario alinhados.
- Mensagens de sucesso, erro, aviso e informacao em blocos visuais.
- Tabelas relacionais com bordas, cabecalho e larguras calculadas.

## Arquivos Principais

- `src/ui/console_ui.h`
- `src/ui/console_ui.c`
- `src/main.c`
- `src/ui/main_ui.c`
- `src/ui/cadastro_ui.c`
- `src/ui/emprestimo_ui.c`
- `src/ui/reserva_ui.c`
- `src/ui/self_checkout_ui.c`
- `src/ui/nosql_ui.c`
- `src/ui/relatorio_ui.c`

## Helpers De Interface

O modulo `console_ui` centraliza os elementos visuais reutilizaveis:

- `ui_header`
- `ui_context`
- `ui_menu_item`
- `ui_menu_back`
- `ui_read_line`
- `ui_read_int`
- `ui_success`
- `ui_error`
- `ui_info`
- `ui_warning`
- `ui_print_pgresult_table`

## Exemplo De Menu

```text
+----------------------------------------------------------------------------+
| SMARTLIBRARY                                                                |
| Menu Principal                                                              |
+----------------------------------------------------------------------------+

  Operador    : Administrador do Sistema
  Perfil      : ADMINISTRADOR

  [1] Cadastros administrativos
  [2] Circulacao
  [3] Reservas
  [4] Self Checkout
  [5] Eventos, logs e auditoria
  [6] Relatorios
  [7] Trocar senha

  [0] Sair

Escolha uma opcao:
```

## Exemplo De Mensagem

```text
+----------------------------------------------------------------------------+
| SUCESSO                                                                     |
+----------------------------------------------------------------------------+
| [OK] Operacao concluida.                                                    |
+----------------------------------------------------------------------------+
```

## Exemplo De Tabela

```text
+------------+----------------------------+----------------+----------------+
| id_usuario | nome                       | cpf            | perfil         |
+------------+----------------------------+----------------+----------------+
| 1          | Administrador do Sistema   | 000.000.000-00 | ADMINISTRADOR  |
+------------+----------------------------+----------------+----------------+
```

## Decisoes De Projeto

- A interface usa apenas ASCII para manter compatibilidade entre Windows, MSYS2 e terminais simples.
- Cores ficaram fora do escopo para evitar dependencias de terminal e manter a demonstracao previsivel.
- Os textos criticos usados pelos testes foram preservados, especialmente marcadores como `[OK]`, `[ERRO]` e `[INFO]`.
- A camada visual nao altera regras de negocio, queries, models ou estrutura dos bancos.

## Validacao

A V5 foi validada com:

```powershell
mingw32-make all
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
git diff --check
```

Resultado esperado dos testes:

```text
[TEST] OK: build, login/perfis, regras Fase 14, cadastros V2, fluxo, relatorios, PostgreSQL e MongoDB validados.
```
