//////////////////////////////////////////////////////////////////////
//
// $Id: StFlowEvent.cxx,v 1.11 2000/08/31 18:58:21 posk Exp $
//
// Author: Raimond Snellings and Art Poskanzer
//////////////////////////////////////////////////////////////////////
//
// Description: A subset of StEvent 
//
//////////////////////////////////////////////////////////////////////
//
// $Log: StFlowEvent.cxx,v $
// Revision 1.11  2000/08/31 18:58:21  posk
// For picoDST, added version number, runID, and multEta for centrality.
// Added centrality cut when reading picoDST.
// Added pt and eta selections for particles corr. wrt event plane.
//
// Revision 1.10  2000/08/12 20:22:19  posk
// Recalculate centrality in read from pico.
//
// Revision 1.9  2000/08/10 23:00:21  posk
// New centralities. pt and eta cuts.
//
// Revision 1.8  2000/08/09 21:38:23  snelling
// PID added
//
// Revision 1.7  2000/06/01 18:26:35  posk
// Increased precision of Track integer data members.
//
// Revision 1.6  2000/05/20 00:55:13  posk
// Condensed flownanoevent.root somewhat.
//
// Revision 1.5  2000/05/16 20:59:29  posk
// Voloshin's flownanoevent.root added.
//
// Revision 1.4  2000/05/12 22:42:04  snelling
// Additions for persistency and minor fix
//
// Revision 1.2  2000/03/15 23:28:50  posk
// Added StFlowSelection.
//
// Revision 1.1  2000/03/02 23:02:48  posk
// Changed extensions from .hh and .cc to .h and .cxx .
//
// Revision 1.16  2000/02/29 22:00:53  posk
// Made SetPhiWeight inline, changed ImpactPar to Dca, etc.
//
// Revision 1.15  2000/02/29 01:26:11  snelling
// removed static const int& nxxx = Flow::nxxx;
//
// Revision 1.14  2000/02/18 22:49:54  posk
// Added PID and centrality.
//
// Revision 1.13  2000/02/11 20:53:09  posk
// Commented out random_shuffle and cout formatting so as to work under CC5.
//
// Revision 1.12  2000/01/31 22:16:58  posk
// CC5 compliant.
//
// Revision 1.9  1999/12/21 17:31:50  posk
// Fixed random_shuffle in making the sub events.
//
// Revision 1.8  1999/12/21 01:10:58  posk
// Added more quantities to StFlowEvent.
//
// Revision 1.7  1999/12/16 18:05:22  posk
// Fixed Linux compatability again.
//
// Revision 1.6  1999/12/15 22:01:25  posk
// Added StFlowConstants.hh
//
// Revision 1.5  1999/12/07 23:30:52  snelling
// Fixed Linux warnings
//
// Revision 1.4  1999/12/04 00:10:32  posk
// Works with the new StEvent
//
// Revision 1.3  1999/11/30 18:52:51  snelling
// First modification for the new StEvent
//
// Revision 1.2  1999/11/24 18:17:13  posk
// Put the methods which act on the data in with the data in StFlowEvent.
//
// Revision 1.1  1999/11/11 23:08:54  posk
// Rearrangement of files.
//
// Revision 1.1  1999/11/04 19:02:04  snelling
// First check in of StFlowMaker. It contains the common code from
// StFlowTagMaker and StFlowAnalysisMaker.
//
//////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include "StFlowEvent.h"
#include "StFlowTrackCollection.h"
#include "StFlowSelection.h"
#include "StFlowConstants.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "TVector2.h"
#define PR(x) cout << "##### FlowEvent: " << (#x) << " = " << (x) << endl;

ClassImp(StFlowEvent)

Float_t  StFlowEvent::mEtaCuts[2][Flow::nHars][Flow::nSels] = {{{0.,0.5},
								{0.,0.},
								{0.,0.5},
								{0.,0.},
								{0.,0.5},
								{0.,0.}},
							       {{1.0,2.},
								{1.0,1.},
								{1.0,2.},
								{1.0,1.},
								{1.0,2.},
								{1.0,1.}}};
