param(
    [string]$PostgresHost = $(if ($env:POSTGRES_HOST) { $env:POSTGRES_HOST } else { "localhost" }),
    [string]$PostgresPort = $(if ($env:POSTGRES_PORT) { $env:POSTGRES_PORT } else { "5432" }),
    [string]$PostgresUser = $(if ($env:POSTGRES_USER) { $env:POSTGRES_USER } else { "postgres" }),
    [string]$PostgresDatabase = $(if ($env:POSTGRES_DB) { $env:POSTGRES_DB } else { "smartlibrary" }),
    [string]$MongoDatabase = $(if ($env:MONGODB_DATABASE) { $env:MONGODB_DATABASE } else { "smartlibrary" }),
    [string]$MongoUri = $(if ($env:MONGODB_URI) { $env:MONGODB_URI } else { "mongodb://localhost:27017" }),
    [switch]$SkipBuild,
    [switch]$RecreateDatabase
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent $PSScriptRoot

function Write-Step {
    param([string]$Message)
    Write-Host "[SETUP] $Message"
}

function Require-Command {
    param([string]$Name, [string]$Hint)
    if (-not (Get-Command $Name -ErrorAction SilentlyContinue)) {
        throw "Dependencia ausente: $Name. $Hint"
    }
}

function Find-MongoShell {
    if (Get-Command mongosh -ErrorAction SilentlyContinue) {
        return "mongosh"
    }

    $candidates = @(
        "C:\Program Files\mongosh\mongosh.exe",
        "C:\Program Files\MongoDB\mongosh\bin\mongosh.exe"
    )

    foreach ($candidate in $candidates) {
        if (Test-Path $candidate) {
            return $candidate
        }
    }

    throw "Dependencia ausente: mongosh. Instale o MongoDB Shell ou adicione mongosh ao PATH."
}

function Assert-DatabaseName {
    param([string]$Name, [string]$Label)
    if ($Name -notmatch '^[A-Za-z0-9_]+$') {
        throw "$Label invalido: use apenas letras, numeros e underscore. Valor recebido: $Name"
    }
}
function Invoke-PsqlChecked {
    param([string[]]$Arguments)
    & psql -h $PostgresHost -p $PostgresPort -U $PostgresUser @Arguments
    if ($LASTEXITCODE -ne 0) {
        throw "psql falhou: $($Arguments -join ' ')"
    }
}

function Invoke-PsqlScalar {
    param([string]$Database, [string]$Sql)
    $result = & psql -h $PostgresHost -p $PostgresPort -U $PostgresUser -d $Database -t -A -v ON_ERROR_STOP=1 -c $Sql
    if ($LASTEXITCODE -ne 0) {
        throw "psql falhou ao executar: $Sql"
    }
    return ($result | Select-Object -First 1).Trim()
}

if (-not $env:POSTGRES_PASSWORD) {
    throw "Defina POSTGRES_PASSWORD somente na sessao atual. Exemplo: `$env:POSTGRES_PASSWORD='sua_senha'"
}

$env:PGPASSWORD = $env:POSTGRES_PASSWORD
$env:POSTGRES_HOST = $PostgresHost
$env:POSTGRES_PORT = $PostgresPort
$env:POSTGRES_USER = $PostgresUser
$env:POSTGRES_DB = $PostgresDatabase
$env:MONGODB_DATABASE = $MongoDatabase
$env:MONGODB_URI = $MongoUri

Assert-DatabaseName $PostgresDatabase "POSTGRES_DB"
Assert-DatabaseName $MongoDatabase "MONGODB_DATABASE"

Write-Step "Validando dependencias..."
Require-Command "psql" "Instale PostgreSQL Client ou adicione o diretorio bin ao PATH."
Require-Command "pkg-config" "Instale via MSYS2/UCRT64 e mantenha no PATH."
Require-Command "gcc" "Instale GCC via MSYS2/UCRT64 e mantenha no PATH."
Require-Command "mingw32-make" "Instale mingw32-make via MSYS2/UCRT64 e mantenha no PATH."
$MongoShell = Find-MongoShell

Write-Step "Verificando banco PostgreSQL '$PostgresDatabase'..."
$dbExists = Invoke-PsqlScalar "postgres" "SELECT 1 FROM pg_database WHERE datname = '$PostgresDatabase';"

if ($dbExists -eq "1" -and $RecreateDatabase) {
    Write-Step "Recriando banco PostgreSQL '$PostgresDatabase'..."
    Invoke-PsqlChecked @("-d", "postgres", "-v", "ON_ERROR_STOP=1", "-c", "DROP DATABASE IF EXISTS $PostgresDatabase WITH (FORCE);")
    Invoke-PsqlChecked @("-d", "postgres", "-v", "ON_ERROR_STOP=1", "-c", "CREATE DATABASE $PostgresDatabase;")
    $dbExists = "1"
} elseif ($dbExists -ne "1") {
    Write-Step "Criando banco PostgreSQL '$PostgresDatabase'..."
    Invoke-PsqlChecked @("-d", "postgres", "-v", "ON_ERROR_STOP=1", "-c", "CREATE DATABASE $PostgresDatabase;")
}

$schemaExists = Invoke-PsqlScalar $PostgresDatabase "SELECT CASE WHEN to_regclass('public.perfil') IS NULL THEN 0 ELSE 1 END;"
if ($schemaExists -eq "0" -or $RecreateDatabase) {
    Write-Step "Aplicando scripts PostgreSQL..."
    Invoke-PsqlChecked @("-d", $PostgresDatabase, "-v", "ON_ERROR_STOP=1", "-f", (Join-Path $ProjectRoot "database\postgresql\001_schema.sql"))
    Invoke-PsqlChecked @("-d", $PostgresDatabase, "-v", "ON_ERROR_STOP=1", "-f", (Join-Path $ProjectRoot "database\postgresql\002_constraints.sql"))
    Invoke-PsqlChecked @("-d", $PostgresDatabase, "-v", "ON_ERROR_STOP=1", "-f", (Join-Path $ProjectRoot "database\postgresql\003_indexes.sql"))
    Invoke-PsqlChecked @("-d", $PostgresDatabase, "-v", "ON_ERROR_STOP=1", "-f", (Join-Path $ProjectRoot "database\postgresql\004_seed.sql"))
} else {
    Write-Step "Schema PostgreSQL ja existe; scripts SQL nao foram reaplicados. Use -RecreateDatabase para recriar."
}

Write-Step "Aplicando validators e indexes MongoDB em '$MongoDatabase'..."
& $MongoShell $MongoUri (Join-Path $ProjectRoot "database\mongodb\validators.js")
if ($LASTEXITCODE -ne 0) {
    throw "Falha ao aplicar validators MongoDB."
}
& $MongoShell $MongoUri (Join-Path $ProjectRoot "database\mongodb\indexes.js")
if ($LASTEXITCODE -ne 0) {
    throw "Falha ao aplicar indexes MongoDB."
}

if (-not $SkipBuild) {
    Write-Step "Compilando projeto..."
    Push-Location $ProjectRoot
    try {
        & mingw32-make
        if ($LASTEXITCODE -ne 0) {
            throw "Build falhou com codigo $LASTEXITCODE."
        }
    } finally {
        Pop-Location
    }
}

Write-Step "Setup local concluido. Execute: .\build\smartlibrary.exe"