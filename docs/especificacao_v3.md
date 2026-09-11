# SmartLibrary - Especificacao V3

## 1. Visao Geral

O SmartLibrary e um sistema academico de biblioteca desenvolvido em C17, com persistencia relacional em PostgreSQL e persistencia documental em MongoDB. A aplicacao oferece interface em terminal para administracao de acervo, circulacao de exemplares, reservas, Self Checkout, consultas NoSQL, relatorios operacionais, autenticacao, controle por perfil, auditoria profissional e testes automatizados.

A Especificacao V3 consolida o escopo final implementado ate a Fase 15. Ela incorpora a especificacao original, a melhoria V2 de cadastros e as evolucoes finais de auditoria, seguranca, regras de negocio, testes e documentacao de avaliacao.

## 2. Objetivos

- Controlar o acervo de uma biblioteca academica.
- Gerenciar usuarios, autores, editoras, generos, livros e exemplares.
- Permitir manutencao completa de cadastros: criar, listar, buscar, alterar e excluir/desativar.
- Associar livros a autores e generos.
- Registrar emprestimos, devolucoes e renovacoes com regras transacionais.
- Controlar reservas e impedir conflitos de circulacao.
- Permitir autoatendimento via Self Checkout.
- Exigir login antes do uso da aplicacao.
- Controlar permissoes por perfil de operador.
- Armazenar senhas de usuarios com hash BCrypt via PostgreSQL `pgcrypto`.
- Registrar eventos, logs e auditoria no MongoDB.
- Registrar auditoria profissional com snapshots reais antes/depois.
- Disponibilizar relatorios operacionais.
- Validar o sistema com testes automatizados integrados.
- Entregar documentacao, roteiro de apresentacao, checklist e evidencias de avaliacao.

## 3. Tecnologias

- Linguagem: C17.
- Banco relacional: PostgreSQL.
- Extensao PostgreSQL: `pgcrypto` para hash BCrypt de senhas.
- Driver PostgreSQL: libpq.
- Banco documental: MongoDB.
- Driver MongoDB: MongoDB C Driver.
- Build: Makefile, com suporte a `mingw32-make` no Windows/MSYS2.
- Testes: script PowerShell integrado ao Makefile.
- Documentacao: Markdown e Mermaid.
- Versionamento: Git e GitHub.

## 4. Arquitetura

O sistema segue arquitetura em camadas:

- `auth`: autenticacao, sessao do operador e verificacao de perfil.
- `ui`: menus e interacao com o usuario via terminal.
- `services`: regras de negocio, validacoes e coordenacao de operacoes.
- `repositories`: acesso a dados, SQL parametrizado e consultas transacionais.
- `database`: conexoes com PostgreSQL e MongoDB.
- `events`: eventos, logs, auditoria e consultas NoSQL.
- `models`: structs e conversores de entidades.
- `utils`: entrada de dados, pausa e limpeza de tela.
- `tests`: preparacao de ambiente, fixtures e validacoes automatizadas.
- `docs`: documentacao tecnica, roteiro e evidencias.

## 5. Modelo Relacional

O PostgreSQL e a fonte da verdade para dados operacionais e transacionais.

Entidades principais:

- `perfil`;
- `usuario`;
- `autor`;
- `editora`;
- `genero`;
- `livro`;
- `livro_autor`;
- `livro_genero`;
- `exemplar`;
- `emprestimo`;
- `emprestimo_item`;
- `reserva`;
- `terminal_self_checkout`;
- `configuracao`.

Regras relacionais:

- CPF de usuario deve ser unico.
- E-mail de usuario deve ser unico quando informado.
- Nome de perfil deve ser unico.
- Nome de genero deve ser unico.
- ISBN de livro deve ser unico quando informado.
- Codigo de barras de exemplar deve ser unico.
- RFID de exemplar deve ser unico quando informado.
- Livros podem se relacionar com autores por `livro_autor`.
- Livros podem se relacionar com generos por `livro_genero`.
- Emprestimos podem ter origem `BALCAO` ou `SELF_CHECKOUT`.
- Status de exemplar: `DISPONIVEL`, `EMPRESTADO`, `RESERVADO`, `MANUTENCAO`, `EXTRAVIADO`.
- Status de emprestimo: `ABERTO`, `ENCERRADO`, `CANCELADO`.
- Status de reserva: `ATIVA`, `ATENDIDA`, `CANCELADA`, `EXPIRADA`.

