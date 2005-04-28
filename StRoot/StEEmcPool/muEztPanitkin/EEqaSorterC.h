// \class  EEqaSorterC
// \author Jan Balewski, Hal Spinka
// $Id: EEqaSorterC.h,v 1.1 2005/04/28 20:54:46 balewski Exp $

#ifndef EEqaSorterC_h
#define EEqaSorterC_h

#include "TObject.h"
#include "StEEmcUtil/EEfeeRaw/EEdims.h"

class TObjArray;

class TH1F;
class TH2F;
class EztEmcRawData;
class StEEmcDbMaker;
class EEmcDb;

class EEqaSorterC :public TObject{ 

 private:
  enum {mxh=4};
  TH2F *h2D[mxh];
  TH1F * hMult[mxh];
  TH2F *hMAPMT;
  TH1F *hSmd[MaxSectors][MaxSmdPlains];   // SMD hits by plane
  TH1F *hnHSmd[MaxSectors][MaxSmdPlains];  // frequency distr. of smd hits

  int adcThrTw,adcThrPrs,adcThrPost,adcThrSmd; // tresholds for frequency plots

  TObjArray *HList;
  EztEmcRawData  *eETow;
  EztEmcRawData  *eESmd;
#ifdef IN_PANITKIN
  EEmcDb*eeDb;
#else
  StEEmcDbMaker  *eeDb;
#endif

 public:
  EEqaSorterC( TObjArray*L,StEEmcDbMaker*dbx);
  void initHisto();
  void initRun();
  void sort(EztEmcRawData  *t,  EztEmcRawData  *s, int ver );

  void sortTower();
  void sortMapmt(int ver);

#ifndef IN_PANITKIN
   ClassDef(EEqaSorterC,1) 
#endif
};
     
#endif

// $Log: EEqaSorterC.h,v $
// Revision 1.1  2005/04/28 20:54:46  balewski
// start
//
// Revision 1.5  2004/02/17 03:09:18  balewski
// *** empty log message ***
//
// Revision 1.4  2004/01/29 17:23:14  balewski
// fix for BTOW
//
// Revision 1.3  2004/01/27 16:29:39  balewski
// reset added
//
// Revision 1.7  2004/01/21 18:53:33  spinka
// Various problems fixed for towers.  More remain.
//
