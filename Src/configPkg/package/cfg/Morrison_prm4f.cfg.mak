# invoke SourceDir generated makefile for Morrison.prm4f
Morrison.prm4f: .libraries,Morrison.prm4f
.libraries,Morrison.prm4f: package/cfg/Morrison_prm4f.xdl
	$(MAKE) -f G:\Embedded\Embedded_Workspace\IAREWARM\Morrison\Src/src/makefile.libs

clean::
	$(MAKE) -f G:\Embedded\Embedded_Workspace\IAREWARM\Morrison\Src/src/makefile.libs clean

