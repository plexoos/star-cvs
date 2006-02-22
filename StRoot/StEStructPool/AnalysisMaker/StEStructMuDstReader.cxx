/**********************************************************************
 *
 * $Id: StEStructMuDstReader.cxx,v 1.6 2006/02/22 22:03:23 prindle Exp $
 *
 * Author: Jeff Porter 
 *
 **********************************************************************
 *
 * Description:  event reader class for common MuDsts
 *               Uses the StMuDstMaker for real reading
 *
 ***********************************************************************/
#include "StEStructMuDstReader.h"

#include "StEStructEventCuts.h"
#include "StEStructTrackCuts.h"
#include "StEStructPool/EventMaker/StEStructEvent.h"
#include "StEStructPool/EventMaker/StEStructTrack.h"

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StDetectorId.h"


StEStructMuDstReader::StEStructMuDstReader() {
    mhasdEdxCuts  = 0;
    mMaker        = 0;
    mECuts        = 0;
    mTCuts        = 0;
    mInChain      = false;
    mAmDone       = false;
    mCentBin      = 0;
};
StEStructMuDstReader::StEStructMuDstReader(StMuDstMaker* maker,
                                           StEStructEventCuts* ecuts,
                                           StEStructTrackCuts* tcuts,
                                           bool inChain,
                                           int  centBin) {
    mhasdEdxCuts  = 0;
    mMaker        = maker;
    setEventCuts(ecuts);
    setTrackCuts(tcuts);
    mInChain      = inChain;
    mAmDone       = false;
    mCentBin      = centBin;
};

//-------------------------------------------------------------------------
StEStructMuDstReader::~StEStructMuDstReader(){};

void StEStructMuDstReader::setMuDstMaker(StMuDstMaker* maker, bool inChain){ 
  mInChain=inChain;
  mMaker=maker; 
};

void StEStructMuDstReader::setEventCuts(StEStructEventCuts* ecuts) { mECuts=ecuts; };
void StEStructMuDstReader::setTrackCuts(StEStructTrackCuts* tcuts) {
    mTCuts=tcuts;
    if ( !mTCuts->goodElectron(100.0) ) {      
        mhasdEdxCuts = 1;
        dEdxBefore = new TH2F("dEdxNoCut","dEdxNoCut",150,0,1.5,150,0,0.000015);
        dEdxAfter  = new TH2F("dEdxCut","dEdxCut",150,0,1.5,150,0,0.000015);
        mTCuts->addCutHists(dEdxBefore, dEdxAfter, "dEdXPlots");
    }
};

bool StEStructMuDstReader::hasMaker() { return (mMaker) ? true : false ; }
bool StEStructMuDstReader::hasEventCuts() { return (mECuts) ? true : false ; }
bool StEStructMuDstReader::hasTrackCuts() { return (mTCuts) ? true : false ; }


//-------------------------------------------------------------------------
StEStructEvent* StEStructMuDstReader::next() {

  StEStructEvent* retVal=NULL;
  if(!mMaker) return retVal;
  if(!mInChain){
     int iret=mMaker->Make();
     if(iret){
        mAmDone=true;
        return retVal;
     }
  }
  if(!mMaker->muDst()) return retVal;
  return fillEvent();
}
     
