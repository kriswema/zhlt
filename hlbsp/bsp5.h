#ifndef HLBSP_H__
#define HLBSP_H__

#if _MSC_VER >= 1000
#pragma once
#endif

#include "cmdlib.h"
#include "messages.h"
#include "win32fix.h"
#include "log.h"
#include "hlassert.h"
#include "mathlib.h"
#include "bspfile.h"
#include "blockmem.h"
#include "filelib.h"
#include "threads.h"
#include "winding.h"

#define ENTITIES_VOID "entities.void"
#define ENTITIES_VOID_EXT ".void"

#define	BOGUS_RANGE	18000

// the exact bounding box of the brushes is expanded some for the headnode
// volume.  is this still needed?
#define	SIDESPACE	24

//============================================================================

#define MIN_SUBDIVIDE_SIZE      64

#ifdef ZHLT_GENERAL
#define MAX_SUBDIVIDE_SIZE      512
#else
#define MAX_SUBDIVIDE_SIZE      240
#endif

#define DEFAULT_SUBDIVIDE_SIZE  240

#define MIN_MAXNODE_SIZE        64
#define MAX_MAXNODE_SIZE        8192
#define DEFAULT_MAXNODE_SIZE    1024

#define DEFAULT_NOFILL          false
#define DEFAULT_NOTJUNC         false
#define DEFAULT_NOCLIP          false
#define DEFAULT_NOOPT			false
#define DEFAULT_LEAKONLY        false
#define DEFAULT_WATERVIS        false
#define DEFAULT_CHART           false
#define DEFAULT_INFO            true

#ifdef ZHLT_NULLTEX // AJM
#define DEFAULT_NULLTEX             true
#endif

#ifdef ZHLT_PROGRESSFILE // AJM
#define DEFAULT_PROGRESSFILE NULL // progress file is only used if g_progressfile is non-null
#endif

#ifdef SYSTEM_WIN32
#define DEFAULT_ESTIMATE        false
#endif

#ifdef SYSTEM_POSIX
#define DEFAULT_ESTIMATE        true
#endif

#ifdef ZHLT_DETAIL // AJM
#define DEFAULT_DETAIL      true
#endif

#define	MAXEDGES			48                 // 32
#define	MAXPOINTS			28                 // don't let a base face get past this
                                                                              // because it can be split more later
#define MAXNODESIZE     1024                               // Valve default is 1024

typedef enum
{
    face_normal = 0,
    face_hint,
    face_skip,
#ifdef ZHLT_NULLTEX // AJM
    face_null,
#endif
#ifdef ZHLT_DETAIL // AJM
    face_detail
#endif
}
facestyle_e;

typedef struct face_s                                      // This structure is layed out so 'pts' is on a quad-word boundary (and the pointers are as well)
{
    struct face_s*  next;
    int             planenum;
    int             texturenum;
    int             contents;                              // contents in front of face

    struct face_s*  original;                              // face on node
    int             outputnumber;                          // only valid for original faces after write surfaces
    int             numpoints;
    facestyle_e     facestyle;

    // vector quad word aligned
    vec3_t          pts[MAXEDGES];                         // FIXME: change to use winding_t

}
face_t;

typedef struct surface_s
{
    struct surface_s* next;
    int             planenum;
    vec3_t          mins, maxs;
    struct node_s*  onnode;                                // true if surface has already been used
    // as a splitting node
    face_t*         faces;                                 // links to all the faces on either side of the surf
}
surface_t;

typedef struct
{
    vec3_t          mins, maxs;
    surface_t*      surfaces;
}
surfchain_t;

//
// there is a node_t structure for every node and leaf in the bsp tree
//
#define	PLANENUM_LEAF		-1

typedef struct node_s
{
    surface_t*      surfaces;

    vec3_t          mins, maxs;                            // bounding volume of portals;

    // information for decision nodes
    int             planenum;                              // -1 = leaf node
    struct node_s*  children[2];                           // only valid for decision nodes
    face_t*         faces;                                 // decision nodes only, list for both sides

    // information for leafs
    int             contents;                              // leaf nodes (0 for decision nodes)
    face_t**        markfaces;                             // leaf nodes only, point to node faces
    struct portal_s* portals;
    int             visleafnum;                            // -1 = solid
    int             valid;                                 // for flood filling
    int             occupied;                              // light number in leaf for outside filling
}
node_t;

