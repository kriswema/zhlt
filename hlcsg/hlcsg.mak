# Microsoft Developer Studio Generated NMAKE File, Based on hlcsg.dsp
!IF "$(CFG)" == ""
CFG=hlcsg - Win32 Super_Debug
!MESSAGE No configuration specified. Defaulting to hlcsg - Win32 Super_Debug.
!ENDIF 

!IF "$(CFG)" != "hlcsg - Win32 Release" && "$(CFG)" != "hlcsg - Win32 Debug" && "$(CFG)" != "hlcsg - Win32 Release w Symbols" && "$(CFG)" != "hlcsg - Win32 Super_Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hlcsg.mak" CFG="hlcsg - Win32 Super_Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hlcsg - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "hlcsg - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "hlcsg - Win32 Release w Symbols" (based on "Win32 (x86) Console Application")
!MESSAGE "hlcsg - Win32 Super_Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "hlcsg - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\hlcsg.exe" "$(OUTDIR)\hlcsg.bsc"


CLEAN :
	-@erase "$(INTDIR)\autowad.obj"
	-@erase "$(INTDIR)\autowad.sbr"
	-@erase "$(INTDIR)\blockmem.obj"
	-@erase "$(INTDIR)\blockmem.sbr"
	-@erase "$(INTDIR)\brush.obj"
	-@erase "$(INTDIR)\brush.sbr"
	-@erase "$(INTDIR)\brushunion.obj"
	-@erase "$(INTDIR)\brushunion.sbr"
	-@erase "$(INTDIR)\bspfile.obj"
	-@erase "$(INTDIR)\bspfile.sbr"
	-@erase "$(INTDIR)\cmdlib.obj"
	-@erase "$(INTDIR)\cmdlib.sbr"
	-@erase "$(INTDIR)\filelib.obj"
	-@erase "$(INTDIR)\filelib.sbr"
	-@erase "$(INTDIR)\files.obj"
	-@erase "$(INTDIR)\files.sbr"
	-@erase "$(INTDIR)\hullfile.obj"
	-@erase "$(INTDIR)\hullfile.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\map.sbr"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\mathlib.sbr"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messages.sbr"
	-@erase "$(INTDIR)\qcsg.obj"
	-@erase "$(INTDIR)\qcsg.sbr"
	-@erase "$(INTDIR)\scriplib.obj"
	-@erase "$(INTDIR)\scriplib.sbr"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\textures.sbr"
	-@erase "$(INTDIR)\threads.obj"
	-@erase "$(INTDIR)\threads.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\wadcfg.obj"
	-@erase "$(INTDIR)\wadcfg.sbr"
	-@erase "$(INTDIR)\wadinclude.obj"
	-@erase "$(INTDIR)\wadinclude.sbr"
	-@erase "$(INTDIR)\wadpath.obj"
	-@erase "$(INTDIR)\wadpath.sbr"
	-@erase "$(INTDIR)\winding.obj"
	-@erase "$(INTDIR)\winding.sbr"
	-@erase "$(OUTDIR)\hlcsg.bsc"
	-@erase "$(OUTDIR)\hlcsg.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\common" /I "..\template" /D "NDEBUG" /D "HLCSG" /D "_MBCS" /D "DOUBLEVEC_T" /D "STDC_HEADERS" /D "WIN32" /D "_CONSOLE" /D "SYSTEM_WIN32" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hlcsg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hlcsg.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\blockmem.sbr" \
	"$(INTDIR)\bspfile.sbr" \
	"$(INTDIR)\cmdlib.sbr" \
	"$(INTDIR)\filelib.sbr" \
	"$(INTDIR)\files.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\mathlib.sbr" \
	"$(INTDIR)\messages.sbr" \
	"$(INTDIR)\scriplib.sbr" \
	"$(INTDIR)\threads.sbr" \
	"$(INTDIR)\winding.sbr" \
	"$(INTDIR)\autowad.sbr" \
	"$(INTDIR)\brush.sbr" \
	"$(INTDIR)\brushunion.sbr" \
	"$(INTDIR)\hullfile.sbr" \
	"$(INTDIR)\map.sbr" \
	"$(INTDIR)\qcsg.sbr" \
	"$(INTDIR)\textures.sbr" \
	"$(INTDIR)\wadcfg.sbr" \
	"$(INTDIR)\wadinclude.sbr" \
	"$(INTDIR)\wadpath.sbr"

