# Relatorios

A Fase 8 adiciona consultas consolidadas para apoio operacional da biblioteca. Os relatorios usam o PostgreSQL como fonte transacional e ficam disponiveis no menu principal em `Relatorios`.

## Relatorios Disponiveis

### Acervo por status

Agrupa livros e exemplares, exibindo totais por disponibilidade:

- total de exemplares;
- exemplares disponiveis;
- exemplares emprestados;
- exemplares reservados;
- exemplares em manutencao ou extraviados.

### Emprestimos atrasados

Lista itens de emprestimo ainda abertos cuja data prevista de devolucao ja passou.

Campos principais: item, emprestimo, usuario, email, codigo de barras, titulo, data prevista e dias de atraso.

### Livros mais emprestados

Ranking dos livros com maior quantidade historica de itens de emprestimo. O usuario informa o limite de linhas no momento da consulta.

### Emprestimos por origem

Agrupa emprestimos por origem operacional, separando `BALCAO` e `SELF_CHECKOUT`, alem de totais por status.

### Reservas ativas

Lista reservas ainda ativas com usuario, livro, data de reserva e data de expiracao.

### Usuarios com pendencias

Lista usuarios com itens atrasados, total de itens pendentes e atraso mais antigo.

## Observacoes De Implementacao

A origem do emprestimo passou a ser persistida a partir da camada de servico. Operacoes iniciadas no menu de circulacao gravam `BALCAO`, enquanto operacoes iniciadas no Self Checkout gravam `SELF_CHECKOUT`.

Os relatorios sao somente leitura e nao alteram dados operacionais.
