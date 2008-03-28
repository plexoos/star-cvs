/***********************************************************************
 *
 * $Id: StMagUtilities.h,v 1.33 2006/06/27 18:17:47 jhthomas Exp $
 *
 * Author: Jim Thomas   11/1/2000
 *
 ***********************************************************************
 *
 * Description: Utilities for the Magnetic Field
 *
 ***********************************************************************
 *
 * $Log: StMagUtilities.h,v $
 * Revision 1.33  2006/06/27 18:17:47  jhthomas
 * ADD new PredictSpaceCharge() function so that it includes fit errors in the prediction
 * It is now capable of including the SSD and SVT hits in the predictor/corrector loop
 *
 * Revision 1.32  2005/05/24 18:53:20  jhthomas
 * Add 3DGridLeak Distortion Correction and Utilities to support it.
 *
 * Revision 1.31  2005/02/17 02:00:51  jhthomas
 * Make GetSpaceChargeMode() a public member function.
 *
 * Revision 1.30  2005/02/09 23:50:36  jeromel
 * Changes by JHT for SpaceCharge / Leak corrections
 *
 * Revision 1.28  2004/10/20 17:53:11  jhthomas
 * Add StGetSpaceChargeMode() function
 *
 * Revision 1.27  2004/07/01 17:49:07  jhthomas
 * Add Event by Event SpaceCharge form GVB.  Start adding incomplete/unfinished work on Endcaps from JT.
 *
 * Revision 1.26  2004/04/01 22:19:48  jhthomas
 * Add 3D SpaceCharge capabilities
 *
 * Revision 1.25  2004/03/16 20:45:32  jhthomas
 * Add new (faster) BField shape distortion routine.  See comments for chain flags.
 *
 * Revision 1.24  2004/03/01 17:23:36  jhthomas
 * Add function to get shorted ring parameters.
 *
 * Revision 1.23  2004/01/06 20:09:26  jhthomas
 * Add new routine to handle shorted stripe on East end of TPC
 *
 * Revision 1.22  2003/09/02 17:57:51  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 1.21  2003/06/27 18:47:12  jhthomas
 * Add new function called FixSpaceChargeDistortion( ,,,,, )
 *
 * Revision 1.20  2002/09/18 22:21:35  jhthomas
 * Add new option for 1/R**2 space charge density distribution.  Flag = 0x800
 *
 * Revision 1.19  2002/02/22 17:44:19  jhthomas
 * Get CathodeV and GG from DB. Change Defaults.  Change Instantiation argument
 * order. Update D'Oxygen documentation.  Remove 2000/2001 E field switch.
 *
 * Revision 1.18  2002/02/02 01:01:09  jeromel
 * Jim's modif for FC & SpaceCharge corrections.
 *
 * Revision 1.17  2001/10/25 23:00:24  hardtke
 * Use database to get a few parameters in StMagUtilities (including twist)
 *
 * Revision 1.16  2001/10/05 21:27:35  jeromel
 * Small comment addition for historical purposes.
 *
 * Revision 1.15  2001/10/05 20:18:02  dunlop
 * Tweaked enumeration of distortion selection to respect first three bits for
 * year flag
 *
 * Revision 1.14  2001/10/05 03:44:25  jeromel
 * Modifications by Jamie so we can turn on/off every corrections.
 *
 * Revision 1.13  2001/08/01 18:34:40  jhthomas
 * Add temporary mode flag for year 2 running (different cathode potentials)
 *
 * Revision 1.12  2001/06/14 22:12:11  jhthomas
 * Speedup UndoBDistorion by adding table lookups
 *
 * Revision 1.11  2001/06/13 16:24:43  jhthomas
 * Speed up PadRow13 Corrections
 *
 * Revision 1.3  2000/12/15 16:10:45  jhthomas
 * Add PadRow13, Clock, and Twist corrections to UndoDistortion
 *
 * Revision 1.2  2000/11/03 02:41:58  jhthomas
 * Added CVS comment structure to .h and .cxx files
 *
 ***********************************************************************/
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StMagUtilities Class                                                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMagUtilities_H
#define StMagUtilities_H
 
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <Stiostream.h>

#include "TSystem.h"
#include "TROOT.h"        // Stop at this point and put further includes in .cxx file

#define  nZ               57            // Standard STAR B field Map. Number of Z points in table
#define  nR               28            // Number of R points in table
#define  nPhi             37            // Number of Phi points in table
#define  neZ              69            // Standard STAR E field Map. Number of Z points in table
#define  neR              33            // Number of R points in table
#define  nePhi            13            // Number of Phi points in table ( add one for 360 == 0 )

