; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Medusa Beta"
#define MyAppVersion "0.17"
#define MyAppPublisher "Medusa haxors team"
#define MyAppURL "http://medusa.swap.gs"
#define MyAppExeName "medusa_qt.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{E6D138D6-280C-455E-8605-6C7A5DCE6CBA}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={#MyAppName} {#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
OutputDir=C:\Users\Eddy\Documents\EPITECH\Projet\medusa
OutputBaseFilename=medusa_setup_beta
Compression=lzma
SolidCompression=yes

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked; OnlyBelowVersion: 0,6.1

[Files]
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\medusa_qt.exe"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\arch_arm.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\arch_gb.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\arch_x86.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\boost_filesystem-vc100-mt-1_46.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\boost_system-vc100-mt-1_46.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\ColorPicker.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\FontPicker.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\ldr_bs.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\ldr_elf.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\ldr_gb.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\ldr_pe.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\ldr_raw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\Medusa.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\medusa\build\bin\sqlite.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Qt\4.7.1\bin\QtCore4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Qt\4.7.1\bin\QtGui4.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "C:\Users\Eddy\Documents\EPITECH\Projet\vcredist_x86.exe"; DestDir: "{app}"; Flags: deleteafterinstall ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; WorkingDir: "{app}"
Name: "{group}\{cm:ProgramOnTheWeb,{#MyAppName}}"; Filename: "{#MyAppURL}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{commondesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon; WorkingDir: "{app}"

[Run]
Filename: "{app}\vcredist_x86.exe"; StatusMsg: "Installing Visual C++ 2010 Redistribuable";
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, "&", "&&")}}"; Flags: nowait postinstall skipifsilent