//-------------------------------------------------------------------------
// Feb 14, 2006 djp Expunging all references to refMult that is stored in
//                  the MuDst. centrality cut will now always be on total
//                  number of tracks passing track cuts for MuDst. For
//                  Hijing this cut can be on number of tracks passing cuts
//                  or impact parameter.
// Apr 12, 2005 djp Jeff put centrality cuts into the event cuts and
//                  cut on refMult. I am changing this to use the
//                  centrality bin returned by the centrality object.
//                  This allows me to use total number of tracks passing cuts
//                  if I choose.
//  The problem is that to use all tracks passing cuts we have to count tracks
//  before we determine the centrality bin. Probably this amount of time is
//  small compared to rest of analysis.
StEStructEvent* StEStructMuDstReader::fillEvent(){

    StEStructEvent* retVal = new StEStructEvent();
    StMuDst* muDst=mMaker->muDst();
    StMuEvent* muEvent=muDst->event();
    if (!muEvent) {
        delete retVal;
        retVal=NULL;
        return retVal;
    }
 
    unsigned int tword=muEvent->l0Trigger().triggerWord();
    mNumGoodTracks = 0;

    float x=muEvent->eventSummary().primaryVertexPosition().x();
    float y=muEvent->eventSummary().primaryVertexPosition().y();
    float z=muEvent->eventSummary().primaryVertexPosition().z();

    bool useEvent=true;

    if ((fabs(x) < 1e-5) && (fabs(y) < 1e-5) && (fabs(z) < 1e-5)) {
        useEvent = false;
    } else if (!mECuts->goodTrigger(muEvent)  ||
               !mECuts->goodPrimaryVertexZ(z)) {
        useEvent=false;
    }
    int nTracks = countGoodTracks();
    if (!mECuts->goodCentrality(nTracks)) {
        useEvent=false;
    }
    retVal->SetCentrality( (double) nTracks );
    int jCent = retVal->Centrality();
    if ((mCentBin >= 0) && (jCent != mCentBin)) {
        useEvent = false;
    } else {
        retVal->SetEventID(muEvent->eventNumber());
        retVal->SetRunID(muEvent->runNumber());
        retVal->SetVertex(x,y,z);
        retVal->SetZDCe((float)muEvent->zdcAdcAttentuatedSumEast());
        retVal->SetZDCw((float)muEvent->zdcAdcAttentuatedSumWest());
        retVal->SetBField((float)muEvent->magneticField());

        if (!fillTracks(retVal)) {
            useEvent=false; 
        }
    }
    mECuts->fillHistogram(mECuts->triggerWordName(),(float)tword,useEvent);
    mECuts->fillHistogram(mECuts->primaryVertexZName(),z,useEvent);
    mECuts->fillHistogram(mECuts->centralityName(),(float)nTracks,useEvent);

    if (!useEvent) {
        delete retVal;
        retVal=NULL;
    }

    if (retVal) {
        retVal->FillChargeCollections();
    }
    return retVal;
} 
 

