  if '%PROCESSOR_ARCHITECTURE%'=='x86' (
    set "ARCH_DIR=x86"
  ) else if '%PROCESSOR_ARCHITECTURE%'=='AMD64' (
    set "ARCH_DIR=x64"
  ) else if '%PROCESSOR_ARCHITECTURE%'=='ARM' (
    set "ARCH_DIR=arm"
  ) else if '%PROCESSOR_ARCHITECTURE%'=='ARM64' (
    set "ARCH_DIR=arm64"
  ) else (
    goto error
  )

  set "GMAKE_TARGET_DIR=%SystemDrive%\__bin\gmake"
  set "GMAKE_SOURCE_DIR=%ARCH_DIR%"
  set "GMAKE_EXE_FILE=gmake-4.2.1.exe"

:create_target_dir
  mkdir "%GMAKE_TARGET_DIR%"

:copy_file
  copy /B /Y ".\%GMAKE_SOURCE_DIR%\%GMAKE_EXE_FILE%" "%GMAKE_TARGET_DIR%"
  goto end

:error
  goto end

:end

