/***************************************************************************
 *
 * $Id: StTofSimMaker.h,v 1.1 2001/09/28 19:11:11 llope Exp $
 *
 * Author:  Frank Geurts
 ***************************************************************************
 *
 * Description: StTofSimMaker virtual base class for TOFp Simulations
 *
 ***************************************************************************
 *
 * $Log: StTofSimMaker.h,v $
 * Revision 1.1  2001/09/28 19:11:11  llope
 * first version
 *
 **************************************************************************/
#ifndef STTOFSIMMAKER_HH
#define STTOFSIMMAKER_HH
#include "StMaker.h"


#include "TFile.h"
#include "TH1.h"
#include "StTofMCSlat.h"
#include "StTofMaker/StTofSlatCollection.h"

class StTofCalibration;
class StTofSimParam;
class StTofGeometry;
class StEvent;
struct g2t_ctf_hit_st;

class StTofSimMaker : public StMaker{
 private:
  StTofGeometry*    mGeomDb;//! 
  StTofCalibration* mCalibDb;//!
  StTofSimParam*    mSimDb;//!

  StEvent *mEvent;//!
  TH1F* mdE;//!
  TH1F* mdS;//!
  TH1F* mNumberOfPhotoelectrons;//!
  TH1F* mT;//!
  TH1F* mTime;//!
  TH1F* mTime1;//!
  TH1F* mPMlength;//!

 protected:
  StTofMCSlat detectorResponse(g2t_ctf_hit_st*);
  void fillRaw(void);
  void electronicNoise(void);
  void fillEvent();
  float slatResponseExp(float&);

 public: 
  StTofSimMaker(const char *name="TofSim");
  virtual ~StTofSimMaker();
  virtual Int_t Init();
  virtual Int_t  Make();
  virtual Int_t  Finish();

  virtual const char *GetCVS() const
    {static const char cvs[]="Tag $Name:  $ $Id: StTofSimMaker.h,v 1.1 2001/09/28 19:11:11 llope Exp $ built "__DATE__" "__TIME__ ; return cvs;}

  ClassDef(StTofSimMaker,1)
};
#endif
