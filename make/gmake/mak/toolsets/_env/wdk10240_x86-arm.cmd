set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if defined ProgramFiles(x86) set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"

if not defined WINSDK10_FOLDER set "WINSDK10_FOLDER=%PROGRAMS_FOLDER.X86%\Windows Kits\10"
if not defined MSVS2015_FOLDER set "MSVS2015_FOLDER=%PROGRAMS_FOLDER.X86%\Microsoft Visual Studio 14.0"
set "PATH=%MSVS2015_FOLDER%\VC\bin\x86_arm;%MSVS2015_FOLDER%\VC\bin;%MSVS2015_FOLDER%\Common7\IDE;%PATH%"
