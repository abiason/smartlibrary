$ErrorActionPreference = "Stop"
$OutputDir = Split-Path -Parent $MyInvocation.MyCommand.Path
$Ffmpeg = "C:\msys64\ucrt64\bin\ffmpeg.exe"
$Ffprobe = "C:\msys64\ucrt64\bin\ffprobe.exe"

if (-not (Test-Path $Ffmpeg)) { throw "ffmpeg nao encontrado em $Ffmpeg" }
if (-not (Test-Path $Ffprobe)) { throw "ffprobe nao encontrado em $Ffprobe" }

Add-Type -AssemblyName System.Drawing
Add-Type -AssemblyName System.Speech

$slidesDir = Join-Path $OutputDir "slides"
$audioDir = Join-Path $OutputDir "audio"
$segmentsDir = Join-Path $OutputDir "segments"
New-Item -ItemType Directory -Force -Path $slidesDir, $audioDir, $segmentsDir | Out-Null

$items = @(
    @{
        Title = "SmartLibrary V4"
        Subtitle = "Sistema academico de biblioteca em C17"
        Bullets = @("PostgreSQL transacional", "MongoDB para eventos, logs e auditoria", "Login, perfis, testes e setup automatizado")
        Narration = "Este e o SmartLibrary, uma aplicacao academica de biblioteca desenvolvida em C dezessete. O sistema usa PostgreSQL como base transacional principal e MongoDB para eventos, logs e auditoria. A versao V4 fecha a entrega com autenticacao, perfis, testes integrados, setup automatizado e roteiro de apresentacao."
    },
    @{
        Title = "Setup E Validacao"
        Subtitle = "Ambiente preparado com um comando"
        Bullets = @("scripts/setup_local.ps1", "Valida psql, mongosh, gcc, pkg-config e mingw32-make", "Cria banco, aplica MongoDB e compila")
        Narration = "Para preparar o ambiente, basta definir a senha do PostgreSQL na sessao e executar o script setup local. Ele valida as dependencias, cria o banco principal quando necessario, aplica os scripts do PostgreSQL em ambiente limpo, aplica validators e indices no MongoDB e compila o projeto. A senha real nao e gravada em arquivo."
    },
    @{
        Title = "Login, Perfis E Senha"
        Subtitle = "Seguranca operacional"
        Bullets = @("Senha com BCrypt via pgcrypto", "ADMINISTRADOR, BIBLIOTECARIO e USUARIO", "Troca de senha com validacao da senha atual")
        Narration = "Ao iniciar, o sistema exige login antes de liberar o menu. As senhas sao validadas com BCrypt usando pgcrypto. Os perfis separam permissao de administrador, bibliotecario e usuario. A V4 tambem adiciona troca de senha dentro da aplicacao, exigindo a senha atual e confirmacao da nova senha."
    },
    @{
        Title = "Primeiro Acesso"
        Subtitle = "Recuperacao quando nao ha administrador ativo"
        Bullets = @("Banco vazio cria administrador inicial", "Usuarios sem admin ativo acionam recuperacao", "Mensagens de login mais claras")
        Narration = "O primeiro acesso ficou mais seguro e claro. Se o banco estiver vazio, o sistema orienta a criacao do administrador inicial. Se ja existirem usuarios, mas nenhum administrador ativo e desbloqueado, o fluxo permite criar um administrador de recuperacao. A mensagem de login recusado tambem orienta melhor o operador."
    },
    @{
        Title = "Cadastros E Regras"
        Subtitle = "CRUD completo com validacoes"
        Bullets = @("Criar, listar, buscar, alterar e excluir ou desativar", "CPF, e-mail e dados editoriais validados", "Livros vinculados a autores e generos")
        Narration = "Nos cadastros administrativos, o sistema vai alem do cadastro basico. Ele permite criar, listar, buscar, alterar, excluir ou desativar usuarios, autores, editoras, generos, livros e exemplares. Tambem valida CPF, e-mail e regras editoriais, alem de controlar vinculos de livros com autores e generos."
    },
    @{
        Title = "Auditoria Rastreavel"
        Subtitle = "Antes, depois e operador real"
        Bullets = @("Snapshots antes e depois no MongoDB", "usuarioId do operador autenticado", "Evidencia forte para avaliacao")
        Narration = "Um dos pontos mais fortes da V4 e a auditoria rastreavel. Alteracoes administrativas registram no MongoDB o estado antes, o estado depois e o usuarioId do operador autenticado. Isso conecta login, permissao e rastreabilidade em uma evidencia tecnica clara."
    },
    @{
        Title = "Circulacao E Self Checkout"
        Subtitle = "Fluxos operacionais da biblioteca"
        Bullets = @("Emprestimo, devolucao e renovacao transacionais", "Reservas com status controlado", "Self Checkout por CPF, codigo de barras ou RFID")
        Narration = "A area operacional cobre emprestimos, devolucoes, renovacoes e reservas. As operacoes criticas usam transacoes e respeitam regras como usuario ativo, atraso, limite de exemplares, disponibilidade e reserva de outro usuario. O Self Checkout permite atendimento por CPF, codigo de barras ou RFID."
    },
    @{
        Title = "NoSQL, Relatorios E Testes"
        Subtitle = "Observabilidade e confianca"
        Bullets = @("Eventos por origem: BALCAO e SELF_CHECKOUT", "Logs e auditoria consultaveis", "mingw32-make test valida fluxo real")
        Narration = "O MongoDB registra eventos por origem, logs operacionais e auditoria recente, todos consultaveis pela propria aplicacao. Os relatorios mostram acervo, atrasos, livros mais emprestados, origens de emprestimo, reservas e pendencias. A suite integrada recompila o projeto, recria bancos de teste e valida PostgreSQL, MongoDB e fluxos reais."
    },
    @{
        Title = "Fechamento"
        Subtitle = "Entrega pronta para apresentacao"
        Bullets = @("Arquitetura em camadas", "PostgreSQL e MongoDB usados de forma coerente", "V4 documentada, testada e versionada no GitHub")
        Narration = "Como fechamento, o SmartLibrary entrega mais que um CRUD: arquitetura em camadas, persistencia relacional e documental, autenticacao, perfis, regras de negocio, auditoria com operador real, setup automatizado, testes integrados e documentacao de avaliacao. A V4 esta concluida, validada e versionada no GitHub."
    }
)

