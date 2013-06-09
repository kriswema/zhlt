#pragma warning(disable: 4018) // '<' : signed/unsigned mismatch

#include "csg.h"

int             g_nummapbrushes;
brush_t         g_mapbrushes[MAX_MAP_BRUSHES];

int             g_numbrushsides;
side_t          g_brushsides[MAX_MAP_SIDES];

int             g_nMapFileVersion;

static const vec3_t   s_baseaxis[18] = {
    {0, 0, 1}, {1, 0, 0}, {0, -1, 0},                      // floor
    {0, 0, -1}, {1, 0, 0}, {0, -1, 0},                     // ceiling
    {1, 0, 0}, {0, 1, 0}, {0, 0, -1},                      // west wall
    {-1, 0, 0}, {0, 1, 0}, {0, 0, -1},                     // east wall
    {0, 1, 0}, {1, 0, 0}, {0, 0, -1},                      // south wall
    {0, -1, 0}, {1, 0, 0}, {0, 0, -1},                     // north wall
};

// =====================================================================================
//  TextureAxisFromPlane
// =====================================================================================
void            TextureAxisFromPlane(const plane_t* const pln, vec3_t xv, vec3_t yv)
{
    int             bestaxis;
    vec_t           dot, best;
    int             i;

    best = 0;
    bestaxis = 0;

    for (i = 0; i < 6; i++)
    {
        dot = DotProduct(pln->normal, s_baseaxis[i * 3]);
        if (dot > best)
        {
            best = dot;
            bestaxis = i;
        }
    }

    VectorCopy(s_baseaxis[bestaxis * 3 + 1], xv);
    VectorCopy(s_baseaxis[bestaxis * 3 + 2], yv);
}

#define ScaleCorrection	(1.0/128.0)