// Float_t  StFlowEvent::mEtaCuts[2][Flow::nHars][Flow::nSels] = {{{0.05,0.5},
// 								{0.05,0.},
// 								{0.05,0.5},
// 								{0.05,0.},
// 								{0.05,0.5},
// 								{0.05,0.}},
// 							       {{1.0,2.},
// 								{1.0,1.},
// 								{1.0,2.},
// 								{1.0,1.},
// 								{1.0,2.},
// 								{1.0,1.}}};
Float_t  StFlowEvent::mPtCuts[2][Flow::nHars][Flow::nSels] =  {{{0.1,0.1},
								{0.1,0.1},
								{0.1,0.1},
								{0.1,0.1},
								{0.1,0.1},
								{0.1,0.1}},
							       {{2.,2.},
								{2.,2.},
								{2.,2.},
								{2.,2.},
								{2.,2.},
								{2.,2.}}};

Float_t StFlowEvent::mPiPlusCuts[2]     = {-3., 3.};
Float_t StFlowEvent::mPiMinusCuts[2]    = {-3., 3.};
Float_t StFlowEvent::mProtonCuts[2]     = {-3., 3.};
Float_t StFlowEvent::mKMinusCuts[2]     = {-3., 3.};
Float_t StFlowEvent::mKPlusCuts[2]      = {-3., 3.};
Float_t StFlowEvent::mAntiProtonCuts[2] = {-3., 3.};

//-----------------------------------------------------------

StFlowEvent::StFlowEvent()  {
  // Make a new track collection

  pTrackCollection = new StFlowTrackCollection;
 
}

//-----------------------------------------------------------

StFlowEvent::~StFlowEvent() {

  delete pTrackCollection;

}

//-------------------------------------------------------------

Double_t StFlowEvent::PhiWeight(Float_t mPhi, Int_t selN, Int_t harN) const {

  if (mPhi < 0.) mPhi += twopi;
  int n = (int)((mPhi/twopi)*Flow::nPhiBins);

  return mPhiWgt[selN][harN][n];
}

//-------------------------------------------------------------

UInt_t StFlowEvent::Mult(StFlowSelection* pFlowSelect) {
  UInt_t mult = 0;

  StFlowTrackIterator itr;
  for (itr = TrackCollection()->begin(); itr != TrackCollection()->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    if (pFlowSelect->Select(pFlowTrack))  mult++;
  }

  return mult;
}

//-------------------------------------------------------------

Float_t StFlowEvent::MeanPt(StFlowSelection* pFlowSelect) {
  Float_t sumPt = 0.;
  UInt_t  mult  = 0;

  StFlowTrackIterator itr;
  for (itr = TrackCollection()->begin(); itr != TrackCollection()->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    if (pFlowSelect->Select(pFlowTrack)) {
      sumPt += pFlowTrack->Pt();
      mult++;
    }
  }

  return (mult) ? sumPt/(float)mult : 0.;
}

//-------------------------------------------------------------

TVector2 StFlowEvent::Q(StFlowSelection* pFlowSelect) { 
  TVector2 mQ;
  Int_t  selN  = pFlowSelect->Sel();
  Int_t  harN  = pFlowSelect->Har();
  double order = (double)(harN + 1);
  double mQx=0., mQy=0.;

  StFlowTrackIterator itr;
  for (itr = TrackCollection()->begin(); itr != TrackCollection()->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    if (pFlowSelect->Select(pFlowTrack)) {
      Float_t mPhi = pFlowTrack->Phi();
      double phiWgt = PhiWeight(mPhi, selN, harN);
      if (pFlowTrack->Eta() < 0. && (harN+1) % 2 == 1) phiWgt *= -1.;
      mQx += phiWgt * cos(mPhi * order);
      mQy += phiWgt * sin(mPhi * order);
    }
  }
  mQ.Set(mQx, mQy);

  return mQ;
}

//-------------------------------------------------------------

Float_t StFlowEvent::Psi(StFlowSelection* pFlowSelect) {
  Int_t  harN = pFlowSelect->Har();
  float order = (float)(harN + 1);
  Float_t psi = 0.;

  TVector2 mQ = Q(pFlowSelect);
  if (mQ.Mod()) {
    psi= mQ.Phi() / order;
    if (psi < 0.) { psi += twopi / order; }
  }
  
  return psi;
}

