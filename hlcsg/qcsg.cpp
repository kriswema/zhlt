#pragma warning(disable: 4018) // '<' : signed/unsigned mismatch

/*
 
    CONSTRUCTIVE SOLID GEOMETRY    -aka-    C S G 

    Code based on original code from Valve Software, 
    Modified by Sean "Zoner" Cavanaugh (seanc@gearboxsoftware.com) with permission.
    Modified by Tony "Merl" Moore (merlinis@bigpond.net.au) [AJM]
    
*/

#include "csg.h" 

/*

 NOTES

 - check map size for +/- 4k limit at load time
 - allow for multiple wad.cfg configurations per compile

*/

static FILE*    out[NUM_HULLS]; // pointer to each of the hull out files (.p0, .p1, ect.)  
static int      c_tiny;        
static int      c_tiny_clip;
static int      c_outfaces;
static int      c_csgfaces;
BoundingBox     world_bounds;

#ifdef HLCSG_WADCFG
char            wadconfigname[MAX_WAD_CFG_NAME];
#endif

vec_t           g_tiny_threshold = DEFAULT_TINY_THRESHOLD;
     
bool            g_noclip = DEFAULT_NOCLIP;              // no clipping hull "-noclip"
bool            g_onlyents = DEFAULT_ONLYENTS;          // onlyents mode "-onlyents"
bool            g_wadtextures = DEFAULT_WADTEXTURES;    // "-nowadtextures"
bool            g_chart = DEFAULT_CHART;                // show chart "-chart"
bool            g_skyclip = DEFAULT_SKYCLIP;            // no sky clipping "-noskyclip"
bool            g_estimate = DEFAULT_ESTIMATE;          // progress estimates "-estimate"
bool            g_info = DEFAULT_INFO;                  // "-info" ?
const char*     g_hullfile = NULL;                      // external hullfile "-hullfie sdfsd"

#ifdef ZHLT_NULLTEX // AJM
bool            g_bUseNullTex = DEFAULT_NULLTEX;        // "-nonulltex"
#endif

#ifdef HLCSG_PRECISIONCLIP // KGP
cliptype		g_cliptype = DEFAULT_CLIPTYPE;			// "-cliptype <value>"
#endif

#ifdef HLCSG_NULLIFY_INVISIBLE
const char*			g_nullfile = NULL;
#endif

#ifdef HLCSG_CLIPECONOMY // AJM
bool            g_bClipNazi = DEFAULT_CLIPNAZI;         // "-noclipeconomy"
#endif

#ifdef HLCSG_AUTOWAD // AJM
bool            g_bWadAutoDetect = DEFAULT_WADAUTODETECT; // "-wadautodetect"
#endif

#ifdef ZHLT_DETAIL // AJM
bool            g_bDetailBrushes = DEFAULT_DETAIL; // "-detail"
#endif

#ifdef ZHLT_PROGRESSFILE // AJM
char*           g_progressfile = DEFAULT_PROGRESSFILE; // "-progressfile path"
#endif

#ifdef ZHLT_INFO_COMPILE_PARAMETERS
// =====================================================================================
//  GetParamsFromEnt
//      parses entity keyvalues for setting information
// =====================================================================================
void            GetParamsFromEnt(entity_t* mapent)
{
    int     iTmp;
    char    szTmp[256];

    Log("\nCompile Settings detected from info_compile_parameters entity\n");

    // verbose(choices) : "Verbose compile messages" : 0 = [ 0 : "Off" 1 : "On" ]
    iTmp = IntForKey(mapent, "verbose");
    if (iTmp == 1)
    {
        g_verbose = true;
    }
    else if (iTmp == 0)
    {
        g_verbose = false;
    }
    Log("%30s [ %-9s ]\n", "Compile Option", "setting");
    Log("%30s [ %-9s ]\n", "Verbose Compile Messages", g_verbose ? "on" : "off");

    // estimate(choices) :"Estimate Compile Times?" : 0 = [ 0: "Yes" 1: "No" ]
    if (IntForKey(mapent, "estimate")) 
    {
        g_estimate = true;
    }
    else
    {
        g_estimate = false;
    }
    Log("%30s [ %-9s ]\n", "Estimate Compile Times", g_estimate ? "on" : "off");

	// priority(choices) : "Priority Level" : 0 = [	0 : "Normal" 1 : "High"	-1 : "Low" ]
	if (!strcmp(ValueForKey(mapent, "priority"), "1"))
    {
        g_threadpriority = eThreadPriorityHigh;
        Log("%30s [ %-9s ]\n", "Thread Priority", "high");
    }
    else if (!strcmp(ValueForKey(mapent, "priority"), "-1"))
    {
        g_threadpriority = eThreadPriorityLow;
        Log("%30s [ %-9s ]\n", "Thread Priority", "low");
    }

    // texdata(string) : "Texture Data Memory" : "4096"
    iTmp = IntForKey(mapent, "texdata") * 1024;
    if (iTmp > g_max_map_miptex)
    {
        g_max_map_miptex = iTmp;
    }
    sprintf_s(szTmp, "%i", g_max_map_miptex);
    Log("%30s [ %-9s ]\n", "Texture Data Memory", szTmp);

    // hullfile(string) : "Custom Hullfile"
    if (ValueForKey(mapent, "hullfile"))
    {
        g_hullfile = ValueForKey(mapent, "hullfile");
        Log("%30s [ %-9s ]\n", "Custom Hullfile", g_hullfile);
    }

#ifdef HLCSG_AUTOWAD
    // wadautodetect(choices) : "Wad Auto Detect" : 0 =	[ 0 : "Off" 1 : "On" ]
    if (!strcmp(ValueForKey(mapent, "wadautodetect"), "1"))
    { 
        g_bWadAutoDetect = true;
    }
    else
    {
        g_bWadAutoDetect = false;
    }
    Log("%30s [ %-9s ]\n", "Wad Auto Detect", g_bWadAutoDetect ? "on" : "off");
#endif

#ifdef HLCSG_WADCFG
	// wadconfig(string) : "Custom Wad Configuration" : ""
    if (strlen(ValueForKey(mapent, "wadconfig")) > 0)
    { 
        safe_strncpy(wadconfigname, ValueForKey(mapent, "wadconfig"), MAX_WAD_CFG_NAME);
        Log("%30s [ %-9s ]\n", "Custom Wad Configuration", wadconfigname);
    }
#endif

#ifdef HLCSG_CLIPECONOMY
    // noclipeconomy(choices) : "Strip Uneeded Clipnodes?" : 1 = [ 1 : "Yes" 0 : "No" ]
    iTmp = IntForKey(mapent, "noclipeconomy");
    if (iTmp == 1)
    {
        g_bClipNazi = true;
    }
    else if (iTmp == 0)
    {
        g_bClipNazi = false;
    }        
    Log("%30s [ %-9s ]\n", "Clipnode Economy Mode", g_bClipNazi ? "on" : "off");
#endif

    /*
    hlcsg(choices) : "HLCSG" : 1 =
    [
        1 : "Normal"
        2 : "Onlyents"
        0 : "Off"
    ]
    */
    iTmp = IntForKey(mapent, "hlcsg");
    g_onlyents = false;
    if (iTmp == 2)
    {
        g_onlyents = true;
    }
    else if (iTmp == 0)
    {
        Fatal(assume_TOOL_CANCEL, 
            "%s was set to \"Off\" (0) in info_compile_parameters entity, execution cancelled", g_Program);
        CheckFatal();  
    }
    Log("%30s [ %-9s ]\n", "Onlyents", g_onlyents ? "on" : "off");

    /*
    nocliphull(choices) : "Generate clipping hulls" : 0 =
    [
        0 : "Yes"
        1 : "No"
    ]
    */
    iTmp = IntForKey(mapent, "nocliphull");
    if (iTmp == 1)
    {
        g_noclip = true;
    }
    else 
    {
        g_noclip = false;
    }
    Log("%30s [ %-9s ]\n", "Clipping Hull Generation", g_noclip ? "off" : "on");
#ifdef HLCSG_PRECISIONCLIP
    // cliptype(choices) : "Clip Hull Type" : 4 = [ 0 : "Smallest" 1 : "Normalized" 2: "Simple" 3 : "Precise" 4 : "Legacy" ]
    iTmp = IntForKey(mapent, "cliptype");
	switch(iTmp)
	{
	case 0:
		g_cliptype = clip_smallest;
		break;
	case 1:
		g_cliptype = clip_normalized;
		break;
	case 2:
		g_cliptype = clip_simple;
		break;
	case 3:
		g_cliptype = clip_precise;
		break;
	default:
		g_cliptype = clip_legacy;
		break;
	}
    Log("%30s [ %-9s ]\n", "Clip Hull Type", GetClipTypeString(g_cliptype));
#endif
    /*
    noskyclip(choices) : "No Sky Clip" : 0 =
    [
        1 : "On"
        0 : "Off"
    ]
    */
    iTmp = IntForKey(mapent, "noskyclip");
    if (iTmp == 1)
    {
        g_skyclip = false;
    }
    else 
    {
        g_skyclip = true;
    }
    Log("%30s [ %-9s ]\n", "Sky brush clip generation", g_skyclip ? "on" : "off");

    ///////////////
    Log("\n");
}
#endif

