# Vocloning Native

**Aplicação nativa de clonagem e geração de voz para Windows**
C++ · JUCE 9.0.6 · CMake · Windows 11 x64

**Projeto:** INDROBEATZ
**Linha de desenvolvimento:** preparação da versão 0.2.9.25
**Última base de código documentada:** 0.2.9.24 HOTFIX

O Vocloning Native é uma aplicação desktop nativa para Windows, dedicada a fluxos de trabalho com referências de voz e geração de fala. É desenvolvido em C++ e JUCE e integra motores locais através de backends próprios.

O projeto dá prioridade a uma voz natural e ao português europeu. O backend Pocket TTS está configurado com o identificador de modelo `portuguese_24l`. O backend Sopro V2 Turbo disponibiliza um percurso de geração em português europeu. O projeto não integra a API xAI.

## Estado do projeto

O responsável pelo projeto confirmou o funcionamento global da aplicação no Windows 11. A evidência disponibilizada demonstra o fluxo Sopro V2 Turbo: seleção da referência, geração, reprodução e exportação WAV. A compilação automática também terminou com sucesso usando MSVC 2022 e JUCE 9.0.6.

A compilação CI e a validação manual da aplicação são verificações diferentes. A disponibilidade dos motores depende dos runtimes e modelos instalados separadamente.

## Funcionalidades

- Interface desktop nativa JUCE.
- Seleção de referências de voz WAV e MP3.
- Biblioteca visual de referências e introdução de texto.
- Backend Pocket TTS configurado para `portuguese_24l`.
- Backend Sopro V2 Turbo através do runtime CLI `audio.cpp`.
- Percurso de geração em português europeu no backend Sopro.
- Reprodução e exportação de áudio WAV.
- Validação do WAV no backend Sopro: leitura, amostras finitas, áudio não silencioso, mono e 24 kHz.
- Arquitetura de runtimes locais, sem integração da API xAI.

## Requisitos

- Windows 11 x64.
- Visual Studio 2022 com **Desenvolvimento para desktop com C++** e Windows SDK.
- CMake 3.22 ou superior.
- JUCE 9.0.6, obtido separadamente e sujeito à respetiva licença.
- Inno Setup 6 apenas para criar o instalador.
- Runtime e modelos do motor de voz escolhido.

## Compilação

Na raiz do repositório, abre PowerShell e define o caminho da JUCE:

```powershell
$env:JUCE_DIR = "C:\caminho\para\JUCE-9.0.6"
Set-ExecutionPolicy -Scope Process Bypass -Force
.\scripts\build-windows.ps1 -JuceDir $env:JUCE_DIR -Config Release
```

O workflow GitHub Actions em `.github/workflows/windows-build.yml` configura e compila a aplicação Windows x64 com Visual Studio 2022 e JUCE 9.0.6. Após uma compilação bem-sucedida, o executável é guardado como artefacto da execução.

## Motores locais

### Pocket TTS

O executável e os modelos Pocket TTS não estão incluídos neste repositório. Instala separadamente um runtime compatível e respeita as licenças do código e dos modelos. O código da aplicação seleciona o identificador `portuguese_24l`.

Projeto upstream: https://github.com/kyutai-labs/pocket-tts

### Sopro V2 Turbo

A integração Sopro utiliza o runtime CLI `audio.cpp` e os ficheiros do modelo Sopro. Antes de instalar ou redistribuir componentes de terceiros, consulta `scripts/install-sopro.ps1`, `INTEGRACAO-SOPRO-0.2.9.23.md` e `docs/SECURITY-AND-LICENSING.md`.

Projetos upstream:
- https://github.com/0xShug0/audio.cpp
- https://huggingface.co/samuel-vitorino/sopro-v2-turbo

A licença do código de um motor não cobre automaticamente todos os executáveis, pesos de modelos ou ficheiros descarregados.

## Documentação

- [Manual de utilização](docs/USER-GUIDE.md)
- [Arquitetura](docs/ARCHITECTURE.md)
- [Relatório de validação Windows](docs/TEST-REPORT-0.2.9.25.md)
- [Notas da versão 0.2.9.25](docs/RELEASE-0.2.9.25.md)
- [Segurança e licenças](docs/SECURITY-AND-LICENSING.md)
- [Contribuir](CONTRIBUTING.md)
- [Comunicação de vulnerabilidades](SECURITY.md)
- [Histórico de versões](CHANGELOG.md)

## Privacidade e utilização responsável

As gravações de voz podem identificar uma pessoa e conter informação pessoal sensível. Usa apenas gravações para as quais tenhas autorização e fundamento legítimo. Não publiques referências de voz, áudio gerado, caches de modelos, tokens ou registos privados.

Não utilizes clonagem de voz para te fazeres passar por outra pessoa, enganar ouvintes ou publicar a voz de terceiros sem autorização.

## Licença

Ainda não foi escolhida uma licença para o código-fonte do Vocloning Native. A visibilidade pública do repositório não concede automaticamente direitos gerais de reutilização, modificação ou redistribuição. JUCE, Pocket TTS, audio.cpp e os modelos têm termos próprios. Consulta `docs/SECURITY-AND-LICENSING.md`.

**Responsável:** INDROBEATZ · https://github.com/INDROBEATZ