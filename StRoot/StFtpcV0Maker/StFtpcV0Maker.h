
#ifndef STAR_St_fv0_Maker
#define STAR_St_fv0_Maker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_fv0_Maker                                                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif
//class St_stk_stkpar;
class St_fv0_Maker : public StMaker {
 private:
   Bool_t drawinit;
// static Char_t  m_VersionCVS = "$Id: StFtpcV0Maker.h,v 1.1 1999/04/12 22:24:54 mheffner Exp $";
// Int_t          m_mode;        // mode 1 = primaries;
// St_stk_stkpar *m_stk_stkpar;  //! pointer to stk parameters
 
 protected:
 TH1F  *m_b;
 TH1F  *m_dca;
 TH1F  *m_kaonMass;
 TH1F  *m_lambdaMass;
 TH1F  *m_antiLambdaMass;
 
 public: 
                  St_fv0_Maker(const char *name="fv0", const char *title="fv0_something");
   virtual       ~St_fv0_Maker();
   virtual Int_t Init();
   virtual Int_t  Make();
   virtual void   PrintInfo();
// virtual void Set_mode       (Int_t   m =      2){m_mode       = m;} // *MENU*
   ClassDef(St_fv0_Maker, 1)   //StAF chain virtual base class for Makers
};

#endif