## 6. Modelo NoSQL

O MongoDB armazena dados complementares de observabilidade, historico documental e auditoria.

### 6.1 Colecao `eventos`

Registra eventos de dominio, circulacao, reservas e Self Checkout.

Exemplos:

- `EMPRESTIMO_REALIZADO`;
- `EMPRESTIMO_NEGADO`;
- `DEVOLUCAO_REALIZADA`;
- `RENOVACAO_REALIZADA`;
- `RESERVA_REALIZADA`;
- `SELF_CHECKOUT_INICIADO`;
- `USUARIO_IDENTIFICADO`.

Campos relevantes:

- tipo;
- data/hora;
- usuario;
- emprestimo;
- origem;
- exemplar.

### 6.2 Colecao `logs`

Registra diagnosticos e eventos operacionais da aplicacao.

Campos relevantes:

- nivel;
- data/hora;
- componente;
- terminal;
- mensagem;
- detalhes opcionais.

### 6.3 Colecao `auditoria`

Registra alteracoes relevantes nas entidades.

Campos relevantes:

- entidade;
- entidadeId;
- acao;
- usuarioId;
- dataHora;
- antes;
- depois.

A auditoria de cadastros deve armazenar snapshots reais antes/depois produzidos pelo PostgreSQL em formato JSON e gravados como objeto BSON quando validos. Em exclusoes fisicas, `depois` deve ficar nulo. Em desativacoes ou alteracoes de status, `depois` deve refletir o novo estado persistido.

## 7. Perfis E Permissoes

O sistema exige login antes de abrir o menu principal.

Perfis:

- `ADMINISTRADOR`: acesso completo.
- `BIBLIOTECARIO`: acesso a circulacao, reservas e relatorios.
- `USUARIO`: acesso ao Self Checkout.

Regras:

- Usuario inativo nao pode autenticar.
- Usuario bloqueado nao pode autenticar.
- Senhas devem ser verificadas por hash BCrypt usando `crypt()` e `gen_salt('bf')` do PostgreSQL `pgcrypto`.
- Se nao houver usuarios cadastrados, a aplicacao deve permitir criar o administrador inicial por fluxo de primeiro acesso.
- O repositorio nao deve versionar senha real de PostgreSQL nem senha administrativa fixa.

## 8. Requisitos Funcionais

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
- livro por ID, ISBN, parte do titulo, editora, autor ou genero;
- exemplar por ID, codigo de barras, RFID ou parte do titulo do livro.

A busca deve exibir registros encontrados em formato tabular no terminal.

### RF04 - Alteracao De Cadastros

O sistema deve permitir alterar dados cadastrais das entidades administrativas.

Campos editaveis:

- usuario: nome, email, telefone, perfil, ativo e bloqueado;
- autor: nome e nacionalidade;
- editora: nome, cidade e pais;
- genero: nome e descricao;
- livro: ISBN, titulo, subtitulo, ano, edicao, editora, idioma e descricao;
- exemplar: codigo de barras, RFID, status e localizacao.

Regras:

- Nao permitir duplicidade em campos unicos.
- Nao alterar IDs primarios.
- Nao alterar CPF e senha pelo fluxo comum de alteracao de usuario.
- Registrar auditoria antes/depois no MongoDB para cada alteracao bem-sucedida.
- Exibir mensagem clara em caso de erro ou violacao de constraint.

### RF05 - Exclusao Ou Desativacao De Cadastros

O sistema deve permitir remover ou desativar registros administrativos, respeitando relacionamentos existentes.

Regras:

- Usuario com historico de emprestimos ou reservas deve ser desativado, nao excluido fisicamente.
- Exemplar com historico de emprestimo deve ser movido para status operacional adequado, como `MANUTENCAO`.
- Exemplar sem historico pode ser excluido.
- Livro so pode ser excluido se nao possuir exemplares ou reservas vinculadas.
- Autor, editora e genero so podem ser excluidos se nao estiverem associados a livros.
- Toda exclusao, desativacao ou alteracao de status deve gerar auditoria no MongoDB.

