//////////////////////////////////////////////////////////////////////
//
// $Id: StFlowTagMaker.cxx,v 1.27 2001/12/11 21:28:09 posk Exp $
//
// Authors: Raimond Snellings and Art Poskanzer, LBNL, Jun 1999
//
//////////////////////////////////////////////////////////////////////
//
// Description:  Maker to Fill the Flow EbyE Tag database
//
//////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <stdlib.h>
#include <math.h>
#include "StFlowMaker/StFlowMaker.h"
#include "StFlowMaker/StFlowEvent.h"
#include "StFlowMaker/StFlowConstants.h"
#include "StFlowMaker/StFlowSelection.h"
#include "StFlowTagMaker.h"
#include "PhysicalConstants.h"
#include "SystemOfUnits.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include "TProfile.h"
#include "TVector2.h"
#include "StMessMgr.h"
#define PR(x) cout << "##### FlowTag: " << (#x) << " = " << (x) << endl;

ClassImp(StFlowTagMaker)

//-------------------------------------------------------------

StFlowTagMaker::StFlowTagMaker(const Char_t* name) : StMaker(name) {
  pFlowSelect = new StFlowSelection();
}

//-------------------------------------------------------------

StFlowTagMaker::~StFlowTagMaker() {
}

//-------------------------------------------------------------

Int_t StFlowTagMaker::Make() {
  // Create a new tag
  pFlowEvent  = NULL;
  pFlowTag    = NULL;
  pSt_FlowTag = NULL;

  // instantiate new St_FlowTag class
  pSt_FlowTag = new St_FlowTag("FlowTag",1);      // table header
  if (!pSt_FlowTag) return kStOK;
  // set the size of the table
  pSt_FlowTag->SetNRows(1);
  // add FlowTag table to the root .data directory
  AddData(pSt_FlowTag,".data");
  // get a pointer to the c-struct containing the variables
  pFlowTag = pSt_FlowTag->GetTable();             // table structure
  // print pointer to flowtag 
  if (Debug()) cout << "StTagPointer: " << pSt_FlowTag << endl;
  if (Debug()) cout << "TagPointer: " << pFlowTag << endl;

  // fill the Flow Tags 
  StFlowMaker* pFlowMaker = (StFlowMaker*)GetMaker("Flow");
  if (pFlowMaker) pFlowEvent = pFlowMaker->FlowEventPointer();
  if (pFlowEvent && pFlowTag && pFlowSelect->Select(pFlowEvent)) {
    FillFlowTag();                                // fill the tag database
  } else {
    pFlowTag = NULL;
    gMessMgr->Info("##### FlowTag: null FlowEvent pointers");
    return kStOK;      // no StFlowEvent or no Tag pointer or no selection
  }

  if (Debug()) PrintTag();

  // fill histograms from the Flow Tags
  FillHistograms();

  if (Debug()) StMaker::PrintInfo();

  return kStOK;
}

//-------------------------------------------------------------

void StFlowTagMaker::PrintTag(ostream& os) {
  os << "##### Event-by-Event Flow Tag Table ---" << endl; 
  if (!pFlowTag) {
    os << "(empty FlowTag)" << endl;
  } else {
    int i;
    os <<  "qxa";
    for(i=0; i<4; i++) os << "[" << i << "] =" << pFlowTag->qxa[i] << ' ';
    os << endl;
  }
}

//-------------------------------------------------------------

Int_t StFlowTagMaker::Finish() {

  delete pFlowSelect;

  cout << endl;
  gMessMgr->Summary(3);
  cout << endl;

  return StMaker::Finish();
}

//-------------------------------------------------------------

