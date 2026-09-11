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
INSERT INTO usuario (id_usuario, nome, cpf, email, telefone, senha_hash, id_perfil, ativo, bloqueado)
VALUES (9000, 'Administrador Teste Fase 13', '000.000.000-00', 'admin.fase13@example.com', '', crypt('admin-fase13', gen_salt('bf')), 3, true, false);

INSERT INTO usuario (id_usuario, nome, cpf, email, telefone, senha_hash, id_perfil, ativo, bloqueado)
VALUES (9001, 'Bibliotecario Teste Fase 13', '111.111.111-11', 'bibliotecario.fase13@example.com', '', crypt('biblio-fase13', gen_salt('bf')), 2, true, false);

INSERT INTO usuario (nome, cpf, email, telefone, senha_hash, id_perfil, ativo, bloqueado)
VALUES ('Usuario Teste Fase 9', '900.000.000-01', 'fase9@example.com', '11999990000', 'hash-teste', 1, true, false);

INSERT INTO editora (id_editora, nome, cidade, pais)
VALUES (1, 'Editora Teste Fase 9', 'Sao Paulo', 'Brasil');

INSERT INTO autor (id_autor, nome, nacionalidade)
VALUES (1, 'Autor Teste Fase 14', 'Brasil');

INSERT INTO genero (id_genero, nome, descricao)
VALUES (1, 'Genero Teste Fase 14', 'Validacao automatizada de vinculo.');

INSERT INTO livro (id_livro, isbn, titulo, ano_publicacao, id_editora, idioma, descricao)
VALUES (1, 'ISBN-F9-001', 'Livro Teste Fase 9', 2026, 1, 'Portugues', 'Fixture automatizada da Fase 9.');

INSERT INTO exemplar (id_exemplar, id_livro, codigo_barras, rfid, status, localizacao)
VALUES (1, 1, 'F9-EX-001', 'RFID-F9-001', 'DISPONIVEL', 'Teste');

INSERT INTO exemplar (id_exemplar, id_livro, codigo_barras, rfid, status, localizacao)
VALUES (2, 1, 'F14-EX-002', 'RFID-F14-002', 'EMPRESTADO', 'Teste');

WITH emprestimo_aberto AS (
    INSERT INTO emprestimo (id_usuario, data_prevista_devolucao, status, origem)
    VALUES (1, CURRENT_DATE + INTERVAL '7 days', 'ABERTO', 'BALCAO')
    RETURNING id_emprestimo
)
INSERT INTO emprestimo_item (id_emprestimo, id_exemplar)
SELECT id_emprestimo, 2 FROM emprestimo_aberto;
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
$inputData = "000.000.000-00`nadmin-fase13`n2`n1`n1`nF9-EX-001`n2`nF9-EX-001`n0`n1`n1`n1`nUsuario CPF Invalido`n111.111.111-11`ninvalido@example.com`n11999992222`nteste123`n1`n1`n0`n3`n1`n900.000.000-01`n4`n1`n1`nUsuario Teste Fase 11`nfase11@example.com`n11999991111`n1`n1`n0`n5`n1`n1`n5`n5`n1`n4`n6`n2`nF14-EX-002B`nRFID-F14-002B`n1`nTeste bloqueado`n6`n1`n1`n1`n6`n3`n1`n1`n3`n5`nAutor Teste Fase 14`n0`n6`n1`n4`n3`n5`n0`n5`n1`n10`n0`n0`n"
$output = $inputData | & $Binary
if ($LASTEXITCODE -ne 0) {
    $output | Write-Host
    throw "Aplicacao retornou codigo $LASTEXITCODE"
}

Assert-True ($output -join "`n").Contains("[OK] Emprestimo realizado") "Fluxo de emprestimo nao confirmou sucesso."
Assert-True ($output -join "`n").Contains("[OK] Devolucao realizada") "Fluxo de devolucao nao confirmou sucesso."
Assert-True ($output -join "`n").Contains("RELATORIOS") "Menu de relatorios nao foi exercitado."
Assert-True ($output -join "`n").Contains("NOSQL - EVENTOS, LOGS E AUDITORIA") "Menu NoSQL nao foi exercitado."
Assert-True ($output -join "`n").Contains("Usuario Teste Fase 9") "Busca de usuario nao retornou fixture esperada."
Assert-True ($output -join "`n").Contains("[OK] Usuario atualizado.") "Alteracao de usuario nao confirmou sucesso."
Assert-True ($output -join "`n").Contains("[OK] Usuario desativado.") "Usuario com historico nao foi desativado."
Assert-True ($output -join "`n").Contains("Livro possui exemplares ou reservas vinculadas.") "Exclusao bloqueada de livro vinculado nao foi validada."
Assert-True ($output -join "`n").Contains("Login realizado: Administrador Teste Fase 13 (ADMINISTRADOR).") "Login administrativo nao foi validado."
Assert-True ($output -join "`n").Contains("Exemplar com emprestimo aberto nao pode ser alterado manualmente") "Alteracao manual de exemplar emprestado nao foi bloqueada."
Assert-True ($output -join "`n").Contains("[OK] Autor vinculado ao livro.") "Vinculo livro/autor nao foi validado."
Assert-True ($output -join "`n").Contains("[OK] Genero vinculado ao livro.") "Vinculo livro/genero nao foi validado."
Assert-True ($output -join "`n").Contains("Autor Teste Fase 14") "Busca de livro por autor nao retornou resultado."

