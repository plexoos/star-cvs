/***************************************************************************
 *
 * $Id: StTrackFitTraits.h,v 2.5 2001/03/24 03:35:00 perev Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTrackFitTraits.h,v $
 * Revision 2.5  2001/03/24 03:35:00  perev
 * clone() -> clone() const
 *
 * Revision 2.4  2001/03/16 21:31:42  ullrich
 * Changed version number from 1 to 2.
 *
 * Revision 2.3  2001/03/16 20:57:45  ullrich
 * Covariant matrix now stored in TArrayF.
 *
 * Revision 2.2  1999/11/01 12:45:17  ullrich
 * Modified unpacking of point counter
 *
 * Revision 2.1  1999/10/28 22:27:35  ullrich
 * Adapted new StArray version. First version to compile on Linux and Sun.
 *
 * Revision 2.0  1999/10/12 18:43:02  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#ifndef StTrackFitTraits_hh
#define StTrackFitTraits_hh
#include "StObject.h"
#include "StEnumerations.h"
#include "StMatrixF.hh"
#include "TArrayF.h"

class dst_track_st;
class StParticleDefinition;

class StTrackFitTraits : public StObject {
public:
    StTrackFitTraits();
    StTrackFitTraits(UShort_t, UShort_t, Float_t[2], Float_t[15]);
    StTrackFitTraits(const dst_track_st&);
    // StTrackFitTraits(const StTrackFitTraits&);            use default
    // StTrackFitTraits& operator=(const StTrackFitTraits&); use default
    virtual ~StTrackFitTraits();

    UShort_t              numberOfFitPoints() const;
    UShort_t              numberOfFitPoints(StDetectorId) const;
    StParticleDefinition* pidHypothesis() const;
    StMatrixF             covariantMatrix() const;
    Double_t              chi2(UInt_t = 0) const;

    void                  clearCovariantMatrix();
    
protected:
    UShort_t mPidHypothesis;       // GeantId
    UShort_t mNumberOfFitPoints;
    Float_t  mChi2[2];
    TArrayF  mCovariantMatrix;
    
    ClassDef(StTrackFitTraits,2) //!OWNSTREAMER
};
#endif
