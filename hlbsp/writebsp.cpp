#include "bsp5.h"

//  WriteClipNodes_r
//  WriteClipNodes
//  WriteDrawLeaf
//  WriteFace
//  WriteDrawNodes_r
//  FreeDrawNodes_r
//  WriteDrawNodes
//  BeginBSPFile
//  FinishBSPFile

#include <map>

typedef std::map<int,int> PlaneMap;
static PlaneMap gPlaneMap;
static int gNumMappedPlanes;
static dplane_t gMappedPlanes[MAX_MAP_PLANES];
extern bool g_noopt;

// =====================================================================================
//  WritePlane
//  hook for plane optimization
// =====================================================================================
static int WritePlane(int planenum)
{
	planenum = planenum & (~1);

	if(g_noopt)
	{
		return planenum;
	}

	PlaneMap::iterator item = gPlaneMap.find(planenum);
	if(item != gPlaneMap.end())
	{
		return item->second;
	}
	//add plane to BSP
	hlassume(gNumMappedPlanes < MAX_MAP_PLANES, assume_MAX_MAP_PLANES);
	gMappedPlanes[gNumMappedPlanes] = g_dplanes[planenum];
	gPlaneMap.insert(PlaneMap::value_type(planenum,gNumMappedPlanes));

	return gNumMappedPlanes++;
}

// =====================================================================================
//  WriteClipNodes_r
// =====================================================================================
static int      WriteClipNodes_r(node_t* node)
{
    int             i, c;
    dclipnode_t*    cn;
    int             num;

    if (node->planenum == -1)
    {
        num = node->contents;
        free(node->markfaces);
        free(node);
        return num;
    }

    // emit a clipnode
    hlassume(g_numclipnodes < MAX_MAP_CLIPNODES, assume_MAX_MAP_CLIPNODES);

    c = g_numclipnodes;
    cn = &g_dclipnodes[g_numclipnodes];
    g_numclipnodes++;
    if (node->planenum & 1)
    {
        Error("WriteClipNodes_r: odd planenum");
    }
    cn->planenum = WritePlane(node->planenum);
    for (i = 0; i < 2; i++)
    {
        cn->children[i] = WriteClipNodes_r(node->children[i]);
    }

    free(node);
    return c;
}

// =====================================================================================
//  WriteClipNodes
//      Called after the clipping hull is completed.  Generates a disk format
//      representation and frees the original memory.
// =====================================================================================
void            WriteClipNodes(node_t* nodes)
{
    WriteClipNodes_r(nodes);
}

// =====================================================================================
//  WriteDrawLeaf
// =====================================================================================
static void     WriteDrawLeaf(const node_t* const node)
{
    face_t**        fp;
    face_t*         f;
    dleaf_t*        leaf_p;

    // emit a leaf
    leaf_p = &g_dleafs[g_numleafs];
    g_numleafs++;

    leaf_p->contents = node->contents;

    //
    // write bounding box info
    //
    VectorCopy(node->mins, leaf_p->mins);
    VectorCopy(node->maxs, leaf_p->maxs);

    leaf_p->visofs = -1;                                   // no vis info yet

    //
    // write the marksurfaces
    //
    leaf_p->firstmarksurface = g_nummarksurfaces;

    hlassume(node->markfaces != NULL, assume_EmptySolid);

    for (fp = node->markfaces; *fp; fp++)
    {
        // emit a marksurface
        f = *fp;
        do
        {
            g_dmarksurfaces[g_nummarksurfaces] = f->outputnumber;
            hlassume(g_nummarksurfaces < MAX_MAP_MARKSURFACES, assume_MAX_MAP_MARKSURFACES);
            g_nummarksurfaces++;
            f = f->original;                               // grab tjunction split faces
        }
        while (f);
    }
    free(node->markfaces);

    leaf_p->nummarksurfaces = g_nummarksurfaces - leaf_p->firstmarksurface;
}

