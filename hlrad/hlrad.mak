# Microsoft Developer Studio Generated NMAKE File, Based on hlrad.dsp
!IF "$(CFG)" == ""
CFG=hlrad - Win32 Super_Debug
!MESSAGE No configuration specified. Defaulting to hlrad - Win32 Super_Debug.
!ENDIF 

!IF "$(CFG)" != "hlrad - Win32 Release" && "$(CFG)" != "hlrad - Win32 Debug" && "$(CFG)" != "hlrad - Win32 Release w Symbols" && "$(CFG)" != "hlrad - Win32 Super_Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "hlrad.mak" CFG="hlrad - Win32 Super_Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "hlrad - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "hlrad - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE "hlrad - Win32 Release w Symbols" (based on "Win32 (x86) Console Application")
!MESSAGE "hlrad - Win32 Super_Debug" (based on "Win32 (x86) Console Application")
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

!IF  "$(CFG)" == "hlrad - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\Release
# End Custom Macros

ALL : "$(OUTDIR)\hlrad.exe" "$(OUTDIR)\hlrad.bsc"


CLEAN :
	-@erase "$(INTDIR)\blockmem.obj"
	-@erase "$(INTDIR)\blockmem.sbr"
	-@erase "$(INTDIR)\bspfile.obj"
	-@erase "$(INTDIR)\bspfile.sbr"
	-@erase "$(INTDIR)\cmdlib.obj"
	-@erase "$(INTDIR)\cmdlib.sbr"
	-@erase "$(INTDIR)\filelib.obj"
	-@erase "$(INTDIR)\filelib.sbr"
	-@erase "$(INTDIR)\lerp.obj"
	-@erase "$(INTDIR)\lerp.sbr"
	-@erase "$(INTDIR)\lightmap.obj"
	-@erase "$(INTDIR)\lightmap.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\mathlib.sbr"
	-@erase "$(INTDIR)\mathutil.obj"
	-@erase "$(INTDIR)\mathutil.sbr"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messages.sbr"
	-@erase "$(INTDIR)\nomatrix.obj"
	-@erase "$(INTDIR)\nomatrix.sbr"
	-@erase "$(INTDIR)\qrad.obj"
	-@erase "$(INTDIR)\qrad.sbr"
	-@erase "$(INTDIR)\qradutil.obj"
	-@erase "$(INTDIR)\qradutil.sbr"
	-@erase "$(INTDIR)\resourcelock.obj"
	-@erase "$(INTDIR)\resourcelock.sbr"
	-@erase "$(INTDIR)\scriplib.obj"
	-@erase "$(INTDIR)\scriplib.sbr"
	-@erase "$(INTDIR)\sparse.obj"
	-@erase "$(INTDIR)\sparse.sbr"
	-@erase "$(INTDIR)\threads.obj"
	-@erase "$(INTDIR)\threads.sbr"
	-@erase "$(INTDIR)\trace.obj"
	-@erase "$(INTDIR)\trace.sbr"
	-@erase "$(INTDIR)\transfers.obj"
	-@erase "$(INTDIR)\transfers.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vismatrix.obj"
	-@erase "$(INTDIR)\vismatrix.sbr"
	-@erase "$(INTDIR)\vismatrixutil.obj"
	-@erase "$(INTDIR)\vismatrixutil.sbr"
	-@erase "$(INTDIR)\winding.obj"
	-@erase "$(INTDIR)\winding.sbr"
	-@erase "$(OUTDIR)\hlrad.bsc"
	-@erase "$(OUTDIR)\hlrad.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /O2 /I "..\common" /I "..\template" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "SYSTEM_WIN32" /D "STDC_HEADERS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hlrad.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hlrad.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\blockmem.sbr" \
	"$(INTDIR)\bspfile.sbr" \
	"$(INTDIR)\cmdlib.sbr" \
	"$(INTDIR)\filelib.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\mathlib.sbr" \
	"$(INTDIR)\messages.sbr" \
	"$(INTDIR)\resourcelock.sbr" \
	"$(INTDIR)\scriplib.sbr" \
	"$(INTDIR)\threads.sbr" \
	"$(INTDIR)\winding.sbr" \
	"$(INTDIR)\lerp.sbr" \
	"$(INTDIR)\lightmap.sbr" \
	"$(INTDIR)\mathutil.sbr" \
	"$(INTDIR)\nomatrix.sbr" \
	"$(INTDIR)\qrad.sbr" \
	"$(INTDIR)\qradutil.sbr" \
	"$(INTDIR)\sparse.sbr" \
	"$(INTDIR)\trace.sbr" \
	"$(INTDIR)\transfers.sbr" \
	"$(INTDIR)\vismatrix.sbr" \
	"$(INTDIR)\vismatrixutil.sbr"

