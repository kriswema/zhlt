#include "csg.h"

plane_t         g_mapplanes[MAX_INTERNAL_MAP_PLANES];
int             g_nummapplanes;

#define DIST_EPSILON   0.01

#if !defined HLCSG_FASTFIND

/*
 * =============
 * FindIntPlane
 *
 * Returns which plane number to use for a given integer defined plane.
 *
 * =============
 */

int             FindIntPlane(const vec_t* const normal, const vec_t* const origin)
{
    int             i, j;
    plane_t*        p;
    plane_t         temp;
    vec_t           t;
    bool            locked;

    p = g_mapplanes;
    locked = false;
    i = 0;

    while (1)
    {
        if (i == g_nummapplanes)
        {
            if (!locked)
            {
                locked = true;
                ThreadLock();                              // make sure we don't race
            }
            if (i == g_nummapplanes)
            {
                break;                                     // we didn't race
            }
        }

        t = 0;                                             // Unrolled loop
        t += (origin[0] - p->origin[0]) * normal[0];
        t += (origin[1] - p->origin[1]) * normal[1];
        t += (origin[2] - p->origin[2]) * normal[2];

        if (fabs(t) < DIST_EPSILON)
        {                                                  // on plane
            // see if the normal is forward, backwards, or off
            for (j = 0; j < 3; j++)
            {
                if (fabs(normal[j] - p->normal[j]) > NORMAL_EPSILON)
                {
                    break;
                }
            }
            if (j == 3)
            {
                if (locked)
                {
                    ThreadUnlock();
                }
                return i;
            }
        }

        i++;
        p++;
    }

    hlassert(locked);

    // create a new plane
    p->origin[0] = origin[0];
    p->origin[1] = origin[1];
    p->origin[2] = origin[2];

    (p + 1)->origin[0] = origin[0];
    (p + 1)->origin[1] = origin[1];
    (p + 1)->origin[2] = origin[2];

    p->normal[0] = normal[0];
    p->normal[1] = normal[1];
    p->normal[2] = normal[2];

    (p + 1)->normal[0] = -normal[0];
    (p + 1)->normal[1] = -normal[1];
    (p + 1)->normal[2] = -normal[2];

    hlassume(g_nummapplanes < MAX_INTERNAL_MAP_PLANES, assume_MAX_INTERNAL_MAP_PLANES);

    VectorNormalize(p->normal);

    p->type = (p + 1)->type = PlaneTypeForNormal(p->normal);

    p->dist = DotProduct(origin, p->normal);
    VectorSubtract(vec3_origin, p->normal, (p + 1)->normal);
    (p + 1)->dist = -p->dist;

    // always put axial planes facing positive first
    if (p->type <= last_axial)
    {
        if (normal[0] < 0 || normal[1] < 0 || normal[2] < 0)
        {
            // flip order
            temp = *p;
            *p = *(p + 1);
            *(p + 1) = temp;
            g_nummapplanes += 2;
            ThreadUnlock();
            return i + 1;
        }
    }

    g_nummapplanes += 2;
    ThreadUnlock();
    return i;
}

#else //ifdef HLCSG_FASTFIND

// =====================================================================================
//  FindIntPlane, fast version (replacement by KGP)
//	This process could be optimized by placing the planes in a (non hash-) set and using
//	half of the inner loop check below as the comparator; I'd expect the speed gain to be
//	very large given the change from O(N^2) to O(NlogN) to build the set of planes.
// =====================================================================================

int FindIntPlane(const vec_t* const normal, const vec_t* const origin)
{
    int             returnval;
    plane_t*        p;
    plane_t         temp;
    vec_t           t;

	returnval = 0;

	find_plane:
	for( ; returnval < g_nummapplanes; returnval++)
	{
		if(	-NORMAL_EPSILON < (t = normal[0] - g_mapplanes[returnval].normal[0]) && t < NORMAL_EPSILON &&
			-NORMAL_EPSILON < (t = normal[1] - g_mapplanes[returnval].normal[1]) && t < NORMAL_EPSILON &&
			-NORMAL_EPSILON < (t = normal[2] - g_mapplanes[returnval].normal[2]) && t < NORMAL_EPSILON )
		{
			//t = (origin - plane_origin) dot (normal), unrolled
			t = (origin[0] - g_mapplanes[returnval].origin[0]) * normal[0]
				+ (origin[1] - g_mapplanes[returnval].origin[1]) * normal[1]
				+ (origin[2] - g_mapplanes[returnval].origin[2]) * normal[2];

			if (-DIST_EPSILON < t && t < DIST_EPSILON) // on plane
			{ return returnval; }
		}
	}

	ThreadLock();
	if(returnval != g_nummapplanes) // make sure we don't race
	{
		ThreadUnlock();
		goto find_plane; //check to see if other thread added plane we need
	}

    // create new planes - double check that we have room for 2 planes
    hlassume(g_nummapplanes+1 < MAX_INTERNAL_MAP_PLANES, assume_MAX_INTERNAL_MAP_PLANES);

	p = &g_mapplanes[g_nummapplanes];

	VectorCopy(origin,p->origin);
	VectorCopy(normal,p->normal);
    VectorNormalize(p->normal);
	p->type = PlaneTypeForNormal(p->normal);
    p->dist = DotProduct(origin, p->normal);

	VectorCopy(origin,(p+1)->origin);
	VectorSubtract(vec3_origin,p->normal,(p+1)->normal);
	(p+1)->type = p->type;
	(p+1)->dist = -p->dist;

    // always put axial planes facing positive first
    if (p->type <= last_axial && (normal[0] < 0 || normal[1] < 0 || normal[2] < 0))	// flip order
	{
		temp = *p;
		*p = *(p+1);
		*(p+1) = temp;
        returnval = g_nummapplanes+1;
	}
	else
	{ returnval = g_nummapplanes; }

	g_nummapplanes += 2;
	ThreadUnlock();
	return returnval;
}

