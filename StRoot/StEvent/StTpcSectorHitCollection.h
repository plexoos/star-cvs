/***************************************************************************
 *
 * $Id: StTpcSectorHitCollection.h,v 2.2 2001/04/05 04:00:44 ullrich Exp $
 *
 * Author: Thomas Ullrich, July 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTpcSectorHitCollection.h,v $
 * Revision 2.2  2001/04/05 04:00:44  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.1  1999/10/13 19:44:06  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StTpcSectorHitCollection_hh
#define StTpcSectorHitCollection_hh

#include "StObject.h"
#include "StTpcPadrowHitCollection.h"

class StTpcSectorHitCollection : public StObject {
public:
    StTpcSectorHitCollection();
    ~StTpcSectorHitCollection();
    // StTpcSectorHitCollection(const StTpcSectorHitCollection&);            use default
    // StTpcSectorHitCollection& operator=(const StTpcSectorHitCollection&); use default
    
    unsigned int numberOfHits() const;
    unsigned int numberOfPadrows() const;
    
    StTpcPadrowHitCollection*       padrow(unsigned int);
    const StTpcPadrowHitCollection* padrow(unsigned int) const;

private:
    enum { mNumberOfPadrows = 45 };
    StTpcPadrowHitCollection mPadrows[mNumberOfPadrows];
    
    ClassDef(StTpcSectorHitCollection,1)
};
#endif
