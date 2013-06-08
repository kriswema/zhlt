#include "cmdlib.h"
#include "filelib.h"
#include "messages.h"
#include "hlassert.h"
#include "log.h"
#include "mathlib.h"
#include "bspfile.h"
#include "scriplib.h"
#include "blockmem.h"

//=============================================================================

int             g_max_map_miptex = DEFAULT_MAX_MAP_MIPTEX;
int				g_max_map_lightdata = DEFAULT_MAX_MAP_LIGHTDATA;

int             g_nummodels;
dmodel_t        g_dmodels[MAX_MAP_MODELS];
int             g_dmodels_checksum;

int             g_visdatasize;
byte            g_dvisdata[MAX_MAP_VISIBILITY];
int             g_dvisdata_checksum;

int             g_lightdatasize;
byte*           g_dlightdata;
int             g_dlightdata_checksum;

int             g_texdatasize;
byte*           g_dtexdata;                                  // (dmiptexlump_t)
int             g_dtexdata_checksum;

int             g_entdatasize;
char            g_dentdata[MAX_MAP_ENTSTRING];
int             g_dentdata_checksum;

int             g_numleafs;
dleaf_t         g_dleafs[MAX_MAP_LEAFS];
int             g_dleafs_checksum;

int             g_numplanes;
dplane_t        g_dplanes[MAX_INTERNAL_MAP_PLANES];
int             g_dplanes_checksum;

int             g_numvertexes;
dvertex_t       g_dvertexes[MAX_MAP_VERTS];
int             g_dvertexes_checksum;

int             g_numnodes;
dnode_t         g_dnodes[MAX_MAP_NODES];
int             g_dnodes_checksum;

int             g_numtexinfo;
texinfo_t       g_texinfo[MAX_MAP_TEXINFO];
int             g_texinfo_checksum;

int             g_numfaces;
dface_t         g_dfaces[MAX_MAP_FACES];
int             g_dfaces_checksum;

int             g_numclipnodes;
dclipnode_t     g_dclipnodes[MAX_MAP_CLIPNODES];
int             g_dclipnodes_checksum;

int             g_numedges;
dedge_t         g_dedges[MAX_MAP_EDGES];
int             g_dedges_checksum;

int             g_nummarksurfaces;
unsigned short  g_dmarksurfaces[MAX_MAP_MARKSURFACES];
int             g_dmarksurfaces_checksum;

int             g_numsurfedges;
int             g_dsurfedges[MAX_MAP_SURFEDGES];
int             g_dsurfedges_checksum;

int             g_numentities;
entity_t        g_entities[MAX_MAP_ENTITIES];

/*
 * ===============
 * FastChecksum
 * ===============
 */

static int      FastChecksum(const void* const buffer, int bytes)
{
    int             checksum = 0;
    char*           buf = (char*)buffer;

    while (bytes--)
    {
        checksum = rotl(checksum, 4) ^ (*buf);
        buf++;
    }

    return checksum;
}

/*
 * ===============
 * CompressVis
 * ===============
 */
int             CompressVis(const byte* const src, const unsigned int src_length, byte* dest, unsigned int dest_length)
{
    unsigned int    j;
    byte*           dest_p = dest;
    unsigned int    current_length = 0;

    for (j = 0; j < src_length; j++)
    {
        current_length++;
        hlassume(current_length <= dest_length, assume_COMPRESSVIS_OVERFLOW);

        *dest_p = src[j];
        dest_p++;

        if (src[j])
        {
            continue;
        }

        unsigned char   rep = 1;

        for (j++; j < src_length; j++)
        {
            if (src[j] || rep == 255)
            {
                break;
            }
            else
            {
                rep++;
            }
        }
        current_length++;
        hlassume(current_length <= dest_length, assume_COMPRESSVIS_OVERFLOW);

        *dest_p = rep;
        dest_p++;
        j--;
    }

    return dest_p - dest;
}

// =====================================================================================
//  DecompressVis
//      
// =====================================================================================
void            DecompressVis(const byte* src, byte* const dest, const unsigned int dest_length)
{
    unsigned int    current_length = 0;
    int             c;
    byte*           out;
    int             row;

    row = (g_numleafs + 7) >> 3;
    out = dest;

    do
    {
        if (*src)
        {
            current_length++;
            hlassume(current_length <= dest_length, assume_DECOMPRESSVIS_OVERFLOW);

            *out = *src;
            out++;
            src++;
            continue;
        }

        c = src[1];
        src += 2;
        while (c)
        {
            current_length++;
            hlassume(current_length <= dest_length, assume_DECOMPRESSVIS_OVERFLOW);

            *out = 0;
            out++;
            c--;

            if (out - dest >= row)
            {
                return;
            }
        }
    }
    while (out - dest < row);
}

