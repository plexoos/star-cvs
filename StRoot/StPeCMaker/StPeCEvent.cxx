//////////////////////////////////////////////////////////////////////
//
// $Id: StPeCEvent.cxx,v 1.8 2001/04/23 21:44:30 meissner Exp $
// $Log: StPeCEvent.cxx,v $
// Revision 1.8  2001/04/23 21:44:30  meissner
// add dEdx z variable to tree, setFormat(1) for tree, use private BetheBloch (temp solution)
//
// Revision 1.7  2001/02/21 20:41:58  yepes
// Add ctb signals to tree
//
// Revision 1.6  2001/02/13 17:54:40  yepes
// still problems on differnt platforms
//
// Revision 1.5  2001/02/12 22:33:51  yepes
// avoid printing
//
// Revision 1.4  2001/02/12 21:15:42  yepes
// New version of StPeCMaker, lots of changes
//
// Revision 1.3  2000/04/24 19:15:27  nystrand
// Fix of a possible memory leak
//
// Revision 1.2  2000/04/21 19:10:30  nystrand
// Include StPeCPair class
//
// Revision 1.1  2000/03/24 22:37:06  nystrand
// First version of StPeCEvent
//
// Revision 1.0  2000/03/20 23:28:50  nystrand
//
//////////////////////////////////////////////////////////////////////
// TODO: 
//  Understand what the redifinition of the TClone Array does
//   -> dereferencing the pointer to get  just the array 
//  Place the cut on  track->flag() in a unuque place (Internal track array?)
//  Understand the dublication of StPeCTrack  (does a tree work with pointers)
// ///////////////////////////////////////////////////////////////////
#include <iostream.h>
#include "StPeCEvent.h"
#include "StEventTypes.h"
#include "StPeCEnumerations.h"


ClassImp(StPeCEvent)

StPeCEvent::StPeCEvent() {
  // StPeCMaxTracks ..... looks more like Fortran......
  pPairs  = new TClonesArray ("StPeCPair", StPeCnMaxTracks);
  sPairs  = new TClonesArray ("StPeCPair", StPeCnMaxTracks);
  tracks  = new TClonesArray ("StPeCTrack",StPeCnMaxTracks);
  nPPairs  = 0 ;
  nSPairs  = 0 ;
  nTracks  = 0 ;
  eventP   = 0 ;
}

StPeCEvent::~StPeCEvent() {
  clear() ;
  delete pPairs ;
  delete sPairs ;
  delete tracks ;
}


void  StPeCEvent::clear ( ) {
   eventN = 0 ;
   runN   = 0 ;
   nTot   = 0 ;
   nPrim  = 0 ;
   qTot   = 0 ;
   pt     = 0 ;
   xVertex = 0 ;
   yVertex = 0 ;
   zVertex = 0 ;
   nPPairs = 0 ;
   nSPairs = 0 ;
   nTracks = 0 ;


   pPairs->Clear();
   sPairs->Clear();
   tracks->Clear();
}

