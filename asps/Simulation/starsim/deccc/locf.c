/*
 * $Id: locf.c,v 1.2 2018/11/21 23:15:26 perev Exp $
 *
 * $Log: locf.c,v $
 * Revision 1.2  2018/11/21 23:15:26  perev
 * Test for zero added
 *
 * Revision 1.1  2018/11/19 23:20:12  perev
 * 64bits new comis files added from /CERN
 *
 * Revision 1.4  2004/07/29 14:06:07  mclareni
 * Alice version for 64-bit pointer systems using the CERNLIB_QMLXIA64 cpp flag
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
 * Revision 1.3  1997/09/02 14:26:38  mclareni
 * WINNT correction
 *
 * Revision 1.2  1997/02/04 17:34:35  mclareni
 * Merge Winnt and 97a versions
 *
 * Revision 1.1.1.1.2.1  1997/01/21 11:29:36  mclareni
 * All mods for Winnt 96a on winnt branch
 *
 * Revision 1.1.1.1  1996/02/15 17:49:24  mclareni
 * Kernlib
 *
 */
/*>    ROUTINE LOCF
  CERN PROGLIB# N100    LOCF            .VERSION KERNFOR  4.36  930602
*/
#include <assert.h>
int csToken(unsigned long fun);
unsigned long  csPoter( int token); 
#define kMASK 0x40000000
#define kMAZK 0xE0000000
int           csvptokn_(unsigned long addr);
unsigned long csvplong (         int  tokn);




  static unsigned long myBase=(unsigned long)&myBase;
//static unsigned long myBase=0;

unsigned long longf_(char *iadr )
{
  return ((unsigned long)iadr)/sizeof(int);
}

int  locf_(char *iadr )
{
  int myDif = (((unsigned long)iadr)>>2) - ((myBase)>>2);
  assert((((myBase)>>2)+myDif)<<2 ==(unsigned long)iadr);
  assert((myDif&kMAZK)!=kMASK);
  return myDif;
}

int  locb_(char *iadr )
{
  assert(myBase>=0);
  int myDif = (unsigned long)iadr - myBase;
  assert((((myBase))+myDif) ==(unsigned long)iadr);
  assert((myDif&kMAZK)!=kMASK);
  return myDif;
}
char *getPntF(int myDif)
{
  if (!myDif) return 0;
  if ((myDif&kMAZK)==kMASK) {
    return (char*)csvplong(myDif);
  } else {
    return (((myBase)>>2)+myDif)<<2;
  }
}
char *getPntB(int myDif)
{
  if (!myDif) return 0;
  if ((myDif&kMAZK)==kMASK) {
    char *ret = (char*)csvplong(myDif);
    return ret;
  } else {
    return (char*)myBase+myDif;
  }
}
int getbyteb_(int *myDif)
{
  return *getPntB(*myDif);
}
int getbytef_(int *myDif)
{
  return *getPntF(*myDif);
}
int getfun2b_(int *myFun)
{
  unsigned long fun = csPoter( *myFun);

  return locb_(&fun);
}
int getf2b_(int *myFun)
{
  unsigned long uk = csPoter(*myFun);
  return locb_(uk);
}
