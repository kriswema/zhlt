// AJM: added this file in

#include "csg.h"

wadpath_t*  g_pWadPaths[MAX_WADPATHS];
int         g_iNumWadPaths = 0;    


// =====================================================================================
//  PushWadPath
//      adds a wadpath into the wadpaths list, without duplicates
// =====================================================================================
void        PushWadPath(const char* const path, bool inuse)
{
    int         i;
    wadpath_t*  current;

    if (!strlen(path))
        return; // no path

    // check for pre-existing path
    for (i = 0; i < g_iNumWadPaths; i++)
    {
        current = g_pWadPaths[i];

        if (!strcmp(current->path, path))
            return; 
    }

    current = (wadpath_t*)malloc(sizeof(wadpath_t));

    safe_strncpy(current->path, path, _MAX_PATH);
    current->usedbymap = inuse;
    current->usedtextures = 0;  // should be updated later in autowad procedures

    g_pWadPaths[g_iNumWadPaths++] = current;

#ifdef _DEBUG
    Log("[dbg] PushWadPath: %i[%s]\n", g_iNumWadPaths, path);
#endif
}

// =====================================================================================
//  IsUsedWadPath
// =====================================================================================
bool        IsUsedWadPath(const char* const path)
{
    int         i;
    wadpath_t*  current;

    for (i = 0; i < g_iNumWadPaths; i++)
    {
        current = g_pWadPaths[i];
        if (!strcmp(current->path, path))
        {
            if (current->usedbymap)
                return true;

            return false;
        }
    }   

    return false;
}

// =====================================================================================
//  IsListedWadPath
// =====================================================================================
bool        IsListedWadPath(const char* const path)
{
    int         i;
    wadpath_t*  current;

    for (i = 0; i < g_iNumWadPaths; i++)
    {
        current = g_pWadPaths[i];
        if (!strcmp(current->path, path))
            return true;
    }

    return false;
}

// =====================================================================================
//  FreeWadPaths
// =====================================================================================
void        FreeWadPaths()
{
    int         i;
    wadpath_t*  current;

    for (i = 0; i < g_iNumWadPaths; i++)
    {
        current = g_pWadPaths[i];
        free(current);
    }
}

// =====================================================================================
//  GetUsedWads
//      parse the "wad" keyvalue into wadpath_t structs
// =====================================================================================
void        GetUsedWads()
{
    const char* pszWadPaths;
    char        szTmp[_MAX_PATH];
    int         i, j;

    pszWadPaths = ValueForKey(&g_entities[0], "wad");

    for(i = 0; i < MAX_WADPATHS; i++)
    {
        memset(szTmp, 0, sizeof(szTmp));    // are you happy zipster?
        for (j = 0; j < _MAX_PATH; j++, pszWadPaths++)
        {
            if (pszWadPaths[0] == ';')
            {
                pszWadPaths++;
                PushWadPath(szTmp, true);
                break;
            }

            if (pszWadPaths[0] == 0)
            {
                PushWadPath(szTmp, true); // fix by AmericanRPG for last wadpath ignorance bug
                return;
            }

            szTmp[j] = pszWadPaths[0];
        }
    }
}