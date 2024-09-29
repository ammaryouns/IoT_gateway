## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,rm4f linker.cmd package/cfg/Morrison_prm4f.orm4f

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/Morrison_prm4f.xdl
	$(SED) 's"^\"\(package/cfg/Morrison_prm4fcfg.cmd\)\"$""\"F:/Embedded/Embedded_Workspace/IAREWARM/Morrison/Src/configPkg/\1\""' package/cfg/Morrison_prm4f.xdl > $@
	-$(SETDATE) -r:max package/cfg/Morrison_prm4f.h compiler.opt compiler.opt.defs