// =====================================================================================
// FixBevelTextures
// =====================================================================================

void FixBevelTextures()
{
	for(int counter = 0; counter < g_numtexinfo; counter++)
	{
		if(g_texinfo[counter].flags & TEX_BEVEL)
		{ g_texinfo[counter].flags &= ~TEX_BEVEL; }
	}
}

// =====================================================================================
//  NewFaceFromFace
//      Duplicates the non point information of a face, used by SplitFace
// =====================================================================================
bface_t*        NewFaceFromFace(const bface_t* const in)
{
    bface_t*        newf;

    newf = (bface_t*)Alloc(sizeof(bface_t));

    newf->contents = in->contents;
    newf->texinfo = in->texinfo;
    newf->planenum = in->planenum;
    newf->plane = in->plane;

    return newf;
}

// =====================================================================================
//  FreeFace
// =====================================================================================
void            FreeFace(bface_t* f)
{
    delete f->w;
    Free(f);
}

// =====================================================================================
//  ClipFace
//      Clips a faces by a plane, returning the fragment on the backside and adding any 
//      fragment to the outside.
//      Faces exactly on the plane will stay inside unless overdrawn by later brush.
//      Frontside is the side of the plane that holds the outside list.
//      Precedence is necesary to handle overlapping coplanar faces.
#define	SPLIT_EPSILON	0.3
// =====================================================================================
static bface_t* ClipFace(bface_t* f, bface_t** outside, const int splitplane, const bool precedence)
{
    bface_t*        front;  // clip face
    Winding*        fw;     // forward wind
    Winding*        bw;     // back wind
    plane_t*        split; // plane to clip on

    // handle exact plane matches special

    if (f->planenum == (splitplane ^ 1)) 
        return f;    // opposite side, so put on inside list

    if (f->planenum == splitplane)  // coplanar
    {       
        // this fragment will go to the inside, because
        //   the earlier one was clipped to the outside
        if (precedence)
            return f;

        f->next = *outside;
        *outside = f;
        return NULL;
    }

    split = &g_mapplanes[splitplane];
    f->w->Clip(split->normal, split->dist, &fw, &bw);

    if (!fw)
    {
        delete bw;
        return f;
    }
    else if (!bw)
    {
        delete fw;
        f->next = *outside;
        *outside = f;
        return NULL;
    }
    else
    {
        delete f->w;
    
        front = NewFaceFromFace(f);
        front->w = fw;
        fw->getBounds(front->bounds);
        front->next = *outside;
        *outside = front;
    
        f->w = bw;
        bw->getBounds(f->bounds);
    
        return f;
    }
}

// =====================================================================================
//  WriteFace
// =====================================================================================
void            WriteFace(const int hull, const bface_t* const f)
{
    unsigned int    i;
    Winding*        w;

    ThreadLock();
    if (!hull)
        c_csgfaces++;

    // .p0 format
    w = f->w;

    // plane summary
    fprintf(out[hull], "%i %i %i %u\n", f->planenum, f->texinfo, f->contents, w->m_NumPoints);

    // for each of the points on the face
    for (i = 0; i < w->m_NumPoints; i++)
    {
        // write the co-ords
        fprintf(out[hull], "%5.2f %5.2f %5.2f\n", w->m_Points[i][0], w->m_Points[i][1], w->m_Points[i][2]);
    }

    // put in an extra line break
    fprintf(out[hull], "\n");

    ThreadUnlock();
}

// =====================================================================================
//  SaveOutside
//      The faces remaining on the outside list are final polygons.  Write them to the 
//      output file.
//      Passable contents (water, lava, etc) will generate a mirrored copy of the face 
//      to be seen from the inside.
// =====================================================================================
static void     SaveOutside(const brush_t* const b, const int hull, bface_t* outside, const int mirrorcontents)
{
    bface_t*        f;
    bface_t*        f2;
    bface_t*        next;
    int             i;
    vec3_t          temp;

    for (f = outside; f; f = next)
    {
        next = f->next;

        if (f->w->getArea() < g_tiny_threshold)
        {
            c_tiny++;
            Verbose("Entity %i, Brush %i: tiny fragment\n", b->entitynum, b->brushnum);
            continue;
        }

        // count unique faces
        if (!hull)
        {
            for (f2 = b->hulls[hull].faces; f2; f2 = f2->next)
            {
                if (f2->planenum == f->planenum)
                {
                    if (!f2->used)
                    {
                        f2->used = true;
                        c_outfaces++;
                    }
                    break;
                }
            }
        }

        WriteFace(hull, f);

        //              if (mirrorcontents != CONTENTS_SOLID)
        {
            f->planenum ^= 1;
            f->plane = &g_mapplanes[f->planenum];
            f->contents = mirrorcontents;

            // swap point orders
            for (i = 0; i < f->w->m_NumPoints / 2; i++)      // add points backwards
            {
                VectorCopy(f->w->m_Points[i], temp);
                VectorCopy(f->w->m_Points[f->w->m_NumPoints - 1 - i], f->w->m_Points[i]);
                VectorCopy(temp, f->w->m_Points[f->w->m_NumPoints - 1 - i]);
            }
            WriteFace(hull, f);
        }

        FreeFace(f);
    }
}

