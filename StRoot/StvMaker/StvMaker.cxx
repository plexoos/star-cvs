// $Id: StvMaker.cxx,v 1.2 2010/09/29 23:42:28 perev Exp $
/*!
\author V Perev 2010

A maker StvMaker is a steering maker for Stv package.
<br>
Main tasks:				
<ul>
<li> Create StvHits;			
<li> Make tracks;				
<li> Make Primary vertices;		
<li> Make Primary tracks;			
<li> Save produced data into StEvent.	
</ul>
More detailed: 				<br>
<ul>
<li>On Init:				
<ul>
<li> Detectors initialization. 
     SetAttr("useTpc"  ,1) && SetAttr("activeTpc"  ,1) 	// default
     SetAttr("useSvt",  1) && SetAttr("activeSvt"  ,0) 	// default
     SetAttr("useSsd"  ,0) && SetAttr("activeSsd"  ,0)	// default Off
     SetAttr("usePixel",0) && SetAttr("activePixel",0)	// default Off
     SetAttr("useIst"  ,0) && SetAttr("activeIst"  ,0)	// default Off
     SetAttr("useHpd"  ,0) && SetAttr("activeHpd"  ,0)	// default Off

     SetAttr("useEventFiller"      ,kTRUE);		// default On
     SetAttr("useTracker"          ,kTRUE);		// default On
     SetAttr("useVertexFinder"     ,kTRUE);		// default On
     SetAttr("makePulls"           ,kFALSE);		// default Off

     SetAttr("noTreeSearch",kFALSE);	// treeSearch default ON
     SetAttr("svtSelf",,kFALSE);	// Svt self align default OFF
</ul>
 
<li>On InitRun:				
<ul>
 <li> Build detectors;			
 <li> Init seed finder;			
 <li> Init hit loader;			
 <li> Init tracker;  			
 <li> Init StEvent filler;			 
 <li> Init vertex finder;			 
</ul>
<li>In Make:				
<ul>
 <li> Load hits; 
 <li> Find seeds; 
 <li> Create global tracks; 
 <li> Save tracks into StEvent;
 <li> Find vertecies; 
 <li> Create and assign primaries tracks; 
 <li> Save primary tracks into StEvent;

*/
#include <Stiostream.h>
#include <math.h>
#include <string>
#include "TSystem.h"
#include "TFile.h"
#include "TTree.h"
#include "TTable.h"
#include "TCernLib.h"
#include "StDetectorId.h"
#include "StEvent.h"
#include "StChainOpt.h"
#include "StvMaker.h"
#include "StarVMC/GeoTestMaker/StVMCApplication.h"
#include "StarVMC/GeoTestMaker/StTGeoHelper.h"
//#include "StvMCInitApp.h"
#include "Stv/StvHit.h"
#include "StvUtil/StvPullEvent.h"
#include "Stv/StvDiver.h"
#include "StvHitLoader.h"
#include "Stv/StvToolkit.h"
#include "Stv/StvDefaultSeedFinder.h"
#include "Stv/StvKalmanTrackFinder.h"
#include "StvTGSelectors.h"
#include "StvUtil/StvHitErrCalculator.h"
#include "Stv/StvFitter.h"
#include "StvStEventFiller.h"
#include "StvStarVertexFinder.h"

/// Definion of minimal primary vertex errors.
/// Typical case,vertex got from simulations with zero errors.
/// But zero errors could to unpredicted problems
/// Now minimal possible error is 1 micron
static const float MIN_VTX_ERR2 = 1e-4*1e-4;
ClassImp(StvMaker)
  
//_____________________________________________________________________________
StvMaker::StvMaker(const char *name) : StMaker(name)

{
  memset(mBeg,0,mEnd-mBeg+1);
  cout <<"StvMaker::StvMaker() -I- Starting"<<endl;

  SetAttr("activeTpc"		,kTRUE);
  SetAttr("useEventFiller"      ,kTRUE);
  SetAttr("useTracker"          ,kTRUE);
  SetAttr("useVertexFinder"     ,kTRUE);
  SetAttr("makePulls"           ,kTRUE);
}

//_____________________________________________________________________________
StvMaker::~StvMaker() 
{
  cout <<"StvMaker::~StvMaker() -I- Started/Done"<<endl;
}

//_____________________________________________________________________________
void StvMaker::Clear(const char*)
{
  StvToolkit::Inst()->Clear();
  if (mPullEvent) mPullEvent->Clear();
  StMaker::Clear();
}

//_____________________________________________________________________________
Int_t StvMaker::Finish()
{
  StTGeoHelper::Inst()->Finish();
  return StMaker::Finish();
}

//_____________________________________________________________________________
Int_t StvMaker::Init()
{
  StTGeoHelper::Inst()->Load("y2009a");
  StVMCApplication *app = new StVMCApplication("y2009a", "StVMC application");
  StvMCInitApp *ini = new StvMCInitApp();
  app->SetInit(ini);
  
  app->Init();

  StTGeoHelper::Inst()->SetActive(kTpcId);
  StTGeoHelper::Inst()->Init(1+2+4);

  const char*  innOutNames[2]  ={"StvTpcInnerHitErrs"    ,"StvTpcOuterHitErrs"    };
  for (int io=0;io<2;io++) {
    StvHitErrCalculator *hec = new StvTpcHitErrCalculator(innOutNames[io]);
    TString ts("Calibrations/tracker/");
    ts+=innOutNames[io];
    TTable *tt = (TTable*)GetDataBase(ts);
//    TTable *tt = (TTable*)GetDataBase(innOutNames[io]);
    assert(tt);

    hec->SetPars((double*)tt->GetArray());
    StvTpcSelector        *sel = new StvTpcSelector(innOutNames[io]);
    int nHP = StTGeoHelper::Inst()->SetHitErrCalc(kTpcId,hec,sel);
    Info("Init","%s: %d HitPlanes",innOutNames[io],nHP);
  }

  StvToolkit *kit =StvToolkit::Inst();
  kit->SetHitLoader(new StvHitLoader);
  kit->HitLoader()->Init();
  kit->SetSeedFinder (new StvDefaultSeedFinder);
  kit->SetTrackFinder(new StvKalmanTrackFinder);
  new StvFitter();
  mEventFiller= new StvStEventFiller;
  InitPulls();
  mVertexFinder = new StvStarVertexFinder("GenericVertex");
//  InitDetectors();
  return StMaker::Init();
}