//
// =====================================================================================
//

// =====================================================================================
//  SwapBSPFile
//      byte swaps all data in a bsp file
// =====================================================================================
static void     SwapBSPFile(const bool todisk)
{
    int             i, j, c;
    dmodel_t*       d;
    dmiptexlump_t*  mtl;

    // models       
    for (i = 0; i < g_nummodels; i++)
    {
        d = &g_dmodels[i];

        for (j = 0; j < MAX_MAP_HULLS; j++)
        {
            d->headnode[j] = LittleLong(d->headnode[j]);
        }

        d->visleafs = LittleLong(d->visleafs);
        d->firstface = LittleLong(d->firstface);
        d->numfaces = LittleLong(d->numfaces);

        for (j = 0; j < 3; j++)
        {
            d->mins[j] = LittleFloat(d->mins[j]);
            d->maxs[j] = LittleFloat(d->maxs[j]);
            d->origin[j] = LittleFloat(d->origin[j]);
        }
    }

    //
    // vertexes
    //
    for (i = 0; i < g_numvertexes; i++)
    {
        for (j = 0; j < 3; j++)
        {
            g_dvertexes[i].point[j] = LittleFloat(g_dvertexes[i].point[j]);
        }
    }

    //
    // planes
    //      
    for (i = 0; i < g_numplanes; i++)
    {
        for (j = 0; j < 3; j++)
        {
            g_dplanes[i].normal[j] = LittleFloat(g_dplanes[i].normal[j]);
        }
        g_dplanes[i].dist = LittleFloat(g_dplanes[i].dist);
        g_dplanes[i].type = (planetypes)LittleLong(g_dplanes[i].type);
    }

    //
    // texinfos
    //      
    for (i = 0; i < g_numtexinfo; i++)
    {
        for (j = 0; j < 8; j++)
        {
            g_texinfo[i].vecs[0][j] = LittleFloat(g_texinfo[i].vecs[0][j]);
        }
        g_texinfo[i].miptex = LittleLong(g_texinfo[i].miptex);
        g_texinfo[i].flags = LittleLong(g_texinfo[i].flags);
    }

    //
    // faces
    //
    for (i = 0; i < g_numfaces; i++)
    {
        g_dfaces[i].texinfo = LittleShort(g_dfaces[i].texinfo);
        g_dfaces[i].planenum = LittleShort(g_dfaces[i].planenum);
        g_dfaces[i].side = LittleShort(g_dfaces[i].side);
        g_dfaces[i].lightofs = LittleLong(g_dfaces[i].lightofs);
        g_dfaces[i].firstedge = LittleLong(g_dfaces[i].firstedge);
        g_dfaces[i].numedges = LittleShort(g_dfaces[i].numedges);
    }

    //
    // nodes
    //
    for (i = 0; i < g_numnodes; i++)
    {
        g_dnodes[i].planenum = LittleLong(g_dnodes[i].planenum);
        for (j = 0; j < 3; j++)
        {
            g_dnodes[i].mins[j] = LittleShort(g_dnodes[i].mins[j]);
            g_dnodes[i].maxs[j] = LittleShort(g_dnodes[i].maxs[j]);
        }
        g_dnodes[i].children[0] = LittleShort(g_dnodes[i].children[0]);
        g_dnodes[i].children[1] = LittleShort(g_dnodes[i].children[1]);
        g_dnodes[i].firstface = LittleShort(g_dnodes[i].firstface);
        g_dnodes[i].numfaces = LittleShort(g_dnodes[i].numfaces);
    }

    //
    // leafs
    //
    for (i = 0; i < g_numleafs; i++)
    {
        g_dleafs[i].contents = LittleLong(g_dleafs[i].contents);
        for (j = 0; j < 3; j++)
        {
            g_dleafs[i].mins[j] = LittleShort(g_dleafs[i].mins[j]);
            g_dleafs[i].maxs[j] = LittleShort(g_dleafs[i].maxs[j]);
        }

        g_dleafs[i].firstmarksurface = LittleShort(g_dleafs[i].firstmarksurface);
        g_dleafs[i].nummarksurfaces = LittleShort(g_dleafs[i].nummarksurfaces);
        g_dleafs[i].visofs = LittleLong(g_dleafs[i].visofs);
    }

    //
    // clipnodes
    //
    for (i = 0; i < g_numclipnodes; i++)
    {
        g_dclipnodes[i].planenum = LittleLong(g_dclipnodes[i].planenum);
        g_dclipnodes[i].children[0] = LittleShort(g_dclipnodes[i].children[0]);
        g_dclipnodes[i].children[1] = LittleShort(g_dclipnodes[i].children[1]);
    }

    //
    // miptex
    //
    if (g_texdatasize)
    {
        mtl = (dmiptexlump_t*)g_dtexdata;
        if (todisk)
        {
            c = mtl->nummiptex;
        }
        else
        {
            c = LittleLong(mtl->nummiptex);
        }
        mtl->nummiptex = LittleLong(mtl->nummiptex);
        for (i = 0; i < c; i++)
        {
            mtl->dataofs[i] = LittleLong(mtl->dataofs[i]);
        }
    }

    //
    // marksurfaces
    //
    for (i = 0; i < g_nummarksurfaces; i++)
    {
        g_dmarksurfaces[i] = LittleShort(g_dmarksurfaces[i]);
    }

    //
    // surfedges
    //
    for (i = 0; i < g_numsurfedges; i++)
    {
        g_dsurfedges[i] = LittleLong(g_dsurfedges[i]);
    }

    //
    // edges
    //
    for (i = 0; i < g_numedges; i++)
    {
        g_dedges[i].v[0] = LittleShort(g_dedges[i].v[0]);
        g_dedges[i].v[1] = LittleShort(g_dedges[i].v[1]);
    }
}

