#ifndef FTFFINDER
#define FTFFINDER
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
//#include <iostream.h>

#include "FtfGeneral.h"
#include "FtfPara.h"
#include "FtfHit.h"
#include "FtfTrack.h"
#include "FtfMcTrack.h"
#include "FtfVolume.h"


class FtfFinder {

public:
   FtfFinder( ) ;
   friend FtfTrack ;
	
<<<<<<< FtfFinder.h
   void    dEdx                    ( ) ;
   void    getTracks               ( ) ;
   void    mergePrimaryTracks      ( ) ;
   double  process ( ) ;
   int     reset                   ( ) ;
   int     setConformalCoordinates ( ) ;
   int     setPointers             ( ) ;
   double  CpuTime                 ( ) ;
   double  RealTime                ( ) ;
=======
   void   dEdx                    ( ) ;
   int    getTracks               ( ) ;
   void   mergePrimaryTracks      ( ) ;
   float  process ( ) ;
   int    reset                   ( ) ;
   int    setConformalCoordinates ( ) ;
   int    setPointers             ( ) ;
   float  time                    ( ) ;
>>>>>>> 1.5
//
   void		printVols ( ) ;
   void		printRows ( ) ;
//
   int           nHits      ;  
   int           maxHits    ;  
   FtfHit        *hit       ;  
   int           nTracks    ; 
   FtfTrack      *track     ;  
   FtfPara       para       ;
   int           maxTracks  ;
   int           nMcTracks  ;
   FtfMcTrack    *mcTrack    ;
   VOLUME        *volume     ;
   ROW           *rowk       ;
   AREA          *trackArea ;
   double        cpuTime ;
   double        realTime ;
private: 

   FtfTrack      *currentTrack ;
    
} ;
#endif

