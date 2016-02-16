@echo off

setlocal

set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if '%PROCESSOR_ARCHITECTURE%'=='x86' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"
)

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.1.exe"

if not defined WDK710_FOLDER set "WDK710_FOLDER=%SystemDrive%\WinDDK\7600.16385.1"
if not defined INTELCL13.0_FOLDER set "INTELCL13.0_FOLDER=%PROGRAMS_FOLDER.X86%\Intel\Composer XE 2013"
set "PATH=%INTELCL13.0_FOLDER%\bin\intel64;%WDK710_FOLDER%\bin\x86\amd64;%PATH%"

@echo on
"%GNU_MAKE_EXE%" target.toolset=icl13.0_wdk7.1.0_x64-x64 target.profile=debug target.kernel=nt6.1
"%GNU_MAKE_EXE%" target.toolset=icl13.0_wdk7.1.0_x64-x64 target.profile=release target.kernel=nt6.1
@echo off

endlocal

:end
