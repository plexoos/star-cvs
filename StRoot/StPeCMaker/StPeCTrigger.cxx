/////////////////////////////////////////////////////////////////////
//
// $Id: StPeCTrigger.cxx,v 1.7 2003/09/02 17:58:46 perev Exp $
// $Log: StPeCTrigger.cxx,v $
// Revision 1.7  2003/09/02 17:58:46  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.6  2002/12/16 23:04:02  yepes
// Field comes in KGauss and should be passed to routines in Teslas
// problem pointed out by Vladimir
//
// Revision 1.5  2002/03/19 22:23:52  meissner
// New variables: zdc unatt., Trigger word, MC tree if Geant Branch, DCA  for primary pairs, all tracks for secondary pairs (Test)
//
// add un attenuated zdc;s
// Revision 1.4  2001/04/25 18:12:05  perev
// HPcorrs
//
// Revision 1.3  2001/02/21 20:54:25  yepes
// *** empty log message ***
//
// Revision 1.0  2000/12/11 
//
//////////////////////////////////////////////////////////////////////
#include <Stiostream.h>
#include "StPeCTrigger.h"
#include "StPeCMaker.h"
#include "StEventTypes.h"
#include "StChain.h"
#include "Stypes.h"
#include "StMessMgr.h"
#include "StPeCCtbSlat.h"



ClassImp(StPeCTrigger)

StPeCTrigger::StPeCTrigger() {
//
//  Set y2k L0
//
   l0_2000 = new StPeCL0 ();
   l0_2000->setYear1Input ();
   l0_2000->setP4SLuts ();
//
//  Set y2k corrected L0
//
   l0_2000Corrected = new StPeCL0 ();
   l0_2000Corrected->setYear1Input ();
   l0_2000Corrected->setP4PLuts ();
//
//  Set y2k+1 L0
//
   l0Offline2001 = new StPeCL0 ();
   l0Offline2001->setYear2Input ();
   l0Offline2001->setCountingLuts ();
//
   ctbSlats  = new TClonesArray ("StPeCCtbSlat", 10);

}
StPeCTrigger::~StPeCTrigger() {
   ctbSlats->Clear();
   delete ctbSlats ;
}

void StPeCTrigger::clear() {
   ctbSlats->Clear();
}

