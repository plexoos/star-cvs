#ifndef STAR_StTriggerDataMaker
#define STAR_StTriggerDataMaker
#include "StMaker.h"

class TH1F;
class StTriggerData;
class StDAQReader;

// class definition
class StTriggerDataMaker : public StMaker {
public: 
  StTriggerDataMaker(const char *name="trgd");
  virtual ~StTriggerDataMaker(){};
  virtual Int_t Make();
  
  virtual const char *GetCVS() const
  {static const char cvs[]="Tag "__DATE__" "__TIME__ ; return cvs;}
  
protected:
  
private:
  ClassDef(StTriggerDataMaker, 0)   //StAF chain virtual base class for Makers
};
#endif
