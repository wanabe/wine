/*
 * DOS interrupt 20h handler (TERMINATE PROGRAM)
 */

#include <stdlib.h>
#include "winbase.h"
#include "miscemu.h"
/* #define DEBUG_INT */
#include "debugtools.h"
#include "task.h"
#include "dosexe.h"

/**********************************************************************
 *	    INT_Int20Handler
 *
 * Handler for int 20h.
 */
void WINAPI INT_Int20Handler( CONTEXT86 *context )
{
        MZ_Exit( context, TRUE, 0 );
}
