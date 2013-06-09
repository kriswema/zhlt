#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef ZHLT_NETVIS
#ifdef SYSTEM_WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif
#endif

#ifdef STDC_HEADERS
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef ZHLT_NETVIS
#include "../netvis/c2cpp.h"
#endif

#include "cmdlib.h"
#include "messages.h"
#include "hlassert.h"
#include "log.h"
#include "filelib.h"

char*           g_Program = "Uninitialized variable ::g_Program";
char            g_Mapname[_MAX_PATH] = "Uninitialized variable ::g_Mapname";

developer_level_t g_developer = DEFAULT_DEVELOPER;
bool            g_verbose = DEFAULT_VERBOSE;
bool            g_log = DEFAULT_LOG;

unsigned long   g_clientid = 0;
unsigned long   g_nextclientid = 0;

static FILE*    CompileLog = NULL;
static bool     fatal = false;

////////

void            ResetTmpFiles()
{
    if (g_log)
    {
        char            filename[_MAX_PATH];

        safe_snprintf(filename, _MAX_PATH, "%s.bsp", g_Mapname);
        _unlink(filename);

        safe_snprintf(filename, _MAX_PATH, "%s.inc", g_Mapname);
        _unlink(filename);

        safe_snprintf(filename, _MAX_PATH, "%s.p0", g_Mapname);
        _unlink(filename);

        safe_snprintf(filename, _MAX_PATH, "%s.p1", g_Mapname);
        _unlink(filename);

        safe_snprintf(filename, _MAX_PATH, "%s.p2", g_Mapname);
        _unlink(filename);

        safe_snprintf(filename, _MAX_PATH, "%s.p3", g_Mapname);
        _unlink(filename);

        safe_snprintf(filename, _MAX_PATH, "%s.prt", g_Mapname);
        _unlink(filename);

        safe_snprintf(filename, _MAX_PATH, "%s.pts", g_Mapname);
        _unlink(filename);

        safe_snprintf(filename, _MAX_PATH, "%s.lin", g_Mapname);
        _unlink(filename);

        safe_snprintf(filename, _MAX_PATH, "%s.wic", g_Mapname);
        _unlink(filename);
    }
}

void            ResetLog()
{
    if (g_log)
    {
        char            logfilename[_MAX_PATH];

        safe_snprintf(logfilename, _MAX_PATH, "%s.log", g_Mapname);
        _unlink(logfilename);
    }
}

void            ResetErrorLog()
{
    if (g_log)
    {
        char            logfilename[_MAX_PATH];

        safe_snprintf(logfilename, _MAX_PATH, "%s.err", g_Mapname);
        _unlink(logfilename);
    }
}

void            CheckForErrorLog()
{
    if (g_log)
    {
        char            logfilename[_MAX_PATH];

        safe_snprintf(logfilename, _MAX_PATH, "%s.err", g_Mapname);
        if (q_exists(logfilename))
        {
            Log(">> There was a problem compiling the map.\n"
                ">> Check the file %s.log for the cause.\n",
                 g_Mapname);
            exit(1);
        }
    }
}

///////

void            LogError(const char* const message)
{
    if (g_log && CompileLog)
    {
        char            logfilename[_MAX_PATH];
        FILE*           ErrorLog = NULL;

        safe_snprintf(logfilename, _MAX_PATH, "%s.err", g_Mapname);
        ErrorLog = fopen(logfilename, "a");

        if (ErrorLog)
        {
            fprintf(ErrorLog, "%s: %s\n", g_Program, message);
            fflush(ErrorLog);
            fclose(ErrorLog);
            ErrorLog = NULL;
        }
        else
        {
            fprintf(stderr, "ERROR: Could not open error logfile %s", logfilename);
            fflush(stderr);
        }
    }
}

