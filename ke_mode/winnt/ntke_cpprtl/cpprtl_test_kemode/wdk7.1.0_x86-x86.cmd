@echo off

setlocal

set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if '%PROCESSOR_ARCHITECTURE%'=='x86' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"
)

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.2.1.exe"

if not defined WDK710_FOLDER set "WDK710_FOLDER=%SystemDrive%\WinDDK\7600.16385.1"

set "PATH=%WDK710_FOLDER%\bin\x86;%WDK710_FOLDER%\bin\x86\x86;%PATH%"


@echo on
"%GNU_MAKE_EXE%"  target.toolset=wdk7.1.0_x86-x86  target.profile=cpprtl_debug    target.kernel=nt5.1
"%GNU_MAKE_EXE%"  target.toolset=wdk7.1.0_x86-x86  target.profile=cpprtl_release  target.kernel=nt5.1
"%GNU_MAKE_EXE%"  target.toolset=wdk7.1.0_x86-x86  target.profile=cpprtl_debug    target.kernel=nt6.1
"%GNU_MAKE_EXE%"  target.toolset=wdk7.1.0_x86-x86  target.profile=cpprtl_release  target.kernel=nt6.1
@echo off
 

endlocal

:end
