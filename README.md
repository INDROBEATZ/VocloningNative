# Vocloning Native

**Versão do código:** `0.2.9.24`  
**Plataforma-alvo:** Windows 11 x64  
**Tecnologia:** C++20, JUCE 9.0.6 e CMake  
**Autor/marca no projeto:** INDRO

Aplicação nativa de geração/clonagem de voz, com interface em português europeu e integração local com dois motores:

- **Sopro V2 Turbo** — integração local para geração em português europeu (`pt-PT`).
- **Pocket TTS 24L** — motor alternativo; o backend seleciona o modelo `portuguese_24l`.

O projeto não integra a xAI API. O objetivo da integração Pocket TTS é manter a abordagem local e a qualidade vocal natural pretendida, sem depender de serviços xAI.

> **Estado de validação:** este repositório contém o código-fonte da versão 0.2.9.24 HOTFIX. A revisão estática identificou a correção de compilação em `MainComponent.cpp`, mas esta cópia não constitui prova de uma compilação limpa nem de testes funcionais no Windows 11. Esses testes continuam por executar/confirmar no ambiente Windows com JUCE 9.0.6.

## Funcionalidades identificadas no código

- Interface gráfica nativa JUCE.
- Importação de referências de voz WAV/MP3.
- Geração com Sopro V2 Turbo ou Pocket TTS 24L.
- Validação do WAV gerado pelo backend Sopro: leitura, amostras finitas, ausência de silêncio, mono e 24 kHz.
- Preparação da referência Pocket TTS em WAV mono, até 30 segundos.
- Reprodução do resultado através da infraestrutura JUCE da aplicação.

A disponibilidade real de cada motor depende da instalação do respetivo runtime e dos modelos.

## Estrutura

```text
Assets/          Ícone e logótipo
Source/          Código C++/JUCE
Runtime/         Instruções para instalar o runtime Pocket TTS
scripts/         Compilação, instalação e empacotamento Windows
installer/       Script Inno Setup
InstallerAssets/ Imagens do instalador
docs/            Auditoria e notas de publicação
```

## Requisitos

- Windows 11 x64.
- Visual Studio 2022 com ferramentas C++ para desktop.
- CMake 3.22 ou superior.
- JUCE 9.0.6 (instalado separadamente).
- Inno Setup 6, apenas para gerar o instalador.
- Runtime Pocket TTS e/ou runtime/modelo Sopro, conforme o motor utilizado.

O código-fonte do JUCE e os binários/modelos dos motores não são incluídos neste repositório. Obtém cada dependência na respetiva fonte oficial e confirma os termos de licença e redistribuição antes de distribuir uma compilação.

## Compilar no Windows

Abre PowerShell na pasta do repositório:

```powershell
Set-ExecutionPolicy -Scope Process Bypass -Force

$env:JUCE_DIR = "C:\CAMINHO\PARA\JUCE-9.0.6"

.\scripts\build-windows.ps1 -JuceDir $env:JUCE_DIR -Config Release
```

O executável deverá ser produzido em:

```text
build\VocloningNative_artefacts\Release\Vocloning.exe
```

O CMake copia `Runtime/` para junto do executável apenas se o runtime existir localmente durante a compilação.

## Pocket TTS 24L

1. Obtém o executável standalone `pocket-tts.exe` nas releases oficiais do projeto Pocket TTS.
2. Coloca-o em `Runtime/pocket-tts.exe`.
3. Executa novamente a compilação.

O runtime é deliberadamente excluído do Git. Os pesos do modelo são descarregados pelo runtime na primeira utilização. A aplicação procura o modelo `portuguese_24l`.

## Sopro V2 Turbo

A integração procura o CLI, a especificação e os ficheiros do modelo Sopro. A localização portátil esperada é:

```text
Runtime/Sopro/audio.cpp/
```

Também é possível indicar a localização através de `VOCLONING_SOPRO_ROOT`. O script `scripts/install-sopro.ps1` automatiza a instalação dos componentes em falta. **Lê e revê o script antes de o executar:** descarrega um runtime de uma release GitHub e pesos de modelo do Hugging Face, sem fixar versões/commits nem validar hashes SHA-256. A configuração atual também escreve a variável de ambiente ao nível da máquina, o que pode exigir privilégios de administrador.

Não publiques nem redistribuas os binários ou pesos Sopro sem confirmar as respetivas licenças e permissões.

## Segurança e privacidade

- Não incluas tokens, credenciais, ficheiros pessoais, caches, modelos ou executáveis compilados em commits.
- O backend Pocket TTS pode ler o token local do Hugging Face (`~/.cache/huggingface/token`) para o disponibilizar temporariamente ao processo filho. Não publiques esse ficheiro nem o seu conteúdo.
- Revê scripts de instalação e dependências antes de os executar.
- Não foram encontrados padrões óbvios de chaves API ou tokens hardcoded nos ficheiros de texto analisados; isto não substitui uma auditoria de segurança completa.

## Licença

Ainda não foi incluído um ficheiro `LICENSE`, porque a licença de distribuição do código deste projeto não foi especificada. Um repositório GitHub público permite consultar o código, mas **não concede automaticamente direitos de reutilização, modificação ou redistribuição**. Define a licença e confirma os direitos sobre os componentes de terceiros antes de convidar contribuições ou permitir reutilização.

## English summary

Vocloning Native is a Windows 11 x64 C++/JUCE voice-generation application. Version 0.2.9.24 includes local Sopro V2 Turbo and Pocket TTS 24L backends. This repository contains source code and build scripts only; third-party runtimes and model weights are intentionally excluded. A clean Windows/MSVC build and end-to-end runtime tests still need to be confirmed.
