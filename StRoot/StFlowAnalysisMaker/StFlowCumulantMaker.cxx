///////////////////////////////////////////////////////////////////////////////
//
// $Id: StFlowCumulantMaker.cxx,v 1.14 2003/03/03 16:24:36 aihong Exp $
//
// Authors:  Aihong Tang, Kent State U. Oct 2001
//           Frame adopted from Art and Raimond's StFlowAnalysisMaker.
//
///////////////////////////////////////////////////////////////////////////////
//
// Description:  Maker to analyze Flow using the latest cumulant method.
//                refer to Phy. Rev. C63 (2001) 054906 (old new method)
//                and      Phy. Rev. C64 (2001) 054901 (new new method)
//                and      nucl-ex/0110016             (Practical Guide)
//                Eq. numbers are from the PG or new new if not specified.
//
///////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "StMaker.h"
#include "StFlowCumulantMaker.h"
#include "StFlowMaker/StFlowMaker.h"
#include "StFlowMaker/StFlowEvent.h"
#include "StFlowMaker/StFlowConstants.h"
#include "StFlowMaker/StFlowSelection.h"
#include "StEnumerations.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "TFile.h"
#include "TString.h"
#include "TObjString.h"
#include "TVectorD.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TProfile2D.h"
#include "TOrdCollection.h"
#include "StMessMgr.h"
#include "TMath.h"

#define PR(x) cout << "##### FlowCumulantAnalysis: " << (#x) << " = " << (x) << endl;

ClassImp(StFlowCumulantMaker)

//-----------------------------------------------------------------------

StFlowCumulantMaker::StFlowCumulantMaker(const Char_t* name): StMaker(name),
  MakerName(name) {
  pFlowSelect = new StFlowSelection();
  mOldMethod  = kFALSE;
  SetHistoRanges();
}

StFlowCumulantMaker::StFlowCumulantMaker(const Char_t* name,
					 const StFlowSelection& flowSelect) :
  StMaker(name), MakerName(name) {
  pFlowSelect = new StFlowSelection(flowSelect); // copy constructor
  mOldMethod  = kFALSE;
  SetHistoRanges();
}

//-----------------------------------------------------------------------

StFlowCumulantMaker::~StFlowCumulantMaker() {
}

//-----------------------------------------------------------------------