#endif //HLCSG_FASTFIND


int PlaneFromPoints(const vec_t* const p0, const vec_t* const p1, const vec_t* const p2)
{
    vec3_t          v1, v2;
    vec3_t          normal;

    VectorSubtract(p0, p1, v1);
    VectorSubtract(p2, p1, v2);
    CrossProduct(v1, v2, normal);
    if (VectorNormalize(normal))
    {
        return FindIntPlane(normal, p0);
    }
    return -1;
}

#ifdef HLCSG_PRECISIONCLIP

const char ClipTypeStrings[5][11] = {{"smallest"},{"normalized"},{"simple"},{"precise"},{"legacy"}};

const char* GetClipTypeString(cliptype ct)
{
	return ClipTypeStrings[ct];
}


// =====================================================================================
//  AddHullPlane (subroutine for replacement of ExpandBrush, KGP)
//  Called to add any and all clip hull planes by the new ExpandBrush.
// =====================================================================================

void AddHullPlane(brushhull_t* hull, const vec_t* const normal, const vec_t* const origin, const bool check_planenum)
{
	int planenum = FindIntPlane(normal,origin);
	//check to see if this plane is already in the brush (optional to speed
	//up cases where we know the plane hasn't been added yet, like axial case)
	if(check_planenum)
	{
		if(g_mapplanes[planenum].type <= last_axial) //we know axial planes are added in last step
		{ return; }

		bface_t* current_face;
		for(current_face = hull->faces; current_face; current_face = current_face->next)
		{
			if(current_face->planenum == planenum)
			{ return; } //don't add a plane twice
		}
	}
	bface_t* new_face = (bface_t*)Alloc(sizeof(bface_t)); // TODO: This leaks
	new_face->planenum = planenum;
	new_face->plane = &g_mapplanes[new_face->planenum];
	new_face->next = hull->faces;
	new_face->contents = CONTENTS_EMPTY;
	hull->faces = new_face;
	new_face->texinfo = 0;
}

// =====================================================================================
//  ExpandBrush (replacement by KGP)
//  Since the six bounding box planes were always added anyway, they've been moved to
//  an explicit separate step eliminating the need to check for duplicate planes (which
//  should be using plane numbers instead of the full definition anyway).
//
//  The core of the new function adds additional bevels to brushes containing faces that
//  have 3 nonzero normal components -- this is necessary to finish the beveling process,
//  but is turned off by default for backward compatability and because the number of
//  clipnodes and faces will go up with the extra beveling.  The advantage of the extra
//  precision comes from the absense of "sticky" outside corners on ackward geometry.
//
//  Another source of "sticky" walls has been the inconsistant offset along each axis
//  (variant with plane normal in the old code).  The normal component of the offset has
//  been scrapped (it made a ~29% difference in the worst case of 45 degrees, or about 10
//  height units for a standard half-life player hull).  The new offsets generate fewer
//  clipping nodes and won't cause players to stick when moving across 2 brushes that flip
//  sign along an axis (this wasn't noticible on floors because the engine took care of the
//  invisible 0-3 unit steps it generated, but was noticible with walls).
//
//  To prevent players from floating 10 units above the floor, the "precise" hull generation
//  option still uses the plane normal when the Z component is high enough for the plane to
//  be considered a floor.  The "simple" hull generation option always uses the full hull
//  distance, resulting in lower clipnode counts.
//
//  Bevel planes might be added twice (once from each side of the edge), so a planenum
//  based check is used to see if each has been added before.
// =====================================================================================

