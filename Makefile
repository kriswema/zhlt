CXX=g++
CXXFLAGS=-pthread -DNDEBUG -DHAVE_CONFIG_H -DHLCSG -DSTDC_HEADERS -D_CONSOLE -DSYSTEM -DSYSTEM_POSIX -DHAVE_ASM_ATOMIC_H -DHAVE_ATOMIC -DHAVE_FCNTL_H -DHAVE_PTHREAD_H -DHAVE_STDDEF_H -DHAVE_SYS_ATOMIC_H -DHAVE_SYS_RESOURCE_H -DHAVE_SYS_STAT_H -DHAVE_SYS_TIME_H -DHAVE_UNISTD_E -DHAVE_UNISTD_H -pthread
INCLUDES=-Icommon -Itemplate
LIBPATH=-Lcommon
EXEDIR=build

all: hlcsg_ hlbsp_ hlvis_ hlrad_

hlcsg_:
	mkdir -p $(EXEDIR)
	$(CXX) $(INCLUDES) $(CXXFLAGS) -o build/hlcsg -DDOUBLEVEC_T common/blockmem.cpp common/bspfile.cpp common/cmdlib.cpp common/filelib.cpp common/files.cpp common/log.cpp common/mathlib.cpp common/messages.cpp common/resourcelock.cpp common/scriplib.cpp common/threads.cpp common/winding.cpp hlcsg/autowad.cpp hlcsg/brush.cpp hlcsg/brushunion.cpp hlcsg/hullfile.cpp hlcsg/map.cpp hlcsg/netvis_in_vis.cpp hlcsg/properties.cpp hlcsg/qcsg.cpp hlcsg/textures.cpp hlcsg/wadcfg.cpp hlcsg/wadinclude.cpp hlcsg/wadpath.cpp

hlbsp_:
	mkdir -p $(EXEDIR)
	$(CXX) $(INCLUDES) $(CXXFLAGS) -o build/hlbsp -DDOUBLEVEC_T common/blockmem.cpp common/bspfile.cpp common/cmdlib.cpp common/filelib.cpp common/files.cpp common/log.cpp common/mathlib.cpp common/messages.cpp common/resourcelock.cpp common/scriplib.cpp common/threads.cpp common/winding.cpp hlbsp/merge.cpp hlbsp/outside.cpp hlbsp/portals.cpp hlbsp/qbsp.cpp hlbsp/solidbsp.cpp hlbsp/surfaces.cpp hlbsp/tjunc.cpp hlbsp/writebsp.cpp

hlvis_:
	mkdir -p $(EXEDIR)
	$(CXX) $(INCLUDES) $(CXXFLAGS) -o build/hlvis -DDOUBLEVEC_T common/blockmem.cpp common/bspfile.cpp common/cmdlib.cpp common/filelib.cpp common/files.cpp common/log.cpp common/mathlib.cpp common/messages.cpp common/resourcelock.cpp common/scriplib.cpp common/threads.cpp common/winding.cpp hlvis/flow.cpp hlvis/vis.cpp hlvis/zones.cpp

hlrad_:
	mkdir -p $(EXEDIR)
	$(CXX) $(INCLUDES) $(CXXFLAGS) -o build/hlrad common/blockmem.cpp common/bspfile.cpp common/cmdlib.cpp common/filelib.cpp common/files.cpp common/log.cpp common/mathlib.cpp common/messages.cpp common/resourcelock.cpp common/scriplib.cpp common/threads.cpp common/winding.cpp hlrad/lerp.cpp hlrad/lightmap.cpp hlrad/mathutil.cpp hlrad/nomatrix.cpp hlrad/qrad.cpp hlrad/qradutil.cpp hlrad/sparse.cpp hlrad/trace.cpp hlrad/transfers.cpp hlrad/transparency.cpp hlrad/vismatrix.cpp hlrad/vismatrixutil.cpp

clean:
	rm -rf build
