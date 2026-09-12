# SmartLibrary - Especificacao V4

## 1. Visao Geral

A Especificacao V4 define uma evolucao de polimento final para o SmartLibrary, partindo do estado consolidado na Especificacao V3. O objetivo desta versao e aumentar a maturidade da entrega academica, aproximando o sistema de criterios de nota maxima sem alterar o escopo central ja implementado nas Fases 1 a 15.

A V4 nao substitui a V3 como retrato do que ja foi entregue. Ela organiza melhorias recomendadas para uma versao imediatamente posterior, com foco em rastreabilidade, seguranca operacional, facilidade de demonstracao e preparacao automatizada do ambiente.

## 2. Objetivos Da V4

- Registrar o operador autenticado real nas auditorias administrativas.
- Permitir troca de senha pela propria aplicacao.
- Melhorar mensagens e fluxo de primeiro acesso/login.
- Criar um script de preparacao local do ambiente.
- Criar um roteiro curto de demonstracao em aproximadamente 5 minutos.
- Manter a compatibilidade com as Fases 1 a 15 ja implementadas.
- Preservar a regra de nao versionar senhas reais.

## 3. Escopo Atual Herdado Da V3

A V4 parte de um sistema ja contendo:

- C17 com arquitetura em camadas.
- PostgreSQL como fonte transacional.
- MongoDB para eventos, logs e auditoria.
- Login obrigatorio.
- Senhas com hash BCrypt via `pgcrypto`.
- Controle de permissoes por perfil.
- Cadastros completos com CRUD administrativo.
- Auditoria antes/depois em documentos MongoDB.
- Circulacao, reservas, Self Checkout e relatorios.
- Testes integrados com bancos isolados.
- Roteiro, checklist e evidencias finais de avaliacao.

## 4. Melhorias Propostas

### 4.1 Auditoria Com Operador Real

Atualmente, auditorias administrativas podem usar `usuarioId = 0` para indicar operacao de sistema ou operador nao propagado. Na V4, toda acao administrativa deve registrar o usuario autenticado responsavel.

Requisitos:

- Propagar `AuthSession` ou `usuario_id` do operador para services administrativos.
- Registrar `usuarioId` real na colecao `auditoria`.
- Preservar `antes` e `depois` da auditoria profissional.
- Manter fallback seguro para operacoes internas do sistema.

Criterios de aceite:

- Alteracao de usuario feita por admin registra `usuarioId` do admin logado.
- Vinculo livro/autor feito por admin registra `usuarioId` do admin logado.
- Teste automatizado consulta MongoDB e confirma que a auditoria possui o operador esperado.

### 4.2 Troca De Senha

A V4 deve incluir uma opcao para troca de senha do usuario autenticado.

Requisitos:

- Criar opcao de menu para trocar senha.
- Solicitar senha atual.
- Solicitar nova senha.
- Confirmar nova senha.
- Validar senha atual usando `crypt()`.
- Armazenar nova senha usando `crypt(nova_senha, gen_salt('bf'))`.
- Registrar log operacional no MongoDB sem expor a senha.

Regras recomendadas:

- Nova senha deve ter tamanho minimo de 6 caracteres.
- Nova senha nao deve ser vazia.
- Senha e confirmacao devem coincidir.
- A aplicacao nunca deve imprimir ou persistir senha em texto puro.

Criterios de aceite:

- Login com senha antiga deixa de funcionar apos a troca.
- Login com senha nova funciona.
- Hash armazenado em `usuario.senha_hash` nao e igual ao texto da senha.
- Log de troca de senha e registrado sem detalhes sensiveis.

### 4.3 Login E Primeiro Acesso Mais Claros

A V4 deve melhorar a experiencia de autenticacao, principalmente quando o banco possui usuarios comuns mas nenhum administrador.

Requisitos:

- Detectar ausencia de usuario com perfil `ADMINISTRADOR` ativo.
- Exibir mensagem clara quando nao houver administrador ativo.
- Permitir fluxo controlado de criacao do primeiro administrador quando nao existir admin ativo.
- Diferenciar falha por credencial invalida de falha por usuario inativo/bloqueado quando possivel.
- Manter limite de tentativas de login.

Criterios de aceite:

- Banco sem usuarios abre primeiro acesso.
- Banco com usuarios comuns, mas sem admin ativo, orienta criacao/recuperacao de admin.
- Mensagens de erro ficam compreensiveis para apresentacao.

### 4.4 Script De Preparacao Local

A V4 deve incluir um script de setup para reduzir passos manuais ao preparar uma maquina nova.

Arquivo sugerido:

```text
scripts/setup_local.ps1
```

Responsabilidades:

- Verificar `POSTGRES_PASSWORD`.
- Criar banco PostgreSQL `smartlibrary` se nao existir.
- Aplicar scripts SQL na ordem correta.
- Aplicar validators e indexes do MongoDB.
- Validar disponibilidade de `psql`, `mongosh` e `mingw32-make`.
- Opcionalmente compilar o projeto ao final.

