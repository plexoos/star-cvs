/*!
 * \class StHit 
 * \author Thomas Ullrich, Jan 1999
 */
/***************************************************************************
 *
 * $Id: StHit.h,v 2.15 2004/08/18 19:00:19 ullrich Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StHit.h,v $
 * Revision 2.15  2004/08/18 19:00:19  ullrich
 * Added access function hardwarePosition().
 *
 * Revision 2.14  2004/08/06 15:37:09  fisyak
 * Add clster id
 *
 * Revision 2.13  2004/07/30 22:28:31  fisyak
 * Add transient pointer to next Hit
 *
 * Revision 2.12  2004/07/15 16:36:24  ullrich
 * Removed all clone() declerations and definitions. Use StObject::clone() only.
 *
 * Revision 2.11  2004/03/30 15:59:08  calderon
 * Added method to set mFitFlag (new chain no longer uses tables, so must set
 * this by hand).
 *
 * Revision 2.10  2004/01/13 21:01:32  fisyak
 * Add Truth and Quality information from simulation
 *
 * Revision 2.9  2002/02/22 22:56:48  jeromel
 * Doxygen basic documentation in all header files. None of this is required
 * for QM production.
 *
 * Revision 2.8  2001/04/25 17:46:56  jeromel
 * Remove last change. There is another way to fix the problems seen (the right way
 * actually) which Thomas will take care off.
 *
 * Revision 2.7  2001/04/25 15:57:22  jeromel
 * Fixed cint problem with StContainers.h
 *
 * Revision 2.6  2001/04/05 04:00:38  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.5  2001/03/24 03:34:50  perev
 * clone() -> clone() const
 *
 * Revision 2.4  2000/07/28 23:29:42  calderon
 * Added handling of Fit Flag: use this flag to tell if the point
 * is used in the fit.
 *
 * Revision 2.3  2000/06/07 09:43:21  ullrich
 * Changed return type of flag() to unsigned int
 *
 * Revision 2.2  2000/06/01 21:38:56  ullrich
 * Added member mFlag and access member flag() and setFlag().
 *
 * Revision 2.1  1999/10/28 22:25:50  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:42:21  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#ifndef StHit_hh
#define StHit_hh

#include "StMeasuredPoint.h"
#include "StEnumerations.h"
#include "StContainers.h"

class StTrackNode;
class StTrack;

class StHit : public StMeasuredPoint {
public:
    StHit();
    StHit(const StThreeVectorF&,
          const StThreeVectorF&,
          unsigned int, float, unsigned char = 0,
	  UShort_t idTruth=0, UShort_t quality=0, UShort_t id =0);
    // StHit(const StHit&);            use default
    // StHit& operator=(const StHit&); use default
    ~StHit();

    int operator==(const StHit&) const;
    int operator!=(const StHit&) const;
    
    float           charge() const;
    unsigned int    trackReferenceCount() const;
    StDetectorId    detector() const;
    unsigned int    flag() const;
    StThreeVectorF  positionError() const;     // overwrite inherited
    StMatrixF       covariantMatrix() const;   // overwrite inherited
    int             usedInFit() const;
    unsigned short  idTruth() const;
    unsigned short  quality() const;
    unsigned short  id()      const;
    StHit*          nextHit() const;
    unsigned int    hardwarePosition() const;
    
    
    void setCharge(float);
    void setFlag(unsigned char);
    void setFitFlag(unsigned char);
    void setTrackReferenceCount(unsigned char);
    void setHardwarePosition(unsigned int);
    void setPositionError(const StThreeVectorF&);
    void setIdTruth(UShort_t id=0) {mIdTruth=id;}
    void setQuality(UShort_t quality=0) {mQuality = quality;}
    void SetNextHit(StHit *next = 0) {mNextHit = next;}
    virtual StPtrVecTrack relatedTracks(const StSPtrVecTrackNode&, StTrackType);
    
protected:
    unsigned int bits(unsigned int, unsigned int) const;
    
    UInt_t         mHardwarePosition;
    Float_t        mCharge;
    UChar_t        mTrackRefCount;
    StThreeVectorF mPositionError;
    UChar_t        mFlag;
    UChar_t        mFitFlag;
    UShort_t       mIdTruth; // simulation track id 
    UShort_t       mQuality; // quality of this information (percentage of charge produced by mIdTruth)
    UShort_t       mId;
    StHit*         mNextHit; //!
    ClassDef(StHit,3)
};

inline unsigned int StHit::bits(unsigned int bit, unsigned int nbits) const
{
    return (mHardwarePosition>>bit) & ~(~0UL<<nbits);
}

inline unsigned short  StHit::idTruth() const {return mIdTruth;}
inline unsigned short  StHit::quality() const {return mQuality;}
inline unsigned short  StHit::id()      const {return mId;}
inline unsigned int    StHit::hardwarePosition() const {return mHardwarePosition;}

#endif
