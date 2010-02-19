#ifndef StTpcHitMaker_H
#define StTpcHitMaker_H

/***************************************************************************
 *
 * $Id: StTpcHitMaker.h,v 1.9 2010/02/19 23:36:08 fisyak Exp $
 * StTpcHitMaker - class to fill the StEvent with TPC clusters from DAQ reader
 * $Log: StTpcHitMaker.h,v $
 * Revision 1.9  2010/02/19 23:36:08  fisyak
 * Add hit Id
 *
 * Revision 1.8  2009/03/16 13:41:45  fisyak
 * Switch to new scheme (avoid legacy) for TPX cluster reading
 *
 * Revision 1.7  2009/03/11 18:38:20  fisyak
 * Add 22 time bins to account subtracted by Tonko, clean up
 *
 * Revision 1.6  2008/12/29 23:58:07  fine
 * Optimize the DAQ data access
 *
 * Revision 1.5  2008/12/18 20:20:26  fine
 * access two different detectors tpx/tpc
 *
 * Revision 1.4  2008/12/15 21:04:01  fine
 * For for the NEW_DAQ_READER
 *
 * Revision 1.3  2008/07/31 20:45:27  fisyak
 * Add TpcMixer
 *
 * Revision 1.2  2008/06/23 20:13:53  fisyak
 * Add real data pixel annotation
 *
 * Revision 1.1.1.1  2008/05/27 14:22:41  fisyak
 * Maker to access TPC DAQ information via EVP_READER
 *
 * Revision 1.3  2008/05/27 14:18:18  fisyak
 * Freeze before moving to STAR repository
 *
 * Revision 1.2  2008/04/28 14:37:15  fisyak
 * Rearrage TpcHitMaker to make it run for parallel taks, add the first version of online clustering
 *
 * Revision 1.1.1.1  2008/04/03 20:16:39  fisyak
 * Initial version
 *
 *--------------------------------------------------------------------------
 *
 ***************************************************************************/

#include "StRTSBaseMaker.h"
#include "TString.h"
class StTpcDigitalSector;
class StTpcHit;
class tpc_cl;
class daq_cld;
class tpc_t;

class StTpcHitMaker : public StRTSBaseMaker {
 public:
  enum EReaderType {kUnknown, kLegacyTpc, kLegacyTpx, kStandardTpx};
  enum EMode {kUndefined, 
	      kTpc, kTpx, 
	      kTpcPulser, kTpxPulser, 
	      kTpcPadMonitor, kTpxPadMonitor, 
	      kTpcDumpPxls2Nt, kTpxDumpPxls2Nt, 
	      kTpcRaw, kTpxRaw, 
	      kAll};
    StTpcHitMaker(const char *name="tpc_hits") : StRTSBaseMaker("tpc",name), kMode(kUndefined),
      kReaderType(kUnknown), mQuery(""), fTpc(0) {}
  virtual ~StTpcHitMaker() {}

  Int_t   Init();
  Int_t   Make();
  void    DoPulser(Int_t sector);
  void    PadMonitor(Int_t sector);
  void    UpdateHitCollection(Int_t sector);
  void    DumpPixels2Ntuple(Int_t sector);
  void    PrintSpecial(Int_t sector);
  Int_t   RawTpcData(Int_t sector);
  Int_t   RawTpxData(Int_t sector);
  StTpcDigitalSector *GetDigitalSector(Int_t sector);
 private:

  EMode   kMode;
  EReaderType kReaderType;
  TString mQuery;
  tpc_t   *fTpc;
  Short_t  ADCs[512];
  UShort_t IDTs[512];
  UShort_t fId; // current cluster Id
 protected:
  StTpcHit *CreateTpcHit(const tpc_cl &cluster, Int_t sector, Int_t row);
  StTpcHit *CreateTpcHit(const daq_cld  &cluster, Int_t sector, Int_t row);
    
 public:

  static Float_t fgDp;             // hardcoded errors
  static Float_t fgDt;
  static Float_t fgDperp;

 public:

  // cvs
  virtual const char *GetCVS() const    {
    static const char cvs[]="Tag $Name:  $Id: built "__DATE__" "__TIME__ ; return cvs;
  }
  ClassDef(StTpcHitMaker, 1)    //StTpcHitMaker - class to fille the StEvewnt from DAQ reader
};

#endif
