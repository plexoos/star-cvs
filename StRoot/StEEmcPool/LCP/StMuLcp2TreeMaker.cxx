// *-- Author : Jan Balewski
// 
// $Id: StMuLcp2TreeMaker.cxx,v 1.1 2003/09/16 19:18:35 balewski Exp $

#include <TFile.h>
#include <TH2.h>

#include "StMuLcp2TreeMaker.h"


#include "TChain.h"
#include "TClonesArray.h"
#include "StL0Trigger.h"
#include "StEventInfo.h"
#include "StEventSummary.h"
#include "StarClassLibrary/StThreeVectorF.hh"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "MikesRejector/ExampleUsage.h"

#include "StThreeVector.hh"


ClassImp(StMuLcp2TreeMaker)


//________________________________________________
//________________________________________________
StMuLcp2TreeMaker::StMuLcp2TreeMaker(const char* self ,const char* muDstMakerName) : StMaker(self){
  mMuDstMaker = (StMuDstMaker*)GetMaker(muDstMakerName);
  assert(mMuDstMaker);

  //rejector=new  ExampleUsage; // Mike's cosmic/laser rejector
  primTrA=0;

  runID=9999;// default not initialized value
  treeName="./";
  eve_cosm=-8;
  SetMinNFitPoint(15); 
  SetMaxDCAxy(3.);// cm
  SetMaxEta(1.4);
  SetMinPt(0.2); // GeV/c
  SetMinFitPfrac(0.55); // (%)
}


//________________________________________________
//________________________________________________
StMuLcp2TreeMaker::~StMuLcp2TreeMaker(){
if(tree) tree->Print();
 
 // Save all objects in this file
 hfile->Write();
 
}


//________________________________________________
//________________________________________________
Int_t StMuLcp2TreeMaker::InitRunFromMake  (int runNumber){
  float Pi=3.14159;
  if(runID==runNumber)  return kStOK;
  
  if(runID !=9999) {
    printf("\n\n FATAL %s::InitRunFromMake() runID=%d changed to %d, (not implemented),STOP \n",GetName(),runID,runNumber);
    assert(runID==runNumber);
  }
  
  runID=runNumber;
  
  treeName+="/R";
  treeName+=runID;
  treeName+=".tree.root";
  printf("%s::InitRunFromMake() \n   runID=%d Setup output tree & histo to '%s' , use BXoff48=%d , CUTS: minNFitPoint=%d maxDCAxy/cm=%.2f maxEta=%.2f minPt/GeV/c=%.2f, minFitPfrac=%.2f\n",GetName(),runID,treeName.Data(),off48,C_minNFitPoint,C_maxDCAxy,C_maxEta,C_minPt,C_minFitPfrac);
  
  hfile=new TFile(treeName,"RECREATE"," histograms & trees with LCP ");
  assert(hfile->IsOpen());
  
  // Create some histograms and a profile histogram
  h[0]=new TH1F("bX7","rates vs. 7bit bXing",129,-0.5,128.5);
  h[1]=new TH1F("bX48","rates vs. 48bit bXing +offset%120 ",129,-0.5,128.5);
  h[2]=new TH1F("bX120","rates vs. true bXing [0-119]",129,-0.5,128.5);
  h[3]=(TH1F *)new TH2F("sBit120","spinBits vs.  bXing120",129,-0.5,128.5,17,-0.5,16.5);
  h[4]   = new TH1F("Vz","Vertex Z/cm",100,-250,250.);
  h[5]   = new TH1F("nPrim","No. of prim TPC tracks",50,-0.5,49.5);
  h[6]   =new TH1F("eta","eta of LCP",100,-2.,2.);

  h[7]   = new TH1F("phi"," phi/rad  of LCP", 240,-Pi,Pi);
  h[8]   = new TH1F("pT","pT/GeV of LCP",100,0,10.);
  

  if (runNumber>10000) {   
    // Create tree
    tree = new TTree("T-LCP","LCP selected from MinB trigger");
    tree->Branch("bx48", &eve_bx48,"bx48/I");
    tree->Branch("bx120",&eve_bx120,"bx120/I");
    tree->Branch("id",   &eve_id,"id/I");
    tree->Branch("sb",   &eve_sb,"sb/I");
    tree->Branch("nPrim",&eve_nPrim,"nPrim/I");
    tree->Branch("vz",   &eve_vz ,"vz/F");
    tree->Branch("cosm",   &eve_cosm ,"cosm/F");
    
    tree->Branch("pt",&lcp_pt ,"pt/F");
    tree->Branch("phi",&lcp_phi ,"phi/F");
    tree->Branch("eta",&lcp_eta ,"eta/F");
    tree->Branch("q",&lcp_q ,"q/I");
    tree->Branch("nFit",&lcp_nFit ,"nFit/I");
  } else {
    printf(" %s::InitRunFromMake(RunNumber=%d) WARN : TTRee NOT created\n",GetName(),runNumber);
    tree=0;
  }

#if 0
  // cuts study
  int nd=0;  float x1=1,x2=2; TString titCore="fixMe2";
  
  //  titCore="nFitPoint"; nd=15; x1=9.5; x2=39.5;
  //  titCore="maxDCAxy/cm"; nd=15; x1=0.; x2=3.0;
  // titCore="maxEta"; nd=15; x1=0.35; x2=1.85;
  titCore="minFitPfrac"; nd=10; x1=0.325; x2=0.825;
  
  hc[0]=new TH1F("CnFP-A","Yield vs."+titCore+", for LCP #Delta#phi <1/8 #pi ",nd,x1,x2);
  hc[1]=new TH1F("CnFP-B","Yield vs."+titCore+", for  LCP #Delta#phi #in [ 1/8 #pi, 3/8 #pi ] ",nd,x1,x2);
  hc[2]=new TH1F("CnFP-C","Yield vs."+titCore+", for LCP #Delta#phi #in [ 3/8 #pi, 5/8 #pi ] ",nd,x1,x2);
  hc[3]=new TH1F("CnFP-D","Yield vs. "+titCore+", for LCP #Delta#phi #in [ 5/8 #pi, 7/8 #pi ] ",nd,x1,x2);
  hc[4]=new TH1F("CnFP-E","Yield vs."+titCore+", for  LCP #Delta#phi > 7/8 #pi ",nd,x1,x2);
  hc[5]=new TH1F("CnFP-L","Yield vs."+titCore+", lost LCP",nd,x1,x2);
  hc[6]=new TH1F("CnFP-W","Yield vs."+titCore+", won LCP",nd,x1,x2);
#endif
  

  return kStOK;
}
 
