#ifndef St_starClockOnlC_h
#define St_starClockOnlC_h

#include "TChair.h"
#include "tables/St_starClockOnl_Table.h"

class St_starClockOnlC : public TChair {
 public:
  static St_starClockOnlC*   instance();
  UInt_t     getNumRows()                     {return GetNRows();}
  starClockOnl_st *Struct(Int_t i = -1);
  UInt_t    RunNumber(Int_t i = -1)           {return Struct(i)->runNumber;}
  Double_t  CurrentFrequency(Int_t i = -1)    {return Struct(i)->frequency;} 
  UInt_t    Time(Int_t i = -1)                {return Struct(i)->time;}   
  Double_t  Frequency(Int_t i = -1)           {return CurrentFrequency(i);}
  // depreciated
  UInt_t    getRunNumber(Int_t i = -1)        {return RunNumber(i);}       
  Double_t  getCurrentFrequency(Int_t i = -1) {return CurrentFrequency(i);} 
  UInt_t    getTime(Int_t i = -1)             {return Time(i);}            
  Double_t  getFrequency(Int_t i = -1)        {return Frequency(i);}       
 protected:
  St_starClockOnlC(St_starClockOnl *table=0) : TChair(table) {}
  virtual ~St_starClockOnlC() {SafeDelete(fgInstance);}
 private:
  static St_starClockOnlC* fgInstance;
  ClassDefChair(St_starClockOnl, starClockOnl_st )
  ClassDef(St_starClockOnlC,1) //C++ TChair for starClockOnl table class
};
#endif
