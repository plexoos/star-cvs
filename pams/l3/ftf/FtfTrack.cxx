//:>------------------------------------------------------------------
//: FILE:       FtfTrack.cxx
//: HISTORY:
//:           28oct1996 version 1.00
//:           11aug1999 ppy primary flag fill in filling routines
//:           22aug1999 ppy fixing Debug routines (TRDEBUG flag on)  
//:           23aug1999 ppy change loop order in seekNextHit
//:           29aug1999 ppy move fill tracks from follow to build
//:           19nov1999 ppy add maxChi2Primary to decide whether track is primary
//:           27jan2000 ppy refHit replaced by xRefHit and yRefHit
//:           27jan2000  VOLUME, ROW and AREA classes replaced by FtfContainer
//:
//:<------------------------------------------------------------------
//:>------------------------------------------------------------------
//: CLASS:       FtfTrack
//: DESCRIPTION: Functions associated with this class
//: AUTHOR:      ppy - Pablo Yepes, yepes@physics.rice.edu
//:>------------------------------------------------------------------
#include <memory.h>
#include <stdio.h>
#include <math.h>
#include "FtfTrack.h"
#include "FtfHit.h"

//extern FtfFinder     tracker ;

#ifdef SL3ROOT
ClassImp(FtfTrack)
#endif

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    Add hits to track
// Arguments:
//        thisHit:  hit pointer
//        way     :  >0 add at beginning, <0 at end
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FtfTrack::add ( FtfHit *thisHit, int way )
{
//
//      Increment # hits in this track
//
  nHits++ ; 
//
//         Update pointers
//
  if ( way < 0 || nHits == 1 ) {
     if ( nHits > 1 ) lastHit->nextTrackHit = thisHit ;
     lastHit = thisHit ;
     innerMostRow = lastHit->row ;
     xLastHit = lastHit->x ;
     yLastHit = lastHit->y ;
  }
  else {
     thisHit->nextTrackHit = firstHit ; 
     firstHit = thisHit ;
     outerMostRow = firstHit->row ;
  }
//
//        Declare hit as used and fill chi2
//
  thisHit->setStatus ( this ) ;
//
//    Check whether a fit update is needed
//
  if ( nHits < para->minHitsForFit ) return ;
//
//    Include hit in xy fit parameter calculation
//
  
  s11Xy = s11Xy + thisHit->wxy ;
  s12Xy = s12Xy + thisHit->wxy * thisHit->xp ;
  s22Xy = s22Xy + thisHit->wxy * square(thisHit->xp) ;
  g1Xy  = g1Xy  + thisHit->wxy * thisHit->yp ;
  g2Xy  = g2Xy  + thisHit->wxy * thisHit->xp * thisHit->yp ;
  
 
  if ( nHits > para->minHitsForFit  )
  {
     ddXy  = s11Xy * s22Xy - square ( s12Xy ) ;
     a1Xy  = ( g1Xy * s22Xy - 
                g2Xy * s12Xy ) / ddXy ;
     a2Xy  = ( g2Xy * s11Xy - 
                g1Xy * s12Xy ) / ddXy ;
  }
//
//     Now in the sz plane
//
  if ( para->szFitFlag ) {
     s11Sz = s11Sz + thisHit->wz ;
     s12Sz = s12Sz + thisHit->wz * thisHit->s ;
     s22Sz = s22Sz + thisHit->wz * thisHit->s * thisHit->s ;
     g1Sz  = g1Sz  + thisHit->wz * thisHit->z ;
     g2Sz  = g2Sz  + thisHit->wz * thisHit->s * thisHit->z ;
  
     if ( nHits > para->minHitsForFit ) {
		
        ddSz  = s11Sz * s22Sz -  s12Sz * s12Sz ;
#ifdef TRDEBUG
	if ( ddSz != 0 ) {
#endif
           a1Sz  = ( g1Sz * s22Sz - 
                   g2Sz * s12Sz ) / ddSz ;
           a2Sz  = ( g2Sz * s11Sz - 
                   g1Sz * s12Sz ) / ddSz ;
#ifdef TRDEBUG
         }
         else
         {
            printf ( " \n Something strange going on " ) ;
            printf ( " \n Track %d ", id ) ;
         }
#endif
      }
   }
}
//****************************************************************************
//   Fill track information tables
//****************************************************************************
void FtfTrack::add ( FtfTrack *piece ) 
{
//
//   Get circle parameters
//
  s11Xy += piece->s11Xy  ;
  s12Xy += piece->s12Xy  ;
  s22Xy += piece->s22Xy  ;
  g1Xy  += piece->g1Xy   ;
  g2Xy  += piece->g2Xy   ;

  ddXy  =   s11Xy * s22Xy - square ( s12Xy ) ;
  a1Xy  = ( g1Xy * s22Xy - g2Xy * s12Xy ) / ddXy ;
  a2Xy  = ( g2Xy * s11Xy - g1Xy * s12Xy ) / ddXy ;
//
//     Now in the sz plane
//
  if ( para->szFitFlag ) {
     double det1 = s11Sz * s22Sz - s12Sz * s12Sz ;
     dtanl = (double) ( s11Sz / det1 );
     dz0   = (double) ( s22Sz / det1 );

     double det2 = piece->s11Sz * piece->s22Sz - piece->s12Sz * piece->s12Sz ;
     piece->dtanl = (double) ( piece->s11Sz / det2 );
     piece->dz0   = (double) ( piece->s22Sz / det2 );
      
     double weight1 = 1./(dtanl*dtanl);
     double weight2 = 1./(piece->dtanl*piece->dtanl);
     double weight  = (weight1+weight2);
     tanl = ( weight1 * tanl + weight2 * piece->tanl ) / weight ; 

     weight1 = 1./(dz0*dz0);
     weight2 = 1./(piece->dz0*piece->dz0);
     weight  = (weight1+weight2);
     z0   = ( weight1 * z0 + weight2 * piece->z0 ) / weight ; 
   }
//
//  Add space points to first track
//
    int counter ;
    if ( piece->outerMostRow < outerMostRow ){
      if ( lastHit != NULL ) {
         counter = 0 ;
	 lastHit->nextTrackHit = piece->firstHit ;
	 lastHit         = piece->lastHit ;
	 for ( currentHit   = (FtfHit *)piece->firstHit ; 
	       currentHit != 0 && counter < piece->nHits ;
	       currentHit  = (FtfHit *)currentHit->nextTrackHit  ) {
	    currentHit->track = this   ;
	    counter++ ;
	 }
      }
      innerMostRow = piece->innerMostRow ;
      xLastHit     = piece->xLastHit ;
      yLastHit     = piece->yLastHit ;
    }
    else {
       if ( piece->lastHit != NULL ) {
	  counter = 0 ;
	  for ( currentHit   = (FtfHit *)piece->firstHit ; 
		currentHit != 0 && counter < piece->nHits ;
		currentHit  = (FtfHit *)currentHit->nextTrackHit  ) {
	     currentHit->track = this   ;
	     counter++;
	  }
	  piece->lastHit->nextTrackHit = firstHit ;
	  firstHit               = piece->firstHit ;
       }
       outerMostRow = piece->outerMostRow ;
    }
//
//
//
   nHits  += piece->nHits ;
   chi2[0] += piece->chi2[0] ;
   chi2[1] += piece->chi2[1] ;
//
//   Update track parameters
//
   para->szFitFlag = 0 ;
   if ( para->fillTracks ) fill ( ) ;
   para->szFitFlag = 1 ;
//
//   Declare track 2 not to be used
//
   piece->flag    = -1 ;
}

