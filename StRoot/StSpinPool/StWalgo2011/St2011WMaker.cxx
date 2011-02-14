// $Id: St2011WMaker.cxx,v 1.2 2011/02/14 02:35:21 stevens4 Exp $
//
//*-- Author : Jan Balewski, MIT
//*-- Author for Endcap: Justin Stevens, IUCF
//*-- Author for JetFinder/JetReader interface: Ilya Selyuzhenkov, IUCF

#include <TH1.h>
#include <TH2.h>
#include <TTree.h>
#include <TString.h>
#include <StMessMgr.h>
#include <StThreeVectorF.hh>


//MuDst
#include <StMuDSTMaker/COMMON/StMuDstMaker.h>
#include <StMuDSTMaker/COMMON/StMuDst.h>
#include <StMuDSTMaker/COMMON/StMuEvent.h>

#include "StEmcUtil/database/StBemcTables.h"
#include "StEmcUtil/geometry/StEmcGeom.h"

#include "StEEmcUtil/database/StEEmcDb.h"       
#include "StEEmcUtil/database/EEmcDbItem.h"     
#include "StEEmcUtil/EEmcGeom/EEmcGeomSimple.h"
#include "StEEmcUtil/StEEmcSmd/EEmcSmdGeom.h"

//jets
#include "StSpinPool/StJets/StJet.h"
#include "StSpinPool/StJets/StJets.h"
#include "StJetMaker/StJetMaker.h"
#include "StSpinPool/StSpinDbMaker/StSpinDbMaker.h"
#include "StJetMaker/StJetReader.h"
#include "StJetMaker/StJetSkimEventMaker.h"

#include "WeventDisplay.h"
#include "St2011WMaker.h"

ClassImp(St2011WMaker)

//_____________________________________________________________________________
//
St2011WMaker::St2011WMaker(const char *name):StMaker(name){
  char muDstMakerName[]="MuDst"; 
  mMuDstMaker=(StMuDstMaker*)GetMaker(muDstMakerName);  

  if(!mMuDstMaker) { //load tree if no MuDst
    mTreeChain=new TChain("mWtree","W candidate events");
    index=0;
  }

  //must have either MuDst or W tree
  assert(mMuDstMaker || mTreeChain);

  mJetReaderMaker = (StJetReader*)GetMaker("JetReader");  
  if(!mJetReaderMaker && mTreeChain) { //jet chain for reading W tree
    mJetTreeChain=new TChain("jet","Jet Tree");
    indexJet=0;
  }
    
  if(!mJetTreeChain && !mJetReaderMaker)
    LOG_WARN<<GetName()<<Form("::constructor() NO JETS , W-algo is not working properly, continue")<<endm;
  
  // preset or clear some params
  par_l2bwTrgID=parE_l2ewTrgID=0;
 
  setHList(0);
  setMC(0);
  nInpEve= nTrigEve= nAccEve=0; 

  //... MC trigger simulator
  par_l0emulAdcThresh=30;
  par_l2emulSeedThresh=5.0;
  par_l2emulClusterThresh=13.0;

  //.. vertex
  par_minPileupVert=3; // to reject events w/o TPC, lower it for MC
  par_vertexZ=100; // (cm)

  //... towers
  par_kSigPed=3; // rawADC-ped cut off
  par_AdcThres=8; // ADC threshold to avoid correlated noise
  par_maxADC=200.; // (adc chan) on the highest tower in events :tmp was 200
    
  //... Barrel Algo
  par_clustET=15.; // (GeV/c) 2x2 cluster ET :tmp was 15.0 GeV
  par_clustFrac24=0.95; // ET ratio 2x2/4x4 cluster
  par_nearTotEtFrac=0.88;  // ratio 2x2/near Tot ET 
  par_delR3D=7.; // cm, dist between projected track and center of cluster 
  par_leptonEta=1.0; // bracket acceptance
  par_ptBalance=15.; // (GeV), ele cluster vector + jet sum vector
  //... track
  par_nFitPts=15; // hits on the track
  par_nHitFrac=0.51;
  par_trackRin=90;  par_trackRout=160; // cm
  par_trackPt=10.;//GeV :tmp, was 10.0

  //... Endcap Algo
  parE_trackEtaMin=0.7; // avoid bad extrapolation to ESMD
  parE_clustET=15.; // (GeV/c) 2x1 cluster ET  :tmp was 15.0 GeV
  parE_clustFrac24=0.90; // ET ratio 2x2/4x4 cluster
  parE_nearTotEtFrac=0.85;  // ratio 2x2/near Tot ET 
  parE_delR3D=10.; // cm, dist between projected track and center of cluster 
  parE_leptonEtaLow=1.1; // bracket acceptance
  parE_leptonEtaHigh=2.0; // bracket acceptance
  parE_ptBalance=15.; // (GeV), ele cluster vector + jet sum vector
  //... track
  parE_nFitPts=5; // hits on the track
  parE_nHitFrac=0.51;
  parE_trackRin=120;  parE_trackRout=70; // cm
  parE_trackPt=10.;//GeV :tmp, was 10.0
  parE_nSmdStrip=20;

  //... search for W's
  par_nearDeltaR=0.7; //(~rad) near-cone size
  par_awayDeltaPhi=0.7; // (rad) away-'cone' size
  par_highET=28.; // (GeV), cut-off for final W-cluster ET
  
  setEtowScale(1.0); // for old the Endcap geometr you need ~1.3
  setBtowScale(1.0);
  
  mRunNo=0;

  // irrelevant for W analysis
  par_DsmThres=31; // only for monitoring
  parE_DsmThres=31; // only for monitoring
  par_maxDisplEve=1; // # of displayed selected events

  use_gains_file = 0;

}


