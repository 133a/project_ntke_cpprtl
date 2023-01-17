set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if defined ProgramFiles(x86) set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"

if not defined WINSDK8.1_FOLDER set "WINSDK8.1_FOLDER=%PROGRAMS_FOLDER.X86%\Windows Kits\8.1"
if not defined MSVS2013_FOLDER set "MSVS2013_FOLDER=%PROGRAMS_FOLDER.X86%\Microsoft Visual Studio 12.0"
set "PATH=%MSVS2013_FOLDER%\VC\bin\amd64_x86;%MSVS2013_FOLDER%\VC\bin\amd64;%PATH%"