Int_t StFlowTagMaker::Init() {
  // Book histograms

  enum { 
    nPsiBins    = 100,
    nMeanPtBins = 100,
    nMultBins   = 100,
    n_qBins     = 100 
  }; 

  const Float_t PsiMin    =  0.;
  const Float_t PsiMax    =  twopi; 
  const Float_t MeanPtMin =  0.;
  const Float_t MeanPtMax =  1.;
  const Float_t MultMin   =  0.;
  const Float_t MultMax   =  2000.;
  const Float_t qMin      =  0.;
  const Float_t qMax      =  2.;

  for (int i = 0; i < Flow::nSels * Flow::nSubs; i++) {
    TString* histTitle;
    char countSubEvents[5];
    sprintf(countSubEvents,"%d",i);

    for (int j = 0; j < Flow::nHars; j++) {
      char countHarmonics[5];
      sprintf(countHarmonics,"%d",j);

      histTitle = new TString("FlowPsi");
      histTitle->Append(*countSubEvents);
      histTitle->Append("Harmonic");
      histTitle->Append(*countHarmonics + 1);
      histSubEvents[i].histHarmonics[j].mHistPsi =
	new TH1F(histTitle->Data(), histTitle->Data(), nPsiBins, PsiMin,
		 (PsiMax / (float)(j + 1)));
      histSubEvents[i].histHarmonics[j].mHistPsi->SetXTitle("Psi (rad)");
      histSubEvents[i].histHarmonics[j].mHistPsi->SetYTitle("Counts");
      delete histTitle;

      histTitle = new TString("FlowMeanPt");
      histTitle->Append(*countSubEvents);
      histTitle->Append("Harmonic");
      histTitle->Append(*countHarmonics + 1);
      histSubEvents[i].histHarmonics[j].mHistMeanPt =
	new TH1F(histTitle->Data(), histTitle->Data(), nMeanPtBins, MeanPtMin,
		 MeanPtMax);
      histSubEvents[i].histHarmonics[j].mHistMeanPt->
	SetXTitle("Mean Pt (GeV)");
      histSubEvents[i].histHarmonics[j].mHistMeanPt->SetYTitle("Counts");
      delete histTitle;

      histTitle = new TString("FlowMult");
      histTitle->Append(*countSubEvents);
      histTitle->Append("Harmonic");
      histTitle->Append(*countHarmonics + 1);
      histSubEvents[i].histHarmonics[j].mHistMult =
	new TH1D(histTitle->Data(), histTitle->Data(), nMultBins, MultMin,
		 MultMax);
      histSubEvents[i].histHarmonics[j].mHistMult->
	SetXTitle("Multiplicity");
      histSubEvents[i].histHarmonics[j].mHistMult->SetYTitle("Counts");
      delete histTitle;

      histTitle = new TString("Flow_q");
      histTitle->Append(*countSubEvents);
      histTitle->Append("Harmonic");
      histTitle->Append(*countHarmonics + 1);
      histSubEvents[i].histHarmonics[j].mHist_q =
	new TH1F(histTitle->Data(), histTitle->Data(), n_qBins, qMin, qMax);
      histSubEvents[i].histHarmonics[j].mHist_q->SetXTitle("q (|Q|/sqrt N)");
      histSubEvents[i].histHarmonics[j].mHist_q->SetYTitle("Counts");
      delete histTitle;

    }
  }

  gMessMgr->SetLimit("##### FlowTag", 5);
  gMessMgr->Info("##### FlowTag: $Id: StFlowTagMaker.cxx,v 1.27 2001/12/11 21:28:09 posk Exp $");
  if (Debug()) StMaker::PrintInfo();

  return StMaker::Init();
}

//-------------------------------------------------------------

