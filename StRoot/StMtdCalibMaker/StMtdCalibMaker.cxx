/*******************************************************************
 *
 *  StMtdCalibMaker.cxx,v 1.0 2014/05/27
 *
 * Author: Xinjie Huang
 *****************************************************************
 *
 * Description: - Mtd Calibration Maker to do the calibration for Mtd
 *
 *              - store into StMtdPidTraits
 *
 *************************************************************************/

#include <iostream>
#include "TH1F.h"
#include "TH2F.h"

#include "StEvent.h"
#include "StBTofCollection.h"
#include "StBTofHeader.h"
#include "StEventTypes.h"
#include "Stypes.h"
#include "StThreeVectorD.hh"
#include "StHelix.hh"
#include "StTrackGeometry.h"
#include "StTrackPidTraits.h"
#include "PhysicalConstants.h"

#include "StBTofCollection.h"
#include "StBTofHeader.h"
#include "StMtdCollection.h"
#include "StMtdHeader.h"

#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuMtdCollection.h"
#include "StMuDSTMaker/COMMON/StMuMtdRawHit.h"
#include "StMuDSTMaker/COMMON/StMuMtdHeader.h"
#include "StMuDSTMaker/COMMON/StMuMtdHit.h"

#include "tables/St_mtdSlewingCorr_Table.h"
#include "tables/St_mtdT0Offset_Table.h"
#include "tables/St_mtdTriggerTimeCut_Table.h"

#include "StMtdCalibMaker.h"

ClassImp(StMtdCalibMaker)

//_____________________________________________________________________________
StMtdCalibMaker::StMtdCalibMaker(const char *name) : StMaker(name)
{
  /// default constructor

  mStEvent                = 0x0;
  mMuDst                  = 0x0;
  mDebug                  = kFALSE;
  mHisto                  = kFALSE;
  mUseTriggercut          = kFALSE;
  mInitFromFile           = kFALSE;  // default initialization from database
  mCalibFileTot           = "";
  mCalibFileT0            = "";
  mCalibFileTrigger       = "";
  hTimeOfFlightCorr       = 0;
  hAllCorr                = 0;
  hTimeOfFlightCorrModule = 0;
  hTriggerTimeBL          = 0;
  hVertexzVsTpcz          = 0;
  hTOFTimeOfFlightTray    = 0;
}

//_____________________________________________________________________________
StMtdCalibMaker::~StMtdCalibMaker()
{ 
  /// default destructor
  if(hTimeOfFlightCorr)       delete hTimeOfFlightCorr;
  if(hAllCorr)                delete hAllCorr;
  if(hTimeOfFlightModule)     delete hTimeOfFlightModule;
  if(hTimeOfFlightCorrModule) delete hTimeOfFlightCorrModule;
  if(hTriggerTimeBL)          delete hTriggerTimeBL;
  if(hVertexzVsTpcz)          delete hVertexzVsTpcz;
  if(hTOFTimeOfFlightTray)    delete hTOFTimeOfFlightTray;
}

//____________________________________________________________________________
Int_t StMtdCalibMaker::Init()
{
  if (mHisto) bookHistograms();

  return kStOK;
}