// =====================================================================================
//  CopyLump
//      balh
// =====================================================================================
static int      CopyLump(int lump, void* dest, int size, const dheader_t* const header)
{
    int             length, ofs;

    length = header->lumps[lump].filelen;
    ofs = header->lumps[lump].fileofs;

    if (length % size)
    {
        Error("LoadBSPFile: odd lump size");
    }
	
	//special handling for tex and lightdata to keep things from exploding - KGP
	if(lump == LUMP_TEXTURES && dest == (void*)g_dtexdata)
	{ hlassume(g_max_map_miptex > length,assume_MAX_MAP_MIPTEX); }
	else if(lump == LUMP_LIGHTING && dest == (void*)g_dlightdata)
	{ hlassume(g_max_map_lightdata > length,assume_MAX_MAP_LIGHTING); }

    memcpy(dest, (byte*) header + ofs, length);

    return length / size;
}


// =====================================================================================
//  LoadBSPFile
//      balh
// =====================================================================================
void            LoadBSPFile(const char* const filename)
{
    dheader_t* header;
    LoadFile(filename, (char**)&header);
    LoadBSPImage(header);
}

// =====================================================================================
//  LoadBSPImage
//      balh
// =====================================================================================
void            LoadBSPImage(dheader_t* const header)
{
    unsigned int     i;

    // swap the header
    for (i = 0; i < sizeof(dheader_t) / 4; i++)
    {
        ((int*)header)[i] = LittleLong(((int*)header)[i]);
    }

    if (header->version != BSPVERSION)
    {
        Error("BSP is version %i, not %i", header->version, BSPVERSION);
    }

    g_nummodels = CopyLump(LUMP_MODELS, g_dmodels, sizeof(dmodel_t), header);
    g_numvertexes = CopyLump(LUMP_VERTEXES, g_dvertexes, sizeof(dvertex_t), header);
    g_numplanes = CopyLump(LUMP_PLANES, g_dplanes, sizeof(dplane_t), header);
    g_numleafs = CopyLump(LUMP_LEAFS, g_dleafs, sizeof(dleaf_t), header);
    g_numnodes = CopyLump(LUMP_NODES, g_dnodes, sizeof(dnode_t), header);
    g_numtexinfo = CopyLump(LUMP_TEXINFO, g_texinfo, sizeof(texinfo_t), header);
    g_numclipnodes = CopyLump(LUMP_CLIPNODES, g_dclipnodes, sizeof(dclipnode_t), header);
    g_numfaces = CopyLump(LUMP_FACES, g_dfaces, sizeof(dface_t), header);
    g_nummarksurfaces = CopyLump(LUMP_MARKSURFACES, g_dmarksurfaces, sizeof(g_dmarksurfaces[0]), header);
    g_numsurfedges = CopyLump(LUMP_SURFEDGES, g_dsurfedges, sizeof(g_dsurfedges[0]), header);
    g_numedges = CopyLump(LUMP_EDGES, g_dedges, sizeof(dedge_t), header);
    g_texdatasize = CopyLump(LUMP_TEXTURES, g_dtexdata, 1, header);
    g_visdatasize = CopyLump(LUMP_VISIBILITY, g_dvisdata, 1, header);
    g_lightdatasize = CopyLump(LUMP_LIGHTING, g_dlightdata, 1, header);
    g_entdatasize = CopyLump(LUMP_ENTITIES, g_dentdata, 1, header);

    Free(header);                                          // everything has been copied out

    //
    // swap everything
    //      
    SwapBSPFile(false);

    g_dmodels_checksum = FastChecksum(g_dmodels, g_nummodels * sizeof(g_dmodels[0]));
    g_dvertexes_checksum = FastChecksum(g_dvertexes, g_numvertexes * sizeof(g_dvertexes[0]));
    g_dplanes_checksum = FastChecksum(g_dplanes, g_numplanes * sizeof(g_dplanes[0]));
    g_dleafs_checksum = FastChecksum(g_dleafs, g_numleafs * sizeof(g_dleafs[0]));
    g_dnodes_checksum = FastChecksum(g_dnodes, g_numnodes * sizeof(g_dnodes[0]));
    g_texinfo_checksum = FastChecksum(g_texinfo, g_numtexinfo * sizeof(g_texinfo[0]));
    g_dclipnodes_checksum = FastChecksum(g_dclipnodes, g_numclipnodes * sizeof(g_dclipnodes[0]));
    g_dfaces_checksum = FastChecksum(g_dfaces, g_numfaces * sizeof(g_dfaces[0]));
    g_dmarksurfaces_checksum = FastChecksum(g_dmarksurfaces, g_nummarksurfaces * sizeof(g_dmarksurfaces[0]));
    g_dsurfedges_checksum = FastChecksum(g_dsurfedges, g_numsurfedges * sizeof(g_dsurfedges[0]));
    g_dedges_checksum = FastChecksum(g_dedges, g_numedges * sizeof(g_dedges[0]));
    g_dtexdata_checksum = FastChecksum(g_dtexdata, g_numedges * sizeof(g_dtexdata[0]));
    g_dvisdata_checksum = FastChecksum(g_dvisdata, g_visdatasize * sizeof(g_dvisdata[0]));
    g_dlightdata_checksum = FastChecksum(g_dlightdata, g_lightdatasize * sizeof(g_dlightdata[0]));
    g_dentdata_checksum = FastChecksum(g_dentdata, g_entdatasize * sizeof(g_dentdata[0]));
}

