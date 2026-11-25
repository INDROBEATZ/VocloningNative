$ErrorActionPreference = 'Stop'
$Root = 'C:\AI\Sopro-PT-PT\audio.cpp'
$Required = @(
    'build\windows-cpu-release\bin\audiocpp_cli.exe',
    'model_specs\sopro_tts.json',
    'models\sopro-v2-turbo\config.json',
    'models\sopro-v2-turbo\tokenizer.model',
    'models\sopro-v2-turbo\model.safetensors',
    'models\sopro-v2-turbo\semantic_encoder.safetensors',
    'models\sopro-v2-turbo\speaker_encoder.safetensors',
    'models\sopro-v2-turbo\vocoder.safetensors'
)
Write-Host '=== VERIFICACAO SOPRO V2 TURBO ===' -ForegroundColor Cyan
$Missing = @()
foreach ($Relative in $Required) {
    $Path = Join-Path $Root $Relative
    if (Test-Path -LiteralPath $Path -PathType Leaf) {
        Write-Host "OK   $Relative" -ForegroundColor Green
    } else {
        Write-Host "FALTA $Relative" -ForegroundColor Red
        $Missing += $Relative
    }
}
if ($Missing.Count -gt 0) {
    throw "Instalacao Sopro incompleta: $($Missing -join ', ')"
}
Write-Host 'Sopro V2 Turbo pronto para ser detetado pelo Vocloning Native.' -ForegroundColor Green