#define	NUM_HULLS		4

//=============================================================================
// solidbsp.c
extern void     SubdivideFace(face_t* f, face_t** prevptr);
extern node_t*  SolidBSP(const surfchain_t* const surfhead, bool report_progress);

//=============================================================================
// merge.c
extern void     MergePlaneFaces(surface_t* plane);
extern void     MergeAll(surface_t* surfhead);

//=============================================================================
// surfaces.c
extern void     MakeFaceEdges();
extern int      GetEdge(const vec3_t p1, const vec3_t p2, face_t* f);

//=============================================================================
// portals.c
typedef struct portal_s
{
    dplane_t        plane;
    node_t*         onnode;                                // NULL = outside box
    node_t*         nodes[2];                              // [0] = front side of plane
    struct portal_s* next[2];
    Winding*        winding;
}
portal_t;

extern node_t   g_outside_node;                            // portals outside the world face this

extern void     AddPortalToNodes(portal_t* p, node_t* front, node_t* back);
extern void     RemovePortalFromNode(portal_t* portal, node_t* l);
extern void     MakeHeadnodePortals(node_t* node, const vec3_t mins, const vec3_t maxs);

extern void     FreePortals(node_t* node);
extern void     WritePortalfile(node_t* headnode);

//=============================================================================
// tjunc.c
void            tjunc(node_t* headnode);

//=============================================================================
// writebsp.c
extern void     WriteClipNodes(node_t* headnode);
extern void     WriteDrawNodes(node_t* headnode);

extern void     BeginBSPFile();
extern void     FinishBSPFile();

//=============================================================================
// outside.c
extern node_t*  FillOutside(node_t* node, bool leakfile, unsigned hullnum);
extern void     LoadAllowableOutsideList(const char* const filename);
extern void     FreeAllowableOutsideList();

//=============================================================================
// misc functions
extern void     GetParamsFromEnt(entity_t* mapent);

extern face_t*  AllocFace();
extern void     FreeFace(face_t* f);

extern struct portal_s* AllocPortal();
extern void     FreePortal(struct portal_s* p);

extern surface_t* AllocSurface();
extern void     FreeSurface(surface_t* s);

extern node_t*  AllocNode();

extern bool     CheckFaceForHint(const face_t* const f);
extern bool     CheckFaceForSkip(const face_t* const f);
#ifdef ZHLT_NULLTEX// AJM
extern bool     CheckFaceForNull(const face_t* const f);
#endif


// =====================================================================================
//Cpt_Andrew - UTSky Check
// =====================================================================================
extern bool     CheckFaceForEnv_Sky(const face_t* const f);
// =====================================================================================


#ifdef ZHLT_DETAIL // AJM
extern bool     CheckFaceForDetail(const face_t* const f);
#endif

//=============================================================================
// cull.c
extern void     CullStuff();

//=============================================================================
// qbsp.c
extern bool     g_nofill;
extern bool     g_notjunc;
extern bool     g_watervis;
extern bool     g_chart;
extern bool     g_estimate;
extern int      g_maxnode_size;
extern int      g_subdivide_size;
extern int      g_hullnum;
extern bool     g_bLeakOnly;
extern bool     g_bLeaked;
extern char     g_portfilename[_MAX_PATH];
extern char     g_pointfilename[_MAX_PATH];
extern char     g_linefilename[_MAX_PATH];
extern char     g_bspfilename[_MAX_PATH];


#ifdef ZHLT_DETAIL // AJM
extern bool g_bDetailBrushes;
#endif

#ifdef ZHLT_NULLTEX // AJM
extern bool     g_bUseNullTex;
#endif

extern face_t*  NewFaceFromFace(const face_t* const in);
extern void     SplitFace(face_t* in, const dplane_t* const split, face_t** front, face_t** back);

#endif // qbsp.c====================================================================== HLBSP_H__
