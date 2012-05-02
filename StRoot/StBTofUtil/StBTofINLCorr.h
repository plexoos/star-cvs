/*******************************************************************
 *
 * $Id: StBTofINLCorr.h,v 1.4 2009/12/14 19:38:30 dongx Exp $
 *
 * Author: Xin Dong
 *****************************************************************
 *
 * Description: INL correction for all TDIG board channels
 *
 *****************************************************************
 *
 * $Log: StBTofINLCorr.h,v $
 * Revision 1.4  2009/12/14 19:38:30  dongx
 * - mNValidBoards set by the read-in database entrie instead of a hard-coded number
 * - clean up mNValidTrays and related functions (not needed since previous versions)
 *
 * Revision 1.3  2009/03/04 04:57:36  dongx
 * INL arrays changed from float to short - memory occupied reduced by a factor of 2
 *
 * Revision 1.2  2009/02/13 23:32:52  dongx
 * fixed the crash when no INL table for some board is available
 *
 * Revision 1.1  2009/02/02 21:57:51  dongx
 * first release - Barrel TOF INL correction functions
 *
 *
 *******************************************************************/
#ifndef STBTOFINLCORR_H
#define STBTOFINLCORR_H

#include "StObject.h"
#include "StMaker.h"
#include <assert.h>
#include "Stypes.h"
#include "TDataSet.h"
#include "TDataSetIter.h"
#include "TObjectSet.h"
#include <string>
#include "StEnumerations.h"

/**
   \class StBTofINLCorr
   Class to retrieve the INL tables from data base
 */
class StBTofINLCorr{
 private:
  static const Int_t mNTray = 120;
  static const Int_t mNTDIGOnTray = 8;
  static const Int_t mNGLOBALCHANMAX = 192;

  static const Int_t mNTDIGMAX = 1200;
  static const Int_t mNChanOnTDIG = 24;
  static const Int_t mNChanMAX = 1024;
  static const Int_t mNBoardIdMAX = 4800;

  static const Int_t mEastVpdTrayId = 122;
  static const Int_t mWestVpdTrayId = 121;

  Int_t mTdigOnTray[mNTray][mNTDIGOnTray];
  Int_t mTdigOnEastVpd[mNTDIGOnTray];
  Int_t mTdigOnWestVpd[mNTDIGOnTray];

  Int_t mBoardId[mNTDIGMAX];
  Int_t mBoardId2Index[mNBoardIdMAX];   // index in mNTDIGMAX for board #Id
  Short_t mINLCorr[mNTDIGMAX][mNChanOnTDIG][mNChanMAX];

  Int_t mNValidBoards;
  
 public:
  StBTofINLCorr();
  ~StBTofINLCorr();

  void init();
  void init(StMaker *maker);
  /// Initial function to access the data base and retrieve INL tables
  void initFromDbase(StMaker *maker);
  void Reset();

  /// To get the INL correction tables for trays
  float getTrayINLCorr(int trayId, int globalTdcChan, int bin);
  /// To get the INL correction tables for vpds
  float getVpdINLCorr(StBeamDirection eastwest, int globalTdcChan, int bin);
  
};

#endif