//____________________________________________________________________________
Int_t StMtdCalibMaker::InitRun(Int_t runnumber)
{
  //return kStOK;
  //initialize correction table
  Int_t dblSize = sizeof(Double_t);
  memset(mMtdT0Corr,      0, mNBackleg*mNModule*mNCell*dblSize);
  memset(mMtdTotCorr,     0, mNBackleg*mNModule*mNBinMax*dblSize);
  memset(mMtdTotEdge,     0, mNBackleg*mNModule*mNBinMax*dblSize);
  memset(mTriggerHighEdge,0, mNBackleg*mNModule*dblSize);
  memset(mTriggerLowEdge, 0, mNBackleg*mNModule*dblSize);

  if(!mInitFromFile) //init from database
    {
      LOG_INFO << "Initializing calibration parameters from data base" << endm;
      TDataSet *mDbDataSet = 0x0;

      // extract T0 offset parameters
      mDbDataSet = GetDataBase("Calibrations/mtd/mtdT0Offset");
      St_mtdT0Offset* mtdT0Offset = static_cast<St_mtdT0Offset*>(mDbDataSet->Find("mtdT0Offset"));
      if(!mtdT0Offset) 
	{
	  LOG_ERROR << "Unable to get the T0 offset parameters from data base" << endm;
	  return kStErr;
	}
      mtdT0Offset_st* tZero = static_cast<mtdT0Offset_st*>(mtdT0Offset->GetTable());
      for(int j=0; j<mNBackleg*mNModule*mNCell; j++)
	{
	  Int_t backleg = j/60;
	  Int_t module  = (j-backleg*60)/12;
	  Int_t cell    =  j-backleg*60-module*12;
	  mMtdT0Corr[backleg][module][cell] = tZero->t0Offset[j];
	}
 
      // extract slewing correction parameters
      mDbDataSet = GetDataBase("Calibrations/mtd/mtdSlewingCorr");
      St_mtdSlewingCorr* mtdSlewingCorr = static_cast<St_mtdSlewingCorr*>(mDbDataSet->Find("mtdSlewingCorr"));
      if(!mtdSlewingCorr) {
	LOG_ERROR << "Unable to get the slewing correction parameters from data base" << endm;
	return kStErr;
      }
      mtdSlewingCorr_st* slewingCorr = static_cast<mtdSlewingCorr_st*>(mtdSlewingCorr->GetTable());
      for(int j=0; j<mNBackleg*mNModule*mNBinMax; j++)
	{
	  Int_t backleg = j/100;
	  Int_t module  = (j-backleg*100)/20;
	  Int_t bin     = j-backleg*100-module*20;
	  mMtdTotEdge[backleg][module][bin] = slewingCorr->slewingBinEdge[j];
	  mMtdTotCorr[backleg][module][bin] = slewingCorr->slewingCorr[j];
	}

      if(mUseTriggercut)
	{
	  // extract trigger time window cuts
	  mDbDataSet = GetDataBase("Calibrations/mtd/mtdTriggerTimeCut");
	  St_mtdTriggerTimeCut* mtdTriggerTimeCut = static_cast<St_mtdTriggerTimeCut*>(mDbDataSet->Find("mtdTriggerTimeCut"));
	  if(!mtdTriggerTimeCut) {
	    LOG_ERROR << "Unable to get the trigger time window cuts from data base" << endm;
	    return kStErr;
	  }
	  mtdTriggerTimeCut_st* trigTimeCut = static_cast<mtdTriggerTimeCut_st*>(mtdTriggerTimeCut->GetTable());
	  for(int j=0; j<mNBackleg*mNModule; j++)
	    {
	      mTriggerLowEdge[j/5][j%5]  = trigTimeCut->minTriggerTime[j];
	      mTriggerHighEdge[j/5][j%5] = trigTimeCut->maxTriggerTime[j];
	    }
	}
      LOG_INFO << "Done initializing calibration parameters from data base" << endm;
    }
  else
    {
      LOG_INFO << "Initializing calibration parameters from local files" << endm;
      ifstream inData;

      //load T0 offset parameters from local file
      if (mCalibFileT0.length()==0)
	{
	  LOG_ERROR << "Please input the local file path for T0 offset parameters" << endm;
	  return kStErr;
	}
      if (mDebug) { LOG_INFO << " Local file for T0 offset : " << mCalibFileT0 << endm; }
      inData.open(mCalibFileT0.c_str());
      if(!inData.is_open())
	{
	  LOG_ERROR << "Unable to get the T0 offset parameters from local file" <<endm;
	  LOG_ERROR << "Check if this file exists: " << mCalibFileT0.c_str() << endm;
	  return kStErr;
	}
      Int_t backlegId, moduleId, cellId;
      Int_t nbin;
      Double_t t0Corr;

      for(Int_t i=0;i<mNBackleg;i++) 
	{
	for(Int_t j=0;j<mNModule;j++) 
	  {
	  for(Int_t l=0;l<mNCell;l++)
	    {
	      inData>>backlegId>>moduleId>>cellId;
	      inData>>t0Corr;
	      mMtdT0Corr[backlegId-1][moduleId-1][cellId]=t0Corr;
	      if (mDebug) { LOG_INFO << "mMtdT0Corr=" <<mMtdT0Corr[backlegId-1][moduleId-1][cellId] << endm;}
	    }
	  }
	}
      inData.close();

      //load slewing correction parameters from local file
      if (mCalibFileTot.length()==0)
	{
	  LOG_ERROR << "Please input the local file path for slewing correction parameters" << endm;
	  return kStErr;
	}
      if (mDebug) { LOG_INFO << " Local file for slewing correction : " << mCalibFileTot << endm; }
      inData.open(mCalibFileTot.c_str());
      if(!inData.is_open())
	{
	  LOG_ERROR << "Unable to get the slewing correction parameters from local file" <<endm;
	  LOG_ERROR << "Check if this file exists: " << mCalibFileTot.c_str() << endm;
	  return kStErr;
	}

      for(Int_t i=0;i<mNBackleg;i++)
	{
	  for(Int_t j=0;j<mNModule;j++) 
	    {
	      inData>>backlegId>>moduleId;
	      inData>>nbin;
	      if (mDebug) { LOG_INFO << "BL,MOD=" <<backlegId<<","<<moduleId<<" ,bin="<<nbin << endm; }
	      for(Int_t k=0;k<nbin;k++)
		{
		  inData>>mMtdTotEdge[backlegId-1][moduleId-1][k];
		  if (mDebug)  { LOG_INFO << "edge=" <<mMtdTotEdge[backlegId-1][moduleId-1][k] << endm; }
		}

	      for(Int_t k=0;k<nbin;k++)
		{
		  inData>>mMtdTotCorr[backlegId-1][moduleId-1][k];
		  if (mDebug)  { LOG_INFO << "Corr=" <<mMtdTotCorr[backlegId-1][moduleId-1][k] << endm; }
		}
	    }
	}
      inData.close();

      if(mUseTriggercut)
	{
	  //load trigger time window cuts from local file
	  if (mCalibFileTrigger.length()==0)
	    {
	      LOG_ERROR << "Please input the local file path for trigger time window cuts" << endm;
	      return kStErr;
	    }
	  if (mDebug) { LOG_INFO << " Local file for trigger time window cuts : " << mCalibFileTrigger << endm; }

	  inData.open(mCalibFileTrigger.c_str());
	  if(!inData.is_open())
	    {
	      LOG_ERROR << "Unable to get the trigger time window cuts from local file" <<endm;
	      LOG_ERROR << "Check if this file exists: " << mCalibFileTrigger.c_str() << endm;
	      return kStErr;
	    }
	  for(Int_t i=0;i<mNBackleg;i++) 
	    {
	      for(Int_t j=0;j<mNModule;j++) 
		{
		  inData>>backlegId>>moduleId>>mTriggerLowEdge[i][j]>>mTriggerHighEdge[i][j];
		  if (mDebug) { LOG_INFO << "triglow = " <<mTriggerLowEdge[i][j]<<", trighigh = " <<mTriggerHighEdge[i][j] << endm; }
		}
	    }
	  inData.close();
	}
    }

  return kStOK;
}


