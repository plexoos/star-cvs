/*********************************************************************
 * $Id: StRichRrsMacros.h,v 2.0 2000/08/09 16:17:04 gans Exp $
 *  This file contains macros defined explicitly for RRS.
 *  The can be defined to limit the initialization/debug
 *  output of the code.
 *********************************************************************
 *
 * $Log: StRichRrsMacros.h,v $
 * Revision 2.0  2000/08/09 16:17:04  gans
 * Readded Files That were not added in last CVS. Cosmetic Changes, naming convention
 * for StRichDrawableT(foo)
 *
 * Revision 1.5  2000/04/05 16:09:18  lasiuk
 * misc
 *
 * Revision 1.4  2000/03/17 14:55:04  lasiuk
 * Large scale revisions after ROOT dependent memory leak
 *
 * Revision 1.3  2000/02/08 23:51:13  lasiuk
 * removal of rrs macro---CC4.2 cannot handle it!
 *
 * Revision 1.2  2000/02/08 16:30:23  lasiuk
 * addition of bounds check and diagnostic rrs debug
 *********************************************************************/
#define uSE_MEMORY_INFO 1
#define RRS_DEBUG 0
#define rICH_COORDINATE_BOUNDS_CHECK 1
#define rICH_WITH_VIEWER 1
#define rICH_WITH_MONITOR 1
