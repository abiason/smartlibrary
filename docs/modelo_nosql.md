# Modelo NoSQL

O MongoDB armazena eventos, logs e auditoria. Ele nao substitui o PostgreSQL como fonte da verdade transacional; as operacoes de emprestimo, devolucao, renovacao e reserva continuam confirmadas primeiro no banco relacional.

## Colecoes

### eventos

Registra eventos de dominio e eventos do Self Checkout.

Campos principais: tipo, dataHora, usuarioId, emprestimoId, terminal, exemplar e origem.

Eventos cobertos nesta fase: SELF_CHECKOUT_INICIADO, SELF_CHECKOUT_FINALIZADO, USUARIO_IDENTIFICADO, USUARIO_BLOQUEADO, EMPRESTIMO_REALIZADO, EMPRESTIMO_NEGADO, DEVOLUCAO_REALIZADA, DEVOLUCAO_NEGADA, RENOVACAO_REALIZADA, RENOVACAO_NEGADA, RESERVA_REALIZADA, RESERVA_CANCELADA, RESERVA_ATENDIDA e RESERVA_EXPIRADA.

A origem diferencia BALCAO, SELF_CHECKOUT e SISTEMA.

### logs

Registra informacoes operacionais e diagnosticas sem interferir na transacao relacional.

Campos principais: nivel, dataHora, componente, terminalCodigo, mensagem e detalhes.

Exemplos de uso: inicializacao do sistema, encerramento e operacoes negadas por regra de negocio.

### auditoria

Registra alteracoes relevantes em entidades de negocio.

Campos principais: entidade, entidadeId, acao, usuarioId, dataHora, antes e depois.

Exemplos de uso: criacao de emprestimo, devolucao, renovacao e mudancas de status de reserva.

## Consultas Na Aplicacao

O menu principal possui a opcao `NoSQL: eventos, logs e auditoria`, com consultas para:

- listar eventos recentes;
- listar logs recentes;
- listar auditoria recente;
- resumir eventos por tipo.

## Scripts

```sh
mongosh database/mongodb/validators.js
mongosh database/mongodb/indexes.js
```

## Regra De Consistencia

Falha ao registrar documento no MongoDB nao desfaz uma operacao ja confirmada no PostgreSQL. Eventos, logs e auditoria sao gravados apos ou ao redor das operacoes relacionais como observabilidade e trilha historica.