// =====================================================================================
//  CopyFace
// =====================================================================================
bface_t*        CopyFace(const bface_t* const f)
{
    bface_t*        n;

    n = NewFaceFromFace(f);
    n->w = f->w->Copy();
    n->bounds = f->bounds;
    return n;
}

// =====================================================================================
//  CopyFaceList
// =====================================================================================
bface_t*        CopyFaceList(bface_t* f)
{
    bface_t*        head;
    bface_t*        n;

    if (f)
    {
        head = CopyFace(f);
        n = head;
        f = f->next;

        while (f)
        {
            n->next = CopyFace(f);

            n = n->next;
            f = f->next;
        }

        return head;
    }
    else
    {
        return NULL;
    }
}

// =====================================================================================
//  FreeFaceList
// =====================================================================================
void            FreeFaceList(bface_t* f)
{
    if (f)
    {
        if (f->next)
        {
            FreeFaceList(f->next);
        }
        FreeFace(f);
    }
}

// =====================================================================================
//  CopyFacesToOutside
//      Make a copy of all the faces of the brush, so they can be chewed up by other 
//      brushes.
//      All of the faces start on the outside list.
//      As other brushes take bites out of the faces, the fragments are moved to the 
//      inside list, so they can be freed when they are determined to be completely 
//      enclosed in solid.
// =====================================================================================
static bface_t* CopyFacesToOutside(brushhull_t* bh)
{
    bface_t*        f;
    bface_t*        newf;
    bface_t*        outside;

    outside = NULL;

    for (f = bh->faces; f; f = f->next)
    {
        newf = CopyFace(f);
        newf->w->getBounds(newf->bounds);
        newf->next = outside;
        outside = newf;
    }

    return outside;
}

// =====================================================================================
//  CSGBrush
// =====================================================================================
static void     CSGBrush(int brushnum)
{
    int             hull;
    brush_t*        b1;
    brush_t*        b2;
    brushhull_t*    bh1;
    brushhull_t*    bh2;
    int             bn;
    bool            overwrite;
    bface_t*        f;
    bface_t*        f2;
    bface_t*        next;
    bface_t*        fcopy;
    bface_t*        outside;
    bface_t*        oldoutside;
    entity_t*       e;
    vec_t           area;

    // get entity and brush info from the given brushnum that we can work with
    b1 = &g_mapbrushes[brushnum];
    e = &g_entities[b1->entitynum];

    // for each of the hulls
    for (hull = 0; hull < NUM_HULLS; hull++)
    {
        bh1 = &b1->hulls[hull];

        // set outside to a copy of the brush's faces
        outside = CopyFacesToOutside(bh1);
        overwrite = false;

        // for each brush in entity e
        for (bn = 0; bn < e->numbrushes; bn++)
        {
            // see if b2 needs to clip a chunk out of b1
            if (bn == brushnum)  
            {
                overwrite = true;                          // later brushes now overwrite
                continue;
            }

            b2 = &g_mapbrushes[e->firstbrush + bn];
            bh2 = &b2->hulls[hull];

            if (!bh2->faces)
                continue;                                  // brush isn't in this hull

            // check brush bounding box first
            // TODO: use boundingbox method instead
            if (bh1->bounds.testDisjoint(bh2->bounds))
            {
                continue;
            }

            // divide faces by the planes of the b2 to find which
            // fragments are inside

            f = outside;
            outside = NULL;
            for (; f; f = next)
            {
                next = f->next;

                // check face bounding box first
                if (bh2->bounds.testDisjoint(f->bounds))
                {                                          // this face doesn't intersect brush2's bbox
                    f->next = outside;
                    outside = f;
                    continue;
                }

                oldoutside = outside;
                fcopy = CopyFace(f);                       // save to avoid fake splits

                // throw pieces on the front sides of the planes
                // into the outside list, return the remains on the inside
                for (f2 = bh2->faces; f2 && f; f2 = f2->next)
                {
                    f = ClipFace(f, &outside, f2->planenum, overwrite);
                }

                area = f ? f->w->getArea() : 0;
                if (f && area < g_tiny_threshold)
                {
                    Verbose("Entity %i, Brush %i: tiny penetration\n", b1->entitynum, b1->brushnum);
                    c_tiny_clip++;
                    FreeFace(f);
                    f = NULL;
                }
                if (f)
                {
                    // there is one convex fragment of the original
                    // face left inside brush2
                    FreeFace(fcopy);

                    if (b1->contents > b2->contents)
                    {                                      // inside a water brush
                        f->contents = b2->contents;
                        f->next = outside;
                        outside = f;
                    }
                    else                                   // inside a solid brush
                    {
                        FreeFace(f);                       // throw it away
                    }
                }
                else
                {                                          // the entire thing was on the outside, even
                    // though the bounding boxes intersected,
                    // which will never happen with axial planes

                    // free the fragments chopped to the outside
                    while (outside != oldoutside)
                    {
                        f2 = outside->next;
                        FreeFace(outside);
                        outside = f2;
                    }

                    // revert to the original face to avoid
                    // unneeded false cuts
                    fcopy->next = outside;
                    outside = fcopy;
                }
            }

        }

        // all of the faces left in outside are real surface faces
        SaveOutside(b1, hull, outside, b1->contents);
    }
}

//
// =====================================================================================
//

// =====================================================================================
//  EmitPlanes
// =====================================================================================
static void     EmitPlanes()
{
    int             i;
    dplane_t*       dp;
    plane_t*        mp;

    g_numplanes = g_nummapplanes;
    mp = g_mapplanes;
    dp = g_dplanes;
    for (i = 0; i < g_nummapplanes; i++, mp++, dp++)
    {
        //if (!(mp->redundant))
        //{
        //    Log("EmitPlanes: plane %i non redundant\n", i);
            VectorCopy(mp->normal, dp->normal);
            dp->dist = mp->dist;
            dp->type = mp->type;
       // }
        //else
       // {
       //     Log("EmitPlanes: plane %i redundant\n", i);
       // }
    }
}

// =====================================================================================
//  SetModelNumbers
//      blah
// =====================================================================================
static void     SetModelNumbers()
{
    int             i;
    int             models;
    char            value[10];

    models = 1;
    for (i = 1; i < g_numentities; i++)
    {
        if (g_entities[i].numbrushes)
        {
            safe_snprintf(value, sizeof(value), "*%i", models);
            models++;
            SetKeyValue(&g_entities[i], "model", value);
        }
    }
}