//_____________________________________________________________________________
Int_t StMtdCalibMaker::Make()
{
  if (mDebug) { LOG_INFO << "StMtdCalibMaker::Maker: starting ..." << endm; }

  mStEvent = (StEvent*) GetInputDS("StEvent");
  if(mStEvent)
    {
      if (mDebug) { LOG_INFO << "Running on StEvent ..." << endm; }
      processStEvent();
      return kStOK;
    }
  else
    {
      StMuDstMaker *muDstMaker = (StMuDstMaker*) GetMaker("MuDst");
      if(muDstMaker) 
	{
	  mMuDst = muDstMaker->muDst();
	  if (mMuDst)
	    {
	      if (mDebug) { LOG_INFO << "Running on muDst ..." << endm; }
	      processMuDst();
	      return kStOK;
	    }
	}
    }

  LOG_WARN << "Neither StEvent nor muDst is available!" << endm;
  return kStWarn;
}

//_____________________________________________________________________________
void StMtdCalibMaker::processStEvent()
{
  // Get collision start time from VPD
  StBTofCollection *btofCollection = mStEvent->btofCollection();
  if (!btofCollection) return;
  StBTofHeader *bTofHeader = btofCollection->tofHeader();
  if (!bTofHeader) return;
  Double_t tStartTime = bTofHeader->tStart();
  if (mDebug) { LOG_INFO << " Collision start time =" <<tStartTime << endm; }

  StMtdCollection *mtdCollection = mStEvent->mtdCollection();
  if(!mtdCollection)
    {
      LOG_WARN << "No Mtd collection is available ..." << endm;
      return;
    }

  // Get trigger time recorded in THUB
  StMtdHeader *mtdHeader = mtdCollection->mtdHeader();
  Double_t mtdTriggerTime[2] = {0,0};
  if (!mtdHeader)
    {
      LOG_WARN << "No mtd header ..." << endm;
      return;
    }
  mtdTriggerTime[0] = 25.*(mtdHeader->triggerTime(0)&0xfff);
  mtdTriggerTime[1] = 25.*(mtdHeader->triggerTime(1)&0xfff);

  StSPtrVecMtdHit& mtdHits = mtdCollection->mtdHits();
  Int_t nMtdHits = mtdHits.size();

  // loop over all the MTD hits
  for(Int_t i=0; i<nMtdHits; i++)
    {
      StMtdHit *aHit = mtdHits[i];
      if(!aHit) continue;
      Int_t backlegId = aHit->backleg();
      Int_t moduleId  = aHit->module();
      Int_t cellId    = aHit->cell();
      pair<Double_t,Double_t>  leadTime =  aHit->leadingEdgeTime();//ns

      //trigger time cut, and fill trigger histo
      Int_t thub = 2;
      if(backlegId>=16 && backlegId<=30) thub = 1;
      Double_t triggerTime = (leadTime.first+leadTime.second)/2-mtdTriggerTime[thub-1];
      while (triggerTime<-51200) triggerTime+=51200;
      if (mUseTriggercut && 
	  (triggerTime<mTriggerLowEdge[backlegId-1][moduleId-1]|| triggerTime>mTriggerHighEdge[backlegId-1][moduleId-1])) 
	continue;
      if (mHisto) hTriggerTimeBL->Fill((backlegId-1)*5+(moduleId-1),triggerTime);

      //time of flight before calibration
      Double_t timeOfFlight = (leadTime.first+leadTime.second)/2-tStartTime; 
      while (timeOfFlight>51200) timeOfFlight-=51200;
      while (timeOfFlight<-51200) timeOfFlight+=51200;
      if (mHisto) hTimeOfFlightModule->Fill((backlegId-1)*5+moduleId-1,timeOfFlight);

      // Get the calibration parameters for T0 and slewing
      pair<Double_t,Double_t> totTime = aHit->tot();
      Double_t tot = sqrt(totTime.first*totTime.second);//geometric mean of tot
      Double_t AllCorr = mtdAllCorr(tot,backlegId,moduleId,cellId);
      if (mDebug) { LOG_INFO << "TOT = "<< tot << endm; }

      // Apply the calibration parameters
      timeOfFlight += AllCorr;
      if (mHisto) hAllCorr->Fill(AllCorr);
      if (mHisto) hTimeOfFlightCorr->Fill(timeOfFlight);
      if (mHisto) hTimeOfFlightCorrModule->Fill((backlegId-1)*5+moduleId-1,timeOfFlight);

      // Save calibrated time-of-flight to mtd Pidtraits
      // global track
      StTrack *gTrack = aHit->associatedTrack();
      if(!gTrack) continue;
      StSPtrVecTrackPidTraits& traits = gTrack->pidTraits();
      StMtdPidTraits *mtdPid = 0;
      for(UInt_t it=0; it<traits.size(); it++)
	{
	  if(traits[it]->detector()==kMtdId)
	    {
	      mtdPid = dynamic_cast<StMtdPidTraits*>(traits[it]);
	      break;
	    }
	}
      if(!mtdPid) continue;
      mtdPid->setTimeOfFlight(timeOfFlight);
      if (mDebug) 
	{ LOG_INFO << "Time-of-flight = " <<timeOfFlight <<",  Calibration parameter = "<<AllCorr << endm; }

      // primary track
      StPrimaryTrack *pTrack = dynamic_cast<StPrimaryTrack *>(gTrack->node()->track(primary));
      StMtdPidTraits *pmtdPid = 0;
      traits = pTrack->pidTraits();
      for(UInt_t it=0; it<traits.size(); it++)
	{
	  if(traits[it]->detector()==kMtdId)
	    {
	      pmtdPid = dynamic_cast<StMtdPidTraits*>(traits[it]);
	      break;
	    }
	}
      if(!pmtdPid) continue;
      pmtdPid->setTimeOfFlight(timeOfFlight);
    }
  return;
}


