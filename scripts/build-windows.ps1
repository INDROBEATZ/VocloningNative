param(
    [string]$JuceDir = "",
    [string]$BuildDir = "build",
    [ValidateSet("Debug","Release")][string]$Config = "Release"
)
$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot

if (-not $JuceDir) { $JuceDir = $env:JUCE_DIR }
if (-not $JuceDir) { $JuceDir = Join-Path $root "third_party\\JUCE" }
if (-not (Test-Path (Join-Path $JuceDir "CMakeLists.txt"))) {
    throw "JUCE não encontrado. Indica -JuceDir ou define JUCE_DIR."
}

$cmakeCommand = Get-Command cmake -ErrorAction SilentlyContinue
$CMakeExe = if ($cmakeCommand) { $cmakeCommand.Source } else { $null }

if (-not $CMakeExe) {
    $vswhere = "${env:ProgramFiles(x86)}\\Microsoft Visual Studio\\Installer\\vswhere.exe"
    if (Test-Path $vswhere) {
        $vsPath = & $vswhere -latest -products * -property installationPath
        $candidate = Join-Path $vsPath "Common7\\IDE\\CommonExtensions\\Microsoft\\CMake\\CMake\\bin\\cmake.exe"
        if (Test-Path $candidate) { $CMakeExe = $candidate }
    }
}
if (-not $CMakeExe) {
    throw "CMake não encontrado. Instala o componente CMake tools for Windows ou indica-o no PATH."
}

$buildPath = Join-Path $root $BuildDir
& $CMakeExe -S $root -B $buildPath -DJUCE_DIR="$JuceDir" -G "Visual Studio 17 2022" -A x64
if ($LASTEXITCODE -ne 0) { throw "Falha na configuração CMake." }

& $CMakeExe --build $buildPath --config $Config --parallel
if ($LASTEXITCODE -ne 0) { throw "Falha na compilação Vocloning." }

Write-Host "Build concluído. Verifica a pasta build\\$Config." -ForegroundColor Green
