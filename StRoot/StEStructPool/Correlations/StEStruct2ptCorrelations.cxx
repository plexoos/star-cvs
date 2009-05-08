/**********************************************************************
 *
 * $Id: StEStruct2ptCorrelations.cxx,v 1.25 2009/05/08 00:09:54 prindle Exp $
 *
 * Author: Jeff Porter adaptation of Aya's 2pt-analysis
 *
 **********************************************************************
 *
 * Description:  Analysis code for 2pt-analysis. 
 *    The analysis runs as follows:
 *       1D and 2D arrays (in yt,eta,phi) are setup
 *       and filled for each of the 8 pair types:
 *       Sibling (++, +-, -+, --)
 *       Mixed   (++, +-, -+, --)
 *       Particle id is done via dEdx and introduced into the analysis via cut-bins.
 *       Order particles so pi always come before K before p and plus comes before
 *       minus. 2D histograms will not be guaranteed to be symmetric.
 *       The 2D versions are additionally divided into z-vertex (via the StEStructBuffer)
 *       After arrays are filled (looped over all events/job), Histograms are 
 *       created, filled, and written out to the data file for further
 *       processing.
 *
 *       Note that if we find charge symmetry we can form LS, US, CD and CI
 *       combinations using these histograms.
 *
 *
 ***********************************************************************/
#include "StEStruct2ptCorrelations.h"

#include "TH1F.h"
#include "TH2F.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TFile.h"

#include "StEStructPool/EventMaker/StEStructEvent.h"
#include "StEStructPool/EventMaker/StEStructTrack.h"
#include "StEStructPool/Correlations/StEStructMaxB.h"  
#include "StEStructPool/Correlations/StEStructBuffer.h"  // to read some constants  

#include "StTimer.hh"
#include "StEStructCutBin.h"

#include "Stiostream.h"

#include "StMessMgr.h"

#include <iostream>
using namespace std;

ClassImp(StEStruct2ptCorrelations);

//--------------------------------------------------------------------------
StEStruct2ptCorrelations::StEStruct2ptCorrelations(int mode) {
  initInternalData();
  manalysisMode=mode;
  mPairCuts = new StEStructPairCuts; 
}


//--------------------------------------------------------------------------
StEStruct2ptCorrelations::StEStruct2ptCorrelations(StEStructPairCuts* pcuts, int mode) {
  initInternalData();
  manalysisMode=mode;
  mPairCuts=pcuts;

}

//----------------------------------------------------------
void  StEStruct2ptCorrelations::initInternalData(){

  mPairCuts = NULL;
  mQAHists = NULL;
  mCurrentEvent = NULL;
  mMixingEvent = NULL;
  moutFileName = NULL;
  mqaoutFileName = NULL;

  mskipPairCuts              = false;
  mdoPairCutHistograms       = false;
  mdoPairDensityHistograms   = false;
  mskipEtaDeltaWeight        = false;
  mdoInvariantMassHistograms = false;
  mdoFillEtaEta              = false;
  mdoFillSumHistograms       = false;
  mdontFillMeanPt            = false;
  mdontFillYtYt              = false;

  mInit = false;
  mDeleted = false;
  mHistosWritten = false;
  mlocalQAHists = false;
  manalysisIndex = -1;

  kZBuffMin   = -75.0;
  kZBuffMax   = +75.0;
  kBuffWidth  = 5.0;
  kNumBuffers = int( (kZBuffMax-kZBuffMin)/kBuffWidth );  // Better be 30.
  //  for(int i=0;i<kNumBuffers;i++)mbuffCounter[i]=0;
  mZBufferCutBinning = 0;
}



//--------------------------------------------------------------------------
StEStruct2ptCorrelations::~StEStruct2ptCorrelations(){ cleanUp(); };


void StEStruct2ptCorrelations::init(){

  cout << "Initializing with analysis mode " << manalysisMode << endl;
  cout << "Use Z Buffer cut binning =  " << mZBufferCutBinning << endl;

  mCurrentEvent=NULL;
  mtimer=NULL;

   //--> code to simplify hist-creation via class held name defs
   char* _tmpName[]={"Sibpp","Sibpm","Sibmp","Sibmm","Mixpp","Mixpm","Mixmp","Mixmm"};
   char* _tmpTitle[]={"Sibling : +.+","Sibling : +.-","Sibling : -.+","Sibling : -.-",
                      "Mixed : +.+","Mixed : +.-","Mixed : -.+","Mixed : -.-"};

   for(int i=0;i<8;i++){
     bName[i]=new char[6];
     strcpy(bName[i],_tmpName[i]);
     bTitle[i]=new char[20];
     strcpy(bTitle[i],_tmpTitle[i]);
   }

  if (manalysisMode & 0x1) {
     mskipPairCuts=true;
  } 
  if (manalysisMode & 0x2) {
    mdoPairCutHistograms=true;
  }
  if (manalysisMode & 0x4) {
    mdoPairDensityHistograms=true;
  }
  if (manalysisMode & 0x8) {
    mskipEtaDeltaWeight = true;
  }
  if (manalysisMode & 0x10) {
    mdoInvariantMassHistograms = true;
  }
  if (manalysisMode & 0x20) {
    mdoFillEtaEta = true;
  }
  if (manalysisMode & 0x40) {
    mdoFillSumHistograms = true;
  }
  if (manalysisMode & 0x80) {
    mdontFillMeanPt = true;
  }
  if (manalysisMode & 0x100) {
    mdontFillYtYt = true;
  }

  cout << "  Skip Pair Cuts = " << mskipPairCuts << endl;
  cout << "  Do Pair Cut Hists = " << mdoPairCutHistograms << endl;
  cout << "  Do Pair Density Hists = " << mdoPairDensityHistograms << endl;
  cout << "  Skip EtaDelta weights = " << mskipEtaDeltaWeight << endl;
  cout << "  Do Invariant mass histograms = " << mdoInvariantMassHistograms << endl;
  cout << "  Fill EtaEta (and PhiPhi) = " << mdoFillEtaEta << endl;
  cout << "  Fill Sum Histograms (SYt, SEta) = " << mdoFillSumHistograms << endl;
  cout << "  Don't fill mean pt histograms = " << mdontFillMeanPt << endl;
  cout << "  Don't fill YtYt histograms = " << mdontFillYtYt << endl;

  for(int i=0;i<8;i++)numPairs[i]=numPairsProcessed[i]=mpossiblePairs[i]=0;

  initArrays();
// Try allocating histograms at start of job.
// If we don't add histograms to directory we don't get the obnoxious
//   Potential memory leak error.
  TH1::AddDirectory(kFALSE);
  //initHistograms();

  /* Event count via Nch distribution */
  char name[2048];
  int nZBins = 1;
  if (mZBufferCutBinning) {
      nZBins = kNumBuffers;
  }
  mHNEventsSib = new TH1D*[nZBins];
  mHNEventsMix = new TH1D*[nZBins];
  mHNEventsPosSib = new TH1D*[nZBins];
  mHNEventsPosMix = new TH1D*[nZBins];
  mHNEventsNegSib = new TH1D*[nZBins];
  mHNEventsNegMix = new TH1D*[nZBins];
  for (int iz=0;iz<nZBins;iz++) {
      sprintf(name,"NEventsSib_zBuf_%i",iz);
      mHNEventsSib[iz]=new TH1D(name,name,1000,0.,2000.);
      sprintf(name,"NEventsMix_zBuf_%i",iz);
      mHNEventsMix[iz]=new TH1D(name,name,1000,0.,2000.);
      sprintf(name,"NEventsPosSib_zBuf_%i",iz);
      mHNEventsPosSib[iz]=new TH1D(name,name,1000,0.,2000.);
      sprintf(name,"NEventsPosMix_zBuf_%i",iz);
      mHNEventsPosMix[iz]=new TH1D(name,name,1000,0.,2000.);
      sprintf(name,"NEventsNegSib_zBuf_%i",iz);
      mHNEventsNegSib[iz]=new TH1D(name,name,1000,0.,2000.);
      sprintf(name,"NEventsNegMix_zBuf_%i",iz);
      mHNEventsNegMix[iz]=new TH1D(name,name,1000,0.,2000.);
  }
  
  StEStructCutBin* cb = StEStructCutBin::Instance();
  int ncutbins = cb->getNumBins();
  int nQAbins  = cb->getNumQABins();

  // Do we want to make invariant mass histograms?
  // (Only works in cutbin mode 5, which uses PID assignments.)
  if (mdoInvariantMassHistograms) {
    cb->setCutBinHistMode(1);
  } else {
    cb->setCutBinHistMode(0);
  }

  /* QA histograms */
  if(!mQAHists) {
    mlocalQAHists = true;
    cout<<"creating QA hists"<<endl;
    mQAHists = new StEStructQAHists(); // if not set .. assume data
    mQAHists->initTrackHistograms(nQAbins,analysisIndex());
  } else {
    cout<<"init QA Hists"<<endl;
    mQAHists->initTrackHistograms(nQAbins);  // will do it for only 1 analysis
  }

  /* Inclusive pt distribution */
    mHptAll = new TH1D("ptAll","ptAll",500,0,10.);

  /*
   * pt distributions for parent distributions.
   * Use to get estimate of mean pt of parent distributionb but also
   * mean number of tracks in the sample.
   */
    int numParentBins=cb->getNumParentBins();
    int nzb = 1;
    if (mZBufferCutBinning) {
        nzb = kNumBuffers;
    }

    mHMeanPtP = new TH1D*[numParentBins*nzb];
    mHMeanPtM = new TH1D*[numParentBins*nzb];
    mHMeanYtP = new TH1D*[numParentBins*nzb];
    mHMeanYtM = new TH1D*[numParentBins*nzb];
    mHEtaP    = new TH1D*[numParentBins*nzb];
    mHEtaM    = new TH1D*[numParentBins*nzb];
    TString hname;
    StEStructBinning* b = StEStructBinning::Instance();
    for(int p=0;p<numParentBins;p++){
        for (int z=0;z<nzb;z++) {
            int pz = p*nzb + z;
            hname = "meanPtP_parentBin"; hname += p; hname += "_zBuf_"; hname += z;
            mHMeanPtP[pz] = new TH1D(hname.Data(),hname.Data(),200,b->ptMin(),b->ptMax());
            hname = "meanPtM_parentBin"; hname += p; hname += "_zBuf_"; hname += z;
            mHMeanPtM[pz] = new TH1D(hname.Data(),hname.Data(),200,b->ptMin(),b->ptMax());
            hname = "meanYtP_parentBin"; hname += p; hname += "_zBuf_"; hname += z;
            mHMeanYtP[pz] = new TH1D(hname.Data(),hname.Data(),200,b->ytMin(),b->ytMax());
            hname = "meanYtM_parentBin"; hname += p; hname += "_zBuf_"; hname += z;
            mHMeanYtM[pz] = new TH1D(hname.Data(),hname.Data(),200,b->ytMin(),b->ytMax());
            hname = "etaP_parentBin"; hname += p; hname += "_zBuf_"; hname += z;
            mHEtaP[pz] = new TH1D(hname.Data(),hname.Data(),100,b->etaMin(),b->etaMax());
            hname = "etaM_parentBin"; hname += p; hname += "_zBuf_"; hname += z;
            mHEtaM[pz] = new TH1D(hname.Data(),hname.Data(),100,b->etaMin(),b->etaMax());
        }
    }
  

  /* Event Mixing Parameters */
  mHmix = new TH2D("EventMixing","Event Mixing: delta-Z vs delta-N",50,0,10, 50,0,200);  // deltaZ vs deltaN
  mHcb = new TH1D("hcb","Cutbin usage",ncutbins,-0.5,ncutbins - 0.5);  // local
  TH1::AddDirectory(kTRUE);

  mInit=true;
}

//--------------------------------------------------------------------------
void StEStruct2ptCorrelations::finish(){

  if(!moutFileName){
    cout<<" NO OUTPUTFILE TO WRITE TO ..... giving up ...."<<endl;
    return;
  }

  if(!mInit){  
    cout<<" WARNING: init=false"<<endl;
    cout<<"No events were processed, either there was a problem reading input files or cuts are too restrictive"<<endl;
    return;
  }

  // We call finish explicitly in doEStruct, but it also gets called when
  // we tell root to quit. I don't know the details of why. If we have
  // already written histograms don't bother a second time.
  // (Also I think finish() will invoke cleanUp() so second time here
  //  all the arrays will be gone and we get a core dump if we try
  //  touching them.)
  if (mHistosWritten) {
      return;
  }
  TH1::AddDirectory(kFALSE);
  initHistograms();
  fillHistograms();
  TH1::AddDirectory(kTRUE);
  TFile * tf=new TFile(moutFileName,"RECREATE");
  tf->cd();
  writeHistograms();
  tf->Close();
  // When we don't delete tf it gets reported as a memory leak
  // (which it is, but we are quitting anyway so why bother with it?)
  mHistosWritten = true;
}

