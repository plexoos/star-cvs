//////////////////////////////////////////////////////////////////////
//
// $Id: StFlowConstants.h,v 1.6 2001/05/22 20:17:09 posk Exp $
//
// Author: Art Poskanzer and Raimond Snellings 
//          FTPC added by Markus Oldenburg, MPI, Dec 2000
//
//////////////////////////////////////////////////////////////////////
//
// Description: constants for the flow makers
//.
//////////////////////////////////////////////////////////////////////

#ifndef StFlowConstants_h
#define StFlowConstants_h
#include "Rtypes.h"

class Flow{

 public:

  enum {
    nHars        =   6, 
    nSels        =   2,
    nSubs        =   2,
    nPhiBins     = 120,
    nPhiBinsFtpc = 360
  };

  typedef Double_t PhiWgt_t[nSels][nHars][nPhiBins];
  typedef Double_t PhiWgtFtpc_t[nSels][nHars][nPhiBinsFtpc];

  ClassDef(Flow,1)               // macro for rootcint
};

#endif

//////////////////////////////////////////////////////////////////////
//
// $Log: StFlowConstants.h,v $
// Revision 1.6  2001/05/22 20:17:09  posk
// Now can do pseudorapidity subevents.
//
// Revision 1.5  2000/12/12 20:22:04  posk
// Put log comments at end of files.
// Deleted persistent StFlowEvent (old micro DST).
//
// Revision 1.4  2000/12/08 17:03:38  oldi
// Phi weights for both FTPCs included.
//
// Revision 1.1  2000/03/02 23:02:36  posk
// Changed extensions from .hh and .cc to .h and .cxx .
//
// Revision 1.1  1999/12/15 22:01:20  posk
// Added StFlowConstants.hh
//
//////////////////////////////////////////////////////////////////////
