@echo off

setlocal

set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
if '%PROCESSOR_ARCHITECTURE%'=='x86' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles%"
) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
  set "PROGRAMS_FOLDER.X86=%ProgramFiles(x86)%"
)

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.1.exe"

if not defined DDK3790SP1_FOLDER set "DDK3790SP1_FOLDER=%SystemDrive%\WinDDK\3790.1830"
set "PATH=%DDK3790SP1_FOLDER%\bin\win64\x86\amd64;%PATH%"

@echo on
"%GNU_MAKE_EXE%" target.toolset=ddk3790sp1_x86-x64 target.profile=debug target.kernel=nt5.2
"%GNU_MAKE_EXE%" target.toolset=ddk3790sp1_x86-x64 target.profile=release target.kernel=nt5.2
@echo off

endlocal

:end
