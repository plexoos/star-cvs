// $Id: StChain.h,v 1.36 2000/11/27 22:46:39 fisyak Exp $
// $Log: StChain.h,v $
// Revision 1.36  2000/11/27 22:46:39  fisyak
// Introduce kIsCalibrated BIT
//
// Revision 1.35  2000/11/27 13:31:23  fisyak
// Add Production time set
//
// Revision 1.34  2000/09/27 19:34:59  fisyak
// Temporal fix to get Run/EventNo from dst
//
// Revision 1.33  2000/07/26 20:58:22  fine
// StChain::GetOption virtual dummy methods have been introduced
//
// Revision 1.32  2000/03/23 00:15:22  fine
// Adjusted to libSTAR for ROOT 2.24
//
// Revision 1.31  1999/12/03 01:24:40  fine
// Advanced timer has been introduced
//
// Revision 1.30  1999/07/15 13:56:46  perev
// cleanup
//
// Revision 1.29  1999/07/14 15:26:18  fine
// Context menu MakeEvent method has been introduced
//
// Revision 1.28  1999/07/13 02:19:33  perev
// GetCVS,StEvtHddr,etc...
//
// Revision 1.27  1999/07/11 20:40:35  perev
// Move Clear from StChain to StMaker
//
// Revision 1.26  1999/03/19 20:30:49  perev
// GetCVSTag introduced
//
// Revision 1.25  1999/03/11 01:23:58  perev
// new schema StChain
//
// Revision 1.18  1998/12/21 19:42:50  fisyak
// Move ROOT includes to non system
//
// Revision 1.17  1998/11/25 21:58:21  fisyak
// Cleanup
//
// Revision 1.16  1998/11/22 18:28:06  fisyak
// Add name of tag
//
// Revision 1.15  1998/11/19 01:23:56  fine
// StChain::MakeDoc has been introduced, StChain::MakeDoc has been fixed (see macros/bfc_doc.C macro
//
// Revision 1.14  1998/10/31 00:21:31  fisyak
// Makers take care about branches
//
// Revision 1.13  1998/10/07 18:43:59  perev
// Add Spy classes for Farm Monitor
//
// Revision 1.12  1998/10/06 18:00:27  perev
// cleanup
//
// Revision 1.11  1998/09/18 14:35:29  fisyak
// Fix makers
//
// Revision 1.10  1998/09/08 22:43:09  fisyak
// Modify St_dst_Maker to account new calling sequence
//
// Revision 1.9  1998/09/08 13:42:00  love
// new St_tpctest_Maker module
//
// Revision 1.8  1998/08/18 14:05:02  fisyak
// Add to bfc dst
//
// Revision 1.7  1998/08/07 19:34:53  fisyak
// Add St_run_Maker
//
// Revision 1.6  1998/07/20 15:08:08  fisyak
// Add tcl and tpt
//

#ifndef STAR_StChain
#define STAR_StChain

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StChain                                                              //
//                                                                      //
// Main base class to control chains for the different STAR "chains"    //
//                                                                      //
// This class :                                                         //
//   - Initialises the run default parameters                           //
//   - Provides API to Set/Get run parameters                           //
//   - Creates the support lists (TClonesArrays) for the Event structure//
//   - Creates the physics objects makers                               //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <stdlib.h>
#include <stdio.h>

#ifndef StMaker_H
#include "StMaker.h"
#endif
class StEvtHddr;
enum EChainBits {
  kIsCalibrated = BIT(24)   // if the TObject has been created after calibration
};
class StChain : public StMaker {
private:
   Int_t               m_Version;    	//StChain version number
   Int_t               m_VersionDate;   //StChain version date
 protected:
   StEvtHddr         *m_EvtHddr;     	//Header of event
public:
                      StChain(const char *name="bfcChain");
   virtual           ~StChain();
   virtual void       Clear(Option_t *option="");
   virtual Int_t      Finish();   // *MENU*
   virtual Int_t      Init();
   virtual Int_t      Make();
   virtual Int_t      IsChain() const {return 1;}
   virtual Int_t      MakeEvent(); // *MENU*
   virtual Bool_t      GetOption(const TString *Opt) {return Opt ? GetOption(Opt->Data()):kFALSE;}
   virtual Bool_t      GetOption(const Char_t *Opt)  {return kFALSE;}
   Int_t              GetVersion() {return m_Version;}
   Int_t              GetVersionDate() {return m_VersionDate;}

 virtual const char *GetCVS() const 
 {static const char cvs[]="Tag $Name:  $ $Id: StChain.h,v 1.36 2000/11/27 22:46:39 fisyak Exp $ built "__DATE__" "__TIME__ ; return cvs;}
   ClassDef(StChain, 0)   //StChain control class
};

#endif