void ExpandBrush(brush_t* brush, const int hullnum)
{
	//for looping through the faces and constructing the hull
	bface_t* current_face;
	plane_t* current_plane;
	brushhull_t* hull;
	vec3_t	origin, normal;

	//for non-axial bevel testing
	Winding* winding;
	bface_t* other_face;
	plane_t* other_plane;
	Winding* other_winding;
	vec3_t  edge_start, edge_end, edge, bevel_edge;
	unsigned int counter, counter2, dir;
	bool start_found,end_found;
	bool axialbevel[last_axial+1][2] = { {false,false}, {false,false}, {false,false} };

	bool warned = false;

	hull = &brush->hulls[hullnum];

	for(current_face = brush->hulls[0].faces; current_face; current_face = current_face->next)
	{
		current_plane = current_face->plane;

		//don't bother adding axial planes,
		//they're defined by adding the bounding box anyway
		if(current_plane->type <= last_axial)
		{
			//flag case where bounding box shouldn't expand
			if((g_texinfo[current_face->texinfo].flags & TEX_BEVEL))
			{
				switch(current_plane->type)
				{
				case plane_x:
					axialbevel[plane_x][(current_plane->normal[0] > 0 ? 1 : 0)] = true;
					break;
				case plane_y:
					axialbevel[plane_y][(current_plane->normal[1] > 0 ? 1 : 0)] = true;
					break;
				case plane_z:
					axialbevel[plane_z][(current_plane->normal[2] > 0 ? 1 : 0)] = true;
					break;
				}
			}
			continue;
		}

		//add the offset non-axial plane to the expanded hull
        VectorCopy(current_plane->origin, origin);
        VectorCopy(current_plane->normal, normal);

		//old code multiplied offset by normal -- this led to post-csg "sticky" walls where a
		//slope met an axial plane from the next brush since the offset from the slope would be less
		//than the full offset for the axial plane -- the discontinuity also contributes to increased
		//clipnodes.  If the normal is zero along an axis, shifting the origin in that direction won't
		//change the plane number, so I don't explicitly test that case.  The old method is still used if
		//preciseclip is turned off to allow backward compatability -- some of the improperly beveled edges
		//grow using the new origins, and might cause additional problems.

		if((g_texinfo[current_face->texinfo].flags & TEX_BEVEL))
		{
			//don't adjust origin - we'll correct g_texinfo's flags in a later step
		}
		else if(g_cliptype == clip_legacy || (g_cliptype == clip_precise && (normal[2] > FLOOR_Z)) || g_cliptype == clip_normalized)
		{
			if(normal[0])
			{ origin[0] += normal[0] * (normal[0] > 0 ? g_hull_size[hullnum][1][0] : -g_hull_size[hullnum][0][0]); }
			if(normal[1])
			{ origin[1] += normal[1] * (normal[1] > 0 ? g_hull_size[hullnum][1][1] : -g_hull_size[hullnum][0][1]); }
			if(normal[2])
			{ origin[2] += normal[2] * (normal[2] > 0 ? g_hull_size[hullnum][1][2] : -g_hull_size[hullnum][0][2]); }
		}
		else
		{
			origin[0] += g_hull_size[hullnum][(normal[0] > 0 ? 1 : 0)][0];
			origin[1] += g_hull_size[hullnum][(normal[1] > 0 ? 1 : 0)][1];
			origin[2] += g_hull_size[hullnum][(normal[2] > 0 ? 1 : 0)][2];
		}

		AddHullPlane(hull,normal,origin,false);
	} //end for loop over all faces

	//split bevel check into a second pass so we don't have to check for duplicate planes when adding offset planes
	//in step above -- otherwise a bevel plane might duplicate an offset plane, causing problems later on.

	//only executes if cliptype is simple, normalized or precise
	if(g_cliptype == clip_simple || g_cliptype == clip_precise || g_cliptype == clip_normalized)
	{
		for(current_face = brush->hulls[0].faces; current_face; current_face = current_face->next)
		{
			current_plane = current_face->plane;
			if(current_plane->type <= last_axial || !current_plane->normal[0] || !current_plane->normal[1] || !current_plane->normal[2])
			{ continue; } //only add bevels to completely non-axial planes

			//test to see if the plane is completely non-axial (if it is, need to add bevels to any
			//existing "inflection edges" where there's a sign change with a neighboring plane's normal for
			//a given axis)

			//move along winding and find plane on other side of each edge.  If normals change sign,
			//add a new plane by offsetting the points of the winding to bevel the edge in that direction.
			//It's possible to have inflection in multiple directions -- in this case, a new plane
			//must be added for each sign change in the edge.

			winding = current_face->w;

			for(counter = 0; counter < (winding->m_NumPoints); counter++) //for each edge
			{
				VectorCopy(winding->m_Points[counter],edge_start);
				VectorCopy(winding->m_Points[(counter+1)%winding->m_NumPoints],edge_end);

				//grab the edge (find relative length)
				VectorSubtract(edge_end,edge_start,edge);

				//brute force - need to check every other winding for common points -- if the points match, the
				//other face is the one we need to look at.
				for(other_face = brush->hulls[0].faces; other_face; other_face = other_face->next)
				{
					if(other_face == current_face)
					{ continue; }
					start_found = false;
					end_found = false;
					other_winding = other_face->w;
					for(counter2 = 0; counter2 < other_winding->m_NumPoints; counter2++)
					{
						if(!start_found && VectorCompare(other_winding->m_Points[counter2],edge_start))
						{ start_found = true; }
						if(!end_found && VectorCompare(other_winding->m_Points[counter2],edge_end))
						{ end_found = true; }
						if(start_found && end_found)
						{ break; } //we've found the face we want, move on to planar comparison
					} // for each point in other winding
					if(start_found && end_found)
					{ break; } //we've found the face we want, move on to planar comparison
				} // for each face

				if(!other_face)
				{
					if(hullnum == 1 && !warned)
					{
						Warning("Illegal Brush (edge without opposite face): Entity %i, Brush %i\n",brush->entitynum, brush->brushnum);
						warned = true;
					}
					continue;
				}

				other_plane = other_face->plane;


				//check each direction for sign change in normal -- zero can be safely ignored
				for(dir = 0; dir < 3; dir++)
				{
					if(current_plane->normal[dir]*other_plane->normal[dir] < 0) //sign changed, add bevel
					{
						//pick direction of bevel edge by looking at normal of existing planes
						VectorClear(bevel_edge);
						bevel_edge[dir] = (current_plane->normal[dir] > 0) ? -1 : 1;

						//find normal by taking normalized cross of the edge vector and the bevel edge
						CrossProduct(edge,bevel_edge,normal);

						//normalize to length 1
						VectorNormalize(normal);

						//get the origin
						VectorCopy(edge_start,origin);

						//unrolled loop - legacy never hits this point, so don't test for it
						if((g_cliptype == clip_precise && (normal[2] > FLOOR_Z)) || g_cliptype == clip_normalized)
						{
							if(normal[0])
							{ origin[0] += normal[0] * (normal[0] > 0 ? g_hull_size[hullnum][1][0] : -g_hull_size[hullnum][0][0]); }
							if(normal[1])
							{ origin[1] += normal[1] * (normal[1] > 0 ? g_hull_size[hullnum][1][1] : -g_hull_size[hullnum][0][1]); }
							if(normal[2])
							{ origin[2] += normal[2] * (normal[2] > 0 ? g_hull_size[hullnum][1][2] : -g_hull_size[hullnum][0][2]); }
						}
						else //simple or precise for non-floors
						{
							//note: if normal == 0 in direction indicated, shifting origin doesn't change plane #
							origin[0] += g_hull_size[hullnum][(normal[0] > 0 ? 1 : 0)][0];
							origin[1] += g_hull_size[hullnum][(normal[1] > 0 ? 1 : 0)][1];
							origin[2] += g_hull_size[hullnum][(normal[2] > 0 ? 1 : 0)][2];
						}

						//add the bevel plane to the expanded hull
						AddHullPlane(hull,normal,origin,true); //double check that this edge hasn't been added yet
					}
				} //end for loop (check for each direction)
			} //end for loop (over all edges in face)
		} //end for loop (over all faces in hull 0)
	} //end if completely non-axial

	//add the bounding box to the expanded hull -- for a
	//completely axial brush, this is the only necessary step

	//add mins
	VectorAdd(brush->hulls[0].bounds.m_Mins, g_hull_size[hullnum][0], origin);
	normal[0] = -1;
	normal[1] = 0;
	normal[2] = 0;
	AddHullPlane(hull,normal,(axialbevel[plane_x][0] ? brush->hulls[0].bounds.m_Mins : origin),false);
	normal[0] = 0;
	normal[1] = -1;
	AddHullPlane(hull,normal,(axialbevel[plane_y][0] ? brush->hulls[0].bounds.m_Mins : origin),false);
	normal[1] = 0;
	normal[2] = -1;
	AddHullPlane(hull,normal,(axialbevel[plane_z][0] ? brush->hulls[0].bounds.m_Mins : origin),false);

	normal[2] = 0;

	//add maxes
	VectorAdd(brush->hulls[0].bounds.m_Maxs, g_hull_size[hullnum][1], origin);
	normal[0] = 1;
	AddHullPlane(hull,normal,(axialbevel[plane_x][1] ? brush->hulls[0].bounds.m_Maxs : origin),false);
	normal[0] = 0;
	normal[1] = 1;
	AddHullPlane(hull,normal,(axialbevel[plane_y][1] ? brush->hulls[0].bounds.m_Maxs : origin),false);
	normal[1] = 0;
	normal[2] = 1;
	AddHullPlane(hull,normal,(axialbevel[plane_z][1] ? brush->hulls[0].bounds.m_Maxs : origin),false);
/*
	bface_t* hull_face; //sanity check

	for(hull_face = hull->faces; hull_face; hull_face = hull_face->next)
	{
		for(current_face = brush->hulls[0].faces; current_face; current_face = current_face->next)
		{
			if(current_face->w->m_NumPoints < 3)
			{ continue; }
			for(counter = 0; counter < current_face->w->m_NumPoints; counter++)
			{
				if(DotProduct(hull_face->plane->normal,hull_face->plane->origin) < DotProduct(hull_face->plane->normal,current_face->w->m_Points[counter]))
				{
					Warning("Illegal Brush (clip hull [%i] has backward face): Entity %i, Brush %i\n",hullnum,brush->entitynum, brush->brushnum);
					break;
				}
			}
		}
	}
*/
}
#else //!HLCSG_PRECISIONCLIP

