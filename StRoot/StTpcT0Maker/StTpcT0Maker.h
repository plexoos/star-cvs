// $Id: StTpcT0Maker.h,v 1.1 2000/08/24 23:51:27 hardtke Exp $
// $Log: StTpcT0Maker.h,v $
// Revision 1.1  2000/08/24 23:51:27  hardtke
// New package for drift velocity calibrations
//
#ifndef STAR_StTpcT0Maker
#define STAR_StTpcT0Maker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StTpcT0Maker virtual base class for Maker                            //
//                                                                      //
//  Submit any problem with this code via begin_html <A HREF="http://www.rhic.bnl.gov/STAR/html/comp_l/sofi/bugs/send-pr.html"><B><I>"STAR Problem Report Form"</I></B></A> end_html
//
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif
class StTpcDb;
class St_tpcDriftVelocity;


class StTpcT0Maker : public StMaker {
 public: 
                  StTpcT0Maker(const char *name="TLA");
   virtual       ~StTpcT0Maker();
   virtual Int_t Init();
   virtual Int_t  Make();
   virtual void PrintInfo();
   virtual void Clear(Option_t *option);
   virtual Int_t Finish();
   float   AverageT0();
   int GetValidityDate();
   int GetValidityTime();
   St_tpcDriftVelocity* driftTable();
   void WriteTableToFile();     //Write drift velocity table (assumes correct trigger offset)
   void SetMinEntries(int entries);  //minimum number of valid events for t0
   void SetDesiredEntries(int entries);  //desired number of valid events for t0
   void SetMaxRMS(float RMS);  //maximum allowed RMS for t0 histogram 
   void WriteHistFile();       // Write out t0hist.root file with results
   void HistFileByDefault();   // Write out file on Finish

   virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StTpcT0Maker.h,v 1.1 2000/08/24 23:51:27 hardtke Exp $ built "__DATE__" "__TIME__ ; return cvs;}

 protected:

 private:
  TH1F* t0result;
  float t0guess;
  float t0current;
  float zVertexWest;
  float zVertexEast;
  float T0HIST_MIN;
  float T0HIST_MAX;
  StTpcDb* theDb; //!
  int    date;
  int    time;
  int    minEntries;
  int    desiredEntries;
  float    maxRMS;
  Bool_t   mHistOut;

   ClassDef(StTpcT0Maker, 1)   //StAF chain virtual base class for Makers
};

#endif
