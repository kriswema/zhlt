/***
*
*	Copyright (c) 1998, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
****/

// csg4.c

#include "ripent.h"

typedef enum
{
    hl_undefined = -1,
    hl_export = 0,
    hl_import = 1
}
hl_types;

static hl_types g_mode = hl_undefined;

// g_parse: command line switch (-parse).
// Added by: Ryan Gregg aka Nem
bool g_parse = DEFAULT_PARSE;

bool g_chart = DEFAULT_CHART;

bool g_info = DEFAULT_INFO;

// ScanForToken()
// Added by: Ryan Gregg aka Nem
// 
// Scans entity data starting  at iIndex for cToken.  Every time a \n char
// is encountered iLine is incremented.  If iToken is not null, the index
// cToken was found at is inserted into it. 
bool ScanForToken(char cToken, int &iIndex, int &iLine, bool bIgnoreWhiteSpace, bool bIgnoreOthers, int *iToken = 0)
{
	for(; iIndex < g_entdatasize; iIndex++)
	{
		// If we found a null char, consider it end of data.
		if(g_dentdata[iIndex] == '\0')
		{
			iIndex = g_entdatasize;
			return false;
		}

		// Count lines (for error message).
		if(g_dentdata[iIndex] == '\n')
		{
			iLine++;
		}

		// Ignore white space, if we are ignoring it.
		if(!bIgnoreWhiteSpace && isspace(g_dentdata[iIndex]))
		{
			continue;
		}

		if(g_dentdata[iIndex] != cToken)
		{
			if(bIgnoreOthers)
				continue;
			else
				return false;
		}

		// Advance the index past the token.
		iIndex++;

		// Return the index of the token if requested.
		if(iToken != 0)
		{
			*iToken = iIndex - 1;
		}

		return true;
	}

	// End of data.
	return false;
}

#include <list>
typedef std::list<char *> CEntityPairList;
typedef std::list<CEntityPairList *> CEntityList;