#define	MAX_HULL_POINTS	32
#define	MAX_HULL_EDGES	64

typedef struct
{
    brush_t*        b;
    int             hullnum;
    int             num_hull_points;
    vec3_t          hull_points[MAX_HULL_POINTS];
    vec3_t          hull_corners[MAX_HULL_POINTS * 8];
    int             num_hull_edges;
    int             hull_edges[MAX_HULL_EDGES][2];
} expand_t;

/*
 * =============
 * IPlaneEquiv
 *
 * =============
 */
bool            IPlaneEquiv(const plane_t* const p1, const plane_t* const p2)
{
    vec_t           t;
    int             j;

    // see if origin is on plane
    t = 0;
    for (j = 0; j < 3; j++)
    {
        t += (p2->origin[j] - p1->origin[j]) * p2->normal[j];
    }
    if (fabs(t) > DIST_EPSILON)
    {
        return false;
    }

    // see if the normal is forward, backwards, or off
    for (j = 0; j < 3; j++)
    {
        if (fabs(p2->normal[j] - p1->normal[j]) > NORMAL_EPSILON)
        {
            break;
        }
    }
    if (j == 3)
    {
        return true;
    }

    for (j = 0; j < 3; j++)
    {
        if (fabs(p2->normal[j] - p1->normal[j]) > NORMAL_EPSILON)
        {
            break;
        }
    }
    if (j == 3)
    {
        return true;
    }

    return false;
}

