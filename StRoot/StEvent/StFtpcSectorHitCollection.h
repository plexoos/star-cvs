/***************************************************************************
 *
 * $Id: StFtpcSectorHitCollection.h,v 2.1 1999/10/13 19:43:11 ullrich Exp $
 *
 * Author: Thomas Ullrich, July 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StFtpcSectorHitCollection.h,v $
 * Revision 2.1  1999/10/13 19:43:11  ullrich
 * Initial Revision
 *
 * Revision 2.1  1999/10/13 19:43:11  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StFtpcSectorHitCollection_hh
#define StFtpcSectorHitCollection_hh

#include "StObject.h"
#include "StArray.h"

class StFtpcHit;

class StFtpcSectorHitCollection : public StObject {
public:
    StFtpcSectorHitCollection();
    // StFtpcSectorHitCollection(const StFtpcSectorHitCollection&);            use default
    // StFtpcSectorHitCollection& operator=(const StFtpcSectorHitCollection&); use default
    ~StFtpcSectorHitCollection();
    
    StSPtrVecFtpcHit&       hits();
    const StSPtrVecFtpcHit& hits() const;

private:
    StSPtrVecFtpcHit mHits;
    
    ClassDef(StFtpcSectorHitCollection,1)
};
#endif