### RF06 - Vinculos Bibliograficos

O sistema deve permitir:

- vincular livro a autor;
- desvincular livro de autor;
- vincular livro a genero;
- desvincular livro de genero.

Regras:

- Livro, autor e genero devem existir.
- V?nculos duplicados nao devem ser criados.
- Operacoes bem-sucedidas devem gerar auditoria do livro alterado.

### RF07 - Emprestimos

O sistema deve permitir realizar emprestimo de exemplar disponivel para usuario ativo e sem bloqueios.

Regras:

- Usuario deve existir, estar ativo e nao bloqueado.
- Usuario nao pode possuir emprestimo atrasado.
- Usuario nao pode exceder o limite de exemplares simultaneos.
- Exemplar deve estar disponivel.
- Livro nao pode estar reservado para outro usuario.
- Operacao deve ser transacional.
- Evento deve ser registrado no MongoDB com origem da operacao.

### RF08 - Devolucoes

O sistema deve permitir devolver exemplar emprestado.

Regras:

- Exemplar deve existir.
- Exemplar deve estar emprestado.
- Deve existir emprestimo aberto associado.
- Exemplar deve voltar para `DISPONIVEL` ao final da devolucao.
- Emprestimo deve ser encerrado quando todos os itens forem devolvidos.
- Evento deve ser registrado no MongoDB.

### RF09 - Renovacoes

O sistema deve permitir renovar item de emprestimo aberto.

Regras:

- Item deve pertencer a emprestimo aberto.
- Item nao pode ter sido devolvido.
- Quantidade maxima de renovacoes deve ser respeitada.
- Renovacao deve ser bloqueada se houver reserva ativa de outro usuario para o livro.

### RF10 - Reservas

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

### RF11 - Self Checkout

O sistema deve permitir autoatendimento com identificacao por CPF.

Funcionalidades:

- identificar usuario;
- realizar emprestimo por codigo de barras ou RFID;
- realizar devolucao por codigo de barras ou RFID;
- renovar emprestimo do usuario identificado;
- consultar emprestimos;
- consultar reservas;
- pesquisar livros.

Eventos gerados no Self Checkout devem registrar origem `SELF_CHECKOUT`.

### RF12 - Eventos, Logs E Auditoria

O sistema deve registrar no MongoDB:

- eventos de circulacao;
- eventos de reserva;
- eventos de Self Checkout;
- logs de sistema;
- logs de autenticacao;
- auditoria de alteracoes relevantes.

### RF13 - Consultas NoSQL

O sistema deve permitir consultar pela aplicacao:

- eventos recentes;
- logs recentes;
- auditoria recente;
- resumo de eventos por tipo.

### RF14 - Relatorios

O sistema deve disponibilizar relatorios:

- acervo por status;
- emprestimos atrasados;
- livros mais emprestados;
- emprestimos por origem;
- reservas ativas;
- usuarios com pendencias.

### RF15 - Validacoes De Negocio

O sistema deve validar:

- CPF com digitos verificadores;
- e-mail quando informado;
- titulo obrigatorio de livro;
- ano de publicacao entre 1450 e 2100 quando informado;
- edicao de livro nao negativa;
- impedimento de alteracao manual de exemplar com emprestimo aberto.

### RF16 - Testes Automatizados

O sistema deve possuir rotina automatizada para validar:

- build;
- schema PostgreSQL;
- fixtures;
- login e permissoes;
- cadastros V2;
- regras reforcadas;
- fluxo de emprestimo/devolucao;
- relatorios;
- eventos, logs e auditoria no MongoDB;
- snapshots antes/depois da auditoria profissional.

### RF17 - Documentacao De Avaliacao

O sistema deve entregar documentos de apoio:

- README de execucao;
- guia de execucao;
- entrega final;
- resumo das fases;
- roteiro de apresentacao;
- checklist de avaliacao;
- matriz de evidencias de teste;
- especificacao consolidada V3.

## 9. Requisitos Nao Funcionais