Int_t StPeCEvent::fill ( StEvent *event ) {

  eventP = event ;

  // Set Run and Event Number
  eventN = event->id() ;
  runN   = event->runId(); 

  //get trigger info

  Int_t   NGlobal=0; 
  Int_t   NPrimaries=0; 
  Int_t   SumQ=0; 
  Float_t SumPx=0.0; 
  Float_t SumPy=0.0;  

  // Get the track nodes
  StSPtrVecTrackNode& exnode = event->trackNodes();
  Int_t nnode=exnode.size();
  // This only dereferences a pointer
  // TClonesArray &pTracks = *tracks;

  // local pointer array for track needed for cuts
  for( Int_t in=0; in<nnode; in++ ) {
    UInt_t nprim = exnode[in]->entries(primary);
    UInt_t nglob = exnode[in]->entries(global);
    if( nprim>1 || nglob>1 ){
      cout<<"There could be a problem! nprim= "<<nprim<<"  nglob= "<<nglob<<endl;
      // HERE must flag be tested. 

    }
    if( nprim==1 ){
      StTrack *tp = exnode[in]->track(primary);
      // DANGER THAT SHOULD BE DONE IN A PROPPER WAY, CHECK 4 PLACES !
      if (! (tp->flag()>0)) continue;
      // -----------------------------------------------------------
      NPrimaries++;
      float px = tp->geometry()->momentum().x();
      float py = tp->geometry()->momentum().y();
      SumPx = SumPx + px; SumPy = SumPy + py;
      SumQ  = SumQ  + tp->geometry()->charge();
      // new(pTracks[nTracks++]) StPeCTrack(1,tp) ;
      // need to dereference the Array pointer first 
      new((*tracks)[nTracks++]) StPeCTrack(1,tp) ;
      // Store the Primaries in a vector for formation of pairs
    }
    if( nprim==0 && nglob==1 ){
      StTrack *tnp = exnode[in]->track(global);
      // DANGER THAT SHOULD BE DONE IN A PROPPER WAY, CHECK 4 PLACES !
      if (! (tnp->flag()>0)) continue;
      // -----------------------------------------------------------
      NGlobal++; 
      // new(pTracks[nTracks++]) StPeCTrack(0,tnp) ;
      new((*tracks)[nTracks++]) StPeCTrack(0,tnp) ;
    }
  }

  nPrim             = NPrimaries ;
  nTot              = NGlobal + NPrimaries  ;
  qTot = SumQ ;
  pt   = sqrt( SumPx*SumPx + SumPy*SumPy );

  StPrimaryVertex* vtx = event->primaryVertex();
  if(vtx) {
    xVertex = vtx->position().x();
    yVertex = vtx->position().y();
    zVertex = vtx->position().z();
    if ( infoLevel > 1 ) {
     cout << "StPeCEvent : primary vertex " << vtx->position().x() << " " 
       << vtx->position().y() << " " << vtx->position().z() << endl;
    }
  }
  else {
    xVertex = -9999. ;
    yVertex = -9999. ;
    zVertex = -9999. ;
    cout<<"StPeCEvent: There was no primary vertex!"<<endl;
  }

  // HERE  flag must  be tested. 
  StPeCPair* lPair ; 
  nPPairs = 0 ;
  StTrack *trk1, *trk2 ;
  for( Int_t i1=0; i1<nnode-1; i1++ ) {
     if( exnode[i1]->entries(primary) !=1 ) continue ;
     for( Int_t i2=i1+1; i2<nnode; i2++ ) {
        if( exnode[i2]->entries(primary) !=1 ) continue ;
 
        trk1 = exnode[i1]->track(primary);
        trk2 = exnode[i2]->track(primary);
	// DANGER 
	if (! (trk1->flag()>0)) continue;
	if (! (trk2->flag()>0)) continue;
	// --------
	// get pointer to memebr ?
        // TClonesArray &ppairs = *pPairs;
        // lPair = new(ppairs[nPPairs++]) StPeCPair(trk1,trk2,1,event) ;
        lPair = new((*pPairs)[nPPairs++]) StPeCPair(trk1,trk2,1,event) ;
#ifdef PECPRINT
        cout << "StPeCEvent : Primary Pair : " 
           << "  sumQ = " << lPair->getSumCharge()
           << "  sumPt = " << lPair->getSumPt()
           << "  mInv = " << lPair->getMInv(pion)
           << "  opening angle = " << lPair->getOpeningAngle()
           << "  cos(theta*) = " << lPair->getCosThetaStar(pion) << endl;
#endif	   
     }
  }
//
//   Look for V0
//
  // HERE must flag be tested. 
  for( Int_t i=0; i<nnode-1; i++ ) {
     if ( exnode[i]->entries(primary)  ) continue ; 
     if ( exnode[i]->entries(global)  !=1 ) continue ;
     for( Int_t j=i+1; j<nnode; j++ ) {
        if ( exnode[j]->entries(primary)     ) continue ; 
        if ( exnode[j]->entries(global)  !=1 ) continue ;
        StTrack *trk1 = exnode[i]->track(global);
        StTrack *trk2 = exnode[j]->track(global);

	// DANGER 
	if (! (trk1->flag()>0)) continue;
	if (! (trk2->flag()>0)) continue;
	// --------
	StPhysicalHelixD h1 = trk1->geometry()->helix() ;
	StPhysicalHelixD h2 = trk2->geometry()->helix() ;

	pairD dcaLengths = h1.pathLengths(h2);
        StThreeVectorD x1 = h1.at(dcaLengths.first);
        StThreeVectorD x2 = h2.at(dcaLengths.second);
        StThreeVectorD x = (x1-x2) ;
	if ( x.mag() > 10 ) continue ; // Hardwire cut

        // TClonesArray &spairs = *sPairs;
        // lPair = new(spairs[nSPairs++]) StPeCPair(trk1,trk2,0,event) ;
        lPair = new((*sPairs)[nSPairs++]) StPeCPair(trk1,trk2,0,event) ;
#ifdef PECPRINT
        cout << "StPeCEvent : Secondary Pair : " 
           << "  sumQ = " << lPair->getSumCharge()
           << "  sumPt = " << lPair->getSumPt()
           << "  mInv = " << lPair->getMInv(pion)
           << "  opening angle = " << lPair->getOpeningAngle()
           << "  cos(theta*) = " << lPair->getCosThetaStar(pion) << endl;
#endif	   
     }
  }
  return 0 ;
}

