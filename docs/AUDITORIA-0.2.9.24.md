# Auditoria estática — Vocloning Native 0.2.9.24 HOTFIX

Data da análise: 25-11-2026  
Base: `VocloningNative-0.2.9.24-HOTFIX-SOURCE (2).zip`

## Resultado da seleção da base

O arquivo recebido contém:
- 931 entradas ZIP;
- aproximadamente 912,8 MB de conteúdo descomprimido;
- uma árvore de código HOTFIX;
- uma árvore adicional `VocloningNative-0.2.9.24-SETUP-SOURCE`;
- duas árvores `build/` com ficheiros gerados;
- executáveis da aplicação, do runtime e do instalador.

A árvore HOTFIX foi escolhida como base porque o seu `Source/MainComponent.cpp` contém a correção descrita no `CHANGELOG-0.2.9.24-HOTFIX.md`.

## Comparação HOTFIX vs SETUP SOURCE

Os ficheiros comuns fora de `build/` são iguais, exceto:

`Source/MainComponent.cpp`
- SETUP SOURCE: 33 848 bytes.
- HOTFIX: 33 988 bytes.
- O HOTFIX remove uma chaveta extra após `contentBounds()` e altera o início vertical da área de conteúdo de `y=132` para `y=18`, com altura ajustada.
- A alteração coincide com a descrição do changelog e é a razão para preferir o HOTFIX.

## Conteúdo removido do repositório preparado

Foram excluídos:
- ambas as pastas `build/` e todos os objetos, bibliotecas, projetos gerados e caches CMake/MSVC;
- `BuildOutput/` (incluía `Vocloning.exe` e uma cópia de `pocket-tts.exe`);
- `dist/` (incluía um instalador compilado);
- cópia duplicada `VocloningNative-0.2.9.24-SETUP-SOURCE/`;
- cópias de segurança `.bak` e `.before-unified-result.bak`;
- `Runtime/pocket-tts.exe`.

O repositório preparado mantém o código C++, assets de marca, scripts, CMake, instruções e fonte do Inno Setup. O runtime Pocket TTS é documentado, mas não incluído.

## Achados e riscos a resolver antes de uma publicação pública

1. **Licenças de terceiros por confirmar.** Confirmar as condições de JUCE, Pocket TTS, audio.cpp/Sopro e dos pesos do modelo. Não assumir que o direito de descarregar equivale a direito de redistribuir.
2. **Script Sopro sem versões fixadas.** `scripts/install-sopro.ps1` usa a última release do repositório audio.cpp e URLs `main`/`resolve/main` do Hugging Face. Não fixa commit/release nem verifica hashes SHA-256. Recomenda-se fixar versões e validar checksums antes de tratar o instalador como cadeia de distribuição fiável.
3. **Alteração de variável de ambiente ao nível da máquina.** O script define `VOCLONING_SOPRO_ROOT` com escopo `Machine`, o que exige elevação e pode não atualizar o ambiente do processo Explorer em execução. Rever se o escopo `User` ou a descoberta pelo caminho da instalação é suficiente.
4. **Instalador não validado nesta auditoria.** O `.iss` referencia `BuildOutput` e `Runtime`; o instalador só pode ser compilado depois de a etapa de packaging criar esses caminhos. Não foi executado Inno Setup.
5. **Sem licença do próprio projeto.** Não foi criado um `LICENSE` arbitrário. O titular deve escolher explicitamente a licença ou manter os direitos reservados.
6. **Validação Windows pendente.** O ambiente de análise não executa MSVC/Windows nem confirma JUCE 9.0.6. A compilação, abertura da GUI, geração real com cada motor e reprodução devem ser testadas no Windows 11.

## Segurança — observações do código

- Não foram encontrados padrões óbvios de chaves API/tokens hardcoded nos ficheiros de texto revistos.
- `PocketTTSBackend.cpp` lê o token local do Hugging Face, se existir, e injeta-o temporariamente no ambiente do processo filho; não é um token incluído no código.
- Os executáveis e modelos foram excluídos do repositório preparado.
- Esta é uma revisão estática limitada, não uma auditoria de segurança formal.

## Estado final

**Preparação GitHub: limpa a nível de estrutura, mas ainda não aprovada como release binária.** Antes de publicar como `Public`, o titular deve decidir a licença, confirmar direitos de terceiros e rever o script de instalação Sopro. Antes de anunciar uma versão funcional, é necessário compilar e testar em Windows 11 com JUCE 9.0.6.