/*
 * ============
 * AddBrushPlane
 * =============
 */
void            AddBrushPlane(const expand_t* const ex, const plane_t* const plane)
{
    plane_t*        pl;
    bface_t*        f;
    bface_t*        nf;
    brushhull_t*    h;

    h = &ex->b->hulls[ex->hullnum];
    // see if the plane has allready been added
    for (f = h->faces; f; f = f->next)
    {
        pl = f->plane;
        if (IPlaneEquiv(plane, pl))
        {
            return;
        }
    }

    nf = (bface_t*)Alloc(sizeof(*nf));                               // TODO: This leaks
    nf->planenum = FindIntPlane(plane->normal, plane->origin);
    nf->plane = &g_mapplanes[nf->planenum];
    nf->next = h->faces;
    nf->contents = CONTENTS_EMPTY;
    h->faces = nf;

    nf->texinfo = 0;                                       // all clip hulls have same texture
}

// =====================================================================================
//  ExpandBrush
// =====================================================================================
void            ExpandBrush(brush_t* b, const int hullnum)
{
    int             x;
    int             s;
    int             corner;
    bface_t*        brush_faces;
    bface_t*        f;
    bface_t*        nf;
    plane_t*        p;
    plane_t         plane;
    vec3_t          origin;
    vec3_t          normal;
    expand_t        ex;
    brushhull_t*    h;
    bool            axial;

    brush_faces = b->hulls[0].faces;
    h = &b->hulls[hullnum];

    ex.b = b;
    ex.hullnum = hullnum;
    ex.num_hull_points = 0;
    ex.num_hull_edges = 0;

    // expand all of the planes

    axial = true;

    // for each of this brushes faces
    for (f = brush_faces; f; f = f->next)
    {
        p = f->plane;
        if (p->type > last_axial) // ajm: last_axial == (planetypes enum)plane_z == (2)
        {
            axial = false;                                 // not an xyz axial plane
        }

        VectorCopy(p->origin, origin);
        VectorCopy(p->normal, normal);

        for (x = 0; x < 3; x++)
        {
            if (p->normal[x] > 0)
            {
                corner = g_hull_size[hullnum][1][x];
            }
            else if (p->normal[x] < 0)
            {
                corner = -g_hull_size[hullnum][0][x];
            }
            else
            {
                corner = 0;
            }
            origin[x] += p->normal[x] * corner;
        }
        nf = (bface_t*)Alloc(sizeof(*nf));                           // TODO: This leaks

        nf->planenum = FindIntPlane(normal, origin);
        nf->plane = &g_mapplanes[nf->planenum];
        nf->next = h->faces;
        nf->contents = CONTENTS_EMPTY;
        h->faces = nf;
        nf->texinfo = 0;                        // all clip hulls have same texture
//        nf->texinfo = f->texinfo;               // Hack to view clipping hull with textures (might crash halflife)
    }

    // if this was an axial brush, we are done
    if (axial)
    {
        return;
    }

    // add any axis planes not contained in the brush to bevel off corners
    for (x = 0; x < 3; x++)
    {
        for (s = -1; s <= 1; s += 2)
        {
            // add the plane
            VectorCopy(vec3_origin, plane.normal);
            plane.normal[x] = s;
            if (s == -1)
            {
                VectorAdd(b->hulls[0].bounds.m_Mins, g_hull_size[hullnum][0], plane.origin);
            }
            else
            {
                VectorAdd(b->hulls[0].bounds.m_Maxs, g_hull_size[hullnum][1], plane.origin);
            }
            AddBrushPlane(&ex, &plane);
        }
    }
}