//--------------------------------------------------------------------------
void StEStruct2ptCorrelations::cleanUp(){ 
  if(mDeleted) return;
  deleteHistograms();
  deleteArrays(); 

  int nZBins = 1;
  if (mZBufferCutBinning) {
      nZBins = kNumBuffers;
  }
  for (int iz=0;iz<nZBins;iz++) {
      delete mHNEventsSib[iz];
      delete mHNEventsMix[iz];
      delete mHNEventsPosSib[iz];
      delete mHNEventsPosMix[iz];
      delete mHNEventsNegSib[iz];
      delete mHNEventsNegMix[iz];
  }
  delete [] mHNEventsSib;
  delete [] mHNEventsMix;
  delete [] mHNEventsPosSib;
  delete [] mHNEventsPosMix;
  delete [] mHNEventsNegSib;
  delete [] mHNEventsNegMix;
  delete mHptAll;
  mDeleted=true;
}

//--------------------------------------------------------------------------
// Parse cuts file for limits on vertex position.
// Want 5cm wide buffers.
void StEStruct2ptCorrelations::setZBuffLimits(StEStructCuts* ecut) {

  kZBuffMin = ecut->minVal("primaryVertexZ");
  kZBuffMax = ecut->maxVal("primaryVertexZ");
  kNumBuffers = int( (kZBuffMax-kZBuffMin)/kBuffWidth ); 

  if(kZBuffMin==kZBuffMax){  // no z vertex cut ...
    kZBuffMin=-100.;
    kZBuffMax=100.;
    kNumBuffers = 1;
    kBuffWidth= (kZBuffMax - kZBuffMin) / kNumBuffers; // adjust widths
  }

  cout<<"Setting ZBuffers:  Max="<<kZBuffMax<<" Min="<<kZBuffMin<<" NumBuff="<<kNumBuffers<<endl;
  if(kNumBuffers<=_MAX_ZVBINS_) return; // we're good to go

  kNumBuffers=_MAX_ZVBINS_;  
  kBuffWidth= (kZBuffMax - kZBuffMin) / kNumBuffers; // adjust widths
  if(kBuffWidth>6.5) { 
     gMessMgr->Warning()<<" Zvertex Width ="<<kBuffWidth<<" gt 6.5 cm"<<endm;
  }
}
//---------------------------------------------------------------------
void StEStruct2ptCorrelations::writeDiagnostics(){

  int nIn=0;
  int nOut=0;
  for(int i=0;i<kNumBuffers;i++){
    nIn  += mbuffer[i].numEventsIn();
    nOut += mbuffer[i].numEventsDeleted();
  }
  cout<<" Analysis["<<analysisIndex()<<"] recieved "<<nIn<<" useful events, deleted "<<nOut<<" of them"<<endl;

}

//
//-------  Analysis Function ------//
//
//--------------------------------------------------------------------------
bool StEStruct2ptCorrelations::doEvent(StEStructEvent* event){

    if(!event) {
        mCurrentEvent=NULL;
        return false;
    }

    if(mInit == false) init();

    if(2>event->Ntrack()){
        delete event;
        return true;
    }

    //cout << "Doing event " << event->EventID() << ":  " << event->Ntrack() << " tracks." << endl;

  moveEvents();
  mCurrentEvent=event;
  int nZBin = 1;
  int iZBin = 0;
  if (mZBufferCutBinning) {
      nZBin = kNumBuffers;
      iZBin = bufferIndex();
  }
  mHNEventsSib[iZBin]->Fill(event->Ntrack());
  mHNEventsPosSib[iZBin]->Fill(event->Npos());
  mHNEventsNegSib[iZBin]->Fill(event->Nneg());

  // inclusive pt distribution
  // Note that cuts are done in yt-yt space and bins are
  // set up before pid is done (so assume pion mass.)
  // I think these are just diagnostic histograms. Even so
  // I am uncomfortable with this pt histogramming.
  // (dEdx histogramming is ok though.)
  // Assign mass according to dEdx pid here.
  // djp 11/28/2005

  float Mass[] = {0.1396, 0.1396, 0.497, 0.9383};

  StEStructCutBin* cb = StEStructCutBin::Instance();

  StEStructTrackCollection *tc;
  for(int ich=0;ich<2;ich++){
    if(ich==0){
       tc=mCurrentEvent->TrackCollectionP();
    } else {
       tc=mCurrentEvent->TrackCollectionM();
    }

    for(StEStructTrackIterator iter = tc->begin(); iter != tc->end(); iter++) {
        int parentClass = cb->getParentBin(mPairCuts,(*iter));
        int ipb = parentClass*nZBin + iZBin;
        if (0 == ich) {
            mHMeanYtP[ipb]->Fill((*iter)->Yt());
            mHMeanPtP[ipb]->Fill((*iter)->Pt());
            mHEtaP[ipb]->Fill((*iter)->Eta());
         } else {
            mHMeanYtM[ipb]->Fill((*iter)->Yt());
            mHMeanPtM[ipb]->Fill((*iter)->Pt());
            mHEtaM[ipb]->Fill((*iter)->Eta());
         }
         mQAHists->fillTrackHistograms(*iter,parentClass);

if (0) {
        // Choose mass according to dEdx (in which case transverse and longitudinal
        // rapidities will be calculated as actual rapidities) or set mass to 0
        // in which case we will use the quantity Jeff was using for transverse rapidity
        // (a mid-rapidity approximation for pions) and eta for longitudinal rapidity.
        // Sould really have a switch accessible from macro level instead of
        // using cutMode.

        if (cb->getMode() == 5) {
            // Try using real rapidity calculation.
            (*iter)->SetMassAssignment(Mass[parentClass]);
            // (*iter)->SetMassAssignment(0);
        } else {
            // 0 should be default, but just to be explicit here/
            (*iter)->SetMassAssignment(0);
        }
        // Always use psuedo-rapidity for now.
        // We are used to looking at the eta-phi plots (more or less.)
        //     (*iter)->SetMassAssignment(0);
} else {
            (*iter)->SetMassAssignment(0);
}
    }
  }

  // Set magnetic field for pair cuts
  mPairCuts->SetBField( mCurrentEvent->BField() );
  return makeSiblingAndMixedPairs();
}


//-----------------------------------------------------------------------
int StEStruct2ptCorrelations::bufferIndex(){
  if(!mCurrentEvent || kBuffWidth==0.) return -1;

  // this should only be in 1 place 
  return (int) floor((mCurrentEvent->VertexZ()-kZBuffMin)/kBuffWidth); 
}

//--------------------------------------------------------------------------
void StEStruct2ptCorrelations::moveEvents(){

  if(!mCurrentEvent) return;
  if (mCurrentEvent->VertexZ() > kZBuffMax) {
      return;
  }

  int i=bufferIndex();

  if(i<0 || i>kNumBuffers-1) return;                              
  mbuffer[i].addEvent(mCurrentEvent);
  //  mbuffCounter[i]++;

}

//--------------------------------------------------------------------------
bool StEStruct2ptCorrelations::makeSiblingAndMixedPairs(){

  if(!mCurrentEvent) return false; // logic problem!
  if (mCurrentEvent->VertexZ() > kZBuffMax) {
      return false;
  }

  int i=bufferIndex();

  if(i<0 || i>kNumBuffers-1) return false;

  // I finally realized the cases 2 and 6 could be handled
  // by 1 and 5 (loop over -+ includes everything +- does.)
  // Those cases should be dropped for a small gain in speed,
  // but I would need to make more changes in the code than I
  // want to right now.   djp June 21, 2006
  mInterestingPair = 0;
  makePairs(mCurrentEvent,mCurrentEvent,0);
  makePairs(mCurrentEvent,mCurrentEvent,1);
  makePairs(mCurrentEvent,mCurrentEvent,2);
  makePairs(mCurrentEvent,mCurrentEvent,3);

  mbuffer[i].resetCounter();
  int mult = mCurrentEvent->Ntrack();
  //cout << "Event " << mCurrentEvent->EventID() << ": " << mult << " tracks;   Z = " << mCurrentEvent->VertexZ() << ", i = " << i << endl;
  //cout << i <<"\t"; mbuffer[i].Print();
  while(1){
    mMixingEvent=mbuffer[i].nextEvent(mult);
    if(!mMixingEvent) break;
    //cout << "\t  mixing " << mMixingEvent->EventID() << ": " << mMixingEvent->Ntrack()  << " tracks." << endl;
    int iZBin = 0;
    if (mZBufferCutBinning) {
        iZBin = bufferIndex();
    }
    mHNEventsMix[iZBin]->Fill(mMixingEvent->Ntrack());
    mHNEventsPosMix[iZBin]->Fill(mMixingEvent->Npos());
    mHNEventsNegMix[iZBin]->Fill(mMixingEvent->Nneg());
    float deltaZ = fabs(mCurrentEvent->VertexZ() - mMixingEvent->VertexZ());
    float deltaN = abs(mCurrentEvent->Ntrack() -  mMixingEvent->Ntrack());
    mHmix->Fill(deltaZ,deltaN);
    makePairs(mCurrentEvent,mMixingEvent,4);
    makePairs(mCurrentEvent,mMixingEvent,5);
    makePairs(mCurrentEvent,mMixingEvent,6);
    makePairs(mMixingEvent,mCurrentEvent,5);
    makePairs(mMixingEvent,mCurrentEvent,6);
    makePairs(mCurrentEvent,mMixingEvent,7);
  }

  return true;
}

//--------------------------------------------------------------------------
/*  For non-pid case:
 *   ++ put pair into array pp at +/- \delta\eta, +/- \delta\phi.
 *   -- put pair into array mm at +/- \delta\eta, +/- \delta\phi.
 *   +- put pair into array pm at +/- \delta\eta, +/- \delta\phi.
 *   -+ ignore pair.
 *   This is because we have an "extra" loop and the same pair will
 *   turn up with opposite sign \delta\eta.
 *   Always put pair into y1-y2 and y2-y1.
 *
 *   For pid case we don't want to symmetrize yt-yt and we want to
 *   distinguish +- from -+ in the case of non-identical particles.
 *   ++ put pair into array pp at +/- \delta\eta, +/- \delta\phi.
 *   -- put pair into array mm at +/- \delta\eta, +/- \delta\phi.
 *   If pid1 == pid2
 *     +- put pair into array pm at +/- \delta\eta, +/- \delta\phi.
 *     -+ ignore
 *     put pair into 
 *   If pid1 != pid2
 *     put pi+K-, pi+\bar p, K+\bar p pair into array pm at +/- \delta\eta, +/- \delta\phi.
 *     put pi-K+, pi-p, K-p pair into array mp at +/- \delta\eta, +/- \delta\phi.
 *   
 */

