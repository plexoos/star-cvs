#ifndef StQACosmicMaker_HH
#define StQACosmicMaker_HH
/***************************************************************************
 *
 * $Id: StQACosmicMaker.h,v 1.15 2003/09/10 19:47:28 perev Exp $
 *
 * Author: Raimond Snellings, LBNL, Jun 1999
 * Description:  Maker to QA the Cosmic data (hitfinding, tracking, 
 *               geometry etc.)
 *
 * $Log: StQACosmicMaker.h,v $
 * Revision 1.15  2003/09/10 19:47:28  perev
 * ansi corrs
 *
 * Revision 1.14  2000/08/18 20:34:47  snelling
 * Added hit errors to ntuple
 *
 * Revision 1.13  2000/07/14 23:18:25  snelling
 * Added the z-residual histograms
 *
 * Revision 1.12  2000/07/03 04:10:29  snelling
 * Fixed bug in ntuple (changed :: to :)
 *
 * Revision 1.11  2000/02/05 01:26:11  snelling
 * Fixed multiple declaration loop variable (SUN compiler does not like it)
 *
 * Revision 1.10  2000/02/03 23:51:39  snelling
 * Removed empty histograms
 *
 * Revision 1.9  1999/11/23 21:21:07  snelling
 * removed references to tphitclus table
 *
 **************************************************************************/
#include "StMaker.h"
#include "St_TableSorter.h"
#include "tpc/St_tcl_Module.h"
#include "tpc/St_tph_Module.h"
#include "tpc/St_tpt_residuals_Module.h"
#include "tpc/St_xyz_newtab_Module.h"
#include "tables/St_tcc_morphology_Table.h"
#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"
#include "TProfile.h"
#include "TString.h"

class StQACosmicMaker : public StMaker {

 public:

  StQACosmicMaker(const char *name="QACosmics");
  virtual        ~StQACosmicMaker();

  virtual Int_t  Init();
  virtual Int_t  Make();
  virtual Int_t  Finish();
  virtual void   PrintInfo();

  virtual void   setSector(const Int_t sectorNumber);
  virtual void   setNrXbins(const Int_t bins) {nXBins = bins;}

  virtual void   WriteTNtupleOn() {WriteTNtuple(kTRUE);}
  virtual void   WriteTNtupleOff(){WriteTNtuple();}
  virtual void   WritePostscriptOn() {WritePostscript(kTRUE);} 
  virtual void   WritePostscriptOff(){WritePostscript();}
  virtual void   WriteHistogramsOn() {WriteHistograms(kTRUE);} 
  virtual void   WriteHistogramsOff(){WriteHistograms();}

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StQACosmicMaker.h,v 1.15 2003/09/10 19:47:28 perev Exp $ built "__DATE__" "__TIME__ ; return cvs;}

 private:

  Int_t  SelectedSector;
  Bool_t bSectorSelectionOn;
  Bool_t bWriteTNtupleOn;
  Bool_t bWritePostscriptOn;
  Bool_t bWriteHistogramsOn;
  Int_t  nXBins;
  TString MakerName;

  Int_t  initClusHistograms();
  Int_t  fillClusHistograms();
  Int_t  initMorphHistograms();
  Int_t  fillMorphHistograms();
  Int_t  initResHistograms();
  Int_t  fillResHistograms();
  Int_t  calcResHistograms();
  Int_t  initChargeHistograms();
  Int_t  fillChargeHistograms();
  Int_t  calcChargeHistograms();
  Int_t  writeOutHistograms();
  Int_t  writeOutPostscript();

  Int_t  initTNtuple();
  Int_t  fillHitTNtuple();
  Int_t  fillTrackTNtuple();
  Int_t  fillMorphTNtuple();
  Int_t  fillHitTNtupleNoTrk();
  Int_t  fillMorphTNtupleNoTrk();
  Int_t  writeOutTNtuple();

  Int_t  fillTablePointers();
  Int_t  cleanUpTableSorters();

  Bool_t btphit;
  Bool_t bmorph;
  Bool_t brestpt;
  Bool_t btptrack;
  
  // bool selection
  void   WritePostscript(Bool_t flag=kFALSE){bWritePostscriptOn=flag;}
  void   WriteTNtuple(Bool_t flag=kFALSE){bWriteTNtupleOn=flag;}
  void   WriteHistograms(Bool_t flag=kFALSE){bWriteHistogramsOn=flag;}
  void   SectorSelection(Bool_t flag=kFALSE){bSectorSelectionOn=flag;}
  void   SectorSelectionOn() {SectorSelection(kTRUE);} 
  void   SectorSelectionOff(){SectorSelection();}

