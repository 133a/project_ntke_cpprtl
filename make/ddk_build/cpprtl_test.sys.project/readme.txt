The building is checked to be completed with the following toolkits (both x86 and x64):
- WDK 6001.18002  (wdk6.1sp1);
- WDK 7600.16385.1  (wdk7.1.0).


How to build:
(1) run the appropriate WDK build environment;
(2) make the folder '$(PROJECT_ROOT)\make\ddk_build\cpprtl_test.sys.project' to be the current;
(3) invoke the 'build.exe';
(4) find the resulting binaries at the '$(PROJECT_ROOT)\__output.ddk'.


Note:
if the 'free' build environment is used set the additional variable to suppress the warnings to be considered as errors - 'set BUILD_ALLOW_ALL_WARNINGS=1'

