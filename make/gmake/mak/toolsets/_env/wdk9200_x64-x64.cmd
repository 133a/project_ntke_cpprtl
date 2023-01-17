set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if defined ProgramFiles(x86) set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"

if not defined WINSDK8.0_FOLDER set "WINSDK8.0_FOLDER=%PROGRAMS_FOLDER.X86%\Windows Kits\8.0"
if not defined MSVS2012_FOLDER set "MSVS2012_FOLDER=%PROGRAMS_FOLDER.X86%\Microsoft Visual Studio 11.0"
set "PATH=%MSVS2012_FOLDER%\VC\bin\amd64;%PATH%"
