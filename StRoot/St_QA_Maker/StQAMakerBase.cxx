// $Id: StQAMakerBase.cxx,v 2.22 2003/02/28 06:17:56 genevb Exp $ 
// $Log: StQAMakerBase.cxx,v $
// Revision 2.22  2003/02/28 06:17:56  genevb
// Allow StQAMakerBase::Make to be called for all events
//
// Revision 2.21  2003/02/20 20:09:54  genevb
// Several changes for new trigger scheme, dAu data
//
// Revision 2.20  2003/02/19 06:38:29  genevb
// Rework trigger and mult/event class sections
//
// Revision 2.19  2003/02/15 22:00:52  genevb
// Add tpcSectors, fix ftpc east/west charge
//
// Revision 2.18  2002/04/23 01:59:56  genevb
// Addition of BBC/FPD histos
//
// Revision 2.17  2002/03/11 20:59:58  genevb
// Fixed bug with placement of trigger hists
//
// Revision 2.16  2002/03/01 22:51:20  genevb
// Small set-to-zero possible bug fix
//
// Revision 2.15  2002/02/12 18:42:00  genevb
// Additional FTPC histograms
//
// Revision 2.14  2002/01/26 03:04:07  genevb
// Fixed some problems with fcl histos
//
// Revision 2.13  2002/01/21 22:09:24  genevb
// Include some ftpc histograms from StFtpcClusterMaker
//
// Revision 2.12  2001/12/28 09:19:13  genevb
// Adjustments for pp running
//
// Revision 2.11  2001/12/20 03:11:08  genevb
// pp trigger words 0x2XXX
//
// Revision 2.10  2001/09/10 18:00:13  genevb
// Another trigger word
//
// Revision 2.9  2001/08/29 20:45:15  genevb
// Trigger word histos
//
// Revision 2.8  2001/08/07 07:51:28  lansdell
// primvtx check for different multiplicities crashed for MC data, now fixed
//
// Revision 2.7  2001/05/23 00:14:53  lansdell
// more changes for qa_shift histograms
//
// Revision 2.6  2001/05/16 20:57:03  lansdell
// new histograms added for qa_shift printlist; some histogram ranges changed; StMcEvent now used in StEventQA
//
// Revision 2.5  2001/04/28 22:05:13  genevb
// Added EMC histograms
//
// Revision 2.4  2001/04/26 16:34:40  genevb
// Fixed some histogram ranges
//
// Revision 2.3  2000/08/25 22:06:15  genevb
// Raised high mult bin to 2500 global tracks
//
// Revision 2.2  2000/08/25 20:29:34  lansdell
// year1 print list changed slightly; cosmetic improvement of some event summary histograms
//
// Revision 2.1  2000/08/25 16:04:10  genevb
// Introduction of files
//
// 
///////////////////////////////////////////////////////////////////////////
//                                                                       //
//  StQAMakerBase base class for QA Histogram Makers                     //
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#include "StMessMgr.h"
#include "StQAMakerBase.h"
#include "StQABookHist.h"
#include "QAH.h"
#include "TList.h"

ClassImp(StQAMakerBase)