"$(OUTDIR)\hlrad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=binmode.obj /nologo /stack:0x400000,0x100000 /subsystem:console /incremental:no /pdb:"$(OUTDIR)\hlrad.pdb" /machine:I386 /out:"$(OUTDIR)\hlrad.exe" 
LINK32_OBJS= \
	"$(INTDIR)\blockmem.obj" \
	"$(INTDIR)\bspfile.obj" \
	"$(INTDIR)\cmdlib.obj" \
	"$(INTDIR)\filelib.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\resourcelock.obj" \
	"$(INTDIR)\scriplib.obj" \
	"$(INTDIR)\threads.obj" \
	"$(INTDIR)\winding.obj" \
	"$(INTDIR)\lerp.obj" \
	"$(INTDIR)\lightmap.obj" \
	"$(INTDIR)\mathutil.obj" \
	"$(INTDIR)\nomatrix.obj" \
	"$(INTDIR)\qrad.obj" \
	"$(INTDIR)\qradutil.obj" \
	"$(INTDIR)\sparse.obj" \
	"$(INTDIR)\trace.obj" \
	"$(INTDIR)\transfers.obj" \
	"$(INTDIR)\vismatrix.obj" \
	"$(INTDIR)\vismatrixutil.obj"

"$(OUTDIR)\hlrad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hlrad - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\Debug
# End Custom Macros

ALL : "$(OUTDIR)\hlrad.exe" "$(OUTDIR)\hlrad.bsc"


CLEAN :
	-@erase "$(INTDIR)\blockmem.obj"
	-@erase "$(INTDIR)\blockmem.sbr"
	-@erase "$(INTDIR)\bspfile.obj"
	-@erase "$(INTDIR)\bspfile.sbr"
	-@erase "$(INTDIR)\cmdlib.obj"
	-@erase "$(INTDIR)\cmdlib.sbr"
	-@erase "$(INTDIR)\filelib.obj"
	-@erase "$(INTDIR)\filelib.sbr"
	-@erase "$(INTDIR)\lerp.obj"
	-@erase "$(INTDIR)\lerp.sbr"
	-@erase "$(INTDIR)\lightmap.obj"
	-@erase "$(INTDIR)\lightmap.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\mathlib.sbr"
	-@erase "$(INTDIR)\mathutil.obj"
	-@erase "$(INTDIR)\mathutil.sbr"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messages.sbr"
	-@erase "$(INTDIR)\nomatrix.obj"
	-@erase "$(INTDIR)\nomatrix.sbr"
	-@erase "$(INTDIR)\qrad.obj"
	-@erase "$(INTDIR)\qrad.sbr"
	-@erase "$(INTDIR)\qradutil.obj"
	-@erase "$(INTDIR)\qradutil.sbr"
	-@erase "$(INTDIR)\resourcelock.obj"
	-@erase "$(INTDIR)\resourcelock.sbr"
	-@erase "$(INTDIR)\scriplib.obj"
	-@erase "$(INTDIR)\scriplib.sbr"
	-@erase "$(INTDIR)\sparse.obj"
	-@erase "$(INTDIR)\sparse.sbr"
	-@erase "$(INTDIR)\threads.obj"
	-@erase "$(INTDIR)\threads.sbr"
	-@erase "$(INTDIR)\trace.obj"
	-@erase "$(INTDIR)\trace.sbr"
	-@erase "$(INTDIR)\transfers.obj"
	-@erase "$(INTDIR)\transfers.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vismatrix.obj"
	-@erase "$(INTDIR)\vismatrix.sbr"
	-@erase "$(INTDIR)\vismatrixutil.obj"
	-@erase "$(INTDIR)\vismatrixutil.sbr"
	-@erase "$(INTDIR)\winding.obj"
	-@erase "$(INTDIR)\winding.sbr"
	-@erase "$(OUTDIR)\hlrad.bsc"
	-@erase "$(OUTDIR)\hlrad.exe"
	-@erase "$(OUTDIR)\hlrad.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /Gm /GX /Zi /Od /I "..\common" /I "..\template" /D "_DEBUG" /D "STDC_HEADERS" /D "WIN32" /D "_CONSOLE" /D "SYSTEM_WIN32" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hlrad.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hlrad.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\blockmem.sbr" \
	"$(INTDIR)\bspfile.sbr" \
	"$(INTDIR)\cmdlib.sbr" \
	"$(INTDIR)\filelib.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\mathlib.sbr" \
	"$(INTDIR)\messages.sbr" \
	"$(INTDIR)\resourcelock.sbr" \
	"$(INTDIR)\scriplib.sbr" \
	"$(INTDIR)\threads.sbr" \
	"$(INTDIR)\winding.sbr" \
	"$(INTDIR)\lerp.sbr" \
	"$(INTDIR)\lightmap.sbr" \
	"$(INTDIR)\mathutil.sbr" \
	"$(INTDIR)\nomatrix.sbr" \
	"$(INTDIR)\qrad.sbr" \
	"$(INTDIR)\qradutil.sbr" \
	"$(INTDIR)\sparse.sbr" \
	"$(INTDIR)\trace.sbr" \
	"$(INTDIR)\transfers.sbr" \
	"$(INTDIR)\vismatrix.sbr" \
	"$(INTDIR)\vismatrixutil.sbr"