//
// =====================================================================================
//

// =====================================================================================
//  AddLump
//      balh
// =====================================================================================
static void     AddLump(int lumpnum, void* data, int len, dheader_t* header, FILE* bspfile)
{
    lump_t* lump =&header->lumps[lumpnum];
    lump->fileofs = LittleLong(ftell(bspfile));
    lump->filelen = LittleLong(len);
    SafeWrite(bspfile, data, (len + 3) & ~3);
}

// =====================================================================================
//  WriteBSPFile
//      Swaps the bsp file in place, so it should not be referenced again
// =====================================================================================
void            WriteBSPFile(const char* const filename)
{
    dheader_t       outheader;
    dheader_t*      header;
    FILE*           bspfile;

    header = &outheader;
    memset(header, 0, sizeof(dheader_t));

    SwapBSPFile(true);

    header->version = LittleLong(BSPVERSION);

    bspfile = SafeOpenWrite(filename);
    SafeWrite(bspfile, header, sizeof(dheader_t));         // overwritten later

    //      LUMP TYPE       DATA            LENGTH                              HEADER  BSPFILE   
    AddLump(LUMP_PLANES,    g_dplanes,      g_numplanes * sizeof(dplane_t),     header, bspfile);
    AddLump(LUMP_LEAFS,     g_dleafs,       g_numleafs * sizeof(dleaf_t),       header, bspfile);
    AddLump(LUMP_VERTEXES,  g_dvertexes,    g_numvertexes * sizeof(dvertex_t),  header, bspfile);
    AddLump(LUMP_NODES,     g_dnodes,       g_numnodes * sizeof(dnode_t),       header, bspfile);
    AddLump(LUMP_TEXINFO,   g_texinfo,      g_numtexinfo * sizeof(texinfo_t),   header, bspfile);
    AddLump(LUMP_FACES,     g_dfaces,       g_numfaces * sizeof(dface_t),       header, bspfile);
    AddLump(LUMP_CLIPNODES, g_dclipnodes,   g_numclipnodes * sizeof(dclipnode_t), header, bspfile);

    AddLump(LUMP_MARKSURFACES, g_dmarksurfaces, g_nummarksurfaces * sizeof(g_dmarksurfaces[0]), header, bspfile);
    AddLump(LUMP_SURFEDGES, g_dsurfedges,   g_numsurfedges * sizeof(g_dsurfedges[0]), header, bspfile);
    AddLump(LUMP_EDGES,     g_dedges,       g_numedges * sizeof(dedge_t),       header, bspfile);
    AddLump(LUMP_MODELS,    g_dmodels,      g_nummodels * sizeof(dmodel_t),     header, bspfile);

    AddLump(LUMP_LIGHTING,  g_dlightdata,   g_lightdatasize,                    header, bspfile);
    AddLump(LUMP_VISIBILITY,g_dvisdata,     g_visdatasize,                      header, bspfile);
    AddLump(LUMP_ENTITIES,  g_dentdata,     g_entdatasize,                      header, bspfile);
    AddLump(LUMP_TEXTURES,  g_dtexdata,     g_texdatasize,                      header, bspfile);

    fseek(bspfile, 0, SEEK_SET);
    SafeWrite(bspfile, header, sizeof(dheader_t));

    fclose(bspfile);
}

