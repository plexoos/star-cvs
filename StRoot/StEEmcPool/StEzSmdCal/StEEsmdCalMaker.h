// $Id: StEEsmdCalMaker.h,v 1.1 2004/06/12 04:09:25 balewski Exp $

#ifndef STAR_StEEsmdCalMaker
#define STAR_StEEsmdCalMaker

/*!
 *                                                                     
 * \class  StEEsmdCalMaker
 * \author Balewski
 * \date   
 * \brief  
 *
 Access EEMC data & DB  from muDst in StRoot-framework
 Only muDst data are decoded by this class 
 Uses EEsmdCal class to do any analysis
 */                                                                      

#ifndef StMaker_H
#include "StMaker.h"
#endif

class TObjArray  ;
class StMuDstMaker;

#include "EEsmdCal.h"

class StEEsmdCalMaker : public StMaker , public  EEsmdCal{

 private: 
  StMuDstMaker* mMuDstMaker;  
  int   unpackMuDst();
  int MCflag;

 public: 
  StEEsmdCalMaker(const char *self="stEEsoloPi0", const char* muDstMakerName="muDstMaker");
  virtual       ~StEEsmdCalMaker();
  virtual Int_t Init();
  virtual Int_t Finish();
  virtual Int_t  Make();
  void Set(TObjArray * x){HList=x;} 
  void SetSector(int x) {setSector(x);}
  void SetMCflag(int x=1) {MCflag=x;}

  /// Displayed on session exit, leave it as-is please ...
  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StEEsmdCalMaker.h,v 1.1 2004/06/12 04:09:25 balewski Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  }

  ClassDef(StEEsmdCalMaker, 1)   //StAF chain virtual base class for Makers
};

#endif


// $Log: StEEsmdCalMaker.h,v $
// Revision 1.1  2004/06/12 04:09:25  balewski
// start
//
// Revision 1.1  2004/06/06 04:54:10  balewski
// dual analyzis
//
// Revision 1.2  2004/04/14 19:34:01  balewski
