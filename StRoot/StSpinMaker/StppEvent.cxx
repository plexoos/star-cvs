//////////////////////////////////////////////////////////////////////
//
// $Id: StppEvent.cxx,v 1.1 2002/01/16 20:22:53 akio Exp $
// $Log: StppEvent.cxx,v $
// Revision 1.1  2002/01/16 20:22:53  akio
// First version
//
//
// Revision 1.0  2001/06/14 Akio Ogawa
//
//////////////////////////////////////////////////////////////////////
#include <iostream.h>
//#include "Rtypes.h"
#include "StEventTypes.h"
#include "StEvent.h"
#include "StTriggerDetectorCollection.h"
#include "StCtbTriggerDetector.h"
#include "StppTrack.h"
#include "StppEvent.h"

ClassImp(StppEvent);

StppEvent::StppEvent(){
#ifdef _Offline_tracks_
  pTracks = new TClonesArray ("StppTrack",200);  //warning hardcoded # of track limits!
#endif
#ifdef _L3_tracks_
  pTracksL3 = new TClonesArray ("StppTrack",200);  //warning hardcoded # of track limits!
#endif 
  infoLevel = 0;
  clear();
}

StppEvent::~StppEvent() {
  clear() ;
#ifdef _Offline_tracks_
  delete pTracks ;
#endif
#ifdef _L3_tracks_
  delete pTracksL3;
#endif 
} 

void StppEvent::clear(){
  runN = 0;
  eventN = 0;
  token = 0;
  triggerWord = 0;
  time = 0;
  bunchId = 0;
  bunchId7bit = 0;
  doubleSpinIndex = 0;

#ifdef _Offline_tracks_
  pTracks->Clear();
  nPrimTrack = 0;
  nGoodTrack = 0;
  xVertex = 0.0;
  yVertex = 0.0;
  zVertex = 0.0;
  LCP = 0;
  sumPt = 0.0;
  vectorSumPt = 0.0;
  weightedEta = 0.0;
  weightedPhi = 0.0;
#endif

#ifdef _L3_tracks_
  pTracksL3->Clear();
  nPrimTrackL3 = 0;
  nGoodTrackL3 = 0;
  xVertexL3 = 0.0;
  yVertexL3 = 0.0;
  zVertexL3 = 0.0;
  LCPL3 = 0;
  sumPtL3=0.0;
  vectorSumPtL3=0.0;
  weightedEtaL3=0.0;
  weightedPhiL3=0.0;
#endif

  bbcAdcSum = 0;
  bbcNHit=0;
  zVertexBbc=0.0;

  fpdAdcSumNorth  = 0;
  fpdAdcSumSouth  = 0;
  fpdAdcSumTop    = 0;
  fpdAdcSumBottom = 0;
  fpdAdcSumPres1  = 0;
  fpdAdcSumPres2  = 0;
  fpdAdcSumSmdX   = 0;
  fpdAdcSumSmdY   = 0;
  fpdSouthVeto    = 0;

  ctbAdcSum = 0.0;
  ctbNHit=0;
  svtNHit=0;
  emcHighTower = 0.0;  
}