//
// =====================================================================================
//

#define ENTRIES(a)		(sizeof(a)/sizeof(*(a)))
#define ENTRYSIZE(a)	(sizeof(*(a)))

// =====================================================================================
//  ArrayUsage
//      blah
// =====================================================================================
static int      ArrayUsage(const char* const szItem, const int items, const int maxitems, const int itemsize)
{
    float           percentage = maxitems ? items * 100.0 / maxitems : 0.0;

    Log("%-12s  %7i/%-7i  %7i/%-7i  (%4.1f%%)\n", szItem, items, maxitems, items * itemsize, maxitems * itemsize, percentage);

    return items * itemsize;
}

// =====================================================================================
//  GlobUsage
//      pritn out global ussage line in chart
// =====================================================================================
static int      GlobUsage(const char* const szItem, const int itemstorage, const int maxstorage)
{
    float           percentage = maxstorage ? itemstorage * 100.0 / maxstorage : 0.0;

    Log("%-12s     [variable]    %7i/%-7i  (%4.1f%%)\n", szItem, itemstorage, maxstorage, percentage);

    return itemstorage;
}

// =====================================================================================
//  PrintBSPFileSizes
//      Dumps info about current file
// =====================================================================================
void            PrintBSPFileSizes()
{
    int             numtextures = g_texdatasize ? ((dmiptexlump_t*)g_dtexdata)->nummiptex : 0;
    int             totalmemory = 0;

    Log("\n");
    Log("Object names  Objects/Maxobjs  Memory / Maxmem  Fullness\n");
    Log("------------  ---------------  ---------------  --------\n");

    totalmemory += ArrayUsage("models", g_nummodels, ENTRIES(g_dmodels), ENTRYSIZE(g_dmodels));
    totalmemory += ArrayUsage("planes", g_numplanes, MAX_MAP_PLANES, ENTRYSIZE(g_dplanes));
    totalmemory += ArrayUsage("vertexes", g_numvertexes, ENTRIES(g_dvertexes), ENTRYSIZE(g_dvertexes));
    totalmemory += ArrayUsage("nodes", g_numnodes, ENTRIES(g_dnodes), ENTRYSIZE(g_dnodes));
    totalmemory += ArrayUsage("texinfos", g_numtexinfo, ENTRIES(g_texinfo), ENTRYSIZE(g_texinfo));
    totalmemory += ArrayUsage("faces", g_numfaces, ENTRIES(g_dfaces), ENTRYSIZE(g_dfaces));
    totalmemory += ArrayUsage("clipnodes", g_numclipnodes, ENTRIES(g_dclipnodes), ENTRYSIZE(g_dclipnodes));
    totalmemory += ArrayUsage("leaves", g_numleafs, ENTRIES(g_dleafs), ENTRYSIZE(g_dleafs));
    totalmemory += ArrayUsage("marksurfaces", g_nummarksurfaces, ENTRIES(g_dmarksurfaces), ENTRYSIZE(g_dmarksurfaces));
    totalmemory += ArrayUsage("surfedges", g_numsurfedges, ENTRIES(g_dsurfedges), ENTRYSIZE(g_dsurfedges));
    totalmemory += ArrayUsage("edges", g_numedges, ENTRIES(g_dedges), ENTRYSIZE(g_dedges));

    totalmemory += GlobUsage("texdata", g_texdatasize, g_max_map_miptex);
    totalmemory += GlobUsage("lightdata", g_lightdatasize, g_max_map_lightdata);
    totalmemory += GlobUsage("visdata", g_visdatasize, sizeof(g_dvisdata));
    totalmemory += GlobUsage("entdata", g_entdatasize, sizeof(g_dentdata));

    Log("%i textures referenced\n", numtextures);

    Log("=== Total BSP file data space used: %d bytes ===\n", totalmemory);
}