void CDECL      OpenLog(const int clientid)
{
    if (g_log)
    {
        char            logfilename[_MAX_PATH];

#ifdef ZHLT_NETVIS
    #ifdef SYSTEM_WIN32
        if (clientid)
        {
            char            computername[MAX_COMPUTERNAME_LENGTH + 1];
            unsigned long   size = sizeof(computername);

            if (!GetComputerName(computername, &size))
            {
                safe_strncpy(computername, "unknown", sizeof(computername));
            }
            safe_snprintf(logfilename, _MAX_PATH, "%s-%s-%d.log", g_Mapname, computername, clientid);
        }
        else
    #endif
    #ifdef SYSTEM_POSIX
        if (clientid)
        {
            char            computername[_MAX_PATH];
            unsigned long   size = sizeof(computername);

            if (gethostname(computername, size))
            {
                safe_strncpy(computername, "unknown", sizeof(computername));
            }
            safe_snprintf(logfilename, _MAX_PATH, "%s-%s-%d.log", g_Mapname, computername, clientid);
        }
    #endif
#endif
        {
            safe_snprintf(logfilename, _MAX_PATH, "%s.log", g_Mapname);
        }
        CompileLog = fopen(logfilename, "a");

        if (!CompileLog)
        {
            fprintf(stderr, "ERROR: Could not open logfile %s", logfilename);
            fflush(stderr);
        }
    }
}

void CDECL      CloseLog()
{
    if (g_log && CompileLog)
    {
        LogEnd();
        fflush(CompileLog);
        fclose(CompileLog);
        CompileLog = NULL;
    }
}

//
//  Every function up to this point should check g_log, the functions below should not
//

#ifdef SYSTEM_WIN32
// AJM: fprintf/flush wasnt printing newline chars correctly (prefixed with \r) under win32
//      due to the fact that those streams are in byte mode, so this function prefixes 
//      all \n with \r automatically.
//      NOTE: system load may be more with this method, but there isnt that much logging going
//      on compared to the time taken to compile the map, so its negligable.
void            Safe_WriteLog(const char* const message)
{
    const char* c;
    
    if (!CompileLog)
        return;

    c = &message[0];

    while (1)
    {
        if (!*c)
            return; // end of string

        if (*c == '\n')
            fputc('\r', CompileLog);

        fputc(*c, CompileLog);

        c++;
    }
}
#endif

void            WriteLog(const char* const message)
{

#ifndef SYSTEM_WIN32
    if (CompileLog)
    {
        fprintf(CompileLog, message);
        fflush(CompileLog);
    }
#else
    Safe_WriteLog(message);
#endif

    fprintf(stdout, message);
    fflush(stdout);
}

// =====================================================================================
//  CheckFatal 
// =====================================================================================
void            CheckFatal()
{
    if (fatal)
    {
        hlassert(false);
        exit(1);
    }
}

#define MAX_ERROR   2048
#define MAX_WARNING 2048
#define MAX_MESSAGE 2048

// =====================================================================================
//  Error
//      for formatted error messages, fatals out
// =====================================================================================
void CDECL      Error(const char* const error, ...)
{
    char            message[MAX_ERROR];
    char            message2[MAX_ERROR];
    va_list         argptr;
    
 /*#if defined( SYSTEM_WIN32 ) && !defined( __MINGW32__ ) && !defined( __BORLANDC__ )
    {
        char* wantint3 = getenv("WANTINT3");
		if (wantint3)
		{
			if (atoi(wantint3))
			{
				__asm
				{
					int 3;
				}
			}
		}
    }
#endif*/

    va_start(argptr, error);
    vsnprintf(message, MAX_ERROR, error, argptr);
    va_end(argptr);

    safe_snprintf(message2, MAX_MESSAGE, "Error: %s\n", message);
    WriteLog(message2);
    LogError(message2);

    fatal = 1;
    CheckFatal();
}