"$(OUTDIR)\hlrad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=binmode.obj /nologo /stack:0x1000000,0x100000 /subsystem:console /profile /map:"$(INTDIR)\hlrad.map" /debug /machine:I386 /out:"$(OUTDIR)\hlrad.exe" 
LINK32_OBJS= \
	"$(INTDIR)\blockmem.obj" \
	"$(INTDIR)\bspfile.obj" \
	"$(INTDIR)\cmdlib.obj" \
	"$(INTDIR)\filelib.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\resourcelock.obj" \
	"$(INTDIR)\scriplib.obj" \
	"$(INTDIR)\threads.obj" \
	"$(INTDIR)\winding.obj" \
	"$(INTDIR)\lerp.obj" \
	"$(INTDIR)\lightmap.obj" \
	"$(INTDIR)\mathutil.obj" \
	"$(INTDIR)\nomatrix.obj" \
	"$(INTDIR)\qrad.obj" \
	"$(INTDIR)\qradutil.obj" \
	"$(INTDIR)\sparse.obj" \
	"$(INTDIR)\trace.obj" \
	"$(INTDIR)\transfers.obj" \
	"$(INTDIR)\vismatrix.obj" \
	"$(INTDIR)\vismatrixutil.obj"

"$(OUTDIR)\hlrad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hlrad - Win32 Release w Symbols"

OUTDIR=.\Release_w_Symbols
INTDIR=.\Release_w_Symbols
# Begin Custom Macros
OutDir=.\Release_w_Symbols
# End Custom Macros

ALL : "$(OUTDIR)\hlrad.exe" "$(OUTDIR)\hlrad.bsc"


