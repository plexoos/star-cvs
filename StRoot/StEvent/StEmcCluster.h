/***************************************************************************
 *
 * $Id: StEmcCluster.h,v 2.4 2001/03/24 03:34:44 perev Exp $
 *
 * Author: Akio Ogawa, Jan 2000
 ***************************************************************************
 *
 * Description: Base class for electromagnetic calorimeter cluster
 *
 ***************************************************************************
 *
 * $Log: StEmcCluster.h,v $
 * Revision 2.4  2001/03/24 03:34:44  perev
 * clone() -> clone() const
 *
 * Revision 2.3  2000/07/31 22:12:23  akio
 * eliminate print() for L3(?)
 *
 * Revision 2.2  2000/03/23 22:24:06  akio
 * Initial version of Emc Point, and Inclusion of track pointers
 *
 * Revision 2.1  2000/02/23 17:55:43  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StEmcCluster_hh
#define StEmcCluster_hh

#include <iostream.h>
#include "StObject.h"
#include "StContainers.h"

class StEmcCluster : public StObject {
public: 
    StEmcCluster();
    ~StEmcCluster();
    // StEmcCluster(const StEmcCluster&);            use default
    // StEmcCluster& operator=(const StEmcCluster&); use default
    
    Float_t eta() const;
    Float_t phi() const;
    Float_t sigmaEta() const;
    Float_t sigmaPhi() const;
    Float_t energy() const;
    Int_t   nHits() const; 
    Int_t   nNeighbors() const;
    Int_t   nTracks() const;
    
    StPtrVecEmcRawHit& hit();
    const StPtrVecEmcRawHit& hit() const;
    StPtrVecEmcCluster& neighbor();
    const StPtrVecEmcCluster& neighbor() const;
    StPtrVecTrack& track();
    const StPtrVecTrack& track() const;
    
    void setEta(Float_t);
    void setPhi(Float_t);
    void setSigmaEta(Float_t);
    void setSigmaPhi(Float_t);
    void setEnergy(Float_t);
    
    void addHit(StEmcRawHit*);
    void addNeighbor(StEmcCluster*);
    void addTrack(StTrack*);
    
private:
    Float_t mEta;
    Float_t mPhi;
    Float_t mSigmaEta;
    Float_t mSigmaPhi;
    Float_t mEnergy;
    StPtrVecEmcRawHit  mHits;
    StPtrVecEmcCluster mNeighbors;
    StPtrVecTrack      mTracks;
    
    StObject* clone() const;
    ClassDef(StEmcCluster,1)
};

ostream& operator<<(ostream&, const StEmcCluster&); // Printing operator
#endif