//_____________________________________________________________________________
//
Int_t 
St2011WMaker::Init(){
  assert(HList);
  initHistos();
  initEHistos();

  if(mMuDstMaker) { 
    //only need DB tables for MuDst analysis
    mBarrelTables = new StBemcTables();
    mDbE = (StEEmcDb*)GetDataSet("StEEmcDb"); 
    assert(mDbE);
    
    if (use_gains_file == 1) {
      fstream f1; f1.open(gains_file,ios::in);
      cout << "Opening gains file " << gains_file << endl;
      char str[200];
      while (f1 >> str) {
	int softID = atoi(str);
	f1 >> str; gains_BTOW[softID] = atof(str);
	f1 >> str; f1 >> str;
      }
    }
    
    //tree only written during MuDst analysis
    mTreeFile=new TFile(mTreeName,"recreate");
    mTreeFile->cd();
    
    wEve=new Wevent2011();
    mWtree=new TTree("mWtree","W candidate Events");
    mWtree->Branch("wEve","Wevent2011",&wEve);
  }
  else { 
    //setup for reading in tree
    wEve=new Wevent2011();
    mTreeChain-> SetBranchAddress("wEve",&wEve);
  }

  mBtowGeom = StEmcGeom::instance("bemc");
  mBSmdGeom[kBSE] = StEmcGeom::instance("bsmde");
  mBSmdGeom[kBSP] = StEmcGeom::instance("bsmdp");        
  geomE= new EEmcGeomSimple();         
  geoSmd= EEmcSmdGeom::instance();
  initGeom();
 
  wDisaply= new WeventDisplay(this,par_maxDisplEve);

  if(isMC) par_minPileupVert=1;

  // ..... initialization of TPC cuts is run dependent, call it 'hack of the day', should be moved to InitRun() and handle multipl runs per job, after APS, JB
  for(int isec=0;isec<mxTpcSec;isec++) {
    int sec=isec+1;
    float Rin=par_trackRin,Rout=par_trackRout;
    float RinE=parE_trackRin,RoutE=parE_trackRout;
    //.... Rin ..... changes
    
    //.... Rout ..... changes
    
    mTpcFilter[isec].setCuts(par_nFitPts,par_nHitFrac,Rin,Rout);
    mTpcFilter[isec].init("sec",sec,HList);
    mTpcFilterE[isec].setCuts(parE_nFitPts,parE_nHitFrac,RinE,RoutE);
    mTpcFilterE[isec].init("secEemcTr",sec,HList);
  }

  return StMaker::Init();
}