// =====================================================================================
//  Fatal
//      For formatted 'fatal' warning messages
//      automatically appends an extra newline to the message
//      This function sets a flag that the compile should abort before completing
// =====================================================================================
void CDECL      Fatal(assume_msgs msgid, const char* const warning, ...)
{
    char            message[MAX_WARNING];
    char            message2[MAX_WARNING];

    va_list         argptr;

    va_start(argptr, warning);
    vsnprintf(message, MAX_WARNING, warning, argptr);
    va_end(argptr);

    safe_snprintf(message2, MAX_MESSAGE, "Error: %s\n", message);
    WriteLog(message2);
    LogError(message2);

    {
        char            message[MAX_MESSAGE];
        const MessageTable_t* msg = GetAssume(msgid);

        safe_snprintf(message, MAX_MESSAGE, "%s\nDescription: %s\nHowto Fix: %s\n", msg->title, msg->text, msg->howto);
        PrintOnce(message);
    }

    fatal = 1;
}

// =====================================================================================
//  PrintOnce
//      This function is only callable one time. Further calls will be ignored
// =====================================================================================
void CDECL      PrintOnce(const char* const warning, ...)
{
    char            message[MAX_WARNING];
    char            message2[MAX_WARNING];
    va_list         argptr;
    static int      count = 0;

    if (count > 0) // make sure it only gets called once
    {
        return;
    }
    count++;

    va_start(argptr, warning);
    vsnprintf(message, MAX_WARNING, warning, argptr);
    va_end(argptr);

    safe_snprintf(message2, MAX_MESSAGE, "Error: %s\n", message);
    WriteLog(message2);
    LogError(message2);
}

// =====================================================================================
//  Warning
//      For formatted warning messages
//      automatically appends an extra newline to the message
// =====================================================================================
void CDECL      Warning(const char* const warning, ...)
{
    char            message[MAX_WARNING];
    char            message2[MAX_WARNING];

    va_list         argptr;

    va_start(argptr, warning);
    vsnprintf(message, MAX_WARNING, warning, argptr);
    va_end(argptr);

    safe_snprintf(message2, MAX_MESSAGE, "Warning: %s\n", message);
    WriteLog(message2);
}

// =====================================================================================
//  Verbose
//      Same as log but only prints when in verbose mode
// =====================================================================================
void CDECL      Verbose(const char* const warning, ...)
{
    if (g_verbose)
    {
        char            message[MAX_MESSAGE];

        va_list         argptr;

        va_start(argptr, warning);
        vsnprintf(message, MAX_MESSAGE, warning, argptr);
        va_end(argptr);

        WriteLog(message);
    }
}

// =====================================================================================
//  Developer
//      Same as log but only prints when in developer mode
// =====================================================================================
void CDECL      Developer(developer_level_t level, const char* const warning, ...)
{
    if (level <= g_developer)
    {
        char            message[MAX_MESSAGE];

        va_list         argptr;

        va_start(argptr, warning);
        vsnprintf(message, MAX_MESSAGE, warning, argptr);
        va_end(argptr);

        WriteLog(message);
    }
}

// =====================================================================================
//  DisplayDeveloperLevel
// =====================================================================================
static void     DisplayDeveloperLevel()
{
    char            message[MAX_MESSAGE];

    safe_strncpy(message, "Developer messages enabled : [", MAX_MESSAGE);
    if (g_developer >= DEVELOPER_LEVEL_MEGASPAM)
    {
        safe_strncat(message, "MegaSpam ", MAX_MESSAGE);
    }
    if (g_developer >= DEVELOPER_LEVEL_SPAM)
    {
        safe_strncat(message, "Spam ", MAX_MESSAGE);
    }
    if (g_developer >= DEVELOPER_LEVEL_FLUFF)
    {
        safe_strncat(message, "Fluff ", MAX_MESSAGE);
    }
    if (g_developer >= DEVELOPER_LEVEL_MESSAGE)
    {
        safe_strncat(message, "Message ", MAX_MESSAGE);
    }
    if (g_developer >= DEVELOPER_LEVEL_WARNING)
    {
        safe_strncat(message, "Warning ", MAX_MESSAGE);
    }
    if (g_developer >= DEVELOPER_LEVEL_ERROR)
    {
        safe_strncat(message, "Error", MAX_MESSAGE);
    }
    if (g_developer)
    {
        safe_strncat(message, "]\n", MAX_MESSAGE);
        Log(message);
    }
}