//_____________________________________________________________________________
void StMtdCalibMaker::processMuDst()
{
  // Get primary vertex position
  StMuEvent* mEvent = mMuDst->event();
  if (!mEvent)
    {
      LOG_WARN << "No muEvent is available ..." << endm;
      return;
    }
  Double_t tpcvz = mEvent->primaryVertexPosition().z();

  // Get collision start time & vertex posision from VPD
  StBTofHeader *bTofHeader = mMuDst->btofHeader();
  if(!bTofHeader)
    {
      LOG_WARN << "No TOF header is available ..." << endm;
      return;
    }
  Double_t tStartTime = bTofHeader->tStart();
  Double_t vpdvz = bTofHeader->vpdVz();
  if (tStartTime==-9999) return;
  if (mHisto) hVertexzVsTpcz->Fill(vpdvz,tpcvz); //check vertex distribution

  // Get trigger time recorded in THUB
  Int_t nhits = mMuDst->numberOfMTDHit();
  if (mDebug) { LOG_INFO << "Total number of MTD hits is: " << nhits << endm; }
  Double_t mtdTriggerTime[2] = {0,0};
  StMuMtdHeader *muMtdHeader = mMuDst->mtdHeader();
  if(!muMtdHeader)
    {
      LOG_WARN << "No MTD header is available ..." << endm;
      return;
    }
  mtdTriggerTime[0] = 25.*(muMtdHeader->triggerTime(1)&0xfff);
  mtdTriggerTime[1] = 25.*(muMtdHeader->triggerTime(2)&0xfff);

  // loop over all the MTD hits
  for(Int_t i=0;i<nhits;i++) 
    {
      StMuMtdHit* aHit = mMuDst->mtdHit(i) ;
      if(!aHit) continue;
      Int_t backlegId = aHit->backleg();
      Int_t moduleId = aHit->module();
      Int_t cellId = aHit->cell();
      pair<Double_t,Double_t>  leadTime  = aHit->leadingEdgeTime();//ns
      pair<Double_t,Double_t>  trailTime = aHit->trailingEdgeTime();

      //trigger time cut, and fill trigger histo
      Int_t thub = 2;
      if(backlegId>=16 && backlegId<=30) thub = 1;
      Double_t triggerTime = (leadTime.first+leadTime.second)/2-mtdTriggerTime[thub-1];
      while (triggerTime<-51200) triggerTime+=51200;
      if (mUseTriggercut && 
	  (triggerTime<mTriggerLowEdge[backlegId-1][moduleId-1]|| triggerTime>mTriggerHighEdge[backlegId-1][moduleId-1])) 
	continue;
      if (mHisto) hTriggerTimeBL->Fill((backlegId-1)*5+(moduleId-1),triggerTime);

      //time of flight before calibration
      Double_t timeOfFlight = (leadTime.first+leadTime.second)/2-tStartTime;
      while (timeOfFlight>51200)  timeOfFlight-=51200;
      while (timeOfFlight<-51200) timeOfFlight+=51200;
      if (mHisto) hTimeOfFlightModule->Fill((backlegId-1)*5+moduleId-1,timeOfFlight);

     // Get the calibration parameters for T0 and slewing
      Double_t totFirst = trailTime.first-leadTime.first;
      if (totFirst<0) totFirst+=51200;
      Double_t totSecond = trailTime.second-leadTime.second;
      if (totSecond<0) totSecond+=51200;
      Double_t tot = sqrt(totFirst*totSecond);
      Double_t AllCorr = mtdAllCorr(tot,backlegId,moduleId,cellId);
      if (mDebug) { LOG_INFO << "TOT = "<< tot << endm; }

      // Apply the calibration parameters
      timeOfFlight += AllCorr;
      if (mHisto) hAllCorr->Fill(AllCorr);
      if (mHisto) hTimeOfFlightCorr->Fill(timeOfFlight);
      if (mHisto) hTimeOfFlightCorrModule->Fill((backlegId-1)*5+moduleId-1,timeOfFlight);


      // Save calibrated time-of-flight to mtd Pidtraits
      // global track
      Int_t index = aHit->index2Global();
      if (mDebug) { LOG_INFO << "Associated global track index = "<< index << endm; }
      if(index<0) continue;
      StMuTrack *gTrack = mMuDst->globalTracks(index);
      if(!gTrack) continue;
      StMuMtdPidTraits pidMtd = gTrack->mtdPidTraits();
      pidMtd.setTimeOfFlight (timeOfFlight);
      if (mDebug) { LOG_INFO << "Time-of-flight = " <<timeOfFlight <<",  Calibration parameter = "<<AllCorr << endm; }

      //primary track
      StMuTrack *pTrack = mMuDst->primaryTracks(index);
      if(!pTrack) continue;
      StMuMtdPidTraits ppidMtd = gTrack->mtdPidTraits();
      ppidMtd.setTimeOfFlight (timeOfFlight);
    }  // end mtd hits loop

  return;
}