//-------------------------------------------------------------

Float_t StFlowEvent::q(StFlowSelection* pFlowSelect) { 
  TVector2 mQ  = Q(pFlowSelect);
  UInt_t mult  = Mult(pFlowSelect);
  
  return (mult) ? mQ.Mod() / sqrt((double)mult) : 0.;
}

//-----------------------------------------------------------------------

void StFlowEvent::SetSelections() {

  StFlowTrackIterator itr;
  for (itr = TrackCollection()->begin(); itr != TrackCollection()->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    Double_t eta = (double)(pFlowTrack->Eta());
    Float_t  Pt  = pFlowTrack->Pt();
    for (int selN = 0; selN < Flow::nSels; selN++) {
      for (int harN = 0; harN < Flow::nHars; harN++) {

	// Eta
	if (mEtaCuts[1][harN][selN] > mEtaCuts[0][harN][selN] && 
 	    (fabs(eta) < mEtaCuts[0][harN][selN] || 
 	     fabs(eta) >= mEtaCuts[1][harN][selN])) continue;
// 	    (eta < mEtaCuts[0][harN][selN]         || both subs at +eta
// 	     eta >= mEtaCuts[1][harN][selN])) continue;
	
	// Pt
	if (mPtCuts[1][harN][selN] > mPtCuts[0][harN][selN] && 
	    (Pt < mPtCuts[0][harN][selN] ||
	     Pt >= mPtCuts[1][harN][selN])) continue;

      	pFlowTrack->SetSelect(harN, selN);

      }
    }
  }

}

//-------------------------------------------------------------

void StFlowEvent::MakeSubEvents() {

  StFlowTrackIterator itr;
  int eventMult[Flow::nHars][Flow::nSels] = {{0}};
  int harN, selN, subN = 0;

  // loop to count the total number of tracks for each selection
  for (itr = TrackCollection()->begin(); itr != TrackCollection()->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    for (selN = 0; selN < Flow::nSels; selN++) {
      for (harN = 0; harN < Flow::nHars; harN++) {
	if (pFlowTrack->Select(harN, selN)) {
	    eventMult[harN][selN]++;
	}
      }
    }
  }

  // loop to set the SubEvent member variable
  for (selN = 0; selN < Flow::nSels; selN++) {
    for (harN = 0; harN < Flow::nHars; harN++) {
      int subEventMult = eventMult[harN][selN] / Flow::nSubs;
      if (subEventMult) {
	subN = 0;
	int countN = 0;
	for (itr = TrackCollection()->begin(); itr != TrackCollection()->end();
	     itr++) {
	  StFlowTrack* pFlowTrack = *itr;
	  if (pFlowTrack->Select(harN, selN)) {
	    pFlowTrack->SetSubevent(harN, selN, subN);
	    countN++;
	    if (countN % subEventMult == 0.) subN++;
	  }
	}
      }
    }
  }
  
}

//-------------------------------------------------------------

// void StFlowEvent::MakeSubEvents() {
//   // subevents with positive and negative eta

//   StFlowTrackIterator itr;
//   int eventMult[Flow::nHars][Flow::nSels] = {{0}};
//   int harN, selN, subN = 0;
  
//   // loop to set the SubEvent member variable
//   for (selN = 0; selN < Flow::nSels; selN++) {
//     for (harN = 0; harN < Flow::nHars; harN++) {
//       for (itr = TrackCollection()->begin(); itr != TrackCollection()->end();
// 	   itr++) {
// 	StFlowTrack* pFlowTrack = *itr;
// 	if (pFlowTrack->Select(harN, selN)) {
// 	  float eta = pFlowTrack->Eta();
// 	  if (eta > 0.) {
// 	    pFlowTrack->SetSubevent(harN, selN, 0);
// 	  } else {
// 	    pFlowTrack->SetSubevent(harN, selN, 1);
// 	  }
// 	}
//       }
//     }
//   }
  
// }

//-----------------------------------------------------------------------

