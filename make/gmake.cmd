:: %1=target.toolset

set "project_folder=\ke_mode\winnt\ntke_cpprtl\cpprtl_test_kemode"

set "x86_make_exe=\make\gmake\bin\x86\gmake-4.4.exe"
set "x64_make_exe=\make\gmake\bin\x64\gmake-4.4.exe"
set "make_exe=%x64_make_exe%"
if "%PROCESSOR_ARCHITECTURE%"=="x86" if not defined PROCESSOR_ARCHITEW6432 set "make_exe=%x86_make_exe%"

for %%k in (%make_kernels%) do (
  for %%p in (cpprtl_debug cpprtl_release) do (
    ".%make_exe%" --directory=".%project_folder%" target.toolset=%1 target.profile=%%p target.kernel=%%k
    if errorlevel 1 goto end
  )
)

:end