//-------------------------------------------------------------
bool StEStructMuDstReader::fillTracks(StEStructEvent* estructEvent) {

    //
    // create a single EbyE track, check cuts,
    // fill variables, add to StEStructEvent tracks which
    // does a copy.
    //

    StMuDst* muDst=mMaker->muDst();
    int numPrimaries=muDst->primaryTracks()->GetEntries();
    if (0==numPrimaries) {
        return false;
    }

    StEStructTrack* eTrack = new StEStructTrack();
    for(int i=0; i<numPrimaries; i++) {
        bool useTrack=true;
        eTrack->SetInComplete();
        StMuTrack* track=muDst->primaryTracks(i);

        if (mhasdEdxCuts) {
            dEdxBefore->Fill((track->p()).mag(),track->dEdx());
        }

        useTrack = isTrackGood( track );
        mTCuts->fillHistograms(useTrack);

        if (useTrack) {
            if (mhasdEdxCuts) {
                dEdxAfter->Fill((track->p()).mag(),track->dEdx());
            }
            fillEStructTrack(eTrack,track);
            estructEvent->AddTrack(eTrack);
        }
    }
    delete eTrack;
    return true;
}
//-------------------------------------------------------------
// This method checks all track cuts.
// No histogramming or copying data around.
bool StEStructMuDstReader::isTrackGood(StMuTrack* track) {

    bool useTrack=true;

    useTrack = (mTCuts->goodFlag(track->flag()) && useTrack);
    useTrack = (mTCuts->goodCharge(track->charge()) && useTrack);
    useTrack = (mTCuts->goodNFitPoints(track->nHitsFit()) && useTrack);
    useTrack = (mTCuts->goodNFitNMax((float)((float)track->nHitsFit()/(float)track->nHitsPoss())) && useTrack);
    useTrack = (mTCuts->goodGlobalDCA(track->dcaGlobal().magnitude()) && useTrack);
    useTrack = (mTCuts->goodEta(track->eta()) && useTrack);
    useTrack = (mTCuts->goodChi2(track->chi2()) && useTrack);
    useTrack = (mTCuts->goodPhi(track->phi()) && useTrack);

    //--> But add a quick electron removal... for selected p ranges
    //    Note I only want to do this if I am defining an electron dEdx cut.
    if (mhasdEdxCuts && mTCuts->goodElectron( track->nSigmaElectron() ) && useTrack )  {      
      float p = (track->p()).mag();
      if( (p>0.2 && p<0.45) || (p>0.7 && p<0.8) ) {
	useTrack=false;
	//cout << "  cut e:  p = " << p << ", nsig = " << track->nSigmaElectron() << endl;
      }
    }
    //--> end of electron pid

    float pt = track->pt();
    useTrack = (mTCuts->goodPt(pt) && useTrack);
    float _r=pt/0.139;
    float yt=log(sqrt(1+_r*_r)+_r);
    useTrack = (mTCuts->goodYt(yt) && useTrack);
    float mt = sqrt(pt*pt + 0.139*0.139);
    float xt = 1 - exp( -1*(mt-0.139)/0.4 );
    useTrack = (mTCuts->goodXt(xt) && useTrack);

    return useTrack;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------
// This method counts all good track.
// No histogramming or copying data around.
int StEStructMuDstReader::countGoodTracks() {
    mNumGoodTracks=0;
    StMuDst* muDst=mMaker->muDst();
    int numPrimaries=muDst->primaryTracks()->GetEntries();
    if (0==numPrimaries) {
        return 0;
    }
    for(int i=0; i<numPrimaries; i++) {
        if (isTrackGood(muDst->primaryTracks(i))) {
            mNumGoodTracks++;
        }
    }
    return mNumGoodTracks;
}

//-------------------------------------------------------------------------
void StEStructMuDstReader::fillEStructTrack(StEStructTrack* eTrack,StMuTrack* mTrack){

  StThreeVectorF p=mTrack->p();  
  eTrack->SetPx(p.x());
  eTrack->SetPy(p.y());
  eTrack->SetPz(p.z());

  StThreeVectorF b=mTrack->dca();
  eTrack->SetBx(b.x());
  eTrack->SetBy(b.y());
  eTrack->SetBz(b.z());

  StThreeVectorF gb=mTrack->dcaGlobal();
  eTrack->SetBxGlobal(gb.x());
  eTrack->SetByGlobal(gb.y());
  eTrack->SetBzGlobal(gb.z());

  eTrack->SetEta(mTrack->eta());
  eTrack->SetPhi(mTrack->phi());

  eTrack->SetDedx(mTrack->dEdx());
  eTrack->SetChi2(mTrack->chi2());

  //
  // -> note in my analysis I chose nSigma instead of prob.
  //
  eTrack->SetPIDe(mTrack->nSigmaElectron()); 
  eTrack->SetPIDpi(mTrack->nSigmaPion());
  eTrack->SetPIDp(mTrack->nSigmaProton());
  eTrack->SetPIDk(mTrack->nSigmaKaon());
  eTrack->SetPIDd(10000.);

  eTrack->SetNFitPoints(mTrack->nHitsFit());
  eTrack->SetNFoundPoints(mTrack->nHits());
  eTrack->SetNMaxPoints(mTrack->nHitsPoss());

  StTrackTopologyMap map = mTrack->topologyMap();

  eTrack->SetTopologyMapTPCNHits(map.numberOfHits(kTpcId));
  eTrack->SetTopologyMapData(0,map.data(0));
  eTrack->SetTopologyMapData(1,map.data(1));

  eTrack->SetDetectorID(1); //TPC
  eTrack->SetFlag(mTrack->flag());
  eTrack->SetCharge(mTrack->charge());

  eTrack->SetHelix(mTrack->helix());

}; 
  

/***********************************************************************
 *
 * $Log: StEStructMuDstReader.cxx,v $
 * Revision 1.6  2006/02/22 22:03:23  prindle
 * Removed all references to multRef
 *
 * Revision 1.5  2005/09/14 17:08:34  msd
 * Fixed compiler warnings, a few tweaks and upgrades
 *
 * Revision 1.4  2005/09/07 20:18:42  prindle
 *   AnalysisMaker: Keep track of currentAnalysis (for use in doEStruct macro)
 *   EventCuts.h:   Added trigger cuts including cucu and year 4.
 *   MuDstReader:   Added dE/dx histograms. Re-arranged code to count tracks
 *                    before making centrality cut.
 *   TrackCuts:     Random changes. Moved some variables from private to public.o
 *
 * Revision 1.1  2003/10/15 18:20:32  porter
 * initial check in of Estruct Analysis maker codes.
 *
 *
 *********************************************************************/
