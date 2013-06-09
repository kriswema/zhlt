#ifndef LOG_H__
#define LOG_H__

#if _MSC_VER >= 1000
#pragma once
#endif

#include "mathtypes.h"
#include "messages.h"

typedef enum
{
    DEVELOPER_LEVEL_ALWAYS,
    DEVELOPER_LEVEL_ERROR,
    DEVELOPER_LEVEL_WARNING,
    DEVELOPER_LEVEL_MESSAGE,
    DEVELOPER_LEVEL_FLUFF,
    DEVELOPER_LEVEL_SPAM,
    DEVELOPER_LEVEL_MEGASPAM
}
developer_level_t;

//
// log.c globals
//

extern char*    g_Program;
extern char     g_Mapname[_MAX_PATH];

#define DEFAULT_DEVELOPER   DEVELOPER_LEVEL_ALWAYS
#define DEFAULT_VERBOSE     false
#define DEFAULT_LOG         true

extern developer_level_t g_developer;
extern bool          g_verbose;
extern bool          g_log;
extern unsigned long g_clientid;                           // Client id of this program
extern unsigned long g_nextclientid;                       // Client id of next client to spawn from this server

//
// log.c Functions
//

extern void     ResetTmpFiles();
extern void     ResetLog();
extern void     ResetErrorLog();
extern void     CheckForErrorLog();

extern void CDECL OpenLog(int clientid);
extern void CDECL CloseLog();
extern void     WriteLog(const char* const message);

extern void     CheckFatal();

extern void CDECL Developer(developer_level_t level, const char* const message, ...);

#ifdef _DEBUG
#define IfDebug(x) (x)
#else
#define IfDebug(x)
#endif

extern void CDECL Verbose(const char* const message, ...);
extern void CDECL Log(const char* const message, ...);
extern void CDECL Error(const char* const error, ...);
extern void CDECL Fatal(assume_msgs msgid, const char* const error, ...);
extern void CDECL Warning(const char* const warning, ...);

extern void CDECL PrintOnce(const char* const message, ...);

extern void     LogStart(const int argc, char** argv);
extern void     LogEnd();
extern void     Banner();

extern void     LogTimeElapsed(float elapsed_time);

// Should be in hlassert.h, but well so what
extern void     hlassume(bool exp, assume_msgs msgid);

#endif // Should be in hlassert.h, but well so what LOG_H__