// =====================================================================================
//  CopySKYtoCLIP
//      clips a particluar sky brush
// =====================================================================================
static void     CopySKYtoCLIP(const brush_t* const b)
{
    int             i;
    entity_t*       mapent;
    brush_t*        newbrush;

    if (b->contents != CONTENTS_SKY)
		Error("[MOD] CopySKYtoCLIP: Got a NON-SKY for passed brush! (%s)",b->contents ); 

    hlassert(b->contents == CONTENTS_SKY);                 // Only SKY brushes should be passed down to this function(sanity check)
    hlassert(b->entitynum == 0);                           // SKY must be in worldspawn entity

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

// =====================================================================================
//  HandleSKYCLIP
//      clips the whole sky, unconditional of g_skyclip
// =====================================================================================
static void     HandleSKYCLIP()
{
    int             i;
    int             last;
    entity_t*       e = &g_entities[0];

    for (i = e->firstbrush, last = e->firstbrush + e->numbrushes; i < last; i++)
    {
        if (g_mapbrushes[i].contents == CONTENTS_SKY)
        {
            CopySKYtoCLIP(&g_mapbrushes[i]);
        }
    }
}

// =====================================================================================
//  CheckForInvisible
//      see if a brush is part of an invisible entity (KGP)
// =====================================================================================
#ifdef HLCSG_NULLIFY_INVISIBLE
static bool CheckForInvisible(entity_t* mapent)
{
	using namespace std;

	string keyval(ValueForKey(mapent,"classname"));
	if(g_invisible_items.count(keyval))
	{ return true; }

	keyval.assign(ValueForKey(mapent,"targetname"));
	if(g_invisible_items.count(keyval))
	{ return true; }

	keyval.assign(ValueForKey(mapent,"zhlt_invisible"));
	if(!keyval.empty() && strcmp(keyval.c_str(),"0"))
	{ return true; }

	return false;
}
#endif
// =====================================================================================
//  ParseBrush
//      parse a brush from script
// =====================================================================================
static contents_t ParseBrush(entity_t* mapent)
{
    brush_t*        b;
    int             i, j;
    side_t*         side;
    contents_t      contents;
    bool            ok;
#ifdef HLCSG_NULLIFY_INVISIBLE // KGP
	bool nullify = CheckForInvisible(mapent);
#endif
    hlassume(g_nummapbrushes < MAX_MAP_BRUSHES, assume_MAX_MAP_BRUSHES);

    b = &g_mapbrushes[g_nummapbrushes];
    g_nummapbrushes++;
    b->firstside = g_numbrushsides;
    b->entitynum = g_numentities - 1;
    b->brushnum = g_nummapbrushes - mapent->firstbrush - 1;

#ifdef HLCSG_CLIPECONOMY // AJM
    b->noclip = 0;
#endif

    mapent->numbrushes++;

	ok = GetToken(true);
    while (ok)
    {
        g_TXcommand = 0;
        if (!strcmp(g_token, "}"))
        {
            break;
        }

        hlassume(g_numbrushsides < MAX_MAP_SIDES, assume_MAX_MAP_SIDES);
        side = &g_brushsides[g_numbrushsides];
        g_numbrushsides++;

        b->numsides++;

        // read the three point plane definition
        for (i = 0; i < 3; i++)
        {
            if (i != 0)
            {
                GetToken(true);
            }
            if (strcmp(g_token, "("))
            {
                Error("Parsing Entity %i, Brush %i, Side %i : Expecting '(' got '%s'",
                      b->entitynum, b->brushnum, b->numsides, g_token);
            }

            for (j = 0; j < 3; j++)
            {
                GetToken(false);
                side->planepts[i][j] = atof(g_token);
            }

            GetToken(false);
            if (strcmp(g_token, ")"))
            {
                Error("Parsing	Entity %i, Brush %i, Side %i : Expecting ')' got '%s'",
                      b->entitynum, b->brushnum, b->numsides, g_token);
            }
        }

        // read the     texturedef
        GetToken(false);
        _strupr(g_token);
#ifdef HLCSG_NULLIFY_INVISIBLE
		if(nullify && strncmp(g_token,"BEVEL",5) && strncmp(g_token,"ORIGIN",6))
		{ safe_strncpy(g_token,"NULL",sizeof(g_token)); }
#endif
        safe_strncpy(side->td.name, g_token, sizeof(side->td.name));

        if (g_nMapFileVersion < 220)                       // Worldcraft 2.1-, Radiant
        {
            GetToken(false);
            side->td.vects.valve.shift[0] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.shift[1] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.rotate = atof(g_token);
            GetToken(false);
            side->td.vects.valve.scale[0] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.scale[1] = atof(g_token);
        }
        else                                               // Worldcraft 2.2+
        {
            // texture U axis
            GetToken(false);
            if (strcmp(g_token, "["))
            {
                hlassume(false, assume_MISSING_BRACKET_IN_TEXTUREDEF);
            }

            GetToken(false);
            side->td.vects.valve.UAxis[0] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.UAxis[1] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.UAxis[2] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.shift[0] = atof(g_token);

            GetToken(false);
            if (strcmp(g_token, "]"))
            {
                Error("missing ']' in texturedef (U)");
            }

            // texture V axis
            GetToken(false);
            if (strcmp(g_token, "["))
            {
                Error("missing '[' in texturedef (V)");
            }

            GetToken(false);
            side->td.vects.valve.VAxis[0] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.VAxis[1] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.VAxis[2] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.shift[1] = atof(g_token);

            GetToken(false);
            if (strcmp(g_token, "]"))
            {
                Error("missing ']' in texturedef (V)");
            }

            // Texture rotation is implicit in U/V axes.
            GetToken(false);
            side->td.vects.valve.rotate = 0;

            // texure scale
            GetToken(false);
            side->td.vects.valve.scale[0] = atof(g_token);
            GetToken(false);
            side->td.vects.valve.scale[1] = atof(g_token);
        }

        ok = GetToken(true);                               // Done with line, this reads the first item from the next line

        if ((g_TXcommand == '1' || g_TXcommand == '2'))
        {
            // We are QuArK mode and need to translate some numbers to align textures its way
            // from QuArK, the texture vectors are given directly from the three points
            vec3_t          TexPt[2];
            int             k;
            float           dot22, dot23, dot33, mdet, aa, bb, dd;

            k = g_TXcommand - '0';
            for (j = 0; j < 3; j++)
            {
                TexPt[1][j] = (side->planepts[k][j] - side->planepts[0][j]) * ScaleCorrection;
            }
            k = 3 - k;
            for (j = 0; j < 3; j++)
            {
                TexPt[0][j] = (side->planepts[k][j] - side->planepts[0][j]) * ScaleCorrection;
            }

            dot22 = DotProduct(TexPt[0], TexPt[0]);
            dot23 = DotProduct(TexPt[0], TexPt[1]);
            dot33 = DotProduct(TexPt[1], TexPt[1]);
            mdet = dot22 * dot33 - dot23 * dot23;
            if (mdet < 1E-6 && mdet > -1E-6)
            {
                aa = bb = dd = 0;
                Warning
                    ("Degenerate QuArK-style brush texture : Entity %i, Brush %i @ (%f,%f,%f) (%f,%f,%f)	(%f,%f,%f)",
                     b->entitynum, b->brushnum, side->planepts[0][0], side->planepts[0][1], side->planepts[0][2],
                     side->planepts[1][0], side->planepts[1][1], side->planepts[1][2], side->planepts[2][0],
                     side->planepts[2][1], side->planepts[2][2]);
            }
            else
            {
                mdet = 1.0 / mdet;
                aa = dot33 * mdet;
                bb = -dot23 * mdet;
                //cc = -dot23*mdet;             // cc = bb
                dd = dot22 * mdet;
            }

            for (j = 0; j < 3; j++)
            {
                side->td.vects.quark.vects[0][j] = aa * TexPt[0][j] + bb * TexPt[1][j];
                side->td.vects.quark.vects[1][j] = -( /*cc */ bb * TexPt[0][j] + dd * TexPt[1][j]);
            }

            side->td.vects.quark.vects[0][3] = -DotProduct(side->td.vects.quark.vects[0], side->planepts[0]);
            side->td.vects.quark.vects[1][3] = -DotProduct(side->td.vects.quark.vects[1], side->planepts[0]);
        }

        side->td.txcommand = g_TXcommand;                  // Quark stuff, but needs setting always
    };

    b->contents = contents = CheckBrushContents(b);

    //
    // origin brushes are removed, but they set
    // the rotation origin for the rest of the brushes
    // in the entity
    //

    if (contents == CONTENTS_ORIGIN)
    {
        char            string[MAXTOKEN];
        vec3_t          origin;

        b->contents = CONTENTS_SOLID;
        CreateBrush(mapent->firstbrush + b->brushnum);     // to get sizes
        b->contents = contents;

        for (i = 0; i < NUM_HULLS; i++)
        {
            b->hulls[i].faces = NULL;
        }

        if (b->entitynum != 0)  // Ignore for WORLD (code elsewhere enforces no ORIGIN in world message)
        {
            VectorAdd(b->hulls[0].bounds.m_Mins, b->hulls[0].bounds.m_Maxs, origin);
            VectorScale(origin, 0.5, origin);
    
            safe_snprintf(string, MAXTOKEN, "%i %i %i", (int)origin[0], (int)origin[1], (int)origin[2]);
            SetKeyValue(&g_entities[b->entitynum], "origin", string);
        }
    }

    return contents;
}


// =====================================================================================
//  ParseMapEntity
//      parse an entity from script
// =====================================================================================
bool            ParseMapEntity()
{
    bool            all_clip = true;
    int             this_entity;
    entity_t*       mapent;
    epair_t*        e;

    if (!GetToken(true))
    {
        return false;
    }

    this_entity = g_numentities;

    if (strcmp(g_token, "{"))
    {
        Error("Parsing Entity %i, expected '{' got '%s'", this_entity, g_token);
    }

    hlassume(g_numentities < MAX_MAP_ENTITIES, assume_MAX_MAP_ENTITIES);
    g_numentities++;

    mapent = &g_entities[this_entity];
    mapent->firstbrush = g_nummapbrushes;
    mapent->numbrushes = 0;

    while (1)
    {
        if (!GetToken(true))
            Error("ParseEntity: EOF without closing brace");

        if (!strcmp(g_token, "}"))  // end of our context
            break;

        if (!strcmp(g_token, "{"))  // must be a brush
        {
            contents_t contents = ParseBrush(mapent);

            if ((contents != CONTENTS_CLIP) && (contents != CONTENTS_ORIGIN))
                all_clip = false;
        }
        else                        // else assume an epair
        {
            e = ParseEpair();

            if (!strcmp(e->key, "mapversion"))
            {
                g_nMapFileVersion = atoi(e->value);
            }

            e->next = mapent->epairs;
            mapent->epairs = e;
        }
    }


    if (mapent->numbrushes && all_clip)
        Fatal(assume_NO_VISIBILE_BRUSHES, "Entity %i has no visible brushes\n", this_entity);

    CheckFatal();

    
#ifdef ZHLT_DETAIL // AJM
    if (!strcmp(ValueForKey(mapent, "classname"), "info_detail") && g_bDetailBrushes && this_entity != 0)
    {
        // mark all of the brushes in this entity as contents_detail
        for (int i = mapent->firstbrush; i < mapent->firstbrush + mapent->numbrushes; i++)
        {
            g_mapbrushes[i].contents = CONTENTS_DETAIL;
        }

        // move these brushes to worldspawn
        {
            brush_t*        temp;
            int             newbrushes;
            int             worldbrushes;
            int             i;

            newbrushes = mapent->numbrushes;
            worldbrushes = g_entities[0].numbrushes;

            temp = (brush_t*)Alloc(newbrushes * sizeof(brush_t));
            memcpy(temp, g_mapbrushes + mapent->firstbrush, newbrushes * sizeof(brush_t));

            for (i = 0; i < newbrushes; i++)
            {
                temp[i].entitynum = 0;
            }

            // make space to move the brushes (overlapped copy)
            memmove(g_mapbrushes + worldbrushes + newbrushes,
                    g_mapbrushes + worldbrushes, sizeof(brush_t) * (g_nummapbrushes - worldbrushes - newbrushes));

            // copy the new brushes down
            memcpy(g_mapbrushes + worldbrushes, temp, sizeof(brush_t) * newbrushes);

            // fix up indexes
            g_numentities--;
            g_entities[0].numbrushes += newbrushes;
            for (i = 1; i < g_numentities; i++)
            {
                g_entities[i].firstbrush += newbrushes;
            }
            memset(mapent, 0, sizeof(*mapent));
            Free(temp);
        }

        // delete this entity
        g_numentities--;
        return true;
    }
#endif


#ifdef ZHLT_INFO_COMPILE_PARAMETERS // AJM
    if (!strcmp(ValueForKey(mapent, "classname"), "info_compile_parameters"))
    {
        GetParamsFromEnt(mapent);
    }
#endif

    // if its the worldspawn entity and we need to skyclip, then do it
    if ((this_entity == 0) && g_skyclip)                  // first entitiy
    {
        HandleSKYCLIP();
    }

    // if the given entity only has one brush and its an origin brush
    if ((mapent->numbrushes == 1) && (g_mapbrushes[mapent->firstbrush].contents == CONTENTS_ORIGIN))
    {
        brushhull_t*    hull = g_mapbrushes[mapent->firstbrush].hulls;

        Error("Entity %i, contains ONLY an origin brush near (%.0f,%.0f,%.0f)\n",
              this_entity, hull->bounds.m_Mins[0], hull->bounds.m_Mins[1], hull->bounds.m_Mins[2]);
    }

    GetVectorForKey(mapent, "origin", mapent->origin);

    // group entities are just for editor convenience
    // toss all brushes into the world entity
    if (!g_onlyents && !strcmp("func_group", ValueForKey(mapent, "classname")))
    {
        // this is pretty gross, because the brushes are expected to be
        // in linear order for each entity
        brush_t*        temp;
        int             newbrushes;
        int             worldbrushes;
        int             i;

        newbrushes = mapent->numbrushes;
        worldbrushes = g_entities[0].numbrushes;

        temp = (brush_t*)Alloc(newbrushes * sizeof(brush_t));
        memcpy(temp, g_mapbrushes + mapent->firstbrush, newbrushes * sizeof(brush_t));

        for (i = 0; i < newbrushes; i++)
        {
            temp[i].entitynum = 0;
        }

        // make space to move the brushes (overlapped copy)
        memmove(g_mapbrushes + worldbrushes + newbrushes,
                g_mapbrushes + worldbrushes, sizeof(brush_t) * (g_nummapbrushes - worldbrushes - newbrushes));

        // copy the new brushes down
        memcpy(g_mapbrushes + worldbrushes, temp, sizeof(brush_t) * newbrushes);

        // fix up indexes
        g_numentities--;
        g_entities[0].numbrushes += newbrushes;
        for (i = 1; i < g_numentities; i++)
        {
            g_entities[i].firstbrush += newbrushes;
        }
        memset(mapent, 0, sizeof(*mapent));
        Free(temp);
    }

    return true;
}

// =====================================================================================
//  CountEngineEntities
// =====================================================================================
unsigned int    CountEngineEntities()
{
    unsigned int x;
    unsigned num_engine_entities = 0;
    entity_t*       mapent = g_entities;

    // for each entity in the map
    for (x=0; x<g_numentities; x++, mapent++)
    {
        const char* classname = ValueForKey(mapent, "classname");

        // if its a light_spot or light_env, dont include it as an engine entity!
        if (classname)
        {
            if (   !strncasecmp(classname, "light", 5) 
                || !strncasecmp(classname, "light_spot", 10) 
                || !strncasecmp(classname, "light_environment", 17)
               )
            {
                const char* style = ValueForKey(mapent, "style");
                const char* targetname = ValueForKey(mapent, "targetname");

                // lightspots and lightenviroments dont have a targetname or style
                if (!strlen(targetname) && !atoi(style))
                {
                    continue;
                }
            }
        }

        num_engine_entities++;
    }

    return num_engine_entities;
}

// =====================================================================================
//  LoadMapFile
//      wrapper for LoadScriptFile
//      parse in script entities
// =====================================================================================
const char*     ContentsToString(const contents_t type);

void            LoadMapFile(const char* const filename)
{
    unsigned num_engine_entities;

    LoadScriptFile(filename);

    g_numentities = 0;

    while (ParseMapEntity())
    {
    }

    // AJM debug
    /*
    for (int i = 0; i < g_numentities; i++)
    {
        Log("entity: %i - %i brushes - %s\n", i, g_entities[i].numbrushes, ValueForKey(&g_entities[i], "classname"));
    }
    Log("total entities: %i\ntotal brushes: %i\n\n", g_numentities, g_nummapbrushes);

    for (i = g_entities[0].firstbrush; i < g_entities[0].firstbrush + g_entities[0].numbrushes; i++)
    {
        Log("worldspawn brush %i: contents %s\n", i, ContentsToString((contents_t)g_mapbrushes[i].contents)); 
    }
    */

    num_engine_entities = CountEngineEntities();

    hlassume(num_engine_entities < MAX_ENGINE_ENTITIES, assume_MAX_ENGINE_ENTITIES);

    CheckFatal();

    Verbose("Load map:%s\n", filename);
    Verbose("%5i brushes\n", g_nummapbrushes);
    Verbose("%5i map entities \n", g_numentities - num_engine_entities);
    Verbose("%5i engine entities\n", num_engine_entities);

    // AJM: added in 
#ifdef HLCSG_AUTOWAD
    GetUsedTextures();
#endif
}
