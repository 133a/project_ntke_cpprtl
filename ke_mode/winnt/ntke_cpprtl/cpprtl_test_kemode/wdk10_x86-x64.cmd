@echo off

setlocal

set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if '%PROCESSOR_ARCHITECTURE%'=='x86' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"
)

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.2.1.exe"

if not defined WINSDK10_FOLDER set "WINSDK10_FOLDER=%PROGRAMS_FOLDER.X86%\Windows Kits\10"
if not defined MSVS2015_FOLDER set "MSVS2015_FOLDER=%PROGRAMS_FOLDER.X86%\msvs2015"

set "PATH=%MSVS2015_FOLDER%\VC\bin\x86_amd64;%MSVS2015_FOLDER%\VC\bin;%MSVS2015_FOLDER%\Common7\IDE;%PATH%"


@echo on
"%GNU_MAKE_EXE%"  target.toolset=wdk10_x86-x64  target.profile=cpprtl_debug    target.kernel=nt10.0
"%GNU_MAKE_EXE%"  target.toolset=wdk10_x86-x64  target.profile=cpprtl_release  target.kernel=nt10.0
@echo off


endlocal

:end
