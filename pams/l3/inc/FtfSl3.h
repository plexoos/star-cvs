/*:>-------------------------------------------------------------------
**: FILE:     FtfSl3.h 
**: HISTORY:  may 29, 1999  Frame for sl3 tracking
**:           aug 23, 1999  add setup with number of hits and tracks
**:           sep 28, 1999  cs: new input functions (pointer to
**:                         bank instead of FILE) using
**:                         daqFormats.h
**:           oct 25, 1999  ppy: use sl3CoordianteTransform header
**:           11/24/99      cle: include <L3Formats.h> intead daqFormats.h
**:                         commented out fillUSTracks
**:                         added public sectorNr variable
**:           12/03/99      ppy: sectorGeometry class added
**:                              variable sectorGeo added
**:                              clean extra includes    
**:           12/06/99      ppy: method added to check whether track can be merged
**:           01/26/00      ppy: delete rawToGlobal declaration
**:           feb 10, 2000  ppy: add xyError and zError
**:           mar 21  2000  ppy: add fill Hits
**:           apr  8  2000  ppy: add variables to cut hit based on charge and time
**:           apr 18  2000  ppy: modify readMezzannine to include rb and mz as arguments
**:           apr 19  2000  ppy: dEdx from Christof
**:           apr 19  2000  cs ppy: include FtfDedx added
**:           oct 30  2000  ppy: ClassDef commented out
**:<------------------------------------------------------------------*/
#ifndef FTFSL3
#define FTFSL3

#include "St_l3_Coordinate_Transformer.h"
#include "FtfFinder.h"
#include "FtfDedx.h"
#include "l3GeneralHeaders.h"
#include "daqFormats.h"

#define checkByteOrder(byte_order)    ( (byte_order) == (DAQ_RAW_FORMAT_ORDER) ? (1) : (0) )

#ifdef SL3ROOT
#include "Rtypes.h"
#else
//#define ClassDef(a,b)
#endif

class sectorGeometry {
public:
   double phiMin ;
   double phiMax ;
   double phiShift ;
   double etaMin ;
   double etaMax ;
};

class FtfSl3: public FtfFinder 
{

   
   int sectorNr; 
   short     debugLevel  ;
   double    xyError ;
   double    zError ;
   void*     sectorGeo ;
public:
   int       minTimeBin ;
   int       maxTimeBin ;
   int       minClusterCharge ;
   int       maxClusterCharge ;

//
//     Constructor
//
   FtfSl3 ( St_l3_Coordinate_Transformer* inTrans ) { 
      debugLevel = 0 ;
      xyError    = 0.3 ;
      zError     = 1.0 ;
      minTimeBin = 0 ;
      maxTimeBin = 500 ;
//    minTimeBin =  35 ;
//    maxTimeBin = 300 ;
      minClusterCharge = 80 ;
      maxClusterCharge = 100000 ;
      coordinateTransformer = inTrans ; 
      sectorGeo = new sectorGeometry[NSECTORS];
   };
   ~FtfSl3 ( ) {
        if ( track != 0 ) delete []track ;
        if ( hit   != 0 ) delete []hit   ;
	delete []sectorGeo ;
   };
//
//  Coordinate Transformer classes
//
   St_l3_Coordinate_Transformer* getCoordinateTransformer()
            { return (St_l3_Coordinate_Transformer*)coordinateTransformer ; } ;
//
//  Sector phase space
//
   sectorGeometry* getSectorGeometry ( int n ) {
      if ( n < 1 && n > NSECTORS ) { 
         printf ( "FtfSl3::getSectorGeometry: wrong sector %d, returning sector 1 \n", n ) ;
	 n = 0 ;
      }
      sectorGeometry* sectorG = (sectorGeometry *)sectorGeo ;
      return &(sectorG[n]);
   }
   //
   int   fillTracks      ( int maxBytes, char* buff, unsigned int token ) ;
   int   fillHits        ( unsigned int maxBytes, char* buff, unsigned int token ) ;
   float getXyError      ( ) { return xyError ; } ;
   float getZError       ( ) { return zError  ; } ;
   int   getNrTracks     ( ) ;

   int   canItBeMerged   ( FtfTrack* thisTrack ) ;
   int   dEdx ( ) ;
   int   processSector   ( ) ;
   int   readMezzanine   ( int sector, int readOutBoard,
                           int MezzanninneNr, struct TPCMZCLD_local *mzcld );
   int   readSector      ( struct TPCSECLP *seclp ) ; 
   int   setParameters   ( ) ;
   void  setCoordinateTransformer ( St_l3_Coordinate_Transformer* in )
                                     { coordinateTransformer = (void *)in ; } ;
   void  setDebugLevel   ( int _debugLevel ) { debugLevel = _debugLevel ; } ;
   void  setSector       ( int _sector     ) { sectorNr   = _sector     ; } ;
   void  setXyError      ( float _xyError  ) { xyError    = _xyError    ; } ;
   void  setZError       ( float _zError   ) { zError     = _zError     ; } ;
   int   setup           ( int maxHitsIn=20000, int maxTracksIn=2000 ) ;
   void  Print() const{};
private:
   void*     coordinateTransformer ;

// ClassDef(FtfSl3,1)

};
#endif
