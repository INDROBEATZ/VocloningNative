# Vocloning Native 0.2.9.24 — hotfix de compilação e alinhamento

## Correções
- Corrigido erro de sintaxe em `MainComponent::contentBounds()` (chaveta extra após o fecho da função), que impedia a compilação MSVC.
- Corrigido o início vertical da área de conteúdo: o deslocamento anterior de 132 px empurrava títulos e cartões para baixo, provocando desalinhamento e cortes. A área passa a começar abaixo do cabeçalho, com margem superior de 18 px.
- Mantida a geometria comum dos controlos da página Texto: seletor, editor, contador e botões continuam ancorados ao mesmo painel, preservando os motores Sopro e Pocket TTS.

## Validação
- Verificação estática do bloco `contentBounds()` e da estrutura C++ realizada.
- Compilação Windows/MSVC e criação do Setup.exe ainda precisam de ser executadas no Windows do utilizador com JUCE 9.0.6 e Inno Setup 6.
- Não alterar nem substituir os ficheiros do modelo Sopro já validados sem necessidade.
