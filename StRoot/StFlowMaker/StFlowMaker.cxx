//////////////////////////////////////////////////////////////////////
//
// $Id: StFlowMaker.cxx,v 1.34 2000/08/09 21:38:23 snelling Exp $
//
// Authors: Raimond Snellings and Art Poskanzer, LBNL, Jun 1999
//
//////////////////////////////////////////////////////////////////////
//
// Description: Maker to fill StFlowEvent from StEvent
//
//////////////////////////////////////////////////////////////////////
//
// $Log: StFlowMaker.cxx,v $
// Revision 1.34  2000/08/09 21:38:23  snelling
// PID added
//
// Revision 1.33  2000/07/20 17:25:51  posk
// Fixed bug in readPico checkEvent.
//
// Revision 1.32  2000/07/14 23:49:03  snelling
// Changed to ConstIterator for new StEvent and removed comparison int uint
//
// Revision 1.31  2000/07/12 17:54:37  posk
// Added chi2 and dca cuts. Multiplied EtaSym by sqrt(mult).
// Apply cuts when reading picoevent file.
//
// Revision 1.30  2000/06/30 14:48:33  posk
// Using MessageMgr, changed Eta Symmetry cut.
//
// Revision 1.29  2000/06/01 18:26:36  posk
// Increased precision of Track integer data members.
//
// Revision 1.28  2000/05/26 21:29:28  posk
// Protected Track data members from overflow.
//
// Revision 1.27  2000/05/23 20:09:44  voloshin
// added StFlowPicoEvent, persistent FlowEvent as plain root TTree
//
// Revision 1.26  2000/05/20 00:55:15  posk
// Condensed flownanoevent.root somewhat.
//
// Revision 1.25  2000/05/16 20:59:31  posk
// Voloshin's flownanoevent.root added.
//
// Revision 1.24  2000/05/12 22:42:04  snelling
// Additions for persistency and minor fix
//
// Revision 1.22  2000/03/28 23:21:02  posk
// Allow multiple instances of the AnalysisMaker.
//
// Revision 1.21  2000/03/21 00:22:01  posk
// Added GetCVS and some print commands.
//
// Revision 1.20  2000/03/15 23:28:52  posk
// Added StFlowSelection.
//
// Revision 1.19  2000/03/07 17:50:57  snelling
// Added Nano DST
//
// Revision 1.18  2000/03/02 23:02:53  posk
// Changed extensions from .hh and .cc to .h and .cxx .
//
// Revision 1.17  2000/02/29 22:00:54  posk
// Made SetPhiWeight inline, changed ImpactPar to Dca, etc.
//
// Revision 1.16  2000/02/29 01:26:11  snelling
// removed static const int& nxxx = Flow::nxxx;
//
// Revision 1.14  2000/02/18 22:49:56  posk
// Added PID and centrality.
//
// Revision 1.13  2000/02/11 20:53:10  posk
// Commented out random_shuffle and cout formatting so as to work under CC5.
//
// Revision 1.9  1999/12/21 01:11:00  posk
// Added more quantities to StFlowEvent.
//
// Revision 1.8  1999/12/16 18:05:23  posk
// Fixed Linux compatability again.
//
// Revision 1.7  1999/12/15 22:01:27  posk
// Added StFlowConstants.hh
//
// Revision 1.6  1999/12/07 23:30:53  snelling
// Fixed Linux warnings
//
// Revision 1.5  1999/12/04 00:10:34  posk
// Works with the new StEvent
//
// Revision 1.4  1999/11/30 18:52:52  snelling
// First modification for the new StEvent
//
// Revision 1.3  1999/11/24 18:17:15  posk
// Put the methods which act on the data in with the data in StFlowEvent.
//
// Revision 1.2  1999/11/11 23:08:57  posk
// Rearrangement of files.
//
// Revision 1.1  1999/11/04 19:02:12  snelling
// First check in of StFlowMaker. It contains the common code from
// StFlowTagMaker and StFlowAnalysisMaker.
//
//////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include "StFlowMaker.h"
#include "StFlowEvent.h"
#include "StFlowNanoEvent.h"
#include "StFlowPicoEvent.h"
#include "StEvent.h"
#include "StEventTypes.h"
#include "StFlowCutEvent.h"
#include "StFlowCutTrack.h"
#include "StFlowSelection.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "StThreeVector.hh"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "StFlowConstants.h"
#include "StPionPlus.hh"
#include "StPionMinus.hh"
#include "StProton.hh"
#include "StKaonMinus.hh"
#include "StKaonPlus.hh"
#include "StAntiProton.hh"
#include "StDeuteron.hh"
#include "StTpcDedxPidAlgorithm.h"
#include "StMessMgr.h"
//#include <algorithm>
#define PR(x) cout << "##### FlowMaker: " << (#x) << " = " << (x) << endl;

