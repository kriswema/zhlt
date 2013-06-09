#include "cmdlib.h"
#include "mathlib.h"
#include "bspfile.h"

// #define      ON_EPSILON      0.001

typedef struct tnode_s
{
    planetypes      type;
    vec3_t          normal;
    float           dist;
    int             children[2];
    int             pad;
} tnode_t;

static tnode_t* tnodes;
static tnode_t* tnode_p;

/*
 * ==============
 * MakeTnode
 * 
 * Converts the disk node structure into the efficient tracing structure
 * ==============
 */
static void     MakeTnode(const int nodenum)
{
    tnode_t*        t;
    dplane_t*       plane;
    int             i;
    dnode_t*        node;

    t = tnode_p++;

    node = g_dnodes + nodenum;
    plane = g_dplanes + node->planenum;

    t->type = plane->type;
    VectorCopy(plane->normal, t->normal);
    t->dist = plane->dist;

    for (i = 0; i < 2; i++)
    {
        if (node->children[i] < 0)
            t->children[i] = g_dleafs[-node->children[i] - 1].contents;
        else
        {
            t->children[i] = tnode_p - tnodes;
            MakeTnode(node->children[i]);
        }
    }

}

/*
 * =============
 * MakeTnodes
 * 
 * Loads the node structure out of a .bsp file to be used for light occlusion
 * =============
 */
void            MakeTnodes(dmodel_t* /*bm*/)
{
    // 32 byte align the structs
    tnodes = (tnode_t*)calloc((g_numnodes + 1), sizeof(tnode_t));

#if SIZEOF_CHARP == 8
    tnodes = (tnode_t*)(((long long)tnodes + 31) & ~31);
#else
    tnodes = (tnode_t*)(((int)tnodes + 31) & ~31);
#endif
    tnode_p = tnodes;

    MakeTnode(0);
}

//==========================================================

int             TestLine_r(const int node, const vec3_t start, const vec3_t stop)
{
    tnode_t*        tnode;
    float           front, back;
    vec3_t          mid;
    float           frac;
    int             side;
    int             r;

	if (   (node == CONTENTS_SOLID) 
        || (node == CONTENTS_SKY  ) 
      /*|| (node == CONTENTS_NULL ) */
       )
		return node;

    if (node < 0)
        return CONTENTS_EMPTY; 

    tnode = &tnodes[node];
    switch (tnode->type)
    {
    case plane_x:
        front = start[0] - tnode->dist;
        back = stop[0] - tnode->dist;
        break;
    case plane_y:
        front = start[1] - tnode->dist;
        back = stop[1] - tnode->dist;
        break;
    case plane_z:
        front = start[2] - tnode->dist;
        back = stop[2] - tnode->dist;
        break;
    default:
        front = (start[0] * tnode->normal[0] + start[1] * tnode->normal[1] + start[2] * tnode->normal[2]) - tnode->dist;
        back = (stop[0] * tnode->normal[0] + stop[1] * tnode->normal[1] + stop[2] * tnode->normal[2]) - tnode->dist;
        break;
    }

    if (front >= -ON_EPSILON && back >= -ON_EPSILON)
        return TestLine_r(tnode->children[0], start, stop);

    if (front < ON_EPSILON && back < ON_EPSILON)
        return TestLine_r(tnode->children[1], start, stop);

    side = front < 0;

    frac = front / (front - back);

    mid[0] = start[0] + (stop[0] - start[0]) * frac;
    mid[1] = start[1] + (stop[1] - start[1]) * frac;
    mid[2] = start[2] + (stop[2] - start[2]) * frac;

    r = TestLine_r(tnode->children[side], start, mid);
    if (r != CONTENTS_EMPTY)
        return r;
    return TestLine_r(tnode->children[!side], mid, stop);
}

int             TestLine(const vec3_t start, const vec3_t stop)
{
    return TestLine_r(0, start, stop);
}