//________________________________________________
//________________________________________________
Int_t StMuLcp2TreeMaker::Init(){
  return StMaker::Init();
}
 

//________________________________________________
//________________________________________________
void StMuLcp2TreeMaker::clearLCP(){
  lcp_eta=lcp_phi=lcp_pt=0;
  lcp_q=0;
  lcp_nFit=0;
}
 

//________________________________________________
//________________________________________________
Int_t StMuLcp2TreeMaker::Make(){
  
  static int kEve=0;
  kEve++;
  
  //  printf("%s::Make() is called ..........\n",GetName());
  
  StMuDst* dst = mMuDstMaker->muDst();
  primTrA=dst->primaryTracks();
    
  StMuEvent* muEve = dst->event();
  StL0Trigger &trig=muEve->l0Trigger();
  StEventInfo &info=muEve->eventInfo();
  StEventSummary &smry=muEve->eventSummary();
  StThreeVectorF ver=smry.primaryVertexPosition();
  if(runID>1000) InitRunFromMake(info.runId());
  
  // use only minB trigger events 
  if(trig.triggerWord()!=0x2000)  return kStOK;
  
  // reject events without primtracks
  if(primTrA->GetEntries()<=0)  return kStOK;
  
  int bx7=trig.bunchCrossingId7bit(info.runId());
  
  // fill eve-related tree values
  eve_id=info.id();
  eve_bx48 = trig.bunchCrossingId();
  eve_bx120 =(trig.bunchCrossingId()+off48 )%120;
  eve_sb=trig.spinBits();
  eve_vz=ver.z();
  
  // test for false vertex
  //eve_cosm=rejector->acceptEvent(dst);
  
  // search for LCP  
  clearLCP();
  StMuTrack* lcp= findLCP(C_minPt,C_minNFitPoint, C_maxDCAxy, C_maxEta,C_minFitPfrac);
  
  // fill histo 1:1
  h[0]->Fill(bx7);
  h[1]->Fill(eve_bx48);
  h[2]->Fill(eve_bx120);   
  ((TH2F*)h[3])->Fill(eve_bx120,eve_sb);
  h[4]->Fill(eve_vz);
  h[5]->Fill(eve_nPrim); // <<-- assigned by findLCP()
  
  if(lcp) { // LCP was found
    lcp_eta=lcp->eta();
    lcp_phi=lcp->phi();
    lcp_pt= lcp->pt();
    lcp_q = lcp->charge();
    lcp_nFit=lcp->nHitsFit();     
    
    // fill those histo only for LCP
    h[6]->Fill(lcp_eta);
    h[7]->Fill(lcp_phi);
    h[8]->Fill(lcp_pt);
  }
  
   if(tree)tree->Fill();
   if(kEve%500==0)printf("%s::Make(%d) nPrim=%d lcp: pt=%f nFit=%d TTRee=%p\n",GetName(),kEve,eve_nPrim,lcp_pt,lcp_nFit,tree);
   
   // extra tests of cuts
   //   examinCut(lcp);
   
   return kStOK;
}

