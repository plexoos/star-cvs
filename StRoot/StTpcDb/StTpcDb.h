/***************************************************************************
 *
 * $Id: StTpcDb.h,v 1.16 2000/05/11 17:17:27 hardtke Exp $
 *
 * Author:  David Hardtke
 ***************************************************************************
 *
 * Description: This is the interface between the database and the offline
 *              TPC software.  This classes takes care of the annoying
 *              calls to the root infrastucture, packages and manipulates
 *              the data, and returns the data to the user via simple
 *              interface classes.    
 *
 ***************************************************************************
 *
 * $Log: StTpcDb.h,v $
 * Revision 1.16  2000/05/11 17:17:27  hardtke
 * make trigger time offset available -- currently NOT different for beam and laser events
 *
 * Revision 1.15  2000/03/27 21:21:22  fine
 * Adjusted to ROOT 2.24
 *
 * Revision 1.14  2000/02/10 00:29:09  hardtke
 * Add tpg functions to StTpcDbMaker, fix a few bugs
 *
 * Revision 1.13  2000/01/25 16:01:10  fisyak
 * Devorce with StAF
 *
 * Revision 1.12  2000/01/11 15:49:52  hardtke
 * get Electronics table from Calibrations database, Fix error messages
 *
 * Revision 1.11  1999/12/16 22:00:53  hardtke
 * add CVS tags
 *
 **************************************************************************/
#ifndef ClassStTpcDb
#define ClassStTpcDb

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StTpcDb                                                              //
//                                                                      //
// This class implements to offline interface to the STAR database      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////


/* #ifndef __CINT__ */
/* #include "StarCallf77.h" */
/* #define numberOfPadsAtRow_ F77_NAME(numberofpadsatrow,NUMBEROFPADSATROW) */
/* #define tpc_row_to_y_ F77_NAME(tpc_row_to_y,TPC_ROW_TO_Y) */
/* #define tpc_pad_to_x_ F77_NAME(tpc_pad_to_x,TPC_PAD_TO_X) */
/* #define tpc_local_to_global_ F77_NAME(tpc_local_to_global,TPC_LOCAL_TO_GLOBAL) */
/* #define tpc_drift_velocity_ F77_NAME(tpc_drift_velocity,TPC_DRIFT_VELOCITY) */
/* #define tpc_time_to_z_ F77_NAME(tpc_time_to_z,TPC_TIME_TO_Z) */
/* extern "C" { */
/* R__EXTERN int type_of_call numberOfPadsAtRow_(int *); */
/* } */
/* extern "C" { */
/* R__EXTERN int type_of_call tpc_row_to_y_(float *,float *); */
/* } */
/* extern "C" { */
/* R__EXTERN int type_of_call tpc_pad_to_x_(float *,float *,float *); */
/* } */
/* extern "C" { */
/* R__EXTERN int type_of_call tpc_global_to_local_(int *,float *,float *); */
/* } */
/* #endif */
#include "StMessMgr.h"
#include "StRTpcPadPlane.h"
#include "StRTpcWirePlane.h"
#include "StRTpcDimensions.h"
#include "StRTpcElectronics.h"
#include "StRTpcGain.h"
#include "StRTpcT0.h"
#include "StRTpcSlowControlSim.h"
#include "St_DataSet.h"
class StMaker;
class St_tpcDriftVelocity;
class St_trgTimeOffset;
//class StTpcCoordinateTransform;

class StTpcDb {
 private:
 enum { kCalibration,kGeometry,kConditions } ;
 StMaker* mk;
 StTpcPadPlaneI*       PadPlane;      //!
 StTpcWirePlaneI*      WirePlane;     //!
 StTpcDimensionsI*     dimensions;    //! 
 StTpcSlowControlSimI* slowControlSim;//! 
 StTpcElectronicsI*    electronics;   //!
 StTpcGainI*           gain[24];      //!
 StTpcT0I*             t0[24];        //! 
 St_DataSet*           tpc[3];        //!
 St_DataSet*           trg[3];        //!
 St_tpcDriftVelocity*  dvel;          //!
 St_trgTimeOffset*     toff;          //!
 // StTpcCoordinateTransform* transform; //!

 protected:
   StTpcDb() {}
   void GetDataBase(StMaker* maker);
 public:
   StTpcDb(St_DataSet* input);
   StTpcDb(StMaker* makerDb);
   virtual ~StTpcDb();
   StTpcPadPlaneI* PadPlaneGeometry();
   StTpcWirePlaneI* WirePlaneGeometry();
   StTpcDimensionsI* Dimensions();
   StTpcSlowControlSimI* SlowControlSim();
   StTpcElectronicsI* Electronics();
   StTpcGainI* Gain(int sector);
   StTpcT0I* T0(int sector);
   St_Table *getTpcTable(int i);
   //small pieces of data:
   float DriftVelocity();
   float triggerTimeOffset();


#ifdef __ROOT__
   ClassDef(StTpcDb,0)
#endif
};

// Global pointers:
R__EXTERN StTpcDb* gStTpcDb;

#endif