// ParseEntityData()
// Added by: Ryan Gregg aka Nem
// 
// Pareses and reformats entity data stripping all non essential
// formatting  and using the formatting  options passed through this
// function.  The length is specified because in some cases (i.e. the
// terminator) a null char is desired to be printed.
void ParseEntityData(const char *cTab, int iTabLength, const char *cNewLine, int iNewLineLength, const char *cTerminator, int iTerminatorLength)
{
	CEntityList EntityList;		// Parsed entities.

	int iIndex = 0;				// Current char in g_dentdata.
	int iLine = 0;				// Current line in g_dentdata.

	char cError[256] = "";

	try
	{
		//
		// Parse entity data.
		//

		Log("\nParsing entity data.\n");

		while(true)
		{
			// Parse the start of an entity.
			if(!ScanForToken('{', iIndex, iLine, false, false))
			{
				if(iIndex == g_entdatasize)
				{
					// We read all the entities.
					break;
				}
				else
				{
					sprintf_s(cError, "expected token %s on line %d.", "{", iLine);
					throw cError;
				}
			}

			CEntityPairList *EntityPairList = new CEntityPairList();

			// Parse the rest of the entity.
			while(true)
			{
				// Parse the key and value.
				for(int j = 0; j < 2; j++)
				{
					int iStart;
					// Parse the start of a string.
					if(!ScanForToken('\"', iIndex, iLine, false, false, &iStart))
					{
						sprintf_s(cError, "expected token %s on line %d.", "\"", iLine);
						throw cError;
					}

					int iEnd;
					// Parse the end of a string.
					if(!ScanForToken('\"', iIndex, iLine, true, true, &iEnd))
					{
						sprintf_s(cError, "expected token %s on line %d.", "\"", iLine);
						throw cError;
					}

					// Extract the string.
					int iLength = iEnd - iStart - 1;
					char *cString = new char[iLength + 1];
					memcpy(cString, &g_dentdata[iStart + 1], iLength);
					cString[iLength] = '\0';

					// Save it.
					EntityPairList->push_back(cString);
				}

				// Parse the end of an entity.
				if(!ScanForToken('}', iIndex, iLine, false, false))
				{
					if(g_dentdata[iIndex] == '\"')
					{
						// We arn't done the entity yet.
						continue;
					}
					else
					{
						sprintf_s(cError, "expected token %s on line %d.", "}", iLine);
						throw cError;
					}
				}

				// We read the entity.
				EntityList.push_back(EntityPairList);
				break;
			}
		}

		Log("%d entities parsed.\n", EntityList.size());

		//
		// Calculate new data length.
		//

		int iNewLength = 0;

		for(CEntityList::iterator i = EntityList.begin(); i != EntityList.end(); ++i)
		{
			// Opening brace.
			iNewLength += 1;

			// New line.
			iNewLength += iNewLineLength;

			CEntityPairList *EntityPairList = *i;

			for(CEntityPairList::iterator j = EntityPairList->begin(); j != EntityPairList->end(); ++j)
			{
				// Tab.
				iNewLength += iTabLength;

				// String.
				iNewLength += 1;
				iNewLength += (int)strlen(*j);
				iNewLength += 1;

				// String seperator.
				iNewLength += 1;

				++j;

				// String.
				iNewLength += 1;
				iNewLength += (int)strlen(*j);
				iNewLength += 1;

				// New line.
				iNewLength += iNewLineLength;
			}

			// Closing brace.
			iNewLength += 1;

			// New line.
			iNewLength += iNewLineLength;
		}

		// Terminator.
		iNewLength += iTerminatorLength;

		//
		// Check our parsed data.
		//

		assume(iNewLength != 0, "No entity data.");
		assume(iNewLength < sizeof(g_dentdata), "Entity data size exceedes dentdata limit.");

		//
		// Clear current data.
		//

		g_entdatasize = 0;

		//
		// Fill new data.
		//

		Log("Formating entity data.\n\n");

		for(CEntityList::iterator i = EntityList.begin(); i != EntityList.end(); ++i)
		{
			// Opening brace.
			g_dentdata[g_entdatasize] = '{';
			g_entdatasize += 1;

			// New line.
			memcpy(&g_dentdata[g_entdatasize], cNewLine, iNewLineLength);
			g_entdatasize += iNewLineLength;

			CEntityPairList *EntityPairList = *i;

			for(CEntityPairList::iterator j = EntityPairList->begin(); j != EntityPairList->end(); ++j)
			{
				// Tab.
				memcpy(&g_dentdata[g_entdatasize], cTab, iTabLength);
				g_entdatasize += iTabLength;

				// String.
				g_dentdata[g_entdatasize] = '\"';
				g_entdatasize += 1;
				memcpy(&g_dentdata[g_entdatasize], *j, strlen(*j));
				g_entdatasize += (int)strlen(*j);
				g_dentdata[g_entdatasize] = '\"';
				g_entdatasize += 1;

				// String seperator.
				g_dentdata[g_entdatasize] = ' ';
				g_entdatasize += 1;

				++j;

				// String.
				g_dentdata[g_entdatasize] = '\"';
				g_entdatasize += 1;
				memcpy(&g_dentdata[g_entdatasize], *j, strlen(*j));
				g_entdatasize += (int)strlen(*j);
				g_dentdata[g_entdatasize] = '\"';
				g_entdatasize += 1;

				// New line.
				memcpy(&g_dentdata[g_entdatasize], cNewLine, iNewLineLength);
				g_entdatasize += iNewLineLength;
			}

			// Closing brace.
			g_dentdata[g_entdatasize] = '}';
			g_entdatasize += 1;

			// New line.
			memcpy(&g_dentdata[g_entdatasize], cNewLine, iNewLineLength);
			g_entdatasize += iNewLineLength;
		}

		// Terminator.
		memcpy(&g_dentdata[g_entdatasize], cTerminator, iTerminatorLength);
		g_entdatasize += iTerminatorLength;

		//
		// Delete entity data.
		//

		for(CEntityList::iterator i = EntityList.begin(); i != EntityList.end(); ++i)
		{
			CEntityPairList *EntityPairList = *i;

			for(CEntityPairList::iterator j = EntityPairList->begin(); j != EntityPairList->end(); ++j)
			{
				delete []*j;
			}

			delete EntityPairList;
		}

		//return true;
	}
	catch(...)
	{
		//
		// Delete entity data.
		//

		for(CEntityList::iterator i = EntityList.begin(); i != EntityList.end(); ++i)
		{
			CEntityPairList *EntityPairList = *i;

			for(CEntityPairList::iterator j = EntityPairList->begin(); j != EntityPairList->end(); ++j)
			{
				delete []*j;
			}

			delete EntityPairList;
		}

		// If we threw the error cError wont be null, this is
		// a message, print it.
		if(*cError != '\0')
		{
			Error(cError);
		}
		Error("unknowen exception.");

		//return false;
	}
}

