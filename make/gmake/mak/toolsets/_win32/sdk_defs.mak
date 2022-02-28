ifndef sdk_defs.included  # include guard
  sdk_defs.included = 1


######################################################################################################
######################################################################################################
###
###    win32 sdk common definitions
###
######################################################################################################
######################################################################################################


  sdk.enum  = $(target.platform) \
              $(target.platform).$(target.kernel) \
              $(target.platform).$(target.arch)


# unicode
ifeq '$(target.charset)'  'unicode'  # sdk     crt
  sdk.definitions                    = UNICODE _UNICODE
endif


# common
  sdk.definitions.win32         = WIN32


# target.arch (_WIN32 is predefined, _WIN64 is predefined for 64-bit targets)

                                # _M_IX86 is predefined
  sdk.definitions.win32.x86     = _X86_

                                # _M_X64 _M_AMD64 are predefined
  sdk.definitions.win32.x64     = _AMD64_ AMD64 WIN64

                                # _M_IA64 is predefined
  sdk.definitions.win32.ia64    = _IA64_ WIN64

                                # _M_ARM is predefined
  sdk.definitions.win32.arm     = _ARM_ 

                                # _M_ARM64 is predefined
  sdk.definitions.win32.arm64   = _ARM64_ WIN64


# target.kernel
  sdk.definitions.win32.nt5.0   = _WIN32_WINNT=0x0500 WINVER=0x0500 NTDDI_VERSION=0x05000400
  sdk.definitions.win32.nt5.1   = _WIN32_WINNT=0x0501 WINVER=0x0501 NTDDI_VERSION=0x05010300
  sdk.definitions.win32.nt5.2   = _WIN32_WINNT=0x0502 WINVER=0x0502 NTDDI_VERSION=0x05020200
  sdk.definitions.win32.nt6.0   = _WIN32_WINNT=0x0600 WINVER=0x0600 NTDDI_VERSION=0x06000000
  sdk.definitions.win32.nt6.1   = _WIN32_WINNT=0x0601 WINVER=0x0601 NTDDI_VERSION=0x06010000
  sdk.definitions.win32.nt6.2   = _WIN32_WINNT=0x0602 WINVER=0x0602 NTDDI_VERSION=0x06020000
  sdk.definitions.win32.nt6.3   = _WIN32_WINNT=0x0603 WINVER=0x0603 NTDDI_VERSION=0x06030000
  sdk.definitions.win32.nt10.0  = _WIN32_WINNT=0x0A00 WINVER=0x0A00 NTDDI_VERSION=0x0A000000


endif  # include guard