Write-Host "[TEST] Validando bloqueio por perfil..."
$restrictedInput = "111.111.111-11`nbiblio-fase13`n1`n0`n"
$restrictedOutput = $restrictedInput | & $Binary
if ($LASTEXITCODE -ne 0) {
    $restrictedOutput | Write-Host
    throw "Aplicacao retornou codigo $LASTEXITCODE no fluxo de permissao"
}
Assert-True ($restrictedOutput -join "`n").Contains("[ERRO] Acesso negado para o perfil atual.") "Bibliotecario nao deveria acessar cadastros administrativos."

Write-Host "[TEST] Validando estado PostgreSQL..."
$loanState = Invoke-PsqlScalar $TestDatabase "SELECT origem || ':' || status FROM emprestimo ORDER BY id_emprestimo DESC LIMIT 1;"
Assert-True ($loanState -eq "BALCAO:ENCERRADO") "Emprestimo esperado como BALCAO:ENCERRADO, obtido $loanState."

$copyStatus = Invoke-PsqlScalar $TestDatabase "SELECT status FROM exemplar WHERE codigo_barras = 'F9-EX-001';"
Assert-True ($copyStatus -eq "DISPONIVEL") "Exemplar deveria estar DISPONIVEL apos devolucao, obtido $copyStatus."

$reportRows = Invoke-PsqlScalar $TestDatabase "SELECT COUNT(*) FROM livro l LEFT JOIN exemplar e ON e.id_livro = l.id_livro;"
Assert-True ([int]$reportRows -gt 0) "Relatorio de acervo nao tem dados-base."

$userState = Invoke-PsqlScalar $TestDatabase "SELECT nome || ':' || ativo::text FROM usuario WHERE id_usuario = 1;"
Assert-True ($userState -eq "Usuario Teste Fase 11:false") "Usuario deveria ter sido alterado e desativado, obtido $userState."

$bookStillExists = Invoke-PsqlScalar $TestDatabase "SELECT COUNT(*) FROM livro WHERE id_livro = 1;"
Assert-True ([int]$bookStillExists -eq 1) "Livro vinculado nao deveria ser excluido."

$invalidCpfUser = Invoke-PsqlScalar $TestDatabase "SELECT COUNT(*) FROM usuario WHERE nome = 'Usuario CPF Invalido';"
Assert-True ([int]$invalidCpfUser -eq 0) "Usuario com CPF invalido nao deveria ser inserido."

$authorLink = Invoke-PsqlScalar $TestDatabase "SELECT COUNT(*) FROM livro_autor WHERE id_livro = 1 AND id_autor = 1;"
Assert-True ([int]$authorLink -eq 1) "Vinculo livro/autor nao foi persistido."

$genreLink = Invoke-PsqlScalar $TestDatabase "SELECT COUNT(*) FROM livro_genero WHERE id_livro = 1 AND id_genero = 1;"
Assert-True ([int]$genreLink -eq 1) "Vinculo livro/genero nao foi persistido."

$blockedCopyCode = Invoke-PsqlScalar $TestDatabase "SELECT codigo_barras FROM exemplar WHERE id_exemplar = 2;"
Assert-True ($blockedCopyCode -eq "F14-EX-002") "Exemplar emprestado nao deveria ter sido alterado manualmente."

Write-Host "[TEST] Validando documentos MongoDB..."
$eventCount = Invoke-MongoScalar "db.eventos.countDocuments({tipo:'EMPRESTIMO_REALIZADO', origem:'BALCAO'})"
Assert-True ([int]$eventCount -ge 1) "Evento EMPRESTIMO_REALIZADO/BALCAO nao foi registrado."

$auditCount = Invoke-MongoScalar "db.auditoria.countDocuments({entidade:'emprestimo'})"
Assert-True ([int]$auditCount -ge 1) "Auditoria de emprestimo nao foi registrada."

$cadastroAuditCount = Invoke-MongoScalar "db.auditoria.countDocuments({entidade:'usuario', acao:{`$in:['ALTERACAO','DESATIVACAO']}})"
Assert-True ([int]$cadastroAuditCount -ge 2) "Auditoria de alteracao/desativacao de usuario nao foi registrada."

$usuarioAuditBefore = Invoke-MongoScalar "const d=db.auditoria.findOne({entidade:'usuario', acao:'ALTERACAO'}); d && d.antes && d.antes.nome"
Assert-True ($usuarioAuditBefore -eq "Usuario Teste Fase 9") "Auditoria antes/depois nao preservou o nome anterior do usuario."

$usuarioAuditAfter = Invoke-MongoScalar "const d=db.auditoria.findOne({entidade:'usuario', acao:'ALTERACAO'}); d && d.depois && d.depois.nome"
Assert-True ($usuarioAuditAfter -eq "Usuario Teste Fase 11") "Auditoria antes/depois nao preservou o nome novo do usuario."

$usuarioDeactivateAfter = Invoke-MongoScalar "const d=db.auditoria.findOne({entidade:'usuario', acao:'DESATIVACAO'}); d && d.depois && d.depois.ativo"
Assert-True ($usuarioDeactivateAfter -eq "false") "Auditoria de desativacao nao registrou usuario inativo no depois."

$logCount = Invoke-MongoScalar "db.logs.countDocuments({componente:'main'})"
Assert-True ([int]$logCount -ge 1) "Logs de sistema nao foram registrados."

$authLogCount = Invoke-MongoScalar "db.logs.countDocuments({componente:'auth', mensagem:'Login realizado com sucesso.'})"
Assert-True ([int]$authLogCount -ge 2) "Logs de autenticacao nao foram registrados."

Write-Host "[TEST] OK: build, login/perfis, regras Fase 14, cadastros V2, fluxo, relatorios, PostgreSQL e MongoDB validados."
