#include "bsp5.h"

//  SubdivideFace

//  InitHash
//  HashVec

//  GetVertex
//  GetEdge
//  MakeFaceEdges

static int      subdivides;

/* a surface has all of the faces that could be drawn on a given plane
   the outside filling stage can remove some of them so a better bsp can be generated */

// =====================================================================================
//  SubdivideFace
//      If the face is >256 in either texture direction, carve a valid sized
//      piece off and insert the remainder in the next link
// =====================================================================================
void            SubdivideFace(face_t* f, face_t** prevptr)
{
    vec_t           mins, maxs;
    vec_t           v;
    int             axis;
    int             i;
    dplane_t        plane;
    face_t*         front;
    face_t*         back;
    face_t*         next;
    texinfo_t*      tex;
    vec3_t          temp;

    // special (non-surface cached) faces don't need subdivision

    tex = &g_texinfo[f->texturenum];

    if (tex->flags & TEX_SPECIAL) 
    {
        return;
    }

    if (f->facestyle == face_hint)
    {
        return;
    }
    if (f->facestyle == face_skip)
    {
        return;
    }

#ifdef ZHLT_NULLTEX    // AJM
    if (f->facestyle == face_null)
        return; // ideally these should have their tex_special flag set, so its here jic
#endif

    for (axis = 0; axis < 2; axis++)
    {
        while (1)
        {
            mins = 999999;
            maxs = -999999;

            for (i = 0; i < f->numpoints; i++)
            {
                v = DotProduct(f->pts[i], tex->vecs[axis]);
                if (v < mins)
                {
                    mins = v;
                }
                if (v > maxs)
                {
                    maxs = v;
                }
            }

            if ((maxs - mins) <= g_subdivide_size)
            {
                break;
            }
                
            // split it
            subdivides++;

            VectorCopy(tex->vecs[axis], temp);
            v = VectorNormalize(temp);

            VectorCopy(temp, plane.normal);
            plane.dist = (mins + g_subdivide_size - 16) / v;
            next = f->next;
            SplitFace(f, &plane, &front, &back);
            if (!front || !back)
            {
                Developer(DEVELOPER_LEVEL_SPAM, "SubdivideFace: didn't split the %d-sided polygon @(%.0f,%.0f,%.0f)",
                        f->numpoints, f->pts[0][0], f->pts[0][1], f->pts[0][2]);
                break;
            }
            *prevptr = back;
            back->next = front;
            front->next = next;
            f = back;
        }
    }
}

//===========================================================================

typedef struct hashvert_s
{
    struct hashvert_s* next;
    vec3_t          point;
    int             num;
    int             numplanes;                             // for corner determination
    int             planenums[2];
    int             numedges;
}
hashvert_t;

// #define      POINT_EPSILON   0.01
#define POINT_EPSILON	ON_EPSILON

static hashvert_t hvertex[MAX_MAP_VERTS];
static hashvert_t* hvert_p;

static face_t*  edgefaces[MAX_MAP_EDGES][2];
static int      firstmodeledge = 1;
static int      firstmodelface;

//============================================================================

#define	NUM_HASH	4096

static hashvert_t* hashverts[NUM_HASH];

static vec3_t   hash_min;
static vec3_t   hash_scale;

// =====================================================================================
//  InitHash
// =====================================================================================
static void     InitHash()
{
    vec3_t          size;
    vec_t           volume;
    vec_t           scale;
    int             newsize[2];
    int             i;

    memset(hashverts, 0, sizeof(hashverts));

    for (i = 0; i < 3; i++)
    {
        hash_min[i] = -8000;
        size[i] = 16000;
    }

    volume = size[0] * size[1];

    scale = sqrt(volume / NUM_HASH);

    newsize[0] = size[0] / scale;
    newsize[1] = size[1] / scale;

    hash_scale[0] = newsize[0] / size[0];
    hash_scale[1] = newsize[1] / size[1];
    hash_scale[2] = newsize[1];

    hvert_p = hvertex;
}