#ifndef __CINT__
Int_t StppEvent::fill(StEvent *event){
  eventN = event->id() ;
  runN   = event->runId(); 
  time   = event->time(); 
  
#ifdef _Offline_tracks_
  // Get primary tracks
  nPrimTrack = 0; 
  nGoodTrack = 0; 
  StSPtrVecTrackNode& exnode = event->trackNodes();
  Int_t nnode=exnode.size();
  float maxpt = 0.0;
  float sumPx=0.0;
  float sumPy=0.0;
  sumPt=0.0;
  vectorSumPt=0.0;
  weightedEta = 0.0;
  weightedPhi = 0.0;
  for( Int_t in=0; in<nnode; in++ ) {
    UInt_t nprim = exnode[in]->entries(primary);
    if(nprim==1){
      StppTrack *t = new((*pTracks)[nPrimTrack]) StppTrack(exnode[in]->track(primary));
      if(t->flag>0 && t->nHits>20 && t->pt>0.2 && fabs(t->eta)<1.4){
	if(t->pt > maxpt){
	  maxpt = t->pt;
	  LCP = nPrimTrack;
	}
	sumPt+=t->pt;
	sumPx+=t->pt * cos(t->phi0);
	sumPt+=t->pt * sin(t->phi0);
	weightedEta += t->pt * t->eta;
	nGoodTrack++;
      }
      nPrimTrack++;
    }
  }
  if(sumPt>0.0){
    vectorSumPt=sqrt(sumPx*sumPx+sumPy*sumPy);
    weightedPhi=(float)atan2((double)sumPy,(double)sumPx);
    weightedEta/=sumPt; 
  }
  //create at least 1 track even if there is none
  if(nPrimTrack==0) StppTrack *t = new((*pTracks)[nPrimTrack]) StppTrack();
  if (infoLevel > 1){
    cout << "StppEvent : number of primary tracks " << nPrimTrack << endl;
  }

  // Get primary vertex
  StPrimaryVertex* vtx = event->primaryVertex();
  if(vtx) {
    xVertex = vtx->position().x();
    yVertex = vtx->position().y();
    zVertex = vtx->position().z();
    if (infoLevel > 1){
     cout << "StppEvent : primary vertex " 
	  << xVertex << " "  << yVertex << " " << zVertex << endl;
    }
  }
  else {
    xVertex = -9999. ;
    yVertex = -9999. ;
    zVertex = -9999. ;
    if (infoLevel > 1){
      cout<<"StppEvent: There was no primary vertex"<<endl;
    } 
  }
#endif

#ifdef _L3_tracks_
  // Get tracks from L3
  StL3Trigger* l3 = event->l3Trigger();
  if(l3 != 0){
    nPrimTrackL3 = 0; 
    nGoodTrackL3 = 0; 
    sumPtL3=0.0;
    vectorSumPtL3=0.0;
    weightedEtaL3=0.0;
    weightedPhiL3=0.0;
    float sumPxL3=0.0;
    float sumPyL3=0.0;
    float maxptL3=0.0;
    StSPtrVecTrackNode& exnodeL3 = l3->trackNodes();
    Int_t nnodeL3=exnodeL3.size();
    for( Int_t in=0; in<nnodeL3; in++ ) {
      UInt_t nprimL3 = exnodeL3[in]->entries(global);
      if(nprimL3==1){
	StppTrack *t = new((*pTracksL3)[nPrimTrackL3]) StppTrack(exnodeL3[in]->track(global));
	if(t->nHits>23) nGoodTrackL3++;
	if(t->pt > maxptL3){
	  maxptL3 = t->pt;
	  LCPL3 = nPrimTrackL3;
	}
	sumPtL3 += t->pt;
	sumPxL3+=t->pt * cos(t->phi0);
	sumPtL3+=t->pt * sin(t->phi0);
	weightedEtaL3 += t->pt * t->eta;
	nPrimTrackL3++;
      }
    }
    //create at least 1 track even if there is none
    if(nPrimTrackL3==0) StppTrack *t = new((*pTracksL3)[nPrimTrackL3]) StppTrack();
    if(sumPtL3>0.0){
      vectorSumPtL3=sqrt(sumPxL3*sumPxL3+sumPyL3*sumPyL3);
      weightedPhiL3=(float)atan2((double)sumPyL3,(double)sumPxL3);
      weightedEtaL3/=sumPtL3; 
    }
    if (infoLevel > 1){
      cout << "StppEvent : number of L3 tracks " << nPrimTrackL3 << endl;
    }
    
    // Get primary vertex
    StPrimaryVertex* vtxL3 = l3->primaryVertex();
    if(vtxL3) {
      xVertexL3 = vtxL3->position().x();
      yVertexL3 = vtxL3->position().y();
      zVertexL3 = vtxL3->position().z();
      if (infoLevel > 1){
	cout << "StppEvent : L3 vertex " 
	     << xVertexL3 << " "  << yVertexL3 << " " << zVertexL3 << endl;
      }
    }
    else {
      xVertexL3 = -9999. ;
      yVertexL3 = -9999. ;
      zVertexL3 = -9999. ;
      if (infoLevel > 1){
	cout<<"StppEvent: There was no L3 vertex"<<endl;
      } 
    }
  }
#endif

  StTriggerDetectorCollection* trg = event->triggerDetectorCollection();
  if(trg){
    StBbcTriggerDetector* bbc = &(trg->bbc());
    if(bbc){
      bbcAdcSum = bbc->adcSumAll();
      bbcNHit = bbc->nHitAll();
      zVertexBbc = bbc->zVertex();
    }
    StCtbTriggerDetector& ctb = trg->ctb();
    ctbAdcSum = 0.0;
    ctbNHit = 0;
    for(unsigned int i=0; i<ctb.numberOfTrays(); i++){
      for(unsigned int j=0; j<ctb.numberOfSlats(); j++){
	ctbAdcSum += ctb.mips(i,j);
	if(ctb.mips(i,j)>0) ctbNHit++;
      }
    }
  }
  
  StFpdCollection* fpd = event->fpdCollection();
  if(fpd){
    token = fpd->token();
    fpdAdcSumNorth  = fpd->sumAdcNorth();
    fpdAdcSumSouth  = fpd->sumAdcSouth();
    fpdAdcSumTop    = fpd->sumAdcTop();
    fpdAdcSumBottom = fpd->sumAdcBottom();
    fpdAdcSumPres1  = fpd->sumAdcPreShower1();
    fpdAdcSumPres2  = fpd->sumAdcPreShower2();
    fpdAdcSumSmdX   = fpd->sumAdcSmdX();
    fpdAdcSumSmdY   = fpd->sumAdcSmdY();
    fpdSouthVeto    = fpd->southVeto();
  }
  
  StL0Trigger* l0=event->l0Trigger();
  if(l0){
    token = l0->triggerToken();
    triggerWord = l0->triggerWord();
    bunchId = l0->bunchCrossingId();
    bunchId7bit = l0->bunchCrossingId7bit();
    doubleSpinIndex = l0->spinBits();
    if(bunchId != bunchId7bit){
      cout << "2 bunch Id did not agree " << bunchId <<" "<< bunchId7bit<<endl;
    }
  }
  
  cout << "AKIO-Run#/Token/Unix-Time/BunchID/7Bit/SpinBits:" 
       << runN << " " << token << " " << time << " " 
       << bunchId << " " << bunchId7bit << " " << doubleSpinIndex  << endl;

  return 0;
}
#endif /*__CINT__*/
