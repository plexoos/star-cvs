/***************************************************************************
 *
 * $Id: StppuDstMaker.cxx,v 1.5 2003/02/04 21:57:10 akio Exp $
 * 
 * Author: Akio Ogawa June 2001
 ***************************************************************************
 *
 * Description:  TTree uDst for spin-pp
 *
 ***************************************************************************
 *
 * $Log: StppuDstMaker.cxx,v $
 * Revision 1.5  2003/02/04 21:57:10  akio
 * Improvments on pi0 reconstruction code and ntuple
 *
 * Revision 1.4  2002/12/04 20:28:09  thenry
 * StppuDstMaker was modified to allow multiple jet analysis modules to be
 * run simultaneosly with various parameters while the Maker loads the events
 * and analyses them.  Four different jet analyzers exist:
 *
 * Konstanin's Analyzers:
 *     Kt type: StppKonstKtJetAnalyzer
 *     Cone type: StppKonstConeJetAnalyzer
 *
 * Mike's Analyzers:
 *     Kt type: StppMikeKtJetAnalyzer
 *     Cone type: StppMikeConeJetAnalyzer
 *
 * These modules all require the StJetFinder modules.
 *
 * Revision 1.3  2002/06/24 13:22:59  akio
 * numerous bug fix & updates
 *
 * Revision 1.2  2002/02/11 20:30:48  akio
 * Many updates, including very first version of jet finder.
 *
 * Revision 1.1  2002/01/16 20:22:54  akio
 * First version
 *
 **************************************************************************/
#include <string.h>
#include <iostream.h>

#include "TFile.h"
#include "TTree.h"

#include "StChain.h"
#include "StEventTypes.h"
#include "StMessMgr.h"
#include "StIOMaker/StIOMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/EMC/StEmcMicroEvent.h"

#include "StppuDstMaker.h"
#include "StppEvent.h"
#include "StppGeant.h"
#include "StTriggerDetectorCollection.h"
#include "StBbcTriggerDetector.h"
#include "StFpdCollection.h"
#include "St_trg_Maker/St_trg_Maker.h"
#include "StEmcClusterCollection.h"
#include "StEmcPoint.h"

extern "C" void initfpdpi0_(const char*);
extern "C" void finishfpdpi0_();

ClassImp(StppuDstMaker)
  
StppuDstMaker::StppuDstMaker(const Char_t *name) 
  : StMaker(name), mGoodCounter(0), mBadCounter(0){
    numJetBranches = 0;
    jetBranches = new (StppJetAnalyzer*)[MAXANALYZERS];
    names = new (char*)[MAXANALYZERS];
    infoLevel = 0;
    mudst=0;
    saveEventWithNoJets = true;
#ifdef _EMC_
    storeEMC = true;
#endif
}

void StppuDstMaker::SetSaveEventWithNoJets(bool saveIt)
{
  saveEventWithNoJets = saveIt;
}

void StppuDstMaker::addAnalyzer(StppJetAnalyzer* a, const char * name)
{
    char* branchName = new char[strlen(name)+2];
    strcpy(branchName, name);
    //this worked for Thomas, but not for Mike
    //strcat(branchName, ".");
    
    names[numJetBranches] = branchName;
    jetBranches[numJetBranches] = a;
    numJetBranches++;
}

