/*

    BINARY SPACE PARTITION    -aka-    B S P

    Code based on original code from Valve Software,
    Modified by Sean "Zoner" Cavanaugh (seanc@gearboxsoftware.com) with permission.
    Modified by Tony "Merl" Moore (merlinis@bigpond.net.au) [AJM]

*/

#ifdef SYSTEM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include "bsp5.h"

/*

 NOTES


*/

static FILE*    polyfiles[NUM_HULLS];
int             g_hullnum = 0;

static face_t*  validfaces[MAX_INTERNAL_MAP_PLANES];

char            g_bspfilename[_MAX_PATH];
char            g_pointfilename[_MAX_PATH];
char            g_linefilename[_MAX_PATH];
char            g_portfilename[_MAX_PATH];

// command line flags
bool			g_noopt = DEFAULT_NOOPT;		// don't optimize BSP on write
bool            g_nofill = DEFAULT_NOFILL;      // dont fill "-nofill"
bool            g_notjunc = DEFAULT_NOTJUNC;
bool            g_noclip = DEFAULT_NOCLIP;      // no clipping hull "-noclip"
bool            g_chart = DEFAULT_CHART;        // print out chart? "-chart"
bool            g_estimate = DEFAULT_ESTIMATE;  // estimate mode "-estimate"
bool            g_info = DEFAULT_INFO;
bool            g_bLeakOnly = DEFAULT_LEAKONLY; // leakonly mode "-leakonly"
bool            g_bLeaked = false;
int             g_subdivide_size = DEFAULT_SUBDIVIDE_SIZE;

#ifdef ZHLT_NULLTEX // AJM
bool            g_bUseNullTex = DEFAULT_NULLTEX; // "-nonulltex"
#endif

#ifdef ZHLT_DETAIL // AJM
bool            g_bDetailBrushes = DEFAULT_DETAIL; // "-nodetail"
#endif

#ifdef ZHLT_PROGRESSFILE // AJM
char*           g_progressfile = DEFAULT_PROGRESSFILE; // "-progressfile path"
#endif

#ifdef ZHLT_INFO_COMPILE_PARAMETERS// AJM
// =====================================================================================
//  GetParamsFromEnt
//      this function is called from parseentity when it encounters the
//      info_compile_parameters entity. each tool should have its own version of this
//      to handle its own specific settings.
// =====================================================================================
void            GetParamsFromEnt(entity_t* mapent)
{
    int iTmp;

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

    /*
    hlbsp(choices) : "HLBSP" : 0 =
    [
       0 : "Off"
       1 : "Normal"
       2 : "Leakonly"
    ]
    */
    iTmp = IntForKey(mapent, "hlbsp");
    if (iTmp == 0)
    {
        Fatal(assume_TOOL_CANCEL,
            "%s flag was not checked in info_compile_parameters entity, execution of %s cancelled", g_Program, g_Program);
        CheckFatal();
    }
    else if (iTmp == 1)
    {
        g_bLeakOnly = false;
    }
    else if (iTmp == 2)
    {
        g_bLeakOnly = true;
    }
    Log("%30s [ %-9s ]\n", "Leakonly Mode", g_bLeakOnly ? "on" : "off");

	iTmp = IntForKey(mapent, "noopt");
	if(iTmp == 0)
	{
		g_noopt = false;
	}
	else
	{
		g_noopt = true;
	}

    /*
    nocliphull(choices) : "Generate clipping hulls" : 0 =
    [
        0 : "Yes"
        1 : "No"
    ]
    */
    iTmp = IntForKey(mapent, "nocliphull");
    if (iTmp == 0)
    {
        g_noclip = false;
    }
    else if (iTmp == 1)
    {
        g_noclip = true;
    }
    Log("%30s [ %-9s ]\n", "Clipping Hull Generation", g_noclip ? "off" : "on");

    //////////////////
    Verbose("\n");
}
#endif

// =====================================================================================
//  Extract File stuff (ExtractFile | ExtractFilePath | ExtractFileBase)
//
// With VS 2005 - and the 64 bit build, i had to pull 3 classes over from
// cmdlib.cpp even with the proper includes to get rid of the lnk2001 error
//
// amckern - amckern@yahoo.com
// =====================================================================================

#define PATHSEPARATOR(c) ((c) == '\\' || (c) == '/')

void            ExtractFileBase(const char* const path, char* dest)
{
    hlassert (path != dest);

    const char*           src;

    src = path + strlen(path) - 1;

    //
    // back up until a \ or the start
    //
    while (src != path && !PATHSEPARATOR(*(src - 1)))
        src--;

    while (*src && *src != '.')
    {
        *dest++ = *src++;
    }
    *dest = 0;
}

