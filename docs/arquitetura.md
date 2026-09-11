# Arquitetura

O SmartLibrary segue uma arquitetura em camadas para manter separacao clara entre interface, regras de negocio e persistencia.

## Camadas

- UI: menus e interacao com usuarios.
- Services: regras de negocio e coordenacao de operacoes.
- Repositories: consultas SQL e persistencia relacional.
- Database: conexoes com PostgreSQL e MongoDB.
- Config: leitura de configuracoes do ambiente.
- Events: registro de eventos, logs e auditoria no MongoDB.

## Decisoes Da Fase 1

- PostgreSQL e a fonte principal da verdade transacional.
- MongoDB fica reservado para eventos, logs, auditoria e telemetria.
- A configuracao inicial usa variaveis de ambiente com valores padrao seguros para desenvolvimento local.
- As conexoes sao abertas na inicializacao e encerradas antes do fim do processo.
- Nenhuma regra de cadastro, emprestimo, reserva, Self Checkout completo ou relatorio foi implementada nesta fase.

## Diagramas

- [C4 Contexto](diagrams/c4_contexto.md)
- [C4 Containers](diagrams/c4_containers.md)
- [Arquitetura Geral](diagrams/arquitetura_geral.md)