// =====================================================================================
//  SetLightStyles
// =====================================================================================
#define	MAX_SWITCHED_LIGHTS	    32 
#define MAX_LIGHTTARGETS_NAME   64

static void     SetLightStyles()
{
    int             stylenum;
    const char*     t;
    entity_t*       e;
    int             i, j;
    char            value[10];
    char            lighttargets[MAX_SWITCHED_LIGHTS][MAX_LIGHTTARGETS_NAME];

#ifdef ZHLT_TEXLIGHT
    	bool			newtexlight = false;
#endif

    // any light that is controlled (has a targetname)
    // must have a unique style number generated for it

    stylenum = 0;
    for (i = 1; i < g_numentities; i++)
    {
        e = &g_entities[i];

        t = ValueForKey(e, "classname");
        if (strncasecmp(t, "light", 5))
        {
#ifdef ZHLT_TEXLIGHT
            //LRC:
			// if it's not a normal light entity, allocate it a new style if necessary.
	        t = ValueForKey(e, "style");
			switch (atoi(t))
			{
			case 0: // not a light, no style, generally pretty boring
				continue;
			case -1: // normal switchable texlight
				safe_snprintf(value, sizeof(value), "%i", 32 + stylenum);
				SetKeyValue(e, "style", value);
				stylenum++;
				continue;
			case -2: // backwards switchable texlight
				safe_snprintf(value, sizeof(value), "%i", -(32 + stylenum));
				SetKeyValue(e, "style", value);
				stylenum++;
				continue;
			case -3: // (HACK) a piggyback texlight: switched on and off by triggering a real light that has the same name
				SetKeyValue(e, "style", "0"); // just in case the level designer didn't give it a name
				newtexlight = true;
				// don't 'continue', fall out
			}
	        //LRC (ends)
#else
            continue;
#endif
        }
        t = ValueForKey(e, "targetname");
        if (!t[0])
        {
            continue;
        }

        // find this targetname
        for (j = 0; j < stylenum; j++)
        {
            if (!strcmp(lighttargets[j], t))
            {
                break;
            }
        }
        if (j == stylenum)
        {
            hlassume(stylenum < MAX_SWITCHED_LIGHTS, assume_MAX_SWITCHED_LIGHTS);
            safe_strncpy(lighttargets[j], t, MAX_LIGHTTARGETS_NAME);
            stylenum++;
        }
        safe_snprintf(value, sizeof(value), "%i", 32 + j);
        SetKeyValue(e, "style", value);
    }

}

// =====================================================================================
//  ConvertHintToEmtpy
// =====================================================================================
static void     ConvertHintToEmpty()
{
    int             i;

    // Convert HINT brushes to EMPTY after they have been carved by csg
    for (i = 0; i < MAX_MAP_BRUSHES; i++)
    {
        if (g_mapbrushes[i].contents == CONTENTS_HINT)
        {
            g_mapbrushes[i].contents = CONTENTS_EMPTY;
        }
    }
}

// =====================================================================================
//  WriteBSP
// =====================================================================================
void WriteBSP(const char* const name)
{
    char path[_MAX_PATH];

    safe_strncpy(path, name, _MAX_PATH);
    DefaultExtension(path, ".bsp");

    SetModelNumbers();
    SetLightStyles();

    if (!g_onlyents)
        WriteMiptex();

    UnparseEntities();
    ConvertHintToEmpty();
    WriteBSPFile(path);
}

//
// =====================================================================================
//

// AJM: added in function
// =====================================================================================
//  CopyGenerictoCLIP
//      clips a generic brush
// =====================================================================================
static void     CopyGenerictoCLIP(const brush_t* const b)
{
    // code blatently ripped from CopySKYtoCLIP()

    int             i;
    entity_t*       mapent;
    brush_t*        newbrush;

    mapent = &g_entities[b->entitynum];
    mapent->numbrushes++;

    newbrush = &g_mapbrushes[g_nummapbrushes];
    newbrush->entitynum = b->entitynum;
    newbrush->brushnum = g_nummapbrushes - mapent->firstbrush;
    newbrush->firstside = g_numbrushsides;
    newbrush->numsides = b->numsides;
    newbrush->contents = CONTENTS_CLIP;
    newbrush->noclip = 0;

    for (i = 0; i < b->numsides; i++)
    {
        int             j;

        side_t*         side = &g_brushsides[g_numbrushsides];

        *side = g_brushsides[b->firstside + i];
        safe_strncpy(side->td.name, "CLIP", sizeof(side->td.name));

        for (j = 0; j < NUM_HULLS; j++)
        {
            newbrush->hulls[j].faces = NULL;
            newbrush->hulls[j].bounds = b->hulls[j].bounds;
        }

        g_numbrushsides++;
        hlassume(g_numbrushsides < MAX_MAP_SIDES, assume_MAX_MAP_SIDES);
    }

    g_nummapbrushes++;
    hlassume(g_nummapbrushes < MAX_MAP_BRUSHES, assume_MAX_MAP_BRUSHES);
}

#ifdef HLCSG_CLIPECONOMY
// AJM: added in 
unsigned int    BrushClipHullsDiscarded = 0; 
unsigned int    ClipNodesDiscarded = 0;

//AJM: added in function
static void     MarkEntForNoclip(entity_t*  ent)
{
    int             i;
    brush_t*        b;

    for (i = ent->firstbrush; i < ent->firstbrush + ent->numbrushes; i++)
    {
        b = &g_mapbrushes[i];
        b->noclip = 1;  

        BrushClipHullsDiscarded++;
        ClipNodesDiscarded += b->numsides;
    }
}