//________________________________________________
//________________________________________________
StMuTrack* StMuLcp2TreeMaker::findLCP( float XminPt,int XminNFitP , float XmaxDCAxy, float XmaxEta, float XminFitPfrac) {
  
  StMuTrack* lp=0;  
  int nTr=primTrA->GetEntries();
  int nTr0=0,nTr1=0; // counts valid prim TPC tracks
  float maxPt=XminPt;// init pT  
  for (int i=0; i<nTr; i++) {
    StMuTrack* pTr = (StMuTrack*)primTrA->UncheckedAt(i);
    if(pTr->flag() <=0) continue;
    if(!pTr->topologyMap().trackTpcOnly()) continue;
    nTr0++;
    // printf("itr=%d pT=%f nFit=%d\n",i, pTr->pt(),pTr->nHitsFit());
    
    StThreeVectorF dca=pTr->dcaGlobal();
    //printf("xyz=%f %f %f, pt=%f\n",dca.x(),dca.y(),dca.z(),dca.perp());
    if(dca.perp()>XmaxDCAxy) continue;

    // switch to oryginal glob-track, since vertex constrain biases pt 
    StMuTrack* gTr=pTr->globalTrack();
    assert(gTr);// every prim must have its global
        
    if ( gTr->nHitsFit() < XminNFitP) continue;
    if ( fabs(gTr->eta()) > XmaxEta ) continue;
    float frac=1.*gTr->nHitsFit()/gTr->nHitsPoss();
    if ( frac < XminFitPfrac) continue;
    nTr1++;

    // now search for high pT
    if ( gTr->pt() < maxPt ) continue;
    maxPt=gTr->pt();
    lp=gTr;
  }
  // tmp - apply pT cut on LCP
  //if(maxPt<1.5 || maxPt>3) lp=0;

  eve_nPrim=nTr0;
  //  printf("nTr0=%d nTr1=%d\n",nTr0,nTr1);
  return lp;
}


//________________________________________________
//________________________________________________
void StMuLcp2TreeMaker::examinCut(StMuTrack*lcp0){

  const float Pi=3.1416;
  
  StThreeVectorF p0;
  float phi0=0;

  if(lcp0) {
    p0=lcp0->p();
    phi0=Pi/2+ atan2(p0.y(),p0.x());
  }
 
  int nb=hc[0]->GetNbinsX();
 
  int i;
  for(i=1;i<=nb;i++) {// vary nFitP ......................
    float x=hc[0]->GetBinCenter(i);
    StMuTrack* lcp1=0;

    // lcp1=findLCP(C_minPt,(int)x,C_maxDCAxy,C_maxEta,C_minFitPfrac);   // test nFitPoint
    //lcp1=findLCP(C_minPt,C_minNFitPoint,x,C_maxEta,C_minFitPfrac);       // test maxDCAxy
    // lcp1=findLCP(C_minPt,C_minNFitPoint,C_maxDCAxy,x,C_minFitPfrac);   // test maxEta
    lcp1=findLCP(C_minPt,C_minNFitPoint,C_maxDCAxy,C_maxEta,x);     // test fitPfrac
    //printf("i=%d x=%f %p %p\n",i,x,lcp0,lcp1);
 
    float delPhi=Pi*1.1;
    if(lcp0 && lcp1==0) {    // lost LCP
      hc[5]->Fill(x);
    } else  if(lcp1 && lcp0==0) { // won LCP
      hc[6]->Fill(x);
    } else if(lcp1 && lcp0) {  // phi could change
      StThreeVectorF p1=lcp1->p();
      float phi1=Pi/2+ atan2(p1.y(),p1.x());
      delPhi=phi1-phi0;
      if(delPhi <0) delPhi*=-1;
      if(delPhi >Pi) delPhi=2*Pi-delPhi;
      // printf("(deg) phi0=%.1f, phi1=%.1f, del=%.1f\n",phi0/Pi*180, phi1/Pi*180, delPhi/Pi*180);
      if( delPhi <1./8*Pi) {
	hc[0]->Fill(x);
      } else if( delPhi <3./8*Pi) {
	hc[1]->Fill(x);
      } else if( delPhi <5./8*Pi) {
	hc[2]->Fill(x);
      } else if( delPhi <7./8*Pi) {
	hc[3]->Fill(x);
      } else {
	hc[4]->Fill(x);
      }
    }
  }
}




// $Log: StMuLcp2TreeMaker.cxx,v $
// Revision 1.1  2003/09/16 19:18:35  balewski
// extraction of LCP from muDst
//
