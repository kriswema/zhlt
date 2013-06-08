HLVIS_DEFINES+=
HLVIS:=$(OUTDIR)/hlvis$(EXEEXT)
HLVIS_SRCDIR:=hlvis
HLVIS_OUTDIR:=$(HLVIS_SRCDIR)/$(OUTDIR)


HLVIS_INPUT := \
$(HLVIS_SRCDIR)/flow.cpp \
$(HLVIS_SRCDIR)/vis.cpp \
$(HLVIS_SRCDIR)/zones.cpp \
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


HLVIS_OUTPUT := \
$(HLVIS_OUTDIR)/flow$(OBJEXT) \
$(HLVIS_OUTDIR)/vis$(OBJEXT) \
$(HLVIS_OUTDIR)/zones$(OBJEXT) \
$(HLVIS_OUTDIR)/blockmem$(OBJEXT) \
$(HLVIS_OUTDIR)/bspfile$(OBJEXT) \
$(HLVIS_OUTDIR)/cmdlib$(OBJEXT) \
$(HLVIS_OUTDIR)/filelib$(OBJEXT) \
$(HLVIS_OUTDIR)/log$(OBJEXT) \
$(HLVIS_OUTDIR)/mathlib$(OBJEXT) \
$(HLVIS_OUTDIR)/messages$(OBJEXT) \
$(HLVIS_OUTDIR)/scriplib$(OBJEXT) \
$(HLVIS_OUTDIR)/threads$(OBJEXT) \
$(HLVIS_OUTDIR)/winding$(OBJEXT) \


