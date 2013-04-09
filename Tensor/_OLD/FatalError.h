/* Taken from MTW project by Abedul Haque, December 2009		*/
/* This file has not been modified at all						*/
/* used for CS3610 class project								*/

#pragma once

// macro to use FatalError() more easily
#define FATAL_ERROR(x)  FatalError(x,__LINE__,__FILE__)

// declaration of the global FatalError() routine
void FatalError(char* szpErrorMessage, int iLineNumber, char* szpFileName);
