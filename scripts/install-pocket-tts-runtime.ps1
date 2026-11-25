param(
    [Parameter(Mandatory=$true)][string]$RuntimeExe,
    [string]$Destination = ""
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if (-not (Test-Path -LiteralPath $RuntimeExe -PathType Leaf)) {
    throw "Executável Pocket TTS não encontrado: $RuntimeExe"
}
if ([IO.Path]::GetFileName($RuntimeExe) -notmatch '^pocket-tts(\.exe)?$') {
    throw "Seleciona o executável pocket-tts.exe da distribuição oficial."
}
if (-not $Destination) { $Destination = Join-Path $root "Runtime" }
New-Item -ItemType Directory -Force -Path $Destination | Out-Null
$target = Join-Path $Destination "pocket-tts.exe"
Copy-Item -LiteralPath $RuntimeExe -Destination $target -Force
if (-not (Test-Path -LiteralPath $target -PathType Leaf)) { throw "Falha ao instalar o runtime." }
Write-Host "Pocket TTS instalado em: $target" -ForegroundColor Green
Write-Host "Modelo selecionado pela aplicação: portuguese_24l" -ForegroundColor Cyan
