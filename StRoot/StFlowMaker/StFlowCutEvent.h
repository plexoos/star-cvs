////////////////////////////////////////////////////////////////////////////
//
// $Id: StFlowCutEvent.h,v 1.5 2000/08/31 18:58:19 posk Exp $
//
// Author: Art Poskanzer and Raimond Snellings, LBNL, Oct 1999
//
// Description:  Class for applying flow event cuts
//               If lo >= hi no cut is applied
//               All functions and data members are static
//               Therefore, no need to instantiate
//               Just use StFlowCutEvent::func();
//
////////////////////////////////////////////////////////////////////////////
//
// $Log: StFlowCutEvent.h,v $
// Revision 1.5  2000/08/31 18:58:19  posk
// For picoDST, added version number, runID, and multEta for centrality.
// Added centrality cut when reading picoDST.
// Added pt and eta selections for particles corr. wrt event plane.
//
// Revision 1.4  2000/07/12 17:54:34  posk
// Added chi2 and dca cuts. Multiplied EtaSym by sqrt(mult).
// Apply cuts when reading picoevent file.
//
// Revision 1.3  2000/06/30 14:48:30  posk
// Using MessageMgr, changed Eta Symmetry cut.
//
// Revision 1.2  2000/05/26 21:29:26  posk
// Protected Track data members from overflow.
//
// Revision 1.1  2000/03/02 23:02:40  posk
// Changed extensions from .hh and .cc to .h and .cxx .
//
// Revision 1.4  1999/12/15 22:01:23  posk
// Added StFlowConstants.hh
//
// Revision 1.3  1999/11/30 18:52:48  snelling
// First modification for the new StEvent
//
// Revision 1.2  1999/11/24 18:17:10  posk
// Put the methods which act on the data in with the data in StFlowEvent.
//
// Revision 1.1  1999/11/11 23:08:49  posk
// Rearrangement of files.
//
// Revision 1.1  1999/11/05 00:06:42  posk
// First versions of Flow cut classes.
//
//
////////////////////////////////////////////////////////////////////////////

#ifndef _StFlowCutEvent_INCLUDED_
#define _StFlowCutEvent_INCLUDED_
#include <iostream.h>
#include <stdlib.h>
#include "Rtypes.h"
class StEvent;
class StFlowPicoEvent;

class StFlowCutEvent {

 public:

               StFlowCutEvent();
  virtual      ~StFlowCutEvent();

  static Bool_t CheckEvent(StEvent* pEvent);
  static Bool_t CheckEvent(StFlowPicoEvent* pPicoEvent);
  static Bool_t CheckEtaSymmetry(StEvent* pEvent);
  static Bool_t CheckEtaSymmetry(StFlowPicoEvent* pPicoEvent);
  static void   PrintCutList();
  static void   SetCent(const Int_t lo, const Int_t hi);
  static void   SetMult(const Int_t lo, const Int_t hi);
  static void   SetVertexX(const Float_t lo, const Float_t hi);
  static void   SetVertexY(const Float_t lo, const Float_t hi);
  static void   SetVertexZ(const Float_t lo, const Float_t hi);
  static void   SetEtaSym(Float_t lo, Float_t hi);
  
 private:

  static UInt_t  mEventN;                // number of events
  static UInt_t  mGoodEventN;            // number of accepted events   
						
  static UInt_t  mCentCut;               // number of not accepted events
  static Int_t   mCentCuts[2];           // range of multiplicity

  static UInt_t  mMultCut;               // number of not accepted events
  static Int_t   mMultCuts[2];           // range of multiplicity

  static UInt_t  mVertexXCut;            // number of not accepted events
  static Float_t mVertexXCuts[2];        // range of X vertex position

  static UInt_t  mVertexYCut;            // number of not accepted events
  static Float_t mVertexYCuts[2];        // range of Y vertex position

  static UInt_t  mVertexZCut;            // number of not accepted events
  static Float_t mVertexZCuts[2];        // range of Z vertex position

  static UInt_t  mEtaSymCutN;            // number not accepted
  static Float_t mEtaSymCuts[2];         // range

  ClassDef(StFlowCutEvent,1)             // macro for rootcint
}; 

inline void StFlowCutEvent::SetCent(const Int_t lo, const Int_t hi) {
  mCentCuts[0] = lo; mCentCuts[1] = hi; }

inline void StFlowCutEvent::SetMult(const Int_t lo, const Int_t hi) {
  mMultCuts[0] = lo; mMultCuts[1] = hi; }

inline void StFlowCutEvent::SetVertexX(const Float_t lo, const Float_t hi) {
  mVertexXCuts[0] = lo; mVertexXCuts[1] = hi; }

inline void StFlowCutEvent::SetVertexY(const Float_t lo, const Float_t hi) {
  mVertexYCuts[0] = lo; mVertexYCuts[1] = hi; }

inline void StFlowCutEvent::SetVertexZ(const Float_t lo, const Float_t hi) {
  mVertexZCuts[0] = lo; mVertexZCuts[1] = hi; }

inline void StFlowCutEvent::SetEtaSym(Float_t lo, Float_t hi) {
  mEtaSymCuts[0] = lo; mEtaSymCuts[1] = hi; }

#endif