void            ExtractFilePath(const char* const path, char* dest)
{
    hlassert (path != dest);

    const char*           src;

    src = path + strlen(path) - 1;

    //
    // back up until a \ or the start
    //
    while (src != path && !PATHSEPARATOR(*(src - 1)))
        src--;

    memcpy(dest, path, src - path);
    dest[src - path] = 0;
}

void            ExtractFile(const char* const path, char* dest)
{
    hlassert (path != dest);

    const char*           src;

    src = path + strlen(path) - 1;

    while (src != path && !PATHSEPARATOR(*(src - 1)))
        src--;

    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = 0;
}

// =====================================================================================
//  NewFaceFromFace
//      Duplicates the non point information of a face, used by SplitFace and MergeFace.
// =====================================================================================
face_t*         NewFaceFromFace(const face_t* const in)
{
    face_t*         newf;

    newf = AllocFace();

    newf->planenum = in->planenum;
    newf->texturenum = in->texturenum;
    newf->original = in->original;
    newf->contents = in->contents;

    return newf;
}

// =====================================================================================
//  SplitFaceTmp
//      blah
// =====================================================================================
static void     SplitFaceTmp(face_t* in, const dplane_t* const split, face_t** front, face_t** back)
{
    vec_t           dists[MAXEDGES + 1];
    int             sides[MAXEDGES + 1];
    int             counts[3];
    vec_t           dot;
    int             i;
    int             j;
    face_t*         newf;
    face_t*         new2;
    vec_t*          p1;
    vec_t*          p2;
    vec3_t          mid;

    if (in->numpoints < 0)
    {
        Error("SplitFace: freed face");
    }
    counts[0] = counts[1] = counts[2] = 0;

    // determine sides for each point
    for (i = 0; i < in->numpoints; i++)
    {
        dot = DotProduct(in->pts[i], split->normal);
        dot -= split->dist;
        dists[i] = dot;
        if (dot > ON_EPSILON)
        {
            sides[i] = SIDE_FRONT;
        }
        else if (dot < -ON_EPSILON)
        {
            sides[i] = SIDE_BACK;
        }
        else
        {
            sides[i] = SIDE_ON;
        }
        counts[sides[i]]++;
    }
    sides[i] = sides[0];
    dists[i] = dists[0];

    if (!counts[0])
    {
        *front = NULL;
        *back = in;
        return;
    }
    if (!counts[1])
    {
        *front = in;
        *back = NULL;
        return;
    }

    *back = newf = NewFaceFromFace(in);
    *front = new2 = NewFaceFromFace(in);

    // distribute the points and generate splits

    for (i = 0; i < in->numpoints; i++)
    {
        if (newf->numpoints > MAXEDGES || new2->numpoints > MAXEDGES)
        {
            Error("SplitFace: numpoints > MAXEDGES");
        }

        p1 = in->pts[i];

        if (sides[i] == SIDE_ON)
        {
            VectorCopy(p1, newf->pts[newf->numpoints]);
            newf->numpoints++;
            VectorCopy(p1, new2->pts[new2->numpoints]);
            new2->numpoints++;
            continue;
        }

        if (sides[i] == SIDE_FRONT)
        {
            VectorCopy(p1, new2->pts[new2->numpoints]);
            new2->numpoints++;
        }
        else
        {
            VectorCopy(p1, newf->pts[newf->numpoints]);
            newf->numpoints++;
        }

        if (sides[i + 1] == SIDE_ON || sides[i + 1] == sides[i])
        {
            continue;
        }

        // generate a split point
        p2 = in->pts[(i + 1) % in->numpoints];

        dot = dists[i] / (dists[i] - dists[i + 1]);
        for (j = 0; j < 3; j++)
        {                                                  // avoid round off error when possible
            if (split->normal[j] == 1)
            {
                mid[j] = split->dist;
            }
            else if (split->normal[j] == -1)
            {
                mid[j] = -split->dist;
            }
            else
            {
                mid[j] = p1[j] + dot * (p2[j] - p1[j]);
            }
        }

        VectorCopy(mid, newf->pts[newf->numpoints]);
        newf->numpoints++;
        VectorCopy(mid, new2->pts[new2->numpoints]);
        new2->numpoints++;
    }

    if (newf->numpoints > MAXEDGES || new2->numpoints > MAXEDGES)
    {
        Error("SplitFace: numpoints > MAXEDGES");
    }
}

// =====================================================================================
//  SplitFace
//      blah
// =====================================================================================
void            SplitFace(face_t* in, const dplane_t* const split, face_t** front, face_t** back)
{
    SplitFaceTmp(in, split, front, back);

    // free the original face now that is is represented by the fragments
    if (*front && *back)
    {
        FreeFace(in);
    }
}