- Usar C17.
- Compilar pelo Makefile.
- Usar SQL parametrizado para entradas externas.
- Evitar versionar senhas reais.
- Nao versionar credencial administrativa fixa.
- Armazenar senhas de usuarios como hash BCrypt.
- Separar responsabilidades por camadas.
- Manter operacoes criticas em transacoes.
- Usar PostgreSQL como fonte transacional principal.
- Usar MongoDB para eventos, logs e auditoria.
- Tratar falhas de MongoDB sem desfazer operacoes relacionais ja confirmadas.
- Manter documentacao em Markdown.
- Manter scripts de banco versionados.
- Permitir execucao automatizada de testes em bancos isolados.

## 10. Plano Incremental V3

| Fase | Nome | Entrega Principal |
| --- | --- | --- |
| 1 | Fundacao | Estrutura C17, Makefile, configuracao e conexoes |
| 2 | Modelagem | Schema PostgreSQL, MongoDB, indexes, structs e diagramas |
| 3 | Cadastros Basicos | Criacao e listagem de entidades administrativas |
| 4 | Circulacao | Emprestimos, devolucoes, renovacoes e transacoes |
| 5 | Reservas | Criacao, cancelamento, atendimento, expiracao e integracao |
| 6 | Self Checkout | Autoatendimento com CPF, codigo de barras e RFID |
| 7 | NoSQL | Eventos, logs, auditoria e consultas documentais |
| 8 | Relatorios | Relatorios operacionais |
| 9 | Testes | Testes integrados com bancos isolados |
| 10 | Documentacao Final | README, guia, entrega final e arquitetura |
| 11 | Cadastros V2 | Busca, alteracao e exclusao/desativacao |
| 12 | Auditoria Profissional | Snapshots antes/depois reais no MongoDB |
| 13 | Seguranca E Perfis | Login, BCrypt, primeiro acesso e permissoes |
| 14 | Regras De Negocio | CPF/e-mail, busca avancada, vinculos e bloqueios |
| 15 | Entrega Premium | Roteiro, checklist e evidencias finais |

## 11. Criterios De Aceite

- O projeto compila com `mingw32-make`.
- O comando `mingw32-make test` termina com codigo zero.
- O teste recria bancos isolados no PostgreSQL e MongoDB.
- O login administrativo funciona com senha validada por BCrypt.
- Bibliotecario e bloqueado ao tentar acessar cadastros administrativos.
- Usuario com CPF invalido nao e cadastrado.
- Usuario pode ser buscado, alterado e desativado.
- Auditoria de usuario registra `antes.nome`, `depois.nome` e `depois.ativo` corretamente.
- Exemplar emprestado nao pode ser alterado manualmente.
- Livro vinculado a exemplar nao pode ser excluido.
- Livro pode ser vinculado a autor e genero.
- Busca de livro por autor retorna o livro esperado.
- Emprestimo e devolucao funcionam e deixam o exemplar disponivel ao final.
- Eventos, logs e auditoria sao persistidos no MongoDB.
- Relatorios e consultas NoSQL sao acessiveis pela aplicacao.
- Documentos finais de apresentacao e avaliacao estao presentes em `docs/`.

## 12. Evidencias De Entrega

Arquivos principais:

- `README.md`;
- `docs/entrega_final.md`;
- `docs/fases.md`;
- `docs/cadastros.md`;
- `docs/testes.md`;
- `docs/evidencias_testes.md`;
- `docs/roteiro_apresentacao.md`;
- `docs/checklist_avaliacao.md`;
- `tests/run_phase9_tests.ps1`.

Comando de validacao:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

Resultado esperado:

```text
[TEST] OK: build, login/perfis, regras Fase 14, cadastros V2, fluxo, relatorios, PostgreSQL e MongoDB validados.
```

## 13. Observacoes Finais

As operacoes de busca, alteracao e exclusao/desativacao nao estavam explicitamente previstas na especificacao original e foram incorporadas na V2. A V3 consolida essa evolucao e adiciona os requisitos efetivamente implementados nas fases 12 a 15: auditoria antes/depois, login, permissoes, senhas com hash, regras reforcadas, vinculos bibliograficos e documentacao final de avaliacao.

Esta especificacao representa o estado final planejado para a entrega premium do SmartLibrary.