"$(OUTDIR)\hlcsg.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=binmode.obj /nologo /stack:0x400000,0x100000 /subsystem:console /incremental:no /pdb:"$(OUTDIR)\hlcsg.pdb" /machine:I386 /out:"$(OUTDIR)\hlcsg.exe" 
LINK32_OBJS= \
	"$(INTDIR)\blockmem.obj" \
	"$(INTDIR)\bspfile.obj" \
	"$(INTDIR)\cmdlib.obj" \
	"$(INTDIR)\filelib.obj" \
	"$(INTDIR)\files.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\scriplib.obj" \
	"$(INTDIR)\threads.obj" \
	"$(INTDIR)\winding.obj" \
	"$(INTDIR)\autowad.obj" \
	"$(INTDIR)\brush.obj" \
	"$(INTDIR)\brushunion.obj" \
	"$(INTDIR)\hullfile.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\qcsg.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\wadcfg.obj" \
	"$(INTDIR)\wadinclude.obj" \
	"$(INTDIR)\wadpath.obj"

"$(OUTDIR)\hlcsg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hlcsg - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\hlcsg.exe" "$(OUTDIR)\hlcsg.bsc"


CLEAN :
	-@erase "$(INTDIR)\autowad.obj"
	-@erase "$(INTDIR)\autowad.sbr"
	-@erase "$(INTDIR)\blockmem.obj"
	-@erase "$(INTDIR)\blockmem.sbr"
	-@erase "$(INTDIR)\brush.obj"
	-@erase "$(INTDIR)\brush.sbr"
	-@erase "$(INTDIR)\brushunion.obj"
	-@erase "$(INTDIR)\brushunion.sbr"
	-@erase "$(INTDIR)\bspfile.obj"
	-@erase "$(INTDIR)\bspfile.sbr"
	-@erase "$(INTDIR)\cmdlib.obj"
	-@erase "$(INTDIR)\cmdlib.sbr"
	-@erase "$(INTDIR)\filelib.obj"
	-@erase "$(INTDIR)\filelib.sbr"
	-@erase "$(INTDIR)\files.obj"
	-@erase "$(INTDIR)\files.sbr"
	-@erase "$(INTDIR)\hullfile.obj"
	-@erase "$(INTDIR)\hullfile.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\map.sbr"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\mathlib.sbr"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messages.sbr"
	-@erase "$(INTDIR)\qcsg.obj"
	-@erase "$(INTDIR)\qcsg.sbr"
	-@erase "$(INTDIR)\scriplib.obj"
	-@erase "$(INTDIR)\scriplib.sbr"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\textures.sbr"
	-@erase "$(INTDIR)\threads.obj"
	-@erase "$(INTDIR)\threads.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wadcfg.obj"
	-@erase "$(INTDIR)\wadcfg.sbr"
	-@erase "$(INTDIR)\wadinclude.obj"
	-@erase "$(INTDIR)\wadinclude.sbr"
	-@erase "$(INTDIR)\wadpath.obj"
	-@erase "$(INTDIR)\wadpath.sbr"
	-@erase "$(INTDIR)\winding.obj"
	-@erase "$(INTDIR)\winding.sbr"
	-@erase "$(OUTDIR)\hlcsg.bsc"
	-@erase "$(OUTDIR)\hlcsg.exe"
	-@erase "$(OUTDIR)\hlcsg.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /Od /I "..\template" /I "..\common" /D "_DEBUG" /D "_MBCS" /D "DOUBLEVEC_T" /D "STDC_HEADERS" /D "WIN32" /D "_CONSOLE" /D "SYSTEM_WIN32" /D "HLCSG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hlcsg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hlcsg.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\blockmem.sbr" \
	"$(INTDIR)\bspfile.sbr" \
	"$(INTDIR)\cmdlib.sbr" \
	"$(INTDIR)\filelib.sbr" \
	"$(INTDIR)\files.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\mathlib.sbr" \
	"$(INTDIR)\messages.sbr" \
	"$(INTDIR)\scriplib.sbr" \
	"$(INTDIR)\threads.sbr" \
	"$(INTDIR)\winding.sbr" \
	"$(INTDIR)\autowad.sbr" \
	"$(INTDIR)\brush.sbr" \
	"$(INTDIR)\brushunion.sbr" \
	"$(INTDIR)\hullfile.sbr" \
	"$(INTDIR)\map.sbr" \
	"$(INTDIR)\qcsg.sbr" \
	"$(INTDIR)\textures.sbr" \
	"$(INTDIR)\wadcfg.sbr" \
	"$(INTDIR)\wadinclude.sbr" \
	"$(INTDIR)\wadpath.sbr"