#endif //HLCSG_PRECISECLIP

// =====================================================================================
//  MakeHullFaces
// =====================================================================================
void            MakeHullFaces(const brush_t* const b, brushhull_t *h)
{
    bface_t*        f;
    bface_t*        f2;
#ifdef HLCSG_PRECISECLIP
	bool warned = false;
#endif

restart:
    h->bounds.reset();

    // for each face in this brushes hull
    for (f = h->faces; f; f = f->next)
    {
        Winding* w = new Winding(f->plane->normal, f->plane->dist);
        for (f2 = h->faces; f2; f2 = f2->next)
        {
            if (f == f2)
            {
                continue;
            }
            const plane_t* p = &g_mapplanes[f2->planenum ^ 1];
            if (!w->Chop(p->normal, p->dist))   // Nothing left to chop (getArea will return 0 for us in this case for below)
            {
                break;
            }
        }
        if (w->getArea() < 0.1)
        {
#ifdef HLCSG_PRECISECLIP
			if(w->getArea() == 0 && !warned) //warn user when there's a bad brush (face not contributing)
			{
				Warning("Illegal Brush (plane doesn't contribute to final shape): Entity %i, Brush %i\n",b->entitynum, b->brushnum);
				warned = true;
			}
#endif
            delete w;
            if (h->faces == f)
            {
                h->faces = f->next;
            }
            else
            {
                for (f2 = h->faces; f2->next != f; f2 = f2->next)
                {
                    ;
                }
                f2->next = f->next;
            }
            goto restart;
        }
        else
        {
            f->w = w;
            f->contents = CONTENTS_EMPTY;
            unsigned int    i;
            for (i = 0; i < w->m_NumPoints; i++)
            {
                h->bounds.add(w->m_Points[i]);
            }
        }
    }

    unsigned int    i;
    for (i = 0; i < 3; i++)
    {
        if (h->bounds.m_Mins[i] < -BOGUS_RANGE / 2 || h->bounds.m_Maxs[i] > BOGUS_RANGE / 2)
        {
            Fatal(assume_BRUSH_OUTSIDE_WORLD, "Entity %i, Brush %i: outside world(+/-%d): (%.0f,%.0f,%.0f)-(%.0f,%.0f,%.0f)",
                  b->entitynum, b->brushnum,
                  BOGUS_RANGE / 2,
                  h->bounds.m_Mins[0], h->bounds.m_Mins[1], h->bounds.m_Mins[2],
                  h->bounds.m_Maxs[0], h->bounds.m_Maxs[1], h->bounds.m_Maxs[2]);
        }
    }
}

