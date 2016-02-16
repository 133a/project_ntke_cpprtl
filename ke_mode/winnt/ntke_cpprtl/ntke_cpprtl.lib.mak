### additional defs for modules using ntke_cpprtl lib
### include it at the end of the client's 'module.mak' to override those definitions


module.entry_point      = _NTKE_CPPRTL_DriverEntry
module.path.include    += $(project.dir.root)/ke_mode/winnt/ntke_cpprtl/std_incl
                                
module.lib.depend      += ke_mode/winnt/ntke_cpprtl/ntke_cpprtl


ifndef module.EHcpp
module.EHcpp = a
endif

ifndef module.RTTI
module.RTTI  = 1
endif