Regras:

- O script nao deve gravar senha real em arquivo.
- O script deve exibir mensagens claras de progresso.
- O script deve falhar com erro claro quando uma dependencia estiver ausente.

Criterios de aceite:

- Ambiente limpo pode ser preparado com um unico comando PowerShell.
- O script nao altera bancos de teste usados por `mingw32-make test`.
- Documentacao referencia o script no README e no guia de execucao.

### 4.5 Roteiro Relampago De Demonstracao

A V4 deve incluir um roteiro curto, objetivo, para apresentacao de aproximadamente 5 minutos.

Arquivo sugerido:

```text
docs/roteiro_5_minutos.md
```

Conteudo minimo:

- Como iniciar a aplicacao.
- Como fazer login.
- Qual fluxo demonstrar primeiro.
- Como provar PostgreSQL e MongoDB.
- Como mostrar testes integrados.
- Como encerrar com os diferenciais tecnicos.

Criterios de aceite:

- O roteiro cabe em uma leitura rapida.
- O roteiro destaca os pontos de maior impacto: login, permissoes, auditoria antes/depois, regras reforcadas e testes.
- O roteiro aponta os comandos essenciais.

## 5. Requisitos Funcionais V4

### RF18 - Auditoria Com Operador Autenticado

O sistema deve registrar na auditoria MongoDB o ID do operador autenticado responsavel por alteracoes administrativas.

### RF19 - Troca De Senha

O sistema deve permitir que o usuario autenticado altere sua propria senha mediante validacao da senha atual.

### RF20 - Diagnostico De Primeiro Acesso

O sistema deve informar de forma clara quando nao houver administrador ativo e orientar o operador sobre a criacao do primeiro administrador.

### RF21 - Setup Local Automatizado

O projeto deve disponibilizar script PowerShell para preparar banco PostgreSQL, MongoDB e dependencias de execucao local.

### RF22 - Roteiro Relampago

O projeto deve disponibilizar roteiro de demonstracao de ate 5 minutos para avaliacao academica.

## 6. Requisitos Nao Funcionais V4

- Nao versionar senhas reais.
- Nao registrar senha em logs, eventos ou auditoria.
- Manter SQL parametrizado.
- Manter compatibilidade com `mingw32-make test`.
- Preservar arquitetura em camadas.
- Manter documentacao em Markdown.
- Evitar aumento desnecessario de complexidade no terminal.

## 7. Plano Incremental V4

| Ordem | Entrega | Impacto Esperado |
| --- | --- | --- |
| 1 | Propagar operador real para auditoria | Implementada e testada |
| 2 | Criar troca de senha | Implementada e testada |
| 3 | Melhorar login e primeiro acesso | Implementada e testada |
| 4 | Criar `scripts/setup_local.ps1` | Implementada e validada |
| 5 | Criar `docs/roteiro_5_minutos.md` | Apresentacao mais objetiva |
| 6 | Atualizar testes e documentos | Evidencia formal da V4 |

## 8. Criterios De Aceite Da V4

- `mingw32-make test` termina com codigo zero.
- Auditoria administrativa registra `usuarioId` do operador logado.
- Troca de senha exige senha atual correta.
- Nova senha permite login e senha antiga deixa de permitir.
- Nenhum log ou documento MongoDB armazena senha em texto puro.
- Primeiro acesso/login exibe mensagens claras quando nao ha admin ativo.
- `scripts/setup_local.ps1` prepara o ambiente local sem versionar segredo.
- `docs/roteiro_5_minutos.md` existe e orienta apresentacao curta.
- README e guia de execucao referenciam as novas facilidades.

## 9. Evidencias Esperadas

Arquivos novos ou alterados esperados:

- `src/auth/auth_service.c`;
- `src/auth/auth_service.h`;
- `src/ui/main_ui.c`;
- `src/services/cadastro_service.c`;
- `src/events/event_service.c`;
- `scripts/setup_local.ps1`;
- `docs/roteiro_5_minutos.md`;
- `docs/especificacao_v4.md`;
- `README.md`;
- `docs/guia_execucao.md`;
- `docs/testes.md`;
- `tests/run_phase9_tests.ps1`.

Comando de validacao esperado:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

## 10. Observacoes Finais

A V3 representa a entrega premium ja concluida. A V4 organiza o ultimo conjunto de melhorias recomendado para tentar cravar nota maxima com mais tranquilidade: saber quem executou cada alteracao, oferecer troca de senha, reduzir atrito de setup e facilitar a apresentacao em tempo curto.

A prioridade tecnica mais importante da V4 e a auditoria com operador real, pois conecta autenticacao, permissoes e rastreabilidade em uma evidencia unica e forte para avaliacao.