  //-----------------------------------------------------------------------
  // pointers to tables
  // pixel table
  St_tfc_adcxyz *phtfc; //!
  tfc_adcxyz_st *ptadcxyz; //!

  // hit table
  St_tcl_tphit *phtcl; //!
  tcl_tphit_st *pttphit; //!

  // Tom's morphology table
  St_tcc_morphology *phmorph; //!
  tcc_morphology_st *ptmorph; //!

  // track table 
  St_tpt_track *phtrk; //!
  tpt_track_st *pttrk; //!

  // residuals
  St_tpt_res *phres;//!
  tpt_res_st *ptres; //!

  // Table sorters
  St_TableSorter *ressorter; //!
  St_TableSorter *trksorter; //!
  St_TableSorter *morphsorter; //!

 protected:

  enum {nResHist = 4, nChargeHist = 4, nClusterHist = 4,
	nMorphHist = 4 };

  enum {hflag,hrow,hsector,hx,hy,hz,hdx,hdy,hdz,hq,dedx,halpha,hlambda,hdalpha,hdlambda,resy,resz,trknfit,trkp,enumLast};
  float ntEntries[enumLast];
 
  TNtuple *mTNtupleTPC; //!
  TNtuple *mTNtupleMorph; //!
  TNtuple *mTNtupleTPCNoTrk; //!
  TNtuple *mTNtupleMorphNoTrk; //!
  TNtuple *mTNtupleTrk; //!

  struct ClusterHist {
    TH1F *mNPadsPerHit;
    TH1F *mNTimeBucketsPerHit;
  };

  struct ClusterHist ClusterHists[nClusterHist]; //!

  struct MorphHist {
    TH1F *mNumberOfSequences;
    TH1F *mNumberOfPixels;
    TH1F *mNumberOfPads;
    TH1F *mNumberOfHits;
    TH1F *mTotalCharge;
    TH1F *mMaxCharge;
    TH1F *mAverageCharge;
    TH1F *mPadSigma1;
    TH1F *mTimeSigma1;
    TH1F *mPadTimeSigma1Sq;
    TH1F *mEcc1;
    TH1F *mLinEcc1;
    TH1F *mPadSigma2;
    TH1F *mTimeSigma2;
    TH1F *mPadTimeSigma2Sq;
    TH1F *mEcc2;
    TH1F *mLinEcc2;
  };
  
  struct MorphHist MorphHists[nMorphHist]; //!
  
