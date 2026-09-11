# Modelo NoSQL

O MongoDB armazena eventos, logs, auditoria e telemetria. Ele nao substitui o PostgreSQL como fonte da verdade transacional.

## Colecoes

### eventos

Registra eventos de dominio e eventos do Self Checkout.

Campos principais: tipo, dataHora, usuarioId, emprestimoId, terminal, exemplar e origem.

Eventos iniciais: SELF_CHECKOUT_INICIADO, SELF_CHECKOUT_FINALIZADO, USUARIO_IDENTIFICADO, USUARIO_BLOQUEADO, EMPRESTIMO_REALIZADO, EMPRESTIMO_NEGADO, DEVOLUCAO_REALIZADA, DEVOLUCAO_NEGADA, RENOVACAO_REALIZADA, RENOVACAO_NEGADA, RESERVA_REALIZADA e RESERVA_CANCELADA.

### logs

Registra informacoes operacionais e diagnosticas.

Campos principais: nivel, dataHora, componente, terminalCodigo, mensagem e detalhes.

### auditoria

Registra alteracoes relevantes em entidades e configuracoes.

Campos principais: entidade, entidadeId, acao, usuarioId, dataHora, antes e depois.

## Scripts

```sh
mongosh database/mongodb/validators.js
mongosh database/mongodb/indexes.js
```

## Regra De Consistencia

Falha ao registrar evento no MongoDB nao deve desfazer uma operacao ja confirmada no PostgreSQL. Eventos devem ser gravados apos o COMMIT da transacao relacional.
