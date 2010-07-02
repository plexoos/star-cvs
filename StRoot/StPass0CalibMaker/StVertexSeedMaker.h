/*!
 * \class StVertexSeedMaker 
 * \author G. Van Buren, BNL
 * \version $Id: StVertexSeedMaker.h,v 1.14 2010/07/02 22:36:10 genevb Exp $
 *
 * calculates mean primary vertex positions from
 * suitable events to use as seeds in finding better       
 * primary vertex positions (helpful for low               
 * multiplicity events like pp collisions)                 
 */


#ifndef STAR_StVertexSeedMaker
#define STAR_StVertexSeedMaker

#ifndef StMaker_H
#include "StMaker.h"
#endif
#include "TString.h"
class TNtuple;
class St_vertexSeed;
class St_vertexSeedTriggers;


class StVertexSeedMaker : public StMaker {
 public: 
                  StVertexSeedMaker(const char *name="VtxSeedMkr",
                    const char* defaultDir="./StarDb/Calibrations/rhic/");
   virtual       ~StVertexSeedMaker();
   virtual Int_t Init();
   virtual Int_t Make();
   virtual void PrintInfo();
   virtual void Clear(Option_t *option);
   virtual Int_t Finish();
   virtual Int_t Aggregate(Char_t* dir=0, const Char_t* cuts="");

   virtual void FitData();
   virtual void FindResult(Bool_t checkDb=kTRUE);
   virtual int GetValidityDate();
   virtual int GetValidityTime();
   virtual void UseEventDateTime();
   virtual void UseFillDateTime();
   virtual void UseAllTriggers();
   virtual St_vertexSeed* VertexSeedTable();
   virtual void WriteTableToFile();     //Write drift velocity table (assumes correct trigger offset)
   virtual void SetMinEntries(int entries);  //minimum number of valid events for seed
   virtual void SetMaxX0Err(float err);  //maximum allowed error for x0 
   virtual void SetMaxY0Err(float err);  //maximum allowed error for y0 
   virtual void WriteHistFile();       // Write out vertexseedhist.root file w/results
   virtual void HistFileByDefault();   // Write out file on Finish
   virtual void SetVertexZmax(float zmax);  //Set max z vertex for seed calculation
   virtual void SetVertexZmin(float zmin);  //Set min z vertex for seed calculation
   virtual void SetVertexR2max(float r2max);  //Set max r^2 vertex for seed calculation
   virtual void SetDefDir(const char* dir) {defDir = dir;}
   virtual const char *GetCVS() const {
     static const char cvs[]="Tag $Name:  $ $Id: StVertexSeedMaker.h,v 1.14 2010/07/02 22:36:10 genevb Exp $ built "__DATE__" "__TIME__ ;
     return cvs;
   }

 protected:
   virtual void Reset();
   virtual Int_t FillAssumed();
   virtual Int_t GetVertexSeedTriggers();
   virtual void FillDateTime();
   virtual void GetFillDateTime();
   virtual Bool_t BetterErrors();
   virtual Bool_t ChangedValues();
   virtual Bool_t CheckTriggers() { return kTRUE; }
   virtual Bool_t ValidTrigger(unsigned int);
   virtual Int_t GetEventData() { return kStErr; }

  TH1F* xdist;
  TH1F* ydist;
  TH1F* xerr;
  TH1F* yerr;
  TNtuple* resNtuple;
  float xguess;
  float yguess;
  float zvertex;
  float yvertex;
  float xvertex;
  float eyvertex;
  float exvertex;
  float mult;
  float trig;
  float eventNumber;
  float HIST_MIN;
  float HIST_MAX;
  float zVertexMax; //maximum allowed z vertex for mean calculation
  float zVertexMin; //minimum allowed z vertex for mean calculation
  float r2VertexMax; //minimum allowed radius^2 vertex for mean calculation
  int    fill;
  int    date;
  int    time;
  int    run;
  float  zdc; // ZDC sum rate
  // The following integer maps can only be stored to 24 bits
  // because of the conversion to float
  int    itpc; // inner tpc track map
  int    otpc; // inner tpc track map
  int    detmap; // map any other detectors
  float  rank;
  int    minEntries;
  float    maxX0Err;
  float    maxY0Err;
  Bool_t   mHistOut;
  TFile*   mTempOut;
  Bool_t   useEventDateTime;
  Bool_t   useAllTriggers;
  double p[4];  // calculated params
  double ep[4]; // calculated errs
  double a[4];  // database params
  double ea[4]; // database errs
  double chi;
  TString defDir;
  St_vertexSeedTriggers* dbTriggersTable;

  ClassDef(StVertexSeedMaker,0)
};


inline void StVertexSeedMaker::UseEventDateTime() {useEventDateTime = kTRUE;}
inline void StVertexSeedMaker::UseFillDateTime() {useEventDateTime = kFALSE;}
inline void StVertexSeedMaker::UseAllTriggers() {useAllTriggers = kTRUE;}
inline void StVertexSeedMaker::SetMinEntries(int entries){minEntries = entries; }
inline void StVertexSeedMaker::SetMaxX0Err(float err){maxX0Err = err;}
inline void StVertexSeedMaker::SetMaxY0Err(float err){maxY0Err = err;}
inline int  StVertexSeedMaker::GetValidityDate(){return date;}
inline int  StVertexSeedMaker::GetValidityTime(){return time;}
inline void StVertexSeedMaker::HistFileByDefault(){mHistOut = kTRUE;} 
inline void StVertexSeedMaker::SetVertexZmax(float zmax){zVertexMax = zmax;}
inline void StVertexSeedMaker::SetVertexZmin(float zmin){zVertexMin = zmin;}
inline void StVertexSeedMaker::SetVertexR2max(float r2max){r2VertexMax = r2max;}

#endif

// $Id: StVertexSeedMaker.h,v 1.14 2010/07/02 22:36:10 genevb Exp $
// $Log: StVertexSeedMaker.h,v $
// Revision 1.14  2010/07/02 22:36:10  genevb
// Option for using all triggers
//
// Revision 1.13  2009/11/23 21:38:56  genevb
// Fix problems with memory-resident TNtuple by using a temporary disk file
//
// Revision 1.12  2009/11/16 22:31:11  genevb
// phase out usage of old tables
//
// Revision 1.11  2009/05/22 23:50:50  genevb
// Code mods for BEMC matches, BeamWidth
//
// Revision 1.10  2008/05/21 17:48:39  genevb
// Use vertex errors for weighting
//
// Revision 1.9  2008/04/29 23:30:34  genevb
// Added cuts capability to Aggregate
//
// Revision 1.8  2007/11/27 23:42:48  genevb
// Move valid triggers from code to DB
//
// Revision 1.7  2006/09/01 22:27:16  genevb
// More detailed info in ntuple
//
// Revision 1.6  2005/07/01 21:46:01  genevb
// Specify output directory
//
// Revision 1.5  2005/06/14 18:51:31  genevb
// Updates to allow for pp2005 triggers, and inheritance
//
// Revision 1.4  2002/03/20 00:40:43  genevb
// Addition of Aggregate feature, minor updates
//
//
//