Int_t StPeCTrigger::process(StEvent *event)
{
  unsigned int i,j;

  // get trigger word 
  tw = event->l0Trigger()->triggerWord();

  
  l0_2000->setInfoLevel ( infoLevel );
//  l0_2000Corrected->setInfoLevel ( infoLevel );

  StL0Trigger* l0Data = event->l0Trigger();
  StTriggerDetectorCollection* trg = event->triggerDetectorCollection();
  if ( !trg ) {
     printf ( "StPeCTrigger::StPeCTrigger: No trigger information \n" ) ;
     return 1 ;
  }
  StCtbTriggerDetector& ctb = trg->ctb();
  StMwcTriggerDetector& mwc = trg->mwc();
  StZdcTriggerDetector& zdc = trg->zdc();

  if ( l0Data && infoLevel > 0 ){
    cout << "L0 mwcCtbMultiplicity = " << l0Data->mwcCtbMultiplicity() << endl;
  }
  if(&zdc){
    if ( infoLevel > 1 ) {
       cout << "ZDC sum " << zdc.adcSum() << endl;
       cout << "ZDC sum west " << zdc.adcSum(west) << endl;
       cout << "ZDC sum east " << zdc.adcSum(east) << endl;
    }
    // attenuated signals 
    zdcWest = zdc.adcSum(west) ;
    zdcEast = zdc.adcSum(east) ;
    zdcSum  = zdc.adcSum() ;
    // unattenuated  // see StZdcTriggerDetector documentation
    zdcWestUA = zdc.adc(0) ;
    zdcEastUA = zdc.adc(4) ;
    zdcSumUA  = zdcEastUA+zdcWestUA ;
  }
  else {
    zdcWestUA = 0 ;
    zdcEastUA = 0 ;
    zdcSumUA  = 0 ;

    zdcWest = 0 ;
    zdcEast = 0 ;
    zdcSum  = 0 ;
  }
//
//   MWC
//
  float mwcsum = 0.0;

  float mwcThres = 2. ;
  nMwcHits = 0 ;
  mwcSW = 0 ;
  mwcNW = 0 ;
  mwcTW = 0 ;
  mwcBW = 0 ;
  mwcSE = 0 ;
  mwcNE = 0 ;
  mwcTE = 0 ;
  mwcBE = 0 ;
  if(&mwc){
    for(i=0; i<mwc.numberOfSectors(); i++){
      for(j=0; j<mwc.numberOfSubSectors(); j++){
	mwcsum += mwc.mips(i,j,0);
	if ( mwc.mips(i,j,0) > mwcThres ) { 
	   nMwcHits++ ;
           if      ( i ==  1 || i ==  2 || i ==  3 ) mwcSW++ ;
	   else if ( i ==  4 || i ==  5 || i ==  6 ) mwcBW++ ;
	   else if ( i ==  7 || i ==  8 || i ==  9 ) mwcNW++ ;
	   else if ( i == 10 || i == 11 || i ==  0 ) mwcTW++ ;
           else if ( i == 13 || i == 14 || i == 15 ) mwcNE++ ;
	   else if ( i == 16 || i == 17 || i == 18 ) mwcBE++ ;
	   else if ( i == 19 || i == 20 || i == 21 ) mwcSE++ ;
	   else if ( i == 22 || i == 23 || i == 12 ) mwcTE++ ;
	}
      }
    }
    if ( infoLevel > 1 ) 
       cout << "mwc mips " << mwcsum << endl;
  }
//
//   CTB
//
  float ctbsum=0.0;
  ctbSW = 0 ;
  ctbNW = 0 ;
  ctbTW = 0 ;
  ctbBW = 0 ;
  ctbSE = 0 ;
  ctbNE = 0 ;
  ctbTE = 0 ;
  ctbBE = 0 ;
  nCtbHits = 0 ;
  float ctbThres = 2. ;
  if(&ctb){
    TClonesArray &pCtbSlats = *ctbSlats;
    for(i=0; i<120; i++){
      for(j=0; j<2; j++){
	ctbsum += ctb.mips(i,j,0);
	if ( ctb.mips(i,j,0) > 0 ) {
	   Byte_t i_eta = (Byte_t)(j+1);
	   Byte_t i_phi = (Byte_t)(i+1);
	   Int_t  adc   = (Int_t)ctb.mips(i,j,0) ;
           new(pCtbSlats[nCtbHits++]) StPeCCtbSlat(i_phi,i_eta,adc) ;
	}

	if ( ctb.mips(i,j,0) > ctbThres ) {
           if      ( i >=   5 && i <  20 ) ctbSW++ ;
	   else if ( i >=  20 && i <  35 ) ctbBW++ ;
	   else if ( i >=  35 && i <  50 ) ctbNW++ ;
	   else if ( i >=  50 && i <  60 ) ctbTW++ ;
	   else if (             i <   5 ) ctbTW++ ;
           else if ( i >=  65 && i <  80 ) ctbNE++ ;
	   else if ( i >=  80 && i <  95 ) ctbBE++ ;
	   else if ( i >=  95 && i < 110 ) ctbSE++ ;
	   else if ( i >= 110            ) ctbTE++ ;
	   else if ( i >=  60 && i <  65 ) ctbTE++ ;
         }
      }
    }
    if ( infoLevel > 1 ) 
       cout << "CTB MIP total sum = " << ctbsum << endl;
  }
  ctbSum = ctbsum ;
  mwcSum = mwcsum ;
//
//   Simulate l0
//
  p4  = l0_2000->process ( event ) ;
  p4c = l0_2000Corrected->process ( event ) ;
  p5  = l0Offline2001->process ( event ) ;
//
//   Take quadrants from 2001 wiring
//
  // L0 simulator
  Bool_t trgOut[40];
  {for ( int i = 0 ; i < 40 ; i++ ) trgOut[i] = 0 ;}

  if ( infoLevel > 2 ) {
     printf ( "trgBits : " ) ;
     for ( int i = 0 ; i < 24 ; i++ ) {
        printf ( "%d ", trgOut[i] ) ;
     }
     printf ( "\n" ) ;
  }
//
//   Get Ftpc info
//
  StFtpcHitCollection*        ftpHitC = 0 ;
  StFtpcPlaneHitCollection*   plane;
  StFtpcSectorHitCollection*  sector;

  ftpW = 0 ;
  ftpE = 0 ;

  ftpHitC = event->ftpcHitCollection();
  int nFtpcPlanes = 20 ;
  if ( ftpHitC ) {
     printf ( "there is a hit collection \n" ) ;
     for ( int iPlane = 0 ; iPlane < nFtpcPlanes ; iPlane++ ) {
        plane = 0 ;
	plane = ftpHitC->plane(iPlane);
	if ( !plane ) continue ;
	for ( int iSector = 0 ; iSector < 6 ; iSector++ ) {
	   sector = 0 ;
	   sector = plane->sector(iSector);
	   if ( !sector ) continue ;
           StSPtrVecFtpcHit& hits = sector->hits();
	   for ( unsigned int iHit = 0 ; iHit < hits.size() ; iHit++ ) {
//     printf ( "plane %d sector %d x y z %f %f %f \n", iPlane, iSector,
//               hits[iHit]->position().x(), hits[iHit]->position().y(), hits[iHit]->position().z()) ;
	      if ( iPlane < 10 ) ftpW++ ;		
	      else              ftpE++ ;

	   }
	}
     }

  }

  return 0 ;
}


