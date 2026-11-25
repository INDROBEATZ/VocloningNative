param(
    [Parameter(Mandatory=$true)][string]$JuceDir,
    [string]$Config = "Release",
    [string]$InnoCompiler = ""
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
$build = Join-Path $root "build"
$stage = Join-Path $root "BuildOutput"
$exe = Join-Path $build "VocloningNative_artefacts\$Config\Vocloning.exe"
if (!(Test-Path (Join-Path $JuceDir "CMakeLists.txt"))) { throw "JUCE 9.0.6 não encontrado: $JuceDir" }
if (Test-Path $stage) { Remove-Item $stage -Recurse -Force }
New-Item -ItemType Directory -Force -Path $stage,(Join-Path $stage "Runtime") | Out-Null

Write-Host "=== BUILD RELEASE ===" -ForegroundColor Cyan
& (Join-Path $PSScriptRoot "build-windows.ps1") -JuceDir $JuceDir -Config $Config
if ($LASTEXITCODE -ne 0 -or !(Test-Path $exe)) { throw "Build da aplicação falhou ou não gerou Vocloning.exe." }

Copy-Item $exe (Join-Path $stage "Vocloning.exe") -Force
$pocket = Join-Path $root "Runtime"
if (Test-Path $pocket) { Copy-Item (Join-Path $pocket "*") (Join-Path $stage "Runtime") -Recurse -Force }

if (!$InnoCompiler) {
    $candidates = @(
        "${env:ProgramFiles(x86)}\Inno Setup 6\ISCC.exe",
        "$env:ProgramFiles\Inno Setup 6\ISCC.exe"
    )
    $InnoCompiler = $candidates | Where-Object { Test-Path $_ } | Select-Object -First 1
}
if (!$InnoCompiler -or !(Test-Path $InnoCompiler)) {
    throw "Inno Setup 6 não encontrado. Instala Inno Setup 6 e volta a executar este script."
}
Write-Host "=== COMPILAR SETUP.EXE ===" -ForegroundColor Cyan
& $InnoCompiler (Join-Path $root "installer\VocloningNative.iss")
if ($LASTEXITCODE -ne 0) { throw "Inno Setup falhou ao compilar o instalador." }
$dist = Join-Path $root "dist"
$setup = Get-ChildItem $dist -Filter "VocloningNative-Setup-*.exe" | Sort-Object LastWriteTime -Descending | Select-Object -First 1
if (!$setup) { throw "ISCC terminou sem produzir o Setup.exe esperado." }
Write-Host "SETUP CRIADO: $($setup.FullName)" -ForegroundColor Green