// =====================================================================================
//  WriteFace
// =====================================================================================
static void     WriteFace(face_t* f)
{
    dface_t*        df;
    int             i;
    int             e;

    if (    CheckFaceForHint(f)
        ||  CheckFaceForSkip(f)
#ifdef ZHLT_NULLTEX
        ||  CheckFaceForNull(f)  // AJM
#endif

// =====================================================================================
//Cpt_Andrew - Env_Sky Check
// =====================================================================================
       ||  CheckFaceForEnv_Sky(f)
// =====================================================================================

       )
    {
        return;
    }

    f->outputnumber = g_numfaces;

    df = &g_dfaces[g_numfaces];
    hlassume(g_numfaces < MAX_MAP_FACES, assume_MAX_MAP_FACES);
    g_numfaces++;

	df->planenum = WritePlane(f->planenum);
	df->side = f->planenum & 1;
    df->firstedge = g_numsurfedges;
    df->numedges = f->numpoints;
    df->texinfo = f->texturenum;
    for (i = 0; i < f->numpoints; i++)
    {
        e = GetEdge(f->pts[i], f->pts[(i + 1) % f->numpoints], f);
        hlassume(g_numsurfedges < MAX_MAP_SURFEDGES, assume_MAX_MAP_SURFEDGES);
        g_dsurfedges[g_numsurfedges] = e;
        g_numsurfedges++;
    }
}

// =====================================================================================
//  WriteDrawNodes_r
// =====================================================================================
static void     WriteDrawNodes_r(const node_t* const node)
{
    dnode_t*        n;
    int             i;
    face_t*         f;

    // emit a node
    hlassume(g_numnodes < MAX_MAP_NODES, assume_MAX_MAP_NODES);
    n = &g_dnodes[g_numnodes];
    g_numnodes++;

    VectorCopy(node->mins, n->mins);
    VectorCopy(node->maxs, n->maxs);

    if (node->planenum & 1)
    {
        Error("WriteDrawNodes_r: odd planenum");
    }
    n->planenum = WritePlane(node->planenum);
    n->firstface = g_numfaces;

    for (f = node->faces; f; f = f->next)
    {
        WriteFace(f);
    }

    n->numfaces = g_numfaces - n->firstface;

    //
    // recursively output the other nodes
    //
    for (i = 0; i < 2; i++)
    {
        if (node->children[i]->planenum == -1)
        {
            if (node->children[i]->contents == CONTENTS_SOLID)
            {
                n->children[i] = -1;
            }
            else
            {
                n->children[i] = -(g_numleafs + 1);
                WriteDrawLeaf(node->children[i]);
            }
        }
        else
        {
            n->children[i] = g_numnodes;
            WriteDrawNodes_r(node->children[i]);
        }
    }
}

// =====================================================================================
//  FreeDrawNodes_r
// =====================================================================================
static void     FreeDrawNodes_r(node_t* node)
{
    int             i;
    face_t*         f;
    face_t*         next;

    for (i = 0; i < 2; i++)
    {
        if (node->children[i]->planenum != -1)
        {
            FreeDrawNodes_r(node->children[i]);
        }
    }

    //
    // free the faces on the node
    //
    for (f = node->faces; f; f = next)
    {
        next = f->next;
        FreeFace(f);
    }

    free(node);
}

// =====================================================================================
//  WriteDrawNodes
//      Called after a drawing hull is completed
//      Frees all nodes and faces
// =====================================================================================
void            WriteDrawNodes(node_t* headnode)
{
    if (headnode->contents < 0)
    {
        WriteDrawLeaf(headnode);
    }
    else
    {
        WriteDrawNodes_r(headnode);
        FreeDrawNodes_r(headnode);
    }
}


// =====================================================================================
//  BeginBSPFile
// =====================================================================================
void            BeginBSPFile()
{
    // these values may actually be initialized
    // if the file existed when loaded, so clear them explicitly
	gNumMappedPlanes = 0;
	gPlaneMap.clear();
    g_nummodels = 0;
    g_numfaces = 0;
    g_numnodes = 0;
    g_numclipnodes = 0;
    g_numvertexes = 0;
    g_nummarksurfaces = 0;
    g_numsurfedges = 0;

    // edge 0 is not used, because 0 can't be negated
    g_numedges = 1;

    // leaf 0 is common solid with no faces
    g_numleafs = 1;
    g_dleafs[0].contents = CONTENTS_SOLID;
}

// =====================================================================================
//  FinishBSPFile
// =====================================================================================
void            FinishBSPFile()
{
    Verbose("--- FinishBSPFile ---\n");

	if(!g_noopt)
	{
		for(int counter = 0; counter < gNumMappedPlanes; counter++)
		{
			g_dplanes[counter] = gMappedPlanes[counter];
		}
		g_numplanes = gNumMappedPlanes;
	}

	if (g_chart)
    {
        PrintBSPFileSizes();
    }

    WriteBSPFile(g_bspfilename);
}
