/***************************************************************************
 *
 * $Id: StHit.cxx,v 2.5 2000/07/28 19:49:28 akio Exp $
 *
 * Author: Thomas Ullrich, Sept 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StHit.cxx,v $
 * Revision 2.5  2000/07/28 19:49:28  akio
 * Change in Detector Id for Endcap SMD
 *
 * Revision 2.4  2000/06/07 09:43:17  ullrich
 * Changed return type of flag() to UInt_t
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

static const char rcsid[] = "$Id: StHit.cxx,v 2.5 2000/07/28 19:49:28 akio Exp $";

ClassImp(StHit)

StHit::StHit()
{
    mFlag = 0;
    mCharge = 0;
    mHardwarePosition = 0;
    mTrackRefCount = 0;
}

StHit::StHit(const StThreeVectorF& p,
             const StThreeVectorF& e,
             ULong_t hp, Float_t q, UChar_t c)
    : StMeasuredPoint(p), mHardwarePosition(hp),
      mCharge(q), mTrackRefCount(c), mPositionError(e)
{
    mFlag = 0;
}

StHit::~StHit() { /* noop */ }

StObject*
StHit::clone() { return new StHit(*this); }
   
Int_t
StHit::operator==(const StHit& h) const
{
    return h.mPosition         == mPosition &&
           h.mPositionError    == h.mPositionError &&
           h.mCharge           == mCharge &&
           h.mHardwarePosition == mHardwarePosition &&
	   h.mFlag             == mFlag;
}

Int_t
StHit::operator!=(const StHit& h) const
{
    return !(*this == h);  // use operator==()
}

void
StHit::setCharge(Float_t val) { mCharge = val; }

void
StHit::setTrackReferenceCount(UChar_t val) { mTrackRefCount = val; }
    
void
StHit::setFlag(UChar_t val) { mFlag = val; }
    
void
StHit::setHardwarePosition(ULong_t val) { mHardwarePosition = val; }

void
StHit::setPositionError(const StThreeVectorF& e) { mPositionError = e; }
    
Float_t
StHit::charge() const { return mCharge; }

UInt_t
StHit::flag() const { return static_cast<UInt_t>(mFlag); }

UInt_t
StHit::trackReferenceCount() const { return static_cast<UInt_t>(mTrackRefCount); }

StDetectorId
StHit::detector() const
{
    unsigned int id = bits(0, 4);
    switch (id) {
    case kTpcId:
        return kTpcId;
    case kSvtId:
        return kSvtId;
    case kRichId:
        return kRichId;
    case kFtpcWestId:
        return kFtpcWestId;
    case kFtpcEastId:
        return kFtpcEastId;
    case kTofPatchId:
        return kTofPatchId;
    case kCtbId:
        return kCtbId;
    case kSsdId:
        return kSsdId;
    case kBarrelEmcTowerId:
        return kBarrelEmcTowerId;
    case kBarrelEmcPreShowerId:
        return kBarrelEmcPreShowerId;
    case kBarrelSmdEtaStripId:
        return kBarrelSmdEtaStripId;
    case kBarrelSmdPhiStripId:
        return kBarrelSmdPhiStripId;
    case kEndcapEmcTowerId:
        return kEndcapEmcTowerId;
    case kEndcapEmcPreShowerId:
        return kEndcapEmcPreShowerId;
    case kEndcapSmdUStripId:
        return kEndcapSmdUStripId;
    case kEndcapSmdVStripId:
        return kEndcapSmdVStripId;
    case kZdcWestId:
        return kZdcWestId;
    case kZdcEastId:
        return kZdcEastId;
    case kMwpcWestId:
        return kMwpcWestId;
    case kMwpcEastId:
        return kMwpcEastId;
    case kTpcSsdId:
        return kTpcSsdId;
    case kTpcSvtId:
        return kTpcSvtId;
    case kTpcSsdSvtId:
        return kTpcSsdSvtId;
    case kSsdSvtId:
        return kSsdSvtId;
    case kUnknownId:
    default:
        return kUnknownId;
    }
}

StThreeVectorF
StHit::positionError() const { return mPositionError; }
   
StMatrixF
StHit::covariantMatrix() const
{
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