CLEAN :
	-@erase "$(INTDIR)\blockmem.obj"
	-@erase "$(INTDIR)\blockmem.sbr"
	-@erase "$(INTDIR)\bspfile.obj"
	-@erase "$(INTDIR)\bspfile.sbr"
	-@erase "$(INTDIR)\cmdlib.obj"
	-@erase "$(INTDIR)\cmdlib.sbr"
	-@erase "$(INTDIR)\filelib.obj"
	-@erase "$(INTDIR)\filelib.sbr"
	-@erase "$(INTDIR)\lerp.obj"
	-@erase "$(INTDIR)\lerp.sbr"
	-@erase "$(INTDIR)\lightmap.obj"
	-@erase "$(INTDIR)\lightmap.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\mathlib.sbr"
	-@erase "$(INTDIR)\mathutil.obj"
	-@erase "$(INTDIR)\mathutil.sbr"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messages.sbr"
	-@erase "$(INTDIR)\nomatrix.obj"
	-@erase "$(INTDIR)\nomatrix.sbr"
	-@erase "$(INTDIR)\qrad.obj"
	-@erase "$(INTDIR)\qrad.sbr"
	-@erase "$(INTDIR)\qradutil.obj"
	-@erase "$(INTDIR)\qradutil.sbr"
	-@erase "$(INTDIR)\resourcelock.obj"
	-@erase "$(INTDIR)\resourcelock.sbr"
	-@erase "$(INTDIR)\scriplib.obj"
	-@erase "$(INTDIR)\scriplib.sbr"
	-@erase "$(INTDIR)\sparse.obj"
	-@erase "$(INTDIR)\sparse.sbr"
	-@erase "$(INTDIR)\threads.obj"
	-@erase "$(INTDIR)\threads.sbr"
	-@erase "$(INTDIR)\trace.obj"
	-@erase "$(INTDIR)\trace.sbr"
	-@erase "$(INTDIR)\transfers.obj"
	-@erase "$(INTDIR)\transfers.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vismatrix.obj"
	-@erase "$(INTDIR)\vismatrix.sbr"
	-@erase "$(INTDIR)\vismatrixutil.obj"
	-@erase "$(INTDIR)\vismatrixutil.sbr"
	-@erase "$(INTDIR)\winding.obj"
	-@erase "$(INTDIR)\winding.sbr"
	-@erase "$(OUTDIR)\hlrad.bsc"
	-@erase "$(OUTDIR)\hlrad.exe"
	-@erase "$(OUTDIR)\hlrad.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MD /W3 /GX /Zi /O2 /I "..\common" /I "..\template" /D "NDEBUG" /D "WIN32" /D "_CONSOLE" /D "SYSTEM_WIN32" /D "STDC_HEADERS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hlrad.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hlrad.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\blockmem.sbr" \
	"$(INTDIR)\bspfile.sbr" \
	"$(INTDIR)\cmdlib.sbr" \
	"$(INTDIR)\filelib.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\mathlib.sbr" \
	"$(INTDIR)\messages.sbr" \
	"$(INTDIR)\resourcelock.sbr" \
	"$(INTDIR)\scriplib.sbr" \
	"$(INTDIR)\threads.sbr" \
	"$(INTDIR)\winding.sbr" \
	"$(INTDIR)\lerp.sbr" \
	"$(INTDIR)\lightmap.sbr" \
	"$(INTDIR)\mathutil.sbr" \
	"$(INTDIR)\nomatrix.sbr" \
	"$(INTDIR)\qrad.sbr" \
	"$(INTDIR)\qradutil.sbr" \
	"$(INTDIR)\sparse.sbr" \
	"$(INTDIR)\trace.sbr" \
	"$(INTDIR)\transfers.sbr" \
	"$(INTDIR)\vismatrix.sbr" \
	"$(INTDIR)\vismatrixutil.sbr"

"$(OUTDIR)\hlrad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=binmode.obj /nologo /stack:0x400000,0x100000 /subsystem:console /pdb:none /map:"$(INTDIR)\hlrad.map" /debug /machine:I386 /out:"$(OUTDIR)\hlrad.exe" 
LINK32_OBJS= \
	"$(INTDIR)\blockmem.obj" \
	"$(INTDIR)\bspfile.obj" \
	"$(INTDIR)\cmdlib.obj" \
	"$(INTDIR)\filelib.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\resourcelock.obj" \
	"$(INTDIR)\scriplib.obj" \
	"$(INTDIR)\threads.obj" \
	"$(INTDIR)\winding.obj" \
	"$(INTDIR)\lerp.obj" \
	"$(INTDIR)\lightmap.obj" \
	"$(INTDIR)\mathutil.obj" \
	"$(INTDIR)\nomatrix.obj" \
	"$(INTDIR)\qrad.obj" \
	"$(INTDIR)\qradutil.obj" \
	"$(INTDIR)\sparse.obj" \
	"$(INTDIR)\trace.obj" \
	"$(INTDIR)\transfers.obj" \
	"$(INTDIR)\vismatrix.obj" \
	"$(INTDIR)\vismatrixutil.obj"

"$(OUTDIR)\hlrad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "hlrad - Win32 Super_Debug"

OUTDIR=.\Super_Debug
INTDIR=.\Super_Debug
# Begin Custom Macros
OutDir=.\Super_Debug
# End Custom Macros