static void     ReadBSP(const char* const name)
{
    char            filename[_MAX_PATH];

    safe_strncpy(filename, name, _MAX_PATH);
    StripExtension(filename);
    DefaultExtension(filename, ".bsp");

    LoadBSPFile(name);
}

static void     WriteBSP(const char* const name)
{
    char            filename[_MAX_PATH];

    safe_strncpy(filename, name, _MAX_PATH);
    StripExtension(filename);
    DefaultExtension(filename, ".bsp");

    WriteBSPFile(filename);
}

static void     WriteEntities(const char* const name)
{
    char filename[_MAX_PATH];

    safe_strncpy(filename, name, _MAX_PATH);
    StripExtension(filename);
    DefaultExtension(filename, ".ent");
    _unlink(filename);

    {
		if(g_parse)  // Added by Nem.
		{
			ParseEntityData("  ", 2, "\r\n", 2, "", 0);
		}

        FILE *f = SafeOpenWrite(filename);
		Log("\nWriting %s.\n", filename);  // Added by Nem.
        SafeWrite(f, g_dentdata, g_entdatasize);
        fclose(f);
    }
}

static void     ReadEntities(const char* const name)
{
    char filename[_MAX_PATH];

    safe_strncpy(filename, name, _MAX_PATH);
    StripExtension(filename);
    DefaultExtension(filename, ".ent");

    {
        FILE *f = SafeOpenRead(filename);
		Log("\nReading %s.\n", filename);  // Added by Nem.

        g_entdatasize = q_filelength(f);

		assume(g_entdatasize != 0, "No entity data.");
        assume(g_entdatasize < sizeof(g_dentdata), "Entity data size exceedes dentdata limit.");

        SafeRead(f, g_dentdata, g_entdatasize);

        fclose(f);

        if (g_dentdata[g_entdatasize-1] != 0)
        {
//            Log("g_dentdata[g_entdatasize-1] = %d\n", g_dentdata[g_entdatasize-1]);

			if(g_parse)  // Added by Nem.
			{
				ParseEntityData("", 0, "\n", 1, "\0", 1);
			}
			else
			{
				if(g_dentdata[g_entdatasize - 1] != '\0')
				{
					g_dentdata[g_entdatasize] = '\0';
					g_entdatasize++;
				}
			}
        }
    }
}

//======================================================================

