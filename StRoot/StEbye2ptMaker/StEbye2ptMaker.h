/**************************************************************************
 *
 * $Id: StEbye2ptMaker.h,v 1.5 2000/12/16 18:41:42 aya Exp $
 *
 * Author: Jeff Reid, UW
 *         with design advice from Thomas Ullrich, Yale
 *
 ***************************************************************************
 *
 * Description:  This is a maker to generate 2-point correlation
 *                spaces (App) from p_t spectra
 *
 ***************************************************************************
 *
 * $Log: StEbye2ptMaker.h,v $
 * Revision 1.5  2000/12/16 18:41:42  aya
 * *** empty log message ***
 *
 * Revision 1.4  2000/09/20 00:53:50  jgreid
 * fixed sorting to work properly with event cuts
 *
 * Revision 1.2  2000/08/14 22:05:20  jseger
 * Added eta-spectra.  Now reads Ebye mini-DST as input.  Bins events in
 * multiplicity and z-vertex position.  Name of output file is no longer hard-wired.
 *
 * Revision 1.1.1.1  2000/02/05 03:15:21  jgreid
 * Two particle correlationspace generation package
 *
 *
 **************************************************************************/
#ifndef StEbye2ptMaker_HH
#define StEbye2ptMaker_HH
#include "StMaker.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"
#include "TTree.h"

class TFile;
class TH1F;
class TH2F;
class StEvent;
class StRun;
class StEbyeEvent;
class StEbyeTrack;

class StEbye2ptMaker : public StMaker {
public:

    StEbye2ptMaker(const Char_t *name="EbyE 2-pt");
    virtual ~StEbye2ptMaker();
    
    void Clear(Option_t *option="");
    Int_t Init();
    Int_t Make();
    Int_t Finish();
    void SetEbyeDSTFileName(const Char_t* name="ebyeevent.root");    
    void SetEbye2ptFileName(const Char_t* name="Ebye2pt.root");    
    void          EbyeDSTRead(Bool_t flag=kFALSE);
    virtual const char *GetCVS() const
    {static const char cvs[]="$Id: StEbye2ptMaker.h,v 1.5 2000/12/16 18:41:42 aya Exp $ built "__DATE__" "__TIME__ ; return cvs;}
    
private:

    // arrays for p_t values that pass track cuts
    Double_t *mThisEventPlus,*mThisEventMinus;
    Double_t *mPreviousEventPlus,*mPreviousEventMinus;
    Double_t *mEtaThisEventPlus,*mEtaThisEventMinus;
    Double_t *mEtaPreviousEventPlus,*mEtaPreviousEventMinus;

    // histograms for sibling and mixed pairs
    //   (+.+, +.-, -.+, -.-)
    TH1F *mMt,*mX,*mEta,*mPt,*mPtX;
    //    TH1F *mEtaX;
    TH2F *mSibPP,*mSibPM,*mSibMP,*mSibMM;
    TH2F *mMixPP,*mMixPM,*mMixMP,*mMixMM;
    TH2F *mSibPPEta,*mSibPMEta,*mSibMPEta,*mSibMMEta;
    TH2F *mMixPPEta,*mMixPMEta,*mMixMPEta,*mMixMMEta;
    TH2F *mZMult;

    Int_t mixEvents();
    Int_t processEvent(StEbyeEvent &event);
  UInt_t           mEventCounter;
  Bool_t           mEbyeDSTRead;
  Int_t            InitEbyeDSTRead();
  Char_t           mEbyeDSTFileName[74];    // mini-DST file name 
  Char_t           mEbye2ptFileName[74];    //2pt histogram file name 
  StEbyeEvent*     EbyeDSTBranch;
  StEbyeEvent*     mEbyeEvent;
  Int_t            mIndex[110000];
  Int_t            mSortArray[1100000];
  Int_t  	   mthisEventBinNumber;
  Int_t	   	   mpreviousEventBinNumber;
  TTree*           pEbyeTree;               // pointer to EbyeDST Tree 
  StEbyeTrack*     pEbyeTrack;
  TFile* 	   pEbyeDST;
  void             SortEvents();
  
    ClassDef(StEbye2ptMaker,1)
};

inline void StEbye2ptMaker::SetEbyeDSTFileName(const Char_t* name) {
  strncpy(mEbyeDSTFileName, name, 74); mEbyeDSTFileName[74] = '\0'; }

inline void StEbye2ptMaker::SetEbye2ptFileName(const Char_t* name) {
  strncpy(mEbye2ptFileName, name, 74); mEbye2ptFileName[74] = '\0'; }

inline void StEbye2ptMaker::EbyeDSTRead(Bool_t flag)
          { mEbyeDSTRead=flag;}

#endif
