/* Taken from MTW project by Abedul Haque, December 2009		*/
/* for SSE check purpose										*/
/* This file has not been modified at all						*/
/* used for CS3610 class project								*/

// CheckForSSE.cpp

// Two routines to confirm the presence of SSE and SSE2 in a processor

#include "stdio.h"
#include "CheckForSSE.h"

// This routine returns non-zero if SSE is present, zero if it is absent
int IfSSE(void)
{
    int iReturn = 0;
    __asm
    {
        pushad;
        mov     eax,1;
        cpuid;
        and     edx,0x02000000;
        mov     iReturn,edx;
        popad;
    }
    return iReturn;
}

// This routine returns non-zero if SSE2 is present, zero if it is absent
int IfSSE2(void)
{
    int iReturn = 0;
    __asm
    {
        pushad;
        mov     eax,1;
        cpuid;
        and     edx,0x04000000;
        mov     iReturn,edx;
        popad;
    }
    return iReturn;
}


/*int SseTest_main(int iArgCnt,char** szpArg)
{
    int iRet = IfSSE();
    if (iRet)
    {
        printf("SSE Present\n");
    }
    else
    {
        printf("SSE Absent\n");
    }
    iRet = IfSSE2();
    if (iRet)
    {
        printf("SSE2 Present\n");
    }
    else
    {
        printf("SSE2 Absent\n");
    }
    return 0;
}*/