// =====================================================================================
//  ParseEpair
//      entity key/value pairs
// =====================================================================================
epair_t*        ParseEpair()
{
    epair_t*        e;

    e = (epair_t*)Alloc(sizeof(epair_t));

    if (strlen(g_token) >= MAX_KEY - 1)
        Error("ParseEpair: Key token too long (%i > MAX_KEY)", (int)strlen(g_token));

    e->key = _strdup(g_token);
    GetToken(false);

    if (strlen(g_token) >= ZHLT3_MAX_VALUE - 1)
        Error("ParseEpar: Value token too long (%i > ZHLT3_MAX_VALUE)", (int)strlen(g_token));

    e->value = _strdup(g_token);

    return e;
}

/*
 * ================
 * ParseEntity
 * ================
 */

#ifdef ZHLT_INFO_COMPILE_PARAMETERS
// AJM: each tool should have its own version of GetParamsFromEnt which parseentity calls
extern void     GetParamsFromEnt(entity_t* mapent);
#endif

bool            ParseEntity()
{
    epair_t*        e;
    entity_t*       mapent;

    if (!GetToken(true))
    {
        return false;
    }

    if (strcmp(g_token, "{"))
    {
        Error("ParseEntity: { not found");
    }

    if (g_numentities == MAX_MAP_ENTITIES)
    {
        Error("g_numentities == MAX_MAP_ENTITIES");
    }

    mapent = &g_entities[g_numentities];
    g_numentities++;

    while (1)
    {
        if (!GetToken(true))
        {
            Error("ParseEntity: EOF without closing brace");
        }
        if (!strcmp(g_token, "}"))
        {
            break;
        }
        e = ParseEpair();
        e->next = mapent->epairs;
        mapent->epairs = e;
    }

#ifdef ZHLT_INFO_COMPILE_PARAMETERS // AJM
    if (!strcmp(ValueForKey(mapent, "classname"), "info_compile_parameters"))
    {
        Log("Map entity info_compile_parameters detected, using compile settings\n");
        GetParamsFromEnt(mapent);
    }
#endif

    return true;
}

// =====================================================================================
//  ParseEntities
//      Parses the dentdata string into entities
// =====================================================================================
void            ParseEntities()
{
    g_numentities = 0;
    ParseFromMemory(g_dentdata, g_entdatasize);

    while (ParseEntity())
    {
    }
}

// =====================================================================================
//  UnparseEntities
//      Generates the dentdata string from all the entities
// =====================================================================================
void            UnparseEntities()
{
    char*           buf;
    char*           end;
    epair_t*        ep;
    char            line[MAXTOKEN];
    int             i;

    buf = g_dentdata;
    end = buf;
    *end = 0;

    for (i = 0; i < g_numentities; i++)
    {
        ep = g_entities[i].epairs;
        if (!ep)
        {
            continue;                                      // ent got removed
        }

        strcat(end, "{\n");
        end += 2;

        for (ep = g_entities[i].epairs; ep; ep = ep->next)
        {
            sprintf(line, "\"%s\" \"%s\"\n", ep->key, ep->value);
            strcat(end, line);
            end += strlen(line);
        }
        strcat(end, "}\n");
        end += 2;

        if (end > buf + MAX_MAP_ENTSTRING)
        {
            Error("Entity text too long");
        }
    }
    g_entdatasize = end - buf + 1;
}

