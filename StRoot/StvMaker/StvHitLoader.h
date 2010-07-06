//StvHitLoader.h

#ifndef StvHitLoader_HH
#define StvHitLoader_HH

#include <string>

#include "TNamed.h"
#include "StEvent/StEnumerations.h"


class StEvent;
class StHit;
class StvHit;
class StEventHitIter;
class StvHitLoader : public TNamed 
{
 public:
    
    StvHitLoader(const char* name = "StHitsLoader");
    virtual ~StvHitLoader();
    void  Clear(const char* opt="");
    int   Init();
    int   LoadHits(const StEvent *stev); 
    int   Finish();
 protected:
    StvHit *MakeStvHit(const StHit *stHit,UInt_t upath); 
    
 private:
 StEventHitIter *mHitIter;
 ClassDef(StvHitLoader,0)
};

#endif
