// $Id: StEEsoloPi0Maker.h,v 1.4 2004/08/26 04:39:40 balewski Exp $

#ifndef STAR_StEEsoloPi0Maker
#define STAR_StEEsoloPi0Maker

/*!
 *                                                                     
 * \class  StEEsoloPi0Maker
 * \author Balewski
 * \date   
 * \brief  
 *
 Axample to access muDst and pass it to ezTree analyzis class
 */                                                                      

#ifndef StMaker_H
#include "StMaker.h"
#endif
class TObjArray  ;
#include "EEsoloPi0.h"
class StMuDstMaker;

class StEEsoloPi0Maker : public StMaker, public  EEsoloPi0 {
 private:
  StMuDstMaker* mMuDstMaker;
  int   unpackMuDst();
  int MCflag;

  // ideal calibration used by Fast simulator 
  float * mfixEmTgain; ///<  (adc=g*de )ideal electromagnetic gains for Towers
  float mfixPgain; ///< (adc=g*de ) fixed gain for pre/post shower
  float mfixSMDgain; ///< (adc=g*de ) fixed gain for SMD

  // old:
  int getEEmcAdc();
  float getCtbSum();
  bool getTrig();

 public: 
  StEEsoloPi0Maker(const char *self="stEEsoloPi0", const char* muDstMakerName="muDstMaker");
  virtual       ~StEEsoloPi0Maker();
  virtual Int_t Init();
  virtual Int_t InitRun(int runNo);
  virtual Int_t Finish();

  virtual Int_t  Make();
  void SetHList(TObjArray * x){HList=x;}
  void SetMCflag(int x=1) {MCflag=x;}

  /// Displayed on session exit, leave it as-is please ...
  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StEEsoloPi0Maker.h,v 1.4 2004/08/26 04:39:40 balewski Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  }

  ClassDef(StEEsoloPi0Maker, 1)   //StAF chain virtual base class for Makers
};

#endif


// $Log: StEEsoloPi0Maker.h,v $
// Revision 1.4  2004/08/26 04:39:40  balewski
// towards pi0
//
// Revision 1.3  2004/08/09 20:28:31  balewski
// add trig selection
//
// Revision 1.2  2004/04/14 19:34:01  balewski
// access to trigger data
//
// Revision 1.1  2004/04/14 17:09:09  balewski
// new copy of pi0finder with towers only, should work on ezTree as well (after small cleanup)
//
