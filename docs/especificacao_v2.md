# SmartLibrary - Especificacao V2

## 1. Visao Geral

O SmartLibrary e um sistema academico de biblioteca desenvolvido em C17, com persistencia relacional em PostgreSQL e persistencia documental em MongoDB. A aplicacao oferece interface em terminal para operacoes administrativas, circulacao de exemplares, reservas, Self Checkout, consultas NoSQL, relatorios e testes automatizados.

Esta versao V2 consolida o escopo original e inclui uma melhoria funcional nos cadastros: buscar, alterar e excluir/desativar registros administrativos.

## 2. Objetivos

- Controlar o acervo de uma biblioteca.
- Gerenciar usuarios, autores, editoras, generos, livros e exemplares.
- Registrar emprestimos, devolucoes e renovacoes com regras transacionais.
- Controlar reservas e impedir conflitos de circulacao.
- Permitir autoatendimento via Self Checkout.
- Registrar eventos, logs e auditoria no MongoDB.
- Disponibilizar relatorios operacionais.
- Validar o sistema com testes automatizados.
- Ampliar os cadastros com operacoes completas de consulta, alteracao e exclusao/desativacao.

## 3. Tecnologias

- Linguagem: C17.
- Banco relacional: PostgreSQL.
- Driver PostgreSQL: libpq.
- Banco documental: MongoDB.
- Driver MongoDB: MongoDB C Driver.
- Build: Makefile, com suporte a `mingw32-make` no Windows/MSYS2.
- Testes: script PowerShell integrado ao Makefile.
- Documentacao: Markdown e Mermaid.

## 4. Arquitetura

O sistema deve seguir arquitetura em camadas:

- UI: menus e interacao com usuario via terminal.
- Services: regras de negocio e coordenacao de operacoes.
- Repositories: acesso a dados e SQL parametrizado.
- Database: conexoes com PostgreSQL e MongoDB.
- Events: eventos, logs, auditoria e consultas NoSQL.
- Models: structs e conversores de entidades.
- Utils: leitura de entrada, limpeza de tela e helpers gerais.
- Tests: preparacao de ambiente, fixtures e validacoes automatizadas.

## 5. Modelo Relacional

O PostgreSQL e a fonte principal da verdade transacional.

Entidades principais:

- perfil;
- usuario;
- autor;
- editora;
- genero;
- livro;
- livro_autor;
- livro_genero;
- exemplar;
- emprestimo;
- emprestimo_item;
- reserva;
- terminal_self_checkout;
- configuracao.

Regras importantes:

- CPF de usuario deve ser unico.
- ISBN de livro deve ser unico quando informado.
- Codigo de barras de exemplar deve ser unico.
- RFID de exemplar deve ser unico quando informado.
- Emprestimos podem ter origem `BALCAO` ou `SELF_CHECKOUT`.
- Status de exemplar: `DISPONIVEL`, `EMPRESTADO`, `RESERVADO`, `MANUTENCAO`, `EXTRAVIADO`.
- Status de emprestimo: `ABERTO`, `ENCERRADO`, `CANCELADO`.
- Status de reserva: `ATIVA`, `ATENDIDA`, `CANCELADA`, `EXPIRADA`.

## 6. Modelo NoSQL

O MongoDB deve armazenar dados complementares:

### eventos

Eventos de dominio e Self Checkout, com origem da operacao.

Exemplos:

- `EMPRESTIMO_REALIZADO`;
- `EMPRESTIMO_NEGADO`;
- `DEVOLUCAO_REALIZADA`;
- `RENOVACAO_REALIZADA`;
- `RESERVA_REALIZADA`;
- `SELF_CHECKOUT_INICIADO`;
- `USUARIO_IDENTIFICADO`.

### logs

Registros operacionais e diagnosticos.

### auditoria

Historico de alteracoes relevantes em entidades do sistema.

Na V2, operacoes de alterar, excluir ou desativar cadastro tambem devem gerar auditoria.

## 7. Requisitos Funcionais

