param(
    [Parameter(Mandatory=$true)][string]$InstallRoot,
    [string]$LogPath = "$env:ProgramData\Vocloning\setup-sopro.log"
)
$ErrorActionPreference = "Stop"
$ProgressPreference = "SilentlyContinue"
$requiredModel = @(
    "config.json","tokenizer.model","model.safetensors",
    "semantic_encoder.safetensors","speaker_encoder.safetensors","vocoder.safetensors"
)
function Write-Log([string]$Message) {
    $line = "$(Get-Date -Format s) $Message"
    Write-Host $line
    Add-Content -LiteralPath $LogPath -Value $line -Encoding UTF8
}
function Test-Model([string]$Root) {
    $d = Join-Path $Root "models\sopro-v2-turbo"
    $minimumBytes = @{
        "config.json" = 1000; "tokenizer.model" = 100000
        "model.safetensors" = 100000000; "semantic_encoder.safetensors" = 10000000
        "speaker_encoder.safetensors" = 1000000; "vocoder.safetensors" = 10000000
    }
    foreach($f in $requiredModel) {
        $p = Join-Path $d $f
        if(!(Test-Path -LiteralPath $p -PathType Leaf)) { return $false }
        if((Get-Item -LiteralPath $p).Length -lt $minimumBytes[$f]) { return $false }
    }
    return $true
}
function Test-Install([string]$Root) {
    return ((Test-Path (Join-Path $Root "build\windows-cpu-release\bin\audiocpp_cli.exe")) -and
            (Test-Path (Join-Path $Root "model_specs\sopro_tts.json")) -and (Test-Model $Root))
}
New-Item -ItemType Directory -Force -Path (Split-Path $LogPath) | Out-Null
Write-Log "A verificar instalações Sopro existentes."
$candidates = @(
    $env:VOCLONING_SOPRO_ROOT,
    "C:\AI\Sopro-PT-PT\audio.cpp",
    "C:\AI\audio.cpp",
    "D:\AI\Sopro-PT-PT\audio.cpp",
    "$env:USERPROFILE\AI\Sopro-PT-PT\audio.cpp",
    "$env:ProgramData\Vocloning\Sopro\audio.cpp",
    "$env:LOCALAPPDATA\Vocloning\Sopro\audio.cpp",
    $InstallRoot
) | Where-Object { $_ -and $_.Trim() } | Select-Object -Unique
$root = $null
foreach($candidate in $candidates) {
    if(Test-Path -LiteralPath $candidate -PathType Container) {
        if(Test-Model $candidate) { $root=$candidate; Write-Log "Modelo existente localizado: $root"; break }
    }
}
if(!$root) { $root=$InstallRoot; Write-Log "Modelo não encontrado. Será instalado em: $root" }
New-Item -ItemType Directory -Force -Path $root | Out-Null
$modelDir=Join-Path $root "models\sopro-v2-turbo"
$specDir=Join-Path $root "model_specs"
$binDir=Join-Path $root "build\windows-cpu-release\bin"
New-Item -ItemType Directory -Force -Path $modelDir,$specDir,$binDir | Out-Null

# Install the official prebuilt CPU runtime only if the CLI is missing.
$cli=Join-Path $binDir "audiocpp_cli.exe"
if(!(Test-Path $cli)) {
    Write-Log "Motor audio.cpp não encontrado. A obter pacote oficial Windows CPU portable."
    $headers=@{"User-Agent"="VocloningNative-Setup";"Accept"="application/vnd.github+json"}
    $release=Invoke-RestMethod -Uri "https://api.github.com/repos/0xShug0/audio.cpp/releases/latest" -Headers $headers
    $asset=$release.assets | Where-Object { $_.name -eq "audiocpp-windows-cpu-portable.zip" } | Select-Object -First 1
    if(!$asset) { throw "O pacote oficial audiocpp-windows-cpu-portable.zip não foi encontrado na última release." }
    $tmp=Join-Path $env:TEMP ("vocloning-audio-" + [guid]::NewGuid().ToString("N"))
    New-Item -ItemType Directory -Force -Path $tmp | Out-Null
    $archive=Join-Path $tmp "audio-cpp.zip"
    Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $archive -Headers @{"User-Agent"="VocloningNative-Setup"}
    Expand-Archive -LiteralPath $archive -DestinationPath (Join-Path $tmp "unpacked") -Force
    $found=Get-ChildItem -Path (Join-Path $tmp "unpacked") -Filter "audiocpp_cli.exe" -File -Recurse | Select-Object -First 1
    if(!$found) { throw "O pacote oficial foi descarregado mas não contém audiocpp_cli.exe." }
    Get-ChildItem -LiteralPath $found.Directory.FullName -Force | Copy-Item -Destination $binDir -Recurse -Force
    Remove-Item $tmp -Recurse -Force
    if(!(Test-Path $cli)) { throw "Falha a instalar audiocpp_cli.exe." }
    Write-Log "Motor CPU instalado."
} else { Write-Log "Motor audio.cpp já instalado; reutilizado." }

# Model spec is required because this build uses --model-spec-override.
$spec=Join-Path $specDir "sopro_tts.json"
if(!(Test-Path $spec)) {
    Write-Log "A instalar especificação Sopro."
    Invoke-WebRequest -Uri "https://raw.githubusercontent.com/0xShug0/audio.cpp/main/model_specs/sopro_tts.json" -OutFile $spec -Headers @{"User-Agent"="VocloningNative-Setup"}
}

# Download only missing model files. Existing valid files are never overwritten.
foreach($file in $requiredModel) {
    $dest=Join-Path $modelDir $file
    $minimum = switch($file) {
        "config.json" { 1000 } "tokenizer.model" { 100000 }
        "model.safetensors" { 100000000 } "semantic_encoder.safetensors" { 10000000 }
        "speaker_encoder.safetensors" { 1000000 } "vocoder.safetensors" { 10000000 }
    }
    if((Test-Path $dest) -and (Get-Item $dest).Length -ge $minimum) {
        Write-Log "Modelo já contém $file e passou a verificação de tamanho; ignorado."
        continue
    }
    $url="https://huggingface.co/samuel-vitorino/sopro-v2-turbo/resolve/main/$file"
    $partial="$dest.partial"
    Write-Log "A descarregar ficheiro do modelo: $file"
    Invoke-WebRequest -Uri $url -OutFile $partial -Headers @{"User-Agent"="VocloningNative-Setup"}
    if(!(Test-Path $partial) -or (Get-Item $partial).Length -lt $minimum) { throw "Download incompleto ou inválido: $file" }
    Move-Item -LiteralPath $partial -Destination $dest -Force
}
if(!(Test-Install $root)) { throw "A instalação Sopro não passou a verificação final." }
[Environment]::SetEnvironmentVariable("VOCLONING_SOPRO_ROOT",$root,"Machine")
Write-Log "Sopro V2 Turbo instalado e validado em $root"
exit 0