//________________________________________________
//________________________________________________
Int_t
St2011WMaker::InitRun(int runNo){
  LOG_INFO<<Form("::InitRun(%d) start",runNo)<<endm;
  assert(mRunNo==0);  // to prevent run merging - it was not tested 
  if(mMuDstMaker) {
    mBarrelTables->loadTables(this );
    mRunNo=runNo;
  }
  else {
    mRunNo=wEve->runNo; // why do we have this line, Jan?
  }

  //barrel algo params
  LOG_INFO<<Form("::InitRun(%d) done\n Barrel W-algo params: trigID L2BW=%d isMC=%d\n TPC: nPileupVert>%d, vertex |Z|<%.1fcm, primEleTrack: nFit>%d, hitFrac>%.2f Rin<%.1fcm, Rout>%.1fcm, PT>%.1fGeV/c\n BTOW ADC: kSigPed=%d AdcThr>%d maxAdc>%.0f clustET>%.1f GeV  ET2x2/ET4x4>%0.2f  ET2x2/nearTotET>%0.2f\n dist(track-clust)<%.1fcm, nearDelR<%.1f\n W selection highET>%.1f awayDelPhi<%.1frad  ptBalance>%.1fGeV  |leptonEta|<%.1f",
		 mRunNo, par_l2bwTrgID,isMC,
		 par_minPileupVert,par_vertexZ,
		 par_nFitPts,par_nHitFrac,  par_trackRin,  par_trackRout, par_trackPt,
		 par_kSigPed, par_AdcThres,par_maxADC,par_clustET,par_clustFrac24,par_nearTotEtFrac,
		 par_delR3D,par_nearDeltaR,
		 par_highET,par_awayDeltaPhi,par_ptBalance,par_leptonEta
		 )<<endm;
  //endcap algo params
  cout<<Form("\n Endcap W-algo params: trigID: L2EW=%d isMC=%d\n TPC: nPileupVert>%d, vertex |Z|<%.1fcm, primEleTrack: nFit>%d, hitFrac>%.2f Rin<%.1fcm, Rout>%.1fcm, PT>%.1fGeV/c\n ETOW ADC: kSigPed=%d AdcThr>%d maxAdc>%.0f clustET>%.1f GeV  ET2x1/ET4x4>%0.2f  ET2x1/nearTotET>%0.2f\n dist(track-clust)<%.1fcm, nearDelR<%.1f\n W selection highET>%.1f awayDelPhi<%.1frad  ptBalance>%.1fGeV ",
		 parE_l2ewTrgID,isMC,
		 par_minPileupVert,par_vertexZ,
		 parE_nFitPts,parE_nHitFrac,parE_trackRin,parE_trackRout,parE_trackPt,
		 par_kSigPed,par_AdcThres,par_maxADC,parE_clustET,parE_clustFrac24,parE_nearTotEtFrac,
		 parE_delR3D,par_nearDeltaR,
		 par_highET,par_awayDeltaPhi,parE_ptBalance
		 )<<endl;
  cout<<Form("\n EtowScaleFact=%.2f  BtowScaleFacor=%.2f" ,par_etowScale, par_btowScale)<<endl;

   return kStOK;
}

//________________________________________________
//________________________________________________
Int_t
St2011WMaker::Finish(){
  if(mMuDstMaker){
    LOG_INFO<<endl<<"Output tree file "<<mTreeName<<endl<<endl;
    mTreeFile->Write();
    mTreeFile->Close();
  }
  return StMaker::Finish();
}

//________________________________________________
//________________________________________________
Int_t
St2011WMaker::FinishRun(int runNo){
  LOG_INFO<<Form("::FinishRun(%d)",runNo)<<endm;
return kStOK;
}

//________________________________________________
//________________________________________________
void
St2011WMaker::Clear(const Option_t*){
  wEve->clear();
}

