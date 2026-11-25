# Vocloning Native 0.2.9.24 — Integração Sopro V2 Turbo

## O que foi alterado

- Novo backend `SoproTTSBackend` em C++/JUCE.
- Sopro V2 Turbo é a seleção inicial e predefinida da interface.
- Pocket TTS 24L continua disponível como alternativa.
- Execução do CLI local `audiocpp_cli.exe` com o modelo Safetensors já instalado.
- Língua fixada em `language=pt` na chamada Sopro.
- Validação do WAV: existência, tamanho, leitura, amostras finitas, não silêncio, mono e 24 kHz.
- O áudio validado continua a ser reproduzido automaticamente pela infraestrutura JUCE já existente.

## Localização esperada no PC

`C:\AI\Sopro-PT-PT\audio.cpp`

O backend procura:

- `build\windows-cpu-release\bin\audiocpp_cli.exe`
- `model_specs\sopro_tts.json`
- `models\sopro-v2-turbo\config.json`
- `models\sopro-v2-turbo\tokenizer.model`
- `models\sopro-v2-turbo\model.safetensors`
- `models\sopro-v2-turbo\semantic_encoder.safetensors`
- `models\sopro-v2-turbo\speaker_encoder.safetensors`
- `models\sopro-v2-turbo\vocoder.safetensors`

O caminho pode ser alterado através da variável de ambiente `VOCLONING_SOPRO_ROOT`. Também está prevista a disposição portátil `Runtime\Sopro\audio.cpp` junto do executável.

## Compilação no Windows 11

A partir da pasta extraída, com JUCE 9.0.6 disponível:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force
.\scripts\check-sopro-install.ps1
.\scripts\build-windows.ps1 -JuceDir 'C:\CAMINHO\PARA\JUCE-9.0.6' -Config Release
```

O executável é produzido pelo CMake em `build\VocloningNative_artefacts\Release\Vocloning.exe` (o caminho exato pode variar com a configuração do gerador).

## Limites da validação desta entrega

O código foi integrado e revisto estaticamente no ambiente de preparação. Este ambiente não dispõe do toolchain MSVC/Windows nem da instalação JUCE 9.0.6 do computador do utilizador; por isso, não se declara a compilação nem a geração dentro da GUI como já testadas. A primeira compilação e o teste funcional têm de ser confirmados no Windows 11.

## Sem serviços externos

A integração chama o CLI local. Não usa xAI API, HTTP, WebView ou Chromium. O modelo e o executável Sopro não são redistribuídos neste ZIP; são usados a partir da instalação local existente.


## Instalador 0.2.9.24

O instalador Windows usa o logótipo e o ícone oficiais da aplicação. Antes de abrir o Vocloning, executa `scripts/install-sopro.ps1`: procura primeiro instalações/modelos existentes, valida os ficheiros essenciais e só descarrega o que estiver em falta. Numa instalação limpa, instala o runtime oficial CPU portable do audio.cpp e os pesos Safetensors do Sopro V2 Turbo na pasta `Runtime\Sopro\audio.cpp` da aplicação.

O instalador necessita de Internet se não encontrar uma instalação completa. O pacote não inclui os pesos do modelo.
