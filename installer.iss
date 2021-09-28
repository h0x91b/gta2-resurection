[Setup]
AppName=GTA Resurected
AppVersion=13.0
WizardStyle=modern
DefaultDirName={autopf}\gta2
DefaultGroupName=GTA2
UninstallDisplayIcon={app}\gta2-resurected.exe
Compression=lzma2
SolidCompression=yes
OutputDir=userdocs:Inno Setup Examples Output

[Files]
Source: "Debug\gta2-resurection.dll"; DestDir: "{app}"
Source: "Readme.txt"; DestDir: "{app}"; Flags: isreadme

[Icons]
Name: "{group}\My Program"; Filename: "{app}\gta2.exe"
