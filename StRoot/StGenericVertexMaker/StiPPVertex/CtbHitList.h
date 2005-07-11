#ifndef CtbHitList_h
#define CtbHitList_h

#include "ScintHitList.h"
class StTriggerData;

class CtbHitList : public ScintHitList {
 private:

  // params:
  float mCtbThres_mev; // M-C hits
  int mCtbThres_ch;// data hits 
  float *geantE;
  enum {mxPhi1=61,mxEta1=5};
  int mcId2bin[mxPhi1][mxEta1]; // map for M-C
  enum {mxSlat=2, mxTray=120};
  int realId2bin[mxSlat][mxTray]; // map for real events
  
 public:
  CtbHitList();
  ~CtbHitList();
  void clear();
  void initRun(); 
  void buildFromMC(St_DataSet *gds);
  void buildFromData(StTriggerData *trgD);
  virtual int etaBin(float eta);
  virtual float bin2EtaLeft(int iEta);
 static void ctb_get_slat_from_data(int slat, int tray, float & phiRad, float &eta);
};

#endif
