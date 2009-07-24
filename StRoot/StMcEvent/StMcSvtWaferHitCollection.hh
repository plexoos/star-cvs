/***************************************************************************
 *
 * $Id: StMcSvtWaferHitCollection.hh,v 2.4 2009/07/24 19:08:08 perev Exp $
 *
 * Author: Manuel Calderon de la Barca Sanchez, Oct 1999
 ***************************************************************************
 *
 * Description: Monte Carlo Svt Wafer Hit Collection class
 *
 ***************************************************************************
 *
 * $Log: StMcSvtWaferHitCollection.hh,v $
 * Revision 2.4  2009/07/24 19:08:08  perev
 * Cleanup + Btof added (Geurts)
 *
 * Revision 2.3  2005/01/27 23:40:48  calderon
 * Adding persistency to StMcEvent as a step for Virtual MonteCarlo.
 *
 * Revision 2.2  2000/03/06 18:05:23  calderon
 * 1) Modified SVT Hits storage scheme from layer-ladder-wafer to
 * barrel-ladder-wafer.
 * 2) Added Rich Hit class and collection, and links to them in other
 * classes.
 *
 * Revision 2.1  1999/11/19 19:06:33  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:01:00  calderon
 * Completely revised for new StEvent
 *
 *
 **************************************************************************/
#ifndef StMcSvtWaferHitCollection_hh
#define StMcSvtWaferHitCollection_hh
#include "StMcContainers.hh"
#include "StObject.h"

class StMcSvtHit;

class StMcSvtWaferHitCollection : public StObject
{
public:
    StMcSvtWaferHitCollection();
    // StMcSvtWaferHitCollection(const StMcSvtWaferHitCollection&); use default
    // const StMcSvtWaferHitCollection& operator=(const StMcSvtWaferHitCollection&); use default
    virtual ~StMcSvtWaferHitCollection();
    
    StSPtrVecMcSvtHit&       hits();
    const StSPtrVecMcSvtHit& hits() const;

private:
    StSPtrVecMcSvtHit mHits;
    ClassDef(StMcSvtWaferHitCollection,1)
};
#endif
