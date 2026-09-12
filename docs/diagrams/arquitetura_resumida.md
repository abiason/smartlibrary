# SmartLibrary - Arquitetura Resumida

```mermaid
flowchart LR
    USUARIOS["Usuarios\nAluno | Bibliotecario | Admin"]

    subgraph APP["Aplicacao C - SmartLibrary"]
        UI["Interface Console"]
        AUTH["Autenticacao e Perfis"]
        CAD["Cadastros\nUsuarios | Livros | Exemplares"]
        CIRC["Circulacao\nEmprestimos | Reservas | Renovacoes | Devolucoes"]
        SELF["Self Checkout"]
        REL["Relatorios"]
        AUD["Auditoria e Eventos"]
        REPO["Camada de Repositorios"]
    end

    PG[("PostgreSQL\nDados transacionais")]
    MDB[("MongoDB\nLogs, eventos e auditoria")]

    USUARIOS --> UI
    UI --> AUTH
    AUTH --> CAD
    AUTH --> CIRC
    AUTH --> SELF
    AUTH --> REL

    CAD --> REPO
    CIRC --> REPO
    SELF --> REPO
    REL --> REPO
    AUTH --> REPO

    REPO --> PG

    AUTH --> AUD
    CAD --> AUD
    CIRC --> AUD
    SELF --> AUD
    REL --> AUD
    AUD --> MDB
```

## Leitura Rapida

- A aplicacao principal e escrita em C e exposta por interface console.
- O PostgreSQL concentra os dados transacionais: usuarios, perfis, livros, exemplares, emprestimos, reservas e devolucoes.
- O MongoDB registra eventos operacionais, logs e trilhas de auditoria.
- A camada de repositorios isola o acesso ao banco relacional.
- O modulo de auditoria recebe eventos dos principais fluxos do sistema.
