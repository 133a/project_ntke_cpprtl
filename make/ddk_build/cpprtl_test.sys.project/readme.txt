The following toolkits are supported (x86/x64): WDK 7600.16385.1

How to build:
(1) start WDK build environment;
(2) make the folder '$(PROJECT_ROOT)\make\ddk_build\cpprtl_test.sys.project' current;
(3) invoke 'build.exe';
(4) the resulting binaries are placed at '$(PROJECT_ROOT)\__output.ddk'.

Note:
if the 'free' build environment is used set the additional variable to suppress warnings to be considered as errors
- 'set BUILD_ALLOW_ALL_WARNINGS=1'
