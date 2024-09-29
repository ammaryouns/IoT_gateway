#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages;C:/ti/tirtos_tivac_2_16_01_14/products/ndk_2_25_00_09/packages;C:/ti/wolfssl-3.9.10/tirtos/../;C:/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b
override XDCROOT = C:/ti/xdctools_3_32_00_06_core
override XDCBUILDCFG = C:/ti/wolfssl-3.9.10/tirtos/wolfssl.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = ti.targets.arm.elf.M4F="" gnu.targets.arm.M4F="" iar.targets.arm.M4F="C:/IAR-Systems/Embedded-Workbench-7.2/arm" NDK="C:/ti/tirtos_tivac_2_16_01_14/products/ndk_2_25_00_09"
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31/packages;C:/ti/tirtos_tivac_2_16_01_14/products/ndk_2_25_00_09/packages;C:/ti/wolfssl-3.9.10/tirtos/../;C:/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b;C:/ti/xdctools_3_32_00_06_core/packages;../../..
HOSTOS = Windows
endif
