; Putanja do .vst3 foldera se salje iz build.yml preko /DVst3Dir=...
; Ako se pokrece rucno, koristi se podrazumevana putanja ispod.
#ifndef Vst3Dir
  #define Vst3Dir "..\build\AtlasRetro_artefacts\Release\VST3\Atlas Retro.vst3"
#endif

[Setup]
AppName=Atlas Retro
AppVersion=1.0.0
AppPublisher=ProdByPerke
DefaultDirName={commoncf64}\VST3
DisableDirPage=yes
DisableProgramGroupPage=yes
OutputDir=Output
OutputBaseFilename=AtlasRetro_Setup
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible
PrivilegesRequired=admin
Compression=lzma2
SolidCompression=yes
WizardStyle=modern
UninstallDisplayName=Atlas Retro (VST3)

[Files]
Source: "{#Vst3Dir}\*"; DestDir: "{commoncf64}\VST3\Atlas Retro.vst3"; Flags: ignoreversion recursesubdirs createallsubdirs

[Messages]
FinishedLabel=Atlas Retro je instaliran u C:\Program Files\Common Files\VST3.%nU FL Studio-u: Options > Manage plugins > Find installed plugins.
