USER_C_FLAGS      =  $(USER_C_FLAGS) /GL-
C_DEFINES         =  $(C_DEFINES) /DNT_KERNEL_MODE /DDDK_VER=7600
USE_RTTI          =  1
USE_NATIVE_EH     =  1
TARGETPATH        =  $(MODULE_OUTPUT_DIR)

!IF $(FREEBUILD)
MSC_OPTIMIZATION  =  /Ox  # /Oxs or /Os requires '__EH_prolog' (x86)
!ENDIF
