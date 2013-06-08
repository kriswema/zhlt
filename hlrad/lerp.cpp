#pragma warning(disable: 4018) //amckern - 64bit - '<' Singed/Unsigned Mismatch

#include "qrad.h"

int             g_lerp_enabled = DEFAULT_LERP_ENABLED;

// =====================================================================================
//  TestWallIntersectTri
//      Returns true if wall polygon intersects patch polygon
// =====================================================================================
static bool     TestWallIntersectTri(const lerpTriangulation_t* const trian, const vec3_t p1, const vec3_t p2, const vec3_t p3)
{
    int             x;
    const lerpWall_t* wall = trian->walls;
    dplane_t        plane;

#ifdef HLRAD_FASTMATH
	PlaneFromPoints(p1,p2,p3,&plane);
#else
    plane_from_points(p1, p2, p3, &plane);
#endif

    // Try first 'vertical' side
    // Since we test each of the 3 segments from patch against wall, only one side of wall needs testing inside 
    // patch (since they either dont intersect at all at this point, or both line segments intersect inside)
    for (x = 0; x < trian->numwalls; x++, wall++)
    {
        vec3_t          point;

        // Try side A
#ifdef HLRAD_FASTMATH
		if(LineSegmentIntersectsPlane(plane,wall->vertex[0],wall->vertex[3],point))
		{
			if(PointInTri(point,&plane,p1,p2,p3))
			{
#else
        if (intersect_linesegment_plane(&plane, wall->vertex[0], wall->vertex[3], point))
        {
            if (point_in_tri(point, &plane, p1, p2, p3))
            {
#endif
#if 0
                Verbose
                    ("Wall side A point @ (%4.3f %4.3f %4.3f) inside patch (%4.3f %4.3f %4.3f) (%4.3f %4.3f %4.3f) (%4.3f %4.3f %4.3f)\n",
                     point[0], point[1], point[2], p1[0], p1[1], p1[2], p2[0], p2[1], p2[2], p3[0], p3[1], p3[2]);
#endif
                return true;
            }
        }
    }
    return false;
}

// =====================================================================================
//  TestLineSegmentIntersectWall
//      Returns true if line would hit the 'wall' (to fix light streaking)
// =====================================================================================
static bool     TestLineSegmentIntersectWall(const lerpTriangulation_t* const trian, const vec3_t p1, const vec3_t p2)
{
    int             x;
    const lerpWall_t* wall = trian->walls;

    for (x = 0; x < trian->numwalls; x++, wall++)
    {
        vec3_t          point;

#ifdef HLRAD_FASTMATH
		if(LineSegmentIntersectsPlane(wall->plane,p1,p2,point))
		{
			if(PointInWall(wall,point))
			{
#else
        if (intersect_linesegment_plane(&wall->plane, p1, p2, point))
        {
            if (point_in_wall(wall, point))
			{
#endif
#if 0
                Verbose
                    ("Tested point @ (%4.3f %4.3f %4.3f) blocks segment from (%4.3f %4.3f %4.3f) to (%4.3f %4.3f %4.3f) intersects wall\n",
                     point[0], point[1], point[2], p1[0], p1[1], p1[2], p2[0], p2[1], p2[2]);
#endif
                return true;
            }
        }
    }
    return false;
}

// =====================================================================================
//  TestTriIntersectWall
//      Returns true if line would hit the 'wall' (to fix light streaking)
// =====================================================================================
static bool     TestTriIntersectWall(const lerpTriangulation_t* trian, const vec3_t p1, const vec3_t p2,
                                     const vec3_t p3)
{
    if (TestLineSegmentIntersectWall(trian, p1, p2) || TestLineSegmentIntersectWall(trian, p1, p3)
        || TestLineSegmentIntersectWall(trian, p2, p3))
    {
        return true;
    }
    return false;
}

// =====================================================================================
//  LerpTriangle
//      pt1 must be closest point
// =====================================================================================
#ifdef ZHLT_TEXLIGHT
static void     LerpTriangle(const lerpTriangulation_t* const trian, const vec3_t point, vec3_t result, const unsigned pt1, const unsigned pt2, const unsigned pt3, int style) //LRC
#else
static void     LerpTriangle(const lerpTriangulation_t* const trian, const vec3_t point, vec3_t result, const unsigned pt1, const unsigned pt2, const unsigned pt3)
#endif
{
    patch_t*        p1;
    patch_t*        p2;
    patch_t*        p3;
    vec3_t          base;
    vec3_t          d1;
    vec3_t          d2;
    vec_t           x;
    vec_t           y;
    vec_t           y1;
    vec_t           x2;
    vec3_t          v;
    dplane_t        ep1;
    dplane_t        ep2;

    p1 = trian->points[pt1];
    p2 = trian->points[pt2];
    p3 = trian->points[pt3];

#ifdef ZHLT_TEXLIGHT
    VectorCopy(*GetTotalLight(p1, style), base); //LRC
    VectorSubtract(*GetTotalLight(p2, style), base, d1); //LRC
    VectorSubtract(*GetTotalLight(p3, style), base, d2); //LRC
#else
    VectorCopy(p1->totallight, base);
    VectorSubtract(p2->totallight, base, d1);
    VectorSubtract(p3->totallight, base, d2);
#endif

    // Get edge normals
    VectorSubtract(p1->origin, p2->origin, v);
    VectorNormalize(v);
    CrossProduct(v, trian->plane->normal, ep1.normal);
    ep1.dist = DotProduct(p1->origin, ep1.normal);

    VectorSubtract(p1->origin, p3->origin, v);
    VectorNormalize(v);
    CrossProduct(v, trian->plane->normal, ep2.normal);
    ep2.dist = DotProduct(p1->origin, ep2.normal);

    x = DotProduct(point, ep1.normal) - ep1.dist;
    y = DotProduct(point, ep2.normal) - ep2.dist;

    y1 = DotProduct(p2->origin, ep2.normal) - ep2.dist;
    x2 = DotProduct(p3->origin, ep1.normal) - ep1.dist;

    VectorCopy(base, result);
    if (fabs(x2) >= ON_EPSILON)
    {
        int             i;

        for (i = 0; i < 3; i++)
        {
            result[i] += x * d2[i] / x2;
        }
    }
    if (fabs(y1) >= ON_EPSILON)
    {
        int             i;

        for (i = 0; i < 3; i++)
        {
            result[i] += y * d1[i] / y1;
        }
    }
}

// =====================================================================================
//  LerpNearest
// =====================================================================================
#ifdef ZHLT_TEXLIGHT
static void     LerpNearest(const lerpTriangulation_t* const trian, vec3_t result, int style) //LRC
#else
static void     LerpNearest(const lerpTriangulation_t* const trian, vec3_t result)
#endif
{
    unsigned        x;
    unsigned        numpoints = trian->numpoints;
    patch_t*        patch;

    // Find nearest in original face
    for (x = 0; x < numpoints; x++)
    {
        patch = trian->points[trian->dists[x].patch];

        if (patch->faceNumber == trian->facenum)
        {
#ifdef ZHLT_TEXLIGHT
            VectorCopy(*GetTotalLight(patch, style), result); //LRC
#else
            VectorCopy(patch->totallight, result);
#endif
            return;
        }
    }

    // If none in nearest face, settle for nearest
    if (numpoints)
    {
#ifdef ZHLT_TEXLIGHT 
        VectorCopy(*GetTotalLight(trian->points[trian->dists[0].patch], style), result); //LRC
#else
        VectorCopy(trian->points[trian->dists[0].patch]->totallight, result);
#endif
    }
    else
    {
        VectorClear(result);
    }
}

// =====================================================================================
//  LerpEdge
// =====================================================================================
#ifdef ZHLT_TEXLIGHT
static bool     LerpEdge(const lerpTriangulation_t* const trian, const vec3_t point, vec3_t result, int style) //LRC
#else
static bool     LerpEdge(const lerpTriangulation_t* const trian, const vec3_t point, vec3_t result)
#endif
{
    patch_t*        p1;
    patch_t*        p2;
    patch_t*        p3;
    vec3_t          v1;
    vec3_t          v2;
    vec_t           d;

    p1 = trian->points[trian->dists[0].patch];
    p2 = trian->points[trian->dists[1].patch];
    p3 = trian->points[trian->dists[2].patch];

    VectorSubtract(point, p1->origin, v2);
    VectorNormalize(v2);

    // Try nearest and 2
    if (!TestLineSegmentIntersectWall(trian, p1->origin, p2->origin))
    {
        VectorSubtract(p2->origin, p1->origin, v1);
        VectorNormalize(v1);
        d = DotProduct(v2, v1);
        if (d >= ON_EPSILON)
        {
            int             i;
            vec_t           length1;
            vec_t           length2;
            vec3_t          segment;
            vec_t           total_length;

            VectorSubtract(point, p1->origin, segment);
            length1 = VectorLength(segment);
            VectorSubtract(point, p2->origin, segment);
            length2 = VectorLength(segment);
            total_length = length1 + length2;

            for (i = 0; i < 3; i++)
            {
#ifdef ZHLT_TEXLIGHT
                result[i] = (((*GetTotalLight(p1, style))[i] * length2) + ((*GetTotalLight(p1, style))[i] * length1)) / total_length; //LRC
#else
                result[i] = ((p1->totallight[i] * length2) + (p2->totallight[i] * length1)) / total_length;
#endif
            }
            return true;
        }
    }

    // Try nearest and 3
    if (!TestLineSegmentIntersectWall(trian, p1->origin, p3->origin))
    {
        VectorSubtract(p3->origin, p1->origin, v1);
        VectorNormalize(v1);
        d = DotProduct(v2, v1);
        if (d >= ON_EPSILON)
        {
            int             i;
            vec_t           length1;
            vec_t           length2;
            vec3_t          segment;
            vec_t           total_length;

            VectorSubtract(point, p1->origin, segment);
            length1 = VectorLength(segment);
            VectorSubtract(point, p3->origin, segment);
            length2 = VectorLength(segment);
            total_length = length1 + length2;

            for (i = 0; i < 3; i++)
            {
#ifdef ZHLT_TEXLIGHT
                result[i] = (((*GetTotalLight(p1, style))[i] * length2) + ((*GetTotalLight(p3, style))[i] * length1)) / total_length; //LRC

#else
                result[i] = ((p1->totallight[i] * length2) + (p3->totallight[i] * length1)) / total_length;
#endif
            }
            return true;
        }
    }
    return false;
}


// =====================================================================================
//
//  SampleTriangulation
//
// =====================================================================================

// =====================================================================================
//  dist_sorter
// =====================================================================================
static int CDECL dist_sorter(const void* p1, const void* p2)
{
    lerpDist_t*     dist1 = (lerpDist_t*) p1;
    lerpDist_t*     dist2 = (lerpDist_t*) p2;

    if (dist1->dist < dist2->dist)
    {
        return -1;
    }
    else if (dist1->dist > dist2->dist)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

// =====================================================================================
//  FindDists
// =====================================================================================
static void     FindDists(const lerpTriangulation_t* const trian, const vec3_t point)
{
    unsigned        x;
    unsigned        numpoints = trian->numpoints;
    patch_t**       patch = trian->points;
    lerpDist_t*     dists = trian->dists;
    vec3_t          delta;

    for (x = 0; x < numpoints; x++, patch++, dists++)
    {
        VectorSubtract((*patch)->origin, point, delta);
        dists->dist = VectorLength(delta);
        dists->patch = x;
    }

    qsort((void*)trian->dists, (size_t) numpoints, sizeof(lerpDist_t), dist_sorter);
}

// =====================================================================================
//  SampleTriangulation
// =====================================================================================
#ifdef ZHLT_TEXLIGHT
void            SampleTriangulation(const lerpTriangulation_t* const trian, vec3_t point, vec3_t result, int style) //LRC
#else
void            SampleTriangulation(const lerpTriangulation_t* const trian, vec3_t point, vec3_t result)
#endif
{
    FindDists(trian, point);

    if ((trian->numpoints > 3) && (g_lerp_enabled))
    {
        unsigned        pt1;
        unsigned        pt2;
        unsigned        pt3;
        vec_t*          p1;
        vec_t*          p2;
        vec_t*          p3;
        dplane_t        plane;

        pt1 = trian->dists[0].patch;
        pt2 = trian->dists[1].patch;
        pt3 = trian->dists[2].patch;

        p1 = trian->points[pt1]->origin;
        p2 = trian->points[pt2]->origin;
        p3 = trian->points[pt3]->origin;

#ifdef HLRAD_FASTMATH // KGP
		PlaneFromPoints(p1,p2,p3,&plane);
		SnapToPlane(&plane,point,0.0);
		if(PointInTri(point,&plane,p1,p2,p3))
		{
#else
        plane_from_points(p1, p2, p3, &plane);
        SnapToPlane(&plane, point, 0.0);
        if (point_in_tri(point, &plane, p1, p2, p3))
        {                                                  // TODO check edges/tri for blocking by wall
#endif
            if (!TestWallIntersectTri(trian, p1, p2, p3) && !TestTriIntersectWall(trian, p1, p2, p3))
            {
#ifdef ZHLT_TEXLIGHT
                LerpTriangle(trian, point, result, pt1, pt2, pt3, style); //LRC
#else
                LerpTriangle(trian, point, result, pt1, pt2, pt3);
#endif
                return;
            }
        }
        else
        {
#ifdef ZHLT_TEXLIGHT
            if (LerpEdge(trian, point, result, style)) //LRC
#else
            if (LerpEdge(trian, point, result))
#endif
            {
                return;
            }
        }
    }

#ifdef ZHLT_TEXLIGHT
    LerpNearest(trian, result, style); //LRC
#else
    LerpNearest(trian, result);
#endif
}

// =====================================================================================
//  AddPatchToTriangulation
// =====================================================================================
static void     AddPatchToTriangulation(lerpTriangulation_t* trian, patch_t* patch)
{
    if (!(patch->flags & ePatchFlagOutside))
    {
        int             pnum = trian->numpoints;

        if (pnum >= trian->maxpoints)
        {
            trian->points = (patch_t**)realloc(trian->points, sizeof(patch_t*) * (trian->maxpoints + DEFAULT_MAX_LERP_POINTS));

            hlassume(trian->points != NULL, assume_NoMemory);
            memset(trian->points + trian->maxpoints, 0, sizeof(patch_t*) * DEFAULT_MAX_LERP_POINTS);   // clear the new block

            trian->maxpoints += DEFAULT_MAX_LERP_POINTS;
        }

        trian->points[pnum] = patch;
        trian->numpoints++;
    }
}

// =====================================================================================
//  CreateWalls
// =====================================================================================
static void     CreateWalls(lerpTriangulation_t* trian, const dface_t* const face)
{
    const dplane_t* p = getPlaneFromFace(face);
    int             facenum = face - g_dfaces;
    int             x;

    for (x = 0; x < face->numedges; x++)
    {
        edgeshare_t*    es;
        dface_t*        f2;
        int             edgenum = g_dsurfedges[face->firstedge + x];

        if (edgenum > 0)
        {
            es = &g_edgeshare[edgenum];
            f2 = es->faces[1];
        }
        else
        {
            es = &g_edgeshare[-edgenum];
            f2 = es->faces[0];
        }

        // Build Wall for non-coplanar neighbhors
        if (f2 && !es->coplanar && VectorCompare(vec3_origin, es->interface_normal))
        {
            const dplane_t* plane = getPlaneFromFace(f2);

            // if plane isn't facing us, ignore it
            if (DotProduct(plane->normal, g_face_centroids[facenum]) < plane->dist)
            {
                continue;
            }

            {
                vec3_t          delta;
                vec3_t          p0;
                vec3_t          p1;
                lerpWall_t*     wall;

                if (trian->numwalls >= trian->maxwalls)
                {
                    trian->walls =
                        (lerpWall_t*)realloc(trian->walls, sizeof(lerpWall_t) * (trian->maxwalls + DEFAULT_MAX_LERP_WALLS));
                    hlassume(trian->walls != NULL, assume_NoMemory);
                    memset(trian->walls + trian->maxwalls, 0, sizeof(lerpWall_t) * DEFAULT_MAX_LERP_WALLS);     // clear the new block
                    trian->maxwalls += DEFAULT_MAX_LERP_WALLS;
                }

                wall = &trian->walls[trian->numwalls];
                trian->numwalls++;

                VectorScale(p->normal, 10000.0, delta);

                VectorCopy(g_dvertexes[g_dedges[abs(edgenum)].v[0]].point, p0);
                VectorCopy(g_dvertexes[g_dedges[abs(edgenum)].v[1]].point, p1);

                // Adjust for origin-based models
                // technically we should use the other faces g_face_offset entries
                // If they are nonzero, it has to be from the same model with
                // the same offset, so we are cool
                VectorAdd(p0, g_face_offset[facenum], p0);
                VectorAdd(p1, g_face_offset[facenum], p1);

                VectorAdd(p0, delta, wall->vertex[0]);
                VectorAdd(p1, delta, wall->vertex[1]);
                VectorSubtract(p1, delta, wall->vertex[2]);
                VectorSubtract(p0, delta, wall->vertex[3]);

                {
                    vec3_t          delta1;
                    vec3_t          delta2;
                    vec3_t          normal;
                    vec_t           dist;

                    VectorSubtract(wall->vertex[2], wall->vertex[1], delta1);
                    VectorSubtract(wall->vertex[0], wall->vertex[1], delta2);
                    CrossProduct(delta1, delta2, normal);
                    VectorNormalize(normal);
                    dist = DotProduct(normal, p0);

                    VectorCopy(normal, wall->plane.normal);
                    wall->plane.dist = dist;
                }
            }
        }
    }
}

// =====================================================================================
//  AllocTriangulation
// =====================================================================================
static lerpTriangulation_t* AllocTriangulation()
{
    lerpTriangulation_t* trian = (lerpTriangulation_t*)calloc(1, sizeof(lerpTriangulation_t));

    trian->maxpoints = DEFAULT_MAX_LERP_POINTS;
    trian->maxwalls = DEFAULT_MAX_LERP_WALLS;

    trian->points = (patch_t**)calloc(DEFAULT_MAX_LERP_POINTS, sizeof(patch_t*));

    trian->walls = (lerpWall_t*)calloc(DEFAULT_MAX_LERP_WALLS, sizeof(lerpWall_t));

    hlassume(trian->points != NULL, assume_NoMemory);
    hlassume(trian->walls != NULL, assume_NoMemory);

    return trian;
}

// =====================================================================================
//  FreeTriangulation
// =====================================================================================
void            FreeTriangulation(lerpTriangulation_t* trian)
{
    free(trian->dists);
    free(trian->points);
    free(trian->walls);
    free(trian);
}

// =====================================================================================
//  CreateTriangulation
// =====================================================================================
lerpTriangulation_t* CreateTriangulation(const unsigned int facenum)
{
    const dface_t*  f = g_dfaces + facenum;
    const dplane_t* p = getPlaneFromFace(f);
    lerpTriangulation_t* trian = AllocTriangulation();
    patch_t*        patch;
    unsigned int    j;
    dface_t*        f2;

    trian->facenum = facenum;
    trian->plane = p;
    trian->face = f;

    for (patch = g_face_patches[facenum]; patch; patch = patch->next)
    {
        AddPatchToTriangulation(trian, patch);
    }

    CreateWalls(trian, f);

    for (j = 0; j < f->numedges; j++)
    {
        edgeshare_t*    es;
        int             edgenum = g_dsurfedges[f->firstedge + j];

        if (edgenum > 0)
        {
            es = &g_edgeshare[edgenum];
            f2 = es->faces[1];
        }
        else
        {
            es = &g_edgeshare[-edgenum];
            f2 = es->faces[0];
        }

        if (!es->coplanar && VectorCompare(vec3_origin, es->interface_normal))
        {
            continue;
        }

        for (patch = g_face_patches[f2 - g_dfaces]; patch; patch = patch->next)
        {
            AddPatchToTriangulation(trian, patch);
        }
    }

    trian->dists = (lerpDist_t*)calloc(trian->numpoints, sizeof(lerpDist_t));
#ifdef HLRAD_HULLU
    //Get rid off error that seems to happen with some opaque faces (when opaque face have all edges 'out' of map)
    if(trian->numpoints != 0)
#endif
    hlassume(trian->dists != NULL, assume_NoMemory);

    return trian;
}