// =====================================================================================
//  Log
//      For formatted log output messages
// =====================================================================================
void CDECL      Log(const char* const warning, ...)
{
    char            message[MAX_MESSAGE];

    va_list         argptr;

    va_start(argptr, warning);
    vsnprintf(message, MAX_MESSAGE, warning, argptr);
    va_end(argptr);

    WriteLog(message);
}

// =====================================================================================
//  LogArgs
// =====================================================================================
static void     LogArgs(int argc, char** argv)
{
    int             i;

    Log("Command line: ");
    for (i = 0; i < argc; i++)
    {
        if (strchr(argv[i], ' '))
        {
            Log("\"%s\"", argv[i]);
        }
        else
        {
            Log("%s ", argv[i]);
        }
    }
    Log("\n");
}

// =====================================================================================
//  Banner
// =====================================================================================
void            Banner()
{
    Log("%s " ZHLT_VERSIONSTRING " " HACK_VERSIONSTRING " (%s)\n", g_Program, __DATE__);
    //Log("BUGGY %s (built: %s)\nUse at own risk.\n", g_Program, __DATE__);

    Log("Zoner's Half-Life Compilation Tools -- Custom Build\n"
        "Based on code modifications by Sean 'Zoner' Cavanaugh\n"
        "Based on Valve's version, modified with permission.\n"
        MODIFICATIONS_STRING);

}

// =====================================================================================
//  LogStart
// =====================================================================================
void            LogStart(int argc, char** argv)
{
    Banner();
    Log("-----  BEGIN  %s -----\n", g_Program);
    LogArgs(argc, argv);
    DisplayDeveloperLevel();
}

// =====================================================================================
//  LogEnd
// =====================================================================================
void            LogEnd()
{
    Log("\n-----   END   %s -----\n\n\n\n", g_Program);
}

// =====================================================================================
//  hlassume
//      my assume
// =====================================================================================
void            hlassume(bool exp, assume_msgs msgid)
{
    if (!exp)
    {
        char            message[MAX_MESSAGE];
        const MessageTable_t* msg = GetAssume(msgid);

        safe_snprintf(message, MAX_MESSAGE, "%s\nDescription: %s\nHowto Fix: %s\n", msg->title, msg->text, msg->howto);
        Error(message);
    }
}

// =====================================================================================
//  seconds_to_hhmm
// =====================================================================================
static void seconds_to_hhmm(unsigned int elapsed_time, unsigned& days, unsigned& hours, unsigned& minutes, unsigned& seconds)
{
    seconds = elapsed_time % 60;
    elapsed_time /= 60;

    minutes = elapsed_time % 60;
    elapsed_time /= 60;

    hours = elapsed_time % 24;
    elapsed_time /= 24;

    days = elapsed_time;
}

// =====================================================================================
//  LogTimeElapsed
// =====================================================================================
void LogTimeElapsed(float elapsed_time)
{
    unsigned days = 0;
    unsigned hours = 0;
    unsigned minutes = 0;
    unsigned seconds = 0;

    seconds_to_hhmm(elapsed_time, days, hours, minutes, seconds);

    if (days)
    {
        Log("%.2f seconds elapsed [%ud %uh %um %us]\n", elapsed_time, days, hours, minutes, seconds);
    }
    else if (hours)
    {
        Log("%.2f seconds elapsed [%uh %um %us]\n", elapsed_time, hours, minutes, seconds);
    }
    else if (minutes)
    {
        Log("%.2f seconds elapsed [%um %us]\n", elapsed_time, minutes, seconds);
    }
    else
    {
        Log("%.2f seconds elapsed\n", elapsed_time);
    }
}