// =====================================================================================
//  SetKeyValue
//      makes a keyvalue
// =====================================================================================
void            SetKeyValue(entity_t* ent, const char* const key, const char* const value)
{
    epair_t*        ep;

    for (ep = ent->epairs; ep; ep = ep->next)
    {
        if (!strcmp(ep->key, key))
        {
            Free(ep->value);
            ep->value = strdup(value);
            return;
        }
    }
    ep = (epair_t*)Alloc(sizeof(*ep));
    ep->next = ent->epairs;
    ent->epairs = ep;
    ep->key = strdup(key);
    ep->value = strdup(value);
}

// =====================================================================================
//  ValueForKey
//      returns the value for a passed entity and key
// =====================================================================================
const char*     ValueForKey(const entity_t* const ent, const char* const key)
{
    epair_t*        ep;

    for (ep = ent->epairs; ep; ep = ep->next)
    {
        if (!strcmp(ep->key, key))
        {
            return ep->value;
        }
    }
    return "";
}

// =====================================================================================
//  IntForKey
// =====================================================================================
int             IntForKey(const entity_t* const ent, const char* const key)
{
    return atoi(ValueForKey(ent, key));
}

// =====================================================================================
//  FloatForKey
// =====================================================================================
vec_t           FloatForKey(const entity_t* const ent, const char* const key)
{
    return atof(ValueForKey(ent, key));
}

// =====================================================================================
//  GetVectorForKey
//      returns value for key in vec[0-2]
// =====================================================================================
void            GetVectorForKey(const entity_t* const ent, const char* const key, vec3_t vec)
{
    const char*     k;
    double          v1, v2, v3;

    k = ValueForKey(ent, key);
    // scanf into doubles, then assign, so it is vec_t size independent
    v1 = v2 = v3 = 0;
    sscanf(k, "%lf %lf %lf", &v1, &v2, &v3);
    vec[0] = v1;
    vec[1] = v2;
    vec[2] = v3;
}

// =====================================================================================
//  FindTargetEntity
//      
// =====================================================================================
entity_t *FindTargetEntity(const char* const target)
{
    int             i;
    const char*     n;

    for (i = 0; i < g_numentities; i++)
    {
        n = ValueForKey(&g_entities[i], "targetname");
        if (!strcmp(n, target))
        {
            return &g_entities[i];
        }
    }

    return NULL;
}


void            dtexdata_init()
{
    g_dtexdata = (byte*)AllocBlock(g_max_map_miptex);
    hlassume(g_dtexdata != NULL, assume_NoMemory);
	g_dlightdata = (byte*)AllocBlock(g_max_map_lightdata);
	hlassume(g_dlightdata != NULL, assume_NoMemory);
}

void CDECL      dtexdata_free()
{
    FreeBlock(g_dtexdata);
    g_dtexdata = NULL;
	FreeBlock(g_dlightdata);
	g_dlightdata = NULL;
}

// =====================================================================================
//  GetTextureByNumber
//      Touchy function, can fail with a page fault if all the data isnt kosher 
//      (i.e. map was compiled with missing textures)
// =====================================================================================
char*           GetTextureByNumber(int texturenumber)
{
    texinfo_t*      info;
    miptex_t*       miptex;
    int             ofs;

    info = &g_texinfo[texturenumber];
    ofs = ((dmiptexlump_t*)g_dtexdata)->dataofs[info->miptex];
    miptex = (miptex_t*)(&g_dtexdata[ofs]);

    return miptex->name;
}

// =====================================================================================
//  EntityForModel
//      returns entity addy for given modelnum
// =====================================================================================
entity_t*       EntityForModel(const int modnum)
{
    int             i;
    const char*     s;
    char            name[16];

    sprintf(name, "*%i", modnum);
    // search the entities for one using modnum
    for (i = 0; i < g_numentities; i++)
    {
        s = ValueForKey(&g_entities[i], "model");
        if (!strcmp(s, name))
        {
            return &g_entities[i];
        }
    }

    return &g_entities[0];
}