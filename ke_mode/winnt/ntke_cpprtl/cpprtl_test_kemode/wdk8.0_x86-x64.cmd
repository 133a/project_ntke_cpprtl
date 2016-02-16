@echo off

setlocal

set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if '%PROCESSOR_ARCHITECTURE%'=='x86' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"
)

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.1.exe"

if not defined WINSDK_8.0_FOLDER set "WINSDK_8.0_FOLDER=%PROGRAMS_FOLDER.X86%\Windows Kits\8.0"
if not defined MSVS2012_FOLDER set "MSVS2012_FOLDER=%PROGRAMS_FOLDER.X86%\msvs2012"
set "PATH=%MSVS2012_FOLDER%\VC\bin\x86_amd64;%MSVS2012_FOLDER%\Common7\IDE;%PATH%"

@echo on
"%GNU_MAKE_EXE%" target.toolset=wdk8.0_x86-x64 target.profile=debug target.kernel=nt6.2
"%GNU_MAKE_EXE%" target.toolset=wdk8.0_x86-x64 target.profile=release target.kernel=nt6.2
@echo off

endlocal

:end
