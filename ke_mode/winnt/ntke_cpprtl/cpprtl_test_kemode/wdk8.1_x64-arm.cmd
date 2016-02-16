@echo off

setlocal

set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if '%PROCESSOR_ARCHITECTURE%'=='x86' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"
)

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.1.exe"

if not defined WINSDK_8.1_FOLDER set "WINSDK_8.1_FOLDER=%PROGRAMS_FOLDER.X86%\Windows Kits\8.1"
if not defined MSVS2013_FOLDER set "MSVS2013_FOLDER=%PROGRAMS_FOLDER.X86%\msvs2013"
set "PATH=%MSVS2013_FOLDER%\VC\bin\amd64_arm;%MSVS2013_FOLDER%\VC\bin\amd64;%PATH%"

@echo on
"%GNU_MAKE_EXE%" target.toolset=wdk8.1_x64-arm target.profile=debug target.kernel=nt6.3
"%GNU_MAKE_EXE%" target.toolset=wdk8.1_x64-arm target.profile=release target.kernel=nt6.3
@echo off

endlocal

:end
