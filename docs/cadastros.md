# Cadastros

A Fase 3 implementa cadastros administrativos basicos para as entidades principais do acervo e dos usuarios.

## Entidades Atendidas

- usuario
- autor
- editora
- genero
- livro
- exemplar

## Operacoes

Cada entidade possui fluxo inicial de criacao e listagem. Atualizacao, exclusao logica detalhada, filtros avancados e associacoes muitos-para-muitos entre livros/autores/generos devem evoluir em fases futuras.

## Separacao De Responsabilidades

- UI: coleta dados do operador e mostra menus.
- Services: valida campos obrigatorios e coordena a operacao.
- Repositories: executam SQL parametrizado com `PQexecParams()`.

## Regras De Seguranca

- A UI nao possui SQL.
- Entradas do operador nao sao concatenadas em consultas.
- Senhas nao sao coletadas em texto puro nesta fase; o cadastro recebe o campo `senha_hash`.
