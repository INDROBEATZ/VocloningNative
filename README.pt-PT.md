# Vocloning Native

**Estúdio de voz nativo para Windows · C++20 · JUCE 9.0.6**

**Base de código:** `0.2.9.24 HOTFIX`  
**Próxima linha de desenvolvimento:** `0.2.9.25` (em preparação; ainda não é uma release validada)

O Vocloning Native é uma aplicação desktop para Windows construída em C++ e JUCE. Permite importar referências de voz, escrever texto, gerar voz através de motores instalados localmente, pré-visualizar o áudio e exportar WAV.

O projeto privilegia execução local e **não integra a xAI API**.

## Funcionalidades identificadas no código

- Interface gráfica nativa JUCE.
- Importação de referências WAV e MP3.
- Biblioteca de referências e fluxo de escrita de texto.
- Backend Pocket TTS configurado para o identificador de modelo `portuguese_24l`.
- Backend Sopro V2 Turbo através do runtime CLI `audio.cpp`, com opção de geração em português europeu.
- Validação do WAV produzido pelo backend Sopro: leitura, amostras finitas, ausência de silêncio, mono e 24 kHz.
- Reprodução e exportação de WAV.

A disponibilidade efetiva dos motores depende dos executáveis, modelos, hardware e compatibilidade das respetivas versões instaladas.

## Requisitos

- Windows 11 x64.
- Visual Studio 2022 com a carga de trabalho **Desenvolvimento para desktop com C++** e Windows SDK.
- CMake 3.22 ou superior.
- JUCE 9.0.6, obtido separadamente e sujeito à respetiva licença.
- Inno Setup 6 apenas para criar o instalador.
- Runtime e modelos Pocket TTS e/ou Sopro, conforme o motor utilizado.

## Compilar

Na raiz do repositório, abre PowerShell e define o caminho da JUCE:

```powershell
$env:JUCE_DIR = "C:\caminho\para\JUCE-9.0.6"
Set-ExecutionPolicy -Scope Process Bypass -Force
.\scripts\build-windows.ps1 -JuceDir $env:JUCE_DIR -Config Release
```

Executável esperado: `build\VocloningNative_artefacts\Release\Vocloning.exe`.

O workflow GitHub Actions executa uma compilação limpa Windows x64 com MSVC e JUCE 9.0.6. Uma compilação bem-sucedida não substitui os testes funcionais dos motores e dos modelos.

## Motores locais

- **Pocket TTS:** o executável e os modelos não são incluídos no repositório. O backend seleciona `portuguese_24l`. Consulta [Runtime/README.txt](Runtime/README.txt) e os termos do [projeto Kyutai](https://github.com/kyutai-labs/pocket-tts).
- **Sopro V2 Turbo:** utiliza o CLI `audio.cpp` e os ficheiros do modelo Sopro. O script atual descarrega a última release e ficheiros de branches móveis, sem fixar versões nem verificar hashes SHA-256. Revê-o antes de executar.

Não redistribuas binários ou pesos de terceiros sem confirmar as respetivas licenças.

## Documentação

- [Arquitetura](docs/ARCHITECTURE.md)
- [Segurança e licenças](docs/SECURITY-AND-LICENSING.md)
- [Auditoria estática 0.2.9.24](docs/AUDITORIA-0.2.9.24.md)
- [Notas de integração Sopro](INTEGRACAO-SOPRO-0.2.9.23.md)
- [Plano 0.2.9.25](docs/RELEASE-0.2.9.25-PLAN.md)

## Licença

A licença do código do Vocloning Native ainda não foi escolhida pelo titular. A visibilidade pública do repositório não concede automaticamente direitos de reutilização, modificação ou redistribuição. JUCE, Pocket TTS, audio.cpp e os pesos dos modelos têm condições próprias.

## Privacidade

As gravações de referência e os áudios gerados podem conter dados pessoais sensíveis. Processa apenas gravações para as quais tenhas autorização. Nunca publiques tokens, amostras privadas, modelos, caches ou ficheiros gerados.

**Responsável pelo projeto:** INDROBEATZ · [GitHub](https://github.com/INDROBEATZ/VocloningNative)