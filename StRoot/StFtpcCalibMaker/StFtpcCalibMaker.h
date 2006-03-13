// $Id: StFtpcCalibMaker.h,v 1.1 2006/03/13 19:59:56 jcs Exp $
//
// $Log: StFtpcCalibMaker.h,v $
// Revision 1.1  2006/03/13 19:59:56  jcs
// commit initial version of the FTPC calibration maker
//

#ifndef STAR_StFtpcCalibMaker
#define STAR_StFtpcCalibMaker

/**
 * \class StFtpcCalibMaker
 * \brief The FTPC calibration maker
 *
 * This class is used to do the FTPC laser and t0 calibration
 * It processes the data in the special root file which is
 *  produced by running StFtpcCluster Maker and StFtpcTrackMaker with
 * the "fdbg" BFC option
 *
 * \author Joern Putschke, Janet Seyboth, Terry Tarnowsky
 *

#ifndef StMaker_H
#include "StMaker.h"
#endif


class StFtpcLaserCalib;
class StFtpcLaserTrafo;
class StFtpcDbReader;
class StFtpcParamReader;

class TH1F;
class TH2F;

class TNtuple;
class TFile;

class DetectorReader;
class St_ftpcClusterPars;
class St_ftpcDimensions;

class St_ftpcPadrowZ;
class St_ftpcEField;
class St_ftpcVDrift;
class St_ftpcDeflection;
class St_ftpcdVDriftdP;
class St_ftpcdDeflectiondP;
class St_ftpcElectronics;
class St_ftpcDriftField;
class St_ftpcGas;

class StFtpcCalibMaker : public StMaker 
{
 private:

  //Bool_t drawinit;
  
  St_ftpcClusterPars   *m_clusterpars;           //!
  St_ftpcDimensions    *m_dimensions;            //!
  St_ftpcPadrowZ       *m_padrow_z;              //!
  St_ftpcEField        *m_efield;                //!
  St_ftpcVDrift        *m_vdrift;                //!
  St_ftpcDeflection    *m_deflection;            //!
  St_ftpcdVDriftdP     *m_dvdriftdp;             //!
  St_ftpcdDeflectiondP *m_ddeflectiondp;         //!
  St_ftpcElectronics   *m_electronics;           //!
  St_ftpcDriftField    *m_driftfield;            //!
  St_ftpcGas           *m_gas;                   //!
  
  //void             MakeHistograms();// Histograms for FTPC cluster finder

  StFtpcDbReader *dbReader;
  StFtpcParamReader *paramReader;

  StFtpcLaserTrafo *trafo;
  StFtpcLaserTrafo *trafo2;
  TFile *anaf;

 protected:
 
 Int_t date;
 Int_t time;
   
 Float_t micropertime;
 Float_t deltapW;
 Float_t deltapE;
 Float_t deltap;
 Float_t tZero;

 public: 
 
   TH1F *hradeall,*hradwall, *hrade, *hradw;
   TH1F *htimee, *htimew;
   
   StFtpcCalibMaker(const char *name="ftpc_calib");

   //void DoLaserCalib(TString filename);
   void GetRunInfo(TString filename);
   void DoLaserCalib(TString filename,int ftpc, int lsec, int straight, int gfit, int minz, int maxz, int minrad, int maxrad, char* t0, char* gas,float mbfield);
   void DoT0Calib(TString filename,char* t0, char* gas, float mbfield);
   void DbInit(float mbfield);
   void HistInit(int nradbins,TString fname, char* t0, char* gas);
   void MakeT0Ps(int nradbins,TString psname, char* t0, char* gas);
   virtual ~StFtpcCalibMaker();


   // inline get functions

   Int_t Date() {return date;}
   Int_t Time() {return time;}

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StFtpcCalibMaker.h,v 1.1 2006/03/13 19:59:56 jcs Exp $ built "__DATE__" "__TIME__ ; return cvs;}


   ClassDef(StFtpcCalibMaker,1)   //StAF chain virtual base class for Makers
     
};
#endif
