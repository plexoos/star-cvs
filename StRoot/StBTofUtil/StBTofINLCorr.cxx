/*******************************************************************
 *
 * $Id: StBTofINLCorr.cxx,v 1.3 2009/02/13 23:32:52 dongx Exp $
 *
 * Author: Xin Dong
 *****************************************************************
 *
 * Description: INL correction for all TDIG board channels
 *
 *****************************************************************
 *
 * $Log: StBTofINLCorr.cxx,v $
 * Revision 1.3  2009/02/13 23:32:52  dongx
 * fixed the crash when no INL table for some board is available
 *
 * Revision 1.2  2009/02/13 22:59:02  dongx
 * new tofINLSCorr table for full barrel system for Run 9++
 *
 * Revision 1.1  2009/02/02 21:57:51  dongx
 * first release - Barrel TOF INL correction functions
 *
 *
 *******************************************************************/
#include <iostream>
#include "tables/St_tofTDIGOnTray_Table.h"
#include "tables/St_tofINLSCorr_Table.h"
#include "StMessMgr.h"
#include "StMaker.h"
#include "StBTofINLCorr.h"

StBTofINLCorr::StBTofINLCorr()
{ 
  Reset();
}

StBTofINLCorr::~StBTofINLCorr()
{ 
  Reset();
}

void StBTofINLCorr::init() {
  Reset();
}

void StBTofINLCorr::init(StMaker *maker) {
  initFromDbase(maker);
}

void StBTofINLCorr::initFromDbase(StMaker *maker) {

  LOG_INFO << "StBTofINLCorr -- rertieving the INL correction table" << endm;
  ///////////////////////////////////////////////////////
  // Load configuration parameters from dbase
  //    need "[shell] setenv Calibrations_tof reconV0"
  ///////////////////////////////////////////////////////

  TDataSet *mDbTOFDataSet = maker->GetDataBase("Calibrations/tof");
  if(!mDbTOFDataSet) {
    LOG_ERROR << "unable to access Calibrations TOF parameters" << endm;
    //    assert(mDbTOFDataSet);
    return; // kStErr;
  }

  St_tofTDIGOnTray* tofTDIGOnTray = static_cast<St_tofTDIGOnTray*>(mDbTOFDataSet->Find("tofTDIGOnTray"));
  if(!tofTDIGOnTray) {
    LOG_ERROR << "unable to get tof INL correction parameters" << endm;
    //    assert(tofTDIGOnTray);
    return; // kStErr;
  }
  tofTDIGOnTray_st* tdigOnTray = static_cast<tofTDIGOnTray_st*>(tofTDIGOnTray->GetArray());

  Int_t numRows = tofTDIGOnTray->GetNRows();
//  LOG_INFO << "number of rows = " << numRows << endm;
  for (Int_t i=0;i<mNValidTrays+2;i++) {
    Int_t trayId = (Int_t)tdigOnTray[i].trayId;

    if(trayId==mEastVpdTrayId) {  // east vpd
      for(Int_t j=0;j<mNTDIGOnTray;j++)
	mTdigOnEastVpd[j] = (Int_t)tdigOnTray[i].tdigId[j];
    } else if (trayId==mWestVpdTrayId) {  // west vpd
      for(Int_t j=0;j<mNTDIGOnTray;j++)
	mTdigOnWestVpd[j] = (Int_t)tdigOnTray[i].tdigId[j];
    } else if (trayId>0 && trayId<= mNTray) { // barrel Tray
      for(Int_t j=0;j<mNTDIGOnTray;j++)
	mTdigOnTray[trayId-1][j] = (Int_t)tdigOnTray[i].tdigId[j];
    }

    if(maker->Debug()) {
      LOG_DEBUG << " tray id=" << trayId;
      for(int j=0;j<mNTDIGOnTray;j++) {
	LOG_DEBUG << "  " << tdigOnTray[i].tdigId[j];
      }
      LOG_DEBUG << endm;
    }

  }

  St_tofINLSCorr* tofINLCorr = static_cast<St_tofINLSCorr*>(mDbTOFDataSet->Find("tofINLSCorr"));
  if(!tofINLCorr) {
    LOG_ERROR << "unable to get tof INL correction parameters" << endm;
    //    assert(tofINLCorr);
    return; // kStErr;
  }
  tofINLSCorr_st* inlcorr = static_cast<tofINLSCorr_st*>(tofINLCorr->GetArray());

  numRows = tofINLCorr->GetNRows();
  if(numRows>mNTDIGMAX*mNChanOnTDIG) {
    { LOG_INFO << " !!! # of Rows in tofINLCorr table exceed the array limit in this function !!! Trancated !!! " << endm; }
  }
  Int_t NTdig = 0;
  Int_t tdigId_old = 0;
  for (Int_t i=0;i<mNValidBoards*mNChanOnTDIG;i++) {
    if(NTdig>mNTDIGMAX) {
      { LOG_INFO << " !!! # of boards read-in exceeds the array limit in this function !!! Trancated !!! " << endm; }
      NTdig = mNTDIGMAX;
      break;
    }

    int tdigId = (Int_t)(inlcorr[i].tdigId);
    int tdcChanId = (Int_t)(inlcorr[i].tdcChanId);
    if(tdigId!=tdigId_old) {
      mBoardId[NTdig] = tdigId;
      NTdig++;
    }
        
    tdigId_old = tdigId;

    if(maker->Debug()) { LOG_DEBUG << " tdigId=" << tdigId << "  tdcChanId=" << tdcChanId << endm; }
    for(Int_t j=0;j<mNChanMAX;j++) {
      float corr = (Float_t)(inlcorr[i].INLCorr[j])/100.;
      mINLCorr[NTdig-1][tdcChanId][j] = corr;
      
      if(maker->Debug()&&(j%200==0)) {
	LOG_DEBUG << " " << corr;
      }
    }
    if(maker->Debug()) LOG_DEBUG << endm;
  }

  LOG_INFO << " Total # of boards read in : " << NTdig << endm;

  // re-organize
  for(Int_t i=0;i<NTdig;i++) {
    int boardId = mBoardId[i];
    if(boardId>0 && boardId<=mNBoardIdMAX) {
      mBoardId2Index[boardId] = i;
    } else {
      { LOG_INFO << " Warning! boardId " << boardId << " out of range!" << endm; }
    }
  }


}

