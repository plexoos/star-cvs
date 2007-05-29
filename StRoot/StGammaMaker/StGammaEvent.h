#ifndef __StGammaEvent_h__
#define __StGammaEvent_h__

#include "TObject.h"
#include "TVector3.h"

#include "StGammaCandidate.h"
//#include "StGammaIsolation.h"
//#include "StGammaDistribution.h"
//#include "StGammaFit.h"

#include "StGammaTrack.h"
#include "StGammaTower.h"
#include "StGammaStrip.h"

#include "TClonesArray.h"

class StMuTrack;

#define TPC_VERTEX 0x0001

class StGammaEvent : public TObject {
  
 public:
  StGammaEvent();
  ~StGammaEvent(){ /* nada */ };

  void Clear(Option_t *opts="");
  UShort_t mFlags;  /// Event flags (see above)
  UShort_t flags(){ return mFlags; }

  StGammaTrack *newTrack( StMuTrack *mutr=0 );/// Add a new track
  StGammaTower *newTower();/// Add a new tower
  StGammaTower *newPreshower1();/// Add a new preshower1 (bprs) element
  StGammaTower *newPreshower2();/// Add a new preshower2 element
  StGammaTower *newPostshower();/// Add a new postshower element
  StGammaStrip *newStrip();/// Add a new SMD strip
  StGammaCandidate *newCandidate();/// Add a new gamma candidate

  Int_t numberOfTracks(){ return nTracks; }/// Return number of tracks
  Int_t numberOfTowers(){ return nTowers; }/// Return number of towers
  Int_t numberOfPreshower1(){ return nPreshower1; }/// Return number of pre1
  Int_t numberOfPreshower2(){ return nPreshower2; }/// Return number of pre2
  Int_t numberOfPostshower(){ return nPostshower; }/// Return number of post
  Int_t numberOfStrips(){ return nStrips; }/// Return number of strips
  Int_t numberOfCandidates(){ return nCandidates; }/// Return number of candidates

  Float_t sumPt( Float_t eta_min = -2.5, Float_t eta_max = +2.5 );/// Returns track+tower pT in eta range
  Float_t sumTrackPt(Float_t eta_min = -2.5, Float_t eta_max = +2.5 );/// Returns track pT in eta range
  Float_t sumTowerPt(Float_t eta_min = -2.5, Float_t eta_max = +2.5 );/// Returns tower pT in eta range

  StGammaTrack *track( Int_t i ){ return (StGammaTrack*)mTracks->At(i); }/// Return ith track
  StGammaTower *tower( Int_t i ){ return (StGammaTower*)mTowers->At(i); }/// Return ith tower
  StGammaTower *preshower1( Int_t i ){ return (StGammaTower*)mPreshower1->At(i); }/// Return ith pre1
  StGammaTower *preshower2( Int_t i ){ return (StGammaTower*)mPreshower2->At(i); }/// Return ith pre2
  StGammaTower *postshower( Int_t i ){ return (StGammaTower*)mPostshower->At(i); }/// Return ith post
  StGammaStrip *strip( Int_t i ){ return (StGammaStrip*)mStrips->At(i); }/// Return ith strip
  StGammaCandidate *candidate( Int_t i ){ return (StGammaCandidate*)mCandidates->At(i); }/// Return ith candidate
  
  //                                                     Run, event and vertex
 protected:
  Int_t mRunNumber;   /// Run number
  Int_t mEventNumber; /// Event number  
  TVector3 mVertex; /// Event vertex (TPC)



 public:
  void SetRunNumber( Int_t run ){ mRunNumber=run; }
  void SetEventNumber( Int_t event ){ mEventNumber=event; }
  void SetVertex( TVector3 vertex ){ mVertex=vertex; }

  Int_t runNumber(){ return mRunNumber; }/// Returns run number
  Int_t eventNumber(){ return mEventNumber; }/// Returns event number
  TVector3 vertex(){ return mVertex; }/// Returns the vertex

  
 private:
  //                                                 Towers, tracks and strips
  Int_t InitArrays();

  Int_t nTracks;
  Int_t nTowers;
  Int_t nPreshower1;
  Int_t nPreshower2;
  Int_t nPostshower;
  Int_t nStrips;
  Int_t nCandidates;

  TClonesArray *mTracks;    //-> array of all tracks
  TClonesArray *mTowers;    //-> array of all towers
  TClonesArray *mPreshower1;//-> array of all preshower1
  TClonesArray *mPreshower2;//-> array of all preshower2
  TClonesArray *mPostshower;//-> array of all postshower
  TClonesArray *mStrips;    //-> array of all strips
  TClonesArray *mCandidates;//-> array of all candidates
 
  ClassDef(StGammaEvent,1);

};

#endif