ALL : "$(OUTDIR)\hlrad.exe" "$(OUTDIR)\hlrad.bsc"


CLEAN :
	-@erase "$(INTDIR)\blockmem.obj"
	-@erase "$(INTDIR)\blockmem.sbr"
	-@erase "$(INTDIR)\bspfile.obj"
	-@erase "$(INTDIR)\bspfile.sbr"
	-@erase "$(INTDIR)\cmdlib.obj"
	-@erase "$(INTDIR)\cmdlib.sbr"
	-@erase "$(INTDIR)\filelib.obj"
	-@erase "$(INTDIR)\filelib.sbr"
	-@erase "$(INTDIR)\lerp.obj"
	-@erase "$(INTDIR)\lerp.sbr"
	-@erase "$(INTDIR)\lightmap.obj"
	-@erase "$(INTDIR)\lightmap.sbr"
	-@erase "$(INTDIR)\log.obj"
	-@erase "$(INTDIR)\log.sbr"
	-@erase "$(INTDIR)\mathlib.obj"
	-@erase "$(INTDIR)\mathlib.sbr"
	-@erase "$(INTDIR)\mathutil.obj"
	-@erase "$(INTDIR)\mathutil.sbr"
	-@erase "$(INTDIR)\messages.obj"
	-@erase "$(INTDIR)\messages.sbr"
	-@erase "$(INTDIR)\nomatrix.obj"
	-@erase "$(INTDIR)\nomatrix.sbr"
	-@erase "$(INTDIR)\qrad.obj"
	-@erase "$(INTDIR)\qrad.sbr"
	-@erase "$(INTDIR)\qradutil.obj"
	-@erase "$(INTDIR)\qradutil.sbr"
	-@erase "$(INTDIR)\resourcelock.obj"
	-@erase "$(INTDIR)\resourcelock.sbr"
	-@erase "$(INTDIR)\scriplib.obj"
	-@erase "$(INTDIR)\scriplib.sbr"
	-@erase "$(INTDIR)\sparse.obj"
	-@erase "$(INTDIR)\sparse.sbr"
	-@erase "$(INTDIR)\threads.obj"
	-@erase "$(INTDIR)\threads.sbr"
	-@erase "$(INTDIR)\trace.obj"
	-@erase "$(INTDIR)\trace.sbr"
	-@erase "$(INTDIR)\transfers.obj"
	-@erase "$(INTDIR)\transfers.sbr"
	-@erase "$(INTDIR)\vc60.idb"
	-@erase "$(INTDIR)\vc60.pdb"
	-@erase "$(INTDIR)\vismatrix.obj"
	-@erase "$(INTDIR)\vismatrix.sbr"
	-@erase "$(INTDIR)\vismatrixutil.obj"
	-@erase "$(INTDIR)\vismatrixutil.sbr"
	-@erase "$(INTDIR)\winding.obj"
	-@erase "$(INTDIR)\winding.sbr"
	-@erase "$(OUTDIR)\hlrad.bsc"
	-@erase "$(OUTDIR)\hlrad.exe"
	-@erase "$(OUTDIR)\hlrad.map"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /I "..\common" /I "..\template" /D "_DEBUG" /D "STDC_HEADERS" /D "WIN32" /D "_CONSOLE" /D "SYSTEM_WIN32" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\hlrad.pch" /YX /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\hlrad.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\blockmem.sbr" \
	"$(INTDIR)\bspfile.sbr" \
	"$(INTDIR)\cmdlib.sbr" \
	"$(INTDIR)\filelib.sbr" \
	"$(INTDIR)\log.sbr" \
	"$(INTDIR)\mathlib.sbr" \
	"$(INTDIR)\messages.sbr" \
	"$(INTDIR)\resourcelock.sbr" \
	"$(INTDIR)\scriplib.sbr" \
	"$(INTDIR)\threads.sbr" \
	"$(INTDIR)\winding.sbr" \
	"$(INTDIR)\lerp.sbr" \
	"$(INTDIR)\lightmap.sbr" \
	"$(INTDIR)\mathutil.sbr" \
	"$(INTDIR)\nomatrix.sbr" \
	"$(INTDIR)\qrad.sbr" \
	"$(INTDIR)\qradutil.sbr" \
	"$(INTDIR)\sparse.sbr" \
	"$(INTDIR)\trace.sbr" \
	"$(INTDIR)\transfers.sbr" \
	"$(INTDIR)\vismatrix.sbr" \
	"$(INTDIR)\vismatrixutil.sbr"

