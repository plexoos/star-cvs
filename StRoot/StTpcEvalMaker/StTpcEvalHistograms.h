// $Id: StTpcEvalHistograms.h,v 1.3 2000/08/07 03:25:14 snelling Exp $
// $Log: StTpcEvalHistograms.h,v $
// Revision 1.3  2000/08/07 03:25:14  snelling
// Added selection on tracks
//
// Revision 1.2  2000/05/25 20:38:09  snelling
// Added TPC evaluation histograms
//
//-----------------------------------------------------------------------
// header file for class StTpcEvalHistograms
//-----------------------------------------------------------------------
#ifndef StTpcEvalHistograms_H
#define StTpcEvalHistograms_H

#include "StObject.h"

class TH1F;
class TH2F;
class TNtuple;
class TProfile;
class TProfile2D;
class TClonesArray;
class MatchedTrackPair; //!

class StTpcEvalHistograms : public StObject {

 public:
  StTpcEvalHistograms();
  virtual ~StTpcEvalHistograms();
  void Book();
  void fillTrackNtuple(MatchedTrackPair*); //!

  TH1F*     tpcHitResX; //! local x difference
  TH1F*     tpcHitResY; //! local y difference (should be almost identical)
  TH1F*     tpcHitResZ; //! local z difference

  TH1F*     matchesToRcHits; //! 
  TH1F*     matchesToMcHits; //!

  TH1F*     mcHitPositionRad; //! for all M.C. hits
  TH1F*     mcHitPositionZ; //! for all M.C. hits

  TH1F*     mcUnmatchedHitPositionRad; //! for M.C. hits that are not matched to any rec. hit
  TH1F*     mcUnmatchedHitPositionSector; //! for M.C. hits that are not matched to any rec. hit
  TH1F*     mcUnmatchedHitPositionZ; //! for M.C. hits that are not matched to any rec. hit

  TH1F*     mc1to1HitPositionRad; //! for 1 to 1 matched btw Rec & M.C.
  TH1F*     mc1to1HitPositionZ; //! for 1 to 1 matched btw Rec & M.C.
    
  TH1F*     mcMergedHitPositionRad; //! for rec. hits that are matched to more than 1 M.C. hit
  TH1F*     mcMergedHitPositionZ; //! for rec. hits that are matched to more than 1 M.C. hit

  TH1F*     rcHitPositionRad; //! for All rec. hits
  TH1F*     rcHitPositionZ; //! for All rec. hits

  TH1F*     rcUnmatchedHitPositionRad; //! for rec. hits that are not matched to any M.C. hit
  TH1F*     rcUnmatchedHitPositionSector; //! for rec. hits that are not matched to any M.C. hit
  TH1F*     rcUnmatchedHitPositionZ; //! for rec. hits that are not matched to any M.C. hit

  TProfile* mHitEfficiency;     //! 
  TProfile* mHitPurity;         //! 

  TH2F*     rcPadSepEfficiencyOuter;  //! 
  TH2F*     mcPadSepEfficiencyOuter;  //! 
  TH2F*     rcPadSepEfficiencyInner;  //! 
  TH2F*     mcPadSepEfficiencyInner;  //! 

  TNtuple*  trackNtuple; //!
    Int_t         mTrackIndex;
    Int_t         mMcMultiplicity;
    Int_t         mRcMultiplicity;
    Int_t         mPairMultiplicity;

 private:

  ClassDef(StTpcEvalHistograms,1)
};

#endif
