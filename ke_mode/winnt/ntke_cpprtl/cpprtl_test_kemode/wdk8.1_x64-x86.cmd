@echo off

setlocal

set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if '%PROCESSOR_ARCHITECTURE%'=='x86' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"
)

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.2.1.exe"

if not defined WINSDK8.1_FOLDER set "WINSDK8.1_FOLDER=%PROGRAMS_FOLDER.X86%\Windows Kits\8.1"
if not defined MSVS2013_FOLDER set "MSVS2013_FOLDER=%PROGRAMS_FOLDER.X86%\msvs2013"

set "PATH=%MSVS2013_FOLDER%\VC\bin\amd64_x86;%MSVS2013_FOLDER%\VC\bin\amd64;%PATH%"


@echo on
"%GNU_MAKE_EXE%"  target.toolset=wdk8.1_x64-x86  target.profile=cpprtl_debug    target.kernel=nt6.3
"%GNU_MAKE_EXE%"  target.toolset=wdk8.1_x64-x86  target.profile=cpprtl_release  target.kernel=nt6.3
@echo off


endlocal

:end
