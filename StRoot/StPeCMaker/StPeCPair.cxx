//////////////////////////////////////////////////////////////////////
//
// $Id: StPeCPair.cxx,v 1.10 2003/02/05 17:14:05 yepes Exp $
// $Log: StPeCPair.cxx,v $
// Revision 1.10  2003/02/05 17:14:05  yepes
// Adding bField and pPairs.psi to tree
//
// Revision 1.9  2002/12/16 23:05:38  yepes
// *** empty log message ***
//
// Revision 1.8  2002/12/16 23:04:02  yepes
// Field comes in KGauss and should be passed to routines in Teslas
// problem pointed out by Vladimir
//
// 
// Revision 1.7  2002/03/19 22:23:49  meissner
// New variables: zdc unatt., Trigger word, MC tree if Geant Branch, DCA  for primary pairs, all tracks for secondary pairs (Test)
//
// Revision 1.6  2001/02/21 20:42:12  yepes
// Add ctb signals to tree
//
// Revision 1.5  2001/02/13 17:54:43  yepes
// still problems on differnt platforms
//
// Revision 1.4  2001/02/12 21:15:59  yepes
// New version of StPeCMaker, lots of changes
//
// Revision 1.1  2000/04/21 19:12:30  nystrand
// First Version
//
// Revision 1.1  2000/03/24 22:37:06  nystrand
// First version of StPeCPair
//
// Revision 1.0  2000/03/20 23:28:50  nystrand
//
//////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include "StPeCPair.h"
#include "StEventTypes.h"

ClassImp(StPeCPair)

StPeCPair::StPeCPair() {
} 

StPeCPair::~StPeCPair() {
}

#ifndef __CINT__
StPeCPair::StPeCPair ( StTrack* trk1, StTrack* trk2, 
                       Bool_t primaryFlag, StEvent* event ) {
   track1 = trk1;
   track2 = trk2;
   fill ( primaryFlag, event ) ;
}

StPeCPair::StPeCPair ( StMuTrack* trk1, StMuTrack* trk2, 
                       Bool_t primaryFlag, StMuEvent* event ) {
   muTrack1 = trk1;
   muTrack2 = trk2;
   fill ( primaryFlag, event ) ;
}

void StPeCPair::setTrack1(StTrack* trk) {
  track1 = trk;
}
void StPeCPair::setTrack2(StTrack* trk) {
  track2 = trk;
}

void StPeCPair::setTrack1(StMuTrack* trk) {
  muTrack1 = trk;
}
void StPeCPair::setTrack2(StMuTrack* trk) {
  muTrack2 = trk;
}

StTrack* StPeCPair::getTrack1() { return track1; }
StTrack* StPeCPair::getTrack2() { return track2; }

StMuTrack* StPeCPair::getMuTrack1() { return muTrack1; }
StMuTrack* StPeCPair::getMuTrack2() { return muTrack2; }

StLorentzVectorF StPeCPair::getPair4Momentum(StPeCSpecies pid) const{
  StLorentzVectorF p4pair(0.0,0.0,0.0,0.0);
  if      ( pid == 0 ) return pionH.Mom4 ;
  else if ( pid == 1 ) return kaonH.Mom4 ;
  else if ( pid == 2 ) return protonH.Mom4 ;
  else if ( pid == 3 ) return electronH.Mom4 ;
  else if ( pid == 4 ) return muonH.Mom4 ;
  else {
     printf ( "StPeCPair::getPair4Momentum: wrong pid %d \n", pid ) ;
     return p4pair ; 
  }
}
#endif /*__CINT__*/



