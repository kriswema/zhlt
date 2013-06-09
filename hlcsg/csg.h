#ifndef HLCSG_H__
#define HLCSG_H__

#if _MSC_VER >= 1000
#pragma once
#endif

#include <deque>
#include <string>
#include <map>

#include "cmdlib.h"
#include "messages.h"
#include "win32fix.h"
#include "log.h"
#include "hlassert.h"
#include "mathlib.h"
#include "scriplib.h"
#include "winding.h"
#include "threads.h"
#include "bspfile.h"
#include "blockmem.h"
#include "filelib.h"
#include "boundingbox.h"
// AJM: added in
#include "wadpath.h"

#ifndef DOUBLEVEC_T
#error you must add -dDOUBLEVEC_T to the project!
#endif

#define DEFAULT_BRUSH_UNION_THRESHOLD 0.0f
#define DEFAULT_TINY_THRESHOLD        0.5
#define DEFAULT_NOCLIP      false
#define DEFAULT_ONLYENTS    false
#define DEFAULT_WADTEXTURES true
#define DEFAULT_SKYCLIP     true
#define DEFAULT_CHART       false
#define DEFAULT_INFO        true

#ifdef HLCSG_PRECISIONCLIP // KGP
#define FLOOR_Z 0.5
#define DEFAULT_CLIPTYPE clip_legacy
#endif

#ifdef ZHLT_NULLTEX // AJM
#define DEFAULT_NULLTEX     true
#endif

#ifdef HLCSG_CLIPECONOMY // AJM
#define DEFAULT_CLIPNAZI    true
#endif

#ifdef HLCSG_AUTOWAD //  AJM
#define DEFAULT_WADAUTODETECT false
#endif

#ifdef ZHLT_DETAIL // AJM
#define DEFAULT_DETAIL      true
#endif

#ifdef ZHLT_PROGRESSFILE // AJM
#define DEFAULT_PROGRESSFILE NULL // progress file is only used if g_progressfile is non-null
#endif

// AJM: added in
#define UNLESS(a)  if (!(a))

#ifdef SYSTEM_WIN32
#define DEFAULT_ESTIMATE    false
#endif

#ifdef SYSTEM_POSIX
#define DEFAULT_ESTIMATE    true
#endif

#define BOGUS_RANGE    8192

typedef struct
{
    vec3_t          normal;
    vec3_t          origin;
    vec_t           dist;
    planetypes      type;
} plane_t;



typedef struct
{
    vec3_t          UAxis;
    vec3_t          VAxis;
    vec_t           shift[2];
    vec_t           rotate;
    vec_t           scale[2];
} valve_vects;

typedef struct
{
    float           vects[2][4];
} quark_vects;

typedef union
{
    valve_vects     valve;
    quark_vects     quark;
}
vects_union;

extern int      g_nMapFileVersion;                         // map file version * 100 (ie 201), zero for pre-Worldcraft 2.0.1 maps

typedef struct
{
    char            txcommand;
    vects_union     vects;
    char            name[32];
} brush_texture_t;

typedef struct side_s
{
    brush_texture_t td;
    vec_t           planepts[3][3];
} side_t;

typedef struct bface_s
{
    struct bface_s* next;
    int             planenum;
    plane_t*        plane;
    Winding*        w;
    int             texinfo;
    bool            used;                                  // just for face counting
    int             contents;
    int             backcontents;
    BoundingBox     bounds;
} bface_t;

// NUM_HULLS should be no larger than MAX_MAP_HULLS
#define NUM_HULLS 4

typedef struct
{
    BoundingBox     bounds;
    bface_t*        faces;
} brushhull_t;

typedef struct brush_s
{
    int             entitynum;
    int             brushnum;

    int             firstside;
    int             numsides;

#ifdef HLCSG_CLIPECONOMY // AJM
    unsigned int    noclip; // !!!FIXME: this should be a flag bitfield so we can use it for other stuff (ie. is this a detail brush...)
#endif

    int             contents;
    brushhull_t     hulls[NUM_HULLS];
} brush_t;


//=============================================================================
// map.c

extern int      g_nummapbrushes;
extern brush_t  g_mapbrushes[MAX_MAP_BRUSHES];

#define MAX_MAP_SIDES   (MAX_MAP_BRUSHES*6)

extern int      g_numbrushsides;
extern side_t   g_brushsides[MAX_MAP_SIDES];