void StBTofINLCorr::Reset() {

  for(Int_t i=0;i<mNTray;i++) {
    for(Int_t j=0;j<mNTDIGOnTray;j++) {
      mTdigOnTray[i][j] = 0;
    }
  }
  for(Int_t i=0;i<mNTDIGOnTray;i++) {
    mTdigOnEastVpd[i] = 0;
    mTdigOnWestVpd[i] = 0;
  }

  for(Int_t i=0;i<mNTDIGMAX;i++) {
    mBoardId[i] = 0;
    for(int j=0;j<mNChanOnTDIG;j++) {
      for(int k=0;k<mNChanMAX;k++) {
	mINLCorr[i][j][k] = 0.0;
      }
    }
  }
  for(Int_t i=0;i<mNBoardIdMAX;i++) {
    mBoardId2Index[i] = -1;
  }

  mNValidTrays = 0;
}

float StBTofINLCorr::getTrayINLCorr(int trayId, int globalTdcChan, int bin) {
  if(trayId<=0 || trayId>mNTray) return 0.0;  // trayId 1-120
  if(globalTdcChan<0 || globalTdcChan>=mNGLOBALCHANMAX) return 0.0;
  if(bin<0 || bin>=mNChanMAX) return 0.0;

  int iTdig = globalTdcChan/mNChanOnTDIG;  // 0-7
  int boardId = mTdigOnTray[trayId-1][iTdig];
  if(boardId<=0 || boardId>mNBoardIdMAX) return 0.0;

  int index = mBoardId2Index[boardId];          // index in the inl array
  int tdcChan = globalTdcChan % mNChanOnTDIG;   // 0-23
  if(index<0||index>=mNValidBoards) {
    LOG_WARN << " Missing INL table for boardId = " << boardId << " on tray " << trayId << endm;
    return 0.0;
  } else {
    return mINLCorr[index][tdcChan][bin];
  }
}

float StBTofINLCorr::getVpdINLCorr(StBeamDirection eastwest, int globalTdcChan, int bin) {
  if(eastwest!=east && eastwest!=west) return 0.0;
  if(globalTdcChan<0 || globalTdcChan>=mNGLOBALCHANMAX) return 0.0;
  if(bin<0 || bin>=mNChanMAX) return 0.0;
  int iTdig = globalTdcChan/mNChanOnTDIG;  // 0-7

  int boardId;
  if(eastwest==east) { // east pvpd
    boardId = mTdigOnEastVpd[iTdig];
  } else if(eastwest==west) {  // west pvpd
    boardId = mTdigOnWestVpd[iTdig];
  }
  if(boardId<=0 || boardId>mNBoardIdMAX) return 0.0;

  int index = mBoardId2Index[boardId];          // index in the inl array
  int tdcChan = globalTdcChan % mNChanOnTDIG;   // 0-23
  if(index<0||index>=mNValidBoards) {
    LOG_WARN << " Missing INL table for boardId = " << boardId << " on vpd " << eastwest << endm;
    return 0.0;
  } else {
    return mINLCorr[index][tdcChan][bin];
  }
}