enum   EBField  { kUndefined = 0, kConstant = 1, kMapped = 2, kChain = 3 } ;
enum   Prime    { IsPrimary = 0 , IsGlobal = 1 } ;

// Bit counting starts at 1 for the mode switch (...,3,2,1)

enum   DistortSelect 
{ 
  kBMap              = 0x08,     // Bit 4
  kPadrow13          = 0x10,     // Bit 5
  kTwist             = 0x20,     // Bit 6
  kClock             = 0x40,     // Bit 7
  kMembrane          = 0x80,     // Bit 8
  kEndcap            = 0x100,    // Bit 9
  kIFCShift          = 0x200,    // Bit 10
  kSpaceCharge       = 0x400,    // Bit 11
  kSpaceChargeR2     = 0x800,    // Bit 12
  kShortedRing       = 0x1000,   // Bit 13
  kFast2DBMap        = 0x2000,   // Bit 14
  kGridLeak          = 0x4000,   // Bit 15
  k3DGridLeak        = 0x8000    // Bit 16
} ;

// DO NOT change the numbering of these constants. StBFChain depends
// on these values to build an option flag. The option flag used in 
// the chain is 2x larger than shown here in order to allow the first 
// bit to be used as an on/off flag.  It is shifted away before entering 
// StMagUtilities.  So, this can be summarized by saying: 
// Bit counting starts at 0 for the chain option flag (...,3,2,1,0) 

class StTpcDb ;
class TDataSet ;
class StDetectorDbSpaceCharge ;
class StDetectorDbTpcVoltages ;
class StDetectorDbTpcOmegaTau ;
class StDetectorDbGridLeak    ;

class TMatrix ;

class StMagUtilities {


 private:
  
  StTpcDb*  thedb ;  
  TDataSet* thedb2 ;
  StDetectorDbSpaceCharge* fSpaceCharge   ;
  StDetectorDbSpaceCharge* fSpaceChargeR2 ;  
  StDetectorDbTpcVoltages* fTpcVolts      ;
  StDetectorDbTpcOmegaTau* fOmegaTau      ;
  StDetectorDbGridLeak*    fGridLeak      ;

  virtual void    SetDb( StTpcDb* dbin , TDataSet* dbin2 ) ;
  virtual void    GetMagFactor ()     ;
  virtual void    GetTPCParams ()     ;
  virtual void    GetTPCVoltages ()   ;
  virtual void    GetSpaceCharge ()   ;
  virtual void    GetSpaceChargeR2 () ;  
  virtual void    GetShortedRing ()   ;  
  virtual void    GetOmegaTau ()      ;
  virtual void    GetGridLeak()       ;

  virtual void    CommonStart ( Int_t mode ) ;
  virtual void    ReadField ( ) ;
  virtual void    Search ( const Int_t N, const Float_t Xarray[], const Float_t x, Int_t &low ) ;
  virtual Int_t   IsPowerOfTwo (Int_t i) ;
  virtual Float_t Interpolate ( const Float_t Xarray[], const Float_t Yarray[], 
				const Int_t ORDER, const Float_t x ) ;
  virtual Float_t Interpolate2DTable  ( const Float_t x, const Float_t y, const Int_t nx, const Int_t ny, 
 				        const Float_t XV[], const Float_t YV[], const TMatrix &Array ) ;
  virtual Float_t Interpolate3DTable ( const Float_t x,    const Float_t y,    const Float_t z,
				       const Int_t  nx,    const Int_t  ny,    const Int_t  nz,
				       const Float_t XV[], const Float_t YV[], const Float_t ZV[],
				       TMatrix **ArrayofArrays ) ;
  virtual void    Interpolate2DBfield ( const Float_t r, const Float_t z, 
					Float_t &Br_value, Float_t &Bz_value ) ;
  virtual void    Interpolate3DBfield ( const Float_t r, const Float_t z, const Float_t phi, 
					Float_t &Br_value, Float_t &Bz_value, Float_t &Bphi_value ) ;
  virtual void    Interpolate2DEdistortion ( const Float_t r, const Float_t z, 
					     const Float_t Er[neZ][neR], Float_t &Er_value ) ;
  virtual void    Interpolate3DEdistortion ( const Float_t r, const Float_t phi, const Float_t z, 
					     const Float_t Er[neZ][nePhi][neR], const Float_t Ephi[neZ][nePhi][neR], 
					     Float_t &Er_value, Float_t &Ephi_value ) ;
  virtual void    PoissonRelaxation  ( TMatrix &ArrayV, const TMatrix &Charge, TMatrix &EroverEz, 
                                       const Int_t ROWS, const Int_t COLUMNS, const Int_t ITERATIONS ) ;

