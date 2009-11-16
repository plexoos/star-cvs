//////////////////////////////////////////////////////////////////////////
//
//
// StTriggerSimuMaker R.Fatemi, Adam Kocoloski , Jan Balewski  (Fall, 2007)
//
// Goal: generate trigger response based on ADC
// implemented BEMC,EEMC,....
// >StTriggerSimu/*SUB*/St*SUB*TriggerSimu.h
// >where *SUB* are the subsystems: Eemc, Bemc, Bbc,.... 
// > L2 is served by a separate maker
//
//////////////////////////////////////////////////////////////////////////

// $Id: StTriggerSimuMaker.cxx,v 1.31 2009/11/16 07:51:19 pibero Exp $


#include <Stiostream.h>
#include "StChain.h"
#include "TFile.h"
#include <math.h>

#include <fstream>
#include "StEmcUtil/database/StEmcDecoder.h"

#include "St_db_Maker/St_db_Maker.h" // just for time stamp
#include "Eemc/EMCdsm2Tree.h"// to access Etot

//StEvent
#include "StEvent/StEvent.h"

//get  EEMC
#include "Eemc/StEemcTriggerSimu.h"
#include "Eemc/EemcHttpInfo.h"
#include "Eemc/EEfeeTPTree.h" // for printouts only
#include "Eemc/EEfeeTP.h"  // for printouts only

//get BEMC
#include "StTriggerUtilities/Bemc/StBemcTriggerSimu.h"
#include "StEmcRawMaker/StBemcTables.h"
#include "StEmcUtil/geometry/StEmcGeom.h"

//get EMC
#include "StTriggerUtilities/Emc/StEmcTriggerSimu.h"

//get BBC
#include "Bbc/StBbcTriggerSimu.h"

//get L2
#include "L2Emulator/StL2TriggerSimu.h"

//get HEAD Maker
#include "StTriggerSimuMaker.h"
#include "StTriggerSimuResult.h"

ClassImp(StTriggerSimuMaker)

StTriggerSimuMaker::StTriggerSimuMaker(const char *name):StMaker(name) {
    mDbMk = dynamic_cast<St_db_Maker*> ( this->GetMakerInheritsFrom("St_db_Maker") );
    mYear=-1;
    mMCflag=0;
    eemc=0;
    bbc=0;
    bemc=0;
    lTwo=0;
    emc = new StEmcTriggerSimu;
    for (int a=0; a<numSimulators; a++){
      mSimulators[a]=0;
    }
}

StTriggerSimuMaker::~StTriggerSimuMaker() { /* no-op */ }


void StTriggerSimuMaker::useEemc(int flag){
    eemc=new StEemcTriggerSimu();
    eemc->setConfig(flag);
    mSimulators[0]=eemc;
}

void StTriggerSimuMaker::useBbc(){
    bbc=new StBbcTriggerSimu;
    mSimulators[1]=bbc;
}

void StTriggerSimuMaker::useBemc(){
    bemc=new StBemcTriggerSimu;
    bemc->setHeadMaker(this);
    mSimulators[2]=bemc;
}

void StTriggerSimuMaker::useL2(StGenericL2Emulator* L2Mk){
    lTwo=new StL2TriggerSimu(L2Mk);
    mSimulators[4]=lTwo;

}

Int_t StTriggerSimuMaker::Init() {
    LOG_INFO <<Form("StTriggerSimuMaker::Init(), MCflag=%d",mMCflag)<<endm;
    if(eemc) {
        eemc->setHList(mHList);
    }

    if(bemc) {
        bemc->setHList(mHList);
    }

    for(Int_t i=0; i<numSimulators; i++) {
      if(mSimulators[i]){  
        mSimulators[i]->setMC(mMCflag);
        mSimulators[i]->Init();
      }

    }


    return StMaker::Init();
}

void StTriggerSimuMaker::Clear(const Option_t*){
    LOG_DEBUG<<"StTriggerSimuMaker::Clear()"<<endm;
    
    for(Int_t i=0; i<numSimulators; i++) {
      if (mSimulators[i]){
        mSimulators[i]->Clear();
      }
    }
    
    mResults.clear();
}

Int_t StTriggerSimuMaker::InitRun(int runNumber) {
    assert(mDbMk);
    mYear = mDbMk->GetDateTime().GetYear();
    int yyyymmdd = mDbMk->GetDateTime().GetDate(); //form of 19971224 (i.e. 24/12/1997)
    int hhmmss = mDbMk->GetDateTime().GetTime(); //form of 123623 (i.e. 12:36:23)
    LOG_INFO << Form("InitRun() run=%d yyyymmdd=%d  hhmmss=%06d\n", runNumber, yyyymmdd, hhmmss) << endm;

    //Use unified EMC trigger for EEMC/BEMC triggers in y=2009 or later
    if (mYear >= 2009 && (mSimulators[0] || mSimulators[2])) {
      emc->setHeadMaker(this);
      emc->setBemc(bemc);
      emc->setEemc(eemc);
      emc->setMC(mMCflag);
      mSimulators[3] = emc;
    }

    for (Int_t i = 0; i < numSimulators; ++i)
      if (mSimulators[i])
	mSimulators[i]->InitRun(runNumber);

    return kStOK;
}

