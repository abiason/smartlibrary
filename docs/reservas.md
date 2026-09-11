# Reservas

A Fase 5 implementa operacoes basicas de reserva e integra essas regras aos emprestimos e renovacoes.

## Operacoes

- Criar reserva para usuario e livro.
- Cancelar reserva ativa.
- Atender reserva ativa.
- Expirar reservas vencidas.
- Listar reservas.

## Regras Implementadas

A criacao de reserva exige:

- usuario existente, ativo e nao bloqueado;
- livro existente;
- ausencia de outra reserva ativa do mesmo usuario para o mesmo livro.

Emprestimos e renovacoes sao negados quando o livro possui reserva ativa de outro usuario.

Quando um usuario realiza emprestimo de um livro que ele mesmo reservou, a reserva ativa correspondente e marcada como ATENDIDA dentro da transacao do emprestimo.

## Status

- ATIVA
- ATENDIDA
- CANCELADA
- EXPIRADA

## Eventos

O sistema registra eventos basicos no MongoDB para:

- RESERVA_REALIZADA
- RESERVA_CANCELADA
- RESERVA_ATENDIDA
- RESERVA_EXPIRADA