ClassImp(StFlowMaker)

//-----------------------------------------------------------------------

StFlowMaker::StFlowMaker(const Char_t* name): 
  StMaker(name), mNanoEventWrite(kFALSE), mNanoEventRead(kFALSE), 
  mPicoEventWrite(kFALSE), mPicoEventRead(kFALSE),
  mFlowEventWrite(kFALSE), mFlowEventRead(kFALSE), pEvent(NULL) {
  pFlowSelect = new StFlowSelection();
  SetNanoEventFileName("flownanoevent.root"); 
  SetPicoEventFileName("flowpicoevent.root");
}

StFlowMaker::StFlowMaker(const Char_t* name,
					 const StFlowSelection& flowSelect) :
  StMaker(name), 
  mNanoEventWrite(kFALSE), mNanoEventRead(kFALSE), 
  mPicoEventWrite(kFALSE), mPicoEventRead(kFALSE), 
  mFlowEventWrite(kFALSE), mFlowEventRead(kFALSE), pEvent(NULL) {
  pFlowSelect = new StFlowSelection(flowSelect); //copy constructor
  SetNanoEventFileName("flownanoevent.root"); 
  SetPicoEventFileName("flowpicoevent.root");
}

//-----------------------------------------------------------------------

StFlowMaker::~StFlowMaker() {
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::Make() {

  // Delete previous StFlowEvent
  if (pFlowEvent) delete pFlowEvent;
  pFlowEvent = NULL;
    
  // Get a pointer to StEvent
  if (!mFlowEventRead && !mNanoEventRead && !mPicoEventRead) {
    pEvent = (StEvent*)GetDataSet("StEvent");
    if (!pEvent) return kStOK; // If no event, we're done
    
    // Check the event cuts and fill StFlowEvent
    if (StFlowCutEvent::CheckEvent(pEvent)) {
      // Instantiate a new StFlowEvent
      pFlowEvent = new StFlowEvent;
      if (!pFlowEvent) return kStOK;
      FillFlowEvent();
      if (!pFlowEvent) return kStOK;  // could have been deleted
      if (mNanoEventWrite) FillNanoEvent();
      if (mPicoEventWrite) FillPicoEvent();
      if (mFlowEventWrite) pFlowMicroTree->Fill();  // fill the tree
    } else {
      Long_t eventID = pEvent->id();
      gMessMgr->Info() << "##### FlowMaker: event " << eventID << " cut" << endm;
    }

  } else if (mFlowEventRead) {
    // Get a pointer to StFlowEvent
    if (!pFlowEvent) return kStOK; // If no event, we're done
    if (mNanoEventWrite) FillNanoEvent();
    if (mPicoEventWrite) FillPicoEvent();

  } else if (mNanoEventRead) {
    // Instantiate a new StFlowEvent
    pFlowEvent = new StFlowEvent;
    if (!pFlowEvent) return kStOK;
    if (!FillFromNanoDST(pNanoEvent)) return kStEOF; // false if EOF
    if (mFlowEventWrite) pFlowMicroTree->Fill();  // fill the tree

  } else if (mPicoEventRead) {
    // Instantiate a new StFlowEvent
    pFlowEvent = new StFlowEvent;
    if (!pFlowEvent) return kStOK;
    if (!FillFromPicoDST(pPicoEvent)) return kStEOF; // false if EOF
    if (!pFlowEvent) return kStOK; // could have been deleted
  }

  UInt_t flowEventMult;
  if (!pFlowEvent) { flowEventMult = 0;}
  else { flowEventMult = pFlowEvent->FlowEventMult(); }
  PR(flowEventMult);

  if (Debug()) StMaker::PrintInfo();

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::Init() {
  // Open PhiWgt file
  ReadPhiWgtFile();

  Int_t kRETURN = kStOK;

  if (mNanoEventWrite) kRETURN += InitNanoEventWrite();
  if (mNanoEventRead)  kRETURN += InitNanoEventRead();
  if (mPicoEventWrite) kRETURN += InitPicoEventWrite();
  if (mPicoEventRead)  kRETURN += InitPicoEventRead();
  if (mFlowEventWrite) kRETURN += InitFlowEventWrite();
  if (mFlowEventRead)  kRETURN += InitFlowEventRead();

  gMessMgr->SetLimit("##### FlowMaker", 5);
  gMessMgr->Info("##### FlowMaker: $Id: StFlowMaker.cxx,v 1.34 2000/08/09 21:38:23 snelling Exp $");
  if (kRETURN) gMessMgr->Info() << "##### FlowMaker: Init return = " << kRETURN << endm;

  return kRETURN;
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::Finish() {
  // Print the cut lists
  cout << "#######################################################" << endl;
  cout << "##### FlowMaker: Cut Lists" << endl;
  if (!mFlowEventRead && !mNanoEventRead) {
    StFlowCutEvent::PrintCutList();
    StFlowCutTrack::PrintCutList();
  }
  pFlowEvent->PrintSelectionList();

  if (mNanoEventWrite && pNanoDST->IsOpen()) {
    pNanoDST->Write();
    pNanoDST->Close();
  }

  if (mNanoEventRead && pNanoDST->IsOpen()) { pNanoDST->Close(); }

  if (mPicoEventWrite && pPicoDST->IsOpen()) {
    pPicoDST->Write();
    pPicoDST->Close();
  }

  if (mPicoEventRead && pPicoDST->IsOpen()) { pPicoDST->Close(); }

  if (mFlowEventWrite && pFlowDST->IsOpen()) {
    pFlowDST->Write();
    pFlowDST->Close();
  }

  if (mFlowEventRead && pFlowDST->IsOpen()) { // pFlowDST->Close(); 
  }

  return StMaker::Finish();
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::ReadPhiWgtFile() {
  // Read the PhiWgt root file

  TDirectory* dirSave = gDirectory;
  TString* fileName = new TString("flowPhiWgt.hist.root");
  fileName->Prepend(pFlowSelect->Number());
  TFile* pPhiWgtFile = new TFile(fileName->Data(), "READ");
  if (!pPhiWgtFile->IsOpen()) {
    gMessMgr->Info("##### FlowMaker: No PhiWgt file. Will set weights = 1.");
  }
  delete fileName;
  gDirectory = dirSave;

  // Fill mPhiWgt
  // for each selection and each harmonic
  for (int k = 0; k < Flow::nSels; k++) {
    char countSels[2];
    sprintf(countSels,"%d",k+1);
    for (int j = 0; j < Flow::nHars; j++) {
      char countHars[2];
      sprintf(countHars,"%d",j+1);
      TString* histTitle = new TString("Flow_Phi_Weight_Sel");
      histTitle->Append(*countSels);
      histTitle->Append("_Har");
      histTitle->Append(*countHars);
      if (pPhiWgtFile->IsOpen()) {
	TH1* phiWgtHist = (TH1*)pPhiWgtFile->Get(histTitle->Data());
	for (int n = 0; n < Flow::nPhiBins; n++) {
	  mPhiWgt[k][j][n] = (phiWgtHist) ? phiWgtHist->GetBinContent(n+1) : 1.;
	}
      } else {
	for (int n = 0; n < Flow::nPhiBins; n++) mPhiWgt[k][j][n] = 1.;
      }
      delete histTitle;
    }
  }

  // Close PhiWgt file
  if (pPhiWgtFile->IsOpen()) pPhiWgtFile->Close();

  return kStOK;
}

//-----------------------------------------------------------------------

void StFlowMaker::FillFlowEvent() {
  // Make StFlowEvent from StEvent

  // Fill PhiWgt array
  pFlowEvent->SetPhiWeight(mPhiWgt);

  // Get event id 
  pFlowEvent->SetEventID((Int_t)(pEvent->id()));

  // Get primary vertex position
  const StThreeVectorF& vertex = pEvent->primaryVertex(0)->position();
  pFlowEvent->SetVertexPos(vertex);

  // include trigger (ZDC and CTB)
  Float_t ctb =-1.;
  Float_t zdce =-1.;
  Float_t zdcw =-1.;
  StTriggerDetectorCollection *triggers = pEvent->triggerDetectorCollection();
  if (triggers)	{
    StCtbTriggerDetector &CTB = triggers->ctb();
    StZdcTriggerDetector &ZDC = triggers->zdc();
    // get CTB
    for (UInt_t slat=0; slat<CTB.numberOfSlats(); slat++) {
      for (UInt_t tray=0; tray<CTB.numberOfTrays();tray++) {
	ctb += CTB.mips(tray,slat,0);
      }
    }
    //get ZDCe and ZDCw        
    zdce = ZDC.adcSum(east);
    zdcw = ZDC.adcSum(west);
  } 
  
  pFlowEvent->SetCTB(ctb);
  pFlowEvent->SetZDCe(zdce);
  pFlowEvent->SetZDCw(zdcw);

  // Get initial multiplicity before TrackCuts 
  UInt_t origMult = pEvent->primaryVertex(0)->numberOfDaughters(); 
  pFlowEvent->SetOrigMult(origMult);
  PR(origMult);

  // loop over tracks in StEvent
  int goodTracks = 0;
  int goodTracksEta1 = 0;
  int goodTracksEta2 = 0;
  const StSPtrVecPrimaryTrack& tracks = pEvent->primaryVertex(0)->daughters();
  StSPtrVecPrimaryTrackConstIterator itr;
  StTpcDedxPidAlgorithm tpcDedxAlgo;
  Float_t nSigma;

  for (itr = tracks.begin(); itr != tracks.end(); itr++) {
    StPrimaryTrack* pTrack = *itr;
    if (pTrack && pTrack->flag() > 0) {
      StThreeVectorD p = pTrack->geometry()->momentum();
      // calculate the number of tracks with positive flag 
      if (fabs(p.pseudoRapidity()) < 1.) {
	goodTracksEta2++;
	if (fabs(p.pseudoRapidity()) < 0.5) {
	  goodTracksEta1++;
	}
      }
      if (StFlowCutTrack::CheckTrack(pTrack)) {
	// Instantiate new StFlowTrack
	StFlowTrack* pFlowTrack = new StFlowTrack;
	if (!pFlowTrack) return;
	pFlowTrack->SetPhi(p.phi());
	pFlowTrack->SetEta(p.pseudoRapidity());
	pFlowTrack->SetPt(p.perp());
	pFlowTrack->SetCharge(pTrack->geometry()->charge());
	pFlowTrack->SetDca(pTrack->impactParameter());
	pFlowTrack->SetChi2((Float_t)(pTrack->fitTraits().chi2()));
	pFlowTrack->SetFitPts(pTrack->fitTraits().numberOfFitPoints());
	pFlowTrack->SetMaxPts(pTrack->numberOfPossiblePoints());
	
	pTrack->pidTraits(tpcDedxAlgo);       // initialize
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StPionPlus::instance());
	pFlowTrack->SetPidPiPlus(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StPionMinus::instance());
	pFlowTrack->SetPidPiMinus(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StProton::instance());
	pFlowTrack->SetPidProton(nSigma);
	
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StKaonMinus::instance());
	pFlowTrack->SetPidKaonMinus(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StKaonPlus::instance());
	pFlowTrack->SetPidKaonPlus(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StAntiProton::instance());
	pFlowTrack->SetPidAntiProton(nSigma);
	nSigma = (float)tpcDedxAlgo.numberOfSigma(StDeuteron::instance());
	pFlowTrack->SetPidDeuteron(nSigma);
	
	// dE/dx
	StPtrVecTrackPidTraits traits = pTrack->pidTraits(kTpcId);
	StDedxPidTraits* pid;
	for (unsigned int i = 0; i < traits.size(); i++) {
	  pid = dynamic_cast<StDedxPidTraits*>(traits[i]);
	  if (pid && pid->method()==kTruncatedMeanId) break;
	}
	pFlowTrack->SetDedx(pid->mean());

	pFlowEvent->TrackCollection()->push_back(pFlowTrack);
	goodTracks++;
      }
    }
  }

  pFlowEvent->SetMultEta1(goodTracksEta1);
  pFlowEvent->SetCentrality(goodTracksEta1);
  pFlowEvent->SetMultEta2(goodTracksEta2);
  
  // Check Eta Symmetry
  if (!StFlowCutEvent::CheckEtaSymmetry(pEvent)) {  
    delete pFlowEvent;             // if kFALSE delete this event
    pFlowEvent = NULL;
    return;
  }

  // For use with STL vector
//   random_shuffle(pFlowEvent->TrackCollection()->begin(),
// 		 pFlowEvent->TrackCollection()->end());

  pFlowEvent->TrackCollection()->random_shuffle();

  pFlowEvent->SetSelections();
  pFlowEvent->MakeSubEvents();
  pFlowEvent->SetPids();

  //PR("Flow Subs");
  //PrintSubeventMults();

}

//-----------------------------------------------------------------------

void StFlowMaker::FillNanoEvent() {

  if (!pNanoEvent) {
    gMessMgr->Warning("##### FlowMaker: No FlowNanoEvent");
    return;
  }

  pNanoEvent->SetEventID(pFlowEvent->EventID());
  pNanoEvent->SetOrigMult(pFlowEvent->OrigMult());
  pNanoEvent->SetMultEta1(pFlowEvent->MultEta1());
  pNanoEvent->SetMultEta2(pFlowEvent->MultEta2());
  pNanoEvent->SetCentrality(pFlowEvent->Centrality());
  pNanoEvent->SetVertexPos(pFlowEvent->VertexPos());
  pNanoEvent->SetCTB(pFlowEvent->CTB());
  pNanoEvent->SetZDCe(pFlowEvent->ZDCe());
  pNanoEvent->SetZDCw(pFlowEvent->ZDCw());
  
  StFlowTrackIterator itr;
  StFlowTrackCollection* pFlowTracks = pFlowEvent->TrackCollection();
  
  for (itr = pFlowTracks->begin(); itr != pFlowTracks->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    pNanoEvent->AddTrack(pFlowTrack);
  }
  pFlowTree->Fill();           //fill the tree
  pNanoEvent->Clear();  
  
}

//----------------------------------------------------------------------

void StFlowMaker::FillPicoEvent() {

  if (!pPicoEvent) {
    gMessMgr->Warning("##### FlowMaker: No FlowPicoEvent");
    return;
  }
  
  pPicoEvent->SetEventID(pFlowEvent->EventID());
  pPicoEvent->SetOrigMult(pFlowEvent->OrigMult());
  pPicoEvent->SetMultEta1(pFlowEvent->MultEta1());
  pPicoEvent->SetMultEta2(pFlowEvent->MultEta2());
  pPicoEvent->SetCentrality(pFlowEvent->Centrality());
  pPicoEvent->SetVertexPos(pFlowEvent->VertexPos().x(),
			       pFlowEvent->VertexPos().y(),
			       pFlowEvent->VertexPos().z());
  pPicoEvent->SetCTB(pFlowEvent->CTB());
  pPicoEvent->SetZDCe(pFlowEvent->ZDCe());
  pPicoEvent->SetZDCw(pFlowEvent->ZDCw());
  
  StFlowTrackIterator itr;
  StFlowTrackCollection* pFlowTracks = pFlowEvent->TrackCollection();
  
  TClonesArray &tracks = *pPicoEvent->Tracks();
  Int_t nt=0;
  for (itr = pFlowTracks->begin(); itr != pFlowTracks->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    new(tracks[nt++]) StFlowPicoTrack(pFlowTrack->Pt(), 
				      pFlowTrack->Eta(),
				      pFlowTrack->Dedx(),
				      pFlowTrack->Phi(),
				      pFlowTrack->Charge(),
				      pFlowTrack->Dca(),
				      pFlowTrack->Chi2(),
				      pFlowTrack->FitPts(),
				      pFlowTrack->MaxPts(),
				      pFlowTrack->PidPiPlus(),
				      pFlowTrack->PidPiMinus(),
				      pFlowTrack->PidProton(),  
				      pFlowTrack->PidKaonPlus(),  
				      pFlowTrack->PidKaonMinus(),  
				      pFlowTrack->PidAntiProton(),  
				      pFlowTrack->PidDeuteron()
				      );
  }  
  pPicoEvent->SetNtrack(nt);
  
  pFlowTree->Fill();             //fill the tree
  pPicoEvent->Clear();
  
}

//-----------------------------------------------------------------------

Bool_t StFlowMaker::FillFromNanoDST(StFlowNanoEvent* pNanoEvent) {
  // Make StFlowEvent from StFlowNanoEvent

  if (!pNanoEvent || !pFlowTree->GetEntry(mNanoEventCounter++)) {
    cout << "##### FlowMaker: no more events" << endl; 
    return kFALSE; 
  }

  // Fill FlowEvent
  pFlowEvent->SetPhiWeight(mPhiWgt);
  pFlowEvent->SetEventID(pNanoEvent->EventID());
  UInt_t origMult = pNanoEvent->OrigMult();
  pFlowEvent->SetOrigMult(origMult);
  pFlowEvent->SetMultEta1(pNanoEvent->MultEta1());
  pFlowEvent->SetMultEta2(pNanoEvent->MultEta2());
  pFlowEvent->SetCentrality(pNanoEvent->MultEta1());
  pFlowEvent->SetVertexPos(pNanoEvent->VertexPos());
  pFlowEvent->SetCTB(pNanoEvent->CTB());
  pFlowEvent->SetZDCe(pNanoEvent->ZDCe());
  pFlowEvent->SetZDCw(pNanoEvent->ZDCw());

  // Fill FlowTracks
  Int_t nTracks = (Int_t)pNanoEvent->GetNtrack();
  TClonesArray &tracks = *pNanoEvent->GetTracks();
  for (Int_t nt=0; nt<nTracks; nt++) {
      pFlowEvent->TrackCollection()->push_back((StFlowTrack*)tracks[nt]);
  }

  pFlowEvent->SetSelections();
  pFlowEvent->MakeSubEvents();
  pFlowEvent->SetPids();

  //  PR("Nano Subs");
  //  PrintSubeventMults();

  return kTRUE;
}

//-----------------------------------------------------------------------

Bool_t StFlowMaker::FillFromPicoDST(StFlowPicoEvent* pPicoEvent) {
  // Make StFlowEvent from StFlowPicoEvent

  if (!pPicoEvent || !pFlowTree->GetEntry(mPicoEventCounter++)) {
    cout << "##### FlowMaker: no more events" << endl; 
    return kFALSE; 
  }

  // Fill FlowEvent
  pFlowEvent->SetPhiWeight(mPhiWgt);
  pFlowEvent->SetEventID(pPicoEvent->EventID());
  UInt_t origMult = pPicoEvent->OrigMult();
  pFlowEvent->SetOrigMult(origMult);
  pFlowEvent->SetVertexPos(StThreeVectorF(pPicoEvent->VertexX(),
					  pPicoEvent->VertexY(),
					  pPicoEvent->VertexZ()) );
  pFlowEvent->SetMultEta1(pPicoEvent->MultEta1());
  pFlowEvent->SetMultEta2(pPicoEvent->MultEta2());
  pFlowEvent->SetCentrality(pPicoEvent->MultEta1());
  pFlowEvent->SetCTB(pPicoEvent->CTB());
  pFlowEvent->SetZDCe(pPicoEvent->ZDCe());
  pFlowEvent->SetZDCw(pPicoEvent->ZDCw());

  // Fill FlowTracks
  for (Int_t nt=0; nt<pPicoEvent->GetNtrack(); nt++) {
    StFlowPicoTrack* pPicoTrack = (StFlowPicoTrack*)pPicoEvent->Tracks()
      ->UncheckedAt(nt);
    if (pPicoTrack && StFlowCutTrack::CheckTrack(pPicoTrack)) {
      // Instantiate new StFlowTrack
      StFlowTrack* pFlowTrack = new StFlowTrack;
      if (!pFlowTrack) return kFALSE;
      pFlowTrack->SetPt(pPicoTrack->Pt());
      pFlowTrack->SetPhi(pPicoTrack->Phi());
      pFlowTrack->SetEta(pPicoTrack->Eta());
      pFlowTrack->SetDedx(pPicoTrack->Dedx());
      pFlowTrack->SetCharge(pPicoTrack->Charge());
      pFlowTrack->SetDca(pPicoTrack->Dca());
      pFlowTrack->SetChi2(pPicoTrack->Chi2());
      pFlowTrack->SetFitPts(pPicoTrack->FitPts());
      pFlowTrack->SetMaxPts(pPicoTrack->MaxPts());
      pFlowTrack->SetPidPiPlus(pPicoTrack->PidPiPlus());
      pFlowTrack->SetPidPiMinus(pPicoTrack->PidPiMinus());
      pFlowTrack->SetPidProton(pPicoTrack->PidProton());
      pFlowTrack->SetPidAntiProton(pPicoTrack->PidAntiProton());
      pFlowTrack->SetPidKaonPlus(pPicoTrack->PidKaonPlus());
      pFlowTrack->SetPidKaonMinus(pPicoTrack->PidKaonMinus());
      pFlowTrack->SetPidDeuteron(pPicoTrack->PidDeuteron());
      pFlowEvent->TrackCollection()->push_back(pFlowTrack);
    }
  }
    
  // Check event cuts and Eta Symmetry
  if (!StFlowCutEvent::CheckEvent(pPicoEvent) ||
      !StFlowCutEvent::CheckEtaSymmetry(pPicoEvent)) {  
    Int_t eventID = pPicoEvent->EventID();
    gMessMgr->Info() << "##### FlowMaker: picoevent " << eventID << " cut" << endm;
    delete pFlowEvent;             // if kFALSE delete this event
    pFlowEvent = NULL;
    return kTRUE;
  }
  
  // For use with STL vector
//   random_shuffle(pFlowEvent->TrackCollection()->begin(),
// 		 pFlowEvent->TrackCollection()->end());

  pFlowEvent->TrackCollection()->random_shuffle();

  pFlowEvent->SetSelections();
  pFlowEvent->MakeSubEvents();
  pFlowEvent->SetPids();

  //PR("Pico Subs");
  //PrintSubeventMults();

  return kTRUE;
}

//-----------------------------------------------------------------------

void StFlowMaker::PrintSubeventMults() {

  int j, k, n;

  pFlowSelect->SetSubevent(-1);
  for (j = 0; j < Flow::nHars; j++) {
    pFlowSelect->SetHarmonic(j);
    for (k = 0; k < Flow::nSels; k++) {
      pFlowSelect->SetSelection(k);
      cout << "j,k= " << j << k << " : " << pFlowEvent->Mult(pFlowSelect) << endl;
    }
  }

  for (j = 0; j < Flow::nHars; j++) {
    pFlowSelect->SetHarmonic(j);
    for (k = 0; k <Flow:: nSels; k++) {
      pFlowSelect->SetSelection(k);
      for (n = 0; n < Flow::nSubs+1; n++) {
	pFlowSelect->SetSubevent(n);
	cout << "j,k,n= " << j << k << n << " : " << 
	  pFlowEvent->Mult(pFlowSelect) << endl;
      }
    }
  }

}

//-----------------------------------------------------------------------

Int_t StFlowMaker::InitNanoEventWrite() {

  Int_t split  = 1;       // by default split Event into sub branches
  Int_t comp   = 1;       // by default file is compressed
  Int_t bufsize = 256000;
  if (split) bufsize /= 4;

  pNanoEvent = new StFlowNanoEvent();   

  // Open an output file
  Char_t* file = mNanoEventFileName;  
  pNanoDST = new TFile(mNanoEventFileName,"RECREATE","Flow Nano DST file");
  if (!pNanoDST) {
    cout << "##### FlowMaker: Warning: no NanoEvents file = " << file << endl;
    return kStFatal;
  }
  pNanoDST->SetFormat(1);
  pNanoDST->SetCompressionLevel(comp);
  gMessMgr->Info() << "##### FlowMaker: NanoEvents file = " << file << endm;

  // Create a ROOT Tree and one superbranch
  pFlowTree = new TTree("FlowTree", "Flow Nano Tree");
  if (!pFlowTree) {
    cout << "##### FlowMaker: Warning: No FlowNanoTree" << endl;
    return kStFatal;
  }

  pFlowTree->SetAutoSave(10000000);  // autosave when 10 Mbyte written
  pFlowTree->Branch("pNanoEvent", "StFlowNanoEvent", &pNanoEvent,
		    bufsize, split);

  return kStOK;  
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::InitPicoEventWrite() {

  Int_t split  = 1;       // by default split Event into sub branches
  Int_t comp   = 1;       // by default file is compressed
  Int_t bufsize = 256000;
  if (split) bufsize /= 4;

  // creat a Picoevent and an output file
  pPicoEvent = new StFlowPicoEvent();   
  Char_t* file = mPicoEventFileName;  
  pPicoDST = new TFile(mPicoEventFileName,"RECREATE","Flow Pico DST file");
  if (!pPicoDST) {
    cout << "##### FlowMaker: Warning: no PicoEvents file = " << file << endl;
    return kStFatal;
  }
  pPicoDST->SetFormat(1);
  pPicoDST->SetCompressionLevel(comp);
  gMessMgr->Info() << "##### FlowMaker: PicoEvents file = " << file << endm;

  // Create a ROOT Tree and one superbranch
  pFlowTree = new TTree("FlowTree", "Flow Pico Tree");
  if (!pFlowTree) {
    cout << "##### FlowMaker: Warning: No FlowPicoTree" << endl;
    return kStFatal;
  }

  pFlowTree->SetAutoSave(10000000);  // autosave when 10 Mbyte written
  pFlowTree->Branch("pPicoEvent", "StFlowPicoEvent", &pPicoEvent,
		    bufsize, split);

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::InitNanoEventRead() {
  
  pNanoEvent = new StFlowNanoEvent(); 
  
  // Open the input file
  Char_t* file = mNanoEventFileName;  
  pNanoDST = new TFile(file);
  if (!pNanoDST->IsOpen()) {
    cout << "##### FlowMaker: Warning: no NanoEvents file = " << file << endl;
    return kStFatal;
  }

  gMessMgr->Info() << "##### FlowMaker: NanoEvents file = " << file << endm;

  // Get the tree, the branch, and the entries
  pFlowTree = (TTree*)pNanoDST->Get("FlowTree");
  if (!pFlowTree) {
    cout << "##### FlowMaker: Warning: No FlowTree" << endl;
    return kStFatal;
  }

  TBranch* branch = pFlowTree->GetBranch("pNanoEvent");
  branch->SetAddress(&pNanoEvent);
  Int_t nEntries = (Int_t)pFlowTree->GetEntries(); 
  gMessMgr->Info() << "##### FlowMaker: events in nano-DST file = " 
		   << nEntries << endm;
  
  mNanoEventCounter = 0;

  return kStOK;  
  }

//-----------------------------------------------------------------------

Int_t StFlowMaker::InitPicoEventRead() {
  
  pPicoEvent = new StFlowPicoEvent(); 
  
  // Open the file
  Char_t* file = mPicoEventFileName;  
  pPicoDST = new TFile(file);
  if (!pPicoDST->IsOpen()) {
    cout << "##### FlowMaker: Error: no PicoEvents file = " << file << endl;
    return kStFatal;
  }

  gMessMgr->Info() << "##### FlowMaker: PicoEvents file = " << file << endm;

  // Get the tree, the branch, and the entries
  pFlowTree = (TTree*)pPicoDST->Get("FlowTree");
  if (!pFlowTree) {
    cout << "##### FlowMaker: Error: No FlowTree" << endl;
    return kStFatal;
  }

  TBranch* branch = pFlowTree->GetBranch("pPicoEvent");
  branch->SetAddress(&pPicoEvent);
  Int_t nEntries = (Int_t)pFlowTree->GetEntries(); 
  gMessMgr->Info() << "##### FlowMaker: events in Pico-DST file = " << nEntries << endm;

  mPicoEventCounter = 0;

  return kStOK;
  }

//-----------------------------------------------------------------------

Int_t StFlowMaker::InitFlowEventWrite() {

  Int_t split  = 1;       // by default, split Event in sub branches
  Int_t comp   = 1;       // by default file is compressed
  Int_t bufsize = 256000;
  if (split)  bufsize /= 4;

  // Create a new ROOT binary machine independent file.
  // Note that this file may contain any kind of ROOT objects, histograms,
  // pictures, graphics objects, detector geometries, tracks, events, etc..
  // This file is now becoming the current directory.
  pFlowDST = new TFile("flowevent.root", "RECREATE", "Flow micro DST file");
  if (!pFlowDST) {
    cout << "##### FlowMaker: Warning: no FlowEvents file" << endl;
    return kStFatal;
  }

  pFlowDST->SetCompressionLevel(comp);

  // Create a ROOT Tree and one superbranch
  pFlowMicroTree = new TTree("FlowMicroTree", "Flow Micro Tree");
  if (!pFlowMicroTree) {
    cout << "##### FlowMaker: Warning: No FlowMicroTree" << endl;
    return kStFatal;
  }

  pFlowMicroTree->SetAutoSave(100000000);  // autosave when 100 Mbyte written
  pFlowMicroTree->Branch("pFlowEvent", "StFlowEvent", &pFlowEvent, bufsize, split);

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StFlowMaker::InitFlowEventRead() {
  pFlowDST = new TFile("flowevent.root", "READ");
  if (pFlowDST) return kStFatal;

  return kStOK;
}

//-----------------------------------------------------------------------
