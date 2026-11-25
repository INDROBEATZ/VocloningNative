#define AppName "Vocloning Native"
#define AppVersion "0.2.9.24"
#define AppPublisher "INDRO"
#define AppExeName "Vocloning.exe"

[Setup]
AppId={{E5A6D5B1-4F5D-4D43-9A81-029240000001}
AppName={#AppName}
AppVersion={#AppVersion}
AppPublisher={#AppPublisher}
DefaultDirName={autopf}\INDRO\Vocloning
DefaultGroupName=Vocloning Native
DisableProgramGroupPage=yes
OutputDir=..\dist
OutputBaseFilename=VocloningNative-Setup-{#AppVersion}
SetupIconFile=..\Assets\Vocloning.ico
WizardImageFile=..\InstallerAssets\VocloningWizard.bmp
WizardSmallImageFile=..\InstallerAssets\VocloningSmall.bmp
Compression=lzma2/ultra64
SolidCompression=yes
WizardStyle=modern
ArchitecturesInstallIn64BitMode=x64
ArchitecturesAllowed=x64
PrivilegesRequired=admin
UninstallDisplayIcon={app}\{#AppExeName}
SetupLogging=yes

[Languages]
Name: "portuguese"; MessagesFile: "compiler:Languages\Portuguese.isl"

[Tasks]
Name: "desktopicon"; Description: "Criar atalho no Ambiente de Trabalho"; GroupDescription: "Atalhos:"; Flags: unchecked

[Files]
Source: "..\BuildOutput\Vocloning.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\BuildOutput\Runtime\*"; DestDir: "{app}\Runtime"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\scripts\install-sopro.ps1"; DestDir: "{app}\SetupSupport"; Flags: ignoreversion
Source: "..\Assets\Vocloning.ico"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
Name: "{group}\Vocloning Native"; Filename: "{app}\{#AppExeName}"; IconFilename: "{app}\Vocloning.ico"
Name: "{autodesktop}\Vocloning Native"; Filename: "{app}\{#AppExeName}"; IconFilename: "{app}\Vocloning.ico"; Tasks: desktopicon

[Run]
Filename: "{sys}\WindowsPowerShell\v1.0\powershell.exe"; Parameters: "-NoProfile -ExecutionPolicy Bypass -File ""{app}\SetupSupport\install-sopro.ps1"" -InstallRoot ""{app}\Runtime\Sopro\audio.cpp"""; StatusMsg: "A verificar e instalar o motor de clonagem Sopro V2 Turbo (pode demorar e requer Internet)..."; Flags: runhidden waituntilterminated
Filename: "{app}\{#AppExeName}"; Description: "Abrir Vocloning Native"; Flags: postinstall nowait skipifsilent

[UninstallDelete]
Type: filesandordirs; Name: "{app}\SetupSupport"