//_____________________________________________________________________________
Double_t StMtdCalibMaker::mtdAllCorr(const Double_t tot, const Int_t iBackleg, const Int_t iModule, const Int_t iCell)
{
  // return the calibration parameters for T0 and slewing combined
  Int_t backleg = iBackleg-1;
  Int_t module = iModule-1;
  Int_t cell = iCell;
  Double_t mtdCorr = 0;
  mtdCorr += mMtdT0Corr[backleg][module][cell]; // T0 offset

  // slewing correction
  Int_t iTotBin = -1;
  for(Int_t i=0;i<mNBinMax-1;i++) 
    {
      if(tot>=mMtdTotEdge[backleg][module][i] && tot<mMtdTotEdge[backleg][module][i+1]) 
        {
          iTotBin = i;
          break;
        }
    }
  if(iTotBin>=0&&iTotBin<mNBinMax) 
    {
      Double_t x1 = mMtdTotEdge[backleg][module][iTotBin];
      Double_t x2 = mMtdTotEdge[backleg][module][iTotBin+1];
      Double_t y1 = mMtdTotCorr[backleg][module][iTotBin];
      Double_t y2 = mMtdTotCorr[backleg][module][iTotBin+1];
      mtdCorr += y1 + (tot-x1)*(y2-y1)/(x2-x1);
    } 
  else 
    {
      if (mDebug) { LOG_INFO << " TOT out of range! totCorr set 0! Tot=" << tot <<"  ,BL,MOD="<<iBackleg<<","<<iModule<< endm; }
    }
  return mtdCorr;
}