// =====================================================================================
//  HashVec
// =====================================================================================
static unsigned HashVec(const vec3_t vec)
{
    unsigned        h;

    h = hash_scale[0] * (vec[0] - hash_min[0]) * hash_scale[2] + hash_scale[1] * (vec[1] - hash_min[1]);
    if (h >= NUM_HASH)
    {
        return NUM_HASH - 1;
    }
    return h;
}

// =====================================================================================
//  GetVertex
// =====================================================================================
static int      GetVertex(const vec3_t in, const int planenum)
{
    int             h;
    int             i;
    hashvert_t*     hv;
    vec3_t          vert;

    for (i = 0; i < 3; i++)
    {
        if (fabs(in[i] - VectorRound(in[i])) < 0.001)
        {
            vert[i] = VectorRound(in[i]);
        }
        else
        {
            vert[i] = in[i];
        }
    }

    h = HashVec(vert);

    for (hv = hashverts[h]; hv; hv = hv->next)
    {
        if (fabs(hv->point[0] - vert[0]) < POINT_EPSILON
            && fabs(hv->point[1] - vert[1]) < POINT_EPSILON && fabs(hv->point[2] - vert[2]) < POINT_EPSILON)
        {
            hv->numedges++;
            if (hv->numplanes == 3)
            {
                return hv->num;                            // allready known to be a corner
            }
            for (i = 0; i < hv->numplanes; i++)
            {
                if (hv->planenums[i] == planenum)
                {
                    return hv->num;                        // allready know this plane
                }
            }
            if (hv->numplanes != 2)
            {
                hv->planenums[hv->numplanes] = planenum;
            }
            hv->numplanes++;
            return hv->num;
        }
    }

    hv = hvert_p;
    hv->numedges = 1;
    hv->numplanes = 1;
    hv->planenums[0] = planenum;
    hv->next = hashverts[h];
    hashverts[h] = hv;
    VectorCopy(vert, hv->point);
    hv->num = g_numvertexes;
    hlassume(hv->num != MAX_MAP_VERTS, assume_MAX_MAP_VERTS);
    hvert_p++;

    // emit a vertex
    hlassume(g_numvertexes < MAX_MAP_VERTS, assume_MAX_MAP_VERTS);

    g_dvertexes[g_numvertexes].point[0] = vert[0];
    g_dvertexes[g_numvertexes].point[1] = vert[1];
    g_dvertexes[g_numvertexes].point[2] = vert[2];
    g_numvertexes++;

    return hv->num;
}

//===========================================================================

// =====================================================================================
//  GetEdge
//      Don't allow four way edges
// =====================================================================================
int             GetEdge(const vec3_t p1, const vec3_t p2, face_t* f)
{
    int             v1;
    int             v2;
    dedge_t*        edge;
    int             i;

    hlassert(f->contents);

    v1 = GetVertex(p1, f->planenum);
    v2 = GetVertex(p2, f->planenum);
    for (i = firstmodeledge; i < g_numedges; i++)
    {
        edge = &g_dedges[i];
        if (v1 == edge->v[1] && v2 == edge->v[0] && !edgefaces[i][1] && edgefaces[i][0]->contents == f->contents)
        {
            edgefaces[i][1] = f;
            return -i;
        }
    }

    // emit an edge
    hlassume(g_numedges < MAX_MAP_EDGES, assume_MAX_MAP_EDGES);
    edge = &g_dedges[g_numedges];
    g_numedges++;
    edge->v[0] = v1;
    edge->v[1] = v2;
    edgefaces[i][0] = f;

    return i;
}

// =====================================================================================
//  MakeFaceEdges
// =====================================================================================
void            MakeFaceEdges()
{
    InitHash();
    firstmodeledge = g_numedges;
    firstmodelface = g_numfaces;
}
