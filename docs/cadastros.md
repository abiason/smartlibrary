# Cadastros

A Fase 3 implementou os cadastros administrativos basicos para as entidades principais do acervo e dos usuarios. A Fase 11 amplia esse modulo com manutencao completa dos registros. A Fase 14 reforca validacoes, buscas e vinculos bibliograficos.

## Entidades Atendidas

- usuario
- autor
- editora
- genero
- livro
- exemplar

## Operacoes

Cada entidade possui fluxo de criacao, listagem, busca, alteracao e exclusao quando permitido pelas regras relacionais.

As buscas aceitam identificadores ou termos textuais relevantes:

- usuario: ID, CPF ou nome;
- autor, editora e genero: ID ou nome;
- livro: ID, ISBN, titulo, editora, autor ou genero;
- exemplar: ID, codigo de barras, RFID ou titulo do livro.


## Vinculos Bibliograficos

O menu de cadastros permite vincular e desvincular livros de autores e generos. As operacoes usam as tabelas associativas `livro_autor` e `livro_genero`, respeitam chaves estrangeiras e registram auditoria do livro alterado.

## Validacoes Da Fase 14

- CPF de usuario e validado pelos digitos verificadores no cadastro.
- E-mail de usuario e validado quando informado.
- Livro exige titulo, ano entre 1450 e 2100 quando informado, e edicao nao negativa.
- Exemplar com emprestimo aberto nao pode ser alterado manualmente; a devolucao deve ocorrer pelo fluxo de circulacao.
## Alteracao

As telas de alteracao coletam novamente os campos editaveis da entidade e enviam os dados para services e repositories com SQL parametrizado.

No cadastro de usuario, CPF e senha nao sao alterados por esse fluxo. O operador pode atualizar nome, email, telefone, perfil, situacao ativa e bloqueio.

## Exclusao E Desativacao

A exclusao respeita vinculos existentes para evitar perda indevida de historico:

- usuario com emprestimos ou reservas e desativado em vez de excluido;
- exemplar com historico de emprestimo e movido para manutencao em vez de excluido;
- livro com exemplares ou reservas vinculadas nao e excluido;
- autor vinculado a livro nao e excluido;
- editora vinculada a livro nao e excluida;
- genero vinculado a livro nao e excluido.

Quando nao ha historico ou vinculo impeditivo, a exclusao fisica e executada.

## Auditoria

Alteracoes, exclusoes e desativacoes bem-sucedidas registram documentos na colecao `auditoria` do MongoDB. A Fase 12 grava snapshots reais do registro antes e depois da operacao, usando JSON produzido pelo PostgreSQL e armazenado como objeto BSON quando valido. Em exclusoes fisicas, o campo `depois` fica nulo; em desativacoes, ele mostra o novo estado do registro.

## Separacao De Responsabilidades

- UI: coleta dados do operador e mostra menus.
- Services: valida campos obrigatorios, coordena regras e registra auditoria.
- Repositories: executam SQL parametrizado com `PQexecParams()`.

## Regras De Seguranca

- A UI nao possui SQL.
- Entradas do operador nao sao concatenadas em consultas.
- Senhas sao coletadas como senha operacional e armazenadas como hash BCrypt no campo `senha_hash`.
