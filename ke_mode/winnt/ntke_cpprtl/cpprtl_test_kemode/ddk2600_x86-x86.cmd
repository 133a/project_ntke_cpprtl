@echo off

setlocal

set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if '%PROCESSOR_ARCHITECTURE%'=='x86' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"
)

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.2.1.exe"

if not defined DDK2600_FOLDER set "DDK2600_FOLDER=%SystemDrive%\WinDDK\2600"

set "PATH=%DDK2600_FOLDER%\bin\x86;%PATH%"


@echo on
"%GNU_MAKE_EXE%"  target.toolset=ddk2600_x86-x86  target.profile=cpprtl_debug    target.kernel=nt5.0
"%GNU_MAKE_EXE%"  target.toolset=ddk2600_x86-x86  target.profile=cpprtl_release  target.kernel=nt5.0
"%GNU_MAKE_EXE%"  target.toolset=ddk2600_x86-x86  target.profile=cpprtl_debug    target.kernel=nt5.1
"%GNU_MAKE_EXE%"  target.toolset=ddk2600_x86-x86  target.profile=cpprtl_release  target.kernel=nt5.1
@echo off


endlocal

:end
