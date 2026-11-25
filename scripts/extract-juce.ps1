param(
    [Parameter(Mandatory=$true)][string]$JuceZip
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$target = Join-Path $root "third_party"
New-Item -ItemType Directory -Force -Path $target | Out-Null
Expand-Archive -LiteralPath $JuceZip -DestinationPath $target -Force
if (-not (Test-Path (Join-Path $target "JUCE\CMakeLists.txt"))) {
    throw "O ZIP não contém a estrutura esperada JUCE\CMakeLists.txt"
}
Write-Host "JUCE extraído para $target\JUCE" -ForegroundColor Green