void StEStruct2ptCorrelations::makePairs(StEStructEvent* e1, StEStructEvent* e2, int j){

  if(j>=8) return;
  StEStructTrackCollection* t1;
  StEStructTrackCollection* t2;

  StEStructPairCuts& mPair = *mPairCuts;

  StEStructBinning* b = StEStructBinning::Instance();
  StEStructCutBin* cb = StEStructCutBin::Instance();

    ytBins**  ytyt;
    ytBins**  nytyt;
    if (!mdontFillYtYt) {
        ytyt        = mYtYt[j];
        nytyt       = mNYtYt[j];
    }

    dphiBins** jtdetadphi =   mJtDEtaDPhi[j];
    dphiBins** prjtdetadphi;
    dphiBins** pajtdetadphi;
    dphiBins** pbjtdetadphi;
    if (!mdontFillMeanPt) {
        prjtdetadphi = mPrJtDEtaDPhi[j];
        pajtdetadphi = mPaJtDEtaDPhi[j];
        pbjtdetadphi = mPbJtDEtaDPhi[j];
    }

    etaBins** etaeta;
    phiBins** phiphi;
    phiBins** nphiphi;
    etaBins** pretaeta;
    etaBins** paetaeta;
    etaBins** pbetaeta;
    phiBins** prphiphi;
    phiBins** paphiphi;
    phiBins** pbphiphi;

    if (mdoFillEtaEta) {
        etaeta   = mEtaEta[j];
        phiphi   = mPhiPhi[j];
        nphiphi  = mNPhiPhi[j];
        if (!mdontFillMeanPt) {
            pretaeta = mPrEtaEta[j];
            paetaeta = mPaEtaEta[j];
            pbetaeta = mPbEtaEta[j];
            prphiphi = mPrPhiPhi[j];
            paphiphi = mPaPhiPhi[j];
            pbphiphi = mPbPhiPhi[j];
        }
    }

    dytBins**  atytyt;
    dytBins**  atnytyt;
    dphiBins** jtsetadphi;
    dphiBins** jtnsetadphi;
    dphiBins** prjtsetadphi;
    dphiBins** pajtsetadphi;
    dphiBins** pbjtsetadphi;
    if (mdoFillSumHistograms) {
        atytyt      = mAtSYtDYt[j];
        atnytyt     = mAtNSYtDYt[j];
        jtsetadphi  = mJtSEtaDPhi[j];
        jtnsetadphi = mJtNSEtaDPhi[j];
        if (!mdontFillMeanPt) {
            prjtsetadphi = mPrJtSEtaDPhi[j];
            pajtsetadphi = mPaJtSEtaDPhi[j];
            pbjtsetadphi = mPbJtSEtaDPhi[j];
        }
    }

  qBins*  qinv  = mQinv[j];
  qBins*  nqinv = mNQinv[j];

  TPCSepBins* avgtsep = mTPCAvgTSep[j];
  TPCSepBins* avgzsep = mTPCAvgZSep[j];
  TPCSepBins* enttsep = mTPCEntTSep[j];
  TPCSepBins* entzsep = mTPCEntZSep[j];
  TPCSepBins* midtsep = mTPCMidTSep[j];
  TPCSepBins* midzsep = mTPCMidZSep[j];
  TPCSepBins* exittsep = mTPCExitTSep[j];
  TPCSepBins* exitzsep = mTPCExitZSep[j];
  TPCSepBins* midtp   = mTPCMidTdptP[j];
  TPCSepBins* midtn   = mTPCMidTdptN[j];
  TPCSepBins* midzp   = mTPCMidZdptP[j];
  TPCSepBins* midzn   = mTPCMidZdptN[j];

  TPCSepBins** avgtz =  mTPCAvgTZ[j];
  TPCSepBins** enttz =  mTPCEntTZ[j];
  TPCSepBins** midtz =  mTPCMidTZ[j];
  TPCSepBins** midtzc =  mTPCMidTZC[j];
  TPCSepBins** midtznc =  mTPCMidTZNC[j];
  TPCSepBins** exittz =  mTPCExitTZ[j];
  dptBins** enttd =  mTPCEntTdpt[j];
  dptBins** midtd =  mTPCMidTdpt[j];
  dptBins** exittd =  mTPCExitTdpt[j];


  switch(j) {
    case 0:
      {
        t1=e1->TrackCollectionP();
        t2=e2->TrackCollectionP();
        mPair.setPairType(0);
        mpossiblePairs[j]+=(int)floor(0.5*(t1->getEntries()*(t2->getEntries()-1)));
        break;
      }
        
  case 1:
    {
        t1=e1->TrackCollectionP();
        t2=e2->TrackCollectionM();
        mPair.setPairType(1);
        mpossiblePairs[j]+=(int)(t1->getEntries()*t2->getEntries());
        break;
    }       
  case 2:
    {
        t1=e1->TrackCollectionM();
        t2=e2->TrackCollectionP();
        mPair.setPairType(1);
        mpossiblePairs[j]+=(int)(t1->getEntries()*t2->getEntries());
        break;
    }       
  case 3:
    {
        t1=e1->TrackCollectionM();
        t2=e2->TrackCollectionM();
        mPair.setPairType(0);
        mpossiblePairs[j]+=(int)floor(0.5*(t1->getEntries()*(t2->getEntries()-1)));
        break;
    }
  case 4:
    {
        t1=e1->TrackCollectionP();
        t2=e2->TrackCollectionP();
        mPair.setPairType(2);
        mpossiblePairs[j]+=(int)(t1->getEntries()*t2->getEntries());
        break;
    }
  case 5:
    {
        t1=e1->TrackCollectionP();
        t2=e2->TrackCollectionM();
        mPair.setPairType(3);
        mpossiblePairs[j]+=(int)(t1->getEntries()*t2->getEntries());
        break;
    }
  case 6:
    {
        t1=e1->TrackCollectionM();
        t2=e2->TrackCollectionP();
        mPair.setPairType(3);
        mpossiblePairs[j]+=(int)(t1->getEntries()*t2->getEntries());
        break;
    }
  case 7:
    {
        t1=e1->TrackCollectionM();
        t2=e2->TrackCollectionM();
        mPair.setPairType(2);
        mpossiblePairs[j]+=(int)(t1->getEntries()*t2->getEntries());
        break;
    }
  }

  // for pair cut analysis, in mixed events you need to correct for the offset from different primary vertex Z values 

// Why did we require mdoPairDensityHistograms to set the ZOffset????
//  if(j>=4 && mdoPairDensityHistograms) mPair.SetZoffset(e2->VertexZ() - e1->VertexZ());
//  else mPair.SetZoffset(0);
    // Note that for j < 4 e1 = e2 (so Vz2-Vz1 = 0)
    mPair.SetZoffset(e2->VertexZ() - e1->VertexZ());

  StEStructTrackIterator Iter1;
  StEStructTrackIterator Iter2;

  int iyt1,iyt2,idyt,isyt;
  int ipt1,ipt2;
  int ieta1,ieta2,ideta,iseta;
  int iphi1,iphi2,idphi;
  int isavgt, isavgz, isentt, isentz;
  int ismidt, ismidz, isexitt, isexitz;
  float pt1, pt2;

  if(mtimer)mtimer->start();

  int nZBin = 1;
  int iZBin = 0;
  if (mZBufferCutBinning) {
      nZBin = kNumBuffers;
      iZBin = bufferIndex();
  }
  // Should be extracting mass from pid information.
  // For now use mass=0 so we can compare eta with non-pid analysis.
  //>>>>>>>>>>>>
  float mass1 = 0, mass2 = 0;

  int it1 = -1;
  for(Iter1=t1->begin();Iter1!=t1->end();++Iter1){
    it1++;

    mPair.SetTrack1(*Iter1);
    int it2;
    if(j==0 || j==3) { 
      Iter2=Iter1+1; 
      it2 = it1;
    } else { 
      Iter2=t2->begin(); 
      it2 = -1;
    }

    mHptAll->Fill(mPair.Track1()->Pt());

    for(; Iter2!=t2->end(); ++Iter2){
      it2++;
      numPairs[j]++;
// QA pair histograms have not been implemented yet.
// Looks like the mdoPairDensityHistograms sections do that job.
// I think the idea of those was that they were turned on during initial data
//  check, then turned off to save CPU time.
//      mQAHists->fillPairHistograms(*Iter1,*Iter2,i,0);
      mPair.SetTrack2(*Iter2);
      if( mskipPairCuts || (mPair.cutPair(mdoPairCutHistograms)==0) ){
        numPairsProcessed[j]++;

        int icb, jcb;
        if (!cb->ignorePair(&mPair) && ((jcb=cb->getCutBin(&mPair,j)) >= 0)) {
          int ncutbins=cb->getNumBins();
          mHcb->Fill(jcb);   // now filled for all cb modes
          if(jcb>=ncutbins) {
              cout << "ERROR, got cutbin " << jcb << " of " << ncutbins << " possible." << endl;
              return;
          }
          icb = jcb*nZBin + iZBin;
//          mQAHists->fillPairHistograms(*Iter1,*Iter2,jcb,1);

          // The following is special code to help save interesting p-p events.
          // Meant to be used in run where we only look at low multiplicity
          // events. Set a flag indicating this event has an interesting pair.
          // Routine calling makePairs may want to save event.
//          if (2*5 == (icb & 2*5)) {
//              mInterestingPair++;;
//          }

          pt1   = mPair.Track1()->Pt();
          pt2   = mPair.Track2()->Pt();

          iyt1  = b->iyt(mPair.Track1()->Yt(mass1));
          ipt1  = b->ipt(pt1);
          ieta1 = b->ieta(mPair.Track1()->Eta(mass1));
          iphi1 = b->iphi(mPair.Track1()->Phi());
          iyt2  = b->iyt(mPair.Track2()->Yt(mass2));
          ipt2  = b->ipt(pt2);
          ieta2 = b->ieta(mPair.Track2()->Eta(mass2));
          iphi2 = b->iphi(mPair.Track2()->Phi());

          float pwgt=pt1*pt2;
          float spta=pt1;
          float sptb=pt2;
          float nwgt = 1.0;
          float ytwgt = 1.0;
          if( !mskipEtaDeltaWeight ) {
             ytwgt = b->getDEtaWeight(mPair.DeltaEta());
//             pwgt*=nwgt;
//             spta*=nwgt;
//             sptb*=nwgt;
          }

         /*
          * Makes no sense for switchXX _and_ symmetrizeXX to be true at same time.
          */
          int symmetrizeXX = cb->symmetrizeXX(&mPair);
          int switchXX     = cb->switchXX(&mPair);
          if (switchXX) {
              if (!mdontFillYtYt) {
                  ytyt[icb][iyt2].yt[iyt1]+=ytwgt;
                  nytyt[icb][iyt2].yt[iyt1]+=1;
              }

              if (mdoFillEtaEta) {
                  etaeta[icb][ieta2].eta[ieta1]+=1; // nwgt;
                  phiphi[icb][iphi2].phi[iphi1]+=nwgt;
                  nphiphi[icb][iphi2].phi[iphi1]+=1;
                  if (!mdontFillMeanPt) {
                      pretaeta[icb][ieta2].eta[ieta1]+=pwgt/nwgt;
                      paetaeta[icb][ieta2].eta[ieta1]+=sptb/nwgt;
                      pbetaeta[icb][ieta2].eta[ieta1]+=spta/nwgt;
                      prphiphi[icb][iphi2].phi[iphi1]+=pwgt;
                      paphiphi[icb][iphi2].phi[iphi1]+=sptb;
                      pbphiphi[icb][iphi2].phi[iphi1]+=spta;
                  }
              }
          } else {
              if (!mdontFillYtYt) {
                  ytyt[icb][iyt1].yt[iyt2]+=ytwgt;
                  nytyt[icb][iyt1].yt[iyt2]+=1;
              }

              if (mdoFillEtaEta) {
                  etaeta[icb][ieta1].eta[ieta2]+=1; // nwgt;
                  phiphi[icb][iphi1].phi[iphi2]+=nwgt;
                  nphiphi[icb][iphi1].phi[iphi2]+=1;
                  if (!mdontFillMeanPt) {
                      pretaeta[icb][ieta1].eta[ieta2]+=pwgt/nwgt;
                      paetaeta[icb][ieta1].eta[ieta2]+=spta/nwgt;
                      pbetaeta[icb][ieta1].eta[ieta2]+=sptb/nwgt;
                      prphiphi[icb][iphi1].phi[iphi2]+=pwgt;
                      paphiphi[icb][iphi1].phi[iphi2]+=spta;
                      pbphiphi[icb][iphi1].phi[iphi2]+=sptb;
                  }
              }
              if (symmetrizeXX) {
                  if (!mdontFillYtYt) {
                      ytyt[icb][iyt2].yt[iyt1]+=ytwgt;
                      nytyt[icb][iyt2].yt[iyt1]+=1;
                  }

                  //-> X vs X (symmetry)
                  if (mdoFillEtaEta) {
                      etaeta[icb][ieta2].eta[ieta1]+=1; // nwgt;
                      phiphi[icb][iphi2].phi[iphi1]+=nwgt;
                      nphiphi[icb][iphi2].phi[iphi1]+=1;
                      if (!mdontFillMeanPt) {
                          pretaeta[icb][ieta2].eta[ieta1]+=pwgt/nwgt;
                          paetaeta[icb][ieta2].eta[ieta1]+=spta/nwgt;
                          pbetaeta[icb][ieta2].eta[ieta1]+=sptb/nwgt;
                          prphiphi[icb][iphi2].phi[iphi1]+=pwgt;
                          paphiphi[icb][iphi2].phi[iphi1]+=spta;
                          pbphiphi[icb][iphi2].phi[iphi1]+=sptb;
                      }
                  }
              }
          }

          //-> delta y vs delta x
          ideta = b->ideta(mPair.DeltaEta(mass1,mass2));
          idphi = b->idphi(mPair.DeltaPhi());

          jtdetadphi[icb][ideta].dphi[idphi] +=nwgt;
          if (!mdontFillMeanPt) {
              prjtdetadphi[icb][ideta].dphi[idphi] += pwgt;
              if (switchXX) {
                  pajtdetadphi[icb][ideta].dphi[idphi] += sptb;
                  pbjtdetadphi[icb][ideta].dphi[idphi] += spta;
              } else {
                  pajtdetadphi[icb][ideta].dphi[idphi] += spta;
                  pbjtdetadphi[icb][ideta].dphi[idphi] += sptb;
              }
          }

          //-> Sum y vs delta x
          // For symmetry only reflect around the delta axis.
          // Symmetrization done later.
          if (mdoFillSumHistograms) {
              idyt  = b->idyt(mPair.DeltaYt(mass1,mass2));
              isyt = b->isyt(mPair.SigmaYt(mass1,mass2));
              iseta= b->iseta(mPair.SigmaEta(mass1,mass2));

              atytyt[icb][isyt].dyt[idyt] +=ytwgt;
              atnytyt[icb][isyt].dyt[idyt] +=1;

              jtsetadphi[icb][iseta].dphi[idphi]+=nwgt;
              jtnsetadphi[icb][iseta].dphi[idphi]+=1;
              if (!mdontFillMeanPt) {
                  prjtsetadphi[icb][iseta].dphi[idphi] += pwgt;
                  if (switchXX) {
                      pajtsetadphi[icb][iseta].dphi[idphi] += sptb;
                      pbjtsetadphi[icb][iseta].dphi[idphi] += spta;
                  } else {
                      pajtsetadphi[icb][iseta].dphi[idphi] += spta;
                      pbjtsetadphi[icb][iseta].dphi[idphi] += sptb;
                  }
              }
          }


          qinv[icb].q[b->iq(mPair.qInv())]+=nwgt;
          nqinv[icb].q[b->iq(mPair.qInv())]+=1;

          int jden = cb->getPairDensityBin(jcb);
          int ipcb = -1;
          if (jden >= 0) {
            ipcb = jden*nZBin + iZBin;
          }
          if (mdoPairDensityHistograms && ipcb >= 0) {
            avgtsep[ipcb].sep[isavgt=b->isep(mPair.NominalTpcAvgXYSeparation())]+=nwgt;
            avgzsep[ipcb].sep[isavgz=b->isep(mPair.NominalTpcAvgZSeparation())]+=nwgt;
            float entXYSep = mPair.NominalTpcXYEntranceSeparation();
            enttsep[ipcb].sep[isentt=b->isep(entXYSep)]+=nwgt;
            entzsep[ipcb].sep[isentz=b->isep(mPair.NominalTpcZEntranceSeparation())]+=nwgt;
            float midXYSep = mPair.MidTpcXYSeparation();
            float midZSep = mPair.MidTpcZSeparation();
            midtsep[ipcb].sep[ismidt=b->isep(midXYSep)]+=nwgt;
            midzsep[ipcb].sep[ismidz=b->isep(midZSep)]+=nwgt;
            float exitXYSep = mPair.NominalTpcXYExitSeparation();
            exittsep[ipcb].sep[isexitt=b->isep(exitXYSep)]+=nwgt;
            exitzsep[ipcb].sep[isexitz=b->isep(mPair.NominalTpcZExitSeparation())]+=nwgt;

            avgtz[ipcb][isavgt].sep[isavgz]+=nwgt;
            enttz[ipcb][isentt].sep[isentz]+=nwgt;
            midtz[ipcb][ismidt].sep[ismidz]+=nwgt;
            exittz[ipcb][isexitt].sep[isexitz]+=nwgt;     

            // need to rearrange pair so that deltaPhi>0, avoiding iSwitch and symmetrize for simplicity
            float delpt; // my delta pt
            int idelpt;  // bin index
            double q1;
            if (mPair.Track1()->Phi() - mPair.Track2()->Phi() >= 0) {
              delpt = mPair.Track1()->Pt() - mPair.Track2()->Pt();
              q1 = mPair.mTrack1->Charge();
            } else {  // redefine delta as 2-1
              delpt = mPair.Track2()->Pt() - mPair.Track1()->Pt();
              q1 = mPair.mTrack2->Charge();
            }
            idelpt = b->idpt(delpt);
            enttd[ipcb][isentt].dpt[idelpt]+=nwgt;
            midtd[ipcb][ismidt].dpt[idelpt]+=nwgt;
            exittd[ipcb][isexitt].dpt[idelpt]+=nwgt;
            if (1 == j || 2 == j || 5 == j || 6 == j) { // US
                if (q1>=0) {
                  midtp[ipcb].sep[ismidt]+=nwgt;
                  midzp[ipcb].sep[ismidz]+=nwgt;
                } else {
                  midtn[ipcb].sep[ismidt]+=nwgt;
                  midzn[ipcb].sep[ismidz]+=nwgt;
                }
            } else { // LS
                if (delpt>=0) {
                  midtp[ipcb].sep[ismidt]+=nwgt;
                  midzp[ipcb].sep[ismidz]+=nwgt;
                } else {
                  midtn[ipcb].sep[ismidt]+=nwgt;
                  midzn[ipcb].sep[ismidz]+=nwgt;
                }
            }

            // If tracks cross then the sign of the transverse distance at the endpoints
            // must be opposite. Plot mid separation XY versus Z for the crossing
            // candidates and noc-crossing separately.
            StThreeVectorF ent1 = mPair.Track1()->NominalTpcEntrancePoint();
            StThreeVectorF ent2 = mPair.Track2()->NominalTpcEntrancePoint();
            float sinEnt = (ent1.x()*ent2.y() - ent1.y()*ent2.x());
            StThreeVectorF exit1 = mPair.Track1()->NominalTpcExitPoint();
            StThreeVectorF exit2 = mPair.Track2()->NominalTpcExitPoint();
            float sinExit = (exit1.x()*exit2.y() - exit1.y()*exit2.x());
            if (sinEnt*sinExit < 0) {
                midtzc[ipcb][ismidt].sep[ismidz]+=nwgt;
            } else {
                midtznc[ipcb][ismidt].sep[ismidz]+=nwgt;
            }
          } // pair density
        };// check on if we include pair.
      };// pair cut
    };// iter2 loop
  };// iter 1 loop

  if(mtimer)mtimer->stop();
}


