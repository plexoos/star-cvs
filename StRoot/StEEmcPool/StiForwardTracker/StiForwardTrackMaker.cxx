// -- Author : Victor Perevoztchikov
// 
// $Id: StiForwardTrackMaker.cxx,v 1.1 2005/09/08 21:42:03 balewski Exp $

#include <StMessMgr.h>

#include "StiForwardTrackMaker.h"
#include "StEvent.h"
#include "StPrimaryVertex.h"

#include "Sti/StiToolkit.h"
#include "Sti/StiKalmanTrack.h"
#include "Sti/StiKalmanTrackNode.h"
#include "StiMaker/StiMaker.h"
#include "Sti/StiTrackContainer.h"
#include "Sti/StiTrackToTrackMap.h"

#define xG(t)   (t->x_g())
#define yG(t)   (t->y_g())
#define zG(t)   (t->z_g())
#define rxyG(t) sqrt(xG(t)*xG(t) + yG(t)*yG(t)) 
#define xL(t)   (t->getX())
#define yL(t)   (t->getY())
#define zL(t)   (t->getZ())
#define ezL(t)  sqrt(t->getCzz())
#define eyL(t)  sqrt(t->getCyy())


ClassImp(StiForwardTrackMaker)

//_____________________________________________________________________________
StiForwardTrackMaker::StiForwardTrackMaker(const char *name):StMaker(name){
  mToolkit=0;
  mTotEve=0;
  HList=0;
  memset(hA,0,sizeof(hA));
  mMaxTrkDcaRxy = 3.0;  // cm

}


//_____________________________________________________________________________
StiForwardTrackMaker::~StiForwardTrackMaker(){
  //
}


//_____________________________________________________________________________
/// Init - is a first method the top level StChain calls to initialize all its makers 
Int_t StiForwardTrackMaker::Init(){

  //get a pointer to StiMaker:
  StiMaker* sti = (StiMaker*)StMaker::GetChain()->GetMaker("Sti");
  if(sti==0) {
    gMessMgr->Warning() <<GetName()<<"no STi Maker, it is fatal"<<endm;
    return kStErr;
  }
  
  //get pointer to Sti toolkit
  mToolkit = sti->getToolkit();
  assert(mToolkit); // internal error of Sti

  HList=new TObjArray(0);
  initHisto();
  HList->ls();

  //  gMessMgr->Info() << "PPV-2 cuts have been activated, mTestMode=" << mTestMode<<endm;
  

  return StMaker::Init();
}


//_____________________________________________________________________________
/// Make - this method is called in loop for each event
Int_t StiForwardTrackMaker::Make(){

  if(mToolkit==0) {
    gMessMgr->Warning() <<"no Sti tool kit "<<GetName()<<" is  OFF"<<endm;
    return kStErr;
  }

  StEvent *event = (StEvent *) GetInputDS("StEvent");
  assert(event);
  eveID=event->id();
  mTotEve++;
  int nV=event->numberOfPrimaryVertices();
  
  gMessMgr->Info() << "\n JJJ  "<<GetName()<<" START nEve="<<mTotEve<<"  eveID="<<eveID<< " nPrimVertex="<<nV<< endm;

  hA[0]->Fill(nV);

  if(nV<=0) {
    gMessMgr->Info() << GetName()<<" event dropped, no vertex found"<<endm;
    return kStOK;
  }

  
  int iv;
  for(iv=0;iv<nV;iv++) {
    StPrimaryVertex *V=event->primaryVertex(iv);
    assert(V);
    const StThreeVectorF &v=V->position();
    const StThreeVectorF &ev=V->positionError();
    printf("iv=%d   Vz=%.2f +/-%.2f \n",iv,v.z(),ev.z()  );
    hA[1]->Fill(v.z());
  }
  
  //get the Sti track container...
  StiTrackContainer* tracks = mToolkit->getTrackContainer();
  if(tracks==0) {
    gMessMgr->Warning() <<"no STi tracks , skip eve"<<endm;
    return kStErr ;
  }

  int nAny=0;
  int nTry=0;
  for (StiTrackContainer::const_iterator it=(*tracks).begin();  it!=(*tracks).end(); ++it) {
    const StiKalmanTrack* track = static_cast<StiKalmanTrack*>(*it);
    if(track->getFlag()!=true) continue; // drop bad events
    nAny++;

    //.... test... find zDca for all tracks with eta>0.8
    if(track->getPseudoRapidity()<0.8) continue;
    float zDca, ezDca, rxyDca;
    examinTrackDca(track, zDca, ezDca, rxyDca);
    nTry++;
  }
 
  gMessMgr->Info() << "\n"<<GetName()<<"  found "<<nAny<<" sti input tracks, try ZDca for "<<nTry<<endm;

  return kStOK;
}

//_____________________________________________________________________________
// called before each event
void 
StiForwardTrackMaker::Clear(const char* opt){
  gMessMgr->Info() <<GetName()<<"::Clear()"<<  endm;
  eveID=0;
}   

//=======================================================
//=======================================================
void
StiForwardTrackMaker::initHisto() {
  assert(HList);
  hA[0]=new TH1F("nV","No. of vertices per eve",20,-0.5,19.5);
  hA[1]=new TH1F("zV","reconstructed vertices (any); Z (cm)",200,-200,200);

  int i;  
  for(i=0;i<mxHA; i++) if(hA[i]) HList->Add(hA[i]);
}

//-------------------------------------------------
//-------------------------------------------------
void
StiForwardTrackMaker::saveHisto(TString fname){
  TString outName=fname+".hist.root";
  TFile f( outName,"recreate");
  assert(f.IsOpen());
  printf("%d histos are written  to '%s' ...\n",HList->GetEntries(),outName.Data());
  HList->ls();
  HList->Write();
  f.Close();
}

//-------------------------------------------------
//-------------------------------------------------
Int_t
StiForwardTrackMaker::Finish(){
  saveHisto(GetName());
  return kStOK;
}

//-------------------------------------------------
//-------------------------------------------------
bool
StiForwardTrackMaker::examinTrackDca(const StiKalmanTrack*track,
		    float &zDca, float &ezDca, float &rxyDca){

  cout <<"#e  track->getPseudoRapidity()="<<track->getPseudoRapidity()<<" track->getFitPointCount()="<<track->getFitPointCount()<<endl;

  // .......... test DCA to beam .............
  StiKalmanTrack track1=*track; // clone track
  StiKalmanTrackNode* bmNode=track1.extrapolateToBeam();
  if(bmNode==0)  {
    cout<<"#a @beam  DCA NULL"<<endl; 
    return false ;
  }

  float rxy=rxyG(bmNode);
  cout<<"#e @beam global DCA x:"<< bmNode->x_g()<<" y:"<< bmNode->y_g()<<" z:"<< bmNode->z_g()<<" Rxy="<< rxy <<endl;
  if(rxy>mMaxTrkDcaRxy) return false;
 
   cout<<"#e inBeam |P|="<<bmNode->getP()<<" pT="<<bmNode->getPt()<<" local x="<<xL(bmNode)<<" y="<<yL(bmNode)<<" +/- "<<eyL(bmNode)<<" z="<<zL(bmNode)<<" +/- "<<ezL(bmNode)<<endl;

  zDca=zG(bmNode);
  ezDca=ezL(bmNode);
  rxyDca=rxy;
  
  return true;
}



// $Log: StiForwardTrackMaker.cxx,v $
// Revision 1.1  2005/09/08 21:42:03  balewski
// star
//








