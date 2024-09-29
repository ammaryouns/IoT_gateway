#
#  ======== products.mak ========
#
#
# Read the http://processors.wiki.ti.com/index.php/Using_wolfSSL_with_TI-RTOS
# for instructions to download the software required.

# XDC_INSTALL_DIR is the path to XDCtools directory.
XDC_INSTALL_DIR        = C:/ti/xdctools_3_32_00_06_core

# BIOS_INSTALL_DIR is the path to TI-RTOS Kernel (SYS/BIOS) directory. If you
# have installed TI-RTOS, it is located in the products/bios_* path.
BIOS_INSTALL_DIR       = C:/ti/tirtos_tivac_2_16_01_14/products/bios_6_45_02_31

# NDK_INSTALL_DIR is the path to TI-RTOS NDK directory. If you have
# installed TI-RTOS, it is located in the products/ndk_* path.
NDK_INSTALL_DIR        = C:/ti/tirtos_tivac_2_16_01_14/products/ndk_2_25_00_09

# TIVAWARE_INSTALL_DIR is the path to Tivaware driverlib directory. If you have
# installed TI-RTOS, it is located in the products/TivaWare_* path.
TIVAWARE_INSTALL_DIR   = C:/ti/tirtos_tivac_2_16_01_14/products/TivaWare_C_Series-2.1.1.71b

# Define the code generation tools path for TI, IAR and GCC ARM compilers.
# If you have installed Code Composer Studio, the TI and GCC compiler are
# located in the ccsv*/tools/compiler/* path.
#
# Leave assignment empty to disable any toolchain.
ti.targets.arm.elf.M4F =
iar.targets.arm.M4F    = C:/IAR-Systems/Embedded-Workbench-7.2/arm
gnu.targets.arm.M4F    =
