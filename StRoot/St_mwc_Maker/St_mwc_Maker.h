// $Id: St_mwc_Maker.h,v 1.10 2002/04/08 22:35:27 vlmrz Exp $
// $Log: St_mwc_Maker.h,v $
// Revision 1.10  2002/04/08 22:35:27  vlmrz
// *** empty log message ***
//
// Revision 1.9  2000/12/12 04:51:09  vlmrz
// *** empty log message ***
//
// Revision 1.8  2000/12/11 10:33:35  vlmrz
// *** empty   log message ***
//
// Revision 1.7  1999/07/15 13:58:17  perev
// cleanup
//
// Revision 1.6  1999/06/09 19:56:31  druss
// Added histograms : # of wire hits, # of sectors hit, # wires hit as a function of eta and phi
//
// Revision 1.5  1999/03/12 21:47:36  perev
// New maker schema
//
// Revision 1.4  1999/02/24 13:57:43  druss
// removed 2d histograms and replaced them with 1d histograms
//
// Revision 1.3  1999/02/19 18:22:58  druss
// init routine now uses parameter files from StRoot/params
// included a few histograms
//
// Revision 1.2  1999/02/08 16:51:58  fisyak
// Fix to parameters
//
// Revision 1.1  1999/01/14 19:11:01  druss
// root Maker definitions/header for mwc
//
// Revision 1.7  1998/10/31 00:25:45  fisyak
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
#ifndef STAR_St_mwc_Maker
#define STAR_St_mwc_Maker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_mwc_Maker virtual base class for Maker                            //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#include <fstream.h>
//#include "TFile.h"
#include "TNtuple.h"
#endif

class St_mwc_geo;
class St_mwc_cal;
class St_mwc_mpar;
class St_mwc_pars;
class TH1F;
class TH2F;

class St_mwc_Maker : public StMaker {
 private:
   Bool_t drawinit;
// static Char_t  m_VersionCVS = "$Id: St_mwc_Maker.h,v 1.10 2002/04/08 22:35:27 vlmrz Exp $";
// Int_t          m_mode;        // mode 1 = primaries;
// St_stk_stkpar *m_stk_stkpar;  //! pointer to stk parameters
   St_mwc_geo  *m_geom; //!
   St_mwc_cal  *m_cal;  //!
   St_mwc_mpar *m_mpar; //!
   St_mwc_pars *m_pars; //!
   Float_t r1max;
   Float_t r1min;
   Float_t r2max;
   Float_t r2min;
   Float_t outsect_ir;
   Float_t insect_ir;
   Float_t wire_step;
   Float_t num_wires_sect;
   Float_t phi_step;
   Int_t empty_counter;
   Int_t skip_counter;
   Int_t double_counter;

 protected:
   TH1F   *m_px;  //!
   TH1F   *m_py;  //!
   TH1F   *m_pz;  //!
   TH1F   *m_Hits; //!
   TH1F   *m_nWiresHit; //!
   TH1F   *m_nSectorsHit; //!
   TH2F   *m_EtaPhi; //!
   TH1F   *m_HitCount; //!
 
  
 public: 
                  St_mwc_Maker(const char *name="mwc");
   virtual       ~St_mwc_Maker();
   virtual Int_t  Init();
   virtual Int_t  Make();
   virtual Int_t  Finish();
// virtual void Set_mode       (Int_t   m =      2){m_mode       = m;} // *MENU*
  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: St_mwc_Maker.h,v 1.10 2002/04/08 22:35:27 vlmrz Exp $ built "__DATE__" "__TIME__ ; return cvs;}

   ClassDef(St_mwc_Maker, 1)   //StAF chain virtual base class for Makers
};

#endif
