////////////////////////////////////////////////////////////////////////
// 
// $Id: StFlowPicoEvent.cxx,v 1.4 2000/08/09 21:38:23 snelling Exp $
//
// Author: Sergei Voloshin and Raimond Snellings, March 2000
//
// Description:  A persistent Flow Pico DST
//
//  The StFlowPicoEvent class has a simple event structure:
//        TClonesArray    *fTracks;
//
//   The StFlowPicoEventHeader class:
//        Int_t           mNtrack;                  // track number
//        Int_t           mEventID;                 // event ID
//        UInt_t          mOrigMult;                // number of StEvent tracks
//        UInt_t          mCentrality;              // centrality bin
//        Float_t         mVertexX, Y, Z;           // primary vertex position
//
//   The StFlowPicoEvent data member fTracks is a pointer to a TClonesArray.
//   It is an array of a variable number of tracks per Event.
//   Each element of the array is an object of class StFlowTrack 
//
////////////////////////////////////////////////////////////////////////
//
// $Log: StFlowPicoEvent.cxx,v $
// Revision 1.4  2000/08/09 21:38:23  snelling
// PID added
//
// Revision 1.3  2000/06/01 18:26:39  posk
// Increased precision of Track integer data members.
//
// Revision 1.2  2000/05/26 21:29:31  posk
// Protected Track data members from overflow.
//
// Revision 1.1  2000/05/23 20:09:48  voloshin
// added StFlowPicoEvent, persistent FlowEvent as plain root TTree
//
// Revision 1.4  2000/05/16 20:59:33  posk
// Voloshin's flowPicoevent.root added.
//
// 
//////////////////////////////////////////////////////////////////////////

#include "StFlowPicoEvent.h"
#define PR(x) cout << "##### FlowPicoEvent: " << (#x) << " = " << (x) << endl;

ClassImp(StFlowPicoEvent)
ClassImp(StFlowPicoTrack)

TClonesArray *StFlowPicoEvent::fgTracks = 0;

//-----------------------------------------------------------------------

StFlowPicoEvent::StFlowPicoEvent()
{
  // Create an StFlowPicoEvent object.
  // When the constructor is invoked for the first time, the class static
  // variable fgTracks is 0 and the TClonesArray fgTracks is created.
  
  if (!fgTracks) fgTracks = new TClonesArray("StFlowPicoTrack", 4000);
  fTracks = fgTracks;
  mNtrack = 0;
}

//-----------------------------------------------------------------------

void StFlowPicoEvent::Clear(Option_t *option)
{
  fTracks->Clear(option);
  mNtrack=0;
}

//---------------------------------------------------------------------

StFlowPicoTrack::StFlowPicoTrack(Float_t pt, 
                                 Float_t eta, 
				 Float_t Dedx, 
                                 Float_t phi,
                                 Short_t charge,
                                 Float_t dca,
                                 Float_t chi2,
                                 Int_t   fitPts,
                                 Int_t   maxPts,
                                 Float_t pidPiPlus, 
                                 Float_t pidPiMinus,
				 Float_t pidProton,
				 Float_t pidKaonPlus,
				 Float_t pidKaonMinus,
				 Float_t pidAntiProton,
				 Float_t pidDeuteron
                                 ) : TObject()
{
  float maxShort  = 32.;
  float maxUShort = 6.4;
  
  if (dca > maxUShort)       dca         = maxUShort;
  if (chi2 > maxUShort)      chi2        = maxUShort;
  if (pidPiPlus > maxShort)  pidPiPlus   = maxShort;
  if (pidPiMinus > maxShort) pidPiMinus  = maxShort;
  if (pidProton > maxShort)  pidProton   = maxShort;
  if (pidAntiProton > maxShort)  pidAntiProton   = maxShort;
  if (pidKaonPlus > maxShort)  pidKaonPlus   = maxShort;
  if (pidKaonMinus > maxShort)  pidKaonMinus   = maxShort;
  if (pidDeuteron > maxShort)  pidDeuteron   = maxShort;
  
  mPt         = pt;
  mPhi        = phi;
  mEta        = eta;
  mDedx       = Dedx;
  mCharge     = (Char_t)charge;
  mDca        = (UShort_t)(dca*10000.);
  mChi2       = (UShort_t)(chi2*10000.);
  mFitPts     = (UChar_t)fitPts;
  mMaxPts     = (UChar_t)maxPts;
  mPidPiPlus  = (Short_t)(pidPiPlus*1000.);
  mPidPiMinus = (Short_t)(pidPiMinus*1000.);
  mPidProton  = (Short_t)(pidProton*1000.);
  mPidAntiProton  = (Short_t)(pidAntiProton*1000.);
  mPidKaonPlus  = (Short_t)(pidKaonPlus*1000.);
  mPidKaonMinus  = (Short_t)(pidKaonMinus*1000.);
  mPidDeuteron  = (Short_t)(pidDeuteron*1000.);
}