"$(OUTDIR)\hlcsg.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=binmode.obj /nologo /stack:0x1000000,0x100000 /subsystem:console /profile /map:"$(INTDIR)\hlcsg.map" /debug /machine:I386 /out:"$(OUTDIR)\hlcsg.exe" 
LINK32_OBJS= \
	"$(INTDIR)\blockmem.obj" \
	"$(INTDIR)\bspfile.obj" \
	"$(INTDIR)\cmdlib.obj" \
	"$(INTDIR)\filelib.obj" \
	"$(INTDIR)\files.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\scriplib.obj" \
	"$(INTDIR)\threads.obj" \
	"$(INTDIR)\winding.obj" \
	"$(INTDIR)\autowad.obj" \
	"$(INTDIR)\brush.obj" \
	"$(INTDIR)\brushunion.obj" \
	"$(INTDIR)\hullfile.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\qcsg.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\wadcfg.obj" \
	"$(INTDIR)\wadinclude.obj" \
	"$(INTDIR)\wadpath.obj"

"$(OUTDIR)\hlcsg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hlcsg - Win32 Release w Symbols"

OUTDIR=.\Release_w_Symbols
INTDIR=.\Release_w_Symbols
# Begin Custom Macros
OutDir=.\Release_w_Symbols
# End Custom Macros

ALL : "$(OUTDIR)\hlcsg.exe" "$(OUTDIR)\hlcsg.bsc"


CLEAN :
	-@erase "$(INTDIR)\autowad.obj"
	-@erase "$(INTDIR)\autowad.sbr"
	-@erase "$(INTDIR)\blockmem.obj"
	-@erase "$(INTDIR)\blockmem.sbr"
	-@erase "$(INTDIR)\brush.obj"
	-@erase "$(INTDIR)\brush.sbr"
	-@erase "$(INTDIR)\brushunion.obj"
	-@erase "$(INTDIR)\brushunion.sbr"
	-@erase "$(INTDIR)\bspfile.obj"
	-@erase "$(INTDIR)\bspfile.sbr"
	-@erase "$(INTDIR)\cmdlib.obj"
	-@erase "$(INTDIR)\cmdlib.sbr"
	-@erase "$(INTDIR)\filelib.obj"
	-@erase "$(INTDIR)\filelib.sbr"
	-@erase "$(INTDIR)\files.obj"
	-@erase "$(INTDIR)\files.sbr"
	-@erase "$(INTDIR)\hullfile.obj"
	-@erase "$(INTDIR)\hullfile.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\map.sbr"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\mathlib.sbr"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messages.sbr"
	-@erase "$(INTDIR)\qcsg.obj"
	-@erase "$(INTDIR)\qcsg.sbr"
	-@erase "$(INTDIR)\scriplib.obj"
	-@erase "$(INTDIR)\scriplib.sbr"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\textures.sbr"
	-@erase "$(INTDIR)\threads.obj"
	-@erase "$(INTDIR)\threads.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wadcfg.obj"
	-@erase "$(INTDIR)\wadcfg.sbr"
	-@erase "$(INTDIR)\wadinclude.obj"
	-@erase "$(INTDIR)\wadinclude.sbr"
	-@erase "$(INTDIR)\wadpath.obj"
	-@erase "$(INTDIR)\wadpath.sbr"
	-@erase "$(INTDIR)\winding.obj"
	-@erase "$(INTDIR)\winding.sbr"
	-@erase "$(OUTDIR)\hlcsg.bsc"
	-@erase "$(OUTDIR)\hlcsg.exe"
	-@erase "$(OUTDIR)\hlcsg.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /O2 /I "..\common" /I "..\template" /D "NDEBUG" /D "_MBCS" /D "DOUBLEVEC_T" /D "STDC_HEADERS" /D "WIN32" /D "_CONSOLE" /D "SYSTEM_WIN32" /D "HLCSG" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hlcsg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hlcsg.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\blockmem.sbr" \
	"$(INTDIR)\bspfile.sbr" \
	"$(INTDIR)\cmdlib.sbr" \
	"$(INTDIR)\filelib.sbr" \
	"$(INTDIR)\files.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\mathlib.sbr" \
	"$(INTDIR)\messages.sbr" \
	"$(INTDIR)\scriplib.sbr" \
	"$(INTDIR)\threads.sbr" \
	"$(INTDIR)\winding.sbr" \
	"$(INTDIR)\autowad.sbr" \
	"$(INTDIR)\brush.sbr" \
	"$(INTDIR)\brushunion.sbr" \
	"$(INTDIR)\hullfile.sbr" \
	"$(INTDIR)\map.sbr" \
	"$(INTDIR)\qcsg.sbr" \
	"$(INTDIR)\textures.sbr" \
	"$(INTDIR)\wadcfg.sbr" \
	"$(INTDIR)\wadinclude.sbr" \
	"$(INTDIR)\wadpath.sbr"

