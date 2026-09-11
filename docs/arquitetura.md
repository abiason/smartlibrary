# Arquitetura

O SmartLibrary segue uma arquitetura em camadas para manter separacao clara entre interface, regras de negocio e persistencia.

## Camadas

- UI: menus e interacao com usuarios.
- Services: regras de negocio e coordenacao de operacoes.
- Repositories: consultas SQL e persistencia relacional.
- Database: conexoes com PostgreSQL e MongoDB.
- Config: leitura de configuracoes do ambiente.
- Events: registro de eventos, logs e auditoria no MongoDB.

## Decisoes Das Fases 1 A 5

- PostgreSQL e a fonte principal da verdade transacional.
- MongoDB fica reservado para eventos, logs, auditoria e telemetria.
- A configuracao inicial usa variaveis de ambiente com valores padrao seguros para desenvolvimento local.
- As conexoes sao abertas na inicializacao e encerradas antes do fim do processo.
- O schema relacional, o modelo documental, o MER e o dicionario de dados foram criados em conjunto.
- A Fase 3 adiciona UI administrativa, services e repositories para cadastros basicos.
- A Fase 4 adiciona emprestimos, devolucoes e renovacoes transacionais.
- A Fase 5 adiciona reservas e integra bloqueio de emprestimo/renovacao por reserva ativa de outro usuario.
- Nenhuma regra de Self Checkout completo ou relatorio foi implementada nesta fase.

## Diagramas

- [C4 Contexto](diagrams/c4_contexto.md)
- [C4 Containers](diagrams/c4_containers.md)
- [C4 Componentes](diagrams/c4_componentes.md)
- [MER](diagrams/modelo_er.md)
- [Arquitetura Geral](diagrams/arquitetura_geral.md)
- [Fluxo de Emprestimo](diagrams/fluxo_emprestimo.md)
- [Fluxo de Devolucao](diagrams/fluxo_devolucao.md)
- [Fluxo de Renovacao](diagrams/fluxo_renovacao.md)
- [Fluxo de Reserva](diagrams/fluxo_reserva.md)
- [Sequencia de Emprestimo](diagrams/sequencia_emprestimo.md)
- [Sequencia de Devolucao](diagrams/sequencia_devolucao.md)
- [Sequencia de Renovacao](diagrams/sequencia_renovacao.md)
- [Sequencia de Reserva](diagrams/sequencia_reserva.md)
