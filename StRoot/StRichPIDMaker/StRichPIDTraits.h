/***************************************************************************
 *
 * $Id: StRichPIDTraits.h,v 2.0 2000/08/09 16:26:19 gans Exp $
 *
 * Author: Matt Horsley, March 29, 2000
 ***************************************************************************
 *
 * Description: patterened after StDedxPidTraits.h
 **************************************************************************/
#ifndef StRichPIDTraits_hh
#define StRichPIDTraits_hh

#include "StEvent/StTrackPidTraits.h"
#include "StParticleDefinition.hh"
#include "TArrayD.h"


class StParticleDefinition;

class StRichPIDTraits : public StTrackPidTraits {

public:
  StRichPIDTraits();
  StRichPIDTraits(StDetectorId, StParticleDefinition*);
  // StRichPIDTraits& operator=(const StRichPIDTraits&); use default

  virtual ~StRichPIDTraits();
  StParticleDefinition*  particle() const;
  void                   addAreaArray(TArrayD&);
  void                   addHitArray(TArrayD&);
  TArrayD                 getDensityArray(); 
  TArrayD&                getHitArray();
  TArrayD&                getAreaArray();
  void                   addNewRingArea(double);
  double                getNewRingArea();
  void                  addRHits(int);
  int                   getRHits();
  void                  setCut(double);
  double                getCut();
  

protected:
  StParticleDefinition*  mParticle;   // !
  TArrayD                mAreaArray; //!
  TArrayD                mHitArray; //!
  
  int mRichHits;
  double mCut;
  double mNewRingArea;

  StObject* clone();
  ClassDef(StRichPIDTraits,1)
};
#endif