function Draw-WrappedText {
    param($Graphics, [string]$Text, $Font, $Brush, [float]$X, [float]$Y, [float]$Width, [float]$LineHeight)
    $words = $Text -split ' '
    $line = ''
    foreach ($word in $words) {
        $candidate = if ($line.Length -eq 0) { $word } else { "$line $word" }
        if ($Graphics.MeasureString($candidate, $Font).Width -le $Width) {
            $line = $candidate
        } else {
            $Graphics.DrawString($line, $Font, $Brush, $X, $Y)
            $Y += $LineHeight
            $line = $word
        }
    }
    if ($line.Length -gt 0) {
        $Graphics.DrawString($line, $Font, $Brush, $X, $Y)
        $Y += $LineHeight
    }
    return $Y
}

function New-SlideImage {
    param([int]$Index, $Item)
    $path = Join-Path $slidesDir ("slide_{0:D2}.png" -f $Index)
    $bmp = New-Object System.Drawing.Bitmap 1920, 1080
    $g = [System.Drawing.Graphics]::FromImage($bmp)
    $g.SmoothingMode = [System.Drawing.Drawing2D.SmoothingMode]::AntiAlias
    $g.TextRenderingHint = [System.Drawing.Text.TextRenderingHint]::AntiAliasGridFit

    $bg = New-Object System.Drawing.Drawing2D.LinearGradientBrush ([System.Drawing.Rectangle]::new(0,0,1920,1080)), ([System.Drawing.Color]::FromArgb(17,24,39)), ([System.Drawing.Color]::FromArgb(15,82,88)), 35
    $g.FillRectangle($bg, 0, 0, 1920, 1080)
    $accent = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(34,211,238))
    $white = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(248,250,252))
    $muted = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(203,213,225))
    $panel = New-Object System.Drawing.SolidBrush ([System.Drawing.Color]::FromArgb(70, 2, 6, 23))

    $g.FillRectangle($panel, 110, 120, 1700, 760)
    $g.FillRectangle($accent, 110, 120, 12, 760)

    $titleFont = New-Object System.Drawing.Font "Segoe UI", 58, ([System.Drawing.FontStyle]::Bold)
    $subtitleFont = New-Object System.Drawing.Font "Segoe UI", 28, ([System.Drawing.FontStyle]::Regular)
    $bulletFont = New-Object System.Drawing.Font "Segoe UI", 34, ([System.Drawing.FontStyle]::Regular)
    $smallFont = New-Object System.Drawing.Font "Segoe UI", 20, ([System.Drawing.FontStyle]::Regular)

    $g.DrawString($Item.Title, $titleFont, $white, 165, 170)
    $g.DrawString($Item.Subtitle, $subtitleFont, $muted, 170, 255)
    $g.DrawLine([System.Drawing.Pens]::White, 170, 325, 1700, 325)

    $y = 390
    foreach ($bullet in $Item.Bullets) {
        $g.FillEllipse($accent, 180, $y + 12, 14, 14)
        $y = Draw-WrappedText $g $bullet $bulletFont $white 220 $y 1450 48
        $y += 34
    }

    $g.DrawString(("Slide {0} de {1}" -f $Index, $items.Count), $smallFont, $muted, 1500, 930)
    $g.DrawString("SmartLibrary V4", $smallFont, $muted, 170, 930)

    $bmp.Save($path, [System.Drawing.Imaging.ImageFormat]::Png)
    $g.Dispose(); $bmp.Dispose(); $bg.Dispose(); $accent.Dispose(); $white.Dispose(); $muted.Dispose(); $panel.Dispose()
    return $path
}