//****************************************************************************
//   Control how the track gets built
//****************************************************************************
int FtfTrack::buildTrack ( FtfHit *frstHit, FtfContainer *volume ) {
//
//   Add first hit to track
//
   add ( frstHit, GO_DOWN ) ;
//
//    Try to build a segment first
//
   if ( !segment ( volume, GO_DOWN ) ) return 0 ;
//
//    If segment build go for a real track with a fit
//
   int rowToStop = para->rowInnerMost ;
   if ( !follow ( volume, GO_DOWN, rowToStop ) ) return 0 ;
//
//    Now to extent track the other direction if requested
//
   if ( para->goBackwards ) follow ( volume, GO_UP, para->rowOuterMost ) ;
//
//  Fill tracks
//
    if ( para->fillTracks ) fill ( ) ;
#ifdef TRDEBUG
   debugFill ( ) ;
#endif

   return 1 ;
}
//***************************************************************************
//   Calculates dEdx
//***************************************************************************
void FtfTrack::dEdx (  ){
   int i, j ;
   FtfHit *nextHit ;
   int nTruncate = max(1,
	           para->dEdxNTruncate*nHits/100) ;
   nTruncate = min(nHits/2,nTruncate) ;
//
//   Define array to keep largest de's
//
   double *de = new double[nTruncate] ;
//
//    Reset
//
   dedx = 0.F ;
   memset ( de, 0, nTruncate*sizeof(double) ) ;
//
//
//
   for  ( nextHit = (FtfHit *)firstHit ; 
          nextHit != 0 ;
          nextHit = (FtfHit *)nextHit->nextTrackHit) { 
    
      dedx += nextHit->q ;
	 
      if ( nextHit->q < de[0] ) continue ;

      for ( i = nTruncate-1 ; i>=0 ; i-- ){
         if ( nextHit->q > de[i] ){
            for ( j=0 ; j<i ; j++ ) de[j] = de[j+1] ;
            de[i] = nextHit->q ;
            break ;
	 }
      }
   }
//
//    Subtract largest de
//
   for ( i=0 ; i<nTruncate ; i++ ) dedx -= de[i] ;
   dedx = dedx / trackLength ;
/*   End track in required volume condition */
      
}
//*********************************************************************** 
//   Delete track candidate 
//***********************************************************************
void FtfTrack::deleteCandidate(void)
{
  FtfHit *curentHit = (FtfHit *)firstHit ;
  FtfHit *nextHit ;
#ifdef TRDEBUG
  debugDeleteCandidate ( ) ;
#endif
  while ( curentHit != 0 )
  {
    nextHit            = (FtfHit *)curentHit->nextTrackHit;
    curentHit->nextTrackHit     =  0 ;
    curentHit->xyChi2   =
    curentHit->szChi2   =  
    curentHit->s        =  0.F ;

    curentHit->setStatus ( 0 ) ;
    curentHit = nextHit;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    Fills track variables with or without fit
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FtfTrack::fill (  ) {
//
//   Get circle parameters
//
   double xc, yc ;
   double rc   = sqrt ( a2Xy * a2Xy + 1 ) / ( 2 * fabs(a1Xy) ) ;
   pt          = (double)(2.9979e-3 * para->bField * rc );
//
   if ( pt > para->ptMinHelixFit ) {
      double combinedChi2 = 0.5*(chi2[0]+chi2[1])/nHits ;
      if ( para->primaries && combinedChi2 < para->maxChi2Primary ) para->vertexConstrainedFit = 1 ;
      else para->vertexConstrainedFit = 0 ;
      fitHelix ( ) ;
   }
   else{
      if ( para->primaries ) fillPrimary ( xc, yc, rc ) ;
      else
         fillSecondary ( xc, yc ) ;
//
//    Get Errors
//
      if ( para->getErrors ) {
         getErrorsCircleFit (  (double)xc, (double)yc, (double)rc ) ;
         double det = s11Sz * s22Sz - s12Sz * s12Sz ;
         dtanl = (double) ( s11Sz / det );
         dz0   = (double) ( s22Sz / det );
      }
   }
}
//****************************************************************************     
//     Fill track information variables
//****************************************************************************
void FtfTrack::fillPrimary (  double &xc, double &yc, double &rc ) {
//
//   Get circle parameters
//
   double xcp = - a2Xy / ( 2. * a1Xy ) ;
   double ycp = - 1.   /  ( 2. * a1Xy ) ;

   xc = xcp + para->xVertex ;
   yc = ycp + para->yVertex ;
//
//   Get track parameters
//
   double angle_vertex  = atan2 ( -ycp, -xcp ) ;
   if ( angle_vertex < 0. ) angle_vertex = angle_vertex + twoPi ;

   double dx_last    = xLastHit - xc ;
   double dy_last    = yLastHit - yc ;
   double angle_last = atan2 ( dy_last, dx_last ) ;
   if ( angle_last < 0. ) angle_last = angle_last + twoPi ;
//
//       Get the rotation
//
   double d_angle = angle_last - angle_vertex ;
// double d_angle = angle_vertex - angle_last ;

   if ( d_angle >  pi ) d_angle =   d_angle - twoPi  ;
   if ( d_angle < -pi ) d_angle =   d_angle + twoPi  ;

   q = ( ( d_angle < 0 ) ? 1 : -1 ) ;
   r0   = para->rVertex ;
   phi0 = para->phiVertex ;
   psi  = (double)(angle_vertex - q * 0.5F * pi) ;
   if ( psi < 0     )  psi = (double)(psi + twoPi );
   if ( psi > twoPi )  psi = (double)(psi - twoPi );
//
//      Get z parameters if needed       
//
   if ( para->szFitFlag == 1 ){
      tanl = -(double)a2Sz ;
      z0   =  (double)(a1Sz + a2Sz * ( trackLength - rc * d_angle * q ) );
   }
   else if ( para->szFitFlag == 2 ) {
      tanl = firstHit->z /
          (double)sqrt ( firstHit->x*firstHit->x + firstHit->y*firstHit->y ) ;
      z0      = 0.F ;
   }
//
//    Store some more track info
//
   eta     = seta(1.,tanl )   ;
//
//   Set primary track
//
   flag = 1 ;

}
//****************************************************************************
//   
//   Fill track information tables
//
//****************************************************************************
void FtfTrack::fillSecondary ( double &xc, double &yc )
{
   xc = - a2Xy / ( 2. * a1Xy ) + xRefHit ;
   yc = - 1.   /  ( 2. * a1Xy ) + yRefHit ;
/*--------------------------------------------------------------------------
     Get angles for initial and final points
------------------------------------------------------------------------------*/
   double dx1    = firstHit->x - xc ;
   double dy1    = firstHit->y - yc ;
   double angle1 = atan2 ( dy1, dx1 ) ;
   if ( angle1 < 0. ) angle1 = angle1 + twoPi ;

   double dx2    = lastHit->x - xc ;
   double dy2    = lastHit->y - yc ;
   double angle2 = atan2 ( dy2, dx2 ) ;
   if ( angle2 < 0. ) angle2 = angle2 + twoPi ;
/*--------------------------------------------------------------------------
     Get the rotation
------------------------------------------------------------------------------*/
   double dangle = angle2 - angle1 ;
 //  if ( dangle >  pi ) dangle =   dangle - twoPi  ;
   if ( dangle < -pi ) dangle =   dangle + twoPi  ;

   q    = ( ( dangle > 0 ) ? 1 : -1 ) ;
   r0   = ((FtfHit *)lastHit)->r   ;
   phi0 = ((FtfHit *)lastHit)->phi ;
   psi  = (double)(angle2 - q * piHalf );
   if ( psi < 0     ) psi = (double)(psi + twoPi );
//
//      Get z parameters if needed       
//
   if ( para->szFitFlag ){
      tanl = -(double)a2Sz ;
      z0   =  (double)(a1Sz + a2Sz * trackLength  );
   }
   else{
      tanl = firstHit->z /
           (double)sqrt ( firstHit->x*firstHit->x + firstHit->y*firstHit->y ) ;
      z0      = 0.F ;
   }
//
//-->    Store some more track info
//
   eta     = seta(1., tanl )   ;
//
//    Set primary track flag
//
   flag = 0 ;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//        Adds hits to a track chosing the closest to fit
// Arguments:
//              volume:	      volume pointer
//              way   :       which way to procede in r (negative or positive)
//              row_to_stop:  row index where to stop
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int FtfTrack::follow ( FtfContainer *volume, int way, int ir_stop ) {

   FtfHit *nextHit ;
   
   if ( way < 0 )
      nextHit = (FtfHit *)lastHit ;
   else
      nextHit = (FtfHit *)firstHit ; 
#ifdef TRDEBUG
        if ( para->trackDebug && para->debugLevel >= 2 )
        printf ( "FtfTrack::follow: ===> Going into Track extension <===\n" );
#endif
//
//     Define variables to keep total chi2
//
   double xyChi2 = chi2[0] ;
   double szChi2 = chi2[1] ;
   
//
//    Loop as long a a hit is found and the segment
//    is shorter than n_hit_segm
//
   while ( way * nextHit->row < way * ir_stop ) {
//
//      Select next hit
//
      chi2[0] = para->hitChi2Cut ;

      nextHit = seekNextHit ( volume, nextHit, way*para->trackRowSearchRange, USE_FOLLOW ) ;

#ifdef TRDEBUG
      if ( para->trackDebug && para->debugLevel >= 1 ){
         if ( nextHit != 0 ){
            printf ( "FtfTrack::follow: Search succesful, hit selected %d\n", 
                      nextHit->id );
//		    nextHit->Show ( para->color_track ) ;
         }
         else{
            printf ( "FtfTrack::follow: Search unsuccesful\n" );
            if ( chi2[0]+chi2[1] > para->hitChi2Cut )
               printf ( " hit chi2 %f larger than cut %f ", chi2[0]+chi2[1], 
                                                            para->hitChi2Cut ) ;
         }
         debugAsk () ;
      }
#endif
//
//    Stop if nothing found
//
      if ( nextHit == 0 ) break ;
//
//   Keep total chi2
//
      double lxyChi2 = chi2[0]-chi2[1] ;
      xyChi2 += lxyChi2 ;
      nextHit->xyChi2 = lxyChi2 ;
//
//   if sz fit update track length
//
      if ( para->szFitFlag  ) {
         trackLength = nextHit->s ;
         szChi2 += chi2[1]  ;
         nextHit->szChi2 = chi2[1] ;
      }
//
//     Add hit to track
//
      add ( nextHit, way ) ;

   } // End while
//
//    Check # hits
//
   if ( nHits < para->minHitsPerTrack ) return 0 ; 
//
//   Store track chi2
//
   chi2[0] = xyChi2 ;
   chi2[1] = szChi2 ;
//
//        Check total chi2
//
   double normalized_chi2 = (chi2[0]+chi2[1])/nHits ;
   if ( normalized_chi2 > para->trackChi2Cut ) return 0 ;
//
   return 1 ;
}
/*******************************************************************************
        Reconstructs tracks
*********************************************************************************/
int FtfTrack::followHitSelection ( FtfHit *baseHit, FtfHit *candidateHit ){
//
   double lszChi2 = 0 ;
   double lchi2 ;
   double slocal, deta, dphi ;
   double dx, dy, dxy, dsz, temp ;
//
//           Check delta eta 
//
//   if ( baseHit->dz < 1000. && candidateHit->dz < 1000 ){
      deta = fabs((baseHit->eta)-(candidateHit->eta)) ;
      if ( deta > para->deta ) return 0 ; 
//   }
//   else deta = 0.F ;
//
//           Check delta phi
//
  dphi = fabs((baseHit->phi)-(candidateHit->phi)) ;
  if ( dphi > para->dphi && dphi < twoPi-para->dphi ) return 0 ;
//
//      If looking for secondaries calculate conformal coordinates
//
   if ( para->primaries == 0 ){
      double xx = candidateHit->x - xRefHit ;
      double yy = candidateHit->y - yRefHit ;
      double rr = xx * xx + yy * yy ;
      candidateHit->xp =   xx / rr ;
      candidateHit->yp = - yy / rr ;

      candidateHit->wxy  = rr * rr /
                        ( square(para->xyErrorScale)  *
                        ( square(candidateHit->dx) + square(candidateHit->dy) ) ) ;
   }
//
//      Calculate distance in x and y
//
   temp = (a2Xy * candidateHit->xp - candidateHit->yp + a1Xy) ;
   dxy  = temp * temp / ( a2Xy * a2Xy + 1.F ) ;
//
//    Calculate chi2
//
   lchi2    = (dxy * candidateHit->wxy) ;

   if ( lchi2 > chi2[0] ) return 0 ;
//
//      Now in the sz plane
//
   if ( para->szFitFlag ){
//
//        Get "s" and calculate distance hit-line
//
      dx     = baseHit->x - candidateHit->x ;
      dy     = baseHit->y - candidateHit->y ;
      slocal = trackLength + sqrt ( dx * dx + dy * dy ) ;

      temp = (a2Sz * slocal - candidateHit->z + a1Sz) ;
      dsz  = temp * temp / ( a2Sz * a2Sz + 1 ) ;
//
//              Calculate chi2
//
      lszChi2 = dsz * candidateHit->wz ;
      lchi2 += lszChi2 ;
   } 
   else {
      lszChi2 = 0.F ;
   }
//
//         Check whether the chi2 square is better than previous one
//
   if ( lchi2 < chi2[0] ) {
      chi2[0]       = (double)lchi2    ;
      chi2[1]       = (double)lszChi2 ;
      
      if ( para->szFitFlag  ) candidateHit->s = (double)slocal ;
//
//       if a good chi2 is found let's stop here
//
      if ( lchi2 < para->goodHitChi2 ) return 2 ;

      return 1 ;
   }
//
//     Return the selected hit
//
   return 0 ;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    Merges tracks
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int FtfTrack::mergePrimary ( FtfContainer *trackArea ){
   short  track_merged ;
   register int  areaIndex ;
   int    i_phi, i_eta ;
   FtfTrack *i_track ;
   int    ip, ie ;
   double  delta_psi ;
//
//   Check Track is primary
//
   if ( flag != 1 ) return 0 ;
//-
//   Get track area       
//
   i_phi = (int)(( psi - para->phiMinTrack ) / para->phiSliceTrack + 1 );
   if ( i_phi < 0 ) {
       printf ( " Track phi index too low  %d \n", i_phi ) ;
       i_phi = 1 ;
   }
   if ( i_phi >= para->nPhiTrackPlusOne ) {
       printf ( " Track phi index too high %d \n", i_phi ) ;
       i_phi = para->nPhiTrack ;
   }
//
//     Now eta
//
   i_eta = (int)(( eta - para->etaMinTrack ) / para->etaSliceTrack + 1 );
   if ( i_eta <= 0 ) {
       printf ( " Track eta index too low  %d \n", i_eta ) ;
       i_eta = 1 ;
   }
   if ( i_eta >= para->nEtaTrackPlusOne ) {
       printf ( " Track eta index too high %d \n", i_eta ) ;
       i_eta = para->nEtaTrack ;
   }
//
//     Loop around selected area
//
   track_merged = 0 ;
   for ( ip = max(i_phi-1,1) ; ip < min(i_phi+2,para->nPhiTrackPlusOne) ; ip++ ) {
      for ( ie = max(i_eta-1,1) ; ie < min(i_eta+2,para->nEtaTrackPlusOne) ; ie++ ) {
         areaIndex = ip * para->nEtaTrackPlusOne + ie ;
//
//    Loop over tracks
//
         for ( i_track = (FtfTrack *)trackArea[areaIndex].first ; 
               i_track != 0 ;
               i_track = i_track->nxatrk  ) {
//
//    Reject track if it is not good
//
         if ( i_track->flag < 0 ) continue ; 
//
// Compare both tracks
//
//   No overlapping tracks
//			short delta1 = i_track->firstHit->row - firstHit->row ;
			short delta1 = i_track->outerMostRow - outerMostRow ;
//			short delta2 = i_track->lastHit->row - lastHit->row ;
			short delta2 = i_track->innerMostRow - innerMostRow ;
			if ( delta1 * delta2 <= 0 ) continue ;
//
//    Tracks close enough
//
            if ( fabs(eta-i_track->eta) > para->detaMerge ) continue ;
            delta_psi = (double)fabs(psi - i_track->psi) ;
            if ( delta_psi > para->dphiMerge && delta_psi < twoPi - para->dphiMerge ) continue ;

            i_track->add ( this ) ;
#ifdef TRDEBUG
			if ( para->debugLevel > 1 )
                  printf ( " \n Track %d merge into %d ", this->id, i_track->id ) ;
#endif
            track_merged = 1 ;
            break ;
         }
      }
   }
//
//->  If track not matched add it
//
   if ( track_merged == 0 ) {
      areaIndex = i_phi * para->nEtaTrackPlusOne + i_eta ;
      /*
      if ( trackArea[areaIndex].firstTrack == 0 )
         trackArea[areaIndex].firstTrack = 
         trackArea[areaIndex].lastTrack = this  ;
      else {
         trackArea[areaIndex].lastTrack->nxatrk = this ; 
         trackArea[areaIndex].lastTrack = this ;
      }
      */
      if ( trackArea[areaIndex].first == 0 )
         trackArea[areaIndex].first = 
         trackArea[areaIndex].last = (void *)this  ;
      else {
         ((FtfTrack *)trackArea[areaIndex].last)->nxatrk = this ; 
	 trackArea[areaIndex].last = (void *)this ;
      }
   }
   return track_merged ;
}
/************************************************************************* 
	Recontruct primary tracks 
*************************************************************************/
void FtfTrack::reset (void)
{
/*----------------------------------------------------------------------
                Set fit parameters to zero
----------------------------------------------------------------------*/

  flag     = para->primaries ;
  nHits    = 0 ;
  s11Xy   = 
  s12Xy   = 
  s22Xy   = 
  g1Xy    = 
  g2Xy    = 
  chi2[0]  = 0.F ;
  nxatrk   = 0 ;
  if ( para->szFitFlag ) 
  {
     s11Sz =
     s12Sz =
     s22Sz =
     g1Sz  =
     g2Sz  =
     chi2[1]  = 
     trackLength         = 0.F ;
  }
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     Function to look for next hit
// Input:	volume:         Volume pointer
//          baseHit:       Last point in track
//          n_r_steps:      How many rows search and which way (up or down)
//		    which_function: Function to be used to decide whether the hit is good
// Returns:	Selected hit
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
FtfHit *FtfTrack::seekNextHit ( FtfContainer  *volume, 
			        FtfHit *baseHit,
				int     n_r_steps,
				int which_function ) {
#define N_LOOP 9 
   int loop_eta[N_LOOP] = { 0, 0, 0,-1,-1,-1, 1, 1, 1 } ;
   int loop_phi[N_LOOP] = { 0,-1, 1, 0,-1, 1, 0,-1, 1 };


   int ir, irp, ipp, itp, k;
   register int areaIndex ; 
   int result ;
   
//-------------------------------------------------------------------------------
//     Calculate limits on the volume loop
//-----------------------------------------------------------------------------*/
   int initialRow, way ;
   if ( n_r_steps < 0 ) {
      initialRow = max(1, (baseHit->row - para->rowInnerMost)/para->modRow);
      n_r_steps  = min(initialRow,-n_r_steps ) ;
      way        = -1 ;
   }
   else {
      initialRow = max(1, (baseHit->row - para->rowInnerMost + 2)/para->modRow);
      n_r_steps  = min((para->rowOuterMost-initialRow+1),n_r_steps) ;
      way = 1 ;
   }

   
   FtfHit *selected_hit  = 0 ;
//
//      Loop over modules
//
   for ( ir = 0 ; ir < n_r_steps ; ir++ ){
      irp = initialRow + way * ir ;
      for ( k=0; k< N_LOOP; k++){ 
         ipp = baseHit->phiIndex + loop_phi[k];
//
//--   Gymnastics if phi is closed
//
         if ( ipp < 1 ) {
            if ( para->phiClosed )
               ipp = para->nPhi + ipp ;
            else
               continue ;
         }
         else if ( ipp > para->nPhi ) {
            if ( para->phiClosed )
               ipp = ipp - para->nPhi ;
            else
               continue ;
         }
//
//     Now get eta index
//
         itp = baseHit->etaIndex + loop_eta[k];
         if ( itp <     1      ) continue ;
         if ( itp > para->nEta ) continue ;
//
#ifdef TRDEBUG
         if ( para->trackDebug && para->debugLevel >= 4 ) 
            printf ( "FtfTrack::seekNextHit: search in row %d \n",irp ) ;
#endif
//
//       Now loop over hits in each volume 
//
         areaIndex = irp   * para->nPhiEtaPlusOne + ipp * para->nEtaPlusOne + itp ;
         for ( FtfHit *candidateHit = (FtfHit *)volume[areaIndex].first ; 
             candidateHit != 0 ;
             candidateHit = candidateHit->nextVolumeHit ){
#ifdef TRDEBUG
             debugInVolume ( baseHit, candidateHit ) ;
#endif
//----------------------------------------------------------------------------
//         Check whether the hit was used before
//--------------------------------------------------------------------------*/
             if ( candidateHit->track != 0 ) continue ;
//--------------------------------------------------------------------------
//         If first points, just choose the closest hit
//-------------------------------------------------------------------------- */
             if ( which_function == USE_SEGMENT ) 
	        result = segmentHitSelection ( baseHit, candidateHit ) ;
	     else 
                result = followHitSelection  ( baseHit, candidateHit ) ;
//
//     Check result
//
             if ( result > 0 ) {
	        selected_hit = candidateHit ;
                if ( result ==2  ) goto found ; 
             }
//
//       End hit loop  
//
         }
//
//     End row loop      
//
      }
//
//   End volume loop inside cone      
//
   }
found: ;

   return selected_hit ;
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//   Forms segments
//   Arguments:
//             volume     :    volume pointer
//             way        :    whether to go to negative or positive ir
//             row_to_stop:    row index where to stop
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int FtfTrack::segment( FtfContainer *volume, int way ){
//
//   Define some variables
//
	double dx, dy, rr ;
	FtfHit* nextHit ;
//
//   Check which way to go
//
	if ( way < 0 ) 
	   nextHit = (FtfHit *)lastHit ;
	else
	   nextHit = (FtfHit *)firstHit ;
#ifdef TRDEBUG
   if ( para->trackDebug && para->debugLevel >= 4 )
        printf ( "FtfTrack:segment: **** Trying to form segment ****\n" );
#endif
//
//    Loop as long a a hit is found and the segment
//    is shorter than n_hit_segm
//
   while ( nextHit != 0 && nHits < para->nHitsForSegment ) {
      chi2[0] = para->maxDistanceSegment ; ;
      nextHit = seekNextHit ( volume, nextHit, way*para->segmentRowSearchRange, 
                              USE_SEGMENT ) ;
#ifdef TRDEBUG
      if ( para->trackDebug && para->debugLevel > 0 ) {
         if ( nextHit != 0 ) {
            printf ( "FtfTrack::segment: Search succesful, hit %d selected\n",
                      nextHit->id );
//       nextHit->Show ( para->color_track ) ;
         }
         else
            printf ( "FtfTrack::segment: Search unsuccesful\n" );
         debugAsk () ;
      }
#endif
//
//     If sz fit update s
//
      if ( nextHit != 0 ){
//
//   Calculate track length if sz plane considered
//
         if ( para->szFitFlag  ){
            dx = nextHit->x - lastHit->x ;
            dy = nextHit->y - lastHit->y ;
            trackLength    += (double)sqrt ( dx * dx + dy * dy ) ;
            nextHit->s      = trackLength ;
         }
//
//   Calculate conformal coordinates
//
         if ( para->primaries == 0 ){
            rr = square ( xRefHit - nextHit->x ) +
                 square ( yRefHit - nextHit->y ) ;


            nextHit->xp    =   ( nextHit->x - xRefHit ) / rr ;
            nextHit->yp    = - ( nextHit->y - yRefHit ) / rr ;
            nextHit->wxy   = rr * rr / ( square(para->xyErrorScale)  *
                                         square(nextHit->dx) + square(nextHit->dy) ) ;
         }
//
//     Add hit to track
//
	 add ( nextHit, way ) ;
      }
   } // End while ( lastHit ...
//
//    If number of hits is as expected return 1 
//
   if ( nHits == para->nHitsForSegment )
      return 1 ;
   else
      return 0 ;
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//     Routine to look for segments.
//	 Arguments:
//	 baseHit:       Hit from which track is being extrapolated
//   candidateHit:  Hit being examined as a candidate to which extend track
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
int FtfTrack::segmentHitSelection ( FtfHit *baseHit, FtfHit *candidateHit ){
   
   double dx, dy, dr, d3, dangle ;
   double dphi, deta ;
   double   angle ;
//
//   select hit with the
//   the smallest value of d3 (defined below)
//
   dphi  = (double)fabs((baseHit->phi) - (candidateHit->phi)) ; 
   if ( dphi > pi ) dphi = (double)fabs( twoPi - dphi ) ;
   if ( dphi > para->dphi && dphi < twoPi -para->dphi ) return 0 ;
//
//    Make sure we want to look at the difference in eta
//
   if ( baseHit->dz < 1000. && candidateHit->dz < 1000. ){
        deta  = (double)fabs((baseHit->eta) - (candidateHit->eta)) ; 
        if ( deta > para->deta ) return 0 ;
   }
   else deta = 0.F ;

   dr    = (double)fabs((double)(baseHit->row - candidateHit->row));
   d3    = (double)(toDeg * dr * ( dphi  + deta ) ) ;
//
//     If initial segment is longer than 2 store angle info in 
//     a1Xy and a1_sz
//
   if ( para->nHitsForSegment > 2 && nHits-1 < para->nHitsForSegment ) {
      dx = candidateHit->x - baseHit->x ;
      dy = candidateHit->y - baseHit->y ;
      angle = (double)atan2 ( dy, dx ) ;
      if ( angle < 0  ) angle = angle + twoPi ;
      lastXyAngle = angle ;
   }
#ifdef TRDEBUG
   if ( para->trackDebug && para->debugLevel >= 3 ) {
      printf ( "FtfTrack::segmentHitSelection:\n");
      printf ( "dr,dphi,deta,distance, Min distance  %7.2f %7.2f %7.2f %7.2f %7.2f\n",
                dr,dphi,deta,d3,chi2[0] ) ;
      if ( d3 < chi2[0] )
         printf ( "Best point, keep it !!!\n" );  
      else{
         printf ( "Worse than previous, reject !!\n" );
//       candidateHit->Show ( para->color_transparent );
      }
      debugAsk() ;
   }
#endif
   if ( d3 < chi2[0] ) {
//
//   For second hit onwards check the difference in angle 
//   between the last two track segments
//
      if ( nHits > 1 ) {
	 dx     = candidateHit->x - baseHit->x ;
         dy     = candidateHit->y - baseHit->y ;
         angle  = (double)atan2 ( dy, dx ) ;
         if ( angle < 0  ) angle = angle + twoPi ;
	    dangle = (double)fabs ( lastXyAngle - angle );
	    lastXyAngle = angle ;
         if ( dangle > para->segmentMaxAngle ) return 0 ;
      }
//
//    Check whether this is the "closest" hit
//
      chi2[0]          = d3 ;
      if ( d3 < para->goodDistance ) return 2 ;
	  return 1 ;
   }
//
//    If hit does not fulfill criterai return 0
//
   return 0 ;
}
#ifdef TRDEBUG
//*****************************************************************************  
//    Ask for a character to keep going
//******************************************************************************/
void FtfTrack::debugAsk (void) 
{
    char cc;
	  
    printf ( "stop(s), continue (any other key)\n" );
    cc = getchar();
    if ( cc == 's' ) para->trackDebug = 0 ;
    if ( cc == '1' ) para->debugLevel = 1 ;
    if ( cc == '2' ) para->debugLevel = 2 ;
    if ( cc == '3' ) para->debugLevel = 3 ;
    if ( cc == '4' ) para->debugLevel = 4 ;
    if ( cc == '5' ) para->debugLevel = 5 ;
    if ( cc == '6' ) para->debugLevel = 6 ;
    if ( cc == '7' ) para->debugLevel = 7 ;

    printf ( "FtfTrack::debugAsk: Debug Level %d\n", para->debugLevel ) ;
	 
}
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//    Debug Delete Candidate
//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void FtfTrack::debugDeleteCandidate(void)
{
  if ( para->trackDebug == 0 || para->debugLevel < 1 ) return ;
  
//  for ( startLoop() ; done() ; nextHit() ) {
//  currentHit->Show ( para->color_back ) ;
//  }
  printf ( "FtfTrack::debugDeleteCandidate: Track %d has %d hits <==\n"
                           ,id, nHits );
  printf ( "FtfTrack::debugDeleteCandidate: Minimum is %d, delete it \n",
            para->minHitsPerTrack  );
//  print ( 31 ) ;
  debugAsk () ;
}
/*****************************************************************************  
     Fill track information tables
******************************************************************************/
void FtfTrack::debugFill (  )
{
   if ( para->trackDebug && para->debugLevel >= 1 ) {
      printf ( "\n ===> Track %d added  <=== ",id+1 );
      Print ( 31 ) ;
   }
}
/*****************************************************************************
        Reconstructs tracks
******************************************************************************/
void FtfTrack::debugFollowCandidate ( FtfHit* candidateHit )
{
  if ( !para->trackDebug || para->debugLevel >= 4 ) return ;
//
//    Show the whole track and fit
//
//for ( startLoop() ; done() ; nextHit() ) {
//  currentHit->Show ( para->color_track ) ;
//}
//  candidateHit->Show ( para->color_candidate );
//
//        Print relevant information
//
  printf ( "FtfTrack::debugFollowCandidate ===> Extension in Follow <===\n" ) ;
 // print ( 31 ) ;

  printf ( "FtfTrack::debugFollowCandidate: Try hit %d\n", candidateHit->id ) ;
  candidateHit->print ( 11 ) ;
//
//     If the hit already used say it and forget about it
//
  if ( candidateHit->track != 0 )
  {
      printf ( "FtfTrack::debugFollowCandidate: hit %d used in track %d\n",
                    candidateHit->id, id );
      debugAsk () ;
//    candidateHit->Show ( para->color_candidate ) ;
      candidateHit->print ( 3 ) ;
  }
}
/*******************************************************************************
        Reconstructs tracks
*********************************************************************************/
void FtfTrack::debugFollowSuccess ( double dxy,      double dsz, double lxyChi2, 
                                       double  lszChi2, double chi2_min,
                                       FtfHit *candidateHit ) {
//
//     Check whether track needs to be debugged
//
   if ( !para->trackDebug     ) return ;
   if (  para->debugLevel < 2 ) return ;
//
//      Show first level of info
//
   double lchi2 = lxyChi2 + lszChi2 ;
   
   printf ( " \n ------------------------------------- " ) ;
   if ( lchi2 < chi2_min ){
       printf ( "FtfTrack::debugFollowSuccess: %f Best Chi2, keep point !!!\n", 
                    lchi2 );
       if ( lchi2 < para->goodHitChi2 ){
           printf ( "This Chi2 is better than the good cut %f\n",
                           lchi2, para->goodHitChi2 );
           printf ( "Stop search !!! " );
       }
   }
   else{
      printf ( "FtfTrack::debugFollowSuccess: Hit %d worse than previous, forget it !! ",
                candidateHit->id );
//    candidateHit->Show ( para->color_track ) ;
   }
   
   
   printf ( " \n ------------------------------------- " ) ;
//
//   Show second level of info
//
   if ( para->debugLevel > 2 ) {
      printf ( "FtfTrack::debugFollowSuccess:\n");
      printf ( "dis_xy dis_sz   %7.2e %7.2e\n ", dxy, dsz );
      printf ( "Error xy   sz   %7.2e %7.2e\n ",  
                       candidateHit->wxy, candidateHit->wz );
      printf ( "xy:a1,a2;sz:a1,a2  %7.2f %7.2f %7.2f %7.2f\n",
                                       a1Xy, a2Xy, a1Sz, a2Sz );
      printf ( "ch2:xy sz tot min  %7.2f %7.2f %7.2f %7.2f\n", 
                                        lxyChi2,lszChi2, lchi2, chi2_min );
   }
   debugAsk() ;
// candidateHit->Show ( para->color_transparent ) ;
}
/*********************************************************************************
     Routine to look for segments.
     Segments are track starting chains
*******************************************************************************/
void FtfTrack::debugInVolume ( FtfHit *baseHit, FtfHit *candidateHit )
{

   if ( para->trackDebug && para->debugLevel >= 2 ) {
/*----------------------------------------------------------------------------
      Show the whole segment
----------------------------------------------------------------------------*/
//    for ( startLoop() ; done() ; nextHit() ) {
//       currentHit->Show ( para->color_track ) ;
//    }
      
//    candidateHit->Show ( para->color_candidate ) ;
/*----------------------------------------------------------------------------
        Print relevant information
----------------------------------------------------------------------------*/
      if ( nHits > para->nHitsForSegment+1 ) Print ( 31 ) ;

      printf ( "FtfTrack:debugInVolume: Try hit %d\n", candidateHit->id ) ; 
      candidateHit->print ( 11 ) ;
/*----------------------------------------------------------------------------
        If the hit already used say it and forget about it
----------------------------------------------------------------------------*/
      if ( candidateHit->track != 0 ) {
         printf ( "FtfTrack:debugInVolume: hit %d used in track %d\n", 
                              candidateHit->id, id+1 );
//       candidateHit->Show ( 0 );
      }
      else {
         double dphi  = (double)fabs(baseHit->phi - candidateHit->phi) ;
         double deta ;
         if ( baseHit->dz < 1000 && candidateHit->dz < 1000 )
            deta  = (double)fabs(baseHit->eta - candidateHit->eta) ;
         else
            deta  = 0.F ;

         if ( dphi > para->dphi )
            printf ( "FtfTrack:debugInVolume: Hits too far apart in phi: %f \n", 
                      dphi ) ;
         if ( deta > para->deta )
            printf ( "FtfTrack:debugInVolume: Hits too far apart in eta: %f \n", 
                      deta ) ;
      }
      debugAsk () ;
   }
}
/*****************************************************************************
     Fill track information tables
******************************************************************************/
void FtfTrack::debugNew (  )
{

  if ( firstHit->id == para->hitDebug ) para->trackDebug = 1 ;
  if ( para->trackDebug && para->debugLevel >= 1 )
  {
     printf ( "================================================ \n" );
     printf ( "FtfTrack::debugNew:Starting track %d from point %d\n", 
              id, firstHit->id );
     printf ( "================================================ \n" );

//   firstHit->Show ( para->color_track ) ;
     debugAsk () ;
   }
}
#endif
