// $Id: StFtpcDbReader.hh,v 1.4 2001/04/04 17:08:42 jcs Exp $
//
// $Log: StFtpcDbReader.hh,v $
// Revision 1.4  2001/04/04 17:08:42  jcs
// remove references to StFtpcParamReader from StFtpcDbReader
//
// Revision 1.3  2001/04/02 12:10:24  jcs
// get FTPC calibrations,geometry from MySQL database and code parameters
// from StarDb/ftpc
//
// Revision 1.2  2001/03/19 15:52:47  jcs
// use ftpcDimensions from database
//
// Revision 1.1  2001/03/06 23:34:06  jcs
// use database instead of params
//
//

#ifndef STAR_StFtpcDbReader
#define STAR_StFtpcDbReader

#include "TObject.h"

#include "tables/St_ftpcDimensions_Table.h"
#include "tables/St_ftpcPadrowZ_Table.h"
#include "tables/St_ftpcEField_Table.h"
#include "tables/St_ftpcVDrift_Table.h"
#include "tables/St_ftpcDeflection_Table.h"
#include "tables/St_ftpcdVDriftdP_Table.h"
#include "tables/St_ftpcdDeflectiondP_Table.h"
#include "tables/St_ftpcAmpSlope_Table.h"
#include "tables/St_ftpcAmpOffset_Table.h"
#include "tables/St_ftpcTimeOffset_Table.h"
#include "tables/St_ftpcGas_Table.h"
#include "tables/St_ftpcDriftField_Table.h"


class StFtpcDbReader : public TObject 
{
  
protected:
  //ClusterFinder parameters (also used by other classes)
  Int_t mNumberOfPadrows;
  Int_t mNumberOfPadrowsPerSide;
  Int_t mFirstPadrowToSearch;
  Int_t mLastPadrowToSearch;
  Int_t mNumberOfSectors;
  Int_t mFirstSectorToSearch;
  Int_t mLastSectorToSearch;
  Int_t mNumberOfPads;
  Int_t mNumberOfTimebins;

  Float_t mPhiOrigin;
  Float_t mPhiPerSector;
  Float_t mRadiansPerPad;
  Float_t mRadiansPerBoundary;
  Float_t mMicrosecondsPerTimebin;
  Float_t mSensitiveVolumeInnerRadius;
  Float_t mSensitiveVolumeOuterRadius;

  Float_t *mPadrowZPosition;

  Int_t   mNumberOfMagboltzBins;
  Float_t *mMagboltzEField;
  Float_t *mMagboltzVDrift;
  Float_t *mMagboltzDeflection;
  Float_t *mMagboltzdVDriftdP;
  Float_t *mMagboltzdDeflectiondP;

  Float_t mPercentAr;
  Float_t mPercentCO2;
  Float_t mPercentNe;
  Float_t mPercentHe;
  Float_t mGasGain;
  Float_t mGasAttenuation;
  Float_t mGasIonizationPotential;

