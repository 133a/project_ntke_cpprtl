USER_C_FLAGS      =  $(USER_C_FLAGS) /GL- /GS-
C_DEFINES         =  $(C_DEFINES) /DNT_KERNEL_MODE /DDDK_VER=6000
USE_RTTI          =  1
USE_NATIVE_EH     =  ASYNC
TARGETPATH        =  $(MODULE_OUTPUT_DIR)

!IF $(FREEBUILD)
MSC_OPTIMIZATION  =  /Ox  # 'cos of /Oxs or /Os gives unresolved '__EH_prolog' the rtl hasn't yet implemented (x86)
!ENDIF