function New-Narration {
    param([int]$Index, [string]$Text)
    $path = Join-Path $audioDir ("audio_{0:D2}.wav" -f $Index)
    $synth = New-Object System.Speech.Synthesis.SpeechSynthesizer
    $voice = $synth.GetInstalledVoices() | Where-Object { $_.VoiceInfo.Culture.Name -eq 'pt-BR' } | Select-Object -First 1
    if ($voice) { $synth.SelectVoice($voice.VoiceInfo.Name) }
    $synth.Rate = 0
    $synth.Volume = 100
    $synth.SetOutputToWaveFile($path)
    $synth.Speak($Text)
    $synth.Dispose()
    return $path
}

function Get-Duration {
    param([string]$Path)
    $duration = & $Ffprobe -v error -show_entries format=duration -of default=noprint_wrappers=1:nokey=1 $Path
    if ($LASTEXITCODE -ne 0) { throw "ffprobe falhou para $Path" }
    return [double]::Parse($duration, [Globalization.CultureInfo]::InvariantCulture)
}

$concatFile = Join-Path $OutputDir "segments.txt"
Set-Content -Path $concatFile -Value "" -Encoding ASCII

for ($i = 0; $i -lt $items.Count; $i++) {
    $index = $i + 1
    $slide = New-SlideImage $index $items[$i]
    $audio = New-Narration $index $items[$i].Narration
    $duration = [Math]::Max((Get-Duration $audio) + 0.4, 5.0)
    $segment = Join-Path $segmentsDir ("segment_{0:D2}.mp4" -f $index)
    & $Ffmpeg -y -loop 1 -framerate 30 -i $slide -i $audio -t $duration -c:v libx264 -tune stillimage -c:a aac -b:a 160k -pix_fmt yuv420p -shortest $segment | Out-Null
    if ($LASTEXITCODE -ne 0) { throw "ffmpeg falhou ao gerar segmento $index" }
    Add-Content -Path $concatFile -Value ("file '{0}'" -f ($segment.Replace("'", "'\''"))) -Encoding ASCII
}

$final = Join-Path $OutputDir "smartlibrary_apresentacao_v4.mp4"
& $Ffmpeg -y -f concat -safe 0 -i $concatFile -c copy $final | Out-Null
if ($LASTEXITCODE -ne 0) { throw "ffmpeg falhou ao unir segmentos" }

Write-Host "Video gerado: $final"