  Float_t mTZero;
  Float_t mMinimumDriftField;
  Float_t mStepSizeDriftField;
  Float_t mRadiusTimesField;
  ftpcAmpSlope_st   *ampslopeTable;
  ftpcAmpOffset_st  *ampoffsetTable;
  ftpcTimeOffset_st *timeoffsetTable;
  //SlowSimulator parameters
  Float_t mPadLength;
  Float_t mPadPitch;
  Float_t mPhiEnd;

private:
  
public:
  // constructor used by StFtpcClusterMaker:
  StFtpcDbReader(St_ftpcDimensions    *dimensions,
                 St_ftpcPadrowZ       *zrow,
                 St_ftpcEField        *efield,
                 St_ftpcVDrift        *vdrift,
                 St_ftpcDeflection    *deflection,
                 St_ftpcdVDriftdP     *dvdriftdp,
                 St_ftpcdDeflectiondP *ddeflectiondp,
                 St_ftpcAmpSlope      *ampslope,
                 St_ftpcAmpOffset     *ampoffset,
                 St_ftpcTimeOffset    *timeoffset,
                 St_ftpcDriftField    *driftfield);
  // constructor used by StFtpcSlowSimMaker and StFtpcDriftMapMaker:
  StFtpcDbReader(St_ftpcDimensions    *dimensions,
                 St_ftpcPadrowZ       *zrow,
                 St_ftpcEField        *efield,
                 St_ftpcVDrift        *vdrift,
                 St_ftpcDeflection    *deflection,
                 St_ftpcdVDriftdP     *dvdriftdp,
                 St_ftpcdDeflectiondP *ddeflectiondp,
                 St_ftpcGas           *gas,
                 St_ftpcDriftField    *driftfield);
  ~StFtpcDbReader();
  Float_t padrowZPosition(Int_t i); 
  Float_t magboltzEField(Int_t i);
  Float_t magboltzVDrift(Int_t i,Int_t padrow);
  Float_t magboltzDeflection(Int_t i, Int_t padrow);
  Float_t magboltzdVDriftdP(Int_t i, Int_t padrow);
  Float_t magboltzdDeflectiondP(Int_t i, Int_t padrow);
  Float_t amplitudeSlope(Int_t i, Int_t padrow);
  Float_t amplitudeOffset(Int_t i, Int_t padrow);
  Float_t timeOffset(Int_t i, Int_t padrow);

  // parameter set functions
  Int_t setMagboltzEField(Int_t i, Float_t newvalue);
  Int_t setMagboltzVDrift(Int_t i, Int_t padrow, Float_t  newvalue);
  Int_t setMagboltzDeflection(Int_t i, Int_t padrow, Float_t  newvalue);
  Int_t setMagboltzdVDriftdP(Int_t i, Int_t padrow, Float_t  newvalue);
  Int_t setMagboltzdDeflectiondP(Int_t i, Int_t padrow, Float_t  newvalue);

  // inline get functions
  Int_t numberOfPadrows() {return mNumberOfPadrows;}
  Int_t numberOfPadrowsPerSide() {return mNumberOfPadrowsPerSide;}
  Int_t firstPadrowToSearch() {return mFirstPadrowToSearch;}
  Int_t lastPadrowToSearch() {return mLastPadrowToSearch;}
  Int_t numberOfSectors() {return mNumberOfSectors;}
  Int_t firstSectorToSearch() {return mFirstSectorToSearch;}
  Int_t lastSectorToSearch() {return mLastSectorToSearch;}
  Int_t numberOfPads() {return mNumberOfPads;}
  Int_t numberOfTimebins() {return mNumberOfTimebins;}

  Int_t numberOfMagboltzBins() {return mNumberOfMagboltzBins;}

  Float_t phiOrigin()    {return mPhiOrigin;}
  Float_t phiPerSector() {return mPhiPerSector;}
  Float_t phiEnd()       {return mPhiOrigin+mPhiPerSector;}
  Float_t padLength()    {return mPadLength;}
  Float_t padPitch()     {return mPadPitch;}
  Float_t radiansPerPad() {return mRadiansPerPad;}
  Float_t radiansPerBoundary() {return mRadiansPerBoundary;}
  Float_t microsecondsPerTimebin() {return mMicrosecondsPerTimebin;}
  Float_t sensitiveVolumeInnerRadius() {return mSensitiveVolumeInnerRadius;}
  Float_t sensitiveVolumeOuterRadius() {return mSensitiveVolumeOuterRadius;}

  Float_t percentAr() {return mPercentAr;}
  Float_t percentCO2() {return mPercentCO2;}
  Float_t percentNe() {return mPercentNe;}
  Float_t percentHe() {return mPercentHe;}
  Float_t gasGain() {return mGasGain;}
  Float_t gasAttenuation() {return mGasAttenuation;}
  Float_t gasIonizationPotential() {return mGasIonizationPotential;}

  Float_t tZero() {return mTZero;}
  Float_t minimumDriftField() {return mMinimumDriftField;}
  Float_t stepSizeDriftField() {return mStepSizeDriftField;}
  Float_t radiusTimesField() {return mRadiusTimesField;}
};

#endif