static void     Usage(void)
{
    //Log("%s " ZHLT_VERSIONSTRING "\n" MODIFICATIONS_STRING "\n", g_Program);
    //Log("  Usage: ripent [-import|-export] [-texdata n] bspname\n");

	// Modified to behave like other tools by Nem.

	Banner();
	Log("\n-= %s Options =-\n\n", g_Program);

	Log("    -export         : Export entity data\n");
	Log("    -import         : Import entity data\n\n");

	Log("    -parse          : Parse and format entity data\n\n");

    Log("    -texdata #      : Alter maximum texture memory limit (in kb)\n");
    Log("    -lightdata #    : Alter maximum lighting memory limit (in kb)\n");
	Log("    -chart          : Display bsp statitics\n");
	Log("    -noinfo         : Do not show tool configuration information\n\n");

	Log("    mapfile         : The mapfile to process\n\n");

    exit(1);
}

// =====================================================================================
//  Settings
// =====================================================================================
static void     Settings()
{
    char*           tmp;

    if (!g_info)
    {
        return; 
    }

    Log("\n-= Current %s Settings =-\n", g_Program);
    Log("Name               |  Setting  |  Default\n" "-------------------|-----------|-------------------------\n");

    // ZHLT Common Settings
    Log("chart               [ %7s ] [ %7s ]\n", g_chart ? "on" : "off", DEFAULT_CHART ? "on" : "off");
    Log("max texture memory  [ %7d ] [ %7d ]\n", g_max_map_miptex, DEFAULT_MAX_MAP_MIPTEX);
	Log("max lighting memory [ %7d ] [ %7d ]\n", g_max_map_lightdata, DEFAULT_MAX_MAP_LIGHTDATA);

    switch (g_mode)
    {
    case hl_import:
    default:
        tmp = "Import";
        break;
    case hl_export:
        tmp = "Export";
        break;
    }

	Log("\n");

    // RipEnt Specific Settings
	Log("mode                [ %7s ] [ %7s ]\n", tmp, "N/A");
    Log("parse               [ %7s ] [ %7s ]\n", g_parse ? "on" : "off", DEFAULT_PARSE ? "on" : "off");

    Log("\n\n");
}

/*
 * ============
 * main
 * ============
 */
int             main(int argc, char** argv)
{
    int             i;
    double          start, end;

    g_Program = "ripent";

    if (argc == 1)
    {
        Usage();
    }

    for (i = 1; i < argc; i++)
    {
        if (!strcasecmp(argv[i], "-import"))
        {
            g_mode = hl_import;
        }
        else if (!strcasecmp(argv[i], "-export"))
        {
            g_mode = hl_export;
        }
		// g_parse: command line switch (-parse).
		// Added by: Ryan Gregg aka Nem
		else if(!strcasecmp(argv[i], "-parse"))
		{
			g_parse = true;
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
        else if (!strcasecmp(argv[i], "-chart"))
        {
            g_chart = true;
        }
        else if (!strcasecmp(argv[i], "-noinfo"))
        {
            g_info = false;
        }
        else
        {
            safe_strncpy(g_Mapname, argv[i], _MAX_PATH);
            StripExtension(g_Mapname);
            DefaultExtension(g_Mapname, ".bsp");
        }
    }

    if (g_mode == hl_undefined)
    {
        fprintf(stderr, "%s", "Must specify either -import or -export\n");
        Usage();
    }

    if (!q_exists(g_Mapname))
    {
        fprintf(stderr, "%s", "bspfile '%s' does not exist\n", g_Mapname);
        Usage();
    }

	LogStart(argc, argv);
	atexit(LogEnd);

	Settings();

    dtexdata_init();
    atexit(dtexdata_free);

    // BEGIN RipEnt
    start = I_FloatTime();

    switch (g_mode)
    {
    case hl_import:
		ReadBSP(g_Mapname);
        ReadEntities(g_Mapname);
        WriteBSP(g_Mapname);
        break;
    case hl_export:
		ReadBSP(g_Mapname);
        WriteEntities(g_Mapname);
        break;
    }

    if (g_chart)
        PrintBSPFileSizes();

    end = I_FloatTime();
    LogTimeElapsed(end - start);
    // END RipEnt

    return 0;
}

// do nothing - we don't have params to fetch
void GetParamsFromEnt(entity_t* mapent) {}