Int_t StFlowCumulantMaker::Make() {
  // Fill histograms

  // Get a pointer to StFlowEvent
  StFlowMaker* pFlowMaker = NULL;
  pFlowMaker = (StFlowMaker*)GetMaker("Flow");
  if (pFlowMaker) pFlowEvent = pFlowMaker->FlowEventPointer();
  if (pFlowEvent) {
    if (pFlowSelect->Select(pFlowEvent)) {      // event selected
      FillFromFlowEvent();                      // get event quantities
      FillEventHistograms();                    // fill from FlowEvent
      if (pFlowEvent) FillParticleHistograms(); // fill particle histograms
    }
    if (Debug()) StMaker::PrintInfo();
  } else {
    gMessMgr->Info("##### FlowCumulantMaker: FlowEvent pointer null");
  }
  
  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StFlowCumulantMaker::Init() {
  // Book histograms

  float ptMaxPart = Flow::ptMaxPart;
  if (pFlowSelect->PtMaxPart()) {
    ptMaxPart = pFlowSelect->PtMaxPart();
  }

  xLabel = "Pseudorapidity";
  if (strlen(pFlowSelect->PidPart()) != 0) { xLabel = "Rapidity"; }  
  
  profScale=1000.;// in new root version, error does not show correctly 
  //if the number is too small. this is a trick to walkaround.


  if (mOldMethod) 
        r0 = 0.06;
  else  r0 = 1.5; // this number should be small, but it could bring numerical 
                  // error if it is too small.
  r0Sq = r0 * r0;

  m_M = 1;  // if m_M = 2, what measured is v2, v4, v6... etc. for harmonic 1,2,3
            // m_M = 2 is not working. Do not know why at the moment.
  
  bool noDenomFileWarned = kFALSE;
  TString* histTitle;

  for (int k = 0; k < Flow::nSels; k++) {
    char countSels[2];
    sprintf(countSels,"%d",k+1);

    // for each selection
    histFull[k].mHistCumul = new TProfile*[Flow::nCumulDiffOrders];
    
    for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++) {
      char theCumulOrder[2]; // if >10, need to use char*
      sprintf(theCumulOrder,"%d",(ord+1)*2);
      histTitle = new TString("Flow_Cumul_Order"); 
      histTitle->Append(*theCumulOrder);           
      histTitle->Append("_Sel");                      
      histTitle->Append(*countSels);          
      histFull[k].mHistCumul[ord] =  
	new TProfile(histTitle->Data(), histTitle->Data(), Flow::nHars, 0.5,
		     (float)(Flow::nHars) + 0.5, -1.*FLT_MAX, FLT_MAX, "");
      histFull[k].mHistCumul[ord]->SetXTitle("harmonic");
      delete histTitle;
    }
    
    // for each harmonic
    for (int j = 0; j < Flow::nHars; j++) {
      char countHars[2];
      sprintf(countHars,"%d",j+1);

      histTitle = new TString("Flow_CumulMultSum_Sel");
      histTitle->Append(*countSels);
      histTitle->Append("_Har");
      histTitle->Append(*countHars);
      histFull[k].histFullHar[j].mHistMultSum =
        new TH1D(histTitle->Data(),histTitle->Data(),1,0.,1.);
      delete histTitle;

      histTitle = new TString("Flow_CumulWgtMultSumq4_Sel");
      histTitle->Append(*countSels);
      histTitle->Append("_Har");
      histTitle->Append(*countHars);
      histFull[k].histFullHar[j].mHistWgtMultSum_q4 =
        new TH1D(histTitle->Data(),histTitle->Data(),1,0.,1.);
      delete histTitle;

      histTitle = new TString("Flow_CumulWgtMultSumq6_Sel");
      histTitle->Append(*countSels);
      histTitle->Append("_Har");
      histTitle->Append(*countHars);
      histFull[k].histFullHar[j].mHistWgtMultSum_q6 =
        new TH1D(histTitle->Data(),histTitle->Data(),1,0.,1.);
      delete histTitle;

      histTitle = new TString("Flow_CumulNEvent_Sel");
      histTitle->Append(*countSels);
      histTitle->Append("_Har");
      histTitle->Append(*countHars);
      histFull[k].histFullHar[j].mHistNEvent =
        new TH1D(histTitle->Data(),histTitle->Data(),1,0.,1.);
      delete histTitle;

      // ****  for differential flow   ****
      
      // cumulant        d_p/n{k}      
      // p is the harmonic of the differential flow 
      // n is the harmonic of the integrated flow used for the differential flow
      // measurment. p = mn = either n or 2n.
      
      // if m_M=1, k=2    dn/n{2}  : cumulant from 2-part corr.
      // if m_M=1, k=4    dn/n{4}  : cumulant from 4-part corr.
      // if m_M=2, k=2    d2n/n{3} : cumulant from 3-part corr., mixed harmonic
      // if m_M=2, k=4    d2n/n{5} : cumulant from 5-part corr., mixed harmonic
      // where {2},{3} corresponds to theCumulIndex=1 below.
      // {4},{5} corresponds to theCumulIndex=2 below.
      
      histFull[k].histFullHar[j].mHistCumul2D  =  
	new TProfile2D*[Flow::nCumulDiffOrders];
      histFull[k].histFullHar[j].mHistCumulEta = 
	new TProfile*[Flow::nCumulDiffOrders];
      histFull[k].histFullHar[j].mHistCumulPt  = 
	new TProfile*[Flow::nCumulDiffOrders];
      
      // For each cumulant order
      for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++) {
	char theCumulOrder[2]; // if >10, need to use char*
	sprintf(theCumulOrder,"%d",(ord+1)*2);
	
	histTitle = new TString("Flow_Cumul2D_Order");
	histTitle->Append(*theCumulOrder);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mHistCumul2D[ord] =	
	  new TProfile2D(histTitle->Data(),histTitle->Data(), mNEtaBins,
			 mEtaMin, mEtaMax, Flow::nPtBins, Flow::ptMin,
			 ptMaxPart, -1.*FLT_MAX, FLT_MAX, "");
	histFull[k].histFullHar[j].mHistCumul2D[ord]->SetXTitle((char*)xLabel.Data());
	histFull[k].histFullHar[j].mHistCumul2D[ord]->SetYTitle("Pt (GeV)");
	delete histTitle;
	
	histTitle = new TString("Flow_CumulEta_Order");
	histTitle->Append(*theCumulOrder);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mHistCumulEta[ord] =  
	  new TProfile(histTitle->Data(),histTitle->Data(), mNEtaBins,
		       mEtaMin, mEtaMax, -1.*FLT_MAX, FLT_MAX, "");
	histFull[k].histFullHar[j].mHistCumulEta[ord]->SetXTitle((char*)xLabel.Data());
	delete histTitle;
	
	histTitle = new TString("Flow_CumulPt_Order");
	histTitle->Append(*theCumulOrder);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mHistCumulPt[ord] =  
	  new TProfile(histTitle->Data(), histTitle->Data(), Flow::nPtBins,
		       Flow::ptMin, ptMaxPart, -1.*FLT_MAX, FLT_MAX, "");
	histFull[k].histFullHar[j].mHistCumulPt[ord]->SetXTitle("Pt (GeV/c)");
	delete histTitle;
	
      }
      
      histFull[k].histFullHar[j].mCumulG0Denom2D = 
	new TProfile2D*[Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax];
      histFull[k].histFullHar[j].mCumulG0DenomEta = 
	new TProfile*[Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax];
      histFull[k].histFullHar[j].mCumulG0DenomPt = 
	new TProfile*[Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax];
      histFull[k].histFullHar[j].mCumulG0Denom = 
	new TProfile*[Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax];

      // For each cumulant order * qMax orders
      for (int pq  = 0; pq < Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax; pq++) { 
	
	TString* histTitleIntegDenom; //for read in
	
	int cumulIndex = (pq/Flow::nCumulDiff_qMax) + 1; // like 1,2,3.
	// for cumulant order 2,4,6 not begining with 0. This is "p" in Eq. (B1, PG5).
        int qIndex        = pq%Flow::nCumulDiff_qMax; // like 0,1,..._qMax-1  
	// begin with 0. This is "q" in Eq. (B3, PG5).
	
	char theCumulOrderChar[2];
        char qIndexOrderChar[2]; // if >10, need to use char*
	sprintf(theCumulOrderChar,"%d",cumulIndex*2); 
        sprintf(qIndexOrderChar,"%d",qIndex);
	
	histTitle = new TString("Flow_CumulDenom2D_Order");
	histTitle->Append(*theCumulOrderChar);
	histTitle->Append("_GenFunIdx");
	histTitle->Append(*qIndexOrderChar);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mCumulG0Denom2D[pq] =
	  new TProfile2D(histTitle->Data(),histTitle->Data(), mNEtaBins,
			 mEtaMin, mEtaMax, Flow::nPtBins, Flow::ptMin,
			 Flow::ptMax,-1.*FLT_MAX, FLT_MAX, "");
	histFull[k].histFullHar[j].mCumulG0Denom2D[pq]->
	  SetXTitle((char*)xLabel.Data());
	histFull[k].histFullHar[j].mCumulG0Denom2D[pq]->SetYTitle("Pt (GeV)");
	delete histTitle;
	
	histTitle = new TString("Flow_CumulDenomEta_Order");
	histTitle->Append(*theCumulOrderChar);
	histTitle->Append("_GenFunIdx");
	histTitle->Append(*qIndexOrderChar);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mCumulG0DenomEta[pq] =
	  new TProfile(histTitle->Data(),histTitle->Data(), mNEtaBins,
		       mEtaMin, mEtaMax, -1.*FLT_MAX, FLT_MAX, "");
	histFull[k].histFullHar[j].mCumulG0DenomEta[pq]->
	  SetXTitle((char*)xLabel.Data());
	delete histTitle;
	
	histTitle = new TString("Flow_CumulDenomPt_Order");
	histTitle->Append(*theCumulOrderChar);
	histTitle->Append("_GenFunIdx");
	histTitle->Append(*qIndexOrderChar);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mCumulG0DenomPt[pq] = 
	  new TProfile(histTitle->Data(),histTitle->Data(), Flow::nPtBins,
		       Flow::ptMin, Flow::ptMax, -1.*FLT_MAX, FLT_MAX, "");
	histFull[k].histFullHar[j].mCumulG0DenomPt[pq]->SetXTitle("Pt (GeV/c)");
	delete histTitle;
	
	histTitle = new TString("Flow_CumulDenom_Order");
	histTitle->Append(*theCumulOrderChar);
	histTitle->Append("_GenFcnqIdx");
	histTitle->Append(*qIndexOrderChar);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mCumulG0Denom[pq] = 
	  new TProfile(histTitle->Data(),histTitle->Data(), 1,0., 1., -1.*FLT_MAX, FLT_MAX, "");
	histFull[k].histFullHar[j].mCumulG0Denom[pq]->SetYTitle("<G>");
	histTitleIntegDenom = new TString(histTitle->Data());
	delete histTitle;
	
	// Open the file and get the histograms 
	// do not move this section [ROOT bug] 
// 	TFile f;
// 	if(!noDenomFileWarned) f("denominator.root","R");
	TFile f("denominator.root","R");
	if (f.IsOpen()) {
	  
	  f.cd();
	  TProfile* tempDenomPtProfile = 
	    dynamic_cast<TProfile*>(f.Get(histTitleIntegDenom->Data()));
	  if (!tempDenomPtProfile) {
	    cout << "##### FlowCumulantAnalysis: can not find " <<
	      histTitleIntegDenom->Data() << endl;
	    return kFALSE;
	  }   
	  delete  histTitleIntegDenom;     

	  histFull[k].histFullHar[j].mCumulG0DenomRead[pq]
	    = tempDenomPtProfile->GetBinContent(1);
	  
	  f.Close();
	  
	} else {

	  if(!noDenomFileWarned) {
	    gMessMgr->Info("##### FlowCumulantAnalysis:denominator.root is not present, assumming this run is just for producing denominator.root. That means cumulant flow result in flow.cumulant.root is nonsense for this run. ");
	    noDenomFileWarned = kTRUE;
	  }
	  histFull[k].histFullHar[j].mCumulG0DenomRead[pq] = 1.; 

	}
	
	double theTempPhi = twopi*((double)qIndex) /  // Eq. (PG5)
	  ((double)Flow::nCumulDiff_qMax); 
	double theRz = r0*sqrt(cumulIndex);
	histFull[k].histFullHar[j].mDiffXz[pq] = theRz*cos(theTempPhi);
	histFull[k].histFullHar[j].mDiffYz[pq] = theRz*sin(theTempPhi);
      }
      
      // ***  for integrated flow  ***

      histFull[k].histFullHar[j].mHistCumulIntegG0Sum =
      new TH1D*[Flow::nCumulIntegOrders*Flow::nCumulInteg_qMax];

      for (int pq = 0; pq <  Flow::nCumulIntegOrders*Flow::nCumulInteg_qMax; pq++) {
	int cumulIndex = (pq/Flow::nCumulInteg_qMax) + 1; // like 1,2,3. 
	//not begining with 0. That is "p" in Eq. (B1, PG5).
        int qIndex = pq%(Flow::nCumulInteg_qMax); // like 0,1,..qMax-1 
	//begining with 0. Just like Eq. (B3, PG5).

	char theCumulOrderChar[2];
        char qIndexOrderChar[2]; // if >10, need to use char*
	sprintf(theCumulOrderChar,"%d",cumulIndex*2); 
        sprintf(qIndexOrderChar,"%d",qIndex);

	double theTempPhi = twopi*((double)qIndex) /
	  ((double)Flow::nCumulInteg_qMax); 
	double theRz = r0*sqrt(cumulIndex);
	histFull[k].histFullHar[j].mIntegXz[pq] = theRz*cos(theTempPhi);
	histFull[k].histFullHar[j].mIntegYz[pq] = theRz*sin(theTempPhi);
	
	histFull[k].histFullHar[j].mCumulIntegG0[pq] = 0.;

  	histTitle = new TString("Flow_CumulIntegG0Sum_Order");
	histTitle->Append(*theCumulOrderChar);
	histTitle->Append("_GenFunIdx");
	histTitle->Append(*qIndexOrderChar);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mHistCumulIntegG0Sum[pq] =
          new TH1D(histTitle->Data(),histTitle->Data(),1,0.,1.);
        delete histTitle;   

      }
      
      histFull[k].histFullHar[j].mMultSum       = 0.;
      histFull[k].histFullHar[j].mWgtMultSum_q4 = 0.;
      histFull[k].histFullHar[j].mWgtMultSum_q6 = 0.;
      histFull[k].histFullHar[j].mNEvent        = 0;
    }
  }
  
  gMessMgr->SetLimit("##### FlowCumulantAnalysis", 2);
  gMessMgr->Info("##### FlowCumulantAnalysis: $Id: StFlowCumulantMaker.cxx,v 1.14 2003/03/03 16:24:36 aihong Exp $");

  return StMaker::Init();
}