extern void     TextureAxisFromPlane(const plane_t* const pln, vec3_t xv, vec3_t yv);
extern void     LoadMapFile(const char* const filename);

//=============================================================================
// textures.c

typedef std::deque< std::string >::iterator WadInclude_i;
extern std::deque< std::string > g_WadInclude;  // List of substrings to wadinclude

extern void     WriteMiptex();
extern int      TexinfoForBrushTexture(const plane_t* const plane, brush_texture_t* bt, const vec3_t origin);

//=============================================================================
// brush.c

extern brush_t* Brush_LoadEntity(entity_t* ent, int hullnum);
extern contents_t CheckBrushContents(const brush_t* const b);

extern void     CreateBrush(int brushnum);

//=============================================================================
// csg.c

extern bool     g_chart;
extern bool     g_onlyents;
extern bool     g_noclip;
extern bool     g_wadtextures;
extern bool     g_skyclip;
extern bool     g_estimate;         
extern const char* g_hullfile;        

#ifdef ZHLT_NULLTEX // AJM:
extern bool     g_bUseNullTex; 
#endif

#ifdef ZHLT_DETAIL // AJM
extern bool g_bDetailBrushes;
#endif

#ifdef HLCSG_CLIPECONOMY // AJM:
extern bool     g_bClipNazi; 
#endif

#ifdef HLCSG_PRECISIONCLIP // KGP
#define EnumPrint(a) #a
typedef enum{clip_smallest,clip_normalized,clip_simple,clip_precise,clip_legacy} cliptype;
extern cliptype g_cliptype;
extern const char*	GetClipTypeString(cliptype);
#define TEX_BEVEL 32768
#endif

#ifdef ZHLT_PROGRESSFILE // AJM
extern char*    g_progressfile ;
#endif

extern vec_t    g_tiny_threshold;
extern vec_t    g_BrushUnionThreshold;

extern plane_t  g_mapplanes[MAX_INTERNAL_MAP_PLANES];
extern int      g_nummapplanes;

extern bface_t* NewFaceFromFace(const bface_t* const in);
extern bface_t* CopyFace(const bface_t* const f);

extern void     FreeFace(bface_t* f);

extern bface_t* CopyFaceList(bface_t* f);
extern void     FreeFaceList(bface_t* f);

extern void     GetParamsFromEnt(entity_t* mapent);

//=============================================================================
// wadinclude.c
// passed 'filename' is extensionless, the function cats ".wic" at runtime

extern void     LoadWadincludeFile(const char* const filename);
extern void     SaveWadincludeFile(const char* const filename);
extern void     HandleWadinclude();

//=============================================================================
// brushunion.c
void            CalculateBrushUnions(int brushnum);
 
//============================================================================
// hullfile.cpp
extern vec3_t   g_hull_size[NUM_HULLS][2];
extern void     LoadHullfile(const char* filename);

#ifdef HLCSG_WADCFG // AJM: 
//============================================================================
// wadcfg.cpp

extern void     LoadWadConfigFile();
extern void     ProcessWadConfiguration();
extern bool     g_bWadConfigsLoaded;
extern void     WadCfg_cleanup();

#define MAX_WAD_CFG_NAME 32
extern char     wadconfigname[MAX_WAD_CFG_NAME];

//JK: needed in wadcfg.cpp for *nix..
#ifndef SYSTEM_WIN32
extern char *g_apppath;
#endif

//JK: 
extern char *g_wadcfgfile;

#endif // HLCSG_WADCFG

#ifdef HLCSG_AUTOWAD
//============================================================================
// autowad.cpp      AJM

extern bool     g_bWadAutoDetect; 
extern int      g_numUsedTextures;

extern void     GetUsedTextures();
extern bool     autowad_IsUsedTexture(const char* const texname);
//extern bool     autowad_IsUsedWad(const char* const path);
//extern void     autowad_PurgeName(const char* const texname);
extern void     autowad_cleanup();
extern void     autowad_UpdateUsedWads();

#endif // HLCSG_AUTOWAD

//=============================================================================
// properties.cpp

#ifdef HLCSG_NULLIFY_INVISIBLE // KGP
#include <string>
#include <set>
extern void properties_initialize(const char* filename);
extern std::set<std::string> g_invisible_items;
#endif

//============================================================================
#endif//HLCSG_H__