Int_t StPeCTrigger::process(StMuDst* mudst)
{
  unsigned int i,j;

  // get trigger word 
  tw = mudst->event()->l0Trigger().triggerWord();

  
  l0_2000->setInfoLevel ( infoLevel );
//  l0_2000Corrected->setInfoLevel ( infoLevel );

  StL0Trigger* l0Data = &mudst->event()->l0Trigger();
//  StFpdCollection* trg = &mudst->event()->fpdCollection();
//  if ( !trg ) {
//     printf ( "StPeCTrigger::StPeCTrigger: No trigger information \n" ) ;
//     return 1 ;
//  }
  StCtbTriggerDetector& ctb = mudst->event()->ctbTriggerDetector();
//  StMwcTriggerDetector& mwc = &mudst->event()->ctbTriggerDetector();
  StZdcTriggerDetector& zdc = mudst->event()->zdcTriggerDetector();

  if ( l0Data && infoLevel > 0 ){
    cout << "L0 mwcCtbMultiplicity = " << l0Data->mwcCtbMultiplicity() << endl;
  }
  if(&zdc){
    if ( infoLevel > 1 ) {
       cout << "ZDC sum " << zdc.adcSum() << endl;
       cout << "ZDC sum west " << zdc.adcSum(west) << endl;
       cout << "ZDC sum east " << zdc.adcSum(east) << endl;
    }
    // attenuated signals 
    zdcWest = zdc.adcSum(west) ;
    zdcEast = zdc.adcSum(east) ;
    zdcSum  = zdc.adcSum() ;
    // unattenuated  // see StZdcTriggerDetector documentation
    zdcWestUA = zdc.adc(0) ;
    zdcEastUA = zdc.adc(4) ;
    zdcSumUA  = zdcEastUA+zdcWestUA ;
  }
  else {
    zdcWestUA = 0 ;
    zdcEastUA = 0 ;
    zdcSumUA  = 0 ;

    zdcWest = 0 ;
    zdcEast = 0 ;
    zdcSum  = 0 ;
  }
//
//   CTB
//
  float ctbsum=0.0;
  ctbSW = 0 ;
  ctbNW = 0 ;
  ctbTW = 0 ;
  ctbBW = 0 ;
  ctbSE = 0 ;
  ctbNE = 0 ;
  ctbTE = 0 ;
  ctbBE = 0 ;
  nCtbHits = 0 ;
  float ctbThres = 2. ;
  if(&ctb){
    TClonesArray &pCtbSlats = *ctbSlats;
    for(i=0; i<120; i++){
      for(j=0; j<2; j++){
	ctbsum += ctb.mips(i,j,0);
	if ( ctb.mips(i,j,0) > 0 ) {
	   Byte_t i_eta = (Byte_t)(j+1);
	   Byte_t i_phi = (Byte_t)(i+1);
	   Int_t  adc   = (Int_t)ctb.mips(i,j,0) ;
           new(pCtbSlats[nCtbHits++]) StPeCCtbSlat(i_phi,i_eta,adc) ;
	}

	if ( ctb.mips(i,j,0) > ctbThres ) {
           if      ( i >=   5 && i <  20 ) ctbSW++ ;
	   else if ( i >=  20 && i <  35 ) ctbBW++ ;
	   else if ( i >=  35 && i <  50 ) ctbNW++ ;
	   else if ( i >=  50 && i <  60 ) ctbTW++ ;
	   else if (             i <   5 ) ctbTW++ ;
           else if ( i >=  65 && i <  80 ) ctbNE++ ;
	   else if ( i >=  80 && i <  95 ) ctbBE++ ;
	   else if ( i >=  95 && i < 110 ) ctbSE++ ;
	   else if ( i >= 110            ) ctbTE++ ;
	   else if ( i >=  60 && i <  65 ) ctbTE++ ;
         }
      }
    }
    if ( infoLevel > 1 ) 
       cout << "CTB MIP total sum = " << ctbsum << endl;
  }
  ctbSum = ctbsum ;
  mwcSum = 0 ;
//
//   Simulate l0
//
  p4  = l0_2000->process(mudst);
  p4c = l0_2000Corrected->process(mudst);
  p5  = l0Offline2001->process(mudst);
//
//   Take quadrants from 2001 wiring
//
  // L0 simulator
  Bool_t trgOut[40];
  {for ( int i = 0 ; i < 40 ; i++ ) trgOut[i] = 0 ;}

  if ( infoLevel > 2 ) {
     printf ( "trgBits : " ) ;
     for ( int i = 0 ; i < 24 ; i++ ) {
        printf ( "%d ", trgOut[i] ) ;
     }
     printf ( "\n" ) ;
  }
  return 0 ;
}