"$(OUTDIR)\hlcsg.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=binmode.obj /nologo /stack:0x400000,0x100000 /subsystem:console /pdb:none /map:"$(INTDIR)\hlcsg.map" /debug /machine:I386 /out:"$(OUTDIR)\hlcsg.exe" 
LINK32_OBJS= \
	"$(INTDIR)\blockmem.obj" \
	"$(INTDIR)\bspfile.obj" \
	"$(INTDIR)\cmdlib.obj" \
	"$(INTDIR)\filelib.obj" \
	"$(INTDIR)\files.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\scriplib.obj" \
	"$(INTDIR)\threads.obj" \
	"$(INTDIR)\winding.obj" \
	"$(INTDIR)\autowad.obj" \
	"$(INTDIR)\brush.obj" \
	"$(INTDIR)\brushunion.obj" \
	"$(INTDIR)\hullfile.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\qcsg.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\wadcfg.obj" \
	"$(INTDIR)\wadinclude.obj" \
	"$(INTDIR)\wadpath.obj"

"$(OUTDIR)\hlcsg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hlcsg - Win32 Super_Debug"

OUTDIR=.\Super_Debug
INTDIR=.\Super_Debug
# Begin Custom Macros
OutDir=.\Super_Debug
# End Custom Macros

ALL : "$(OUTDIR)\hlcsg.exe" "$(OUTDIR)\hlcsg.bsc"


CLEAN :
	-@erase "$(INTDIR)\autowad.obj"
	-@erase "$(INTDIR)\autowad.sbr"
	-@erase "$(INTDIR)\blockmem.obj"
	-@erase "$(INTDIR)\blockmem.sbr"
	-@erase "$(INTDIR)\brush.obj"
	-@erase "$(INTDIR)\brush.sbr"
	-@erase "$(INTDIR)\brushunion.obj"
	-@erase "$(INTDIR)\brushunion.sbr"
	-@erase "$(INTDIR)\bspfile.obj"
	-@erase "$(INTDIR)\bspfile.sbr"
	-@erase "$(INTDIR)\cmdlib.obj"
	-@erase "$(INTDIR)\cmdlib.sbr"
	-@erase "$(INTDIR)\filelib.obj"
	-@erase "$(INTDIR)\filelib.sbr"
	-@erase "$(INTDIR)\files.obj"
	-@erase "$(INTDIR)\files.sbr"
	-@erase "$(INTDIR)\hullfile.obj"
	-@erase "$(INTDIR)\hullfile.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\map.obj"
	-@erase "$(INTDIR)\map.sbr"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\mathlib.sbr"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messages.sbr"
	-@erase "$(INTDIR)\qcsg.obj"
	-@erase "$(INTDIR)\qcsg.sbr"
	-@erase "$(INTDIR)\scriplib.obj"
	-@erase "$(INTDIR)\scriplib.sbr"
	-@erase "$(INTDIR)\textures.obj"
	-@erase "$(INTDIR)\textures.sbr"
	-@erase "$(INTDIR)\threads.obj"
	-@erase "$(INTDIR)\threads.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\wadcfg.obj"
	-@erase "$(INTDIR)\wadcfg.sbr"
	-@erase "$(INTDIR)\wadinclude.obj"
	-@erase "$(INTDIR)\wadinclude.sbr"
	-@erase "$(INTDIR)\wadpath.obj"
	-@erase "$(INTDIR)\wadpath.sbr"
	-@erase "$(INTDIR)\winding.obj"
	-@erase "$(INTDIR)\winding.sbr"
	-@erase "$(OUTDIR)\hlcsg.bsc"
	-@erase "$(OUTDIR)\hlcsg.exe"
	-@erase "$(OUTDIR)\hlcsg.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\common" /I "..\template" /D "_DEBUG" /D "HLCSG" /D "_MBCS" /D "DOUBLEVEC_T" /D "STDC_HEADERS" /D "WIN32" /D "_CONSOLE" /D "SYSTEM_WIN32" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hlcsg.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hlcsg.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\blockmem.sbr" \
	"$(INTDIR)\bspfile.sbr" \
	"$(INTDIR)\cmdlib.sbr" \
	"$(INTDIR)\filelib.sbr" \
	"$(INTDIR)\files.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\mathlib.sbr" \
	"$(INTDIR)\messages.sbr" \
	"$(INTDIR)\scriplib.sbr" \
	"$(INTDIR)\threads.sbr" \
	"$(INTDIR)\winding.sbr" \
	"$(INTDIR)\autowad.sbr" \
	"$(INTDIR)\brush.sbr" \
	"$(INTDIR)\brushunion.sbr" \
	"$(INTDIR)\hullfile.sbr" \
	"$(INTDIR)\map.sbr" \
	"$(INTDIR)\qcsg.sbr" \
	"$(INTDIR)\textures.sbr" \
	"$(INTDIR)\wadcfg.sbr" \
	"$(INTDIR)\wadinclude.sbr" \
	"$(INTDIR)\wadpath.sbr"

