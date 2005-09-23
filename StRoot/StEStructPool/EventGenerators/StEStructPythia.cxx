/**********************************************************************
 *
 * $Id: StEStructPythia.cxx,v 1.7 2005/09/23 23:37:23 prindle Exp $
 *
 * Author: Jeff Porter 
 *
 **********************************************************************
 *
 * Description:  EStructEventReader wrapper for (T)Pythia event generator
 *
 **********************************************************************/
#include "StEStructPythia.h"

#include "StEStructPool/AnalysisMaker/StEStructEventCuts.h"
#include "StEStructPool/AnalysisMaker/StEStructTrackCuts.h"
#include "StEStructPool/EventMaker/StEStructEvent.h"
#include "StEStructPool/EventMaker/StEStructTrack.h"

StEStructPythia::StEStructPythia() {
    mPythia       = 0;
    mECuts        = 0;
    mTCuts        = 0;
    mInChain      = false;
    mEventsToDo   = 100;
    mUseAllTracks = false;
    mCentBin      = 0;
    mEventCount   = 0;
    mAmDone       = false;
};

StEStructPythia::StEStructPythia(TPythia6* pythia,
                                 StEStructEventCuts* ecuts,
                                 StEStructTrackCuts* tcuts,
                                 bool inChain,
                                 bool useAllTracks,
                                 int  multBin,
                                 int  eventsToDo) {
    mPythia       = pythia;
    mECuts        = ecuts;
    mTCuts        = tcuts;
    mInChain      = inChain;
    mEventsToDo   = eventsToDo;
    mUseAllTracks = useAllTracks;
    mCentBin      = multBin;
    mEventCount   = 0;
    mAmDone       = false;
};

bool StEStructPythia::hasGenerator() { return (mPythia) ? true : false ; };
bool StEStructPythia::hasEventCuts() { return (mECuts) ? true : false ; }
bool StEStructPythia::hasTrackCuts() { return (mTCuts) ? true : false ; }


//-------------------------------------------------------------------------
StEStructEvent* StEStructPythia::next() {

    if (!mPythia || mEventCount==mEventsToDo) {
        mAmDone = true;
        return (StEStructEvent*)NULL;
    }
    StEStructEvent* retVal = NULL;

    if (!mInChain ) {
          mPythia->GenerateEvent();
          mEventCount++;
    }

    retVal = new StEStructEvent();

    retVal->SetVx(0);
    retVal->SetVy(0);
    retVal->SetVz(0);
    retVal->SetBField(0.5);

    int nTracks = countGoodTracks();
    if (mUseAllTracks) {
        retVal->SetCentrality( (double) nTracks );
    } else {
        // Should have an option to use tracks with |\eta| < 0.5 ??
        retVal->SetCentrality( (double) nTracks );
    }
    int jCent = retVal->Centrality();
    if (((mCentBin >= 0) && (jCent != mCentBin)) ||
        !mECuts->goodNumberOfTracks(mRefMult)) {
        delete retVal;
        retVal=NULL;
        mECuts->fillHistogram(mECuts->numTracksName(),(float)mRefMult,false);
    } else {
        fillTracks(retVal);
        retVal->SetOrigMult(mRefMult);
        retVal->SetCentMult(mRefMult);
        retVal->FillChargeCollections();
        mECuts->fillHistogram(mECuts->numTracksName(),(float)mRefMult,true);
    }
    return retVal;
}   

//--------------------------------------------------------------------------
void StEStructPythia::fillTracks(StEStructEvent* estructEvent){

    mRefMult=0;
    Pyjets_t* pstr= mPythia->GetPyjets();
    int numParticles=mPythia->GetN();

    StEStructTrack* eTrack= new StEStructTrack();
    for (int i=2;i<numParticles;i++) { // 0 & 1 for incoming protons 

        if (pstr->K[0][i]==21) continue;
        int pid = pstr->K[1][i];
        // require no daughters & if pi,k,proton, or electron; else skip...
        if (!(0==pstr->K[3][i])) continue;  

        if (!isTrackGood(i)) {
            mTCuts->fillHistograms(false);
            continue;
        }
        mTCuts->fillHistograms(true);
        eTrack->SetInComplete();

        float p[3];
        float v[3];
        for(int k=0;k<3;k++){
            p[k]=pstr->P[k][i];
            v[k]=pstr->V[k][i];
        }

        float pt    = sqrt(p[0]*p[0] + p[1]*p[1]);
        float theta = acos(p[2]/ TMath::Sqrt(pt*pt + p[2]*p[2]) );
        float eta   = -1.0*log(tan(theta/2.0));
        float *gdca = globalDCA(p,v);
        float phi=atan2((double)p[1], (double)p[0]);

        eTrack->SetBx(gdca[0]);
        eTrack->SetBy(gdca[1]);
        eTrack->SetBz(gdca[2]);
        eTrack->SetBxPrimary(gdca[0]);
        eTrack->SetByPrimary(gdca[1]);
        eTrack->SetBzPrimary(gdca[2]);
        eTrack->SetBxGlobal(gdca[0]);
        eTrack->SetByGlobal(gdca[1]);
        eTrack->SetBzGlobal(gdca[2]);
        delete [] gdca;

        eTrack->SetPIDe(0);
        eTrack->SetPIDpi(0);
        eTrack->SetPIDk(0);
        eTrack->SetPIDp(0);
        eTrack->SetPIDd(0);
        if ((pid == 7) || (pid == 8)) {
            eTrack->SetPIDe(1);
        } else if ((pid == -211) || (pid == 211)) {
            eTrack->SetPIDpi(1);
        } else if ((pid == -321) || (pid == 321)) {
            eTrack->SetPIDk(1);
        } else if ((pid == -2212) || (pid == 2212)) {
            eTrack->SetPIDp(1);
        } else if (pid ==95) {
            // No anti-deuteron. I think Hijing does not make deuterons and
            // this pid code is only intended for use with GEANT?
            eTrack->SetPIDd(1);
        }

        eTrack->SetPx(p[0]);
        eTrack->SetPy(p[1]);
        eTrack->SetPz(p[2]);
        eTrack->SetEta(eta);
        eTrack->SetPhi(phi);
        eTrack->SetDedx(0);
        eTrack->SetChi2(1);
        eTrack->SetTopologyMapTPCNHits(45);
        eTrack->SetNMaxPoints(45);
        eTrack->SetNFoundPoints(45);
        eTrack->SetNFitPoints(45);

        if(pid<0){
          eTrack->SetCharge(-1);
        } else {
          eTrack->SetCharge(1);
        }    


        // I don't know what this next block of code does. Jeff must have put this in.
        // djp: Sept. 13, 2005
        // now add fragmentation history (up to 4 lines back) in the tpcmap area

        int ip[4] = {0,0,0,0};
        ip[0] = pstr->K[2][i];
        for (int k=1;k<4;k++) {
            if (ip[k-1]<3) {
	            ip[k-1]=0;
                break;
            } 
            ip[k] = pstr->K[2][ip[k-1]-1];    
        }
        unsigned int map[2];
        map[0]=(unsigned int)((ip[1]<<16)+ip[0]);
        map[1]=(unsigned int)((ip[3]<<16)+ip[2]);
        eTrack->SetTopologyMapData(0,map[0]);
        eTrack->SetTopologyMapData(1,map[1]);

        // I included this line because I have in in the Hijing interface
        // but I don't know what it does right off.  djp: Sept. 13, 2005
        eTrack->SetTopologyMapTPCNHits(45);

        estructEvent->AddTrack(eTrack);
    }; // particle loop

  delete eTrack;
  return;

}    


