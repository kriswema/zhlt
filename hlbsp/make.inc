HLBSP_DEFINES+=-D DOUBLEVEC_T
HLBSP:=$(OUTDIR)/hlbsp$(EXEEXT)
HLBSP_SRCDIR:=hlbsp
HLBSP_OUTDIR:=$(HLBSP_SRCDIR)/$(OUTDIR)


HLBSP_INPUT := \
$(HLBSP_SRCDIR)/merge.cpp \
$(HLBSP_SRCDIR)/outside.cpp \
$(HLBSP_SRCDIR)/portals.cpp \
$(HLBSP_SRCDIR)/qbsp.cpp \
$(HLBSP_SRCDIR)/solidbsp.cpp \
$(HLBSP_SRCDIR)/surfaces.cpp \
$(HLBSP_SRCDIR)/tjunc.cpp \
$(HLBSP_SRCDIR)/writebsp.cpp \
$(COMMON_SRCDIR)/blockmem.cpp \
$(COMMON_SRCDIR)/bspfile.cpp \
$(COMMON_SRCDIR)/cmdlib.cpp \
$(COMMON_SRCDIR)/filelib.cpp \
$(COMMON_SRCDIR)/log.cpp \
$(COMMON_SRCDIR)/mathlib.cpp \
$(COMMON_SRCDIR)/messages.cpp \
$(COMMON_SRCDIR)/scriplib.cpp \
$(COMMON_SRCDIR)/threads.cpp \
$(COMMON_SRCDIR)/winding.cpp \


HLBSP_OUTPUT := \
$(HLBSP_OUTDIR)/merge$(OBJEXT) \
$(HLBSP_OUTDIR)/outside$(OBJEXT) \
$(HLBSP_OUTDIR)/portals$(OBJEXT) \
$(HLBSP_OUTDIR)/qbsp$(OBJEXT) \
$(HLBSP_OUTDIR)/solidbsp$(OBJEXT) \
$(HLBSP_OUTDIR)/surfaces$(OBJEXT) \
$(HLBSP_OUTDIR)/tjunc$(OBJEXT) \
$(HLBSP_OUTDIR)/writebsp$(OBJEXT) \
$(HLBSP_OUTDIR)/blockmem$(OBJEXT) \
$(HLBSP_OUTDIR)/bspfile$(OBJEXT) \
$(HLBSP_OUTDIR)/cmdlib$(OBJEXT) \
$(HLBSP_OUTDIR)/filelib$(OBJEXT) \
$(HLBSP_OUTDIR)/log$(OBJEXT) \
$(HLBSP_OUTDIR)/mathlib$(OBJEXT) \
$(HLBSP_OUTDIR)/messages$(OBJEXT) \
$(HLBSP_OUTDIR)/scriplib$(OBJEXT) \
$(HLBSP_OUTDIR)/threads$(OBJEXT) \
$(HLBSP_OUTDIR)/winding$(OBJEXT) \


