param(
    [string]$PostgresUser = $(if ($env:POSTGRES_USER) { $env:POSTGRES_USER } else { "postgres" }),
    [string]$PostgresHost = $(if ($env:POSTGRES_HOST) { $env:POSTGRES_HOST } else { "localhost" }),
    [string]$PostgresPort = $(if ($env:POSTGRES_PORT) { $env:POSTGRES_PORT } else { "5432" }),
    [string]$TestDatabase = "smartlibrary_test",
    [string]$MongoDatabase = "smartlibrary_test"
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$Binary = Join-Path $ProjectRoot "build\smartlibrary.exe"
$MongoShell = if (Get-Command mongosh -ErrorAction SilentlyContinue) { "mongosh" } else { "C:\Program Files\mongosh\mongosh.exe" }

function Assert-True {
    param([bool]$Condition, [string]$Message)
    if (-not $Condition) {
        throw "ASSERTION FAILED: $Message"
    }
}

function Invoke-Psql {
    param([string[]]$Arguments)
    & psql -h $PostgresHost -p $PostgresPort -U $PostgresUser @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "psql failed: $($Arguments -join ' ')"
    }
}

function Invoke-PsqlScalar {
    param([string]$Database, [string]$Sql)
    $result = & psql -h $PostgresHost -p $PostgresPort -U $PostgresUser -d $Database -t -A -v ON_ERROR_STOP=1 -c $Sql
    if ($LASTEXITCODE -ne 0) {
        throw "psql scalar failed: $Sql"
    }
    return ($result | Select-Object -First 1).Trim()
}

function Invoke-MongoScalar {
    param([string]$Eval)
    $result = & $MongoShell $MongoDatabase --quiet --eval $Eval
    if ($LASTEXITCODE -ne 0) {
        throw "mongosh failed: $Eval"
    }
    return ($result | Select-Object -Last 1).Trim()
}

if (-not $env:POSTGRES_PASSWORD) {
    throw "Defina POSTGRES_PASSWORD antes de executar os testes. Exemplo: `$env:POSTGRES_PASSWORD='sua_senha'; mingw32-make test"
}

if (-not (Test-Path $Binary)) {
    throw "Binario nao encontrado em $Binary. Execute mingw32-make antes dos testes."
}

$env:PGPASSWORD = $env:POSTGRES_PASSWORD

Write-Host "[TEST] Recriando banco PostgreSQL $TestDatabase..."
Invoke-Psql @("-d", "postgres", "-v", "ON_ERROR_STOP=1", "-c", "DROP DATABASE IF EXISTS $TestDatabase WITH (FORCE);")
Invoke-Psql @("-d", "postgres", "-v", "ON_ERROR_STOP=1", "-c", "CREATE DATABASE $TestDatabase;")

Write-Host "[TEST] Aplicando schema, constraints, indexes e seed..."
Invoke-Psql @("-d", $TestDatabase, "-v", "ON_ERROR_STOP=1", "-f", (Join-Path $ProjectRoot "database\postgresql\001_schema.sql"))
Invoke-Psql @("-d", $TestDatabase, "-v", "ON_ERROR_STOP=1", "-f", (Join-Path $ProjectRoot "database\postgresql\002_constraints.sql"))
Invoke-Psql @("-d", $TestDatabase, "-v", "ON_ERROR_STOP=1", "-f", (Join-Path $ProjectRoot "database\postgresql\003_indexes.sql"))
Invoke-Psql @("-d", $TestDatabase, "-v", "ON_ERROR_STOP=1", "-f", (Join-Path $ProjectRoot "database\postgresql\004_seed.sql"))

Write-Host "[TEST] Inserindo fixtures relacionais..."
$fixtureSql = @"
INSERT INTO usuario (nome, cpf, email, telefone, senha_hash, id_perfil, ativo, bloqueado)
VALUES ('Usuario Teste Fase 9', '900.000.000-01', 'fase9@example.com', '11999990000', 'hash-teste', 1, true, false);

INSERT INTO editora (nome, cidade, pais)
VALUES ('Editora Teste Fase 9', 'Sao Paulo', 'Brasil');