Int_t StppuDstMaker::Init(const Char_t *filename) 
{
    // creating uDst file name
    TString uDstFileName(filename);
    StIOMaker* pIOMaker = (StIOMaker*)GetMaker("IO");
    if(!pIOMaker){ pIOMaker = (StIOMaker*)GetMaker("inputStream"); }
    if(pIOMaker){
	uDstFileName = pIOMaker->GetFile() ;
	char* ccc = "/" ;
	Ssiz_t slashPosition = uDstFileName.Last(*ccc) ;
	if ( slashPosition != -1 &&
	     slashPosition < uDstFileName.Length() )uDstFileName.Remove(0,slashPosition+1);
    }
    uDstFileName.ReplaceAll(".dst.root",".spinDst.root");
    uDstFileName.ReplaceAll(".event.root",".spinDst.root");
    uDstFileName.ReplaceAll(".MuDst.root",".spinDst.root");
    uDstFileName.ReplaceAll(".daq",".spinDst.root");
    uDstFileName.ReplaceAll(":MuDst",".spinDst.root");
    cout << "StppuDstMaker: spiunDst output file: " << uDstFileName << endl;
    
    //open udst file
    m_outfile = new TFile(uDstFileName,"recreate");
    //  m_outfile->SetFormat(1);
    m_outfile->SetCompressionLevel(1);
    
    //create udst & its branches
    ppuDst  = new TTree("uDst","ppSpinuDst",99);
    ppEvent = new StppEvent(); ppEvent->setInfoLevel(infoLevel);
    ppuDst->Branch ("Event","StppEvent",&ppEvent,64000,99);
    for(int i = 0; i < numJetBranches; i++)
	{
	    jetBranches[i]->addBranch(names[i], ppuDst);
	    ppEvent->addAnalyzer(jetBranches[i]);
	}
#ifdef _GEANT_
    ppGeant = new StppGeant(); ppGeant->setInfoLevel(infoLevel);
    ppuDst->Branch ("Geant","StppGeant",&ppGeant,64000,99);
#endif
#ifdef _BBC_data_
    bbc     = new StBbcTriggerDetector();
    ppuDst->Branch ("Bbc","StBbcTriggerDetector",&bbc,64000,99);
#endif
#ifdef _FPD_data_
    fpd     = new StFpdCollection();
    ppuDst->Branch ("Fpd","StFpdCollection",&fpd,64000,99);
#endif
#ifdef _EMC_
    if(storeEMC)
	{
	    emcEvent = new StEmcMicroEvent();  //will get this data from the EmcMicroEvent
	    ppuDst->Branch ("EmcMuEvent", "StEmcMicroEvent", &emcEvent, 64000, 99);
	}
#endif
#ifdef _EMC_CLUSTERS_
    emcClusters[0]=new StEmcClusterCollection();
    emcClusters[1]=new StEmcClusterCollection();
    emcClusters[2]=new StEmcClusterCollection();
    //  cout << "********* adding emc cluster branch" << endl; 
    //  ppuDst->Branch ("EmcClusterTower","StEmcClusterCollection",&emcClusters[0],64000,99);
    //  ppuDst->Branch ("EmcClusterSMDX","StEmcClusterCollection",&emcClusters[1],64000,99);
    //  ppuDst->Branch ("EmcClusterSMDY","StEmcClusterCollection",&emcClusters[2],64000,99);
#endif
#ifdef _EMC_POINTS_
    ppEmcPoints = new StppEmcPoints();
    //emcPoints = new TClonesArray("EmcPoints", MAX_EMC_POINTS);
    cout << "*********  adding stppemcpoint branch" << endl; 
    ppuDst->Branch ("EmcPoints","StppEmcPoints",&ppEmcPoints,64000,99);
#endif  
    
    uDstFileName.ReplaceAll(".spinDst.root",".spinDst.nt");
    initfpdpi0_(uDstFileName.Data());
    
    return StMaker::Init();
}

Int_t StppuDstMaker::Make() {
  cout <<" Start StppuDstMaker :: "<< GetName() <<" mode="<<m_Mode<<endl;   

  ppEvent->clear();
#ifdef _GEANT_
  ppGeant->clear();
#endif
#ifdef _EMC_POINTS_
  //emcPoints->Clear();
  ppEmcPoints->Clear();
#endif
  
  // Get MuDst, or if it's not there, get StEvent
  StEvent* event;
  if(mudst) {
    ppEvent->setMuDst(mudst);
    event = 0;
  }
  else{
    event = (StEvent *)GetInputDS("StEvent");
    if(!event){
      mBadCounter++;
      return kStOK;
    }
  }
  
  // fill ppEvent 
  int res;
  res = ppEvent->fill(event);
  if(res<0){
    mBadCounter++;
    return kStOK;
  }
  
  // Get geant info, if any, and fill geant branch
#ifdef _GEANT_
  TDataSet *geantBranch = GetInputDS("geantBranch");
  if(geantBranch){
    ppGeant->fill(geantBranch);
  }
#endif
  
  // Get FPD & BBC infos
#ifdef _BBC_data_
  if(event) bbc = &(event->triggerDetectorCollection()->bbc());
  if(mudst) bbc = &(mudst->event()->bbcTriggerDetector());
#endif
#ifdef _FPD_data_
  if(event) fpd = event->fpdCollection();
  if(mudst) fpd = &(mudst->event()->fpdCollection());
#endif

  //Get EMC info
#ifdef _EMC_CLUSTERS_
  //cout << "filling emc cluster branch" << endl; 
  if(event){
    emcClusters[0] = event->emcCollection()->detector(kBarrelEmcTowerId)->cluster();
    emcClusters[1] = event->emcCollection()->detector(kBarrelSmdEtaStripId)->cluster();
    emcClusters[2] = event->emcCollection()->detector(kBarrelSmdPhiStripId)->cluster();
  }
#endif
#ifdef _EMC_POINTS_
  cout << "filling emcpoint branch" << endl; 
  if(event){
    StSPtrVecEmcPoint& bemcp = event->emcCollection()->barrelPoints();
    ppEmcPoints->setNumPoints(bemcp.size());
    for (int i=0; i<bemcp.size(); i++){
      ppemcPoints->Fill(i, *bemcp[i]);
      //cout << i << endl;
    }
    ppemcPoints->ConstructNbrArray();
  }
#endif
  
  //write out to uDst
  if(saveEventWithNoJets)
  {
    ppuDst->Fill();
  }
  else if(ppEvent->hasJets())
  {
    ppuDst->Fill();
  }
  
  mGoodCounter++;

  return kStOk;
}

Int_t StppuDstMaker::Finish()
{
  m_outfile->Write();
  m_outfile->Close();
  finishfpdpi0_();
  cout << "=================================================================\n";
  cout << "StppuDstger statistics:\n";
  cout << "events with StppuDstger data: " << mGoodCounter << endl;
  cout << "events without StppuDstger data: " << mBadCounter << endl;
  cout << "=================================================================\n";    
  StMaker::Finish();
  return kStOK;
}