Int_t StPeCPair::fill ( Bool_t primaryFlag, StEventSummary* summary, 
                        StThreeVectorF& p1, StPhysicalHelixD& h1, short charge1,
                        StThreeVectorF& p2, StPhysicalHelixD& h2, short charge2,  
                        StThreeVectorF& primaryVertexPosition ) {
//
//  Check whether tracks are primary or secondary
//  if they are secondary find point of closest approach
//  and work with momentum at that point
//
   pPartDca  = 0. ;
   pV0Dca    = 0. ;
   rV0       = 0. ;
   phiV0     = 0. ;
   zV0       = 0. ;
   // Want the DCA Vertex info  also for the Primary Pair !!
   //  if ( !primaryFlag ) {
   pairD dcaLengths ;
   dcaLengths = h1.pathLengths(h2);

   Float_t bField ;
   if ( summary != 0 ) bField = summary->magneticField();
   else bField = 2.5 ;

   // The momentum we do not need for the primary pair ....
   if ( !primaryFlag ) {
      p1 = h1.momentumAt(dcaLengths.first, bField*0.1 ) ;
      p2 = h2.momentumAt(dcaLengths.second, bField*0.1 ) ;
   }

   StThreeVectorD x1 = h1.at(dcaLengths.first);
   StThreeVectorD x2 = h2.at(dcaLengths.second);
   StThreeVectorD x = (x1-x2) ;
   pPartDca = x.mag();
   //
   //  Construct a helix with very large momentums
   //  to get intersection of V0 with vertex
   //
   StThreeVectorD xMean = (x1+x2)/2. ;
   rV0   = xMean.perp();
   phiV0 = xMean.phi();
   zV0   = xMean.z();
   
   StThreeVectorD pSum  = p1+p2 ;
   StPhysicalHelixD v0Helix ( pSum, xMean, 0.1*bField, 100000./GeV ) ;
   pV0Dca = v0Helix.distance ( primaryVertexPosition ) ;
   

   StThreeVectorF p = p1 + p2 ;
   pPt              = p.perp() ;
   pPz              = p.z() ;
   pPsi             = p.phi();
  // Opening angle of the pair in the CM (lab)
  // cos(theta) = (p1"dot"p2)/(abs(p1)*abs(p2))
   Float_t ScalarProduct = p1*p2;
   Float_t Denominator   = p1.mag()*p2.mag();
   if(Denominator) {
     pAngle = acos(ScalarProduct/Denominator);
   }else{
     pAngle = -999;
   }
   if (p1.perp() * p2.perp()) {
     pXyAngle = acos((p1.x()*p2.x()+p1.y()*p2.y())/p1.perp()/p2.perp());
   } else { 
     pXyAngle = -999;
   }
//
//  Calculate Armenteros variables
//
   Float_t p1AlongPtot = p*p1/p.mag() ; 
   Float_t p2AlongPtot = p*p2/p.mag() ;

   Float_t pt1Ptot = sqrt(p1.mag()*p1.mag()-p1AlongPtot*p1AlongPtot);

   pPtArm = pt1Ptot ;
   pAlpha = (p1AlongPtot-p2AlongPtot)/(p1AlongPtot+p2AlongPtot);
// printf ( "p1AlongPtot p1AlongPtot alpha %f %f %f \n",
//           p1AlongPtot, p2AlongPtot, pAlpha ) ;
// printf ( " p1 %f %f %f \n", p1.x(), p1.y(), p1.z() ) ;
// printf ( " p2 %f %f %f \n", p2.x(), p2.y(), p2.z() ) ;
// printf ( " p  %f %f %f \n", p.x(), p.y(), p.z() ) ;

   Float_t mptcle=0.0;
//
//   Loop over different species
//
   Float_t mInv, cosThetaStar ;
   StLorentzVectorF FourMomentum ; 
   StPeCSpec* species ;
   for ( int i = 0 ; i < nSpecies ; i++ )
	   {
      if ( i == pion )    {
         mptcle = pion_plus_mass_c2;
	 species = &pionH ;
      }
      else if ( i == kaon ) {
         mptcle = 493.677*MeV;
	 species = &kaonH ;
      }
      else if ( i == proton ) {
         mptcle = proton_mass_c2;
	 species = &protonH ;
      }
      else if ( i == electron ) {
         mptcle = electron_mass_c2;
	 species = &electronH ;
      }
      else if ( i == muon ) {
         mptcle = 105.6584*MeV; 
	 species = &muonH ;
      }
      else {
         printf ( "StPecPair:calculatePair4Momentum; wrong pid %d \n", i ) ;
 	continue ;
      }
      
      StLorentzVectorF p4pair(0.0,0.0,0.0,0.0);
      Float_t          e1 = p1.massHypothesis(mptcle);
      Float_t          e2 = p2.massHypothesis(mptcle);
      StLorentzVectorF pf1(e1,p1);
      StLorentzVectorF pf2(e2,p2);
      p4pair = pf1 + pf2;
      FourMomentum = p4pair ;
      mInv = p4pair.m() ;

  // ThetaStar is the angle between of one of the daughter tracks
  // and the Z-axis in the Helicity frame. The Helicity frame is
  // that rest frame of the parent particle in which the direction of 
  // the scattered nucleus is along the negative Z-axis. See K.Schilling,
  // P.Seyboth, G. Wolf Nucl. Phys. B 15(1970)397-412.
  // Since the outgoing nuclei are not tagged, this direction cannot 
  // be determined exactly. Because of the low momentum transfers it is,
  // however, a reasonable approximation to assume that the nuclei move 
  // parallel to the Z-axis of the lab frame. 

//
// Get the sign right for the boost lab --> parent rest frame
// Default is in the other direction
      StThreeVectorF sp = -1.0*p4pair.vect();
      p4pair.setVect(sp);
      pf1 = pf1.boost(p4pair);
      pf2 = pf2.boost(p4pair);
      Float_t d1th = pf1.cosTheta();  
      Float_t d2th = pf2.cosTheta();
  // Define cosThetaStar in the interval 0<cosThetaStar<1
      if( d1th > 0 ) cosThetaStar = d1th;
      else           cosThetaStar = d2th;

      species->pid = i ;
      species->mInv         = mInv ;
      species->yRap         = FourMomentum.rapidity() ;
      species->Mom4         = FourMomentum ;
      species->cosThetaStar = cosThetaStar ;
   }
//
//  fill our local Track class
#ifndef __CINT__
   Int_t prim = 1 ;
	if (track1 && track2)
	{
	   tr1.set(prim,track1);
	   tr2.set(prim,track2);
	}
	else if (muTrack1 && muTrack2)
	{
	   tr1.set(prim,muTrack1);
	   tr2.set(prim,muTrack2);
	}
#endif


   return 0 ;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

Int_t StPeCPair::fill ( Bool_t primaryFlag, StMuEvent* event  ) {

   pCharge           = muTrack1->charge()+muTrack2->charge();

   StThreeVectorF p1 ; 
   StThreeVectorF p2 ;
   StPhysicalHelixD h1 ;
   StPhysicalHelixD h2 ;
   short charge1, charge2 ;
//
//  If charges have different sign
//  1=+ and 2=-
//
   if ( muTrack1->charge() > 0 && muTrack2->charge() < 0 ) {
      p1      = muTrack1->momentum();
      p2      = muTrack2->momentum();
      charge1 = muTrack1->charge();
      charge2 = muTrack2->charge();
      h1      = muTrack1->helix() ;
      h2      = muTrack2->helix() ;
   }
   else {
      p1      = muTrack2->momentum();
      p2      = muTrack1->momentum();
      charge1 = muTrack2->charge();
      charge2 = muTrack1->charge();
      h1      = muTrack2->helix() ;
      h2      = muTrack1->helix() ;
   }

   StThreeVectorF vtx = event->primaryVertexPosition() ;

   fill ( primaryFlag, &(event->eventSummary()), 
	  p1, h1, charge1, p2, h2, charge2, vtx ) ; 
   
   return 0 ;

}
Int_t StPeCPair::fill ( Bool_t primaryFlag, StEvent* event  ) {
//
   pCharge           = track1->geometry()->charge()+track2->geometry()->charge();

   StThreeVectorF p1 ; 
   StThreeVectorF p2 ;
   StPhysicalHelixD h1 ;
   StPhysicalHelixD h2 ;
   short charge1, charge2 ;
//
//  If charges have different sign
//  1=+ and 2=-
//
   if ( track1->geometry()->charge() > 0 && track2->geometry()->charge() < 0 ) {
      p1      = track1->geometry()->momentum();
      p2      = track2->geometry()->momentum();
      charge1 = track1->geometry()->charge();
      charge2 = track2->geometry()->charge();
      //   if ( !primaryFlag ) {
      h1 = track1->geometry()->helix() ;
      h2 = track2->geometry()->helix() ;
      // }
   }
   else {
      p1 = track2->geometry()->momentum();
      p2 = track1->geometry()->momentum();
      charge1 = track2->geometry()->charge();
      charge2 = track1->geometry()->charge();
      // if ( !primaryFlag ) {
      h1 = track2->geometry()->helix() ;
      h2 = track1->geometry()->helix() ;
      // }
   }

   StEventSummary* summary = 0 ;
   StPrimaryVertex* vtx = 0;
   vtx = event->primaryVertex();
   summary = event->summary();
   StThreeVectorF vtxP  ;
   //
   //  If there is no primary vertex assume (0,0,0)
   //
   if ( vtx ) vtxP = vtx->position() ;
   else {
      vtxP.setX(0.);
      vtxP.setY(0.);
      vtxP.setZ(0.);
   }

   fill ( primaryFlag, summary, p1, h1, charge1, p2, h2, charge2, vtxP ) ;


   return 0 ;
}


Int_t StPeCPair::getSumCharge() const{
  return pCharge ; 
}

Float_t StPeCPair::getSumPt() const{
  return pPt ;
}

Float_t StPeCPair::getSumPz() const{
  return pPz ;
}

Float_t StPeCPair::getMInv(StPeCSpecies pid) const{
  if   ( pid == 0 )    return pionH.mInv ;
  else if ( pid == 1 ) return kaonH.mInv ;
  else if ( pid == 2 ) return protonH.mInv ;
  else if ( pid == 3 ) return electronH.mInv ;
  else if ( pid == 4 ) return muonH.mInv ;
  else {
     printf ( "StPeCPair::getMInv: wrong pid %d \n", pid ) ;
     return 0 ; 
  }
}

Float_t StPeCPair::getOpeningAngle() const{
  return pAngle ;
}
Float_t StPeCPair::getCosThetaStar(StPeCSpecies pid) const{
  if   ( pid == 0 )    return pionH.cosThetaStar ;
  else if ( pid == 1 ) return kaonH.cosThetaStar ;
  else if ( pid == 2 ) return protonH.cosThetaStar ;
  else if ( pid == 3 ) return electronH.cosThetaStar ;
  else if ( pid == 4 ) return muonH.cosThetaStar ;
  else {
    printf ( "StPeCPair::getCosThetaStar: wrong pid %d \n", pid ) ;
     return 0 ; 
  }
}