//-----------------------------------------------------------------------

void StFlowCumulantMaker::FillFromFlowEvent() {
  // Get event quantities from StFlowEvent
  
  for (int k = 0; k < Flow::nSels; k++) {
    pFlowSelect->SetSelection(k);
    for (int j = 0; j < Flow::nHars; j++) {
      pFlowSelect->SetHarmonic(j);
      pFlowSelect->SetSubevent(-1);
      
      // full event quantities
      mMult[k][j]       = pFlowEvent->Mult(pFlowSelect);
      mWgtMult_q4[k][j] = pFlowEvent->WgtMult_q4(pFlowSelect);              
      mWgtMult_q6[k][j] = pFlowEvent->WgtMult_q6(pFlowSelect);              
      
    }
  }
}

//-----------------------------------------------------------------------

void StFlowCumulantMaker::FillEventHistograms() {
  // Fill histograms with event quantities
  
  for (int k = 0; k < Flow::nSels; k++) {
    pFlowSelect->SetSelection(k);
    for (int j = 0; j < Flow::nHars; j++) {
      pFlowSelect->SetHarmonic(j);
      
      histFull[k].histFullHar[j].mMultSum       += (float)mMult[k][j];
      histFull[k].histFullHar[j].mWgtMultSum_q4 += mWgtMult_q4[k][j];
      histFull[k].histFullHar[j].mWgtMultSum_q6 += mWgtMult_q6[k][j];
      histFull[k].histFullHar[j].mNEvent++;
      
      for (int pq = 0; pq < Flow::nCumulIntegOrders*Flow::nCumulInteg_qMax; pq++) {   
	
	if (mOldMethod) {
	  histFull[k].histFullHar[j].mCumulIntegG0[pq] += 
	    pFlowEvent->G_Old( pFlowSelect,  
			       histFull[k].histFullHar[j].mIntegXz[pq], 
			       histFull[k].histFullHar[j].mIntegYz[pq] );
        } else {
	  histFull[k].histFullHar[j].mCumulIntegG0[pq] += 
	    pFlowEvent->G_New( pFlowSelect,  
			       histFull[k].histFullHar[j].mIntegXz[pq], 
			       histFull[k].histFullHar[j].mIntegYz[pq] );
	}
      }
    }
  }
  
}

//-----------------------------------------------------------------------