void StFlowTagMaker::FillFlowTag() {  
  // Fill Tag table

  TVector2 Q;

  for (int j = 0; j < Flow::nHars ; j++) {
    pFlowSelect->SetHarmonic(j);

    // fill sub1 tags
    pFlowSelect->SetSelection(0);
    pFlowSelect->SetSubevent(0);
    Q = pFlowEvent->Q(pFlowSelect);
    pFlowTag->qxa[j]  = Q.X();
    pFlowTag->qya[j]  = Q.Y();
    pFlowTag->na[j]   = pFlowEvent->Mult(pFlowSelect);
    pFlowTag->mpta[j] = pFlowEvent->MeanPt(pFlowSelect);

    // fill sub2 tags
    pFlowSelect->SetSelection(0);
    pFlowSelect->SetSubevent(1);
    Q = pFlowEvent->Q(pFlowSelect);
    pFlowTag->qxb[j]  = Q.X();
    pFlowTag->qyb[j]  = Q.Y();
    pFlowTag->nb[j]   = pFlowEvent->Mult(pFlowSelect);
    pFlowTag->mptb[j] = pFlowEvent->MeanPt(pFlowSelect);

    // fill sub3 tags
    pFlowSelect->SetSelection(1);
    pFlowSelect->SetSubevent(0);
    Q = pFlowEvent->Q(pFlowSelect);
    pFlowTag->qxc[j]  = Q.X();
    pFlowTag->qyc[j]  = Q.Y();
    pFlowTag->nc[j]   = pFlowEvent->Mult(pFlowSelect);
    pFlowTag->mptc[j] = pFlowEvent->MeanPt(pFlowSelect);

    // fill sub4 tags
    pFlowSelect->SetSelection(1);
    pFlowSelect->SetSubevent(1);
    Q = pFlowEvent->Q(pFlowSelect);
    pFlowTag->qxd[j]  = Q.X();
    pFlowTag->qyd[j]  = Q.Y();
    pFlowTag->nd[j]   = pFlowEvent->Mult(pFlowSelect);
    pFlowTag->mptd[j] = pFlowEvent->MeanPt(pFlowSelect);

  }

}

//-------------------------------------------------------------

Int_t StFlowTagMaker::FillHistograms() {
  // Fill histograms from Tag table

  for (int j = 0; j < Flow::nHars; j++) {
    float order = (float)(j+1);

    histSubEvents[0].histHarmonics[j].mHistMeanPt->
      Fill(pFlowTag->mpta[j]);
    histSubEvents[0].histHarmonics[j].mHistMult->Fill(pFlowTag->na[j]);
    histSubEvents[0].histHarmonics[j].mHist_q->
      Fill(sqrt(pFlowTag->qxa[j]*pFlowTag->qxa[j] +
	pFlowTag->qya[j]*pFlowTag->qya[j]) / sqrt((double)(pFlowTag->na[j])));
    histSubEvents[1].histHarmonics[j].mHistMeanPt->
      Fill(pFlowTag->mptb[j]);
    histSubEvents[1].histHarmonics[j].mHistMult->Fill(pFlowTag->nb[j]);
    histSubEvents[1].histHarmonics[j].mHist_q->
      Fill(sqrt(pFlowTag->qxb[j]*pFlowTag->qxb[j] +
	pFlowTag->qyb[j]*pFlowTag->qyb[j]) / sqrt((double)(pFlowTag->nb[j])));
    histSubEvents[2].histHarmonics[j].mHistMeanPt->
      Fill(pFlowTag->mptc[j]);
    histSubEvents[2].histHarmonics[j].mHistMult->Fill(pFlowTag->nc[j]);
    histSubEvents[2].histHarmonics[j].mHist_q->
      Fill(sqrt(pFlowTag->qxc[j]*pFlowTag->qxc[j] +
        pFlowTag->qyc[j]*pFlowTag->qyc[j]) / sqrt((double)(pFlowTag->nc[j])));
    histSubEvents[3].histHarmonics[j].mHistMeanPt->
      Fill(pFlowTag->mptd[j]);
    histSubEvents[3].histHarmonics[j].mHistMult->Fill(pFlowTag->nd[j]);
    histSubEvents[3].histHarmonics[j].mHist_q->
      Fill(sqrt(pFlowTag->qxd[j]*pFlowTag->qxd[j] +
        pFlowTag->qyd[j]*pFlowTag->qyd[j]) / sqrt((double)(pFlowTag->nd[j])));

    float EventPlaneAngle1 = atan2(pFlowTag->qya[j], pFlowTag->qxa[j]) / order;
    if (EventPlaneAngle1 < 0.) {EventPlaneAngle1 += twopi / order;}
    histSubEvents[0].histHarmonics[j].mHistPsi->Fill(EventPlaneAngle1);
    float EventPlaneAngle2 = atan2(pFlowTag->qyb[j], pFlowTag->qxb[j]) / order;
    if (EventPlaneAngle2 < 0.) {EventPlaneAngle2 += twopi / order;}
    histSubEvents[1].histHarmonics[j].mHistPsi->Fill(EventPlaneAngle2);
    float EventPlaneAngle3 = atan2(pFlowTag->qyc[j], pFlowTag->qxc[j]) / order;
    if (EventPlaneAngle3 < 0.) {EventPlaneAngle3 += twopi / order;}
    histSubEvents[2].histHarmonics[j].mHistPsi->Fill(EventPlaneAngle3);
    float EventPlaneAngle4 = atan2(pFlowTag->qyd[j], pFlowTag->qxd[j]) / order;
    if (EventPlaneAngle4 < 0.) {EventPlaneAngle4 += twopi / order;}
    histSubEvents[3].histHarmonics[j].mHistPsi->Fill(EventPlaneAngle4);
  }

  return kStOK;
}

