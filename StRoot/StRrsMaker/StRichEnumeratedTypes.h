/****************************************************************
 * $Id: StRichEnumeratedTypes.h,v 2.2 2001/02/07 16:09:27 lasiuk Exp $
 *
 * Description:
 *   Enumerated type definitions
 *
 ****************************************************************
 *
 * $Log: StRichEnumeratedTypes.h,v $
 * Revision 2.2  2001/02/07 16:09:27  lasiuk
 * add flags for charged sharing (pixels)
 *
 * Revision 2.1  2000/09/13 20:59:05  lasiuk
 * add cluster/hit flags
 *
 * Revision 2.0  2000/08/09 16:17:00  gans
 * Readded Files That were not added in last CVS. Cosmetic Changes, naming convention
 * for StRichDrawableT(foo)
 *
 * Revision 1.4  2000/05/17 22:19:22  lasiuk
 * noise type
 *
 * Revision 1.3  2000/04/05 15:58:35  lasiuk
 * set bits instead of incremental numbers
 *
 * Revision 1.2  2000/03/17 14:54:26  lasiuk
 * Large scale revisions after ROOT dependent memory leak
 *
 * Revision 1.1  2000/02/29 18:14:09  lasiuk
 * Initial Revision
 *
 ****************************************************************/
#ifndef ST_RICH_ENUMERATED_TYPES_H
#define ST_RICH_ENUMERATED_TYPES_H

//
// Bit operations for the cluster finder
//
enum StRichSinglePixelFlag {eUsed=1, eBorder=2, eGood=4, eIsolated=8, eLocalMaximum=16, eNotACentralMaximum=32, eMaxHasAVerticalNeighbor=64, eMaxHasAHorizontalNeighbor=128, eSaturatedPixel=256, eToBeUsedOnce=512, eToBeUsedTwice=1024,eCurrentLocalMaximum=2048};

//
// MC flags for repsonsible particle
//
enum StRichSignalType {eUnknown=0, eCharged=1, ePhoton=2, eFeedback=4, eNoise=8};
#endif