"$(OUTDIR)\hlrad.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=binmode.obj /nologo /stack:0x1000000,0x100000 /subsystem:console /profile /map:"$(INTDIR)\hlrad.map" /debug /machine:I386 /out:"$(OUTDIR)\hlrad.exe" 
LINK32_OBJS= \
	"$(INTDIR)\blockmem.obj" \
	"$(INTDIR)\bspfile.obj" \
	"$(INTDIR)\cmdlib.obj" \
	"$(INTDIR)\filelib.obj" \
	"$(INTDIR)\log.obj" \
	"$(INTDIR)\mathlib.obj" \
	"$(INTDIR)\messages.obj" \
	"$(INTDIR)\resourcelock.obj" \
	"$(INTDIR)\scriplib.obj" \
	"$(INTDIR)\threads.obj" \
	"$(INTDIR)\winding.obj" \
	"$(INTDIR)\lerp.obj" \
	"$(INTDIR)\lightmap.obj" \
	"$(INTDIR)\mathutil.obj" \
	"$(INTDIR)\nomatrix.obj" \
	"$(INTDIR)\qrad.obj" \
	"$(INTDIR)\qradutil.obj" \
	"$(INTDIR)\sparse.obj" \
	"$(INTDIR)\trace.obj" \
	"$(INTDIR)\transfers.obj" \
	"$(INTDIR)\vismatrix.obj" \
	"$(INTDIR)\vismatrixutil.obj"

"$(OUTDIR)\hlrad.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
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
!IF EXISTS("hlrad.dep")
!INCLUDE "hlrad.dep"
!ELSE 
!MESSAGE Warning: cannot find "hlrad.dep"
!ENDIF 
!ENDIF 


!IF "$(CFG)" == "hlrad - Win32 Release" || "$(CFG)" == "hlrad - Win32 Debug" || "$(CFG)" == "hlrad - Win32 Release w Symbols" || "$(CFG)" == "hlrad - Win32 Super_Debug"
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


SOURCE=..\common\log.cpp

"$(INTDIR)\log.obj"	"$(INTDIR)\log.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\mathlib.cpp

"$(INTDIR)\mathlib.obj"	"$(INTDIR)\mathlib.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\messages.cpp

"$(INTDIR)\messages.obj"	"$(INTDIR)\messages.sbr" : $(SOURCE) "$(INTDIR)"
	$(CPP) $(CPP_PROJ) $(SOURCE)


SOURCE=..\common\resourcelock.cpp

"$(INTDIR)\resourcelock.obj"	"$(INTDIR)\resourcelock.sbr" : $(SOURCE) "$(INTDIR)"
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


SOURCE=.\lerp.cpp

"$(INTDIR)\lerp.obj"	"$(INTDIR)\lerp.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\lightmap.cpp

"$(INTDIR)\lightmap.obj"	"$(INTDIR)\lightmap.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\mathutil.cpp

"$(INTDIR)\mathutil.obj"	"$(INTDIR)\mathutil.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\nomatrix.cpp

"$(INTDIR)\nomatrix.obj"	"$(INTDIR)\nomatrix.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\qrad.cpp

"$(INTDIR)\qrad.obj"	"$(INTDIR)\qrad.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\qradutil.cpp

"$(INTDIR)\qradutil.obj"	"$(INTDIR)\qradutil.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\sparse.cpp

"$(INTDIR)\sparse.obj"	"$(INTDIR)\sparse.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\trace.cpp

"$(INTDIR)\trace.obj"	"$(INTDIR)\trace.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\transfers.cpp

"$(INTDIR)\transfers.obj"	"$(INTDIR)\transfers.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vismatrix.cpp

"$(INTDIR)\vismatrix.obj"	"$(INTDIR)\vismatrix.sbr" : $(SOURCE) "$(INTDIR)"


SOURCE=.\vismatrixutil.cpp

"$(INTDIR)\vismatrixutil.obj"	"$(INTDIR)\vismatrixutil.sbr" : $(SOURCE) "$(INTDIR)"



!ENDIF 