//////////////////////////////////////////////////////////////////////
//
// $Log: StFlowTagMaker.cxx,v $
// Revision 1.27  2001/12/11 21:28:09  posk
// Uses Flow class in StFlowMaker::StFlowConstants
//
// Revision 1.26  2001/05/22 19:51:54  posk
// Put log comments at end of file.
//
// Revision 1.25  2000/06/30 14:43:37  posk
// Using MessageMgr.
//
// Revision 1.24  2000/05/26 21:26:51  posk
// Number of subevents fixed at 2.
//
// Revision 1.21  2000/03/28 23:23:25  posk
// Allow multiple instances of the AnalysisMaker.
//
// Revision 1.20  2000/03/15 23:30:55  posk
// Added StFlowSelection.
//
// Revision 1.19  2000/03/08 02:28:28  posk
// Increased the range of the multiplicity histograms to prevent overflow.
//
// Revision 1.18  2000/03/02 23:00:08  posk
// Changed header file extensions from .hh to .h .
//
// Revision 1.17  2000/02/29 21:53:29  posk
// Removed static const int& statements.
//
// Revision 1.16  2000/02/23 22:14:08  posk
// Renamed histograms to contain "Flow".
//
// Revision 1.14  2000/01/20 02:00:05  snelling
// Fixed StFlowTag table size SetNRows=1 (Thanks Iwona)
//
// Revision 1.13  2000/01/14 05:44:34  snelling
// Added St_FlowTag Table to .data
//
// Revision 1.11  2000/01/14 01:36:02  snelling
// changed include path ../FlowMaker/ to FlowMaker/
//
// Revision 1.10  2000/01/13 23:22:05  snelling
// modified sum pt (spt) to mean pt (mpt)
//
// Revision 1.7  1999/12/15 21:56:21  posk
// Increased number of harmonics from 4 to 6.
//
// Revision 1.6  1999/12/07 23:39:15  snelling
// Fixed Linux warnings
//
// Revision 1.5  1999/12/04 00:13:35  posk
// Works with StFlowEvent which works with the new StEvent
//
// Revision 1.4  1999/11/11 23:12:59  posk
// Rearrangement of files.
//
// Revision 1.3  1999/08/09 21:43:05  snelling
// removed parameters from cxx file
//
// Revision 1.2  1999/07/26 23:39:34  snelling
// added histograms for FlowTag QA
//
//////////////////////////////////////////////////////////////////////