// =====================================================================================
//  AllocFace
// =====================================================================================
face_t*         AllocFace()
{
    face_t*         f;

    f = (face_t*)malloc(sizeof(face_t));
    memset(f, 0, sizeof(face_t));

    f->planenum = -1;

    return f;
}

// =====================================================================================
//  FreeFace
// =====================================================================================
void            FreeFace(face_t* f)
{
    free(f);
}

// =====================================================================================
//  AllocSurface
// =====================================================================================
surface_t*      AllocSurface()
{
    surface_t*      s;

    s = (surface_t*)malloc(sizeof(surface_t));
    memset(s, 0, sizeof(surface_t));

    return s;
}

// =====================================================================================
//  FreeSurface
// =====================================================================================
void            FreeSurface(surface_t* s)
{
    free(s);
}

// =====================================================================================
//  AllocPortal
// =====================================================================================
portal_t*       AllocPortal()
{
    portal_t*       p;

    p = (portal_t*)malloc(sizeof(portal_t));
    memset(p, 0, sizeof(portal_t));

    return p;
}

// =====================================================================================
//  FreePortal
// =====================================================================================
void            FreePortal(portal_t* p) // consider: inline
{
    free(p);
}


// =====================================================================================
//  AllocNode
//      blah
// =====================================================================================
node_t*         AllocNode()
{
    node_t*         n;

    n = (node_t*)malloc(sizeof(node_t));
    memset(n, 0, sizeof(node_t));

    return n;
}

// =====================================================================================
//  AddPointToBounds
// =====================================================================================
void            AddPointToBounds(const vec3_t v, vec3_t mins, vec3_t maxs)
{
    int             i;
    vec_t           val;

    for (i = 0; i < 3; i++)
    {
        val = v[i];
        if (val < mins[i])
        {
            mins[i] = val;
        }
        if (val > maxs[i])
        {
            maxs[i] = val;
        }
    }
}

// =====================================================================================
//  AddFaceToBounds
// =====================================================================================
static void     AddFaceToBounds(const face_t* const f, vec3_t mins, vec3_t maxs)
{
    int             i;

    for (i = 0; i < f->numpoints; i++)
    {
        AddPointToBounds(f->pts[i], mins, maxs);
    }
}

// =====================================================================================
//  ClearBounds
// =====================================================================================
static void     ClearBounds(vec3_t mins, vec3_t maxs)
{
    mins[0] = mins[1] = mins[2] = 99999;
    maxs[0] = maxs[1] = maxs[2] = -99999;
}

// =====================================================================================
//  SurflistFromValidFaces
//      blah
// =====================================================================================
static surfchain_t* SurflistFromValidFaces()
{
    surface_t*      n;
    int             i;
    face_t*         f;
    face_t*         next;
    surfchain_t*    sc;

    sc = (surfchain_t*)malloc(sizeof(*sc));
    ClearBounds(sc->mins, sc->maxs);
    sc->surfaces = NULL;

    // grab planes from both sides
    for (i = 0; i < g_numplanes; i += 2)
    {
        if (!validfaces[i] && !validfaces[i + 1])
        {
            continue;
        }
        n = AllocSurface();
        n->next = sc->surfaces;
        sc->surfaces = n;
        ClearBounds(n->mins, n->maxs);
        n->planenum = i;

        n->faces = NULL;
        for (f = validfaces[i]; f; f = next)
        {
            next = f->next;
            f->next = n->faces;
            n->faces = f;
            AddFaceToBounds(f, n->mins, n->maxs);
        }
        for (f = validfaces[i + 1]; f; f = next)
        {
            next = f->next;
            f->next = n->faces;
            n->faces = f;
            AddFaceToBounds(f, n->mins, n->maxs);
        }

        AddPointToBounds(n->mins, sc->mins, sc->maxs);
        AddPointToBounds(n->maxs, sc->mins, sc->maxs);

        validfaces[i] = NULL;
        validfaces[i + 1] = NULL;
    }

    // merge all possible polygons

    MergeAll(sc->surfaces);

    return sc;
}

#ifdef ZHLT_NULLTEX// AJM
// =====================================================================================
//  CheckFaceForNull
//      Returns true if the passed face is facetype null
// =====================================================================================
bool            CheckFaceForNull(const face_t* const f)
{
    // null faces are only of facetype face_null if we are using null texture stripping
    if (g_bUseNullTex)
    {
        texinfo_t*      info;
        miptex_t*       miptex;
        int             ofs;

        info = &g_texinfo[f->texturenum];
        ofs = ((dmiptexlump_t*)g_dtexdata)->dataofs[info->miptex];
        miptex = (miptex_t*)(&g_dtexdata[ofs]);

        if (!strcasecmp(miptex->name, "null"))
            return true;
        else
            return false;
    }
    else // otherwise, under normal cases, null textured faces should be facetype face_normal
    {
        return false;
    }
}
// =====================================================================================
//Cpt_Andrew - UTSky Check
// =====================================================================================
bool            CheckFaceForEnv_Sky(const face_t* const f)
{
        texinfo_t*      info;
        miptex_t*       miptex;
        int             ofs;

        info = &g_texinfo[f->texturenum];
        ofs = ((dmiptexlump_t*)g_dtexdata)->dataofs[info->miptex];
        miptex = (miptex_t*)(&g_dtexdata[ofs]);

        if (!strcasecmp(miptex->name, "env_sky"))
            return true;
        else
            return false;
}
// =====================================================================================




