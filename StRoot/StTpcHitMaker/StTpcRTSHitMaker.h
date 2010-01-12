#ifndef StTpcMcHitMaker_H
#define StTpcRTSHitMaker_H

/***************************************************************************
 *
 * $Id: StTpcRTSHitMaker.h,v 1.6 2010/01/12 22:55:17 fisyak Exp $
 * StTpcRTSHitMaker - class to runonline (RTS) cluster maker over StTpcRawData
 * $Log: StTpcRTSHitMaker.h,v $
 * Revision 1.6  2010/01/12 22:55:17  fisyak
 * Add minimum cluster charge in ADC for <= 20090101
 *
 * Revision 1.5  2009/11/25 21:34:04  fisyak
 * replace daqReader by daq_tpx
 *
 * Revision 1.4  2009/11/10 21:05:08  fisyak
 * Add attributes for sector and pad  row selections
 *
 * Revision 1.3  2009/10/07 13:46:35  fine
 * replace  the obsolete rts_reader with the modern daqReader
 *
 * Revision 1.2  2008/12/15 21:04:01  fine
 * For for the NEW_DAQ_READER
 *
 * Revision 1.1.1.1  2008/05/27 14:22:41  fisyak
 * Maker to access TPC DAQ information via EVP_READER
 *
 * Revision 1.1  2008/04/28 14:37:16  fisyak
 * Rearrage TpcHitMaker to make it run for parallel taks, add the first version of online clustering
 *
 * Revision 1.1.1.1  2008/04/03 20:16:39  fisyak
 * Initial version
 *
 *--------------------------------------------------------------------------
 *
 ***************************************************************************/

#include "StMaker.h"

class StTpcDigitalSector;

#include "StDAQMaker/StRtsReaderMaker.h"
class daq_tpx;
class StTpcRTSHitMaker : public StMaker {
 public:
  StTpcRTSHitMaker(const char *name="tpc_hits") : StMaker(name), fTpx(0), fminCharge(0) {}
  virtual ~StTpcRTSHitMaker();
  
  Int_t               Init();
  Int_t               InitRun(Int_t runumber);
  Int_t               Make();
 private:
  daq_tpx *fTpx; //!
  Double_t fminCharge; // ! minimum cluster charge in ADC
  // cvs
  virtual const char *GetCVS() const    {
    static const char cvs[]="Tag $Name:  $Id: built "__DATE__" "__TIME__ ; return cvs;
  }
  ClassDef(StTpcRTSHitMaker, 1)    //StTpcRTSHitMaker - class to fille the StEvewnt from DAQ reader
};

#endif
