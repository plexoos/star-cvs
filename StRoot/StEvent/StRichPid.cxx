/***************************************************************************
 *
 * $Id: StRichPid.cxx,v 2.3 2000/11/21 19:47:33 lasiuk Exp $
 *
 * Author: Matt Horsley, Sep 2000
 ***************************************************************************
 *
 * Description: Definition of PID object
 *
 ***************************************************************************
 *
 * $Log: StRichPid.cxx,v $
 * Revision 2.3  2000/11/21 19:47:33  lasiuk
 * add the d information for each hit
 * use the TArrayF
 *
 * Revision 2.3  2000/11/21 19:47:33  lasiuk
 * add the d information for each hit
 * use the TArrayF
 *
 * Revision 2.2  2000/11/01 16:45:43  lasiuk
 * Keep the pointers to the hits that are associated with the track
 * in order to use the bit flag information.  These are kept
 * in an StPtrVec (does not own the hits)  The PDG encoded number
 * is kept as a data member now
 *
 * Revision 2.1  2000/09/28 10:53:56  ullrich
 * Initial Revision.
 *
 ***************************************************************************/
#include "StRichPid.h"

static const char rcsid[] = "$Id: StRichPid.cxx,v 2.3 2000/11/21 19:47:33 lasiuk Exp $";

ClassImp(StRichPid)

StRichPid::StRichPid()
    : mParticleNumber(0),
      mTotalAzimuth(0), mTotalArea(0), mTotalHits(0), mTotalDensity(0),
      mTruncatedAzimuth(0), mTruncatedArea(0), mTruncatedHits(0), mTruncatedDensity(0) {/* noop */}

StRichPid::StRichPid(StParticleDefinition* type, StThreeVectorD resid, Float_t totAzim,
                     Float_t totArea, UShort_t totHits, Float_t trunAzim, Float_t trunArea,
                     UShort_t trunHits)
    : mMipResidual(resid), mTotalAzimuth(totAzim), mTotalArea(totArea),
      mTotalHits(totHits), mTruncatedAzimuth(trunAzim), mTruncatedArea(trunArea),
      mTruncatedHits(trunHits) {

    mParticleNumber = type->pdgEncoding();
    
    if (mTotalArea>0) {mTotalDensity=mTotalHits/mTotalArea;}
    else mTotalDensity=0;
    
    if (mTruncatedArea>0) {mTruncatedDensity=mTruncatedHits/mTruncatedArea;}
    else mTruncatedDensity=0;
}

StRichPid::~StRichPid() {/* noop */}

void StRichPid::setRingType(StParticleDefinition* t)   {
    mParticleType=t;
    mParticleNumber = t->pdgEncoding();
}

TArrayF& StRichPid::getDVector() {return mDDistribution;}

float StRichPid::getD(int i) {
    //
    // should really be a constant
    // function, but ROOT does not
    // use const to define At(int)

    //
    // Bounds Check
    //
    if(i>=0 && mDDistribution.GetSize() < i) {
	return mDDistribution.At(i);
    }
    else {
	cout << "StRichPid::getD()\n";
	cout << "\tERROR: out of bounds\n";
	cout << "\treturn -999" << endl;
	return -999.;
    }
}

void StRichPid::addNormalizedD(float d) {

    int currentSize = mDDistribution.GetSize();
    mDDistribution.Set(currentSize+1);

    mDDistribution.AddAt(d,currentSize);
}

Int_t
StRichPid::operator==(const StRichPid& pid) const {
    return ( mParticleNumber   == pid.getParticleNumber() &&
	     mMipResidual      == pid.getMipResidual() &&
	     mTotalAzimuth     == pid.getTotalAzimuth() &&
	     mTotalArea        == pid.getTotalArea() &&
	     mTotalHits        == pid.getTotalHits() &&
	     mTotalDensity     == pid.getTotalDensity() &&
	     mTruncatedAzimuth == pid.getTruncatedAzimuth() &&
	     mTruncatedArea    == pid.getTruncatedArea() &&
	     mTruncatedHits    == pid.getTruncatedHits() &&
	     mTruncatedDensity == pid.getTruncatedDensity() );
}
