/***************************************************************************
 *
 * $Id: StHit.cxx,v 2.19 2006/01/19 21:49:41 ullrich Exp $
 *
 * Author: Thomas Ullrich, Sept 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StHit.cxx,v $
 * Revision 2.19  2006/01/19 21:49:41  ullrich
 * Changed order of initializer in constructor.
 *
 * Revision 2.18  2005/07/19 21:34:10  perev
 * quality ==> qaTruth to avoid misleading
 *
 * Revision 2.17  2005/07/06 18:57:48  fisyak
 * Add StHit print out
 *
 * Revision 2.16  2004/10/13 16:10:52  ullrich
 * Fixed bug in operator==
 *
 * Revision 2.15  2004/08/06 15:37:09  fisyak
 * Add clster id
 *
 * Revision 2.14  2004/07/30 22:28:31  fisyak
 * Add transient pointer to next Hit
 *
 * Revision 2.13  2004/07/15 16:36:24  ullrich
 * Removed all clone() declerations and definitions. Use StObject::clone() only.
 *
 * Revision 2.12  2004/03/30 15:59:08  calderon
 * Added method to set mFitFlag (new chain no longer uses tables, so must set
 * this by hand).
 *
 * Revision 2.11  2004/01/13 21:01:32  fisyak
 * Add Truth and Quality information from simulation
 *
 * Revision 2.10  2001/04/05 04:00:51  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.9  2001/03/24 03:34:50  perev
 * clone() -> clone() const
 *
 * Revision 2.8  2001/03/06 21:04:30  ullrich
 * Modified detector() method. Replaced switch
 * statement by simple static_cast.
 *
 * Revision 2.7  2000/12/08 20:21:07  genevb
 * Changed kTofPatchId -> kTofId
 *
 * Revision 2.6  2000/07/28 23:29:42  calderon
 * Added handling of Fit Flag: use this flag to tell if the point
 * is used in the fit.
 *
 * Revision 2.5  2000/07/28 19:49:28  akio
 * Change in Detector Id for Endcap SMD
 *
 * Revision 2.4  2000/06/07 09:43:17  ullrich
 * Changed return type of flag() to unsigned int
 *
 * Revision 2.3  2000/06/01 21:38:53  ullrich
 * Added member mFlag and access member flag() and setFlag().
 *
 * Revision 2.2  2000/05/19 18:33:14  ullrich
 * Minor changes (add const) to cope with modified StArray.
 *
 * Revision 2.1  1999/10/28 22:25:47  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:42:17  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include "StHit.h"
#include "StTrack.h"
#include "StTrackNode.h"
#include "StTrackDetectorInfo.h"

static const char rcsid[] = "$Id: StHit.cxx,v 2.19 2006/01/19 21:49:41 ullrich Exp $";

ClassImp(StHit)

StHit::StHit()
{
    mHardwarePosition = 0;
    mCharge = 0;
    mTrackRefCount = 0;
    mFlag = mFitFlag = 0;
    mIdTruth = mQuality = 0;
    mId = 0;
    mNextHit = 0;
}

StHit::StHit(const StThreeVectorF& p,
             const StThreeVectorF& e,
             unsigned int hp, float q, unsigned char c, UShort_t idTruth, UShort_t quality, UShort_t id)
    : StMeasuredPoint(p), mPositionError(e), mHardwarePosition(hp),
      mCharge(q), mTrackRefCount(c), 
      mIdTruth(idTruth), mQuality(quality), mId(id), mNextHit(0)
{
    mFlag = mFitFlag = 0;
}

StHit::~StHit() { /* noop */ }

int
StHit::operator==(const StHit& h) const
{
    return h.mPosition         == mPosition &&
           h.mPositionError    == h.mPositionError &&
           h.mCharge           == mCharge &&
           h.mHardwarePosition == mHardwarePosition &&
           h.mFlag             == mFlag &&
           h.mIdTruth          == mIdTruth &&
           h.mQuality          == mQuality &&
           h.mId               == mId &&
           h.mNextHit          == mNextHit;
}

int
StHit::operator!=(const StHit& h) const
{
    return !(*this == h);  // use operator==()
}

void
StHit::setCharge(float val) { mCharge = val; }

void
StHit::setTrackReferenceCount(unsigned char val) { mTrackRefCount = val; }
    
void
StHit::setFlag(unsigned char val) { mFlag = val; }
    
void
StHit::setFitFlag(unsigned char val) { mFitFlag = val; }
    
void
StHit::setHardwarePosition(unsigned int val) { mHardwarePosition = val; }

void
StHit::setPositionError(const StThreeVectorF& e) { mPositionError = e; }
    
float
StHit::charge() const { return mCharge; }

unsigned int
StHit::flag() const { return static_cast<unsigned int>(mFlag); }

int
StHit::usedInFit() const { return static_cast<int>(mFitFlag); }

unsigned int
StHit::trackReferenceCount() const { return static_cast<unsigned int>(mTrackRefCount); }

StDetectorId
StHit::detector() const
{
    return static_cast<StDetectorId>(bits(0, 4));
}

StThreeVectorF
StHit::positionError() const { return mPositionError; }
   
StMatrixF
StHit::covariantMatrix() const
{
    // for now the diagonal elements is all we have
    StMatrixF m(3,3);
    m(1,1) = mPositionError.x()*mPositionError.x();
    m(2,2) = mPositionError.y()*mPositionError.y();
    m(3,3) = mPositionError.z()*mPositionError.z();
    return m;
}

StPtrVecTrack
StHit::relatedTracks(const StSPtrVecTrackNode& nodes, StTrackType type)
{
    StPtrVecTrack vec;
    StPtrVecHit hvec;
    StDetectorId id = this->detector();

    for (unsigned int i=0; i<nodes.size(); i++) {
        const StTrackNode *node = nodes[i];
        unsigned int ntracks = node->entries(type);
        for (unsigned int k=0; k<ntracks; k++) {
            const StTrack *track = node->track(type, k);
            hvec = track->detectorInfo()->hits(id);
            for (unsigned int j=0; j<hvec.size(); j++)
                if (hvec[j] == this) vec.push_back(const_cast<StTrack*>(track));
        }
    }
    return vec;
}

void StHit::setIdTruth(int idtru,int qatru) 
{
  if (qatru==0) qatru = (idtru>>16);
  idtru    = idtru&((1<<16)-1);
  mIdTruth = (UShort_t)(idtru);
  mQuality = (UShort_t) qatru;
}

int StHit::idTruth() const
{
  return mIdTruth;
}
ostream&  operator<<(ostream& os, const StHit& v)
{
  return os << "StHit: charge\t" << v.charge() 
	    << "\ttrackReferenceCount " << v.trackReferenceCount()
	    << "\tdetector " << v.detector() 
	    << "\tposition " << v.position()
	    << "\tpositionError " << v.positionError()
	    << "\tcovariantMatrix " << v.covariantMatrix()
	    << "\tusedInFit " << v.usedInFit()
	    << "\tidTruth " << v.idTruth()
	    << "\tquality " << v.qaTruth()
	    << "\tid " << v.id() 
	    << "\thardwarePosition " << v.hardwarePosition();
}

void
StHit::Print(Option_t *option) const {cout << *this << endl;}
