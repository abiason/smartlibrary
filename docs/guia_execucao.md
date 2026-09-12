# Guia De Execucao

Este guia resume o caminho para preparar, testar e executar o SmartLibrary localmente.

## 1. Dependencias

Confirme que os comandos abaixo respondem no terminal:

```powershell
gcc --version
pkg-config --version
psql --version
mongosh --version
mingw32-make --version
```

No ambiente Windows validado, o MongoDB Server esta instalado separadamente e o `mongosh` pode estar em `C:\Program Files\mongosh\mongosh.exe`.

## 2. Variaveis De Ambiente

Defina a senha do PostgreSQL apenas na sessao atual:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
```

Opcionalmente ajuste host, porta, usuario e bancos:

```powershell
$env:POSTGRES_HOST='localhost'
$env:POSTGRES_PORT='5432'
$env:POSTGRES_DB='smartlibrary'
$env:POSTGRES_USER='postgres'
$env:MONGODB_URI='mongodb://localhost:27017'
$env:MONGODB_DATABASE='smartlibrary'
```

## 3. Setup Automatizado

O caminho recomendado para uma maquina local nova e executar o script de setup:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
PowerShell -ExecutionPolicy Bypass -File scripts/setup_local.ps1
```

O script valida `psql`, `mongosh`, `gcc`, `pkg-config` e `mingw32-make`, cria o banco PostgreSQL principal quando ele nao existe, aplica os scripts SQL em ambiente limpo, aplica validators/indexes MongoDB e compila o projeto.

Se o banco principal ja existir com schema aplicado, os scripts SQL nao sao reaplicados. Para recriar o banco principal, use:

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
PowerShell -ExecutionPolicy Bypass -File scripts/setup_local.ps1 -RecreateDatabase
```

Use `-RecreateDatabase` apenas quando puder apagar o banco principal local.

## 4. Criar Banco PostgreSQL Manualmente

```powershell
$env:PGPASSWORD=$env:POSTGRES_PASSWORD
createdb -U postgres smartlibrary
psql -U postgres -d smartlibrary -f database/postgresql/001_schema.sql
psql -U postgres -d smartlibrary -f database/postgresql/002_constraints.sql
psql -U postgres -d smartlibrary -f database/postgresql/003_indexes.sql
psql -U postgres -d smartlibrary -f database/postgresql/004_seed.sql
```

Se o banco ja existir, aplique apenas os scripts ainda nao aplicados ou recrie o ambiente manualmente conforme a necessidade.

## 5. Preparar MongoDB

```powershell
mongosh database/mongodb/validators.js
mongosh database/mongodb/indexes.js
```

Esses scripts criam ou atualizam as colecoes `eventos`, `logs` e `auditoria`, alem dos indices usados pelas consultas.

## 6. Compilar

```powershell
mingw32-make clean
mingw32-make
```

## 7. Testar

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
mingw32-make test
```

A rotina de testes usa bancos isolados `smartlibrary_test`, sem depender do banco principal.

## 8. Executar

```powershell
$env:POSTGRES_PASSWORD='sua_senha'
.\build\smartlibrary.exe
```

Fluxo simples para demonstracao:

1. Entre em `Cadastros administrativos` e crie usuario, editora, livro e exemplar.
2. Entre em `Circulacao` e realize emprestimo/devolucao.
3. Entre em `Reservas` e crie/cancele uma reserva.
4. Entre em `Self Checkout` e identifique o usuario por CPF.
5. Entre em `NoSQL` e consulte eventos, logs e auditoria.
6. Entre em `Relatorios` e consulte acervo, origens e ranking.

## 9. Primeiro Acesso E Perfis

Ao iniciar a aplicacao, o sistema exige login. Se a tabela `usuario` estiver vazia, sera aberto um fluxo de primeiro acesso para criar o administrador inicial. Se existirem usuarios, mas nenhum administrador ativo e desbloqueado, o mesmo fluxo orienta a criacao de um administrador de recuperacao.

A senha do administrador inicial nao fica versionada no repositorio. Ela e informada localmente pelo operador e gravada no PostgreSQL com hash BCrypt via `pgcrypto`.

Permissoes atuais:

- `ADMINISTRADOR`: cadastros, circulacao, reservas, self checkout, NoSQL e relatorios.
- `BIBLIOTECARIO`: circulacao, reservas e relatorios.
- `USUARIO`: self checkout.