// AJM
// =====================================================================================
//  CheckForNoClip
//      marks the noclip flag on any brushes that dont need clipnode generation, eg. func_illusionaries
// =====================================================================================
static void     CheckForNoClip()
{
    int             i;
    entity_t*       ent;

    char            entclassname[MAX_KEY]; 
    int             spawnflags;

    if (!g_bClipNazi) 
        return; // NO CLIP FOR YOU!!!

    for (i = 0; i < g_numentities; i++)
    {
        if (!g_entities[i].numbrushes) 
            continue; // not a model

        if (!i) 
            continue; // dont waste our time with worldspawn

        ent = &g_entities[i];

        strcpy_s(entclassname, ValueForKey(ent, "classname"));
        spawnflags = atoi(ValueForKey(ent, "spawnflags"));

		// condition 0, it's marked noclip (KGP)
		if(strlen(ValueForKey(ent,"zhlt_noclip")) && strcmp(ValueForKey(ent,"zhlt_noclip"),"0"))
		{ 
			MarkEntForNoclip(ent);
		}
        // condition 1, its a func_illusionary 
		else if (!strncasecmp(entclassname,      "func_illusionary", 16))  
        {
            MarkEntForNoclip(ent);
        }
        // condition 2, flag 4 (8) is set and it is either a func_door, func_train, momentary_door, 
        //  func_door_rotating or func_tracktrain (passable, not-solid flag )        
        else if (    (spawnflags & 8)
                     && 
                     (   /* NOTE: func_doors as far as i can tell may need clipnodes for their
                            player collision detection, so for now, they stay out of it. */
                          (!strncasecmp(entclassname, "func_train",         10))
                       || (!strncasecmp(entclassname, "func_door",           9)) 
                  //   || (!strncasecmp(entclassname, "momentary_door",     14))
                  //   || (!strncasecmp(entclassname, "func_door_rotating", 18))
                       || (!strncasecmp(entclassname, "func_tracktrain",    15))
                     )
                ) 
        {
            MarkEntForNoclip(ent);
        } 
        // condition 3: flag 2 (2) is set, and its a func_conveyor (not solid flag)  
        else if ( (spawnflags & 2) && (!strncasecmp(entclassname, "func_conveyor", 13)) ) 
        {
            MarkEntForNoclip(ent);
        }
        // condition 4: flag 1 (1) is set, and its a func_rot_button (not solid flag)     
        else if ( (spawnflags & 1) && (!strncasecmp(entclassname, "func_rot_button", 15)) )
        {
            MarkEntForNoclip(ent);
        }
        // condition 5: flag 7 (64) is set, and its a func_rotating                     
        else if ( (spawnflags & 64) && (!strncasecmp(entclassname, "func_rotating", 13)) )
        {
            MarkEntForNoclip(ent);
        }            
        /*
        // condition 6: its a func_wall, while we noclip it, we remake the clipnodes manually 
        else if (!strncasecmp(entclassname, "func_wall", 9)) 
        {
            for (int j = ent->firstbrush; j < ent->firstbrush + ent->numbrushes; j++)
                CopyGenerictoCLIP(&g_mapbrushes[i]);

            MarkEntForNoclip(ent);
        }
*/
    }

    Log("%i brushes (totalling %i sides) discarded from clipping hulls\n", BrushClipHullsDiscarded, ClipNodesDiscarded);
}
#endif

// =====================================================================================
//  ProcessModels
// =====================================================================================
#define NUM_TYPECONTENTS    5 // AJM: should reflect the number of values below
int typecontents[NUM_TYPECONTENTS] = { 
    CONTENTS_WATER, CONTENTS_SLIME, CONTENTS_LAVA, CONTENTS_SKY, CONTENTS_HINT 
};


static void     ProcessModels()
{
    int             i, j, type;
    int             placed;
    int             first, contents;
    brush_t         temp;

    for (i = 0; i < g_numentities; i++)
    {
        if (!g_entities[i].numbrushes) // only models
            continue;

        // sort the contents down so stone bites water, etc
        first = g_entities[i].firstbrush;
        placed = 0;
        for (type = 0; type < NUM_TYPECONTENTS; type++)                 // for each of the contents types
        {
            contents = typecontents[type];
            for (j = placed + 1; j < g_entities[i].numbrushes; j++)     // for each of the model's brushes
            {
                // if this brush is of the contents type in this for iteration
                if (g_mapbrushes[first + j].contents == contents)       
                {
                    temp = g_mapbrushes[first + placed];
                    g_mapbrushes[first + placed] = g_mapbrushes[j];
                    g_mapbrushes[j] = temp;
                    placed++;
                }
            }
        }

        // csg them in order
        if (i == 0) // if its worldspawn....
        {
            NamedRunThreadsOnIndividual(g_entities[i].numbrushes, g_estimate, CSGBrush);
            CheckFatal();
        }
        else
        {
            for (j = 0; j < g_entities[i].numbrushes; j++)
            {
                CSGBrush(first + j);
            }
        }

        // write end of model marker
        for (j = 0; j < NUM_HULLS; j++)
        {
            fprintf(out[j], "-1 -1 -1 -1\n");
        }
    }
}

// =====================================================================================
//  SetModelCenters
// =====================================================================================
static void     SetModelCenters(int entitynum)
{
    int             i;
    int             last;
    char            string[MAXTOKEN];
    entity_t*       e = &g_entities[entitynum];
    BoundingBox     bounds;
    vec3_t          center;

    if ((entitynum == 0) || (e->numbrushes == 0)) // skip worldspawn and point entities
        return;

    if (!*ValueForKey(e, "light_origin")) // skip if its not a zhlt_flags light_origin
        return;

    for (i = e->firstbrush, last = e->firstbrush + e->numbrushes; i < last; i++)
    {
        if (g_mapbrushes[i].contents != CONTENTS_ORIGIN)
        {
            bounds.add(g_mapbrushes[i].hulls->bounds);
        }
    }

    VectorAdd(bounds.m_Mins, bounds.m_Maxs, center);
    VectorScale(center, 0.5, center);

    safe_snprintf(string, MAXTOKEN, "%i %i %i", (int)center[0], (int)center[1], (int)center[2]);
    SetKeyValue(e, "model_center", string);
}

//
// =====================================================================================
//

// =====================================================================================
//  BoundWorld
// =====================================================================================
static void     BoundWorld()
{
    int             i;
    brushhull_t*    h;

    world_bounds.reset();

    for (i = 0; i < g_nummapbrushes; i++)
    {
        h = &g_mapbrushes[i].hulls[0];
        if (!h->faces)
        {
            continue;
        }
        world_bounds.add(h->bounds);
    }

    Verbose("World bounds: (%i %i %i) to (%i %i %i)\n",
            (int)world_bounds.m_Mins[0], (int)world_bounds.m_Mins[1], (int)world_bounds.m_Mins[2],
            (int)world_bounds.m_Maxs[0], (int)world_bounds.m_Maxs[1], (int)world_bounds.m_Maxs[2]);
}