//-------------------------------------------------------------
// This method checks all track cuts.
// No histogramming or copying data around.
bool StEStructPythia::isTrackGood(int i) {
    Pyjets_t* pstr= mPythia->GetPyjets();
    int pid = pstr->K[1][i];
    if (!measureable(pid)) {     // checks if pi,k,p or e
        return false;
    }
    float p[3];
    float v[3];
    for (int k=0;k<3;k++) {
        p[k] = pstr->P[k][i];
        v[k] = pstr->V[k][i];
    }

    float pt = sqrt(p[0]*p[0]+p[1]*p[1]);
    if (pt < 0.15) {
        return false;
    }

    float theta = acos(p[2]/ sqrt(pt * pt + p[2] * p[2]) );
    float eta   = -1.0*log(tan(theta/2.0));
    float phi = atan2((double)p[1], (double)p[0]);
    float* gdca  = globalDCA(p,v);
    float _r = pt/0.139;
    float yt = log(sqrt(1+_r*_r)+_r);

    bool useTrack = true;
    useTrack = (mTCuts->goodGlobalDCA(gdca[3]) && useTrack);
    useTrack = (mTCuts->goodEta(eta) && useTrack);
    useTrack = (mTCuts->goodPhi(phi) && useTrack);
    useTrack = (mTCuts->goodPt(pt) && useTrack);
    useTrack = (mTCuts->goodYt(yt) && useTrack);
    delete [] gdca;

    return useTrack;
}

//-------------------------------------------------------------------------
//-------------------------------------------------------------
// This method counts all good track.
// No histogramming or copying data around.
int StEStructPythia::countGoodTracks() {
    mRefMult = 0;
    int numParticles = mPythia->GetN();
    for (int i=2;i<numParticles;i++) { // 0 & 1 for incoming protons 
        if (isTrackGood(i)) {
            mRefMult++;
        }
    }
    return mRefMult;
}
//--------------------------------------------------------------------------
void StEStructPythia::setEventCuts(StEStructEventCuts* cuts) {
    if(mECuts) delete mECuts;
    mECuts=cuts;
}

//---------------------------------------------------------------
void StEStructPythia::setTrackCuts(StEStructTrackCuts* cuts) {
    if(mTCuts) delete mTCuts;
    mTCuts=cuts;
}




/**********************************************************************
 *
 * $Log: StEStructPythia.cxx,v $
 * Revision 1.7  2005/09/23 23:37:23  prindle
 * Starting to add vertex distribution and track acceptance dependance on
 * number of possible hits.
 *   Make Pythia interface look like Hijing interface so it now works within
 * my Fluctuation and Correlation framework.
 *
 * Revision 1.6  2004/09/24 01:43:12  prindle
 * Add call to define centrality by multiplicity.
 *
 * Revision 1.5  2004/07/01 00:35:29  porter
 * modified 'startrigger'... still need to make this a switch
 *
 * Revision 1.4  2004/06/25 03:13:01  porter
 * added simple trigger selection implemented like BBC-AND plus CTB
 *
 * Revision 1.3  2004/06/09 22:37:51  prindle
 * Moved some variable declarations inside comment to get rid of
 * unused variable warnings.
 * ved
 *
 * Revision 1.2  2004/03/24 21:26:52  porter
 * fixed error calculating eta as rapidity
 *
 * Revision 1.1  2003/11/21 06:24:56  porter
 * Pythia event generater as an StEStructEventReader
 *
 *
 *********************************************************************/
