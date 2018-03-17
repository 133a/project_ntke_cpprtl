@echo off

setlocal

if not defined GNU_MAKE_EXE set "GNU_MAKE_EXE=%SystemDrive%\__bin\gmake\gmake-4.2.1.exe"
if not defined EWDK16299_FOLDER set "EWDK16299_FOLDER=%SystemDrive%\__bin\EWDK.16299"

set "PATH=%EWDK16299_FOLDER%\Program Files\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.11.25503\bin\Hostx64\arm;%EWDK16299_FOLDER%\Program Files\Microsoft Visual Studio\2017\BuildTools\VC\Tools\MSVC\14.11.25503\bin\Hostx64\x64;%PATH%"


@echo on
"%GNU_MAKE_EXE%"  target.toolset=ewdk16299-km_x64-arm  target.profile=cpprtl_debug    target.kernel=nt6.2
"%GNU_MAKE_EXE%"  target.toolset=ewdk16299-km_x64-arm  target.profile=cpprtl_release  target.kernel=nt6.2
"%GNU_MAKE_EXE%"  target.toolset=ewdk16299-km_x64-arm  target.profile=cpprtl_debug    target.kernel=nt6.3
"%GNU_MAKE_EXE%"  target.toolset=ewdk16299-km_x64-arm  target.profile=cpprtl_release  target.kernel=nt6.3
"%GNU_MAKE_EXE%"  target.toolset=ewdk16299-km_x64-arm  target.profile=cpprtl_debug    target.kernel=nt10.0
"%GNU_MAKE_EXE%"  target.toolset=ewdk16299-km_x64-arm  target.profile=cpprtl_release  target.kernel=nt10.0
@echo off


endlocal

:end