//_____________________________________________________________________________
Int_t StvMaker::InitDetectors()
{
  return kStOk;
}

//_____________________________________________________________________________
Int_t StvMaker::InitRun(int run)
{
static int initialized = 0;
  if (!initialized)
  {
  }
  
  return StMaker::InitRun(run);
}

//_____________________________________________________________________________
Int_t StvMaker::Make()
{
  cout <<"StvMaker::Make() -I- Starting on new event"<<endl;

  StEvent   *event = dynamic_cast<StEvent*>(GetInputDS("StEvent"));

  if (!event) return kStWarn;
  StvToolkit* kit = StvToolkit::Inst();
  kit->HitLoader()->LoadHits(event);
  kit->Reset();
  kit->TrackFinder()->FindTracks();


  if (mEventFiller) {
    mEventFiller->Set(event,&kit->GetTracks());
    mEventFiller->fillEvent();
  }


  if (mVertexFinder) {
     int nVtx = mVertexFinder->Fit(event);
     Info("Make","VertexFinder found %d vertices",nVtx);
     const StvHits &vertexes = mVertexFinder->Result();
     if (vertexes.size()) {
              //Set minimal errors
       for (size_t i=0;i<vertexes.size();i++) {
	 StvHit *vtx=vertexes[i];
         float vtxErr[6];
	 memcpy(vtxErr,vtx->errMtx(),sizeof(vtxErr));
         if (vtxErr[0]>MIN_VTX_ERR2
         &&  vtxErr[2]>MIN_VTX_ERR2
         &&  vtxErr[5]>MIN_VTX_ERR2) continue;
         memset(vtxErr,0,sizeof(vtxErr));
         vtxErr[0]=MIN_VTX_ERR2;
         vtxErr[2]=MIN_VTX_ERR2;
         vtxErr[5]=MIN_VTX_ERR2;
         vtx->setError(vtxErr);
       }
       //cout << "StvMaker::Make() -I- Got Vertex; extend Tracks"<<endl;
////????       kit->TrackFinder()->extendTracksToVertices(vertexes);
  } }

  int iAns=kStOK,iAnz;
  if (mPullTTree) {iAns = FillPulls();}
  cout<< "StvMaker::Make() -I- Done"<<endl;
  iAnz = StMaker::Make();
  kit->Clear();
  if (iAns) return iAns;
  if (iAnz) return iAnz;
  return kStOK;
}
//_____________________________________________________________________________
Int_t StvMaker::InitPulls()
{
  if (!IAttr("makePulls")) 	return 0;
  
  const StChainOpt *bfc = GetChainOpt();
  assert(bfc);
  TFile *tfile  = GetTFile();
  if (!tfile) {
    TString ts  = bfc->GetFileIn();
    ts= gSystem->BaseName(ts);
    int ext = ts.Index(".");
    if (ext>0) ts.Replace(ext,999,"");
    ts +=".stipull.root";
    tfile = mPullFile = new TFile(ts,"RECREATE","TTree Stv Pulls ROOT file");
  }
  tfile->cd();
  mPullTTree = new TTree("StvPulls","TTree Stv pulls");
  mPullTTree->SetAutoSave(100000000);  // autosave when 0.1 Gbyte written
  mPullEvent = new StvPullEvent;
  TBranch *branch = mPullTTree->Branch("event", mPullEvent->ClassName(),&mPullEvent, 16000,99);
  branch->SetAutoDelete(kFALSE);
  mEventFiller->Set(mPullEvent);
  return 0;
}
//_____________________________________________________________________________
Int_t StvMaker::FillPulls()
{
  StEvtHddr   *hddr = GetEvtHddr();
  mPullEvent->mRun  = hddr->GetRunNumber();
  mPullEvent->mEvt  = hddr->GetEventNumber();
  mPullEvent->mDate = hddr->GetDateTime();	//DAQ time (GMT)
  const StvHit *vertex   = 0;
  if (mVertexFinder && mVertexFinder->Result().size()) vertex = mVertexFinder->Result()[0];

  mPullEvent->mChi2 = 0;	
  memset(mPullEvent->mVtx,0,sizeof(mPullEvent->mVtx));
  memset(mPullEvent->mEtx,0,sizeof(mPullEvent->mEtx));
  if (vertex) {
    mPullEvent->mVtx[0] = vertex->x_g()[0];
    mPullEvent->mVtx[1] = vertex->x_g()[1];
    mPullEvent->mVtx[2] = vertex->x_g()[2];
    TCL::ucopy(vertex->errMtx(),mPullEvent->mEtx,6);
  }
  mPullEvent->Finish();
  mPullTTree->Fill();
  mPullEvent->Clear();
  return kStOK;  
}  