### RF01 - Cadastros Administrativos

O sistema deve permitir cadastrar:

- usuarios;
- autores;
- editoras;
- generos;
- livros;
- exemplares.

### RF02 - Listagem De Cadastros

O sistema deve permitir listar todos os registros das entidades administrativas.

### RF03 - Busca De Cadastros

O sistema deve permitir buscar cadastros por criterios relevantes:

- usuario por ID, CPF ou parte do nome;
- autor por ID ou parte do nome;
- editora por ID ou parte do nome;
- genero por ID ou parte do nome;
- livro por ID, ISBN ou parte do titulo;
- exemplar por ID, codigo de barras, RFID ou parte do titulo do livro.

A busca deve exibir registros encontrados em formato tabular no terminal.

### RF04 - Alteracao De Cadastros

O sistema deve permitir alterar dados cadastrais das entidades administrativas.

Campos sugeridos:

- usuario: nome, email, telefone, perfil, ativo e bloqueado;
- autor: nome e nacionalidade;
- editora: nome, cidade e pais;
- genero: nome e descricao;
- livro: ISBN, titulo, subtitulo, ano, edicao, editora, idioma e descricao;
- exemplar: codigo de barras, RFID, status e localizacao.

Regras:

- Nao permitir duplicidade em campos unicos.
- Nao alterar IDs primarios.
- Registrar auditoria no MongoDB para cada alteracao bem-sucedida.
- Exibir mensagem clara em caso de erro ou violacao de constraint.

### RF05 - Exclusao Ou Desativacao De Cadastros

O sistema deve permitir remover ou desativar registros administrativos, respeitando relacionamentos existentes.

Regras recomendadas:

- Usuario deve ser desativado, nao excluido fisicamente, quando tiver historico de emprestimos ou reservas.
- Exemplar pode ser excluido apenas se nao possuir historico de emprestimo; caso contrario, deve mudar para status operacional adequado, como `MANUTENCAO` ou `EXTRAVIADO`.
- Livro so pode ser excluido se nao possuir exemplares, reservas ou historico relacionado.
- Autor, editora e genero so podem ser excluidos se nao estiverem associados a livros.
- Toda exclusao, desativacao ou alteracao de status deve gerar auditoria no MongoDB.

### RF06 - Emprestimos

O sistema deve permitir realizar emprestimo de exemplar disponivel para usuario ativo e sem bloqueios.

Regras:

- Usuario deve existir, estar ativo e nao bloqueado.
- Usuario nao pode possuir emprestimo atrasado.
- Usuario nao pode exceder o limite de exemplares simultaneos.
- Exemplar deve estar disponivel.
- Livro nao pode estar reservado para outro usuario.
- Operacao deve ser transacional.

### RF07 - Devolucoes

O sistema deve permitir devolver exemplar emprestado.

Regras:

- Exemplar deve existir.
- Exemplar deve estar emprestado.
- Deve existir emprestimo aberto associado.
- Exemplar deve voltar para `DISPONIVEL` ao final da devolucao.
- Emprestimo deve ser encerrado quando todos os itens forem devolvidos.

### RF08 - Renovacoes

O sistema deve permitir renovar item de emprestimo aberto.

Regras:

- Item deve pertencer a emprestimo aberto.
- Item nao pode ter sido devolvido.
- Quantidade maxima de renovacoes deve ser respeitada.
- Renovacao deve ser bloqueada se houver reserva ativa de outro usuario para o livro.

### RF09 - Reservas

O sistema deve permitir:

- criar reserva;
- cancelar reserva;
- atender reserva;
- expirar reservas vencidas;
- listar reservas.

Regras:

- Usuario deve estar ativo e nao bloqueado.
- Livro deve existir.
- Usuario nao pode ter reserva ativa duplicada para o mesmo livro.
- Emprestimo do proprio usuario deve atender sua reserva ativa quando aplicavel.

### RF10 - Self Checkout

O sistema deve permitir autoatendimento com identificacao por CPF.

