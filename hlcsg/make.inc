HLCSG_DEFINES+=-D DOUBLEVEC_T
HLCSG:=$(OUTDIR)/hlcsg$(EXEEXT)
HLCSG_SRCDIR:=hlcsg
HLCSG_OUTDIR:=$(HLCSG_SRCDIR)/$(OUTDIR)


HLCSG_INPUT := \
$(HLCSG_SRCDIR)/brush.cpp \
$(HLCSG_SRCDIR)/map.cpp \
$(HLCSG_SRCDIR)/qcsg.cpp \
$(HLCSG_SRCDIR)/textures.cpp \
$(HLCSG_SRCDIR)/wadinclude.cpp \
$(HLCSG_SRCDIR)/brushunion.cpp \
$(HLCSG_SRCDIR)/hullfile.cpp \
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


HLCSG_OUTPUT := \
$(HLCSG_OUTDIR)/brush$(OBJEXT) \
$(HLCSG_OUTDIR)/map$(OBJEXT) \
$(HLCSG_OUTDIR)/qcsg$(OBJEXT) \
$(HLCSG_OUTDIR)/textures$(OBJEXT) \
$(HLCSG_OUTDIR)/wadinclude$(OBJEXT) \
$(HLCSG_OUTDIR)/brushunion$(OBJEXT) \
$(HLCSG_OUTDIR)/hullfile$(OBJEXT) \
$(HLCSG_OUTDIR)/blockmem$(OBJEXT) \
$(HLCSG_OUTDIR)/bspfile$(OBJEXT) \
$(HLCSG_OUTDIR)/cmdlib$(OBJEXT) \
$(HLCSG_OUTDIR)/filelib$(OBJEXT) \
$(HLCSG_OUTDIR)/log$(OBJEXT) \
$(HLCSG_OUTDIR)/mathlib$(OBJEXT) \
$(HLCSG_OUTDIR)/messages$(OBJEXT) \
$(HLCSG_OUTDIR)/scriplib$(OBJEXT) \
$(HLCSG_OUTDIR)/threads$(OBJEXT) \
$(HLCSG_OUTDIR)/winding$(OBJEXT) \