//
//------------ Below are init, delete, write functions -------///
//

//--------------------------------------------------------------------------
void StEStruct2ptCorrelations::fillHistograms() {

  float xv,yv;
  StEStructBinning* b=StEStructBinning::Instance();
  int numCutBins=StEStructCutBin::Instance()->getNumBins();
  int nden=StEStructCutBin::Instance()->getNumPairDensityBins();
  int nzb = 1;
  if (mZBufferCutBinning) {
      nzb = kNumBuffers;
  }

  TH1::AddDirectory(kFALSE);

  for(int i=0; i<8; i++){

    qBins*    qinv    = mQinv[i];
    phiBins** nphiphi = mNPhiPhi[i];
    qBins*    nqinv   = mNQinv[i];

    ytBins**  ytyt;
    ytBins**  nytyt;
    if (!mdontFillYtYt) {
        ytyt   = mYtYt[i];
        nytyt   = mNYtYt[i];
    }

    dphiBins** jtdetadphi  = mJtDEtaDPhi[i];
    dphiBins** prjtdetadphi;
    dphiBins** pajtdetadphi;
    dphiBins** pbjtdetadphi;
    if (!mdontFillMeanPt) {
        prjtdetadphi = mPrJtDEtaDPhi[i];
        pajtdetadphi = mPaJtDEtaDPhi[i];
        pbjtdetadphi = mPbJtDEtaDPhi[i];
    }

    etaBins** etaeta;
    phiBins** phiphi;
    etaBins** pretaeta;
    phiBins** prphiphi;
    etaBins** paetaeta;
    phiBins** paphiphi;
    etaBins** pbetaeta;
    phiBins** pbphiphi;

    if (mdoFillEtaEta) {
        etaeta  = mEtaEta[i];
        phiphi  = mPhiPhi[i];
        if (!mdontFillMeanPt) {
            pretaeta = mPrEtaEta[i];
            prphiphi = mPrPhiPhi[i];
            paetaeta = mPaEtaEta[i];
            paphiphi = mPaPhiPhi[i];
            pbetaeta = mPbEtaEta[i];
            pbphiphi = mPbPhiPhi[i];
        }
    }

    dytBins**  atytyt;
    dytBins**  atnytyt;
    dphiBins** jtsetadphi;
    dphiBins** jtnsetadphi;
    dphiBins** prjtsetadphi;
    dphiBins** pajtsetadphi;
    dphiBins** pbjtsetadphi;
    if (mdoFillSumHistograms) {
        atytyt   = mAtSYtDYt[i];
        atnytyt   = mAtNSYtDYt[i];
        jtsetadphi = mJtSEtaDPhi[i];
        jtnsetadphi = mJtNSEtaDPhi[i];
        if (!mdontFillMeanPt) {
            prjtsetadphi = mPrJtSEtaDPhi[i];
            pajtsetadphi = mPaJtSEtaDPhi[i];
            pbjtsetadphi = mPbJtSEtaDPhi[i];
        }
    }


    for(int y=0;y<numCutBins;y++){
        for (int z=0;z<nzb;z++) {
            int yz = y*nzb + z;

            if (!mdontFillYtYt) {
                createHist2D(mHYtYt,"YtYt",i,y,z,yz,b->ytBins(),b->ytMin(),b->ytMax(),b->ytBins(),b->ytMin(),b->ytMax());
                createHist2D(mHNYtYt,"NYtYt",i,y,z,yz,b->ytBins(),b->ytMin(),b->ytMax(),b->ytBins(),b->ytMin(),b->ytMax());
                for(int k=0;k<b->ytBins();k++){
                  for(int j=0;j<b->ytBins();j++){
                    mHYtYt[i][yz]->Fill(b->ytVal(k),b->ytVal(j),ytyt[yz][k].yt[j]);
                    mHNYtYt[i][yz]->Fill(b->ytVal(k),b->ytVal(j),nytyt[yz][k].yt[j]);
                  }
                }
                delete [] ytyt[yz];
                delete [] nytyt[yz];
            }

            createHist2D(mHJtDEtaDPhi,  "DEtaDPhiArr",i,y,z,yz,b->detaBins(),0.5,b->detaBins()+0.5,b->dphiBins(),0.5,b->dphiBins()+0.5);
            for(int k=0;k<b->detaBins();k++){
              for(int j=0;j<b->dphiBins();j++){
                // Symmetrize dEta,dPhi in StEStructHAdd of Support code.
                // here is just a copy of the array.
                mHJtDEtaDPhi[i][yz]->SetBinContent(k+1,j+1,jtdetadphi[yz][k].dphi[j]);
              }
            }
            delete [] jtdetadphi[yz];
            if (!mdontFillMeanPt) {
                createHist2D(mHPrJtDEtaDPhi,"PrDEtaDPhiArr",i,y,z,yz,b->detaBins(),0.5,b->detaBins()+0.5,b->dphiBins(),0.5,b->dphiBins()+0.5);
                createHist2D(mHPaJtDEtaDPhi,"PaDEtaDPhiArr",i,y,z,yz,b->detaBins(),0.5,b->detaBins()+0.5,b->dphiBins(),0.5,b->dphiBins()+0.5);
                createHist2D(mHPbJtDEtaDPhi,"PbDEtaDPhiArr",i,y,z,yz,b->detaBins(),0.5,b->detaBins()+0.5,b->dphiBins(),0.5,b->dphiBins()+0.5);
                for(int k=0;k<b->detaBins();k++){
                  for(int j=0;j<b->dphiBins();j++){
                    // Symmetrize dEta,dPhi in StEStructHAdd of Support code.
                    // here is just a copy of the array.
                    mHPrJtDEtaDPhi[i][yz]->SetBinContent(k+1,j+1,prjtdetadphi[yz][k].dphi[j]);
                    mHPaJtDEtaDPhi[i][yz]->SetBinContent(k+1,j+1,pajtdetadphi[yz][k].dphi[j]);
                    mHPbJtDEtaDPhi[i][yz]->SetBinContent(k+1,j+1,pbjtdetadphi[yz][k].dphi[j]);
                  }
                }
                delete [] prjtdetadphi[yz];
                delete [] pajtdetadphi[yz];
                delete [] pbjtdetadphi[yz];
            }

            if (mdoFillEtaEta) {
                createHist2D(mHPhiPhi,"PhiPhi",i,y,z,yz,b->phiBins(),b->phiMin(),b->phiMax(),b->phiBins(),b->phiMin(),b->phiMax());
                createHist2D(mHNPhiPhi,"NPhiPhi",i,y,z,yz,b->phiBins(),b->phiMin(),b->phiMax(),b->phiBins(),b->phiMin(),b->phiMax());
                for(int k=0;k<b->phiBins();k++){
                  for(int j=0;j<b->phiBins();j++){
                    mHPhiPhi[i][yz]->Fill(xv=b->phiVal(k),yv=b->phiVal(j),phiphi[yz][k].phi[j]);
                    mHNPhiPhi[i][yz]->Fill(xv,yv,nphiphi[yz][k].phi[j]);
                  }
                }
                delete [] phiphi[yz];
                delete [] nphiphi[yz];
                if (!mdontFillMeanPt) {
                    createHist2D(mHPrPhiPhi,"PrPhiPhi",i,y,z,yz,b->phiBins(),b->phiMin(),b->phiMax(),b->phiBins(),b->phiMin(),b->phiMax());
                    createHist2D(mHPaPhiPhi,"PaPhiPhi",i,y,z,yz,b->phiBins(),b->phiMin(),b->phiMax(),b->phiBins(),b->phiMin(),b->phiMax());
                    createHist2D(mHPbPhiPhi,"PbPhiPhi",i,y,z,yz,b->phiBins(),b->phiMin(),b->phiMax(),b->phiBins(),b->phiMin(),b->phiMax());
                    for(int k=0;k<b->phiBins();k++){
                      for(int j=0;j<b->phiBins();j++){
                        mHPrPhiPhi[i][yz]->Fill(xv=b->phiVal(k),yv=b->phiVal(j),prphiphi[yz][k].phi[j]);
                        mHPaPhiPhi[i][yz]->Fill(xv,yv,paphiphi[yz][k].phi[j]);
                        mHPbPhiPhi[i][yz]->Fill(xv,yv,pbphiphi[yz][k].phi[j]);
                      }
                    }
                    delete [] prphiphi[yz];
                    delete [] paphiphi[yz];
                    delete [] pbphiphi[yz];
                }

                createHist2D(mHEtaEta,"EtaEta",i,y,z,yz,b->etaBins(),b->etaMin(),b->etaMax(),b->etaBins(),b->etaMin(),b->etaMax());
                for(int k=0;k<b->etaBins();k++){
                  for(int j=0;j<b->etaBins();j++){
                    mHEtaEta[i][yz]->Fill(xv=b->etaVal(k),yv=b->etaVal(j),etaeta[yz][k].eta[j]);
                  }
                }
                delete [] etaeta[yz];
                if (!mdontFillMeanPt) {
                    createHist2D(mHPrEtaEta,"PrEtaEta",i,y,z,yz,b->etaBins(),b->etaMin(),b->etaMax(),b->etaBins(),b->etaMin(),b->etaMax());
                    createHist2D(mHPaEtaEta,"PaEtaEta",i,y,z,yz,b->etaBins(),b->etaMin(),b->etaMax(),b->etaBins(),b->etaMin(),b->etaMax());
                    createHist2D(mHPbEtaEta,"PbEtaEta",i,y,z,yz,b->etaBins(),b->etaMin(),b->etaMax(),b->etaBins(),b->etaMin(),b->etaMax());
                    for(int k=0;k<b->etaBins();k++){
                      for(int j=0;j<b->etaBins();j++){
                        mHPrEtaEta[i][yz]->Fill(xv=b->etaVal(k),yv=b->etaVal(j),pretaeta[yz][k].eta[j]);
                        mHPaEtaEta[i][yz]->Fill(xv,yv,paetaeta[yz][k].eta[j]);
                        mHPbEtaEta[i][yz]->Fill(xv,yv,pbetaeta[yz][k].eta[j]);
                      }
                    }
                    delete [] pretaeta[yz];
                    delete [] paetaeta[yz];
                    delete [] pbetaeta[yz];
                }
            }

            if (mdoFillSumHistograms) {
                createHist2D(mHAtSYtDYt, "SYtDYt",i,y,z,yz,b->sytBins(),b->sytMin(),b->sytMax(),b->dytBins(),b->dytMin(),b->dytMax());
                createHist2D(mHAtNSYtDYt,"NSYtDYt",i,y,z,yz,b->sytBins(),b->sytMin(),b->sytMax(),b->dytBins(),b->dytMin(),b->dytMax());
                for(int k=0;k<b->sytBins();k++){
                  for(int j=0;j<b->dytBins();j++){
                    mHAtSYtDYt[i][yz]->Fill(b->sytVal(k),b->dytVal(j),atytyt[yz][k].dyt[j]);
                    mHAtNSYtDYt[i][yz]->Fill(b->sytVal(k),b->dytVal(j),atnytyt[yz][k].dyt[j]);
                  }
                }
                delete [] atytyt[yz];
                delete [] atnytyt[yz];

                createHist2D(mHJtSEtaDPhi,  "SEtaDPhiArr",i,y,z,yz,b->setaBins(),0.5,b->setaBins()+0.5,b->dphiBins(),0.5,b->dphiBins()+0.5);
                createHist2D(mHJtNSEtaDPhi, "NSEtaDPhiArr",i,y,z,yz,b->setaBins(),0.5,b->setaBins()+0.5,b->dphiBins(),0.5,b->dphiBins()+0.5);
                for(int k=0;k<b->setaBins();k++) {
                  for(int j=0;j<b->dphiBins();j++) {
                    mHJtSEtaDPhi[i][yz]->SetBinContent(k+1,j+1,jtsetadphi[yz][k].dphi[j]);
                    mHJtNSEtaDPhi[i][yz]->SetBinContent(k+1,j+1,jtnsetadphi[yz][k].dphi[j]);
                  }
                }
                delete [] jtsetadphi[yz];
                delete [] jtnsetadphi[yz];
                if (!mdontFillMeanPt) {
                    createHist2D(mHPrJtSEtaDPhi,"PrSEtaDPhiArr",i,y,z,yz,b->setaBins(),0.5,b->setaBins()+0.5,b->dphiBins(),0.5,b->dphiBins()+0.5);
                    createHist2D(mHPaJtSEtaDPhi,"PaSEtaDPhiArr",i,y,z,yz,b->setaBins(),0.5,b->setaBins()+0.5,b->dphiBins(),0.5,b->dphiBins()+0.5);
                    createHist2D(mHPbJtSEtaDPhi,"PbSEtaDPhiArr",i,y,z,yz,b->setaBins(),0.5,b->setaBins()+0.5,b->dphiBins(),0.5,b->dphiBins()+0.5);
                    for(int k=0;k<b->setaBins();k++) {
                      for(int j=0;j<b->dphiBins();j++) {
                        mHPrJtSEtaDPhi[i][yz]->SetBinContent(k+1,j+1,prjtsetadphi[yz][k].dphi[j]);
                        mHPaJtSEtaDPhi[i][yz]->SetBinContent(k+1,j+1,pajtsetadphi[yz][k].dphi[j]);
                        mHPbJtSEtaDPhi[i][yz]->SetBinContent(k+1,j+1,pbjtsetadphi[yz][k].dphi[j]);
                      }
                    }
                    delete [] prjtsetadphi[yz];
                    delete [] pbjtsetadphi[yz];
                    delete [] pajtsetadphi[yz];
                }
            }

            createHist1D(mHQinv,"Qinv",i,y,z,yz,b->qBins(),b->qMin(),b->qMax());
            createHist1D(mHNQinv,"NQinv",i,y,z,yz,b->qBins(),b->qMin(),b->qMax());
            for(int k=0;k<b->qBins();k++){
                mHQinv[i][yz]->Fill(b->qVal(k),qinv[yz].q[k]);
                mHNQinv[i][yz]->Fill(b->qVal(k),nqinv[yz].q[k]);
            }
            //    delete [] qinv[yz];

        } // for z
    } // for y
  } // for i

  if(mdoPairDensityHistograms) {
    for(int i=0; i<8; i++){
      TPCSepBins* avgtsep = mTPCAvgTSep[i];
      TPCSepBins* avgzsep = mTPCAvgZSep[i];
      TPCSepBins* enttsep = mTPCEntTSep[i];
      TPCSepBins* entzsep = mTPCEntZSep[i];
      TPCSepBins* midtsep = mTPCMidTSep[i];
      TPCSepBins* midzsep = mTPCMidZSep[i];
      TPCSepBins* exittsep= mTPCExitTSep[i];
      TPCSepBins* exitzsep= mTPCExitZSep[i];
      TPCSepBins* midtp =   mTPCMidTdptP[i];
      TPCSepBins* midtn =   mTPCMidTdptN[i];
      TPCSepBins* midzp =   mTPCMidZdptP[i];
      TPCSepBins* midzn =   mTPCMidZdptN[i];
      TPCSepBins** avgtz =  mTPCAvgTZ[i];
      TPCSepBins** enttz =  mTPCEntTZ[i];
      TPCSepBins** midtz   = mTPCMidTZ[i];
      TPCSepBins** midtzc  = mTPCMidTZC[i];
      TPCSepBins** midtznc = mTPCMidTZNC[i];
      TPCSepBins** exittz = mTPCExitTZ[i];
      dptBins** enttd =  mTPCEntTdpt[i];
      dptBins** midtd =  mTPCMidTdpt[i];
      dptBins** exittd =  mTPCExitTdpt[i];

      for(int y=0;y<nden;y++){
        for (int z=0;z<nzb;z++) {
          int yz = y*nzb + z;

          createHist1D(mHTPCAvgTSep,"TPCAvgTSep",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCAvgZSep,"TPCAvgZSep",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCEntTSep,"TPCEntTSep",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCEntZSep,"TPCEntZSep",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCMidTSep,"TPCMidTSep",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCMidZSep,"TPCMidZSep",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCExitTSep,"TPCExitTSep",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCExitZSep,"TPCExitZSep",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCMidTdptP,"TPCMidTdptP",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCMidTdptN,"TPCMidTdptN",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCMidZdptP,"TPCMidZdptP",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist1D(mHTPCMidZdptN,"TPCMidZdptN",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          for(int k=0;k<b->TPCSepBins();k++) {
            mHTPCAvgTSep[i][yz]->Fill(xv=b->sepVal(k),avgtsep[yz].sep[k]);
            mHTPCAvgZSep[i][yz]->Fill(xv,avgzsep[yz].sep[k]);
            mHTPCEntTSep[i][yz]->Fill(xv,enttsep[yz].sep[k]);
            mHTPCEntZSep[i][yz]->Fill(xv,entzsep[yz].sep[k]);
            mHTPCMidTSep[i][yz]->Fill(xv,midtsep[yz].sep[k]);
            mHTPCMidZSep[i][yz]->Fill(xv,midzsep[yz].sep[k]);
            mHTPCExitTSep[i][yz]->Fill(xv,exittsep[yz].sep[k]);
            mHTPCExitZSep[i][yz]->Fill(xv,exitzsep[yz].sep[k]);
            mHTPCMidTdptP[i][yz]->Fill(xv,midtp[yz].sep[k]);
            mHTPCMidTdptN[i][yz]->Fill(xv,midtn[yz].sep[k]);
            mHTPCMidZdptP[i][yz]->Fill(xv,midzp[yz].sep[k]);
            mHTPCMidZdptN[i][yz]->Fill(xv,midzn[yz].sep[k]);
          }
/*
 * I think this is the right way to delete these 1D arrays, but until I can test it
 * I will live with possibly using a little extra memory.
          delete avgtsep[yz];
          delete avgzsep[yz];
          delete enttsep[yz];
          delete entzsep[yz];
          delete midtsep[yz];
          delete midzsep[yz];
          delete exittsep[yz];
          delete exitzsep[yz];
          delete midtpsep[yz];
          delete midtnsep[yz];
          delete midzpsep[yz];
          delete midznsep[yz];
 */
          createHist2D(mHTPCAvgTZ, "TPCAvgTZ", i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax(),b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist2D(mHTPCEntTZ, "TPCEntTZ", i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax(),b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist2D(mHTPCMidTZ, "TPCMidTZ", i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax(),b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist2D(mHTPCMidTZC, "TPCMidTZC", i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax(),b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist2D(mHTPCMidTZNC, "TPCMidTZNC", i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax(),b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          createHist2D(mHTPCExitTZ,"TPCExitTZ",i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax(),b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax());
          for(int k=0;k<b->TPCSepBins();k++) {
            for(int j=0;j<b->TPCSepBins();j++) {
              mHTPCAvgTZ[i][yz]->Fill(xv=b->sepVal(k),yv=b->sepVal(j),avgtz[yz][k].sep[j]);
              mHTPCEntTZ[i][yz]->Fill(xv,yv,enttz[yz][k].sep[j]);
              mHTPCMidTZ[i][yz]->Fill(xv,yv,midtz[yz][k].sep[j]);
              mHTPCMidTZC[i][yz]->Fill(xv,yv,midtzc[yz][k].sep[j]);
              mHTPCMidTZNC[i][yz]->Fill(xv,yv,midtznc[yz][k].sep[j]);
              mHTPCExitTZ[i][yz]->Fill(xv,yv,exittz[yz][k].sep[j]);
            }
          }
          delete [] avgtz[yz];
          delete [] enttz[yz];
          delete [] midtz[yz];
          delete [] midtzc[yz];
          delete [] midtznc[yz];
          delete [] exittz[yz];

          createHist2D(mHTPCEntTdpt, "TPCEntTdpt", i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax(),b->dptBins(),b->dptMin(),b->dptMax());
          createHist2D(mHTPCMidTdpt, "TPCMidTdpt", i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax(),b->dptBins(),b->dptMin(),b->dptMax());
          createHist2D(mHTPCExitTdpt, "TPCExitTdpt", i,y,z,yz,b->TPCSepBins(),b->TPCSepMin(),b->TPCSepMax(),b->dptBins(),b->dptMin(),b->dptMax());
          for(int k=0;k<b->TPCSepBins();k++) {
            for(int j=0;j<b->dptBins();j++) {
              mHTPCEntTdpt[i][yz]->Fill(xv=b->sepVal(k),yv=b->dptVal(j),enttd[yz][k].dpt[j]);
              mHTPCMidTdpt[i][yz]->Fill(xv,yv,midtd[yz][k].dpt[j]);
              mHTPCExitTdpt[i][yz]->Fill(xv,yv,exittd[yz][k].dpt[j]);
            }
          }
          delete [] enttd[yz];
          delete [] midtd[yz];
          delete [] exittd[yz];
        }
      }
    }
  } // if pair density

}

//--------------------------------------------------------------------------
void StEStruct2ptCorrelations::writeHistograms() {

  int numCutBins=StEStructCutBin::Instance()->getNumBins();
  int nden=StEStructCutBin::Instance()->getNumPairDensityBins();
  int numParentBins=StEStructCutBin::Instance()->getNumParentBins();
  int nZBins = 1;
  if (mZBufferCutBinning) {
      numCutBins *= kNumBuffers;
      nden *= kNumBuffers;
      numParentBins *= kNumBuffers;
      nZBins = kNumBuffers;
  }
  for (int j=0;j<nZBins;j++) {
      mHNEventsSib[j]->Write();
      mHNEventsMix[j]->Write();
      mHNEventsPosSib[j]->Write();
      mHNEventsPosMix[j]->Write();
      mHNEventsNegSib[j]->Write();
      mHNEventsNegMix[j]->Write();
  }
  mHmix->Write();
  mHcb->Write();

  mHptAll->Write();
  for(int j=0;j<numParentBins;j++){
     mHMeanPtP[j]->Write();
     mHMeanPtM[j]->Write();
     mHMeanYtP[j]->Write();
     mHMeanYtM[j]->Write();
     mHEtaP[j]->Write();
     mHEtaM[j]->Write();
  }
  for(int i=0;i<8;i++){
    for(int j=0;j<numCutBins;j++){
      if (!mdontFillYtYt) {
          mHYtYt[i][j]->Write();
          mHNYtYt[i][j]->Write();
      }

      mHJtDEtaDPhi[i][j]->Write();
      if (!mdontFillMeanPt) {
          mHPrJtDEtaDPhi[i][j]->Write();
          mHPaJtDEtaDPhi[i][j]->Write();
          mHPbJtDEtaDPhi[i][j]->Write();
      }

      if (mdoFillEtaEta) {
          mHPhiPhi[i][j]->Write();
          mHNPhiPhi[i][j]->Write();
          mHEtaEta[i][j]->Write();
          if (!mdontFillMeanPt) {
              mHPrPhiPhi[i][j]->Write();
              mHPrEtaEta[i][j]->Write();
              mHPaPhiPhi[i][j]->Write();
              mHPaEtaEta[i][j]->Write();
              mHPbPhiPhi[i][j]->Write();
              mHPbEtaEta[i][j]->Write();
          }
      }

      if (mdoFillSumHistograms) {
          mHAtSYtDYt[i][j]->Write();
          mHAtNSYtDYt[i][j]->Write();
          mHJtSEtaDPhi[i][j]->Write();
          mHJtNSEtaDPhi[i][j]->Write();
          if (!mdontFillMeanPt) {
              mHPrJtSEtaDPhi[i][j]->Write();
              mHPaJtSEtaDPhi[i][j]->Write();
              mHPbJtSEtaDPhi[i][j]->Write();
          }
      }

      mHQinv[i][j]->Write();
      mHNQinv[i][j]->Write();
    }

    if(mdoPairDensityHistograms) {
      for (int j=0;j<nden;j++) {
        mHTPCAvgTSep[i][j]->Write();
        mHTPCAvgZSep[i][j]->Write();
        mHTPCEntTSep[i][j]->Write();
        mHTPCEntZSep[i][j]->Write();
        mHTPCMidTSep[i][j]->Write();
        mHTPCMidZSep[i][j]->Write();
        mHTPCExitTSep[i][j]->Write();
        mHTPCExitZSep[i][j]->Write();

        mHTPCMidTdptP[i][j]->Write();
        mHTPCMidTdptN[i][j]->Write();
        mHTPCMidZdptP[i][j]->Write();
        mHTPCMidZdptN[i][j]->Write();

        mHTPCAvgTZ[i][j]->Write();
        mHTPCEntTZ[i][j]->Write();
        mHTPCMidTZ[i][j]->Write();
        mHTPCMidTZC[i][j]->Write();
        mHTPCMidTZNC[i][j]->Write();
        mHTPCExitTZ[i][j]->Write();
        mHTPCEntTdpt[i][j]->Write();
        mHTPCMidTdpt[i][j]->Write();
        mHTPCExitTdpt[i][j]->Write();
      }
    }
  }
}

//--------------------------------------------------------------------------
void StEStruct2ptCorrelations::writeQAHists(TFile *qtf) {

  if(!mlocalQAHists) return;

  if(!qtf){
    cout<<" NO QA OUTPUT TFile TO WRITE TO ... giving up ..."<<endl;
    return;
  }

  mQAHists->writeTrackHistograms(qtf);


}

//--------------------------------------------------------------------------
void StEStruct2ptCorrelations::initArrays(){

  int numCutBins=StEStructCutBin::Instance()->getNumBins();
  int nden=StEStructCutBin::Instance()->getNumPairDensityBins();
  if (mZBufferCutBinning) {
      numCutBins *= kNumBuffers;
      nden *= kNumBuffers;
  }

  for(int i=0;i<8;i++){

     if (!mdontFillYtYt) {
         mYtYt[i]=new ytBins*[numCutBins];
         mNYtYt[i]=new ytBins*[numCutBins];
     }

     mJtDEtaDPhi[i]  =new dphiBins*[numCutBins];
     if (!mdontFillMeanPt) {
         mPaJtDEtaDPhi[i]=new dphiBins*[numCutBins];
         mPbJtDEtaDPhi[i]=new dphiBins*[numCutBins];
         mPrJtDEtaDPhi[i]=new dphiBins*[numCutBins];
     }
     
     if (mdoFillEtaEta) {
         mEtaEta[i]=new etaBins*[numCutBins];
         mPhiPhi[i]=new phiBins*[numCutBins];
         mNPhiPhi[i]=new phiBins*[numCutBins];
         if (!mdontFillMeanPt) {
             mPrEtaEta[i]=new etaBins*[numCutBins];
             mPaEtaEta[i]=new etaBins*[numCutBins];
             mPbEtaEta[i]=new etaBins*[numCutBins];
             mPrPhiPhi[i]=new phiBins*[numCutBins];
             mPaPhiPhi[i]=new phiBins*[numCutBins];
             mPbPhiPhi[i]=new phiBins*[numCutBins];
         }
     }

     if (mdoFillSumHistograms) {
         mAtSYtDYt[i]=new dytBins*[numCutBins];
         mAtNSYtDYt[i]=new dytBins*[numCutBins];
         mJtSEtaDPhi[i]=new dphiBins*[numCutBins];
         mJtNSEtaDPhi[i]=new dphiBins*[numCutBins];
         if (!mdontFillMeanPt) {
             mPaJtSEtaDPhi[i]=new dphiBins*[numCutBins];
             mPbJtSEtaDPhi[i]=new dphiBins*[numCutBins];
             mPrJtSEtaDPhi[i]=new dphiBins*[numCutBins];
         }
     }

     /*  --- I cut out the ql,qo,qs
     if(mPair.doHbt3D()){
       mQlQs[i]=new qBins*[numCutBins];
       mQoQop[i]=new qBins*[numCutBins];
     }
     */
     mQinv[i]=new qBins[numCutBins];
     mNQinv[i]=new qBins[numCutBins];
     memset(mQinv[i],0,numCutBins*sizeof(qBins)); // do the memset here
     memset(mNQinv[i],0,numCutBins*sizeof(qBins)); // do the memset here

     if(mdoPairDensityHistograms) {     
       mTPCAvgTSep[i]=new TPCSepBins[nden];  //1D  
       mTPCAvgZSep[i]=new TPCSepBins[nden];
       mTPCEntTSep[i]=new TPCSepBins[nden];
       mTPCEntZSep[i]=new TPCSepBins[nden];
       mTPCMidTSep[i]=new TPCSepBins[nden];
       mTPCMidZSep[i]=new TPCSepBins[nden];
       mTPCExitTSep[i]=new TPCSepBins[nden];
       mTPCExitZSep[i]=new TPCSepBins[nden];
       mTPCMidTdptP[i]=new TPCSepBins[nden];
       mTPCMidTdptN[i]=new TPCSepBins[nden];
       mTPCMidZdptP[i]=new TPCSepBins[nden];
       mTPCMidZdptN[i]=new TPCSepBins[nden];
       memset(mTPCAvgTSep[i], 0,nden*sizeof(TPCSepBins)); 
       memset(mTPCAvgZSep[i], 0,nden*sizeof(TPCSepBins));
       memset(mTPCEntTSep[i], 0,nden*sizeof(TPCSepBins)); 
       memset(mTPCEntZSep[i], 0,nden*sizeof(TPCSepBins));
       memset(mTPCMidTSep[i], 0,nden*sizeof(TPCSepBins)); 
       memset(mTPCMidZSep[i], 0,nden*sizeof(TPCSepBins));
       memset(mTPCExitTSep[i], 0,nden*sizeof(TPCSepBins)); 
       memset(mTPCExitZSep[i], 0,nden*sizeof(TPCSepBins));
       memset(mTPCMidTdptP[i],0,nden*sizeof(TPCSepBins));
       memset(mTPCMidTdptN[i],0,nden*sizeof(TPCSepBins));
       memset(mTPCMidZdptP[i],0,nden*sizeof(TPCSepBins));
       memset(mTPCMidZdptN[i],0,nden*sizeof(TPCSepBins));

       mTPCAvgTZ[i]=new TPCSepBins*[nden];  //2D
       mTPCEntTZ[i]=new TPCSepBins*[nden];  
       mTPCMidTZ[i]=new TPCSepBins*[nden];  
       mTPCMidTZC[i]=new TPCSepBins*[nden];  
       mTPCMidTZNC[i]=new TPCSepBins*[nden];  
       mTPCExitTZ[i]=new TPCSepBins*[nden]; 
       mTPCEntTdpt[i]=new dptBins*[nden];
       mTPCMidTdpt[i]=new dptBins*[nden];
       mTPCExitTdpt[i]=new dptBins*[nden];  // Initialization to 0 done later.
     }


    for(int j=0;j<numCutBins;j++){
      if (!mdontFillYtYt) {
          mYtYt[i][j]=new ytBins[ESTRUCT_YT_BINS];
          memset(mYtYt[i][j],0,ESTRUCT_YT_BINS*sizeof(ytBins));
          mNYtYt[i][j]=new ytBins[ESTRUCT_YT_BINS];
          memset(mNYtYt[i][j],0,ESTRUCT_YT_BINS*sizeof(ytBins));
      }

      mJtDEtaDPhi[i][j]=new dphiBins[ESTRUCT_DETA_BINS];
      memset(mJtDEtaDPhi[i][j],0,ESTRUCT_DETA_BINS*sizeof(dphiBins));
      if (!mdontFillMeanPt) {
          mPaJtDEtaDPhi[i][j]=new dphiBins[ESTRUCT_DETA_BINS];
          memset(mPaJtDEtaDPhi[i][j],0,ESTRUCT_DETA_BINS*sizeof(dphiBins));
          mPbJtDEtaDPhi[i][j]=new dphiBins[ESTRUCT_DETA_BINS];
          memset(mPbJtDEtaDPhi[i][j],0,ESTRUCT_DETA_BINS*sizeof(dphiBins));
          mPrJtDEtaDPhi[i][j]=new dphiBins[ESTRUCT_DETA_BINS];
          memset(mPrJtDEtaDPhi[i][j],0,ESTRUCT_DETA_BINS*sizeof(dphiBins));
      }

      if (mdoFillEtaEta) {
          mEtaEta[i][j]=new etaBins[ESTRUCT_ETA_BINS];
          memset(mEtaEta[i][j],0,ESTRUCT_ETA_BINS*sizeof(etaBins));
          mPhiPhi[i][j]=new phiBins[ESTRUCT_PHI_BINS];
          memset(mPhiPhi[i][j],0,ESTRUCT_PHI_BINS*sizeof(phiBins));
          mNPhiPhi[i][j]=new phiBins[ESTRUCT_PHI_BINS];
          memset(mNPhiPhi[i][j],0,ESTRUCT_PHI_BINS*sizeof(phiBins));
          if (!mdontFillMeanPt) {
              mPrEtaEta[i][j]=new etaBins[ESTRUCT_ETA_BINS];
              memset(mPrEtaEta[i][j],0,ESTRUCT_ETA_BINS*sizeof(etaBins));
              mPaEtaEta[i][j]=new etaBins[ESTRUCT_ETA_BINS];
              memset(mPaEtaEta[i][j],0,ESTRUCT_ETA_BINS*sizeof(etaBins));
              mPbEtaEta[i][j]=new etaBins[ESTRUCT_ETA_BINS];
              memset(mPbEtaEta[i][j],0,ESTRUCT_ETA_BINS*sizeof(etaBins));
              mPrPhiPhi[i][j]=new phiBins[ESTRUCT_PHI_BINS];
              memset(mPrPhiPhi[i][j],0,ESTRUCT_PHI_BINS*sizeof(phiBins));
              mPaPhiPhi[i][j]=new phiBins[ESTRUCT_PHI_BINS];
              memset(mPaPhiPhi[i][j],0,ESTRUCT_PHI_BINS*sizeof(phiBins));
              mPbPhiPhi[i][j]=new phiBins[ESTRUCT_PHI_BINS];
              memset(mPbPhiPhi[i][j],0,ESTRUCT_PHI_BINS*sizeof(phiBins));
          }
      }

      if (mdoFillSumHistograms) {
          mAtSYtDYt[i][j]=new dytBins[ESTRUCT_SYT_BINS];
          memset(mAtSYtDYt[i][j],0,ESTRUCT_SYT_BINS*sizeof(dytBins));
          mAtNSYtDYt[i][j]=new dytBins[ESTRUCT_SYT_BINS];
          memset(mAtNSYtDYt[i][j],0,ESTRUCT_SYT_BINS*sizeof(dytBins));
          mJtSEtaDPhi[i][j]=new dphiBins[ESTRUCT_SETA_BINS];
          memset(mJtSEtaDPhi[i][j],0,ESTRUCT_SETA_BINS*sizeof(dphiBins));
          mJtNSEtaDPhi[i][j]=new dphiBins[ESTRUCT_SETA_BINS];
          memset(mJtNSEtaDPhi[i][j],0,ESTRUCT_SETA_BINS*sizeof(dphiBins));
          if (!mdontFillMeanPt) {
              mPrJtSEtaDPhi[i][j]=new dphiBins[ESTRUCT_SETA_BINS];
              memset(mPrJtSEtaDPhi[i][j],0,ESTRUCT_SETA_BINS*sizeof(dphiBins));
              mPaJtSEtaDPhi[i][j]=new dphiBins[ESTRUCT_SETA_BINS];
              memset(mPaJtSEtaDPhi[i][j],0,ESTRUCT_SETA_BINS*sizeof(dphiBins));
              mPbJtSEtaDPhi[i][j]=new dphiBins[ESTRUCT_SETA_BINS];
              memset(mPbJtSEtaDPhi[i][j],0,ESTRUCT_SETA_BINS*sizeof(dphiBins));
          }
      }
    }

    if(mdoPairDensityHistograms) {
      for(int j=0;j<nden;j++){
        mTPCAvgTZ[i][j]=new TPCSepBins[ESTRUCT_TPCSEP_BINS];
        memset(mTPCAvgTZ[i][j],0,ESTRUCT_TPCSEP_BINS*sizeof(TPCSepBins));
        mTPCEntTZ[i][j]=new TPCSepBins[ESTRUCT_TPCSEP_BINS];
        memset(mTPCEntTZ[i][j],0,ESTRUCT_TPCSEP_BINS*sizeof(TPCSepBins));
        mTPCMidTZ[i][j]=new TPCSepBins[ESTRUCT_TPCSEP_BINS];
        memset(mTPCMidTZ[i][j],0,ESTRUCT_TPCSEP_BINS*sizeof(TPCSepBins));
        mTPCMidTZC[i][j]=new TPCSepBins[ESTRUCT_TPCSEP_BINS];
        memset(mTPCMidTZC[i][j],0,ESTRUCT_TPCSEP_BINS*sizeof(TPCSepBins));
        mTPCMidTZNC[i][j]=new TPCSepBins[ESTRUCT_TPCSEP_BINS];
        memset(mTPCMidTZNC[i][j],0,ESTRUCT_TPCSEP_BINS*sizeof(TPCSepBins));
        mTPCExitTZ[i][j]=new TPCSepBins[ESTRUCT_TPCSEP_BINS];
        memset(mTPCExitTZ[i][j],0,ESTRUCT_TPCSEP_BINS*sizeof(TPCSepBins));
       
        mTPCEntTdpt[i][j]=new dptBins[ESTRUCT_TPCSEP_BINS];
        memset(mTPCEntTdpt[i][j],0,ESTRUCT_TPCSEP_BINS*sizeof(dptBins));
        mTPCMidTdpt[i][j]=new dptBins[ESTRUCT_TPCSEP_BINS];
        memset(mTPCMidTdpt[i][j],0,ESTRUCT_TPCSEP_BINS*sizeof(dptBins));
        mTPCExitTdpt[i][j]=new dptBins[ESTRUCT_TPCSEP_BINS];
        memset(mTPCExitTdpt[i][j],0,ESTRUCT_TPCSEP_BINS*sizeof(dptBins));
      }
     
    }
  }
}

//--------------------------------------------------------------------------
void StEStruct2ptCorrelations::deleteArrays(){

  int numCutBins=StEStructCutBin::Instance()->getNumBins();
  int nden=StEStructCutBin::Instance()->getNumPairDensityBins();
  if (mZBufferCutBinning) {
      numCutBins *= kNumBuffers;
      nden *= kNumBuffers;
  }

  for(int i=0;i<8;i++){
    for(int j=0;j<numCutBins;j++){ // Why were these all commented out?
      
      if (!mdontFillYtYt) {
          delete []  mYtYt[i][j];
          delete []  mNYtYt[i][j];
      }

      delete []  mJtDEtaDPhi[i][j];
      if (!mdontFillMeanPt) {
          delete []  mPrJtDEtaDPhi[i][j];
          delete []  mPaJtDEtaDPhi[i][j];
          delete []  mPbJtDEtaDPhi[i][j];
      }

      if (mdoFillEtaEta) {
          delete []  mEtaEta[i][j];
          delete []  mPhiPhi[i][j];
          delete []  mNPhiPhi[i][j];
          if (!mdontFillMeanPt) {
              delete []  mPrEtaEta[i][j];
              delete []  mPrPhiPhi[i][j];
              delete []  mPaEtaEta[i][j];
              delete []  mPaPhiPhi[i][j];
              delete []  mPbEtaEta[i][j];
              delete []  mPbPhiPhi[i][j];
          }
      }
      
      if (mdoFillSumHistograms) {
          delete []  mAtSYtDYt[i][j];
          delete []  mJtSEtaDPhi[i][j];
          delete []  mJtNSEtaDPhi[i][j];
          if (!mdontFillMeanPt) {
              delete []  mPrJtSEtaDPhi[i][j];
              delete []  mPaJtSEtaDPhi[i][j];
              delete []  mPbJtSEtaDPhi[i][j];
          }
      }
      
    }
      
    if(mdoPairDensityHistograms)  { // These are the 2D arrays. Delete along one axis.
      for(int j=0;j<nden;j++){
        delete []  mTPCAvgTZ[i][j];
        delete []  mTPCEntTZ[i][j];
        delete []  mTPCMidTZ[i][j];
        delete []  mTPCMidTZC[i][j];
        delete []  mTPCMidTZNC[i][j];
        delete []  mTPCExitTZ[i][j];
        delete []  mTPCEntTdpt[i][j];
        delete []  mTPCMidTdpt[i][j];
        delete []  mTPCExitTdpt[i][j];
      }
    }
    
    if (!mdontFillYtYt) {
        delete []  mYtYt[i];
        delete []  mNYtYt[i];
    }

    delete []  mJtDEtaDPhi[i];
    if (!mdontFillMeanPt) {
        delete []  mPrJtDEtaDPhi[i];
        delete []  mPaJtDEtaDPhi[i];
        delete []  mPbJtDEtaDPhi[i];
    }

    if (mdoFillEtaEta) {
        delete []  mEtaEta[i];
        delete []  mPhiPhi[i];
        delete []  mNPhiPhi[i];
        if (!mdontFillMeanPt) {
            delete []  mPrEtaEta[i];
            delete []  mPrPhiPhi[i];
            delete []  mPaEtaEta[i];
            delete []  mPaPhiPhi[i];
            delete []  mPbEtaEta[i];
            delete []  mPbPhiPhi[i];
        }
    }

    if (mdoFillSumHistograms) {
        delete []  mAtSYtDYt[i];
        delete []  mJtSEtaDPhi[i];
        delete []  mJtNSEtaDPhi[i];
        if (!mdontFillMeanPt) {
            delete []  mPrJtSEtaDPhi[i];
            delete []  mPaJtSEtaDPhi[i];
            delete []  mPbJtSEtaDPhi[i];
        }
    }
    
    delete []  mQinv[i];
    delete []  mNQinv[i];

    if(mdoPairDensityHistograms) {
      delete [] mTPCAvgTSep[i];
      delete [] mTPCAvgZSep[i];
      delete [] mTPCEntTSep[i];
      delete [] mTPCEntZSep[i];
      delete [] mTPCMidTSep[i];
      delete [] mTPCMidZSep[i];
      delete [] mTPCExitTSep[i];
      delete [] mTPCExitZSep[i];
      delete [] mTPCMidTdptP[i];
      delete [] mTPCMidTdptN[i];
      delete [] mTPCMidZdptP[i];
      delete [] mTPCMidZdptN[i];
      delete [] mTPCAvgTZ[i];
      delete [] mTPCEntTZ[i];
      delete [] mTPCMidTZ[i];
      delete [] mTPCMidTZC[i];
      delete [] mTPCMidTZNC[i];
      delete [] mTPCExitTZ[i];
      delete [] mTPCEntTdpt[i];
      delete [] mTPCMidTdpt[i];
      delete [] mTPCExitTdpt[i]; 
    }
  }

}

//--------------------------------------------------------------------------

void StEStruct2ptCorrelations::initHistograms(){

  int numCutBins=StEStructCutBin::Instance()->getNumBins();
  int nden=StEStructCutBin::Instance()->getNumPairDensityBins();
  int numParentBins=StEStructCutBin::Instance()->getNumParentBins();
  if (mZBufferCutBinning) {
      numCutBins *= kNumBuffers;
      nden *= kNumBuffers;
      numParentBins *= kNumBuffers;
  }

  for(int i=0; i<8; i++){

    if (!mdontFillYtYt) {
        mHYtYt[i]=new TH2D*[numCutBins];
        mHNYtYt[i]=new TH2D*[numCutBins];
    }

    mHJtDEtaDPhi[i]=new TH2D*[numCutBins];
    if (!mdontFillMeanPt) {
        mHPrJtDEtaDPhi[i]=new TH2D*[numCutBins];
        mHPaJtDEtaDPhi[i]=new TH2D*[numCutBins];
        mHPbJtDEtaDPhi[i]=new TH2D*[numCutBins];
    }

    if (mdoFillEtaEta) {
        mHEtaEta[i]=new TH2D*[numCutBins];
        mHPhiPhi[i]=new TH2D*[numCutBins];
        mHNPhiPhi[i]=new TH2D*[numCutBins];
        if (!mdontFillMeanPt) {
            mHPrEtaEta[i]=new TH2D*[numCutBins];
            mHPrPhiPhi[i]=new TH2D*[numCutBins];
            mHPaEtaEta[i]=new TH2D*[numCutBins];
            mHPaPhiPhi[i]=new TH2D*[numCutBins];
            mHPbEtaEta[i]=new TH2D*[numCutBins];
            mHPbPhiPhi[i]=new TH2D*[numCutBins];
        }
    }

    if (mdoFillSumHistograms) {
        mHAtSYtDYt[i]=new TH2D*[numCutBins];
        mHAtNSYtDYt[i]=new TH2D*[numCutBins];
        mHJtSEtaDPhi[i]=new TH2D*[numCutBins];
        mHJtNSEtaDPhi[i]=new TH2D*[numCutBins];
        if (!mdontFillMeanPt) {
            mHPrJtSEtaDPhi[i]=new TH2D*[numCutBins];
            mHPaJtSEtaDPhi[i]=new TH2D*[numCutBins];
            mHPbJtSEtaDPhi[i]=new TH2D*[numCutBins];
        }
    }

    mHQinv[i]=new TH1D*[numCutBins];
    mHNQinv[i]=new TH1D*[numCutBins];

    if(mdoPairDensityHistograms) {
      mHTPCAvgTSep[i]=new TH1D*[nden];
      mHTPCAvgZSep[i]=new TH1D*[nden];
      mHTPCEntTSep[i]=new TH1D*[nden];
      mHTPCEntZSep[i]=new TH1D*[nden];
      mHTPCMidTSep[i]=new TH1D*[nden];
      mHTPCMidZSep[i]=new TH1D*[nden];
      mHTPCExitTSep[i]=new TH1D*[nden];
      mHTPCExitZSep[i]=new TH1D*[nden];
      mHTPCMidTdptP[i]=new TH1D*[nden];
      mHTPCMidTdptN[i]=new TH1D*[nden];
      mHTPCMidZdptP[i]=new TH1D*[nden];
      mHTPCMidZdptN[i]=new TH1D*[nden];
      mHTPCAvgTZ[i]=new TH2D*[nden];
      mHTPCEntTZ[i]=new TH2D*[nden];
      mHTPCMidTZ[i]=new TH2D*[nden];
      mHTPCMidTZC[i]=new TH2D*[nden];
      mHTPCMidTZNC[i]=new TH2D*[nden];
      mHTPCExitTZ[i]=new TH2D*[nden];
      mHTPCEntTdpt[i]=new TH2D*[nden];
      mHTPCMidTdpt[i]=new TH2D*[nden];
      mHTPCExitTdpt[i]=new TH2D*[nden];
    }
    
  }
}

//--------------------------------------------------------------------------
void StEStruct2ptCorrelations::deleteHistograms(){
  
  int numCutBins=StEStructCutBin::Instance()->getNumBins();
  int nden=StEStructCutBin::Instance()->getNumPairDensityBins();
  int numParentBins=StEStructCutBin::Instance()->getNumParentBins();
  if (mZBufferCutBinning) {
      numCutBins *= kNumBuffers;
      nden *= kNumBuffers;
      numParentBins *= kNumBuffers;
  }

  for(int j=0;j<numParentBins;j++){
    delete mHMeanPtP[j];
    delete mHMeanPtM[j];
    delete mHMeanYtP[j];
    delete mHMeanYtM[j];
    delete mHEtaP[j];
    delete mHEtaM[j];
  }
  for(int i=0;i<8;i++){
    for(int j=0;j<numCutBins;j++){
      if (!mdontFillYtYt) {
          delete mHYtYt[i][j];
          delete mHNYtYt[i][j];
      }
      
      delete mHJtDEtaDPhi[i][j];
      if (!mdontFillMeanPt) {
          delete mHPrJtDEtaDPhi[i][j];
          delete mHPaJtDEtaDPhi[i][j];
          delete mHPbJtDEtaDPhi[i][j];
      }

      if (mdoFillEtaEta) {
          delete mHEtaEta[i][j];
          delete mHPhiPhi[i][j];
          delete mHNPhiPhi[i][j];
          if (!mdontFillMeanPt) {
              delete mHPrEtaEta[i][j];
              delete mHPrPhiPhi[i][j];
              delete mHPaEtaEta[i][j];
              delete mHPaPhiPhi[i][j];
              delete mHPbEtaEta[i][j];
              delete mHPbPhiPhi[i][j];
          }
      }

      if (mdoFillSumHistograms) {
          delete mHAtSYtDYt[i][j];
          delete mHAtNSYtDYt[i][j];
          delete mHJtSEtaDPhi[i][j];
          delete mHJtNSEtaDPhi[i][j];
          if (!mdontFillMeanPt) {
              delete mHPrJtSEtaDPhi[i][j];
              delete mHPaJtSEtaDPhi[i][j];
              delete mHPbJtSEtaDPhi[i][j];
          }
      }

      delete mHQinv[i][j];
      delete mHNQinv[i][j];
    }
    if(mdoPairDensityHistograms) {
      for(int j=0;j<nden;j++){
        delete mHTPCAvgTSep[i][j];
        delete mHTPCAvgZSep[i][j];
        delete mHTPCEntTSep[i][j];
        delete mHTPCEntZSep[i][j];
        delete mHTPCMidTSep[i][j];
        delete mHTPCMidZSep[i][j];
        delete mHTPCExitTSep[i][j];
        delete mHTPCExitZSep[i][j];
        delete mHTPCMidTdptP[i][j];
        delete mHTPCMidTdptN[i][j];
        delete mHTPCMidZdptP[i][j];
        delete mHTPCMidZdptN[i][j];
        delete mHTPCAvgTZ[i][j];
        delete mHTPCEntTZ[i][j];
        delete mHTPCMidTZ[i][j];
        delete mHTPCMidTZC[i][j];
        delete mHTPCMidTZNC[i][j];
        delete mHTPCExitTZ[i][j];
        delete mHTPCEntTdpt[i][j];
        delete mHTPCMidTdpt[i][j];
        delete mHTPCExitTdpt[i][j];       
      }
    
    }

    if (!mdontFillYtYt) {
        delete [] mHYtYt[i];
        delete [] mHNYtYt[i];
    }

    delete [] mHJtDEtaDPhi[i];
    if (!mdontFillMeanPt) {
        delete [] mHPrJtDEtaDPhi[i];
        delete [] mHPaJtDEtaDPhi[i];
        delete [] mHPbJtDEtaDPhi[i];
    }

    if (mdoFillEtaEta) {
        delete [] mHEtaEta[i];
        delete [] mHPhiPhi[i];
        delete [] mHNPhiPhi[i];
        if (!mdontFillMeanPt) {
            delete [] mHPrEtaEta[i];
            delete [] mHPrPhiPhi[i];
            delete [] mHPaEtaEta[i];
            delete [] mHPaPhiPhi[i];
            delete [] mHPbEtaEta[i];
            delete [] mHPbPhiPhi[i];
        }
    }

    if (mdoFillSumHistograms) {
        delete [] mHAtSYtDYt[i];
        delete [] mHAtNSYtDYt[i];
        delete [] mHJtSEtaDPhi[i];
        delete [] mHJtNSEtaDPhi[i];
        if (!mdontFillMeanPt) {
            delete [] mHPrJtSEtaDPhi[i];
            delete [] mHPaJtSEtaDPhi[i];
            delete [] mHPbJtSEtaDPhi[i];
        }
    }

    delete [] mHQinv[i];
    delete [] mHNQinv[i];

    if(mdoPairDensityHistograms) {
      delete [] mHTPCAvgTSep[i];
      delete [] mHTPCAvgZSep[i];
      delete [] mHTPCEntTSep[i];
      delete [] mHTPCEntZSep[i];
      delete [] mHTPCMidTSep[i];
      delete [] mHTPCMidZSep[i];
      delete [] mHTPCExitTSep[i];
      delete [] mHTPCExitZSep[i];
      delete [] mHTPCMidTdptP[i];
      delete [] mHTPCMidTdptN[i];
      delete [] mHTPCMidZdptP[i];
      delete [] mHTPCMidZdptN[i];
      delete [] mHTPCAvgTZ[i];
      delete [] mHTPCEntTZ[i];
      delete [] mHTPCMidTZ[i];
      delete [] mHTPCMidTZC[i];
      delete [] mHTPCMidTZNC[i];
      delete [] mHTPCExitTZ[i];
      delete [] mHTPCEntTdpt[i];
      delete [] mHTPCMidTdpt[i];
      delete [] mHTPCExitTdpt[i];
    }

  }

}

//-----------------------------------------------------------------
void StEStruct2ptCorrelations::createHist2D(TH2D*** h, const char* name, int iknd, int icut, int zcut, int ncut, int nx, float xmin, float xmax, int ny, float ymin, float ymax ){

  TString hname(bName[iknd]);
  hname+=name;
  hname += "_cutBin_";
  hname += icut;
  hname += "_zBuf_";
  hname += zcut;
  TString htitle(bTitle[iknd]);
  htitle+=name;
  h[iknd][ncut]=new TH2D(hname.Data(),htitle.Data(),nx,xmin,xmax,ny,ymin,ymax);

}
//-----------------------------------------------------------------
void StEStruct2ptCorrelations::createHist1D(TH1D*** h, const char* name, int iknd, int icut, int zcut, int ncut, int nx, float xmin, float xmax){

  TString hname(bName[iknd]);
  hname+=name;
  hname += "_cutBin_";
  hname += icut;
  hname += "_zBuf_";
  hname += zcut;
  TString htitle(bTitle[iknd]);
  htitle+=name;
  h[iknd][ncut]=new TH1D(hname.Data(),htitle.Data(),nx,xmin,xmax);

}
//-----------------------------------------------------------------
void StEStruct2ptCorrelations::createHist1D(TH1F*** h, const char* name, int iknd, int icut, int zcut, int ncut, int nx, float xmin, float xmax){

  TString hname(bName[iknd]);
  hname+=name;
  hname += "_cutBin_";
  hname += icut;
  hname += "_zBuf_";
  hname += zcut;
  TString htitle(bTitle[iknd]);
  htitle+=name;
  h[iknd][ncut]=new TH1F(hname.Data(),htitle.Data(),nx,xmin,xmax);

}


/***********************************************************************
 *
 * $Log: StEStruct2ptCorrelations.cxx,v $
 * Revision 1.25  2009/05/08 00:09:54  prindle
 * In 2ptCorrelations we added switches to select blocks of histograms to fill.
 * (See constructor in StEStruct2ptCorrelations.cxx)
 * Use a brute force method for checking crossing cuts. I had too many corner
 * cases with my clever check.
 * In Binning, change Yt limit and add methods for accessing number of histogram bins
 * to use (used in Support)
 *
 * Revision 1.24  2008/12/02 23:45:04  prindle
 * Changed switchYt to switchXX (etc.) to better reflect function.
 * Change minYt to 1.0 in Binning so YtYt histogram doesn't have empty lower bin (pt = 0.164 for yt = 1.0)
 * In CutBin: remove initPtBin
 *            add mode 8
 *            add notSymmetrized (used in Support)
 * Added LUT (Look Up Table) for pair cuts. Experimental for now.
 * Modified cutMerging2 (to look at track separation at a few radii)
 * and cutCrossing2 so it doesn't accidentally reject almost back to back tracks.
 *
 * Revision 1.23  2008/03/19 22:06:00  prindle
 * Added doInvariantMass flag.
 * Added some plots in pairDensityHistograms.
 * SetZOffset used to only be done when doPairDensity was true.
 * Moved creating/copying pairDensity histograms to same place as other histograms.
 * Added cutBinHistMode
 * mode3 neck was defined as yt1<2.2 && yt2<2.2 (and not soft)
 *            now is        1.8<yt1<2.2  && 1.8<yt2<2.2
 * Added gooddzdxy, Merging2 and Crossing2 to pair cuts.
 *
 * Revision 1.22  2007/11/26 19:55:22  prindle
 * In 2ptCorrelations: Support for keeping all z-bins of selected centralities
 *                     Change way \hat{p_t} is calculated for parent distributions in pid case.
 *    Binning          Added parent binning (for \hat{p_t}
 *    CutBin:          Mode 5 extensively modified.
 *                     Added invariant mass cuts (probably a bad idea in general.)
 *
 * Revision 1.21  2007/05/27 22:45:00  msd
 * Added new cut bin modes 2 (soft/hard SS/AS), 6 (z-vertex binning), and 7 (modes 2*6).
 * Fixed bug in merging cut.
 * Added a few histograms to 2pt corr.
 *
 * Revision 1.20  2007/01/26 17:17:04  msd
 * Implemented new binning scheme: dEta stored in array with bin centered at zero, dPhi array has bins centered at zero and pi.  Final DEtaDPhi has 25x25 bins with dPhi bin width of pi/12 so all major angles are centered in bins.
 *
 * Revision 1.19  2006/10/02 22:20:51  prindle
 * Store only quadrant of eta_Delta - phi_Delta array/histogram.
 * Store half of eta_Sigma - phi_Delta array/histogram.
 * This required modifications in Binning.
 * I had a bug in the pair loop (which left +- not fully symmetrized)
 * and had to make changes in cut bins for mode 5 (and 3 I think)
 * when I fixed this.
 * Also change crossing cut to use only two parameters, the sign of
 * the magnetic field being taken from the MuDst.
 *
 * Revision 1.18  2006/05/03 18:14:43  msd
 * Fixed pair density again, removed references to iSwitch and symmetrize for simplicity
 *
 * Revision 1.17  2006/05/03 17:52:11  msd
 * Fixed pair density plots broken by recent symmetry changes
 *
 * Revision 1.16  2006/04/27 22:40:35  porter
 * 3 changes: 1) added npair hists for errors needed with eta_delta weighting
 * 2) commented out a few histograms to trim memory usage
 * 3) changed all hists to double precision (reflected in createHists member functions)
 *
 * Revision 1.15  2006/04/13 23:02:35  prindle
 * Added comment about not deleting output root file (which gets reported
 * as a memory leak but this is just before the program quits anayway.)
 *
 * Revision 1.14  2006/04/12 19:09:07  porter
 * added logic should z-vertex cut be ommitted (i.e. analysis of event generators)
 *
 * Revision 1.13  2006/04/10 23:42:32  porter
 * Added sameSide() & awaySide() methods to PairCut (so only defined in 1 place)
 * and added the eta_delta weighting as a binned correctin defined by the eta-limits in
 * the StEStructBinning object
 *
 * Revision 1.12  2006/04/06 01:01:11  prindle
 *
 *   New mode in CutBin, 5, to do pid correlations. There is still an issue
 * of how to set the pt ranges allowed for the different particle types.
 * For data we probably want to restrict p to below 1GeV for pi and K, but
 * for Hijing and Pythia we can have perfect pid. Currently cuts are type
 * into the code (so you have to re-compile to change them.)
 *
 *   In the Correlations code I split -+ from +- and am keeping track of
 * pt for each cut bin. These required changes in the Support code.
 *
 * Revision 1.11  2006/04/04 22:10:07  porter
 * a handful of changes (specific to correlations)
 *  - added StEStructQAHists so that if NOT input frm Maker, each analysis has its own
 *  - used ability to get any max,min val from the cut class - or z-vertex binning
 *  - put z-vertex binning into 1 place
 *  - switched back 1st line of pair cut method to keep pair if good, not to reject if bad.
 *  - Pair cut object is now pointer in correlations
 *  - some diagnostic printouts available from macro
 *  - Duncan's delta-phi binning change
 *
 * Revision 1.10  2006/02/22 22:05:10  prindle
 * Removed all references to multRef (?)
 * Added cut mode 5 for particle identified correlations.
 * Other cut modes should be same as before
 *
 * Revision 1.8  2005/10/10 16:22:51  msd
 * fixing bug in buffer initialization
 *
 * Revision 1.7  2005/09/14 17:14:17  msd
 * Large update, added new pair-cut system, added pair density plots for new analysis mode (4), added event mixing cuts (rewrote buffer for this)
 *
 * Revision 1.6  2005/09/07 20:21:13  prindle
 *
 *   2ptCorrelations: Rearranged array/histogram initialization/destruction.
 *                    Now histograms are only allocated at end of job,
 *                    just before they are filled then written.
 *
 * Revision 1.5  2005/03/03 01:30:43  porter
 * updated StEStruct2ptCorrelations to include pt-correlations and removed
 * old version of pt-correlations from chunhuih (StEStruct2ptPtNbar)
 *
 * Revision 1.4  2004/09/16 23:44:05  chunhuih
 *
 * call init() from doEvent() instead of the constructor. This is needed
 * because we want to use the polymorphic feature of this virtual function.
 * The constructor would always call the local version of a virtual function.
 *
 * Revision 1.3  2004/07/01 00:34:52  porter
 * correct accounting for possible pairs in stats files
 *
 * Revision 1.2  2004/06/25 03:11:48  porter
 * New cut-binning implementation and modified pair-cuts for chunhui to review
 *
 * Revision 1.1  2003/10/15 18:20:46  porter
 * initial check in of Estruct Analysis maker codes.
 *
 *
 *********************************************************************/


