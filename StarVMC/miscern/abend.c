/*
 * $Id: abend.c,v 1.1 2005/05/26 22:27:59 fisyak Exp $
 *
 * $Log: abend.c,v $
 * Revision 1.1  2005/05/26 22:27:59  fisyak
 * Add stuff necessary for reconstruction packages
 *
 * Revision 1.1.1.1  2004/07/17 20:01:56  perev
 * STAR version of Geant321 TGeant3 etc
 *
 * Revision 1.2  2002/12/02 16:37:45  brun
 * Changes from Federico Carminati and Peter Hristov who ported the system
 * on the Ithanium processors.It is tested on HP, Sun, and Alpha, everything
 * seems to work. The optimisation is switched off in case of gcc2.xx.yyy
 *
 * Revision 1.1.1.1  2002/07/24 15:56:28  rdm
 * initial import into CVS
 *
 * Revision 1.1.1.1  2002/06/16 15:18:46  hristov
 * Separate distribution  of Geant3
 *
 * Revision 1.1.1.1  1999/05/18 15:55:28  fca
 * AliRoot sources
 *
 * Revision 1.2  1997/02/04 17:34:12  mclareni
 * Merge Winnt and 97a versions
 *
 * Revision 1.1.1.1.2.1  1997/01/21 11:29:22  mclareni
 * All mods for Winnt 96a on winnt branch
 *
 * Revision 1.1.1.1  1996/02/15 17:49:20  mclareni
 * Kernlib
 *
 */
#include "kerngen/pilot.h"
#include "kerngen/fortranc.h"
#include "stdlib.h"

/*>    ROUTINE ABEND
  CERN PROGLIB# Z035    ABEND           .VERSION KERNFOR  4.31  911111
*/
#if defined(CERNLIB_QX_SC)
void type_of_call abend_()
#endif
#if defined(CERNLIB_QXNO_SC)
void type_of_call abend()
#endif
#if defined(CERNLIB_QXCAPT)
void type_of_call ABEND()
#endif
{
    exit(7);
}
/*> END <----------------------------------------------------------*/
#ifdef CERNLIB_TCGEN_ABEND
#undef CERNLIB_TCGEN_ABEND
#endif
