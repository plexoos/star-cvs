// $Id: St_geom_Maker.h,v 1.3 1998/12/16 20:56:24 fisyak Exp $
// $Log: St_geom_Maker.h,v $
// Revision 1.3  1998/12/16 20:56:24  fisyak
// Add gstar to ROOT
//
// Revision 1.2  1998/12/04 19:36:48  fisyak
// Add Pavel/Ruben gstar interface
//
// Revision 1.1  1998/10/31 00:28:31  fisyak
// Makers take care about branches
//
// Revision 1.6  1998/10/06 18:00:31  perev
// cleanup
//
// Revision 1.5  1998/08/26 12:15:13  fisyak
// Remove asu & dsl libraries
//
// Revision 1.4  1998/08/14 15:25:58  fisyak
// add options
//
// Revision 1.3  1998/08/10 02:32:07  fisyak
// Clean up
//
// Revision 1.2  1998/07/20 15:08:15  fisyak
// Add tcl and tpt
//
#ifndef STAR_St_geom_Maker
#define STAR_St_geom_Maker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_geom_Maker virtual base class for Maker                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
class TNode;

#endif
//class St_stk_stkpar;
class St_geom_Maker : public StMaker {
 private:
   Bool_t drawinit;
// static Char_t  m_VersionCVS = "$Id: St_geom_Maker.h,v 1.3 1998/12/16 20:56:24 fisyak Exp $";
// Int_t          m_mode;        // mode 1 = primaries;
// St_stk_stkpar *m_stk_stkpar;  //! pointer to stk parameters
 TNode*   fNode;
 
 protected:
 public: 
                  St_geom_Maker(const char *name="geom", const char *title="run/geant/Run");
   virtual       ~St_geom_Maker();
   virtual Int_t  Finish(){SafeDelete(m_DataSet); return kStOK;}
   virtual Int_t  Init();
   virtual Int_t  Make();
   virtual void   PrintInfo();
   virtual void   Clear(Option_t *option){}; // No clearance for parameters
   void Draw();  
   void Do  (const char*);  
   void Work();  
   TNode* GetNode() { return fNode; }
// virtual void Set_mode       (Int_t   m =      2){m_mode       = m;} // *MENU*
   ClassDef(St_geom_Maker, 1)   //StAF chain virtual base class for Makers
};

#endif

