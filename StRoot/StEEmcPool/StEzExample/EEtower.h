// \class  EEtower
// \author Jan Balewski
#ifndef EEtower_h
#define EEtower_h
/*********************************************************************
 * $Id: EEtower.h,v 1.1 2004/06/06 04:54:10 balewski Exp $
 *********************************************************************
 * Descripion:
 *  finds pi0 based on EEMC tower response
 *********************************************************************/

class EEmcGeomSimple;
class TVector3;
class TH1F ;
class EEmcDbItem;

/// the ultimate source of dimensions is in this header
#include "StEEmcUtil/EEfeeRaw/EEdims.h"


/// the trick to switch between two DB readers
#ifdef StRootFREE
  class EEmcDb;
  typedef EEmcDb EEDB;
#else
  class StEEmcDbMaker;
  typedef StEEmcDbMaker EEDB;
#endif


class EEtower {

 private:

  EEmcGeomSimple *geom;
  float th1,th2;/// some thresholds

  TH1F *hA[32]; // some histograms
  
 protected:
  // only this variables cna be altered by extrenal classes
  int nInpEve; /// no. of input events

  /// local event storage
  float towerE[MaxEtaBins][MaxPhiBins]; 

  void clear();
  void task1();
  EEDB *eeDb; /// DB access point
  TObjArray  *HList; /// output histo access point
  
 public:
  
  EEtower();
  virtual ~EEtower();
  void print();
  void finish();

  void init(); 
  void setThres(float a, float b ){ th1=a; th2=b;}
  void saveHisto(TString fname="fixMe3");
 
  ClassDef(EEtower,1) 
};
#endif

/* fix in St-code
1) clear soloMipDb[MxTw] in InitRun

*/
 


/*****************************************************************
 * $Log: EEtower.h,v $
 * Revision 1.1  2004/06/06 04:54:10  balewski
 * dual analyzis
 *
 *
 ********************************************************************/

