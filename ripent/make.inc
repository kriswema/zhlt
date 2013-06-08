RIPENT_DEFINES+=
RIPENT:=$(OUTDIR)/ripent$(EXEEXT)
RIPENT_SRCDIR:=ripent
RIPENT_OUTDIR:=$(RIPENT_SRCDIR)/$(OUTDIR)


RIPENT_INPUT := \
$(RIPENT_SRCDIR)/ripent.cpp \
$(COMMON_SRCDIR)/blockmem.cpp \
$(COMMON_SRCDIR)/bspfile.cpp \
$(COMMON_SRCDIR)/cmdlib.cpp \
$(COMMON_SRCDIR)/filelib.cpp \
$(COMMON_SRCDIR)/log.cpp \
$(COMMON_SRCDIR)/messages.cpp \
$(COMMON_SRCDIR)/scriplib.cpp \
$(COMMON_SRCDIR)/mathlib.cpp \


RIPENT_OUTPUT := \
$(RIPENT_OUTDIR)/ripent$(OBJEXT) \
$(RIPENT_OUTDIR)/blockmem$(OBJEXT) \
$(RIPENT_OUTDIR)/bspfile$(OBJEXT) \
$(RIPENT_OUTDIR)/cmdlib$(OBJEXT) \
$(RIPENT_OUTDIR)/filelib$(OBJEXT) \
$(RIPENT_OUTDIR)/log$(OBJEXT) \
$(RIPENT_OUTDIR)/messages$(OBJEXT) \
$(RIPENT_OUTDIR)/scriplib$(OBJEXT) \
$(RIPENT_OUTDIR)/mathlib$(OBJEXT) \
