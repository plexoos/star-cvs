/***************************************************************************
 *
 * $Id: StTrackTopologyMap.h,v 2.7 2001/04/24 21:32:07 genevb Exp $
 *
 * Author: Thomas Ullrich, AUg 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrackTopologyMap.h,v $
 * Revision 2.7  2001/04/24 21:32:07  genevb
 * Additional helper functions
 *
 * Revision 2.6  2001/04/05 04:00:46  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.5  2000/05/17 17:21:38  ullrich
 * New method largestGap() and new output operator.
 *
 * Revision 2.4  2000/04/12 19:44:03  genevb
 * Reimplement mMap data members as individual unsigned ints
 *
 * Revision 2.3  2000/04/10 19:59:33  genevb
 * StRoot/StEvent/doc/tex/
 *
 * Revision 2.2  1999/12/13 20:16:39  ullrich
 * Changed numbering scheme for hw_position unpack methods (STAR conventions).
 *
 * Revision 2.1  1999/10/13 19:44:17  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StTrackTopologyMap_hh
#define StTrackTopologyMap_hh

#include <iostream.h>
#include "StObject.h"
#include "StEnumerations.h"

class StTrackTopologyMap : public StObject {
public:
    StTrackTopologyMap();
    StTrackTopologyMap(unsigned int, unsigned int);
    StTrackTopologyMap(const unsigned long*);
    StTrackTopologyMap(const unsigned int*);
    // StTrackTopologyMap(const StTrackTopologyMap&);            use default
    // StTrackTopologyMap& operator=(const StTrackTopologyMap&); use default
    ~StTrackTopologyMap();

    bool           primaryVertexUsed() const;
    unsigned int   numberOfHits(StDetectorId) const;
    bool           hasHitInDetector(StDetectorId) const;
    bool           hasHitInRow(StDetectorId, unsigned int) const; // first row = 1
    bool           hasHitInSvtLayer(unsigned int) const;          // first layer = 1

    bool           trackTpcOnly() const;
    bool           trackSvtOnly() const;
    bool           trackTpcSvt() const;
    bool           trackFtpcEast() const;
    bool           trackFtpcWest() const;
    bool           trackFtpc() const;

    bool           turnAroundFlag() const;
    unsigned int   data(unsigned int) const;

    int            largestGap(StDetectorId) const;
    
protected:
    bool bit(int) const;             // range 0-63
    bool ftpcFormat() const;
    
private:
    UInt_t  mMap0;
    UInt_t  mMap1;
    
    ClassDef(StTrackTopologyMap,1)
};

ostream& operator<< (ostream&, const StTrackTopologyMap&);
#endif
