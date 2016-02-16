@echo off

setlocal

set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if '%PROCESSOR_ARCHITECTURE%'=='x86' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"
)

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.1.exe"

if not defined WDK61SP1_FOLDER set "WDK61SP1_FOLDER=%SystemDrive%\WinDDK\6001.18002"
set "PATH=%WDK61SP1_FOLDER%\bin\x86;%WDK61SP1_FOLDER%\bin\x86\x86;%PATH%"

@echo on
"%GNU_MAKE_EXE%" target.toolset=wdk6.1sp1_x86-x86 target.profile=debug target.kernel=nt6.0
"%GNU_MAKE_EXE%" target.toolset=wdk6.1sp1_x86-x86 target.profile=release target.kernel=nt6.0
@echo off

endlocal

:end