// =====================================================================================
//  Usage
//      prints out usage sheet
// =====================================================================================
static void     Usage()
{
    Banner(); // TODO: Call banner from main CSG process? 

    Log("\n-= %s Options =-\n\n", g_Program);
    Log("    -nowadtextures   : include all used textures into bsp\n");
    Log("    -wadinclude file : place textures used from wad specified into bsp\n");
    Log("    -noclip          : don't create clipping hull\n");
    
#ifdef HLCSG_CLIPECONOMY    // AJM
    Log("    -noclipeconomy   : turn clipnode economy mode off\n");
#endif

#ifdef HLCSG_PRECISIONCLIP // KGP
	Log("    -cliptype value  : set to smallest, normalized, simple, precise, or legacy (default)\n");
#endif
#ifdef HLCSG_NULLIFY_INVISIBLE // KGP
	Log("    -nullfile file   : specify list of entities to retexture with NULL\n");
#endif

    Log("    -onlyents        : do an entity update from .map to .bsp\n");
    Log("    -noskyclip       : disable automatic clipping of SKY brushes\n");
    Log("    -tiny #          : minmum brush face surface area before it is discarded\n");
    Log("    -brushunion #    : threshold to warn about overlapping brushes\n\n");
    Log("    -hullfile file   : Reads in custom collision hull dimensions\n");
    Log("    -texdata #       : Alter maximum texture memory limit (in kb)\n");
    Log("    -lightdata #     : Alter maximum lighting memory limit (in kb)\n");
    Log("    -chart           : display bsp statitics\n");
    Log("    -low | -high     : run program an altered priority level\n");
    Log("    -nolog           : don't generate the compile logfiles\n");
    Log("    -threads #       : manually specify the number of threads to run\n");
#ifdef SYSTEM_WIN32
    Log("    -estimate        : display estimated time during compile\n");
#endif
#ifdef ZHLT_PROGRESSFILE // AJM
    Log("    -progressfile path  : specify the path to a file for progress estimate output\n");
#endif
#ifdef SYSTEM_POSIX
    Log("    -noestimate      : do not display continuous compile time estimates\n");
#endif
    Log("    -verbose         : compile with verbose messages\n");
    Log("    -noinfo          : Do not show tool configuration information\n");

#ifdef ZHLT_NULLTEX // AJM
    Log("    -nonulltex       : Turns off null texture stripping\n");
#endif

#ifdef ZHLT_DETAIL // AJM
    Log("    -nodetail        : dont handle detail brushes\n");
#endif

    Log("    -dev #           : compile with developer message\n\n");

#ifdef HLCSG_WADCFG // AJM
    Log("    -wadconfig name  : Specify a configuration to use from wad.cfg\n");
    Log("    -wadcfgfile path : Manually specify a path to the wad.cfg file\n"); //JK:
#endif

#ifdef HLCSG_AUTOWAD // AJM:
    Log("    -wadautodetect   : Force auto-detection of wadfiles\n");
#endif
    Log("    mapfile          : The mapfile to compile\n\n");

    exit(1);
}

// =====================================================================================
//  DumpWadinclude
//      prints out the wadinclude list
// =====================================================================================
static void     DumpWadinclude()
{
    Log("Wadinclude list :\n");
    WadInclude_i it;
    for (it = g_WadInclude.begin(); it != g_WadInclude.end(); it++)
    {
        Log("[%s]\n", it->c_str());
    }
}

// =====================================================================================
//  Settings
//      prints out settings sheet
// =====================================================================================
static void     Settings()
{
    char*           tmp;

    if (!g_info)
        return; 

    Log("\nCurrent %s Settings\n", g_Program);
    Log("Name                 |  Setting  |  Default\n"
        "---------------------|-----------|-------------------------\n");

    // ZHLT Common Settings
    if (DEFAULT_NUMTHREADS == -1)
    {
        Log("threads               [ %7d ] [  Varies ]\n", g_numthreads);
    }
    else
    {
        Log("threads               [ %7d ] [ %7d ]\n", g_numthreads, DEFAULT_NUMTHREADS);
    }

    Log("verbose               [ %7s ] [ %7s ]\n", g_verbose ? "on" : "off", DEFAULT_VERBOSE ? "on" : "off");
    Log("log                   [ %7s ] [ %7s ]\n", g_log ? "on" : "off", DEFAULT_LOG ? "on" : "off");

    Log("developer             [ %7d ] [ %7d ]\n", g_developer, DEFAULT_DEVELOPER);
    Log("chart                 [ %7s ] [ %7s ]\n", g_chart ? "on" : "off", DEFAULT_CHART ? "on" : "off");
    Log("estimate              [ %7s ] [ %7s ]\n", g_estimate ? "on" : "off", DEFAULT_ESTIMATE ? "on" : "off");
    Log("max texture memory    [ %7d ] [ %7d ]\n", g_max_map_miptex, DEFAULT_MAX_MAP_MIPTEX);
	Log("max lighting memory   [ %7d ] [ %7d ]\n", g_max_map_lightdata, DEFAULT_MAX_MAP_LIGHTDATA);

    switch (g_threadpriority)
    {
    case eThreadPriorityNormal:
    default:
        tmp = "Normal";
        break;
    case eThreadPriorityLow:
        tmp = "Low";
        break;
    case eThreadPriorityHigh:
        tmp = "High";
        break;
    }
    Log("priority              [ %7s ] [ %7s ]\n", tmp, "Normal");
    Log("\n");

    // HLCSG Specific Settings

    Log("noclip                [ %7s ] [ %7s ]\n", g_noclip          ? "on" : "off", DEFAULT_NOCLIP       ? "on" : "off");

#ifdef ZHLT_NULLTEX // AJM:
    Log("null texture stripping[ %7s ] [ %7s ]\n", g_bUseNullTex     ? "on" : "off", DEFAULT_NULLTEX      ? "on" : "off");
#endif

#ifdef ZHLT_DETAIL // AJM
    Log("detail brushes        [ %7s ] [ %7s ]\n", g_bDetailBrushes  ? "on" : "off", DEFAULT_DETAIL       ? "on" : "off");
#endif

#ifdef HLCSG_CLIPECONOMY // AJM
    Log("clipnode economy mode [ %7s ] [ %7s ]\n", g_bClipNazi       ? "on" : "off", DEFAULT_CLIPNAZI     ? "on" : "off");
#endif

#ifdef HLCSG_PRECISIONCLIP // KGP
	Log("clip hull type        [ %7s ] [ %7s ]\n", GetClipTypeString(g_cliptype), GetClipTypeString(DEFAULT_CLIPTYPE));
#endif

    Log("onlyents              [ %7s ] [ %7s ]\n", g_onlyents        ? "on" : "off", DEFAULT_ONLYENTS     ? "on" : "off");
    Log("wadtextures           [ %7s ] [ %7s ]\n", g_wadtextures     ? "on" : "off", DEFAULT_WADTEXTURES  ? "on" : "off");
    Log("skyclip               [ %7s ] [ %7s ]\n", g_skyclip         ? "on" : "off", DEFAULT_SKYCLIP      ? "on" : "off");
    Log("hullfile              [ %7s ] [ %7s ]\n", g_hullfile ? g_hullfile : "None", "None");
#ifdef HLCSG_NULLIFY_INVISIBLE // KGP
	Log("nullfile              [ %7s ] [ %7s ]\n", g_nullfile ? g_nullfile : "None", "None");
#endif
    // calc min surface area
    {
        char            tiny_penetration[10];
        char            default_tiny_penetration[10];

        safe_snprintf(tiny_penetration, sizeof(tiny_penetration), "%3.3f", g_tiny_threshold);
        safe_snprintf(default_tiny_penetration, sizeof(default_tiny_penetration), "%3.3f", DEFAULT_TINY_THRESHOLD);
        Log("min surface area      [ %7s ] [ %7s ]\n", tiny_penetration, default_tiny_penetration);
    }

    // calc union threshold
    {
        char            brush_union[10];
        char            default_brush_union[10];

        safe_snprintf(brush_union, sizeof(brush_union), "%3.3f", g_BrushUnionThreshold);
        safe_snprintf(default_brush_union, sizeof(default_brush_union), "%3.3f", DEFAULT_BRUSH_UNION_THRESHOLD);
        Log("brush union threshold [ %7s ] [ %7s ]\n", brush_union, default_brush_union);
    }

    Log("\n");
}