//--------------------------------------
//--------------------------------------
Int_t 
St2011WMaker::Make(){
  nInpEve++; 
  if(mMuDstMaker){ //standard MuDst analysis
    wEve->id=mMuDstMaker->muDst()->event()->eventId();
    wEve->runNo=mMuDstMaker->muDst()->event()->runId();
    const char *afile = mMuDstMaker->GetFile();
    //printf("inpEve=%d eveID=%d daqFile=%s\n",nInpEve, wEve->id,afile);
    if(nInpEve%200==1) printf("\n-----in---- %s, nEve: inp=%d trig=%d accpt=%d daqFile=%s\n", GetName(),nInpEve,nTrigEve, nAccEve,afile);

    hA[0]->Fill("inp",1.); 
    hE[0]->Fill("inp",1.); 
    
    int btowStat=accessBTOW(); // get energy in BTOW
    int etowStat=accessETOW(); // get energy in ETOW
   
    int btrig=accessBarrelTrig();
    int etrig=accessEndcapTrig();

    if( btrig && etrig ) return kStOK; //skip event w/o valid trig ID
 
    nTrigEve++; 
    
    if( accessVertex()) return kStOK; //skip event w/o ~any reasonable vertex  
    
    if( accessTracks()) return kStOK; //skip event w/o ~any highPt track

    accessBSMD();// get energy in BSMD
    accessESMD();// get energy in ESMD
    accessEPRS();// get energy in EPRS

    mWtree->Fill(); //write all events w/ pt>10 track to tree
    
    if(wEve->l2bitET && wEve->bemc.tileIn[0]==1) hA[0]->Fill("B-in",1.0);
    if(wEve->l2EbitET && wEve->etow.etowIn==1)    hE[0]->Fill("E-in",1.0);
    if(wEve->l2bitET && !btowStat) hA[0]->Fill("B200",1.0);
    if(wEve->l2EbitET && !etowStat) hE[0]->Fill("E200",1.0);

    if( btowStat && etowStat ) return kStOK; //skip event w/o energy in BTOW && ETOW   
    
    if(mJetReaderMaker) {// just QA plots for jets
      mJets = getJets(mJetTreeBranch); //get input jet info
      for (int i_jet=0; i_jet< nJets; ++i_jet){
	StJet* jet = getJet(i_jet);
	float jet_pt = jet->Pt();
	float jet_eta = jet->Eta();
	float jet_phi = jet->Phi();
	hA[117]->Fill(jet_eta,jet_phi);
	hA[118]->Fill(jet_pt);
      }
    }
  }
  else { //analysis of W tree
    if(getEvent(index++,indexJet++)==kStEOF) return kStEOF; //get next event from W and jet tree
    if(mJetTreeChain) {// just QA plots for jets
      mJets = getJetsTreeAnalysis(mJetTreeBranch); //get input jet info
      for (int i_jet=0; i_jet< nJets; ++i_jet){
	StJet* jet = getJet(i_jet);
	float jet_pt = jet->Pt();
	float jet_eta = jet->Eta();
	float jet_phi = jet->Phi();
	hA[117]->Fill(jet_eta,jet_phi);
	hA[118]->Fill(jet_pt);
      }
    }
  }

  //find barrel candidates
  extendTrack2Barrel(); 
  int bmatch=matchTrack2BtowCluster(); 

  //find endcap candidates
  extendTrack2Endcap();
  int ematch=matchTrack2EtowCluster();

  if(bmatch && ematch) return kStOK; //no matched BTOW or ETOW clusters

  nAccEve++;
 
  /* now it starts to get interesting, process every track 
     on the list  till the end. */
  findNearJet();
  findAwayJet();

  if(mJetReaderMaker || mJetTreeChain) {
    findPtBalance();
    tag_Z_boson();
  }

  //endcap specific analysis
  analyzeESMD();
  
  find_W_boson();
  findEndcap_W_boson();
  if(nAccEve<2 ||nAccEve%1000==1 ) wEve->print(0x0,isMC);
  
  return kStOK;
}


//--------------------------------------
//--------------------------------------
void 
St2011WMaker::initGeom(){

  //...... BTOW ...........
  memset(mapBtowIJ2ID,0,sizeof(mapBtowIJ2ID));
  for(int softID=1; softID<=mxBtow; softID++) {    
    //........... querry BTOW geom
    int m,e,s;
    mBtowGeom->getBin(softID,m,e,s);    
    float etaF,phiF;
    mBtowGeom->getEta(m,e,etaF);
    mBtowGeom->getPhi(m,s,phiF);  // -pi <= phi < pi
    
    int iEta, iPhi;
    assert(L2algoEtaPhi2IJ(etaF,phiF,iEta, iPhi)==0); // tower must be localized at the known position
    int IJ=iEta+ iPhi*mxBTetaBin;
    assert(mapBtowIJ2ID[IJ ]==0); // avoid overlaping mapping
    mapBtowIJ2ID[IJ ]=softID;
    
    Float_t x,y,z;
    assert( mBtowGeom->getXYZ(softID,x,y,z)==0);
    positionBtow[softID-1]=TVector3(x,y,z);
  }// end of loop over towers

  //...... BSMD-E, -P ...........
  for(int iep=0;iep<mxBSmd;iep++) {
    for(int softID=1; softID<=mxBStrips; softID++) {    
      Float_t x,y,z;
      assert( mBSmdGeom[iep]->getXYZ(softID,x,y,z)==0);
      positionBsmd[iep][softID-1]=TVector3(x,y,z);
    }// end of loop over towers
  }

  //...... ETOW .............  
  for(int isec=0;isec<mxEtowSec;isec++){
    for(int isub=0;isub<mxEtowSub;isub++){
      for(int ieta=0;ieta<mxEtowEta;ieta++){
	positionEtow[isec*mxEtowSub+isub][ieta]=geomE->getTowerCenter(isec,isub,ieta);
      }
    }
  }
  
}


