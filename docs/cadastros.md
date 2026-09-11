# Cadastros

A Fase 3 implementou os cadastros administrativos basicos para as entidades principais do acervo e dos usuarios. A Fase 11 amplia esse modulo com manutencao completa dos registros.

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
- livro: ID, ISBN ou titulo;
- exemplar: ID, codigo de barras, RFID ou titulo do livro.

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

Alteracoes, exclusoes e desativacoes bem-sucedidas registram documentos na colecao `auditoria` do MongoDB. Isso mantem uma trilha operacional complementar ao banco transacional.

## Separacao De Responsabilidades

- UI: coleta dados do operador e mostra menus.
- Services: valida campos obrigatorios, coordena regras e registra auditoria.
- Repositories: executam SQL parametrizado com `PQexecParams()`.

## Regras De Seguranca

- A UI nao possui SQL.
- Entradas do operador nao sao concatenadas em consultas.
- Senhas nao sao coletadas em texto puro nesta fase; o cadastro recebe o campo `senha_hash` na criacao.
