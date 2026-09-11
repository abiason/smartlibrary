# Self Checkout

A Fase 6 implementa o terminal de autoatendimento do SmartLibrary.

## Fluxo Geral

O atendimento inicia com identificacao do usuario por CPF. Apenas usuarios ativos e nao bloqueados entram no menu de autoatendimento.

## Operacoes Disponiveis

- Realizar emprestimo por codigo de barras ou RFID.
- Realizar devolucao por codigo de barras ou RFID.
- Renovar emprestimo do usuario identificado.
- Consultar emprestimos abertos do usuario identificado.
- Consultar reservas do usuario identificado.
- Pesquisar livros por titulo ou ISBN.
- Encerrar atendimento.

## Regras

- O Self Checkout reaproveita as regras transacionais da circulacao.
- Renovacao pelo Self Checkout valida se o item de emprestimo pertence ao usuario identificado.
- A interface de Self Checkout nao contem SQL.
- Acesso a dados fica em repositories; regras ficam em services.

## Eventos

O terminal registra eventos basicos no MongoDB:

- SELF_CHECKOUT_INICIADO
- SELF_CHECKOUT_FINALIZADO
- USUARIO_IDENTIFICADO
- USUARIO_BLOQUEADO
- EMPRESTIMO_REALIZADO
- EMPRESTIMO_NEGADO
- DEVOLUCAO_REALIZADA
- DEVOLUCAO_NEGADA
- RENOVACAO_REALIZADA
- RENOVACAO_NEGADA
