//! $Id: StHistMaker.h,v 2.0 2000/08/25 16:02:40 genevb Exp $
//! $Log: StHistMaker.h,v $
//! Revision 2.0  2000/08/25 16:02:40  genevb
//! New revision: new structure, multiplicity classes
//!
//! Revision 1.2  2000/06/29 04:46:01  lansdell
//! removed virtual from inline methods
//!
//! Revision 1.1  2000/06/23 21:16:18  kathy
//! code that will collect all histograms that were added together in StHistUtil::AddHists - has to be in a maker in order to write it out
//!
//                                                                       //
///////////////////////////////////////////////////////////////////////////

#ifndef STAR_StHistMaker
#define STAR_StHistMaker

#include "StHistUtil.h"
#include "StMaker.h"
#include "TH1.h"

//////////////////////////////////////////////////////////////////////////

class StHistMaker : public StMaker {

 private:

  TH1** mHArray; //!


 public: 

//! static Char_t m_VersionCVS = "$Id: StHistMaker.h,v 2.0 2000/08/25 16:02:40 genevb Exp $";

  StHistMaker(const char *name="QA", const char *title="SummedQAHist");
  virtual       ~StHistMaker();
  virtual Int_t  Init();
  virtual Int_t  Finish();
  virtual Int_t  Make();

  void SetHArray(TH1**);

// the following is a ROOT macro  that is needed in all ROOT code
  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StHistMaker.h,v 2.0 2000/08/25 16:02:40 genevb Exp $ built "__DATE__" "__TIME__ ; return cvs;}

  ClassDef(StHistMaker, 1)   //StAF chain virtual base class for Makers
    };
    
inline void StHistMaker::SetHArray(TH1** val)
                           {mHArray = val; }


#endif