"$(OUTDIR)\hlcsg.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=binmode.obj /nologo /stack:0x1000000,0x100000 /subsystem:console /profile /map:"$(INTDIR)\hlcsg.map" /debug /machine:I386 /out:"$(OUTDIR)\hlcsg.exe" 
LINK32_OBJS= \
	"$(INTDIR)\blockmem.obj" \
	"$(INTDIR)\bspfile.obj" \
	"$(INTDIR)\cmdlib.obj" \
	"$(INTDIR)\filelib.obj" \
	"$(INTDIR)\files.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\scriplib.obj" \
	"$(INTDIR)\threads.obj" \
	"$(INTDIR)\winding.obj" \
	"$(INTDIR)\autowad.obj" \
	"$(INTDIR)\brush.obj" \
	"$(INTDIR)\brushunion.obj" \
	"$(INTDIR)\hullfile.obj" \
	"$(INTDIR)\map.obj" \
	"$(INTDIR)\qcsg.obj" \
	"$(INTDIR)\textures.obj" \
	"$(INTDIR)\wadcfg.obj" \
	"$(INTDIR)\wadinclude.obj" \
	"$(INTDIR)\wadpath.obj"

"$(OUTDIR)\hlcsg.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(INTDIR)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(NO_EXTERNAL_DEPS)" != "1"
!IF EXISTS("hlcsg.dep")
!INCLUDE "hlcsg.dep"
!ELSE 
!MESSAGE Warning: cannot find "hlcsg.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hlcsg - Win32 Release" || "$(CFG)" == "hlcsg - Win32 Debug" || "$(CFG)" == "hlcsg - Win32 Release w Symbols" || "$(CFG)" == "hlcsg - Win32 Super_Debug"
SOURCE=..\common\blockmem.cpp

"$(INTDIR)\blockmem.obj"	"$(INTDIR)\blockmem.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\bspfile.cpp

"$(INTDIR)\bspfile.obj"	"$(INTDIR)\bspfile.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\cmdlib.cpp

"$(INTDIR)\cmdlib.obj"	"$(INTDIR)\cmdlib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\filelib.cpp

"$(INTDIR)\filelib.obj"	"$(INTDIR)\filelib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\files.cpp

"$(INTDIR)\files.obj"	"$(INTDIR)\files.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\log.cpp

"$(INTDIR)\log.obj"	"$(INTDIR)\log.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\mathlib.cpp

"$(INTDIR)\mathlib.obj"	"$(INTDIR)\mathlib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\messages.cpp

"$(INTDIR)\messages.obj"	"$(INTDIR)\messages.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\scriplib.cpp

"$(INTDIR)\scriplib.obj"	"$(INTDIR)\scriplib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\threads.cpp

"$(INTDIR)\threads.obj"	"$(INTDIR)\threads.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\winding.cpp

"$(INTDIR)\winding.obj"	"$(INTDIR)\winding.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=.\autowad.cpp

"$(INTDIR)\autowad.obj"	"$(INTDIR)\autowad.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\brush.cpp

"$(INTDIR)\brush.obj"	"$(INTDIR)\brush.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\brushunion.cpp

"$(INTDIR)\brushunion.obj"	"$(INTDIR)\brushunion.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\hullfile.cpp

"$(INTDIR)\hullfile.obj"	"$(INTDIR)\hullfile.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\map.cpp

"$(INTDIR)\map.obj"	"$(INTDIR)\map.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\qcsg.cpp

"$(INTDIR)\qcsg.obj"	"$(INTDIR)\qcsg.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\textures.cpp

"$(INTDIR)\textures.obj"	"$(INTDIR)\textures.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wadcfg.cpp

"$(INTDIR)\wadcfg.obj"	"$(INTDIR)\wadcfg.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wadinclude.cpp

"$(INTDIR)\wadinclude.obj"	"$(INTDIR)\wadinclude.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\wadpath.cpp

"$(INTDIR)\wadpath.obj"	"$(INTDIR)\wadpath.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