Funcionalidades:

- identificar usuario;
- realizar emprestimo por codigo de barras ou RFID;
- realizar devolucao por codigo de barras ou RFID;
- renovar emprestimo do usuario identificado;
- consultar emprestimos;
- consultar reservas;
- pesquisar livros.

### RF11 - Eventos, Logs E Auditoria

O sistema deve registrar no MongoDB:

- eventos de circulacao;
- eventos de reserva;
- eventos de Self Checkout;
- logs de sistema;
- auditoria de alteracoes relevantes.

### RF12 - Consultas NoSQL

O sistema deve permitir consultar pela aplicacao:

- eventos recentes;
- logs recentes;
- auditoria recente;
- resumo de eventos por tipo.

### RF13 - Relatorios

O sistema deve disponibilizar relatorios:

- acervo por status;
- emprestimos atrasados;
- livros mais emprestados;
- emprestimos por origem;
- reservas ativas;
- usuarios com pendencias.

### RF14 - Testes Automatizados

O sistema deve possuir rotina automatizada para validar:

- build;
- schema PostgreSQL;
- fixtures;
- fluxo de emprestimo/devolucao;
- relatorios;
- eventos, logs e auditoria no MongoDB.

## 8. Requisitos Nao Funcionais

- Usar C17.
- Usar SQL parametrizado para entradas externas.
- Evitar versionar senhas reais.
- Separar responsabilidades por camadas.
- Manter operacoes criticas em transacoes.
- Tratar falhas de MongoDB sem desfazer operacoes relacionais ja confirmadas.
- Manter documentacao em Markdown.
- Manter scripts de banco versionados.

## 9. Plano Incremental V2

### Fase 1 - Fundacao

Estrutura do projeto, Makefile, configuracao e conexoes.

### Fase 2 - Modelagem

Schema PostgreSQL, validators MongoDB, indexes, structs e documentacao de dados.

### Fase 3 - Cadastros Basicos

Criacao e listagem de usuarios, autores, editoras, generos, livros e exemplares.

### Fase 4 - Circulacao

Emprestimos, devolucoes e renovacoes.

### Fase 5 - Reservas

Criacao, cancelamento, atendimento, expiracao e integracao com emprestimos.

### Fase 6 - Self Checkout

Autoatendimento com CPF, codigo de barras e RFID.

### Fase 7 - NoSQL

Eventos, logs, auditoria e consultas documentais.

### Fase 8 - Relatorios

Relatorios operacionais.

### Fase 9 - Testes

Testes automatizados integrados com bancos isolados.

### Fase 10 - Documentacao Final

README, guia de execucao, resumo de entrega e consolidacao arquitetural.

### Fase 11 - Melhoria De Cadastros V2

Busca, alteracao e exclusao/desativacao de cadastros administrativos.

Entregas esperadas:

- novos metodos em `cadastro_repository`;
- novas validacoes em `cadastro_service`;
- novos menus em `cadastro_ui`;
- auditoria MongoDB para alteracoes, exclusoes e desativacoes;
- testes automatizados para busca, alteracao e exclusao/desativacao;
- atualizacao de README e docs.

## 10. Criterios De Aceite Da Fase 11

- Buscar usuario por CPF retorna o usuario correto.
- Buscar livro por ISBN ou titulo retorna registros compativeis.
- Alterar usuario atualiza dados e preserva o ID.
- Alterar livro/exemplar respeita campos unicos.
- Desativar usuario com historico nao remove o registro fisicamente.
- Excluir autor/editora/genero em uso deve ser bloqueado com mensagem clara.
- Alteracoes bem-sucedidas geram documentos na colecao `auditoria`.
- `mingw32-make test` continua passando.

## 11. Observacoes

As operacoes de busca, alteracao e exclusao/desativacao nao estavam explicitamente previstas na especificacao original. Nesta V2, elas entram como melhoria funcional para tornar o modulo de cadastros mais completo e mais proximo de um CRUD administrativo real.