#endif

#ifdef ZHLT_DETAIL
// =====================================================================================
//  CheckFaceForDetail
//      Returns true if the passed face is part of a detail brush
// =====================================================================================
bool            CheckFaceForDetail(const face_t* const f)
{
    if (f->contents == CONTENTS_DETAIL)
    {
        //Log("CheckFaceForDetail:: got a detail face");
        return true;
    }

    return false;
}
#endif

// =====================================================================================
//  CheckFaceForHint
//      Returns true if the passed face is facetype hint
// =====================================================================================
bool            CheckFaceForHint(const face_t* const f)
{
    texinfo_t*      info;
    miptex_t*       miptex;
    int             ofs;

    info = &g_texinfo[f->texturenum];
    ofs = ((dmiptexlump_t *)g_dtexdata)->dataofs[info->miptex];
    miptex = (miptex_t *)(&g_dtexdata[ofs]);

    if (!strcasecmp(miptex->name, "hint"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// =====================================================================================
//  CheckFaceForSkipt
//      Returns true if the passed face is facetype skip
// =====================================================================================
bool            CheckFaceForSkip(const face_t* const f)
{
    texinfo_t*      info;
    miptex_t*       miptex;
    int             ofs;

    info = &g_texinfo[f->texturenum];
    ofs = ((dmiptexlump_t*)g_dtexdata)->dataofs[info->miptex];
    miptex = (miptex_t*)(&g_dtexdata[ofs]);

    if (!strcasecmp(miptex->name, "skip"))
    {
        return true;
    }
    else
    {
        return false;
    }
}

// =====================================================================================
//  SetFaceType
// =====================================================================================
static          facestyle_e SetFaceType(face_t* f)
{
    if (CheckFaceForHint(f))
    {
        f->facestyle = face_hint;
    }
    else if (CheckFaceForSkip(f))
    {
        f->facestyle = face_skip;
    }
#ifdef ZHLT_NULLTEX         // AJM
    else if (CheckFaceForNull(f))
    {
        f->facestyle = face_null;
    }
#endif

// =====================================================================================
//Cpt_Andrew - Env_Sky Check
// =====================================================================================
   //else if (CheckFaceForUTSky(f))
	else if (CheckFaceForEnv_Sky(f))
    {
        f->facestyle = face_null;
    }
// =====================================================================================


#ifdef ZHLT_DETAIL
    else if (CheckFaceForDetail(f))
    {
        //Log("SetFaceType::detail face\n");
        f->facestyle = face_detail;
    }
#endif
    else
    {
        f->facestyle = face_normal;
    }
    return f->facestyle;
}

// =====================================================================================
//  ReadSurfs
// =====================================================================================
static surfchain_t* ReadSurfs(FILE* file)
{
    int             r;
    int             planenum, g_texinfo, contents, numpoints;
    face_t*         f;
    int             i;
    double          v[3];
    int             line = 0;

    // read in the polygons
    while (1)
    {
        line++;
        r = fscanf(file, "%i %i %i %i\n", &planenum, &g_texinfo, &contents, &numpoints);
        if (r == 0 || r == -1)
        {
            return NULL;
        }
        if (planenum == -1)                                // end of model
        {
            break;
        }
        if (r != 4)
        {
            Error("ReadSurfs (line %i): scanf failure", line);
        }
        if (numpoints > MAXPOINTS)
        {
            Error("ReadSurfs (line %i): %i > MAXPOINTS\nThis is caused by a face with too many verticies (typically found on end-caps of high-poly cylinders)\n", line, numpoints);
        }
        if (planenum > g_numplanes)
        {
            Error("ReadSurfs (line %i): %i > g_numplanes\n", line, planenum);
        }
        if (g_texinfo > g_numtexinfo)
        {
            Error("ReadSurfs (line %i): %i > g_numtexinfo", line, g_texinfo);
        }

        if (!strcasecmp(GetTextureByNumber(g_texinfo), "skip"))
        {
            Verbose("ReadSurfs (line %i): skipping a surface", line);

            for (i = 0; i < numpoints; i++)
            {
                line++;
                //Verbose("skipping line %d", line);
                r = fscanf(file, "%lf %lf %lf\n", &v[0], &v[1], &v[2]);
                if (r != 3)
                {
                    Error("::ReadSurfs (face_skip), fscanf of points failed at line %i", line);
                }
            }
            fscanf(file, "\n");
            continue;
        }

        f = AllocFace();
        f->planenum = planenum;
        f->texturenum = g_texinfo;
        f->contents = contents;
        f->numpoints = numpoints;
        f->next = validfaces[planenum];
        validfaces[planenum] = f;

        SetFaceType(f);

        for (i = 0; i < f->numpoints; i++)
        {
            line++;
            r = fscanf(file, "%lf %lf %lf\n", &v[0], &v[1], &v[2]);
            if (r != 3)
            {
                Error("::ReadSurfs (face_normal), fscanf of points failed at line %i", line);
            }
            VectorCopy(v, f->pts[i]);
        }
        fscanf(file, "\n");
    }

    return SurflistFromValidFaces();
}


#ifdef HLBSP_THREADS// AJM
// =====================================================================================
//  ProcessModelThreaded
// time to compl
// =====================================================================================
void            ProcessModel(int modelnum)
{
    surfchain_t*    surfs;
    node_t*         nodes;
    dmodel_t*       model;
    int             startleafs;

    surfs = ReadSurfs(polyfiles[0]);

    if (!surfs)
        return; // all models are done

    hlassume(g_nummodels < MAX_MAP_MODELS, assume_MAX_MAP_MODELS);

    startleafs = g_numleafs;
    int modnum = g_nummodels;
    model = &g_dmodels[modnum];
    g_nummodels++;

//    Log("ProcessModel: %i (%i f)\n", modnum, model->numfaces);

    VectorCopy(surfs->mins, model->mins);
    VectorCopy(surfs->maxs, model->maxs);

    // SolidBSP generates a node tree
    nodes = SolidBSP(surfs);

    // build all the portals in the bsp tree
    // some portals are solid polygons, and some are paths to other leafs
    if (g_nummodels == 1 && !g_nofill)                       // assume non-world bmodels are simple
    {
        nodes = FillOutside(nodes, (g_bLeaked != true), 0);                  // make a leakfile if bad
    }

    FreePortals(nodes);

    // fix tjunctions
    tjunc(nodes);

    MakeFaceEdges();

    // emit the faces for the bsp file
    model->headnode[0] = g_numnodes;
    model->firstface = g_numfaces;
    WriteDrawNodes(nodes);
    model->numfaces = g_numfaces - model->firstface;;
    model->visleafs = g_numleafs - startleafs;

    if (g_noclip)
    {
        return true;
    }

    // the clipping hulls are simpler
    for (g_hullnum = 1; g_hullnum < NUM_HULLS; g_hullnum++)
    {
        surfs = ReadSurfs(polyfiles[g_hullnum]);
        nodes = SolidBSP(surfs);
        if (g_nummodels == 1 && !g_nofill)                   // assume non-world bmodels are simple
        {
            nodes = FillOutside(nodes, (g_bLeaked != true), g_hullnum);
        }
        FreePortals(nodes);
        model->headnode[g_hullnum] = g_numclipnodes;
        WriteClipNodes(nodes);
    }

    return true;
}

#else
// =====================================================================================
//  ProcessModel
// =====================================================================================
static bool     ProcessModel()
{
    surfchain_t*    surfs;
    node_t*         nodes;
    dmodel_t*       model;
    int             startleafs;

    surfs = ReadSurfs(polyfiles[0]);

    if (!surfs)
        return false;                                      // all models are done

    hlassume(g_nummodels < MAX_MAP_MODELS, assume_MAX_MAP_MODELS);

    startleafs = g_numleafs;
    int modnum = g_nummodels;
    model = &g_dmodels[modnum];
    g_nummodels++;

//    Log("ProcessModel: %i (%i f)\n", modnum, model->numfaces);

    VectorCopy(surfs->mins, model->mins);
    VectorCopy(surfs->maxs, model->maxs);

    // SolidBSP generates a node tree
    nodes = SolidBSP(surfs,modnum==0);

    // build all the portals in the bsp tree
    // some portals are solid polygons, and some are paths to other leafs
    if (g_nummodels == 1 && !g_nofill)                       // assume non-world bmodels are simple
    {
        nodes = FillOutside(nodes, (g_bLeaked != true), 0);                  // make a leakfile if bad
    }

    FreePortals(nodes);

    // fix tjunctions
    tjunc(nodes);

    MakeFaceEdges();

    // emit the faces for the bsp file
    model->headnode[0] = g_numnodes;
    model->firstface = g_numfaces;
    WriteDrawNodes(nodes);
    model->numfaces = g_numfaces - model->firstface;;
    model->visleafs = g_numleafs - startleafs;

    if (g_noclip)
    {
		/*
			KGP 12/31/03 - store empty content type in headnode pointers to signify
			lack of clipping information in a way that doesn't crash the half-life
			engine at runtime.
		*/
		model->headnode[1] = CONTENTS_EMPTY;
		model->headnode[2] = CONTENTS_EMPTY;
		model->headnode[3] = CONTENTS_EMPTY;
        return true;
    }

    // the clipping hulls are simpler
    for (g_hullnum = 1; g_hullnum < NUM_HULLS; g_hullnum++)
    {
        surfs = ReadSurfs(polyfiles[g_hullnum]);
        nodes = SolidBSP(surfs,modnum==0);
        if (g_nummodels == 1 && !g_nofill)                   // assume non-world bmodels are simple
        {
            nodes = FillOutside(nodes, (g_bLeaked != true), g_hullnum);
        }
        FreePortals(nodes);
		/*
			KGP 12/31/03 - need to test that the head clip node isn't empty; if it is
			we need to set model->headnode equal to the content type of the head, or create
			a trivial single-node case where the content type is the same for both leaves
			if setting the content type is invalid.
		*/
		if(nodes->planenum == -1) //empty!
		{
			model->headnode[g_hullnum] = nodes->contents;
		}
		else
		{
	        model->headnode[g_hullnum] = g_numclipnodes;
		    WriteClipNodes(nodes);
		}
    }

    return true;
}
#endif

// =====================================================================================
//  Usage
// =====================================================================================
static void     Usage()
{
    Banner();

    Log("\n-= %s Options =-\n\n", g_Program);
    Log("    -leakonly      : Run BSP only enough to check for LEAKs\n");
    Log("    -subdivide #   : Sets the face subdivide size\n");
    Log("    -maxnodesize # : Sets the maximum portal node size\n\n");
    Log("    -notjunc       : Don't break edges on t-junctions     (not for final runs)\n");
    Log("    -noclip        : Don't process the clipping hull      (not for final runs)\n");
    Log("    -nofill        : Don't fill outside (will mask LEAKs) (not for final runs)\n");
	Log("	 -noopt         : Don't optimize planes on BSP write   (not for final runs)\n\n");
    Log("    -texdata #     : Alter maximum texture memory limit (in kb)\n");
    Log("    -lightdata #   : Alter maximum lighting memory limit (in kb)\n");
    Log("    -chart         : display bsp statitics\n");
    Log("    -low | -high   : run program an altered priority level\n");
    Log("    -nolog         : don't generate the compile logfiles\n");
    Log("    -threads #     : manually specify the number of threads to run\n");
#ifdef SYSTEM_WIN32
    Log("    -estimate      : display estimated time during compile\n");
#endif
#ifdef ZHLT_PROGRESSFILE // AJM
    Log("    -progressfile path  : specify the path to a file for progress estimate output\n");
#endif
#ifdef SYSTEM_POSIX
    Log("    -noestimate    : do not display continuous compile time estimates\n");
#endif

#ifdef ZHLT_NULLTEX         // AJM
    Log("    -nonulltex     : Don't strip NULL faces\n");
#endif

#ifdef ZHLT_DETAIL // AJM
    Log("    -nodetail      : don't handle detail brushes\n");
#endif

    Log("    -verbose       : compile with verbose messages\n");
    Log("    -noinfo        : Do not show tool configuration information\n");
    Log("    -dev #         : compile with developer message\n\n");
    Log("    mapfile        : The mapfile to compile\n\n");

    exit(1);
}

// =====================================================================================
//  Settings
// =====================================================================================
static void     Settings()
{
    char*           tmp;

    if (!g_info)
        return;

    Log("\nCurrent %s Settings\n", g_Program);
    Log("Name               |  Setting  |  Default\n" "-------------------|-----------|-------------------------\n");

    // ZHLT Common Settings
    if (DEFAULT_NUMTHREADS == -1)
    {
        Log("threads             [ %7d ] [  Varies ]\n", g_numthreads);
    }
    else
    {
        Log("threads             [ %7d ] [ %7d ]\n", g_numthreads, DEFAULT_NUMTHREADS);
    }

    Log("verbose             [ %7s ] [ %7s ]\n", g_verbose ? "on" : "off", DEFAULT_VERBOSE ? "on" : "off");
    Log("log                 [ %7s ] [ %7s ]\n", g_log ? "on" : "off", DEFAULT_LOG ? "on" : "off");
    Log("developer           [ %7d ] [ %7d ]\n", g_developer, DEFAULT_DEVELOPER);
    Log("chart               [ %7s ] [ %7s ]\n", g_chart ? "on" : "off", DEFAULT_CHART ? "on" : "off");
    Log("estimate            [ %7s ] [ %7s ]\n", g_estimate ? "on" : "off", DEFAULT_ESTIMATE ? "on" : "off");
    Log("max texture memory  [ %7d ] [ %7d ]\n", g_max_map_miptex, DEFAULT_MAX_MAP_MIPTEX);

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
    Log("priority            [ %7s ] [ %7s ]\n", tmp, "Normal");
    Log("\n");

    // HLBSP Specific Settings
    Log("noclip              [ %7s ] [ %7s ]\n", g_noclip ? "on" : "off", DEFAULT_NOCLIP ? "on" : "off");
    Log("nofill              [ %7s ] [ %7s ]\n", g_nofill ? "on" : "off", DEFAULT_NOFILL ? "on" : "off");
	Log("noopt               [ %7s ] [ %7s ]\n", g_noopt ? "on" : "off", DEFAULT_NOOPT ? "on" : "off");
#ifdef ZHLT_NULLTEX // AJM
    Log("null tex. stripping [ %7s ] [ %7s ]\n", g_bUseNullTex ? "on" : "off", DEFAULT_NULLTEX ? "on" : "off" );
#endif
#ifdef ZHLT_DETAIL // AJM
    Log("detail brushes      [ %7s ] [ %7s ]\n", g_bDetailBrushes ? "on" : "off", DEFAULT_DETAIL ? "on" : "off" );
#endif
    Log("notjunc             [ %7s ] [ %7s ]\n", g_notjunc ? "on" : "off", DEFAULT_NOTJUNC ? "on" : "off");
    Log("subdivide size      [ %7d ] [ %7d ] (Min %d) (Max %d)\n",
        g_subdivide_size, DEFAULT_SUBDIVIDE_SIZE, MIN_SUBDIVIDE_SIZE, MAX_SUBDIVIDE_SIZE);
    Log("max node size       [ %7d ] [ %7d ] (Min %d) (Max %d)\n",
        g_maxnode_size, DEFAULT_MAXNODE_SIZE, MIN_MAXNODE_SIZE, MAX_MAXNODE_SIZE);

    Log("\n\n");
}

// =====================================================================================
//  ProcessFile
// =====================================================================================
static void     ProcessFile(const char* const filename)
{
    int             i;
    char            name[_MAX_PATH];

    // delete existing files
    safe_snprintf(g_portfilename, _MAX_PATH, "%s.prt", filename);
    unlink(g_portfilename);

    safe_snprintf(g_pointfilename, _MAX_PATH, "%s.pts", filename);
    unlink(g_pointfilename);

    safe_snprintf(g_linefilename, _MAX_PATH, "%s.lin", filename);
    unlink(g_linefilename);

    // open the hull files
    for (i = 0; i < NUM_HULLS; i++)
    {
                   //mapname.p[0-3]
        sprintf(name, "%s.p%i", filename, i);
        polyfiles[i] = fopen(name, "r");

        if (!polyfiles[i])
            Error("Can't open %s", name);
    }

    // load the output of csg
    safe_snprintf(g_bspfilename, _MAX_PATH, "%s.bsp", filename);
    LoadBSPFile(g_bspfilename);
    ParseEntities();

    Settings(); // AJM: moved here due to info_compile_parameters entity

    // init the tables to be shared by all models
    BeginBSPFile();

#ifdef HLBSP_THREADS // AJM
    NamedRunThreadsOnIndividual(nummodels, g_estimate, ProcessModel);
#else
    // process each model individually
    while (ProcessModel())
        ;
#endif

    // write the updated bsp file out
    FinishBSPFile();
}

// =====================================================================================
//  main
// =====================================================================================
int             main(const int argc, char** argv)
{
    int             i;
    double          start, end;
    const char*     mapname_from_arg = NULL;

    g_Program = "hlbsp";

    // if we dont have any command line argvars, print out usage and die
    if (argc == 1)
        Usage();

    // check command line args
    for (i = 1; i < argc; i++)
    {
        if (!strcasecmp(argv[i], "-threads"))
        {
            if (i < argc)
            {
                int             g_numthreads = atoi(argv[++i]);

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
        else if (!strcasecmp(argv[i], "-notjunc"))
        {
            g_notjunc = true;
        }
        else if (!strcasecmp(argv[i], "-noclip"))
        {
            g_noclip = true;
        }
        else if (!strcasecmp(argv[i], "-nofill"))
        {
            g_nofill = true;
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

#ifdef ZHLT_NETVIS
        else if (!strcasecmp(argv[i], "-client"))
        {
            if (i < argc)
            {
                g_clientid = atoi(argv[++i]);
            }
            else
            {
                Usage();
            }
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
        else if (!strcasecmp(argv[i], "-leakonly"))
        {
            g_bLeakOnly = true;
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

#ifdef ZHLT_NULLTEX // AJM
        else if (!strcasecmp(argv[i], "-nonulltex"))
        {
            g_bUseNullTex = false;
        }
#endif

#ifdef ZHLT_DETAIL // AJM
        else if (!strcasecmp(argv[i], "-nodetail"))
        {
            g_bDetailBrushes = false;
        }
#endif
		else if (!strcasecmp(argv[i], "-noopt"))
		{
			g_noopt = true;
		}
        else if (!strcasecmp(argv[i], "-subdivide"))
        {
            if (i < argc)
            {
                g_subdivide_size = atoi(argv[++i]);
                if (g_subdivide_size > MAX_SUBDIVIDE_SIZE)
                {
                    Warning
                        ("Maximum value for subdivide size is %i, '-subdivide %i' ignored",
                         MAX_SUBDIVIDE_SIZE, g_subdivide_size);
                    g_subdivide_size = MAX_SUBDIVIDE_SIZE;
                }
                else if (g_subdivide_size < MIN_SUBDIVIDE_SIZE)
                {
                    Warning
                        ("Mininum value for subdivide size is %i, '-subdivide %i' ignored",
                         MIN_SUBDIVIDE_SIZE, g_subdivide_size);
                    g_subdivide_size = MAX_SUBDIVIDE_SIZE;
                }
            }
            else
            {
                Usage();
            }
        }
        else if (!strcasecmp(argv[i], "-maxnodesize"))
        {
            if (i < argc)
            {
                g_maxnode_size = atoi(argv[++i]);
                if (g_maxnode_size > MAX_MAXNODE_SIZE)
                {
                    Warning
                        ("Maximum value for max node size is %i, '-maxnodesize %i' ignored",
                         MAX_MAXNODE_SIZE, g_maxnode_size);
                    g_maxnode_size = MAX_MAXNODE_SIZE;
                }
                else if (g_maxnode_size < MIN_MAXNODE_SIZE)
                {
                    Warning
                        ("Mininimum value for max node size is %i, '-maxnodesize %i' ignored",
                         MIN_MAXNODE_SIZE, g_maxnode_size);
                    g_maxnode_size = MAX_MAXNODE_SIZE;
                }
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

    if (!mapname_from_arg)
    {
        Log("No mapfile specified\n");
        Usage();
    }

    safe_strncpy(g_Mapname, mapname_from_arg, _MAX_PATH);
    FlipSlashes(g_Mapname);
    StripExtension(g_Mapname);
    OpenLog(g_clientid);
    atexit(CloseLog);
    ThreadSetDefault();
    ThreadSetPriority(g_threadpriority);
    LogStart(argc, argv);

    CheckForErrorLog();

    dtexdata_init();
    atexit(dtexdata_free);
    //Settings();
    // END INIT

    // Load the .void files for allowable entities in the void
    {
        char            g_source[_MAX_PATH];
        char            strSystemEntitiesVoidFile[_MAX_PATH];
        char            strMapEntitiesVoidFile[_MAX_PATH];

        safe_strncpy(g_source, mapname_from_arg, _MAX_PATH);
        StripExtension(g_source);

        // try looking in the current directory
        safe_strncpy(strSystemEntitiesVoidFile, ENTITIES_VOID, _MAX_PATH);
        if (!q_exists(strSystemEntitiesVoidFile))
        {
            char tmp[_MAX_PATH];
            // try looking in the directory we were run from
#ifdef SYSTEM_WIN32
            GetModuleFileName(NULL, tmp, _MAX_PATH);
#else
            safe_strncpy(tmp, argv[0], _MAX_PATH);
#endif
            ExtractFilePath(tmp, strSystemEntitiesVoidFile);
            safe_strncat(strSystemEntitiesVoidFile, ENTITIES_VOID, _MAX_PATH);
        }

        // Set the optional level specific lights filename
        safe_strncpy(strMapEntitiesVoidFile, g_source, _MAX_PATH);
        DefaultExtension(strMapEntitiesVoidFile, ENTITIES_VOID_EXT);

        LoadAllowableOutsideList(strSystemEntitiesVoidFile);    // default entities.void
        if (*strMapEntitiesVoidFile)
        {
            LoadAllowableOutsideList(strMapEntitiesVoidFile);   // automatic mapname.void
        }
    }

    // BEGIN BSP
    start = I_FloatTime();

    ProcessFile(g_Mapname);

    end = I_FloatTime();
    LogTimeElapsed(end - start);
    // END BSP

    FreeAllowableOutsideList();

    return 0;
}
