#ifndef CONFIG_H
#define CONFIG_H

#ifdef SYSTEM_POSIX

// TODO: These are now unsafe
#define strcpy_s strcpy
#define sprintf_s sprintf
#define fscanf_s fscanf 
#define sscanf_s sscanf 

#define _strlwr strlwr
#define _strupr strupr
#define _strdup strdup 
#define _open open
#define _close close
#define _read read
#define _unlink unlink 

//#include <unistd.h>
#endif // SYSTEM_POSIX

#endif // CONFIG_H
