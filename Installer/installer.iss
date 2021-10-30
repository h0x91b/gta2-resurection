[Setup]
AppName=GTA2 Resurected
AppVersion=13.0
WizardStyle=modern
DefaultDirName={reg:HKCU\SOFTWARE\GTA2 Game Hunter,GTA2Folder|D:\games\gta2}
DefaultGroupName=GTA2
DisableProgramGroupPage=yes
UninstallDisplayName=GTA2-Resurected
UninstallDisplayIcon={app}\gta2-resurected.exe
Compression=lzma2
SolidCompression=yes
OutputDir=userdocs:Inno Setup Examples Output
DirExistsWarning=no
AppendDefaultDirName=false

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; \
    GroupDescription: "{cm:AdditionalIcons}";

[Files]
; Source: "..\Debug\gta2-resurection.dll"; DestDir: "{app}"
Source: "..\Release\gta2-resurection.dll"; DestDir: "{app}"
Source: "dgVoodooCpl.exe"; DestDir: "{app}"
Source: "dgVoodoo.conf"; DestDir: "{app}"
Source: "gta2-resurected.exe"; DestDir: "{app}"
Source: "*.dll"; DestDir: "{app}"
Source: "clua\*.dll"; DestDir: "{app}\scripts"
Source: "imgui.ini"; DestDir: "{app}"
Source: "..\gta2-resurection\*.lua"; DestDir: "{app}\scripts"
Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{userdesktop}\GTA 2 Resurected"; Filename: "{app}\gta2-resurected.exe"; \
    IconFilename: "{app}\gta2-resurected.exe"; Tasks: desktopicon

[Registry]

Root: HKCU; Subkey: "Software\DMA Design Ltd\GTA2\screen"; ValueType: dword; ValueName: "start_mode"; ValueData: "$0"; 

[Code]

{ Below code warns end user if he tries to install into a folder that does not gta2.exe }
function NextButtonClick(CurPageID: Integer): Boolean;
begin
  Log('NextButtonClick(' + IntToStr(CurPageID) + ') called');
  case CurPageID of
    wpSelectDir:
    if not FileExists(ExpandConstant('{app}\gta2.exe')) then begin
      MsgBox('Setup has detected that that this is not the main program folder of a GTA 2 install, and the created shortcuts to launch GTA 2 resurected will not work.' #13#13 'You should probably go back and browse for a valid GTA 2 folder (and not any subfolders like addon).', mbError, MB_OK);
      end;
    wpReady:
  end;
  Result := True;
end;
