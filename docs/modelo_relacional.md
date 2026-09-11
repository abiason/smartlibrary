# Modelo Relacional

O PostgreSQL e a fonte principal da verdade transacional do SmartLibrary. Emprestimos, devolucoes, renovacoes e reservas devem ser confirmados no PostgreSQL antes de qualquer registro complementar no MongoDB.

## Entidades

- perfil
- usuario
- autor
- editora
- genero
- livro
- livro_autor
- livro_genero
- exemplar
- emprestimo
- emprestimo_item
- reserva
- terminal_self_checkout
- configuracao

## Relacionamentos Principais

- Um perfil possui muitos usuarios.
- Uma editora publica muitos livros.
- Um livro possui muitos exemplares.
- Livro e autor possuem relacionamento muitos-para-muitos via livro_autor.
- Livro e genero possuem relacionamento muitos-para-muitos via livro_genero.
- Um usuario realiza muitos emprestimos e reservas.
- Um emprestimo possui um ou mais itens.
- Cada item de emprestimo referencia um exemplar.

## Scripts

Execute os scripts na ordem abaixo:

```sh
psql -d smartlibrary -f database/postgresql/001_schema.sql
psql -d smartlibrary -f database/postgresql/002_constraints.sql
psql -d smartlibrary -f database/postgresql/003_indexes.sql
psql -d smartlibrary -f database/postgresql/004_seed.sql
```

## Status Controlados

Status de exemplar: DISPONIVEL, EMPRESTADO, RESERVADO, MANUTENCAO, EXTRAVIADO.

Status de emprestimo: ABERTO, ENCERRADO, CANCELADO.

Origem de emprestimo: BALCAO, SELF_CHECKOUT.

Status de reserva: ATIVA, ATENDIDA, CANCELADA, EXPIRADA.