//--------------------------------------
//--------------------------------------
int // returns error code
St2011WMaker::L2algoEtaPhi2IJ(float etaF,float phiF,int &iEta, int &iPhi) {
  if( phiF<0) phiF+=2*C_PI; // I want phi in [0,2Pi]
  if(fabs(etaF)>=0.99) return -1;
  int kEta=1+(int)((etaF+1.)/0.05);
  iPhi=24-(int)( phiF/C_PI*60.);
  if(iPhi<0) iPhi+=120;
  // convention:  iPhi=[0,119], kEta=[1,40]  
  iEta=kEta-1;
  //printf("IJ=%d %d\n",iEta,iPhi);
  if(iEta<0 || iEta>=mxBTetaBin) return -2;
  if(iPhi<0 || iPhi>=mxBTphiBin) return -3;
  return 0;
}


//________________________________________________
//________________________________________________
TClonesArray*
St2011WMaker::getJets(TString branchName){
  if(mJetReaderMaker ==0) {
    nJets=-1; return 0;
  }
  assert(mJetReaderMaker->getStJets(branchName)->eventId()==wEve->id);
  assert(mJetReaderMaker->getStJets(branchName)->runId()==wEve->runNo);
  nJets = mJetReaderMaker->getStJets(branchName)->nJets();
  return mJetReaderMaker->getStJets(branchName)->jets();

}

// Below is only used for Tree analysis
// ----------------------------------------------------------------------------
TClonesArray*
St2011WMaker::getJetsTreeAnalysis(TString branchName){
  if(mJetTreeChain==0){
    nJets=-1; return 0;
  }

  //cout<<"looking for matching jet event"<<endl;

  StJets* jetTmp=getStJetsCopy(branchName);
  while(jetTmp->eventId()!=wEve->id || jetTmp->runId()!=wEve->runNo) {
    mJetTreeChain->GetEntry(indexJet++);
    jetTmp=getStJetsCopy(branchName);
  }

  //cout<<"found matching jet event"<<endl;

  assert(jetTmp->eventId()==wEve->id);
  assert(jetTmp->runId()==wEve->runNo);
  nJets = jetTmp->nJets();
  return jetTmp->jets();
}

// ----------------------------------------------------------------------------
StJets* 
St2011WMaker::getStJetsCopy(TString branchName){
  TBranch* branch = mJetTreeChain->GetBranch(branchName);
  return branch ? *(StJets**)branch->GetAddress() : 0;
}

// ----------------------------------------------------------------------------
Int_t St2011WMaker::getEvent(Int_t i, Int_t ijet)
{
  Int_t stat=mTreeChain->GetEntry(i);
  Int_t statJet=mJetTreeChain->GetEntry(ijet);
  if (!stat && !statJet) return kStEOF;
  return kStOK;
}

// ----------------------------------------------------------------------------
void St2011WMaker::chainFile( const Char_t *file )
{

  TString fname=file;
  cout<<"Chain W tree files"<<endl;
  if ( !fname.Contains("tree.root") ) return;
  cout << "+ " << fname << endl;
  mTreeChain->Add(fname);
}

// ----------------------------------------------------------------------------
void St2011WMaker::chainJetFile( const Char_t *file )
{

  TString fname=file;
  cout<<"Chain jet tree files"<<endl;
  if ( !fname.Contains("jets_") ) return;
  cout << "+ " << fname << endl;
  mJetTreeChain->Add(fname);
}

// $Log: St2011WMaker.cxx,v $
// Revision 1.2  2011/02/14 02:35:21  stevens4
// change back to 10 GeV track pt and 15 GeV cluster ET as default for both B + E algos
//
// Revision 1.1  2011/02/10 20:33:22  balewski
// start
//
