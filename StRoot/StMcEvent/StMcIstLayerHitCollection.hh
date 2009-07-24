/***************************************************************************
 *
 * $Id: StMcIstLayerHitCollection.hh,v 2.3 2009/07/24 19:08:07 perev Exp $
 *
 * Author: Fabrice Retiere/Kai Schweda, Aug 2003
 ***************************************************************************
 *
 * Description: Monte Carlo Ist Layer Hit Collection class from Kai
 *
 * The pixel detector hits are stored here.
 *
 ***************************************************************************
 *
 * $Log: StMcIstLayerHitCollection.hh,v $
 * Revision 2.3  2009/07/24 19:08:07  perev
 * Cleanup + Btof added (Geurts)
 *
 * Revision 2.2  2005/05/11 20:54:29  calderon
 * Added persistency: ClassImp, ClassDef and inheritance from StObject.
 *
 * Revision 2.1  2004/09/14 05:00:30  calderon
 * Added support for Ist, Ssd and changes to Pixel, from "El Kai".
 *
 * Revision 2.1  2003/08/20 18:50:21  calderon
 * Addition of Tof classes and Ist classes.  Modified track, event, and
 * container code to reflect this.
 * Fix bug in StMcVertex and in clearing of some hit collections.
 *
 *
 *
 **************************************************************************/
#ifndef StMcIstLayerHitCollection_hh
#define StMcIstLayerHitCollection_hh

#include "StMcContainers.hh"
#include "StObject.h"

class StMcIstHit;

class StMcIstLayerHitCollection : public StObject {
public:
    StMcIstLayerHitCollection();
    virtual ~StMcIstLayerHitCollection();
    
    unsigned long numberOfHits() const;

    StSPtrVecMcIstHit&       hits();
    const StSPtrVecMcIstHit& hits() const; 

private:
    StSPtrVecMcIstHit mHits;
    ClassDef(StMcIstLayerHitCollection,1)
};
#endif