void StFlowCumulantMaker::FillParticleHistograms() {
  // Fill histograms from the particles
  
  // Initialize Iterator
  StFlowTrackCollection* pFlowTracks = pFlowEvent->TrackCollection();
  StFlowTrackIterator itr;
  
  // In the view of good coding, the following block should be placed inside
  // the track loop. It is put here to reduce run time.
  // If somebody changes the pFlowSelect to select tracks for v-part, 
  // he/she has to change pFlowSelect in this block for consistency. 
  // Selections have to be consistent between here and the loop.

  double* evtG[Flow::nSels][Flow::nHars];
  double* theCrossterm[Flow::nSels][Flow::nHars];
  double  theSqrtOfSumWgtSqr[Flow::nSels][Flow::nHars];
  
  for (int k = 0; k < Flow::nSels; k++) {
    for (int j = 0; j < Flow::nHars; j++) { 
       evtG[k][j] = 
	new double[Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax];
       theCrossterm[k][j]    = 
        new double[Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax];
    }
  }

  for (int k = 0; k < Flow::nSels; k++) { 
    for (int j = 0; j < Flow::nHars; j++) {
      pFlowSelect->SetSelection(k);
      pFlowSelect->SetHarmonic(j);
      
      theSqrtOfSumWgtSqr[k][j] = sqrt(pFlowEvent->SumWeightSquare(pFlowSelect));
      
      for (int pq = 0; pq < Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax; pq++) {
	
	evtG[k][j][pq] = (mOldMethod) ?
	  (pFlowEvent->G_Old( pFlowSelect,
			      histFull[k].histFullHar[j].mDiffXz[pq],
			      histFull[k].histFullHar[j].mDiffYz[pq] )) :
	  (pFlowEvent->G_New( pFlowSelect,
			      histFull[k].histFullHar[j].mDiffXz[pq],
			      histFull[k].histFullHar[j].mDiffYz[pq] )) ;
	theCrossterm[k][j][pq] =evtG[k][j][pq];
        histFull[k].histFullHar[j].mCumulG0Denom[pq]->Fill(0.5,evtG[k][j][pq]);
      } 
    }
  }

  for (itr = pFlowTracks->begin(); itr != pFlowTracks->end(); itr++) {
    StFlowTrack* pFlowTrack = *itr;
    
    float phi = pFlowTrack->Phi();
    if (phi < 0.) phi += twopi;
    float eta = pFlowTrack->Eta();
    float pt  = pFlowTrack->Pt();
    
    for (int k = 0; k < Flow::nSels; k++) {
      pFlowSelect->SetSelection(k);
      double cumuTemp[Flow::nCumulDiffOrders];
      double cumuTempFlip[Flow::nCumulDiffOrders];
      double order;
      double phiWgt;
      for (int j = 0; j < Flow::nHars; j++) {
	bool oddHar = (j+1) % 2;
	pFlowSelect->SetHarmonic(j);
	order  = (double)(j+1);
	
 	if (pFlowSelect->Select(pFlowTrack)) {
// 	  // Get detID
// 	  StDetectorId detId;
// 	  if (pFlowTrack->TopologyMap().hasHitInDetector(kTpcId) || 
// 	      (pFlowTrack->TopologyMap().data(0) == 0 && 
// 	       pFlowTrack->TopologyMap().data(1) == 0)) {
// 	    // Tpc track, or TopologyMap not available
// 	    detId = kTpcId;
// 	  } else if (pFlowTrack->TopologyMap().trackFtpcEast()) {
// 	    detId = kFtpcEastId;
// 	  } else if (pFlowTrack->TopologyMap().trackFtpcWest()) {
// 	    detId = kFtpcWestId;
// 	  } else {
// 	    detId = kUnknownId;
// 	  }
	  
	  // Get phiWgt
	  phiWgt = pFlowEvent->PhiWeight(k, j, pFlowTrack);
	}
	
	// Caculate v for all particles selected for correlation analysis
	if (pFlowSelect->SelectPart(pFlowTrack)) {
	  
	  float yOrEta = 
	    (strlen(pFlowSelect->PidPart()) != 0) ?  pFlowTrack->Y() : eta;
	  
	  double Dp[Flow::nCumulDiffOrders]; // the Dp in (B7, PG11)
	  for (int pq = 0; pq < Flow::nCumulDiffOrders; pq++) {
	    Dp[pq] = 0.; }
	  
	  for (int pq = 0; pq < Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax; pq++) {
	    int theCumulOrder = (pq/Flow::nCumulDiff_qMax) + 1; // like 1,2. 
	    // not begin with 0. which is "p" in (B6, PG5)
	    int qIndex        = pq%(Flow::nCumulDiff_qMax); // like 0,1,..qMax-1 
	    // begin with 0. which is "q" in (B6, PG5)
	    
	    //Calculate PG11
	    double theCoeff = pow(r0*sqrt(theCumulOrder), m_M) /
	      float(Flow::nCumulDiff_qMax); // first term in (B7)
	    double theCosTerm = cos(twopi*float(qIndex)*float(m_M) /
				    float(Flow::nCumulDiff_qMax)); // cos() in (B7)
	    double theSinTerm = sin(twopi*float(qIndex)*float(m_M) /
				    float(Flow::nCumulDiff_qMax)); // sin() in (B7)
	    	    
	    if ( (pFlowSelect->SelectPart(pFlowTrack)) && 
		 (pFlowSelect->Select(pFlowTrack)) ) { // remove autocorrelation
	      if (mOldMethod) {           
		theCrossterm[k][j][pq] = evtG[k][j][pq] / 
		  exp( (phiWgt/theSqrtOfSumWgtSqr[k][j]) *
		       (2.*histFull[k].histFullHar[j].mDiffXz[pq]*cos(phi*order) +
			2.*histFull[k].histFullHar[j].mDiffYz[pq]*sin(phi*order) ) );
	      } else {
		theCrossterm[k][j][pq] = evtG[k][j][pq] / 
		  (1. + (phiWgt/mMult[k][j]) *
		   (2.*histFull[k].histFullHar[j].mDiffXz[pq]*cos(phi*order) +
		    2.*histFull[k].histFullHar[j].mDiffYz[pq]*sin(phi * order) ) ); 
		// the argument in the last paragraph in page 9. 
	      }
	    }

	    // for writting out <G>, the denominator in (B6, PG10)
	    histFull[k].histFullHar[j].mCumulG0Denom2D[pq]->
	      Fill(yOrEta, pt, theCrossterm[k][j][pq]); 
	    histFull[k].histFullHar[j].mCumulG0DenomPt[pq]->
	      Fill(pt, theCrossterm[k][j][pq]); 
	    histFull[k].histFullHar[j].mCumulG0DenomEta[pq]->
	      Fill(yOrEta, theCrossterm[k][j][pq]);  
	    
	    double theXpq = (theCrossterm[k][j][pq]*cos(float(m_M) * order * phi)) / // (B6)
	      histFull[k].histFullHar[j].mCumulG0DenomRead[pq]; 
	    double theYpq = (theCrossterm[k][j][pq]*sin(float(m_M) * order * phi)) / // (B6)
	      histFull[k].histFullHar[j].mCumulG0DenomRead[pq];
	    
	    Dp[theCumulOrder-1] += 
	      theCoeff*(theCosTerm*theXpq + theSinTerm*theYpq); // (B7, PG11)
	    
	  }
	  
	  if (m_M==1) {
	    cumuTemp[0] = ((2.*Dp[1-1])-(0.5*Dp[2-1]))/r0Sq; // (B9, PG12)
	    cumuTemp[1] = ((-2.*Dp[1-1])+Dp[2-1])/(r0Sq*r0Sq);
	  } else if (m_M==2) {
	    cumuTemp[0] = ((4.*Dp[1-1])-(0.5*Dp[2-1]))/(r0Sq*r0Sq); // (B10)
	    cumuTemp[1] = ((-6.*Dp[1-1])+(1.5*Dp[2-1]))/(r0Sq*r0Sq*r0Sq);
	  }
	  
	  cumuTempFlip[0] = cumuTemp[0];
	  cumuTempFlip[1] = cumuTemp[1];
	  if (eta < 0 && oddHar) {
	    cumuTempFlip[0] *= -1.;
	    cumuTempFlip[1] *= -1.;
	  }
	  
	  for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++) {
	    histFull[k].histFullHar[j].mHistCumul2D[ord]->
	      Fill(yOrEta, pt,  ((ord>0) ? cumuTemp[ord]*profScale : cumuTemp[ord])); 
	    histFull[k].histFullHar[j].mHistCumulEta[ord]->
	      Fill(yOrEta, ((ord>0) ? cumuTemp[ord]*profScale : cumuTemp[ord])); 
	    histFull[k].histFullHar[j].mHistCumulPt[ord]->
	      Fill(pt, ((ord>0) ? cumuTempFlip[ord]*profScale : cumuTempFlip[ord])); 
	  }	  
	  
	  for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++)
	    histFull[k].mHistCumul[ord]->Fill(order, ((ord>0) ? cumuTempFlip[ord]*profScale : cumuTempFlip[ord]) );
	}
      }
    }  
  }
  
}

