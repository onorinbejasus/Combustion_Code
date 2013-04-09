/* Taken from MTW project by Abedul Haque, December 2009		*/
/* This file has not been modified at all						*/
/* used for CS3610 class project								*/

// FatalError.cpp : implementation file
//

//#include "stdafx.h"
#include "FatalError.h"
#include "assert.h"
#include "stdio.h"

// global routine to elicit a fatal error message box
void FatalError(char* szpErrorMessage, int iLineNumber, char* szpFileName)
{
    printf("Fatal error: %s\n", szpErrorMessage);
/*    char szWork[2000];
    sprintf(szWork,
            "A FATAL ERROR HAS OCCURRED!\n\n"
            "Error description:\n"
            "%s\n\n"
            "Source-code reference: Line number %d of program module\n"
            "%s",
            szpErrorMessage,
            iLineNumber,
            szpFileName);
    AfxMessageBox(szWork,
                  MB_OK | MB_ICONSTOP);
    assert(0);
    exit(1);
	*/
}

