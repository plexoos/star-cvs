//////////////////////////////////////////////////////////////////////
//
// $Id: StPeCEvent.h,v 1.7 2003/02/05 17:14:05 yepes Exp $
// $Log: StPeCEvent.h,v $
// Revision 1.7  2003/02/05 17:14:05  yepes
// Adding bField and pPairs.psi to tree
//
// Revision 1.6  2002/12/19 18:09:53  yepes
// MuDST input added
//
// Revision 1.5  2002/03/19 22:23:28  meissner
// New variables: zdc unatt., Trigger word, MC tree if Geant Branch, DCA  for primary pairs, all tracks for secondary pairs (Test)
//
// Revision 1.4  2001/04/27 19:32:07  akio
// fixing a #define conflict bug
//
// Revision 1.3  2001/02/12 21:15:44  yepes
// New version of StPeCMaker, lots of changes
//
// Revision 1.2  2000/04/21 19:10:27  nystrand
// Include StPeCPair class
//
// Revision 1.1  2000/03/24 22:36:56  nystrand
// First version of StPeCEvent
//
// Revision 1.0  2000/01/20 23:28:51  nystrand
// First Version of StPeCEvent 
//
//////////////////////////////////////////////////////////////////////
//
// StPeCEvent
//
// Event class for Peripheral Collisions
//
//////////////////////////////////////////////////////////////////////
#ifndef StPeCEvent_h
#define StPeCEvent_h

#include "Rtypes.h"
#include "StPeCEnumerations.h"
#include "TObject.h"
#include "TClonesArray.h"
#ifndef __CINT__
#include "PhysicalConstants.h"
///#include "StEventTypes.h"
#endif /* __CINT__ */
#include "StPeCPair.h"
#include "StPeCTrack.h"
#include "SystemOfUnits.h"

class StPeCEvent: public TObject {

public:

                                  StPeCEvent();
  virtual                         ~StPeCEvent();


  Long_t                          eventNumber() const;
  Long_t                          runNumber() const;
  Int_t                           getNTot() const;
  Int_t                           getNPrim() const;
  Int_t                           getQTot() const;
  Int_t                           getNPriPairs ( ) { return nPPairs ; } ;
  Int_t                           getNSecPairs ( ) { return nSPairs ; } ;
  StPeCPair*                      getPriPair ( Int_t i ) ;
  StPeCPair*                      getSecPair ( Int_t i ) ;
  Float_t                         getPt() const;
  Float_t                         yRap() const;
  Float_t                         getXVertex() const;
  Float_t                         getYVertex() const;
  Float_t                         getZVertex() const;
#ifndef __CINT__
  StMuDst* muDst;
  StEvent*                        eventP ;
  void                            addPair(StPeCPair* pair) const;
  Int_t                           fill ( StEvent* event ) ;
  Int_t                           fill(StMuDst* mudst);
  Int_t                           infoLevel ;
  void                            setInfoLevel ( Int_t in ) { infoLevel = in ; } ;
  StLorentzVectorF                getEvent4Momentum(StPeCSpecies pid) const;
#endif /*__CINT__*/
  void                            clear ( ) ;
  void                            reset ( ) ;

  Float_t                         mInv(StPeCSpecies pid) const;
  Float_t                         yRap(StPeCSpecies pid) const;

private:
  Int_t                           eventN;
  Int_t                           runN;
  Int_t                           nTot;
  Int_t                           nPrim;
  Int_t                           qTot;
  Float_t                         bField ;
  Float_t                         pt;
  Float_t                         xVertex;
  Float_t                         yVertex;
  Float_t                         zVertex;
  Float_t                         rVertex;
  Int_t                           nTracks ;
  Int_t                           nPPairs ;
  Int_t                           nSPairs ;
  TClonesArray                   *pPairs ;
  TClonesArray                   *sPairs ;
  TClonesArray                   *tracks;


  ClassDef(StPeCEvent,1)
};

#endif