//-----------------------------------------------------------------------

Int_t StFlowCumulantMaker::Finish() {
  
  TString* histTitle;
  
  TOrdCollection* XpqYpqDenomNames = new TOrdCollection(Flow::nSels*Flow::nHars); 
  
  cout << endl << "##### Cumulant Maker:" << endl;
  for (int k = 0; k < Flow::nSels; k++) {
    char countSels[2];
    sprintf(countSels,"%d",k+1);
    
    cout << "##### selection "<< k+1 <<"  #### "<<endl;
    
    // integrated flow from cumulant
    histFull[k].mHist_v = new TH1D*[Flow::nCumulDiffOrders];
    
    for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++) {
      char theCumulOrder[2]; // if >10, need to use char*
      sprintf(theCumulOrder,"%d",(ord+1)*2);

      histTitle = new TString("Flow_Cumul_v_Order");
      histTitle->Append(*theCumulOrder);
      histTitle->Append("_Sel");
      histTitle->Append(*countSels);
      histFull[k].mHist_v[ord] = 
	new  TH1D(*(histFull[k].mHistCumul[ord]->ProjectionX(histTitle->Data(),"e")));
      histFull[k].mHist_v[ord]->SetTitle(histTitle->Data());
      histFull[k].mHist_v[ord]->SetXTitle("harmonic");
      histFull[k].mHist_v[ord]->SetYTitle("v (%)");
      delete histTitle;
if (ord>0)  histFull[k].mHist_v[ord]->Scale(1./profScale);
      AddHist(histFull[k].mHist_v[ord]);
    }
    
    double  meanIntegV[Flow::nHars];     // V**1
    double  meanIntegV2[Flow::nHars];    // V**2
    double  meanIntegV3[Flow::nHars];    // V**3
    double  meanIntegV4[Flow::nHars];    // V**4
    double  cumulInteg1[Flow::nHars];    // outside of harmonic loop
    double  cumulInteg2[Flow::nHars];
    double  cumulInteg3[Flow::nHars];
    double  q2[Flow::nHars]; // for old method. <Q>**2 in (74) of old paper.
    double  q4[Flow::nHars];
    double  q6[Flow::nHars];
    
    for (int j = 0; j < Flow::nHars; j++) {
      meanIntegV[j]  = 0.;
      meanIntegV2[j] = 0.;
      meanIntegV3[j] = 0.;
      meanIntegV4[j] = 0.;
      cumulInteg1[j] = 0.;
      cumulInteg2[j] = 0.;
      cumulInteg3[j] = 0.;
    }
    
    for (int j = 0; j < Flow::nHars; j++) {
      char countHars[2];
      sprintf(countHars,"%d",j+1);
      
      double mAvMult = // average multiplicity
	float(histFull[k].histFullHar[j].mMultSum)/
	(float(histFull[k].histFullHar[j].mNEvent));
      
      double mAvWgtMult_q4 = // for getting q4 with wgt
	float(histFull[k].histFullHar[j].mWgtMultSum_q4)/
	(float(histFull[k].histFullHar[j].mNEvent));
      
      double mAvWgtMult_q6 = // for getting q6 with wgt
	float(histFull[k].histFullHar[j].mWgtMultSum_q6)/
	(float(histFull[k].histFullHar[j].mNEvent));
      
       histFull[k].histFullHar[j].mHistMultSum->
	 SetBinContent(1,double(histFull[k].histFullHar[j].mMultSum));
       histFull[k].histFullHar[j].mHistWgtMultSum_q4->
	 SetBinContent(1,double(histFull[k].histFullHar[j].mWgtMultSum_q4));
       histFull[k].histFullHar[j].mHistWgtMultSum_q6->
	 SetBinContent(1,double(histFull[k].histFullHar[j].mWgtMultSum_q6));
       histFull[k].histFullHar[j].mHistNEvent->
	 SetBinContent(1,double(histFull[k].histFullHar[j].mNEvent));

      double CpInteg[Flow::nCumulIntegOrders]; // Cp in (B4, PG6)
      
      for (int pq = 0; pq < Flow::nCumulIntegOrders; pq ++) CpInteg[pq] = 0.;
      for (int pq = 0; pq < Flow::nCumulIntegOrders*Flow::nCumulInteg_qMax; pq++) {   
	int theCumulOrder = (pq/Flow::nCumulInteg_qMax) + 1; // like 1,2,3. 
	// not begining with 0. That is "p" in (B3, PG5)
	//int qIndex = pq%(Flow::nCumulInteg_qMax); // like 0,1,..qMax-1 
	// begining with 0. "q" in (B3, PG5)

	histFull[k].histFullHar[j].mHistCumulIntegG0Sum[pq]-> // don't move it.
          SetBinContent(1,histFull[k].histFullHar[j].mCumulIntegG0[pq]);

        //mCumulIntegG0 is the sum of IntegG0 until the code right below,
        //where mCumulIntegG0 becomes <Gn(z)> :
	histFull[k].histFullHar[j].mCumulIntegG0[pq] /= 
	  float(histFull[k].histFullHar[j].mNEvent);        // <Gn(z)> (PG 4)


	if (mOldMethod) {
	  CpInteg[theCumulOrder-1] +=
	    (log(histFull[k].histFullHar[j].mCumulIntegG0[pq]) /
	     ((float)Flow::nCumulInteg_qMax));
	} else {
	  CpInteg[theCumulOrder-1] +=
	    (mAvMult*(pow(histFull[k].histFullHar[j].mCumulIntegG0[pq], 1./mAvMult) -1.) /
	     float(Flow::nCumulInteg_qMax)); // (B3, PG6) 
	}
      }
      
      // add Xpq Ypq denominator to write out file list
      for (int pq = 0; pq < Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax; pq++) {
	XpqYpqDenomNames->AddLast(histFull[k].histFullHar[j].mCumulG0Denom2D[pq]);
	XpqYpqDenomNames->AddLast(histFull[k].histFullHar[j].mCumulG0DenomPt[pq]);
	XpqYpqDenomNames->AddLast(histFull[k].histFullHar[j].mCumulG0DenomEta[pq]);
	XpqYpqDenomNames->AddLast(histFull[k].histFullHar[j].mCumulG0Denom[pq]);
      }

      cumulInteg1[j] = // (B5, PG7)
	(3.*CpInteg[1-1] - (3./2.)*CpInteg[2-1] + (1./3.)*CpInteg[3-1]) / r0Sq;
      
      cumulInteg2[j] = ((-10.*CpInteg[1-1]) + (8.*CpInteg[2-1]) - 
			 (2.*CpInteg[3-1])) / (r0Sq*r0Sq); 
      
      cumulInteg3[j] = ( (18.*CpInteg[1-1]) - (18.*CpInteg[2-1]) + (6.*CpInteg[3-1]))
			/ (r0Sq*r0Sq*r0Sq);
      
      // now histograms for flow results:
      histFull[k].histFullHar[j].mHist_v2D  = new TH2D*[Flow::nCumulDiffOrders];
      histFull[k].histFullHar[j].mHist_vEta = new TH1D*[Flow::nCumulDiffOrders];
      histFull[k].histFullHar[j].mHist_vPt  = new TH1D*[Flow::nCumulDiffOrders];
      
      for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++) {
	char theCumulOrder[2]; // if >10, need to use char*
	sprintf(theCumulOrder,"%d",(ord+1)*2);
	
	histTitle = new TString("Flow_Cumul_v2D_Order");
	histTitle->Append(*theCumulOrder);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mHist_v2D[ord] = 
	  new TH2D(*(histFull[k].histFullHar[j].mHistCumul2D[ord]->
		     ProjectionXY(histTitle->Data(),"e")));
	histFull[k].histFullHar[j].mHist_v2D[ord]->SetTitle(histTitle->Data());
	histFull[k].histFullHar[j].mHist_v2D[ord]->SetXTitle((char*)xLabel.Data());
	histFull[k].histFullHar[j].mHist_v2D[ord]->SetYTitle("Pt (GeV)");
	histFull[k].histFullHar[j].mHist_v2D[ord]->SetZTitle("v (%)");
	delete histTitle;
	
if (ord>0) histFull[k].histFullHar[j].mHist_v2D[ord]->Scale(1./profScale);

	histTitle = new TString("Flow_Cumul_vEta_Order");
	histTitle->Append(*theCumulOrder);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mHist_vEta[ord] = 
	  new  TH1D(*(histFull[k].histFullHar[j].mHistCumulEta[ord]->
		      ProjectionX(histTitle->Data(),"e")));
	histFull[k].histFullHar[j].mHist_vEta[ord]->SetTitle(histTitle->Data());
	histFull[k].histFullHar[j].mHist_vEta[ord]->SetXTitle((char*)xLabel.Data());
	histFull[k].histFullHar[j].mHist_vEta[ord]->SetYTitle("v (%)");
	delete histTitle;
	
if (ord>0) histFull[k].histFullHar[j].mHist_vEta[ord]->Scale(1./profScale);

	histTitle = new TString("Flow_Cumul_vPt_Order");
	histTitle->Append(*theCumulOrder);
	histTitle->Append("_Sel");
	histTitle->Append(*countSels);
	histTitle->Append("_Har");
	histTitle->Append(*countHars);
	histFull[k].histFullHar[j].mHist_vPt[ord] = 
	  new  TH1D(*(histFull[k].histFullHar[j].mHistCumulPt[ord]->
		      ProjectionX(histTitle->Data(),"e")));
	histFull[k].histFullHar[j].mHist_vPt[ord]->SetTitle(histTitle->Data());
	histFull[k].histFullHar[j].mHist_vPt[ord]->SetXTitle("Pt (GeV)");
	histFull[k].histFullHar[j].mHist_vPt[ord]->SetYTitle("v (%)");
	delete histTitle;

if (ord>0) histFull[k].histFullHar[j].mHist_vPt[ord]->Scale(1./profScale);

      }
      
      if (mOldMethod) {
	q2[j] = cumulInteg1[j]-1.;  // old paper (Eq. 74a)
	q4[j] = -1.*cumulInteg2[j]-(1./mAvWgtMult_q4); 
	q6[j] = (1./4.)*cumulInteg3[j]-(1./(mAvWgtMult_q6)); 
	meanIntegV[j]  = sqrt(q2[j]);        // <Q>  for 2-part,  m=1
	meanIntegV2[j] = q2[j];              // <Q**2>for 2-part, m=2
	meanIntegV3[j] = pow(q4[j],3./4.);   // <Q**3>for 4-part, m=1
	meanIntegV4[j] = q4[j];              // <Q**4>for 4-part, m=2
       
	if (q2[j]<0.) cout<<" Sel"<<k+1<<", <Q**2> less than zero ! v"
			  <<j+1<<" from 2 particle correlation failed."<<endl;
	if (q4[j]<0.) cout<<" Sel"<<k+1<<", <Q**4> less than zero ! v"
			  <<j+1<<" from 4 particle correlation failed."<<endl;
         
      } else { // new method, Eq. (PG8)
	meanIntegV[j]  = sqrt(cumulInteg1[j]);           // <v>    2-part, m=1
	meanIntegV2[j] = cumulInteg1[j];                 // <v**2> 2-part, m=2
	meanIntegV3[j] = pow(-1.*cumulInteg2[j], 3./4.); // <v**3> 4-part, m=1
	meanIntegV4[j] = -1.*cumulInteg2[j];             // <v**4> 4-part, m=2

        if (meanIntegV2[j]<0.) cout<<" Sel"<<k+1<<", <V**2> less than zero ! v"
			  <<j+1<<" from 2 particle correlation failed."<<endl;
        if (meanIntegV4[j]<0.) cout<<" Sel"<<k+1<<", <V**4> less than zero ! v"
			  <<j+1<<" from 4 particle correlation failed."<<endl;

      }
      
      if (m_M==1) { // Eq. (PG14)
	histFull[k].histFullHar[j].mHist_v2D[0]->Scale(1./(meanIntegV[j]*perCent)); // (34a)
	histFull[k].histFullHar[j].mHist_v2D[1]->Scale(-1./(meanIntegV3[j]*perCent));// (34b)
	histFull[k].histFullHar[j].mHist_vEta[0]->Scale(1./(meanIntegV[j]*perCent)); // (34a)
	histFull[k].histFullHar[j].mHist_vEta[1]->Scale(-1./(meanIntegV3[j]*perCent)); // (34b)
	histFull[k].histFullHar[j].mHist_vPt[0]->Scale(1./(meanIntegV[j]*perCent)); // (34a)
	histFull[k].histFullHar[j].mHist_vPt[1]->Scale(-1./(meanIntegV3[j]*perCent)); // (34b)
      } else if (m_M==2) {
	histFull[k].histFullHar[j].mHist_v2D[0]->Scale(1./(meanIntegV2[j]*perCent)); // (35a)
        histFull[k].histFullHar[j].mHist_v2D[1]->Scale(-0.5/(meanIntegV4[j]*perCent)); // (35b)
	histFull[k].histFullHar[j].mHist_vEta[0]->Scale(1./(meanIntegV2[j]*perCent)); 	// (35a)
        histFull[k].histFullHar[j].mHist_vEta[1]->Scale(-0.5/(meanIntegV4[j]*perCent) ); // (35b)
	histFull[k].histFullHar[j].mHist_vPt[0]->Scale(1./(meanIntegV2[j]*perCent)); // (35a)
        histFull[k].histFullHar[j].mHist_vPt[1]->Scale(-0.5/(meanIntegV4[j]*perCent)); // (35b)
      }
      
      for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++) {
	AddHist(histFull[k].histFullHar[j].mHist_v2D[ord]);
	AddHist(histFull[k].histFullHar[j].mHist_vEta[ord]);
	AddHist(histFull[k].histFullHar[j].mHist_vPt[ord]);
      }
    }
    
    if (m_M==1) {
      
      TH1D* histOfMeanIntegV = new TH1D(*(histFull[k].mHist_v[0]));
      histOfMeanIntegV->Reset();
      
      TH1D* histOfMeanIntegV3 = new TH1D(*(histFull[k].mHist_v[1]));
      histOfMeanIntegV3->Reset();
      
      for (int j = 1; j < Flow::nHars+1; j++) {
	histOfMeanIntegV->SetBinContent(j, 1./(meanIntegV[j-1]*perCent));
	histOfMeanIntegV->SetBinError(j,0.);
	histOfMeanIntegV3->SetBinContent(j, -1./(meanIntegV3[j-1]*perCent));
	histOfMeanIntegV3->SetBinError(j,0.);
      }
      histFull[k].mHist_v[0]->Multiply(histOfMeanIntegV);
      histFull[k].mHist_v[1]->Multiply(histOfMeanIntegV3);
      
      //force to be zero if the value is "nan"
      for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++){
	for (int j=0; j<histFull[k].mHist_v[ord]->GetNbinsX(); j++) {
          if ( !(histFull[k].mHist_v[ord]->GetBinContent(j) < FLT_MAX &&
                 histFull[k].mHist_v[ord]->GetBinContent(j) > -1.*FLT_MAX) ) {
	    histFull[k].mHist_v[ord]->SetBinContent(j,0.);
	    histFull[k].mHist_v[ord]->SetBinError(j,0.);
	  }
	}
      }

      for (int j = 1; j < Flow::nHars+1; j++) {
	cout << "##### 2-part v" << j << " = (" 
	     << histFull[k].mHist_v[0]->GetBinContent(j) 
	     <<" +/- "<< histFull[k].mHist_v[0]->GetBinError(j)<<" )"<<endl;
	cout << "##### 4-part v" << j << " = (" 
	     << histFull[k].mHist_v[1]->GetBinContent(j) 
	     <<" +/- "<< histFull[k].mHist_v[1]->GetBinError(j)<<" )"<<endl;
      }
      
      delete histOfMeanIntegV; 
      delete histOfMeanIntegV3;
      
    } else if (m_M==2) {
      
      TH1D* histOfMeanIntegV2 = new TH1D(*(histFull[k].mHist_v[0]));
      histOfMeanIntegV2->Reset();
      
      TH1D* histOfMeanIntegV4 = new TH1D(*(histFull[k].mHist_v[1]));
      histOfMeanIntegV4->Reset();
      
      for (int j = 1; j < Flow::nHars+1; j++) {
	histOfMeanIntegV2->SetBinContent(j, 1./(meanIntegV2[j-1]*perCent));
	histOfMeanIntegV2->SetBinError(j,0.);
	histOfMeanIntegV4->SetBinContent(j, -0.5/(meanIntegV4[j-1]*perCent));
	histOfMeanIntegV4->SetBinError(j,0.);
      }
      histFull[k].mHist_v[0]->Multiply(histOfMeanIntegV2);
      histFull[k].mHist_v[1]->Multiply(histOfMeanIntegV4);

      //force to be zero if the value is "nan"
      for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++){
	for (int j=0; j<histFull[k].mHist_v[ord]->GetNbinsX(); j++) {
          if ( !(histFull[k].mHist_v[ord]->GetBinContent(j) < FLT_MAX &&
                 histFull[k].mHist_v[ord]->GetBinContent(j) > -1.*FLT_MAX) ){
	    histFull[k].mHist_v[ord]->SetBinContent(j,0.);
	    histFull[k].mHist_v[ord]->SetBinError(j,0.);
	  }
	}
      }

      for (int j = 1; j < Flow::nHars+1; j++) {
	cout << "##### 2-part v" << j << " = (" 
	     << histFull[k].mHist_v[0]->GetBinContent(j) 
	     <<") +/- "<< histFull[k].mHist_v[0]->GetBinError(j)<<endl;
	cout << "##### 4-part v" << j << " = (" 
	     << histFull[k].mHist_v[1]->GetBinContent(j) 
	     <<") +/- "<< histFull[k].mHist_v[1]->GetBinError(j)<<endl;
      }
      
      delete histOfMeanIntegV2; 
      delete histOfMeanIntegV4;
    }
    
    for (int ord = 0; ord < Flow::nCumulDiffOrders; ord++)
      AddHist(histFull[k].mHist_v[ord]);
    
  }

  // GetHistList()->ls();
  
  // Write most histograms
  TFile histFile("flow.cumulant.root", "RECREATE");
  TList* hisList = GetHistList(); 
  for (int k = 0; k < Flow::nSels; k++) {
    for (int j = 0; j < Flow::nHars; j++) {
      for (int pq = 0; pq <  Flow::nCumulDiffOrders*Flow::nCumulDiff_qMax; pq++) {
	hisList->Remove(histFull[k].histFullHar[j].mCumulG0Denom2D[pq]);
	hisList->Remove(histFull[k].histFullHar[j].mCumulG0DenomPt[pq]);
	hisList->Remove(histFull[k].histFullHar[j].mCumulG0DenomEta[pq]);
	hisList->Remove(histFull[k].histFullHar[j].mCumulG0Denom[pq]);
      }
    }
  }

  TVectorD* cumulConstants = new TVectorD(30);
  (*cumulConstants)(0)=double(Flow::nHars);
  (*cumulConstants)(1)=double(Flow::nSels);
  (*cumulConstants)(2)=double(Flow::nSubs);
  (*cumulConstants)(3)=double(Flow::nPhiBins);
  (*cumulConstants)(4)=double(Flow::nPhiBinsFtpc);
  (*cumulConstants)(5)=double(mNEtaBins);
  (*cumulConstants)(6)=double(Flow::nPtBins);
  (*cumulConstants)(7)=double(Flow::nCumulIntegOrders);
  (*cumulConstants)(8)=double(Flow::nCumulInteg_qMax);
  (*cumulConstants)(9)=double(Flow::nCumulDiffOrders);
  (*cumulConstants)(10)=double(Flow::nCumulDiff_qMax);
  (*cumulConstants)(11)=r0;
  (*cumulConstants)(12)=m_M;
  (*cumulConstants)(13)=(strlen(pFlowSelect->PidPart()) != 0) ? 1 : 0;//1,pidflow
  (*cumulConstants)(14)=double(profScale);//1,pidflow

  cumulConstants->Write("CumulConstants",TObject::kOverwrite | TObject::kSingleKey);

  TObjString* cumulMethodTag 
       = new TObjString( (mOldMethod) ? "cumulOld" : "cumulNew" );
  cumulMethodTag->Write("CumulMethodTag",TObject::kOverwrite | TObject::kSingleKey);

  hisList->Write();
  
  histFile.Close();
  
  // write profile for <G>, the denominator of XpqYpq.
  TFile XpqYpqDenomNewFile("denominatorNew.root","RECREATE"); 
  XpqYpqDenomNames->Write();
  XpqYpqDenomNewFile.Close();
  delete XpqYpqDenomNames;
    
  delete pFlowSelect;
    
  return StMaker::Finish();
}