void StFlowEvent::SetPids() {
  
  StFlowTrackIterator itr;
  for (itr = TrackCollection()->begin(); itr != TrackCollection()->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    Char_t pid[10] = "none";
    Short_t charge  = pFlowTrack->Charge();

    if (charge == 1) {
      Float_t piPlus  = pFlowTrack->PidPiPlus();
      Float_t proton  = pFlowTrack->PidProton();
      Float_t kPlus   = pFlowTrack->PidKaonPlus();
      if (piPlus > mPiPlusCuts[0] && piPlus < mPiPlusCuts[1]) {
	strcpy(pid, "pi+");
      } else if ( proton > mProtonCuts[0] && proton < mProtonCuts[1]) {
	strcpy(pid, "proton");
      } else if ( kPlus > mKPlusCuts[0] && kPlus < mKPlusCuts[1]) {
	strcpy(pid, "k+");
      }
    } else if (charge == -1) {
      Float_t piMinus = pFlowTrack->PidPiMinus();
      Float_t antiProton  = pFlowTrack->PidAntiProton();
      Float_t kMinus   = pFlowTrack->PidKaonMinus();
      if (piMinus > mPiMinusCuts[0] && piMinus < mPiMinusCuts[1]) {
	strcpy(pid, "pi-");
      } else if ( antiProton > mAntiProtonCuts[0] && antiProton < mAntiProtonCuts[1]) {
	strcpy(pid, "pbar");
      } else if ( kMinus > mKMinusCuts[0] && kMinus < mKMinusCuts[1]) {
	strcpy(pid, "k-");
      }
    }

    pFlowTrack->SetPid(pid);

  }
}

//-----------------------------------------------------------------------

void StFlowEvent::SetCentrality(const UInt_t& tracks) {

  UInt_t cent[] = {20,100,180,270,360,460,560,660,870};
  if (tracks < cent[0]) {
    mCentrality = 0;
  }
  else if (tracks >= cent[0] && tracks < cent[1]) {
    mCentrality = 1;
  }
  else if (tracks >= cent[1] && tracks < cent[2]) {
    mCentrality = 2;
  }
  else if (tracks >= cent[2] && tracks < cent[3]) {
    mCentrality = 3;
  }
  else if (tracks >= cent[3] && tracks < cent[4]) {
    mCentrality = 4;
  }
  else if (tracks >= cent[4] && tracks < cent[5]) {
    mCentrality = 5;
  }
  else if (tracks >= cent[5] && tracks < cent[6]) {
    mCentrality = 6;
  }
  else if (tracks >= cent[6] && tracks < cent[7]) {
    mCentrality = 7;
  }
  else if (tracks >= cent[7] && tracks < cent[8]) {
    mCentrality = 8;
  }
  else if (tracks >= cent[8]) {
    mCentrality = 9;
  }

}

//-----------------------------------------------------------------------

void StFlowEvent::PrintSelectionList() {
  // Prints the list of selection cuts
  // Call in Finish

  cout << "#######################################################" << endl;
  cout << "# Pid Cuts:" << endl; 
  cout << "#    PiPlus cuts=  " << mPiPlusCuts[0] << ", " 
       << mPiPlusCuts[1] << endl;
  cout << "#    PiMinus cuts= " << mPiMinusCuts[0] << ", " 
       << mPiMinusCuts[1] << endl;
  cout << "#    Proton cuts=  " << mProtonCuts[0] << ", " 
       << mProtonCuts[1] << endl;
  cout << "#######################################################" << endl;
  cout << "# Track Selections List:" << endl; 
  for (int k = 0; k < Flow::nSels; k++) {
    for (int j = 0; j < Flow::nHars; j++) {
      cout << "#  selection= " << k+1 << " harmonic= " 
	   << j+1 << endl;
      cout << "#    abs(Eta) cuts= " << mEtaCuts[0][j][k] << ", " 
	   << mEtaCuts[1][j][k] << endl;
      cout << "#    Pt cuts= " << mPtCuts[0][j][k] << ", "
	   << mPtCuts[1][j][k] << endl;
    }
  }
  cout << "#######################################################" << endl;

}

