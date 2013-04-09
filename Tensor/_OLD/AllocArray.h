/* Taken from MTW project by Abedul Haque, December 2009		*/
/* for memory allocation purpose								*/
/* This file has not been modified at all						*/
/* used for CS3610 class project								*/

// AllocArray.h

// header file for dynamic array allocation routines

#pragma once

#include "assert.h"

#ifndef sqr
#define sqr(x)                   ((x) * (x))                               // a simple square macro
#endif

#ifndef QUIT
#define QUIT /* fatal error macro */                                                \
{                                                                                   \
    fprintf(stderr,"\n***** Fatal error on line %d of '%s'\n",__LINE__,__FILE__);   \
    _flushall();                                                                    \
    assert(0);                                                                      \
    exit(999);                                                                      \
}
#endif

#ifndef NOALLOC
#define NOALLOC(x) /* allocation error macro */                             \
{                                                                           \
    fprintf(stderr,"\n***** Unable to allocate enough memory for %s\n",x);  \
    QUIT;                                                                   \
}
#endif


#ifdef __cplusplus

extern "C" void** AllocArray2D(int nRows,
                               int nCols,
                               int nSize);

extern "C" void*** AllocArray3D(int nPlanes,
                                int nRows,
                                int nCols,
                                int nSize);

// convert an integer into a comma-separated ASCII string
extern "C" char* Comma(int iValue);

#else //  __cplusplus

extern void** AllocArray2D(int nRows,
                           int nCols,
                           int nSize);

extern void*** AllocArray3D(int nPlanes,
                            int nRows,
                            int nCols,
                            int nSize);

// convert an integer into a comma-separated ASCII string
char* Comma(int iValue);

#endif // __cplusplus
