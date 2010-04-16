// $Id: St2009WjjMaker.h,v 1.1 2010/04/16 01:04:43 balewski Exp $
//
//*-- Author : Jan Balewski, MIT


#ifndef STAR_St2009WjjMaker
#define STAR_St2009WjjMaker

/*!
 *                                                                     
 * \class  St2009WjjMaker

 * \author Jan Balewski, MIT
 * \date   August 2009
 * \brief  spin sorting of W's 
 *
 *
 *
 */                                                                      

#ifndef StMaker_H
#include "StMaker.h"
#endif
#include <TString.h>

class St2009WMaker;
class StSpinDbMaker;

class St2009WjjMaker : public StMaker {
 private:
  int nRun;
  int Tfirst,Tlast;

  float par_jetPtLow,par_jetPtHigh, par_djPtHigh ; 
  float par_jetEtaLow,par_jetEtaHigh ; 
  bool  par_spinSort;
  float par_vertexZ;
  int   isMC;

  St2009WMaker *wMK; // W-algo maker with all data
  StSpinDbMaker *spinDb;
  TString core; // name attached to all histos

  // histograms
  TObjArray *HList;
  enum {mxHA=32}; TH1 * hA[mxHA];
  TH1F *hbxIdeal;
  
  void initHistos();
  void bXingSort();  
 public: 
  St2009WjjMaker(const char *name="2009Wjetjet");
  virtual       ~St2009WjjMaker(){};
  virtual Int_t  Init();
  virtual Int_t  InitRun  (int runumber);
  virtual Int_t  Make();
  void setHList(TObjArray * x){HList=x;}
  void setSpinSort(bool x){ par_spinSort=x;}
  void setMC(int x){ isMC=x;}

  void attachWalgoMaker(St2009WMaker *mk) { wMK=mk;}
  void attachSpinDb(StSpinDbMaker *mk){ spinDb=mk;}
  virtual Int_t FinishRun(int runumber);


  /// Displayed on session exit, leave it as-is please ...
  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: St2009WjjMaker.h,v 1.1 2010/04/16 01:04:43 balewski Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  }

  ClassDef(St2009WjjMaker,0)   //StAF chain virtual base class for Makers
};

#endif


// $Log: St2009WjjMaker.h,v $
// Revision 1.1  2010/04/16 01:04:43  balewski
// start
//