  virtual void    Poisson3DRelaxation( TMatrix **ArrayofArrayV, TMatrix **ArrayofCharge, TMatrix **ArrayofEroverEz, 
				       TMatrix **ArrayofEPhioverEz,
				       const Int_t ROWS, const Int_t COLUMNS,  const Int_t PHISLICES, const Float_t DeltaPhi, 
				       const Int_t ITERATIONS, const Int_t SYMMETRY) ;

  Int_t    mDistortionMode;             // Distortion mode - determines which corrections are run

  Float_t  StarDriftV ;                 // Drift Velocity (cm/microSec) Magnitude
  Float_t  TPC_Z0 ;                     // Z location of STAR TPC Ground Wire Plane (cm) Magnitude
  Float_t  XTWIST ;                     // X Displacement of West end of TPC wrt magnet (mRad)
  Float_t  YTWIST ;                     // Y Displacement of West end of TPC wrt magnet (mRad)
  Double_t CathodeV ;                   // Cathode Potential (volts)
  Double_t GG ;                         // Gating Grid voltage (volts)
  Float_t  EASTCLOCKERROR ;             // Phi rotation of East end of TPC in milli-radians
  Float_t  WESTCLOCKERROR ;             // Phi rotation of West end of TPC in milli-radians
  Float_t  IFCRadius ;                  // Radius of the Inner Field Cage
  Float_t  OFCRadius ;                  // Radius of the Outer Field Cage
  Float_t  GAPRADIUS ;                  // Radius of the gap between the Inner sectors and the Outer sectors
  Float_t  GAP13_14 ;                   // Width of the gap between the grids at row 13 and row 14 (cm)
  Float_t  StarMagE ;                   // STAR Electric Field (V/cm) Magnitude
  Float_t  IFCShift ;                   // Shift of the IFC towards the West Endcap (cm)
  Float_t  TensorV1 ;                   // Omega Tau tensor parameter - in the ExB direction
  Float_t  TensorV2 ;                   // Omega Tau tensor parameter - in the direction perpendicular to ExB and Z axis
  Float_t  Const_0, Const_1, Const_2  ; // OmegaTau parameters
  Double_t SpaceCharge, SpaceChargeR2 ; // Space Charge parameters (uniform or 1/R**2 in the TPC - arbitrary units)
  Double_t InnerGridLeakStrength      ; // Relative strength of the Inner grid leak
  Double_t InnerGridLeakRadius        ; // Location (in local Y coordinates) of the Inner grid leak 
  Double_t InnerGridLeakWidth         ; // Half-width of the Inner grid leak.  Must be larger than life for numerical reasons.
  Double_t MiddlGridLeakStrength      ; // Relative strength of the Middle grid leak
  Double_t MiddlGridLeakRadius        ; // Location (in local Y coordinates) of the Middle grid leak 
  Double_t MiddlGridLeakWidth         ; // Half-width of the Middle grid leak.  Must be larger than life for numerical reasons.
  Double_t OuterGridLeakStrength      ; // Relative strength of the Outer grid leak
  Double_t OuterGridLeakRadius        ; // Location (in local Y coordinates) of the Outer grid leak 
  Double_t OuterGridLeakWidth         ; // Half-width of the Outer grid leak.  Must be larger than life for numerical reasons.
  Int_t    ShortTableRows             ; // Number of rows in the Shorted Ring Table
  Int_t    Side[10]                   ; // Location of Short   E=0 /   W=1
  Int_t    Cage[10]                   ; // Location of Short IFC=0 / OFC=1
  Float_t  Ring[10]                   ; // Location of Short counting out from the CM.  CM==0 
  Float_t  MissingResistance[10]      ; // Amount of Missing Resistance due to this short (MOhm)
  Float_t  Resistor[10]               ; // Amount of compensating resistance added for this short

