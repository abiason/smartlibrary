# SmartLibrary - Especificacao V5

## 1. Contexto

A V5 representa a ultima melhoria planejada para o SmartLibrary. O objetivo nao e ampliar o escopo funcional principal, mas elevar a qualidade de uso e apresentacao da aplicacao console.

Mesmo sendo uma aplicacao em C executada no terminal, o sistema deve transmitir organizacao, clareza e maturidade visual. A V5 sera focada em padronizar telas, menus, mensagens, formularios e listagens.

## 2. Objetivo Geral

Melhorar o aspecto e a disposicao da interface com o usuario, mantendo o SmartLibrary como uma aplicacao console em C17, compativel com Windows e com o fluxo atual do projeto.

## 3. Objetivos Especificos

- Criar uma base visual reutilizavel para a interface console.
- Padronizar cabecalhos, rodapes, menus e mensagens.
- Melhorar a legibilidade de formularios e listagens.
- Reduzir poluicao visual e inconsistencias entre telas.
- Preservar as regras de negocio, os acessos ao banco e os fluxos ja implementados.
- Manter o projeto compilando e com testes passando.

## 4. Escopo da V5

### 4.1 Base Visual Console

Criar um modulo dedicado para elementos comuns de interface:

- `src/ui/console_ui.c`
- `src/ui/console_ui.h`

Funcoes previstas:

- Limpeza de tela.
- Cabecalho padronizado.
- Subtitulo/contexto do modulo.
- Impressao de itens de menu.
- Rodape de navegacao.
- Mensagens de sucesso, erro, aviso e informacao.
- Separadores horizontais.
- Apoio a tabelas simples.

### 4.2 Menus Padronizados

Aplicar o novo padrao visual nas principais telas:

- Primeiro acesso.
- Login.
- Menu principal.
- Cadastros administrativos.
- Circulacao.
- Reservas.
- Self Checkout.
- NoSQL: eventos, logs e auditoria.
- Relatorios.
- Troca de senha.

### 4.3 Formularios e Entradas

Melhorar a organizacao visual de entradas de dados:

- Rotulos alinhados.
- Indicao clara de cancelamento quando aplicavel.
- Agrupamento por contexto.
- Mensagens de validacao mais legiveis.

### 4.4 Mensagens de Feedback

Padronizar comunicacoes com o usuario:

- Sucesso.
- Erro.
- Aviso.
- Informacao.
- Acesso negado.
- Operacao cancelada ou encerrada.

### 4.5 Listagens e Relatorios

Melhorar a apresentacao de dados tabulares:

- Cabecalhos mais legiveis.
- Separadores consistentes.
- Colunas com largura previsivel.
- Tratamento visual para ausencia de registros.

## 5. Fora de Escopo

- Criacao de interface grafica desktop.
- Criacao de interface web.
- Mudanca do banco de dados.
- Alteracao das regras centrais de negocio.
- Inclusao de novas funcionalidades funcionais alem do polimento de interface.
- Uso obrigatorio de cores no terminal.

## 6. Proposta Visual

### 6.1 Menu Principal

```text
+------------------------------------------------------------+
| SMARTLIBRARY                               v5.0 | ADMIN    |
+------------------------------------------------------------+
| Operador: Administrador do Sistema                         |
| Modulo: Menu Principal                                     |
+------------------------------------------------------------+

  [1] Cadastros administrativos
  [2] Circulacao
  [3] Reservas
  [4] Self Checkout
  [5] Eventos, logs e auditoria
  [6] Relatorios
  [7] Trocar senha

  [0] Sair

+------------------------------------------------------------+
Escolha uma opcao: _
```

### 6.2 Formulario

```text
+------------------------------------------------------------+
| CADASTROS > NOVO USUARIO                                   |
+------------------------------------------------------------+

  Nome completo : _
  CPF           :
  E-mail        :
  Perfil        :

+------------------------------------------------------------+
[0] cancelar
```

### 6.3 Mensagem

```text
+------------------------------------------------------------+
| SUCESSO                                                    |
+------------------------------------------------------------+
| Usuario cadastrado com sucesso.                            |
+------------------------------------------------------------+
Pressione Enter para continuar...
```

### 6.4 Tabela

```text
+----+---------------------------+----------------+----------+
| ID | Nome                      | CPF            | Perfil   |
+----+---------------------------+----------------+----------+
| 1  | Administrador do Sistema  | 000.000.000-00 | ADMIN    |
+----+---------------------------+----------------+----------+
```

## 7. Plano de Execucao

### Fase 16 - Base Visual da Interface Console

- Criar modulo `console_ui`.
- Centralizar funcoes de cabecalho, separador, menu e mensagens.
- Integrar a limpeza de tela existente ao novo padrao visual.

### Fase 17 - Padronizacao dos Menus

- Atualizar menus principais e secundarios.
- Aplicar cabecalhos consistentes.
- Melhorar labels de opcoes.
- Preservar permissoes e fluxos atuais.

### Fase 18 - Formularios e Mensagens

- Padronizar prompts de entrada.
- Melhorar mensagens de erro, sucesso e informacao.
- Reduzir mensagens tecnicas para o usuario final.

### Fase 19 - Listagens e Relatorios

- Melhorar tabelas simples exibidas no console.
- Padronizar mensagens para listas vazias.
- Revisar listagens de cadastros, emprestimos, reservas e relatorios.

### Fase 20 - Fechamento da V5

- Atualizar documentacao final.
- Atualizar roteiro ou observacoes de apresentacao, se necessario.
- Executar setup/testes.
- Publicar no GitHub.

## 8. Criterios de Aceite

- Todos os menus principais seguem o mesmo padrao visual.
- As telas deixam de usar cabecalhos antigos baseados apenas em linhas de `=`.
- Mensagens importantes sao exibidas por helpers padronizados.
- Listagens ficam mais legiveis no terminal.
- A aplicacao continua console, em C17.
- O projeto continua compilando.
- Os testes continuam passando.
- A documentacao final referencia a V5 como ultima melhoria planejada.

## 9. Resultado Esperado

Ao final da V5, o SmartLibrary deve parecer mais profissional durante a execucao, com telas mais organizadas, menus consistentes e melhor experiencia para o usuario, sem alterar a proposta academica original nem descaracterizar a aplicacao console em C.

## 10. Status Final

A V5 foi concluida como a ultima melhoria planejada do SmartLibrary.

| Fase | Entrega | Status |
| --- | --- | --- |
| 16 | Base visual da interface console | Concluida |
| 17 | Padronizacao dos menus | Concluida |
| 18 | Formularios e mensagens | Concluida |
| 19 | Tabelas e listagens | Concluida |
| 20 | Documentacao final e validacao geral | Concluida |

O fechamento preserva a aplicacao como console em C17, sem introduzir interface grafica, dependencia de cores no terminal ou mudancas nas regras centrais de negocio.