//_____________________________________________________________________________
void StMtdCalibMaker::bookHistograms()
{
  hTimeOfFlightCorr       = new TH1D("hTimeOfFlightCorr","hTimeOfFlightCorr",4000,-1000,1000);
  hAllCorr                = new TH1D("hAllCorr","hAllCorr",1000,-500,500);
  hTimeOfFlightModule     = 
    new TH2F("hTimeOfFlightModule","hTimeOfFlightModule",mNBackleg*mNModule,0,mNBackleg*mNModule,1000,-500,500);
  hTimeOfFlightCorrModule = 
    new TH2F("hTimeOfFlightCorrModule","hTimeOfFlightCorrModule",mNBackleg*mNModule,0,mNBackleg*mNModule,1000,-500,500);
  hTriggerTimeBL          = new TH2F("hTriggerTimeBL","hTriggerTimeBL",150,0,150,400,2700,3100);
  hVertexzVsTpcz          = new TH2F("hVertexzVsTpcz","hVertexzVsTpcz",800,-100,100,800,-100,100);
  hTOFTimeOfFlightTray    = new TH2F("hTOFTimeOfFlightTray","hTOFTimeOfFlightTray",120,0,120,1000,-1000,1000);

  AddHist(hTimeOfFlightCorr);
  AddHist(hAllCorr);
  AddHist(hTimeOfFlightModule);
  AddHist(hTimeOfFlightCorrModule);
  AddHist(hTriggerTimeBL);
  AddHist(hVertexzVsTpcz);
  AddHist(hTOFTimeOfFlightTray);
  return;
}


//
// $Id: StMtdCalibMaker.cxx,v 1.1 2014/10/09 16:59:52 jeromel Exp $
// $Log: StMtdCalibMaker.cxx,v $
// Revision 1.1  2014/10/09 16:59:52  jeromel
// Reviewed version of the MtdCalibMaker - first commit
//