Int_t StTriggerSimuMaker::Make() {
   
  for(Int_t i=0; i<numSimulators; i++) {
    if (mSimulators[i]){
      mSimulators[i]->Make();
    }
  }

  std::vector<int> trigIds = triggerIds();
  TString line = "Triggers: ";
  for (size_t i = 0; i < trigIds.size(); ++i) line += Form("%d ",trigIds[i]);
  LOG_DEBUG << line << endm;

  return kStOK;
}

bool StTriggerSimuMaker::isTrigger(int trigId) {

  for(Int_t i=0; i<numSimulators; i++) {
    if (mSimulators[i]){  
      
      if(mSimulators[i]->triggerDecision(trigId) == kNo) return false;
    }
  }
  
  return true;
}

std::vector<int> StTriggerSimuMaker::triggerIds() const
{
  std::vector<int> v;

  if (mYear >= 2009 && emc) {
    std::set<int> s = emc->triggerIds();
    std::copy(s.begin(),s.end(),std::back_inserter(v));
  }

  return v;
}

const StTriggerSimuResult& StTriggerSimuMaker::detailedResult(unsigned int trigId) {
    // first check if we already filled this result
    for(unsigned i=0; i<mResults.size(); i++) {
        if(mResults[i].triggerId() == trigId) return mResults[i];
    }
    
    StTriggerSimuResult result;
    result.setTriggerId(trigId);
    if(bbc) {
        result.setBbcDecision(bbc->triggerDecision(trigId));
    }
    if(bemc) {
        result.setBemcDecision(bemc->triggerDecision(trigId));

        if(bemc->triggerDecision(trigId)==1) {
            // Record HTs above Threshold
            vector< pair<int,int> > Towers=bemc->getTowersAboveThreshold(trigId);
            for(vector< pair<int,int> >::iterator itr=Towers.begin(); itr!=Towers.end(); itr++){
                result.addHighTower((*itr).first,(*itr).second);
            }
            // Record TPs above Threshold
            vector< pair<int,int> > tPatches=bemc->getTriggerPatchesAboveThreshold(trigId);
            for(vector< pair<int,int> >::iterator itr=tPatches.begin(); itr!=tPatches.end(); itr++){
                result.addTriggerPatch((*itr).first,(*itr).second);
            }
            // Record JPs above Threshold
            vector< pair<int,int> > jPatches=bemc->getJetPatchesAboveThreshold(trigId);
            for(vector< pair<int,int> >::iterator itr=jPatches.begin(); itr!=jPatches.end(); itr++){
                result.addJetPatch((*itr).first,(*itr).second);	    
            }
        }
    }
    if(eemc) {
        result.setEemcDecision(eemc->triggerDecision(trigId));
        // fill HT/TP/JP details here
    }
    if(lTwo) {
        result.setL2Decision(lTwo->triggerDecision(trigId));
        result.setL2Result(lTwo->result());
    }
    mResults.push_back(result);
    return mResults.back();
}

Int_t StTriggerSimuMaker::Finish() {
  return StMaker::Finish();
}

/*****************************************************************************
 * $Log: StTriggerSimuMaker.cxx,v $
 * Revision 1.31  2009/11/16 07:51:19  pibero
 * Added LOG_DEBUG messages and triggerIds()
 *
 * Revision 1.30  2009/11/13 20:00:52  pibero
 * Updates for Run 9 to work on simulation
 *
 * Revision 1.29  2009/10/12 18:04:12  pibero
 * Moved StEEmcUtil/EEdsm to StTriggerUtilities/Eemc
 *
 * Revision 1.28  2009/09/26 18:46:28  pibero
 * Migration from ROOT MySQL to STAR DB API
 *
 * Revision 1.27  2009/09/23 22:35:30  pibero
 * Removed dependencies on ROOT MySQL
 *
 * Revision 1.26  2009/09/20 06:46:29  pibero
 * Updates for Run 9
 *
 * Revision 1.25  2009/02/04 20:00:48  rfatemi
 * change includes for StEmcDecoder
 *
 * Revision 1.24  2009/02/03 15:40:38  rfatemi
 * Changed structure of mSimulators to accomodate 2009 EMC simulator update
 *
 * Revision 1.23  2009/01/17 13:09:02  pibero
 * Initial Version of EMC DSM algorithms for 2009
 *
 * Revision 1.22  2008/01/22 18:06:26  kocolosk
 * added detailedResult code for BEMC L0, courtesy Dave Staszak
 * fixed two bugs in vector accessors in result class (also thanks to Dave)
 *
 * Revision 1.21  2008/01/17 17:04:07  kocolosk
 * some revisions to StTriggerSimuResult structure to hopefully improve clarity and maintainability
 *
 * Revision 1.20  2008/01/17 01:58:25  kocolosk
 * StTriggerSimuResult makes detailed emulation results persistent
 *
 *
 *****************************************************************************/
