# Emprestimos, Devolucoes E Renovacoes

A Fase 4 implementa operacoes transacionais de circulacao no PostgreSQL.

## Operacoes

- Realizar emprestimo por usuario e codigo de barras do exemplar.
- Realizar devolucao por codigo de barras do exemplar.
- Renovar item de emprestimo aberto.
- Listar emprestimos abertos.

## Regras Implementadas

Emprestimo e negado quando:

- usuario nao existe;
- usuario esta inativo;
- usuario esta bloqueado;
- usuario possui emprestimo atrasado;
- usuario atingiu 5 exemplares simultaneos;
- exemplar nao existe;
- exemplar nao esta disponivel;
- livro possui reserva ativa de outro usuario.

Renovacao e negada quando:

- item de emprestimo nao existe ou nao esta aberto;
- limite de 2 renovacoes foi atingido;
- livro possui reserva ativa de outro usuario.

Devolucao e negada quando:

- exemplar nao existe;
- exemplar nao esta emprestado;
- nao existe emprestimo aberto para o exemplar.

## Transacoes E Concorrencia

As operacoes usam `BEGIN`, `COMMIT` e `ROLLBACK`. O exemplar e bloqueado com `SELECT ... FOR UPDATE` para impedir que dois terminais emprestem ou devolvam o mesmo exemplar simultaneamente.

## Eventos

Depois de uma operacao confirmada no PostgreSQL, o sistema registra eventos basicos no MongoDB:

- EMPRESTIMO_REALIZADO
- EMPRESTIMO_NEGADO
- DEVOLUCAO_REALIZADA
- DEVOLUCAO_NEGADA
- RENOVACAO_REALIZADA
- RENOVACAO_NEGADA

Falha no MongoDB gera aviso, mas nao desfaz uma transacao ja confirmada no PostgreSQL.