  Float_t  Bz[nZ][nR], Br[nZ][nR] ;         
  Float_t  Radius[nR], ZList[nZ] ;         
  Float_t  Bz3D[nPhi][nZ][nR], Br3D[nPhi][nZ][nR], Bphi3D[nPhi][nZ][nR] ;         
  Float_t  R3D[nR], Z3D[nZ], Phi3D[nPhi] ;         
  Float_t  cmEr[neZ][nePhi][neR],    cmEphi[neZ][nePhi][neR] ;
  Float_t  endEr[neZ][nePhi][neR],   endEphi[neZ][nePhi][neR] ;
  Float_t  shiftEr[neZ][neR] ;
  Float_t  spaceEr[neZ][neR] ;
  Float_t  spaceR2Er[neZ][neR] ;
  Float_t  shortEr[neZ][neR] ;
  Float_t  eRadius[neR], ePhiList[nePhi], eZList[neZ]  ;         

 public:

  StMagUtilities () ;
  StMagUtilities ( StTpcDb* dbin,  TDataSet* dbin2, Int_t mode = 0 ) ;
  StMagUtilities ( const EBField map, const Float_t factor, Int_t mode = 0 ) ;
  virtual ~StMagUtilities () {}

  virtual void    BField ( const Float_t x[], Float_t B[] ) ;
  virtual void    BrBzField( const Float_t r, const Float_t z, Float_t &Br_value, Float_t &Bz_value ) ;
  virtual void    B3DField ( const Float_t x[], Float_t B[] ) ;
  virtual void    BrBz3DField ( const Float_t r, const Float_t z, const Float_t phi,
				Float_t &Br_value, Float_t &Bz_value, Float_t &Bphi_value ) ;
  virtual void    DoDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoBDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    Undo2DBDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    FastUndoBDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    FastUndo2DBDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoPad13Distortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoTwistDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoClockDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoMembraneDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoEndcapDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoSpaceChargeDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoSpaceChargeR2Distortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoGridLeakDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    Undo3DGridLeakDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoIFCShiftDistortion ( const Float_t x[], Float_t Xprime[] ) ;
  virtual void    UndoShortedRingDistortion ( const Float_t x[], Float_t Xprime[] ) ;

  virtual void    FixSpaceChargeDistortion ( const Int_t Charge, const Float_t x[3], const Float_t p[3],
					     const Prime PrimaryOrGlobal, 
					     Float_t x_new[3], Float_t p_new[3],
					     const unsigned int RowMask1 = 0xFFFFFF00 , 
					     const unsigned int RowMask2 = 0x1FFFFF,
					     const Float_t VertexError = 0.0200 ) ;

  virtual void    ApplySpaceChargeDistortion ( const Double_t sc, const Int_t Charge, 
					       const Float_t x[3], const Float_t p[3], 
					       const Prime PrimaryOrGlobal, Int_t &new_Charge, 
					       Float_t x_new[3], Float_t p_new[3],
					       const unsigned int RowMask1 = 0xFFFFFF00 , 
					       const unsigned int RowMask2 = 0x1FFFFF,
					       const Float_t VertexError = 0.0200 ) ;

  virtual Int_t   PredictSpaceChargeDistortion (Int_t   Charge, 
						Float_t Pt, 
						Float_t VertexZ, 
						Float_t PseudoRapidity, 
						Float_t DCA,  
						const unsigned int RowMask1, 
						const unsigned int RowMask2, 
						Float_t &pSpace ) ;

  virtual Int_t   PredictSpaceChargeDistortion (Int_t   Charge, 
						Float_t Pt, 
						Float_t VertexZ, 
						Float_t PseudoRapidity, 
						Float_t DCA,  
						const unsigned int RowMask1, 
						const unsigned int RowMask2, 
						Float_t RowMaskErrorR[64], 
						Float_t RowMaskErrorRPhi[64], 
						Float_t &pSpace ) ;

  virtual Int_t    GetSpaceChargeMode();
  virtual void     ManualSpaceCharge(Double_t SpcChg)   { SpaceCharge   = SpcChg ; fSpaceCharge   = 0 ; }
  virtual void     ManualSpaceChargeR2(Double_t SpcChg) { SpaceChargeR2 = SpcChg ; fSpaceChargeR2 = 0 ; }
  virtual void     AutoSpaceCharge()   {GetSpaceCharge()  ; } // use DB
  virtual void     AutoSpaceChargeR2() {GetSpaceChargeR2(); } // use DB
  virtual Double_t CurrentSpaceCharge()   {return SpaceCharge  ;}
  virtual Double_t CurrentSpaceChargeR2() {return SpaceChargeR2;}

  ClassDef(StMagUtilities,1)    // Base class for all STAR MagField

};

#endif