// =====================================================================================
//  MakeBrushPlanes
// =====================================================================================
bool            MakeBrushPlanes(brush_t* b)
{
    int             i;
    int             j;
    int             planenum;
    side_t*         s;
    bface_t*        f;
    vec3_t          origin;

    //
    // if the origin key is set (by an origin brush), offset all of the values
    //
    GetVectorForKey(&g_entities[b->entitynum], "origin", origin);

    //
    // convert to mapplanes
    //
    // for each side in this brush
    for (i = 0; i < b->numsides; i++)
    {
        s = &g_brushsides[b->firstside + i];
        for (j = 0; j < 3; j++)
        {
            VectorSubtract(s->planepts[j], origin, s->planepts[j]);
        }
        planenum = PlaneFromPoints(s->planepts[0], s->planepts[1], s->planepts[2]);
        if (planenum == -1)
        {
            Fatal(assume_PLANE_WITH_NO_NORMAL, "Entity %i, Brush %i, Side %i: plane with no normal", b->entitynum, b->brushnum, i);
        }

        //
        // see if the plane has been used already
        //
        for (f = b->hulls[0].faces; f; f = f->next)
        {
            if (f->planenum == planenum || f->planenum == (planenum ^ 1))
            {
                Fatal(assume_BRUSH_WITH_COPLANAR_FACES, "Entity %i, Brush %i, Side %i: has a coplanar plane at (%.0f, %.0f, %.0f), texture %s",
                      b->entitynum, b->brushnum, i, s->planepts[0][0] + origin[0], s->planepts[0][1] + origin[1],
                      s->planepts[0][2] + origin[2], s->td.name);
            }
        }

        f = (bface_t*)Alloc(sizeof(*f));                             // TODO: This leaks

        f->planenum = planenum;
        f->plane = &g_mapplanes[planenum];
        f->next = b->hulls[0].faces;
        b->hulls[0].faces = f;
        f->texinfo = g_onlyents ? 0 : TexinfoForBrushTexture(f->plane, &s->td, origin);
    }

    return true;
}


// =====================================================================================
//  TextureContents
// =====================================================================================
static contents_t TextureContents(const char* const name)
{
    if (!strncasecmp(name, "sky", 3))
        return CONTENTS_SKY;

// =====================================================================================
//Cpt_Andrew - Env_Sky Check
// =====================================================================================
    if (!strncasecmp(name, "env_sky", 3))
        return CONTENTS_SKY;
// =====================================================================================

    if (!strncasecmp(name + 1, "!lava", 5))
        return CONTENTS_LAVA;

    if (!strncasecmp(name + 1, "!slime", 6))
        return CONTENTS_SLIME;

    if (name[0] == '!') //optimized -- don't check for current unless it's liquid (KGP)
	{
		if (!strncasecmp(name, "!cur_90", 7))
			return CONTENTS_CURRENT_90;
		if (!strncasecmp(name, "!cur_0", 6))
			return CONTENTS_CURRENT_0;
		if (!strncasecmp(name, "!cur_270", 8))
			return CONTENTS_CURRENT_270;
		if (!strncasecmp(name, "!cur_180", 8))
			return CONTENTS_CURRENT_180;
		if (!strncasecmp(name, "!cur_up", 7))
			return CONTENTS_CURRENT_UP;
		if (!strncasecmp(name, "!cur_dwn", 8))
			return CONTENTS_CURRENT_DOWN;
        return CONTENTS_WATER; //default for liquids
	}

    if (!strncasecmp(name, "origin", 6))
        return CONTENTS_ORIGIN;

    if (!strncasecmp(name, "clip", 4))
        return CONTENTS_CLIP;

    if (!strncasecmp(name, "hint", 4))
        return CONTENTS_HINT;
    if (!strncasecmp(name, "skip", 4))
        return CONTENTS_HINT;

    if (!strncasecmp(name, "translucent", 11))
        return CONTENTS_TRANSLUCENT;

    if (name[0] == '@')
        return CONTENTS_TRANSLUCENT;

#ifdef ZHLT_NULLTEX // AJM:
	if (!strncasecmp(name, "null", 4))
        return CONTENTS_NULL;
#ifdef HLCSG_PRECISIONCLIP // KGP
	if(!strncasecmp(name,"bevel",5))
		return CONTENTS_NULL;
#endif //precisionclip
#endif //nulltex

    return CONTENTS_SOLID;
}

// =====================================================================================
//  ContentsToString
// =====================================================================================
const char*     ContentsToString(const contents_t type)
{
    switch (type)
    {
    case CONTENTS_EMPTY:
        return "EMPTY";
    case CONTENTS_SOLID:
        return "SOLID";
    case CONTENTS_WATER:
        return "WATER";
    case CONTENTS_SLIME:
        return "SLIME";
    case CONTENTS_LAVA:
        return "LAVA";
    case CONTENTS_SKY:
        return "SKY";
    case CONTENTS_ORIGIN:
        return "ORIGIN";
    case CONTENTS_CLIP:
        return "CLIP";
    case CONTENTS_CURRENT_0:
        return "CURRENT_0";
    case CONTENTS_CURRENT_90:
        return "CURRENT_90";
    case CONTENTS_CURRENT_180:
        return "CURRENT_180";
    case CONTENTS_CURRENT_270:
        return "CURRENT_270";
    case CONTENTS_CURRENT_UP:
        return "CURRENT_UP";
    case CONTENTS_CURRENT_DOWN:
        return "CURRENT_DOWN";
    case CONTENTS_TRANSLUCENT:
        return "TRANSLUCENT";
    case CONTENTS_HINT:
        return "HINT";

#ifdef ZHLT_NULLTEX // AJM
    case CONTENTS_NULL:
        return "NULL";
#endif

#ifdef ZHLT_DETAIL // AJM
    case CONTENTS_DETAIL:
        return "DETAIL";
#endif

    default:
        return "UNKNOWN";
    }
}