  struct FitHist {
    TH1D *mXYResVersusAlpha_mean;
    TH1D *mXYResVersusAlpha_sigma;
    TH1D *mXYResVersusAlpha_mag;
    TH1D *mXYResVersusAlpha_chi;
    TH1D *mXYResVersusAlphaZ1_mean;
    TH1D *mXYResVersusAlphaZ1_sigma;
    TH1D *mXYResVersusAlphaZ1_mag;
    TH1D *mXYResVersusAlphaZ1_chi;
    TH1D *mXYResVersusAlphaZ2_mean;
    TH1D *mXYResVersusAlphaZ2_sigma;
    TH1D *mXYResVersusAlphaZ2_mag;
    TH1D *mXYResVersusAlphaZ2_chi;
    TH1D *mXYResVersusAlphaZ3_mean;
    TH1D *mXYResVersusAlphaZ3_sigma;
    TH1D *mXYResVersusAlphaZ3_mag;
    TH1D *mXYResVersusAlphaZ3_chi;
    TH1D *mXYResVersusAlphaZ4_mean;
    TH1D *mXYResVersusAlphaZ4_sigma;
    TH1D *mXYResVersusAlphaZ4_mag;
    TH1D *mXYResVersusAlphaZ4_chi;
    TH1D *mXYResVersusDip_mean;
    TH1D *mXYResVersusDip_sigma;
    TH1D *mXYResVersusDip_mag;
    TH1D *mXYResVersusDip_chi;
    TH1D *mXYResVersusDipZ1_mean;
    TH1D *mXYResVersusDipZ1_sigma;
    TH1D *mXYResVersusDipZ1_mag;
    TH1D *mXYResVersusDipZ1_chi;
    TH1D *mXYResVersusDipZ2_mean;
    TH1D *mXYResVersusDipZ2_sigma;
    TH1D *mXYResVersusDipZ2_mag;
    TH1D *mXYResVersusDipZ2_chi;
    TH1D *mXYResVersusDipZ3_mean;
    TH1D *mXYResVersusDipZ3_sigma;
    TH1D *mXYResVersusDipZ3_mag;
    TH1D *mXYResVersusDipZ3_chi;
    TH1D *mXYResVersusDipZ4_mean;
    TH1D *mXYResVersusDipZ4_sigma;
    TH1D *mXYResVersusDipZ4_mag;
    TH1D *mXYResVersusDipZ4_chi;
    TH1D *mZResVersusAlpha_mean;
    TH1D *mZResVersusAlpha_sigma;
    TH1D *mZResVersusAlpha_mag;
    TH1D *mZResVersusAlpha_chi;
    TH1D *mZResVersusAlphaZ1_mean;
    TH1D *mZResVersusAlphaZ1_sigma;
    TH1D *mZResVersusAlphaZ1_mag;
    TH1D *mZResVersusAlphaZ1_chi;
    TH1D *mZResVersusAlphaZ2_mean;
    TH1D *mZResVersusAlphaZ2_sigma;
    TH1D *mZResVersusAlphaZ2_mag;
    TH1D *mZResVersusAlphaZ2_chi;
    TH1D *mZResVersusAlphaZ3_mean;
    TH1D *mZResVersusAlphaZ3_sigma;
    TH1D *mZResVersusAlphaZ3_mag;
    TH1D *mZResVersusAlphaZ3_chi;
    TH1D *mZResVersusAlphaZ4_mean;
    TH1D *mZResVersusAlphaZ4_sigma;
    TH1D *mZResVersusAlphaZ4_mag;
    TH1D *mZResVersusAlphaZ4_chi;
    TH1D *mZResVersusDip_mean;
    TH1D *mZResVersusDip_sigma;
    TH1D *mZResVersusDip_mag;
    TH1D *mZResVersusDip_chi;
    TH1D *mZResVersusDipZ1_mean;
    TH1D *mZResVersusDipZ1_sigma;
    TH1D *mZResVersusDipZ1_mag;
    TH1D *mZResVersusDipZ1_chi;
    TH1D *mZResVersusDipZ2_mean;
    TH1D *mZResVersusDipZ2_sigma;
    TH1D *mZResVersusDipZ2_mag;
    TH1D *mZResVersusDipZ2_chi;
    TH1D *mZResVersusDipZ3_mean;
    TH1D *mZResVersusDipZ3_sigma;
    TH1D *mZResVersusDipZ3_mag;
    TH1D *mZResVersusDipZ3_chi;
    TH1D *mZResVersusDipZ4_mean;
    TH1D *mZResVersusDipZ4_sigma;
    TH1D *mZResVersusDipZ4_mag;
    TH1D *mZResVersusDipZ4_chi;
  };

  struct ResidualHist;
  friend struct ResidualHist;

  struct ResidualHist {
    TH2F *mXYResVersusAlpha;
    TH2F *mXYResVersusAlphaZ1;
    TH2F *mXYResVersusAlphaZ2;
    TH2F *mXYResVersusAlphaZ3;
    TH2F *mXYResVersusAlphaZ4;
    TH2F *mXYResVersusDip;
    TH2F *mXYResVersusDipZ1;
    TH2F *mXYResVersusDipZ2;
    TH2F *mXYResVersusDipZ3;
    TH2F *mXYResVersusDipZ4;
    TH2F *mZResVersusAlpha;
    TH2F *mZResVersusAlphaZ1;
    TH2F *mZResVersusAlphaZ2;
    TH2F *mZResVersusAlphaZ3;
    TH2F *mZResVersusAlphaZ4;
    TH2F *mZResVersusDip;
    TH2F *mZResVersusDipZ1;
    TH2F *mZResVersusDipZ2;
    TH2F *mZResVersusDipZ3;
    TH2F *mZResVersusDipZ4;
    struct FitHist FitHists; 
  };

  // histograms for inner/outer sector plus low/high momentum
  struct ResidualHist ResidualHists[nResHist]; //! 


  // histograms for Charge Uniformity
  struct FitQHist {
    TH1D *mQ_mean;
    TH1D *mQ_sigma;
    TH1D *mQ_mag;
    TH1D *mQ_chi;
  };

  struct ChargeHist;
  friend struct ChargeHist;

  struct ChargeHist {
    TH2F *mQdist;
    TProfile *mQprof;
    struct FitQHist FitQHists; 
  };

  // histograms for q versus x,y,z
  struct ChargeHist ChargeHists[nChargeHist]; //! 

  ClassDef(StQACosmicMaker,0) //macro for rootcint
};

#endif