// AJM: added in
// =====================================================================================
//  CSGCleanup
// =====================================================================================
void            CSGCleanup()
{
    //Log("CSGCleanup\n");
#ifdef HLCSG_AUTOWAD
    autowad_cleanup();
#endif
#ifdef HLCSG_WADCFG
    WadCfg_cleanup();
#endif
#ifdef HLCSG_NULLIFY_TEXTURES
	properties_cleanup();
#endif
    FreeWadPaths();
}

// =====================================================================================
//  Main
//      Oh, come on.
// =====================================================================================
int             main(const int argc, char** argv)
{
    int             i;                          
    char            name[_MAX_PATH];            // mapanme 
    double          start, end;                 // start/end time log
    const char*     mapname_from_arg = NULL;    // mapname path from passed argvar

    g_Program = "hlcsg";

    if (argc == 1)
        Usage();

    // Hard coded list of -wadinclude files, used for HINT texture brushes so lazy
    // mapmakers wont cause beta testers (or possibly end users) to get a wad 
    // error on zhlt.wad etc
    g_WadInclude.push_back("zhlt.wad");

    memset(wadconfigname, 0, sizeof(wadconfigname));//AJM

    // detect argv
    for (i = 1; i < argc; i++)
    {
        if (!strcasecmp(argv[i], "-threads"))
        {
            if (i < argc)
            {
                g_numthreads = atoi(argv[++i]);
                if (g_numthreads < 1)
                {
                    Log("Expected value of at least 1 for '-threads'\n");
                    Usage();
                }
            }
            else
            {
                Usage();
            }
        }

#ifdef SYSTEM_WIN32
        else if (!strcasecmp(argv[i], "-estimate"))
        {
            g_estimate = true;
        }
#endif

#ifdef SYSTEM_POSIX
        else if (!strcasecmp(argv[i], "-noestimate"))
        {
            g_estimate = false;
        }
#endif

        else if (!strcasecmp(argv[i], "-dev"))
        {
            if (i < argc)
            {
                g_developer = (developer_level_t)atoi(argv[++i]);
            }
            else
            {
                Usage();
            }
        }
        else if (!strcasecmp(argv[i], "-verbose"))
        {
            g_verbose = true;
        }
        else if (!strcasecmp(argv[i], "-noinfo"))
        {
            g_info = false;
        }
        else if (!strcasecmp(argv[i], "-chart"))
        {
            g_chart = true;
        }
        else if (!strcasecmp(argv[i], "-low"))
        {
            g_threadpriority = eThreadPriorityLow;
        }
        else if (!strcasecmp(argv[i], "-high"))
        {
            g_threadpriority = eThreadPriorityHigh;
        }
        else if (!strcasecmp(argv[i], "-nolog"))
        {
            g_log = false;
        }
        else if (!strcasecmp(argv[i], "-skyclip"))
        {
            g_skyclip = true;
        }
        else if (!strcasecmp(argv[i], "-noskyclip"))
        {
            g_skyclip = false;
        }
        else if (!strcasecmp(argv[i], "-noclip"))
        {
            g_noclip = true;
        }
        else if (!strcasecmp(argv[i], "-onlyents"))
        {
            g_onlyents = true;
        }

#ifdef ZHLT_NULLTEX  // AJM: added in -nonulltex
        else if (!strcasecmp(argv[i], "-nonulltex"))
        {
            g_bUseNullTex = false;
        }
#endif

#ifdef HLCSG_CLIPECONOMY    // AJM: added in -noclipeconomy
        else if (!strcasecmp(argv[i], "-noclipeconomy"))
        {
            g_bClipNazi = false;
        }
#endif

#ifdef HLCSG_PRECISIONCLIP	// KGP: added in -cliptype
		else if (!strcasecmp(argv[i], "-cliptype"))
		{
			if (i < argc)
			{
				++i;
				if(!strcasecmp(argv[i],"smallest"))
				{ g_cliptype = clip_smallest; }
				else if(!strcasecmp(argv[i],"normalized"))
				{ g_cliptype = clip_normalized; }
				else if(!strcasecmp(argv[i],"simple"))
				{ g_cliptype = clip_simple; }
				else if(!strcasecmp(argv[i],"precise"))
				{ g_cliptype = clip_precise; }
				else if(!strcasecmp(argv[i],"legacy"))
				{ g_cliptype = clip_legacy; }
			}
            else
            {
                Log("Error: -cliptype: incorrect usage of parameter\n");
                Usage();
            }
		}
#endif

#ifdef HLCSG_WADCFG
        // AJM: added in -wadconfig
        else if (!strcasecmp(argv[i], "-wadconfig"))
        { 
            if (i < argc)
            {
                safe_strncpy(wadconfigname, argv[++i], MAX_WAD_CFG_NAME);
                if (strlen(argv[i]) > MAX_WAD_CFG_NAME)
                {
                    Warning("wad configuration name was truncated to %i chars", MAX_WAD_CFG_NAME);
                    wadconfigname[MAX_WAD_CFG_NAME] = 0;
                }
            }
            else
            {
                Log("Error: -wadconfig: incorrect usage of parameter\n");
                Usage();
            }
        }

        //JK: added in -wadcfgfile
        else if (!strcasecmp(argv[i], "-wadcfgfile"))
        {
            if (i < argc)
            {
                g_wadcfgfile = argv[++i];
            }
            else
            {
            	Log("Error: -wadcfgfile: incorrect usage of parameter\n");
                Usage();
            }
        }
#endif
#ifdef HLCSG_NULLIFY_INVISIBLE
		else if (!strcasecmp(argv[i], "-nullfile"))
		{
            if (i < argc)
            {
                g_nullfile = argv[++i];
            }
            else
            {
            	Log("Error: -nullfile: expected path to null ent file following parameter\n");
                Usage();
            }
		}
#endif

#ifdef HLCSG_AUTOWAD // AJM
        else if (!strcasecmp(argv[i], "-wadautodetect"))
        { 
            g_bWadAutoDetect = true;
        }
#endif

#ifdef ZHLT_DETAIL // AJM
        else if (!strcasecmp(argv[i], "-nodetail"))
        {
            g_bDetailBrushes = false;
        }
#endif

#ifdef ZHLT_PROGRESSFILE // AJM
        else if (!strcasecmp(argv[i], "-progressfile"))
        {
            if (i < argc)
            {
                g_progressfile = argv[++i];
            }
            else
            {
            	Log("Error: -progressfile: expected path to progress file following parameter\n");
                Usage();
            }
        }
#endif

        else if (!strcasecmp(argv[i], "-nowadtextures"))
        {
            g_wadtextures = false;
        }
        else if (!strcasecmp(argv[i], "-wadinclude"))
        {
            if (i < argc)
            {
                g_WadInclude.push_back(argv[++i]);
            }
            else
            {
                Usage();
            }
        }
        else if (!strcasecmp(argv[i], "-texdata"))
        {
            if (i < argc)
            {
                int             x = atoi(argv[++i]) * 1024;

                if (x > g_max_map_miptex)
                {
                    g_max_map_miptex = x;
                }
            }
            else
            {
                Usage();
            }
        }
        else if (!strcasecmp(argv[i], "-lightdata"))
        {
            if (i < argc)
            {
                int             x = atoi(argv[++i]) * 1024;

                if (x > g_max_map_lightdata)
                {
                    g_max_map_lightdata = x;
                }
            }
            else
            {
                Usage();
            }
        }
        else if (!strcasecmp(argv[i], "-brushunion"))
        {
            if (i < argc)
            {
                g_BrushUnionThreshold = (float)atof(argv[++i]);
            }
            else
            {
                Usage();
            }
        }
        else if (!strcasecmp(argv[i], "-tiny"))
        {
            if (i < argc)
            {
                g_tiny_threshold = (float)atof(argv[++i]);
            }
            else
            {
                Usage();
            }
        }
        else if (!strcasecmp(argv[i], "-hullfile"))
        {
            if (i < argc)
            {
                g_hullfile = argv[++i];
            }
            else
            {
                Usage();
            }
        }
        else if (argv[i][0] == '-')
        {
            Log("Unknown option \"%s\"\n", argv[i]);
            Usage();
        }
        else if (!mapname_from_arg)
        {
            mapname_from_arg = argv[i];
        }
        else
        {
            Log("Unknown option \"%s\"\n", argv[i]);
            Usage();
        }
    }

    // no mapfile?
    if (!mapname_from_arg)
    {
        // what a shame.
        Log("No mapfile specified\n");
        Usage();
    }

    // handle mapname
    safe_strncpy(g_Mapname, mapname_from_arg, _MAX_PATH);
    FlipSlashes(g_Mapname);
    StripExtension(g_Mapname);

    // onlyents
    if (!g_onlyents)
        ResetTmpFiles();

    // other stuff
    ResetErrorLog();                     
    ResetLog();                          
    OpenLog(g_clientid);                  
    atexit(CloseLog);                       
    LogStart(argc, argv);                   
    atexit(CSGCleanup); // AJM
    dtexdata_init();                        
    atexit(dtexdata_free);

    // START CSG
    // AJM: re-arranged some stuff up here so that the mapfile is loaded
    //  before settings are finalised and printed out, so that the info_compile_parameters
    //  entity can be dealt with effectively
    start = I_FloatTime();
    
    LoadHullfile(g_hullfile);               // if the user specified a hull file, load it now
#ifdef HLCSG_NULLIFY_INVISIBLE
	if(g_bUseNullTex)
	{ properties_initialize(g_nullfile); }
#endif
    safe_strncpy(name, mapname_from_arg, _MAX_PATH); // make a copy of the nap name
    DefaultExtension(name, ".map");                  // might be .reg
    
    LoadMapFile(name);
    ThreadSetDefault();                    
    ThreadSetPriority(g_threadpriority);  
    Settings();


#ifdef HLCSG_WADCFG // AJM
    // figure out what to do with the texture settings
    if (wadconfigname[0])           // custom wad configuations will take precedence
    {
        LoadWadConfigFile();
        ProcessWadConfiguration();
    }
    else
    {
        Log("Using mapfile wad configuration\n");
    }
    if (!g_bWadConfigsLoaded)  // dont try and override wad.cfg
#endif
    {
        GetUsedWads(); 
    }

#ifdef HLCSG_AUTOWAD
    if (g_bWadAutoDetect)
    {
        Log("Wadfiles not in use by the map will be excluded\n");
    }
#endif

    DumpWadinclude();
    Log("\n");

    // if onlyents, just grab the entites and resave
    if (g_onlyents)
    {
        char            out[_MAX_PATH];

        safe_snprintf(out, _MAX_PATH, "%s.bsp", g_Mapname);
        LoadBSPFile(out);
        LoadWadincludeFile(g_Mapname);

        HandleWadinclude();

        // Write it all back out again.
        if (g_chart)
        {
            PrintBSPFileSizes();
        }
        WriteBSP(g_Mapname);

        end = I_FloatTime();
        LogTimeElapsed(end - start);
        return 0;
    }
    else
    {
        SaveWadincludeFile(g_Mapname);
    }

#ifdef HLCSG_CLIPECONOMY // AJM
    CheckForNoClip(); 
#endif

    // createbrush
    NamedRunThreadsOnIndividual(g_nummapbrushes, g_estimate, CreateBrush);
    CheckFatal();

#ifdef HLCSG_PRECISIONCLIP // KGP - drop TEX_BEVEL flag
	FixBevelTextures();
#endif

    // boundworld
    BoundWorld();

    Verbose("%5i map planes\n", g_nummapplanes);

    // Set model centers
    NamedRunThreadsOnIndividual(g_numentities, g_estimate, SetModelCenters);

    // Calc brush unions
    if ((g_BrushUnionThreshold > 0.0) && (g_BrushUnionThreshold <= 100.0))
    {
        NamedRunThreadsOnIndividual(g_nummapbrushes, g_estimate, CalculateBrushUnions);
    }

    // open hull files
    for (i = 0; i < NUM_HULLS; i++)
    {
        char            name[_MAX_PATH];

        safe_snprintf(name, _MAX_PATH, "%s.p%i", g_Mapname, i);

        out[i] = fopen(name, "w");

        if (!out[i]) 
            Error("Couldn't open %s", name);
    }

    ProcessModels();

    Verbose("%5i csg faces\n", c_csgfaces);
    Verbose("%5i used faces\n", c_outfaces);
    Verbose("%5i tiny faces\n", c_tiny);
    Verbose("%5i tiny clips\n", c_tiny_clip);

    // close hull files 
    for (i = 0; i < NUM_HULLS; i++)
        fclose(out[i]);

    EmitPlanes();

    if (g_chart)
        PrintBSPFileSizes();

    WriteBSP(g_Mapname);

    // AJM: debug
#if 0
    Log("\n---------------------------------------\n"
        "Map Plane Usage:\n"
        "  #  normal             origin             dist   type\n"
        "    (   x,    y,    z) (   x,    y,    z) (     )\n"
        );
    for (i = 0; i < g_nummapplanes; i++)
    {
        plane_t* p = &g_mapplanes[i];

        Log(
        "%3i (%4.0f, %4.0f, %4.0f) (%4.0f, %4.0f, %4.0f) (%5.0f) %i\n",
        i,     
        p->normal[1], p->normal[2], p->normal[3],
        p->origin[1], p->origin[2], p->origin[3],
        p->dist,
        p->type
        );
    }
    Log("---------------------------------------\n\n");
#endif

    // elapsed time
    end = I_FloatTime();
    LogTimeElapsed(end - start);

    return 0;
}