//-----------------------------------------------------------------------

void StFlowCumulantMaker::SetHistoRanges(Bool_t ftpc_included) {

    if (ftpc_included) {
	  mEtaMin = Flow::etaMin;
	  mEtaMax = Flow::etaMax;
	mNEtaBins = Flow::nEtaBins;
    }
    else {
	  mEtaMin = Flow::etaMinTpcOnly;
 	  mEtaMax = Flow::etaMaxTpcOnly;
	mNEtaBins = Flow::nEtaBinsTpcOnly;
    }

    return;
}

////////////////////////////////////////////////////////////////////////////
//
// $Log: StFlowCumulantMaker.cxx,v $
// Revision 1.14  2003/03/03 16:24:36  aihong
// blow up 4-part cumulant by 1000 in order to let error bars calculated by ROOT
//
// Revision 1.13  2003/01/10 16:40:36  oldi
// Several changes to comply with FTPC tracks:
// - Switch to include/exclude FTPC tracks introduced.
//   The same switch changes the range of the eta histograms.
// - Eta symmetry plots for FTPC tracks added and separated from TPC plots.
// - PhiWgts and related histograms for FTPC tracks split in FarEast, East,
//   West, FarWest (depending on vertex.z()).
// - Psi_Diff plots for 2 different selections and the first 2 harmonics added.
// - Cut to exclude mu-events with no primary vertex introduced.
//   (This is possible for UPC events and FTPC tracks.)
// - Global DCA cut for FTPC tracks added.
// - Global DCA cuts for event plane selection separated for TPC and FTPC tracks.
// - Charge cut for FTPC tracks added.
//
// Revision 1.12  2002/05/19 18:58:00  aihong
// speed up cumulant
//
// Revision 1.11  2002/02/19 14:42:18  jeromel
// Added float.h for Linux 7.2
//
// Revision 1.10  2002/02/02 01:10:12  posk
// Added documentation
//
// Revision 1.9  2002/01/24 20:53:51  aihong
// add histograms for combining results from parallel jobs
//
// Revision 1.8  2002/01/14 23:42:36  posk
// Renamed ScalerProd histograms. Moved print commands to FlowMaker::Finish().
//
// Revision 1.7  2001/12/21 17:01:59  aihong
// minor changes
//
// Revision 1.6  2001/12/18 19:27:27  posk
// "proton" and "antiproton" replaced by "pr+" and "pr-".
//
// Revision 1.5  2001/12/11 22:04:01  posk
// Four sets of phiWgt histograms.
// StFlowMaker StFlowEvent::PhiWeight() changes.
// Cumulant histogram names changed.
//
// Revision 1.4  2001/12/06 01:21:14  jeromel
// Mandatory correction : Extraneous comma removed.
//
// Revision 1.3  2001/11/09 21:14:50  posk
// Switched from CERNLIB to TMath. Using global dca instead of dca.
//
// Revision 1.2  2001/11/08 03:12:24  aihong
// clean up redundant histograms
//
// Revision 1.1  2001/11/02 04:47:42  aihong
// install cumulant maker
//
////////////////////////////////////////////////////////////////////////////