INSERT INTO livro (isbn, titulo, ano_publicacao, id_editora, idioma, descricao)
VALUES ('ISBN-F9-001', 'Livro Teste Fase 9', 2026, 1, 'Portugues', 'Fixture automatizada da Fase 9.');

INSERT INTO exemplar (id_livro, codigo_barras, rfid, status, localizacao)
VALUES (1, 'F9-EX-001', 'RFID-F9-001', 'DISPONIVEL', 'Teste');
"@
Invoke-Psql @("-d", $TestDatabase, "-v", "ON_ERROR_STOP=1", "-c", $fixtureSql)

Write-Host "[TEST] Recriando banco MongoDB $MongoDatabase..."
& $MongoShell $MongoDatabase --quiet --eval "db.dropDatabase()" | Out-Null
if ($LASTEXITCODE -ne 0) {
    throw "mongosh nao conseguiu recriar o banco $MongoDatabase"
}

Write-Host "[TEST] Executando fluxo automatizado pela aplicacao..."
$env:POSTGRES_HOST = $PostgresHost
$env:POSTGRES_PORT = $PostgresPort
$env:POSTGRES_DB = $TestDatabase
$env:POSTGRES_USER = $PostgresUser
$env:MONGODB_DATABASE = $MongoDatabase
$inputData = "2`n1`n1`nF9-EX-001`n2`nF9-EX-001`n0`n6`n1`n4`n3`n5`n0`n5`n1`n10`n0`n0`n"
$output = $inputData | & $Binary
if ($LASTEXITCODE -ne 0) {
    $output | Write-Host
    throw "Aplicacao retornou codigo $LASTEXITCODE"
}

Assert-True ($output -join "`n").Contains("[OK] Emprestimo realizado") "Fluxo de emprestimo nao confirmou sucesso."
Assert-True ($output -join "`n").Contains("[OK] Devolucao realizada") "Fluxo de devolucao nao confirmou sucesso."
Assert-True ($output -join "`n").Contains("RELATORIOS") "Menu de relatorios nao foi exercitado."
Assert-True ($output -join "`n").Contains("NOSQL - EVENTOS, LOGS E AUDITORIA") "Menu NoSQL nao foi exercitado."

Write-Host "[TEST] Validando estado PostgreSQL..."
$loanState = Invoke-PsqlScalar $TestDatabase "SELECT origem || ':' || status FROM emprestimo ORDER BY id_emprestimo DESC LIMIT 1;"
Assert-True ($loanState -eq "BALCAO:ENCERRADO") "Emprestimo esperado como BALCAO:ENCERRADO, obtido $loanState."

$copyStatus = Invoke-PsqlScalar $TestDatabase "SELECT status FROM exemplar WHERE codigo_barras = 'F9-EX-001';"
Assert-True ($copyStatus -eq "DISPONIVEL") "Exemplar deveria estar DISPONIVEL apos devolucao, obtido $copyStatus."

$reportRows = Invoke-PsqlScalar $TestDatabase "SELECT COUNT(*) FROM livro l LEFT JOIN exemplar e ON e.id_livro = l.id_livro;"
Assert-True ([int]$reportRows -gt 0) "Relatorio de acervo nao tem dados-base."

Write-Host "[TEST] Validando documentos MongoDB..."
$eventCount = Invoke-MongoScalar "db.eventos.countDocuments({tipo:'EMPRESTIMO_REALIZADO', origem:'BALCAO'})"
Assert-True ([int]$eventCount -ge 1) "Evento EMPRESTIMO_REALIZADO/BALCAO nao foi registrado."

$auditCount = Invoke-MongoScalar "db.auditoria.countDocuments({entidade:'emprestimo'})"
Assert-True ([int]$auditCount -ge 1) "Auditoria de emprestimo nao foi registrada."

$logCount = Invoke-MongoScalar "db.logs.countDocuments({componente:'main'})"
Assert-True ([int]$logCount -ge 1) "Logs de sistema nao foram registrados."

Write-Host "[TEST] Fase 9 OK: build, fluxo, relatorios, PostgreSQL e MongoDB validados."