//_____________________________________________________________________________
StQAMakerBase::StQAMakerBase(const char *name, const char *title, const char* type) :
 StMaker(name,title), QAMakerType(type) {

  hists = 0;
  histsList.Expand(32);
  histsSet = StQA_Undef;
  eventClass = 3;

//  - Set all the histogram booking constants

  ntrk   = 50;
  nmnpt  = 50;
  nmneta = 50;
  nxyz   = 50;

//  - Zero all histogram pointers
  mNullPrimVtx = 0; // histogram for number of events without primary vertex
  mMultClass = 0;   // histogram for number of events in mult classes
  mTrigWord = 0;    // histogram for event trigger words
  mTrigBits = 0;    // histogram for event trigger bits
  for (Int_t i=0; i<24; i++) mTpcSectorPlot[i] = 0;

// for method MakeEvSum - from table software monitor
  m_glb_trk_chg=0;          //! all charge east/west, tpc
  m_glb_trk_chgF=0;         //! all charge east/west, ftpc

// FTPC histograms
  m_ftpc_chargestepW=0; //! Chargestep from ftpc west
  m_ftpc_chargestepE=0; //! Chargestep from ftpc east
  m_ftpc_fcl_radialW=0;  //! ftpc west cluster radial position
  m_ftpc_fcl_radialE=0;  //! ftpc east cluster radial position

}
//_____________________________________________________________________________
StQAMakerBase::~StQAMakerBase() {
}
//_____________________________________________________________________________
Int_t StQAMakerBase::Init() {
// Histogram booking must wait until first event Make() to determine event type
  firstEvent = kTRUE;
  firstEventClass = kTRUE;
  return StMaker::Init();
}
//_____________________________________________________________________________
void StQAMakerBase::Clear(Option_t* opt) {
  firstEventClass = kTRUE;
  StMaker::Clear(opt);
}
//_____________________________________________________________________________
Int_t StQAMakerBase::Make() {

  if (Debug())
    gMessMgr->Info(" In StQAMakerBase::Make()");

  if (firstEvent) BookHist();
  // See BookHist() for definitions of histsSet values,
  // which should be set during Make() of the derived QA Maker class
  // event class also decided in derived Make()
  switch (histsSet) {
    case (StQA_AuAu) :
      mMultClass->Fill((float) eventClass);
    default : {
      if (!eventClass) return kStOk;
      hists = (StQABookHist*) histsList.At((--eventClass));
    }
  }

  if (!hists) NewQABookHist();
    

  if (!fillHists) return kStOk;
  // Call methods to fill histograms


  // Those not divided by event class:
  if (firstEventClass) {
    // histograms from table event_summary
    MakeHistEvSum();
    
    firstEventClass = kFALSE;
  }

  // Those divided by event class:
  // histograms from table globtrk
  MakeHistGlob();
  // histograms from table primtrk - must be done after global tracks
  MakeHistPrim();
  // histograms from table primtrk & dst_dedx
  MakeHistPID();
  // histograms from table dst_dedx
  MakeHistDE();
  // histograms from table point
  MakeHistPoint();
  // histograms from table g2t_rch_hit
  MakeHistRich();
  // histograms from table dst_vertex,dst_v0_vertex,dst_xi_vertex,dst_kinkVertex
  MakeHistVertex();
  // histograms from EMC in StEvent
  MakeHistEMC();
  // histograms from geant and reco tables 
  if (histsSet==StQA_MC) MakeHistEval();
  // histograms from BBC in StEvent
  MakeHistBBC();
  // histograms from FPD in StEvent
  MakeHistFPD();

  return kStOk;
}
//_____________________________________________________________________________
void StQAMakerBase::NewQABookHist() {
  const char* pre = prefix[eventClass].Data();
  if (Debug())
    gMessMgr->Info() <<
      "StQAMakerBase: booking histograms with prefix: " <<
      pre << endm;
  QAH::maker = (StMaker*) (this);
  hists = new StQABookHist(pre);
  histsList.AddAt(hists,eventClass);
  hists->BookHist(histsSet);
}
//_____________________________________________________________________________
TH2F* StQAMakerBase::MH1F(const Text_t* name, const Text_t* title,
                          Int_t nbinsx, Axis_t xlow, Axis_t xup) {
  TH2F* h = QAH::MH1F(name,title,nbinsx,xlow,xup,eventClass);
  if (eventClass>1) {
    h->Rebin(0,"low mult");
    h->Rebin(1,"mid mult");
    h->Rebin(2,"high mult");
  }
  return h;
}
//_____________________________________________________________________________
void StQAMakerBase::BookHist() {  
// book histograms

  firstEvent = kFALSE;
  Int_t tempClass = eventClass;

  switch (histsSet) {

    // Real data with three multiplicity classes (low, medium, high)
    case (StQA_AuAu) : {
      (prefix[0] = QAMakerType) += "LM";
      (prefix[1] = QAMakerType) += "MM";
      (prefix[2] = QAMakerType) += "HM";
      eventClass = 3;
      break; }

    // Real data with event classes for different triggers
    case (StQA_dAu) : {
      prefix[0] = QAMakerType;  // Minbias
      (prefix[1] = QAMakerType) += "HP";
      (prefix[2] = QAMakerType) += "XX";
      eventClass = 3;
      break; }

 // the following data sets use the defaults
 
    // Generic data (e.g. Monte Carlo) with just one event class
    case (StQA_MC) :

    // pp data with just one event class
    case (StQA_pp) :

    default  : {
      prefix[0] = QAMakerType;
      eventClass = 1;
    }
  }
  
  QAH::maker = (StMaker*) (this);
  QAH::preString = QAMakerType;

  BookHistTrigger();
  BookHistGeneral();
  BookHistEvSum();
  BookHistFcl();

  Int_t tempClass2 = eventClass;
  // Must book the histograms with no special prefix now
  for (eventClass=0; eventClass<tempClass2; eventClass++) {
    if (!(QAMakerType.CompareTo(prefix[eventClass]))) {
      NewQABookHist();
      break;
    }
  }
  eventClass = tempClass;
  
}
//_____________________________________________________________________________
void StQAMakerBase::BookHistGeneral(){  

  mNullPrimVtx = QAH::H1F("QaNullPrimVtx","event primary vertex check",40,-2,2);
  mNullPrimVtx->SetXTitle("has primary vertex? (yes = 1, no = -1)");
  mNullPrimVtx->SetYTitle("# of events");

  if (histsSet == StQA_AuAu) {
    mMultClass = QAH::H1F("QaMultClass","event multiplicity class",5,-0.5,4.5);
    mMultClass->SetXTitle("mult class (0=?/MC, 1=LM, 2=MM, 3=HM)");
    mMultClass->SetYTitle("# of events");
  }

  char namebuf[32];
  char titlebuf[64];
  for (Int_t i=0; i<24; i++) {
    sprintf(namebuf ,"QaTpcSector%d",i+1);
    sprintf(titlebuf,"Hits in TPC Sector %d",i+1);
    //mTpcSectorPlot[i] = QAH::H2F(namebuf,titlebuf,104,-52.,52.,150,50.,200.);
    mTpcSectorPlot[i] = QAH::H2F(namebuf,titlebuf,104,-52.,52.,47,-0.5,46.5);
    mTpcSectorPlot[i]->SetXTitle("along padrows [cm]");
    //mTpcSectorPlot[i]->SetYTitle("across padrows [cm]");
    mTpcSectorPlot[i]->SetYTitle("padrow");
  }
}
//_____________________________________________________________________________
void StQAMakerBase::BookHistTrigger(){  

  QAH::maker = (StMaker*) (this);
  QAH::preString = QAMakerType;
  if (mTrigWord) return;
  mTrigWord = QAH::H1F("QaTrigWord","trigger word",8,0.5,8.5);
  mTrigWord->SetXTitle("1=MinBias, 2=Central, 3=Other Physics, 4=pp, 7=Laser, 8=Other");
  mTrigBits = QAH::H1F("QaTrigBits","trigger bits",32,-0.5,31.5);
}
//_____________________________________________________________________________
void StQAMakerBase::BookHistEvSum(){  

// for method MakeEvSum - from software monitor

  m_glb_trk_chg = MH1F("QaEvsumTotChg","softmon: all charge east/west,tpc",60,0,3);
  m_glb_trk_chgF = MH1F("QaEvsumTotChgF","softmon: all charge east/west,ftpc",60,0,3);
}
//_____________________________________________________________________________
void StQAMakerBase::BookHistFcl(){

  // Get fcl histograms from FTPC makers
  if (!(m_ftpc_chargestepW)) {
    // First try to get histograms from StFtpcClusterMaker named "ftpc_hits"
    StMaker* fhMaker = GetMaker("ftpc_hits");
    if (fhMaker) {
      m_ftpc_chargestepW = (TH1F*) (fhMaker->GetHist("fcl_chargestepW"));
      AddHist(m_ftpc_chargestepW);
      m_ftpc_chargestepE = (TH1F*) (fhMaker->GetHist("fcl_chargestepE"));
      AddHist(m_ftpc_chargestepE);
      m_ftpc_fcl_radialW = (TH1F*) (fhMaker->GetHist("fcl_radialW"));
      AddHist(m_ftpc_fcl_radialW);
      m_ftpc_fcl_radialE = (TH1F*) (fhMaker->GetHist("fcl_radialE"));
      AddHist(m_ftpc_fcl_radialE);
    } else {
      // "ftpc_hits" maker doesn't exist, so look in hist branch
      // *** Currently isn't working for bfcread_event_QAhist.C ***
      St_DataSet* hDS = GetDataSet("histBranch");
      if (hDS) {
        // hDS->ls(9);
        St_DataSet* fhDS = hDS->Find("ftpc_hitsHist");
        if (fhDS) {
          m_ftpc_chargestepW =
	    (TH1F*) (fhDS->FindObject("fcl_chargestepW"));
          AddHist(m_ftpc_chargestepW);
          m_ftpc_chargestepE =
	    (TH1F*) (fhDS->FindObject("fcl_chargestepE"));
          AddHist(m_ftpc_chargestepE);
          m_ftpc_fcl_radialW =
	    (TH1F*) (fhDS->FindObject("fcl_radialW"));
          AddHist(m_ftpc_fcl_radialW);
          m_ftpc_fcl_radialE =
	    (TH1F*) (fhDS->FindObject("fcl_radialE"));
          AddHist(m_ftpc_fcl_radialE);
        }
      }
    }
  }
}
//_____________________________________________________________________________