// =====================================================================================
//  CheckBrushContents
//      Perfoms abitrary checking on brush surfaces and states to try and catch errors
// =====================================================================================
contents_t      CheckBrushContents(const brush_t* const b)
{
    contents_t      best_contents;
    contents_t      contents;
    side_t*         s;
    int             i;

    s = &g_brushsides[b->firstside];

    // cycle though the sides of the brush and attempt to get our best side contents for
    //  determining overall brush contents
    best_contents = TextureContents(s->td.name);
    s++;
    for (i = 1; i < b->numsides; i++, s++)
    {
        contents_t contents_consider = TextureContents(s->td.name);
        if (contents_consider > best_contents)
        {
            // if our current surface contents is better (larger) than our best, make it our best.
            best_contents = contents_consider;
        }
    }
    contents = best_contents;

    // attempt to pick up on mixed_face_contents errors
    s = &g_brushsides[b->firstside];
    s++;
    for (i = 1; i < b->numsides; i++, s++)
    {
        contents_t contents2 = TextureContents(s->td.name);

        // AJM: sky and null types are not to cause mixed face contents
        if (contents2 == CONTENTS_SKY)
            continue;

#ifdef ZHLT_NULLTEX
        if (contents2 == CONTENTS_NULL)
            continue;
#endif

        if (contents2 != best_contents)
        {
            Fatal(assume_MIXED_FACE_CONTENTS, "Entity %i, Brush %i: mixed face contents\n    Texture %s and %s",
                b->entitynum, b->brushnum, g_brushsides[b->firstside].td.name, s->td.name);
        }
    }

    // check to make sure we dont have an origin brush as part of worldspawn
    if ((b->entitynum == 0) || (strcmp("func_group", ValueForKey(&g_entities[b->entitynum], "classname"))==0))
    {
        if (contents == CONTENTS_ORIGIN)
        {
            Fatal(assume_BRUSH_NOT_ALLOWED_IN_WORLD, "Entity %i, Brush %i: %s brushes not allowed in world\n(did you forget to tie this origin brush to a rotating entity?)", b->entitynum, b->brushnum, ContentsToString(contents));
        }
    }
    else
    {
        // otherwise its not worldspawn, therefore its an entity. check to make sure this brush is allowed
        //  to be an entity.
        switch (contents)
        {
        case CONTENTS_SOLID:
        case CONTENTS_WATER:
        case CONTENTS_SLIME:
        case CONTENTS_LAVA:
        case CONTENTS_ORIGIN:
        case CONTENTS_CLIP:
#ifdef ZHLT_NULLTEX // AJM
        case CONTENTS_NULL:
            break;
#endif
        default:
            Fatal(assume_BRUSH_NOT_ALLOWED_IN_ENTITY, "Entity %i, Brush %i: %s brushes not allowed in entity", b->entitynum, b->brushnum, ContentsToString(contents));
            break;
        }
    }

    return contents;
}


// =====================================================================================
//  CreateBrush
//      makes a brush!
// =====================================================================================
void CreateBrush(const int brushnum)
{
    brush_t*        b;
    int             contents;
    int             h;

    b = &g_mapbrushes[brushnum];

    contents = b->contents;

    if (contents == CONTENTS_ORIGIN)
        return;

    //  HULL 0
    MakeBrushPlanes(b);
    MakeHullFaces(b, &b->hulls[0]);

    // these brush types do not need to be represented in the clipping hull
    switch (contents)
    {
        case CONTENTS_LAVA:
        case CONTENTS_SLIME:
        case CONTENTS_WATER:
        case CONTENTS_TRANSLUCENT:
        case CONTENTS_HINT:
            return;
    }

#ifdef HLCSG_CLIPECONOMY // AJM
    if (b->noclip)
        return;
#endif

    // HULLS 1-3
    if (!g_noclip)
    {
        for (h = 1; h < NUM_HULLS; h++)
        {
			ExpandBrush(b, h);
            MakeHullFaces(b, &b->hulls[h]);
        }
    }

    // clip brushes don't stay in the drawing hull
    if (contents == CONTENTS_CLIP)
    {
        b->hulls[0].faces = NULL;
        b->contents = CONTENTS_SOLID;
    }
}
