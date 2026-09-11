# Dicionario De Dados

Este dicionario deve ser atualizado sempre que migrations alterarem tabelas, campos, tipos, constraints, indices, relacionamentos ou valores de status.

## perfil

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_perfil | SERIAL | Sim | PK | automatico | Identificador do perfil |
| nome | VARCHAR(50) | Sim | UNIQUE | - | Nome do perfil |
| descricao | VARCHAR(200) | Nao | - | NULL | Descricao do perfil |

## usuario

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_usuario | SERIAL | Sim | PK | automatico | Identificador |
| nome | VARCHAR(150) | Sim | - | - | Nome completo |
| cpf | VARCHAR(14) | Sim | UNIQUE | - | CPF |
| email | VARCHAR(150) | Nao | UNIQUE | NULL | E-mail |
| telefone | VARCHAR(20) | Nao | - | NULL | Telefone |
| senha_hash | VARCHAR(255) | Sim | - | - | Hash da senha |
| id_perfil | INTEGER | Sim | FK | - | Perfil |
| ativo | BOOLEAN | Sim | - | TRUE | Usuario ativo |
| bloqueado | BOOLEAN | Sim | - | FALSE | Bloqueio operacional |
| data_cadastro | TIMESTAMP | Sim | - | CURRENT_TIMESTAMP | Cadastro |

## autor

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_autor | SERIAL | Sim | PK | automatico | Identificador |
| nome | VARCHAR(150) | Sim | - | - | Nome do autor |
| nacionalidade | VARCHAR(100) | Nao | - | NULL | Nacionalidade |

## editora

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_editora | SERIAL | Sim | PK | automatico | Identificador |
| nome | VARCHAR(150) | Sim | - | - | Nome da editora |
| cidade | VARCHAR(100) | Nao | - | NULL | Cidade |
| pais | VARCHAR(100) | Nao | - | NULL | Pais |

## genero

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_genero | SERIAL | Sim | PK | automatico | Identificador |
| nome | VARCHAR(100) | Sim | UNIQUE | - | Nome do genero |
| descricao | VARCHAR(255) | Nao | - | NULL | Descricao |

## livro

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_livro | SERIAL | Sim | PK | automatico | Identificador |
| isbn | VARCHAR(20) | Nao | UNIQUE | NULL | ISBN |
| titulo | VARCHAR(200) | Sim | - | - | Titulo |
| subtitulo | VARCHAR(200) | Nao | - | NULL | Subtitulo |
| ano_publicacao | INTEGER | Nao | - | NULL | Ano |
| edicao | INTEGER | Nao | - | NULL | Edicao |
| id_editora | INTEGER | Nao | FK | NULL | Editora |
| idioma | VARCHAR(50) | Nao | - | NULL | Idioma |
| descricao | TEXT | Nao | - | NULL | Sinopse ou descricao |

## livro_autor

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_livro | INTEGER | Sim | PK/FK | - | Livro |
| id_autor | INTEGER | Sim | PK/FK | - | Autor |

## livro_genero

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_livro | INTEGER | Sim | PK/FK | - | Livro |
| id_genero | INTEGER | Sim | PK/FK | - | Genero |

## exemplar

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_exemplar | SERIAL | Sim | PK | automatico | Identificador |
| id_livro | INTEGER | Sim | FK | - | Livro |
| codigo_barras | VARCHAR(50) | Sim | UNIQUE | - | Codigo de barras |
| rfid | VARCHAR(100) | Nao | UNIQUE | NULL | Identificacao RFID |
| status | VARCHAR(30) | Sim | - | DISPONIVEL | Estado atual |
| localizacao | VARCHAR(100) | Nao | - | NULL | Localizacao fisica |
| data_aquisicao | DATE | Nao | - | NULL | Data de aquisicao |

Status permitidos: DISPONIVEL, EMPRESTADO, RESERVADO, MANUTENCAO, EXTRAVIADO.

## emprestimo

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_emprestimo | SERIAL | Sim | PK | automatico | Identificador |
| id_usuario | INTEGER | Sim | FK | - | Usuario |
| data_emprestimo | TIMESTAMP | Sim | - | CURRENT_TIMESTAMP | Data da operacao |
| data_prevista_devolucao | DATE | Sim | - | - | Prazo |
| data_encerramento | TIMESTAMP | Nao | - | NULL | Encerramento |
| status | VARCHAR(30) | Sim | - | ABERTO | Estado |
| origem | VARCHAR(30) | Sim | - | - | BALCAO ou SELF_CHECKOUT |

## emprestimo_item

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_emprestimo_item | SERIAL | Sim | PK | automatico | Identificador |
| id_emprestimo | INTEGER | Sim | FK | - | Emprestimo |
| id_exemplar | INTEGER | Sim | FK | - | Exemplar |
| data_devolucao | TIMESTAMP | Nao | - | NULL | Data real da devolucao |
| quantidade_renovacoes | INTEGER | Sim | - | 0 | Numero de renovacoes |

## reserva

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_reserva | SERIAL | Sim | PK | automatico | Identificador |
| id_usuario | INTEGER | Sim | FK | - | Usuario |
| id_livro | INTEGER | Sim | FK | - | Livro |
| data_reserva | TIMESTAMP | Sim | - | CURRENT_TIMESTAMP | Criacao |
| data_expiracao | TIMESTAMP | Nao | - | NULL | Expiracao |
| status | VARCHAR(30) | Sim | - | ATIVA | ATIVA, ATENDIDA, CANCELADA ou EXPIRADA |

## terminal_self_checkout

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_terminal | SERIAL | Sim | PK | automatico | Identificador |
| codigo | VARCHAR(50) | Sim | UNIQUE | - | Codigo do terminal |
| descricao | VARCHAR(150) | Nao | - | NULL | Descricao |
| localizacao | VARCHAR(100) | Nao | - | NULL | Localizacao |
| ativo | BOOLEAN | Sim | - | TRUE | Estado |
| ultima_comunicacao | TIMESTAMP | Nao | - | NULL | Ultima atividade |

## configuracao

| Campo | Tipo | Obrigatorio | Chave | Padrao | Descricao |
| --- | --- | --- | --- | --- | --- |
| id_configuracao | SERIAL | Sim | PK | automatico | Identificador |
| chave | VARCHAR(100) | Sim | UNIQUE | - | Nome da configuracao |
| valor | VARCHAR(255) | Sim | - | - | Valor configurado |
| descricao | VARCHAR(255) | Nao | - | NULL | Descricao da configuracao |