StPeCPair* StPeCEvent::getPriPair ( Int_t i ) {
   if ( i < 0 || i >= nPPairs ) return 0 ;
   TClonesArray &pairs = *pPairs;
   return (StPeCPair *)pairs[i] ;
}

StPeCPair* StPeCEvent::getSecPair ( Int_t i ) {
   if ( i < 0 || i >= nSPairs ) return 0 ;
   TClonesArray &pairs = *sPairs;
   return (StPeCPair *)pairs[i] ;
}

void StPeCEvent::reset() { 
   delete pPairs ;
   delete sPairs ;
   delete tracks ;
   pPairs  = 0 ;
   sPairs  = 0 ;
   nPPairs  = 0 ;
   nSPairs  = 0 ;
   tracks = 0 ;
   nTracks = 0 ;
}

Long_t  StPeCEvent::eventNumber() const{ return eventN; }
Long_t  StPeCEvent::runNumber() const{ return runN; }
Int_t   StPeCEvent::getNTot() const{ return nTot; }
Int_t   StPeCEvent::getNPrim() const{ return nPrim; }
Int_t   StPeCEvent::getQTot() const{ return qTot; }
Float_t StPeCEvent::getPt() const{ return pt; }
Float_t StPeCEvent::getXVertex() const{ return xVertex; }
Float_t StPeCEvent::getYVertex() const{ return yVertex; }
Float_t StPeCEvent::getZVertex() const{ return zVertex; }
#ifndef __CINT__


StLorentzVectorF StPeCEvent::getEvent4Momentum(StPeCSpecies pid) const{
  Float_t mptcle=0.0;
  if(pid==pion){
    mptcle = pion_plus_mass_c2;
  }
  if(pid==kaon){
    mptcle = 493.677*MeV;
  }
  if(pid==proton){
    mptcle = proton_mass_c2;
  }
  if(pid==electron){
    mptcle = electron_mass_c2;
  }
  if(pid==muon){
    mptcle = 105.6584*MeV; 
  }
  StLorentzVectorF p4event(0.0,0.0,0.0,0.0);

  StSPtrVecTrackNode& exnode = eventP->trackNodes();
  if ( !eventP ) {
     printf ( "StPeCEvent::getEvent4Momentum eventP null \n" ) ;
     return p4event ;
  }
  Int_t nnode=exnode.size();

  for( Int_t in=0; in<nnode; in++ ) {
    if( exnode[in]->entries(global) != 1 ) continue ;
    StTrack* trk = exnode[in]->track(primary);
    StThreeVectorF p = trk->geometry()->momentum();
    Float_t energy = p.massHypothesis(mptcle);
    StLorentzVectorF pfour(energy,p);
    p4event = p4event + pfour;
  }

  return p4event;
}
#endif /*__CINT__*/

Float_t StPeCEvent::mInv(StPeCSpecies pid) const{

  StLorentzVectorF p4event = getEvent4Momentum(pid);

  return p4event.m();
}

Float_t StPeCEvent::yRap(StPeCSpecies pid) const{
 

  StLorentzVectorF p4event = getEvent4Momentum(pid);

  return p4event.rapidity();
}

