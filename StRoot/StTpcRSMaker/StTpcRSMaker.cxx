/// \author Y.Fisyak, fisyak@bnl.gov
/// \date
// doxygen info here
/*
  The maker's algorithms and formulae based on 
  http://www.inst.bnl.gov/programs/gasnobledet/publications/Mathieson's_Book.pdf,
  and  Photo Absorption Model
  "Ionization energy loss in very thin absorbers.", V.M. Grishin, V.K. Ermilova, S.K. Kotelnikov Nucl.Instrum.Meth.A309:476-484,1991
  "A method to improve tracking and particle identification in TPC's and silicon detectors.", Hans Bichsel, Nucl.Instrum.Meth.A562:154-197,2006
*/
#include <assert.h>
#include "StTpcRSMaker.h"
#include "Stiostream.h"
// SCL
#include "StGlobals.hh"
#include "StThreeVectorD.hh"
#include "StPhysicalHelixD.hh"
// ROOT
#include "TClassTable.h"
#include "TDataSetIter.h"
#include "TTableSorter.h"
#include "TRandom.h"
#include "TSystem.h"
#include "TFile.h"
#include "TBenchmark.h"
#include "TProfile2D.h"
#include "TVirtualMC.h"
#include "TInterpreter.h"
#include "Math/SpecFuncMathMore.h"
// Dave's Header file
#include "StDbUtilities/StTpcCoordinateTransform.hh"
#include "StDbUtilities/StCoordinates.hh" 
#include "StDbUtilities/StMagUtilities.h"
#include "StDaqLib/TPC/trans_table.hh"
#include "StDetectorDbMaker/St_TpcAltroParametersC.h"
#include "StDetectorDbMaker/St_asic_thresholdsC.h"
#include "StDetectorDbMaker/St_asic_thresholds_tpxC.h"
#include "StDetectorDbMaker/St_tss_tssparC.h"
#include "StDetectorDbMaker/St_tpcPadGainT0C.h"
#include "StDetectorDbMaker/St_TpcResponseSimulatorC.h"
#include "StDetectorDbMaker/St_tpcAnodeHVavgC.h"
#include "StDetectorDbMaker/StDetectorDbTpcRDOMasks.h"
#include "Altro.h"
#include "TRVector.h"
#define PrPP(A,B) {LOG_INFO << "StTpcRSMaker::" << (#A) << "\t" << (#B) << " = \t" << (B) << endm;}
static const char rcsid[] = "$Id: StTpcRSMaker.cxx,v 1.36 2010/04/20 13:56:24 fisyak Exp $";
//#define __ClusterProfile__
#define Laserino 170
#define Chasrino 171
//                                    Inner        Outer
static       Double_t t0IO[2]   = {1.20868e-9, 1.43615e-9}; // recalculated in InducedCharge
static const Double_t tauC[2]   = {999.655e-9, 919.183e-9}; 
TF1*     StTpcRSMaker::fgTimeShape3[2]    = {0, 0};
TF1*     StTpcRSMaker::fgTimeShape0[2]    = {0, 0};
//________________________________________________________________________________
static const Int_t nx[2] = {200,500};
static const Double_t xmin[2] =  {-10., -6};
static const Double_t xmax[2] =  { 10., 44};
#ifdef __ClusterProfile__
static const Int_t nz = 42;
static const Double_t zmin = -210;
static const Double_t zmax = -zmin;
//                      io pt
static TProfile2D *hist[4][2];
#endif
//________________________________________________________________________________
ClassImp(StTpcRSMaker);
//________________________________________________________________________________
StTpcRSMaker::StTpcRSMaker(const char *name): 
  StMaker(name), fTree(0),
  m_SignalSum(0),
  mdNdx(0), mdNdE(0),
  mChargeFractionInner(0), mPadResponseFunctionInner(0), 
  mChargeFractionOuter(0), mPadResponseFunctionOuter(0), mPolya(0),
  mPAI(0), mLaserScale(1),
  minSignal(1e-4),
  InnerAlphaVariation(0),
  OuterAlphaVariation(0),
  innerSectorAnodeVoltage(1170),
  outerSectorAnodeVoltage(1390),
  ElectronRange(0.0055), // Electron Range(.055mm)
  ElectronRangeEnergy(3000), // eV
  ElectronRangePower(1.78), // sigma =  ElectronRange*(eEnery/ElectronRangeEnergy)**ElectronRangePower
  NoOfSectors(24),
  NoOfRows(45),
  NoOfInnerRows(13),
  NoOfPads(182),
  NoOfTimeBins(__MaxNumberOfTimeBins__),
  mAltro(0),
  mCutEle(1e-4)
{
  memset (mShaperResponses, 0, sizeof(mShaperResponses));
  m_Mode = 0;
  //  SETBIT(m_Mode,kPAI); 
  SETBIT(m_Mode,kBICHSEL);  // Default is Bichsel
  SETBIT(m_Mode,kdEdxCorr);
  SETBIT(m_Mode,kDistortion);
}
//________________________________________________________________________________
StTpcRSMaker::~StTpcRSMaker() {
  SafeDelete(mAltro);
  Finish();
}
//________________________________________________________________________________
Int_t StTpcRSMaker::Finish() {
  //  SafeDelete(fTree);
  free(m_SignalSum); m_SignalSum = 0;
  SafeDelete(mdNdx);
  SafeDelete(mdNdE);
  for (Int_t i = 0; i < 2; i++) // Inner/Outer
    for (Int_t j = 0; j < NoOfSectors; j++) {
      if (!TestBit(kNotDeleted)) {SafeDelete(mShaperResponses[i][j]);}
    }
  SafeDelete(mChargeFractionInner);
  SafeDelete(mPadResponseFunctionInner);
  SafeDelete(mChargeFractionOuter);
  SafeDelete(mPadResponseFunctionOuter);
  SafeDelete(mPolya);
  SafeDelete(m_TpcdEdxCorrection);
  SafeDelete(mPAI);
  return StMaker::Finish();
}
//________________________________________________________________________________
Int_t StTpcRSMaker::InitRun(Int_t runnumberOf) {
  if (!gStTpcDb) {
    LOG_ERROR << "Database Missing! Can't initialize TpcRS" << endm;
    return kStFatal;
  }
#if 0
  if (! gMC) {
    LOG_INFO << "TVirtualMC has not been instantiated" << endm;
    return kStFatal;
  }
  TString cmd("Gccuts_t *ccuts  = (Gccuts_t *) ((");// TGeant3 *) gMC
  if (gClassTable->GetID("TGiant3") >= 0) { // root4star
    cmd += "TGiant";
  } else {
    cmd += "TGeant";
  }
  cmd += Form(" *) %p))->Gccuts();",gMC); 
  cmd += Form("((StTpcRSMaker *) %p)->SetCutEle(ccuts->cutele;",this);
  TInterpreter::EErrorCode error = TInterpreter::kNoError;
  gInterpreter->ProcessLine(cmd.Data(), &error);
  assert(error == TInterpreter::kNoError);
#endif
  if (TESTBIT(m_Mode, kPAI)) {
    mPAI = PAI::Instance(); 
    LOG_INFO << "StTpcRSMaker:: use PAI model for dE/dx simulation" << endm;
  }
  else if (TESTBIT(m_Mode, kBICHSEL)) {
    LOG_INFO << "StTpcRSMaker:: use H.Bichsel model for dE/dx simulation" << endm;
    if (! mdNdE || ! mdNdx) {
      const Char_t *path  = ".:./StarDb/dEdxModel:./StarDb/global/dEdx"
	":./StRoot/StBichsel:$STAR/StarDb/dEdxModel:$STAR/StarDb/global/dEdx:$STAR/StRoot/StBichsel";
      const Char_t *Files[2] = {"dNdE_Bichsel.root","dNdx_Bichsel.root"};
      for (Int_t i = 0; i < 2; i++) { // Inner/Outer
	Char_t *file = gSystem->Which(path,Files[i],kReadPermission);
	if (! file) Fatal("StTpcRSMaker::Init","File %s has not been found in path %s",Files[i],path);
	else        Warning("StTpcRSMaker::Init","File %s has been found as %s",Files[i],file);
	TFile       *pFile = new TFile(file);
	if (i == 0) {mdNdE = (TH1D *) pFile->Get("dNdEI"); assert(mdNdE);   mdNdE->SetDirectory(0);}
	if (i == 1) {mdNdx = (TH1D *) pFile->Get("dNdx"); assert(mdNdx);   mdNdx->SetDirectory(0);}
	delete pFile;
      }
    }
  }
  else {LOG_INFO << "StTpcRSMaker:: use GEANT321 model for dE/dx simulation" << endm;}
  // Distortions
  if (TESTBIT(m_Mode,kdEdxCorr)) {
    LOG_INFO << "StTpcRSMaker:: use Tpc dE/dx correction from calibaration" << endm;
    Int_t Mask = -1; // 22 bits
    CLRBIT(Mask,StTpcdEdxCorrection::kAdcCorrection);
    m_TpcdEdxCorrection = new StTpcdEdxCorrection(Mask);
  }
  if (TESTBIT(m_Mode,kDistortion)) {
    LOG_INFO << "StTpcRSMaker:: use Tpc distortion correction" << endm;
  }
  if (Debug() && gStTpcDb->PadResponse()) gStTpcDb->PadResponse()->Table()->Print(0,1);
  Double_t samplingFrequency              = 1.e6*gStTpcDb->Electronics()->samplingFrequency(); // Hz
  Double_t TimeBinWidth                  = 1./samplingFrequency;
  numberOfInnerSectorAnodeWires  = gStTpcDb->WirePlaneGeometry()->numberOfInnerSectorAnodeWires ();
  firstInnerSectorAnodeWire      = gStTpcDb->WirePlaneGeometry()->firstInnerSectorAnodeWire();
  lastInnerSectorAnodeWire       = gStTpcDb->WirePlaneGeometry()->lastInnerSectorAnodeWire ();
  numberOfOuterSectorAnodeWires  = gStTpcDb->WirePlaneGeometry()->numberOfOuterSectorAnodeWires ();
  firstOuterSectorAnodeWire      = gStTpcDb->WirePlaneGeometry()->firstOuterSectorAnodeWire();
  lastOuterSectorAnodeWire       = gStTpcDb->WirePlaneGeometry()->lastOuterSectorAnodeWire ();
  anodeWirePitch                 = gStTpcDb->WirePlaneGeometry()->anodeWirePitch           ();
  anodeWireRadius                = gStTpcDb->WirePlaneGeometry()->anodeWireRadius(); 
  Float_t BFieldG[3]; 
  Float_t xyz[3] = {0,0,0};
  StMagF::Agufld(xyz,BFieldG);
  // Shapers
  Double_t timeBinMin = -0.5;
  Double_t timeBinMax = 44.5;
  const Char_t *Names[2] = {"I","O"};
  Double_t CathodeAnodeGap[2] = {0.2, 0.4};
  innerSectorAnodeVoltage = outerSectorAnodeVoltage = 0;
  Int_t nAliveInner = 0;
  Int_t nAliveOuter = 0;
  for (Int_t sec = 1; sec <= 24; sec++) {
    for (Int_t row = 1; row <= 45; row++) {
      if (St_tpcAnodeHVavgC::instance()->livePadrow(sec,row)) {
	if (row <= 13) {
	  nAliveInner++;
	  innerSectorAnodeVoltage += St_tpcAnodeHVavgC::instance()->voltagePadrow(sec,row);
	} else {
	  nAliveOuter++;
	  outerSectorAnodeVoltage += St_tpcAnodeHVavgC::instance()->voltagePadrow(sec,row);
	}
      }
    }
  }
  if (! nAliveInner && ! nAliveOuter) {
    LOG_INFO << "Illegal date/time. Tpc Anode Voltage is not set to run condition: AliveInner: " << nAliveInner 
	     << "\tAliveOuter: " << nAliveOuter 
	     << "\tStop the run" << endm;
    return kStFatal;
  }
  if (nAliveInner > 1) innerSectorAnodeVoltage /= nAliveInner;
  if (nAliveOuter > 1) outerSectorAnodeVoltage /= nAliveOuter;
  
  for (Int_t io = 0; io < 2; io++) {// In/Out
    if (io == 0) {
      LOG_INFO << "Inner Sector ======================" << endm;
      InnerAlphaVariation = InducedCharge(anodeWirePitch,
					  CathodeAnodeGap[io],
					  anodeWireRadius,
					  innerSectorAnodeVoltage, t0IO[io]);
    }
    else {
      LOG_INFO << "Outer Sector ======================" << endm;
      OuterAlphaVariation = InducedCharge(anodeWirePitch,
					  CathodeAnodeGap[io],
					  anodeWireRadius,
					  outerSectorAnodeVoltage, t0IO[io]);
    }
    Double_t params3[7] = {t0IO[io], 
			   St_TpcResponseSimulatorC::instance()->tauF(), 
			   St_TpcResponseSimulatorC::instance()->tauP(), 
			   St_TpcResponseSimulatorC::instance()->tauIntegration(), 
			   TimeBinWidth,     0, io};
    Double_t params0[5] = {t0IO[io],             St_TpcResponseSimulatorC::instance()->tauX()[io], TimeBinWidth,     0, io};
    if (! fgTimeShape3[io]) {// old electronics, intergation + shaper alltogether
      fgTimeShape3[io] = new TF1(Form("TimeShape3%s",Names[io]),
				 shapeEI3,timeBinMin*TimeBinWidth,timeBinMax*TimeBinWidth,7);
      fgTimeShape3[io]->SetParNames("t0","tauF","tauP", "tauI","width","tauC","io");
      fgTimeShape3[io]->SetParameters(params3);
      params3[5] = fgTimeShape3[io]->Integral(timeBinMin*TimeBinWidth,timeBinMax*TimeBinWidth);
      fgTimeShape3[io]->SetTitle(fgTimeShape3[io]->GetName());
      fgTimeShape3[io]->GetXaxis()->SetTitle("time (secs)");
      fgTimeShape3[io]->GetYaxis()->SetTitle("signal");
    }
    if (! fgTimeShape0[io]) {// new electronics only integration
      fgTimeShape0[io] = new TF1(Form("TimeShape%s",Names[io]),
			    shapeEI,timeBinMin*TimeBinWidth,timeBinMax*TimeBinWidth,5);
      fgTimeShape0[io]->SetParNames("t0","tauI","width","tauC","io");
      params0[3] = St_TpcResponseSimulatorC::instance()->tauC()[io];
      fgTimeShape0[io]->SetParameters(params0);
      params0[3] = fgTimeShape0[io]->Integral(0,timeBinMax*TimeBinWidth);
      fgTimeShape0[io]->SetTitle(fgTimeShape0[io]->GetName());
      fgTimeShape0[io]->GetXaxis()->SetTitle("time (secs)");
      fgTimeShape0[io]->GetYaxis()->SetTitle("signal");
    }
    
    for (Int_t sector = 1; sector <= NoOfSectors; sector++) {
      if (St_TpcAltroParametersC::instance()->N(sector-1) < 0) {// old TPC
	// check if the function has been created
	for (Int_t sec = 1; sec < sector; sec++) {
	  if (St_TpcAltroParametersC::instance()->N(sec-1) < 0 &&
	      ! mShaperResponses[io][sec-1]) {
	    mShaperResponses[io][sector-1] = mShaperResponses[io][sec-1];
	    break;
	  }
	}
	if (! mShaperResponses[io][sector-1]) {
	  mShaperResponses[io][sector-1] = new TF1(Form("ShaperFunc_%s_S%02i",Names[io],sector),
						    StTpcRSMaker::shapeEI3_I,timeBinMin,timeBinMax,7);  
	  mShaperResponses[io][sector-1]->SetParameters(params3);
	  mShaperResponses[io][sector-1]->SetParNames("t0","tauF","tauP", "tauI", "width","norm","io");
	  mShaperResponses[io][sector-1]->SetNpx((Int_t) (10*(timeBinMax-timeBinMin)));
	  mShaperResponses[io][sector-1]->Save(timeBinMin,timeBinMax,0,0,0,0);
	  mShaperResponses[io][sector-1]->SetTitle(mShaperResponses[io][sector-1]->GetName());
	  mShaperResponses[io][sector-1]->GetXaxis()->SetTitle("time (buckets)");
	  mShaperResponses[io][sector-1]->GetYaxis()->SetTitle("signal");
	}
	continue;
      } 
      //Altro
      for (Int_t sec = 1; sec < sector; sec++) {
	if (St_TpcAltroParametersC::instance()->N(sec-1) >= 0 &&
	    ! mShaperResponses[io][sec-1] ) {
	  mShaperResponses[io][sector-1] = mShaperResponses[io][sec-1];
	  break;
	}
      }
      if (! mShaperResponses[io][sector-1]) {
	mShaperResponses[io][sector-1] = new TF1(Form("ShaperFunc_%s_S%02i",Names[io],sector),
						  StTpcRSMaker::shapeEI_I,timeBinMin,timeBinMax,5);  
	mShaperResponses[io][sector-1]->SetParameters(params0);
	mShaperResponses[io][sector-1]->SetParNames("t0","tauI", "width","norm","io");
	mShaperResponses[io][sector-1]->SetNpx((Int_t) (10*(timeBinMax-timeBinMin)));
	mShaperResponses[io][sector-1]->Save(timeBinMin,timeBinMax,0,0,0,0);
	mShaperResponses[io][sector-1]->SetTitle(mShaperResponses[io][sector-1]->GetName());
	mShaperResponses[io][sector-1]->GetXaxis()->SetTitle("time (buckets)");
	mShaperResponses[io][sector-1]->GetYaxis()->SetTitle("signal");
      }
    }
  }
  //                             w       h         s      a       l   i
  //  Double_t paramsI[6] = {0.2850, 0.2000,  0.4000, 0.0010, 1.1500, 0};
  //  Double_t paramsO[6] = {0.6200, 0.4000,  0.4000, 0.0010, 1.1500, 0};
  if (! mPadResponseFunctionInner ||
      ! mPadResponseFunctionOuter ||
      ! mChargeFractionInner ||
      ! mChargeFractionOuter) {
    Double_t params[5];
    params[0] = gStTpcDb->PadPlaneGeometry()->innerSectorPadWidth();                     // w = width of pad       
    params[1] = gStTpcDb->WirePlaneGeometry()->innerSectorAnodeWirePadPlaneSeparation(); // h = Anode-Cathode gap   
    params[2] = gStTpcDb->WirePlaneGeometry()->anodeWirePitch();                         // s = wire spacing       
    //    params[3] = anodeWireRadius;                                                         // a = Anode wire radius  
    params[4] = 0;
    params[5] = 0;
    Double_t xmaxP = 2.75;//4.5*gStTpcDb->PadPlaneGeometry()->innerSectorPadWidth();// 4.5 
    Double_t xminP = -xmaxP; 
    if (! mPadResponseFunctionInner) {
      mPadResponseFunctionInner = new TF1("PadResponseFunctionInner",
					  StTpcRSMaker::PadResponseFunc,xminP,xmaxP,6); 
      params[3] = St_TpcResponseSimulatorC::instance()->K3IP();
      params[4] = 0;
      params[5] = gStTpcDb->PadPlaneGeometry()->innerSectorPadPitch();
      mPadResponseFunctionInner->SetParameters(params);
      mPadResponseFunctionInner->SetTitle(mPadResponseFunctionInner->GetName());
      mPadResponseFunctionInner->SetParNames("PadWidth","Anode-Cathode gap","wire spacing","K3IP","CrossTalkInner","PadPitch");
      mPadResponseFunctionInner->GetXaxis()->SetTitle("pads");
      mPadResponseFunctionInner->GetYaxis()->SetTitle("Signal");
      mPadResponseFunctionInner->Save(xminP,xmaxP,0,0,0,0);
    }
    if (! mChargeFractionInner) {
      xmaxP = 1.5;//5*gStTpcDb->PadPlaneGeometry()->innerSectorPadLength(); // 1.42
      xminP = - xmaxP;
      mChargeFractionInner = new TF1("ChargeFractionInner",
				     StTpcRSMaker::PadResponseFunc,xminP,xmaxP,6);
      params[0] = gStTpcDb->PadPlaneGeometry()->innerSectorPadLength();
      params[3] = St_TpcResponseSimulatorC::instance()->K3IR();
      params[4] = 0;
      params[5] = 1.; 
      mChargeFractionInner->SetParameters(params);
      mChargeFractionInner->SetParNames("PadLength","Anode-Cathode gap","wire spacing","K3IR","CrossTalkInner","RowPitch");
      mChargeFractionInner->SetTitle(mChargeFractionInner->GetName());
      mChargeFractionInner->GetXaxis()->SetTitle("Distance (cm)");
      mChargeFractionInner->GetYaxis()->SetTitle("Signal");
      mChargeFractionInner->Save(xminP,xmaxP,0,0,0,0);
    }
    if (! mPadResponseFunctionOuter) {
      xmaxP = 2.75;//5.*gStTpcDb->PadPlaneGeometry()->outerSectorPadWidth(); // 3.
      xminP = - xmaxP;
      mPadResponseFunctionOuter = new TF1("PadResponseFunctionOuter",
					  StTpcRSMaker::PadResponseFunc,xminP,xmaxP,6); 
      params[0] = gStTpcDb->PadPlaneGeometry()->outerSectorPadWidth();                    // w = width of pad       
      params[1] = gStTpcDb->WirePlaneGeometry()->outerSectorAnodeWirePadPlaneSeparation();// h = Anode-Cathode gap   
      params[2] = gStTpcDb->WirePlaneGeometry()->anodeWirePitch();                        // s = wire spacing       
      //      params[3] = gStTpcDb->WirePlaneGeometry()->anodeWireRadius();               // a = Anode wire radius  
      params[3] = St_TpcResponseSimulatorC::instance()->K3OP();
      params[4] = 0;
      params[5] = gStTpcDb->PadPlaneGeometry()->outerSectorPadPitch();
      mPadResponseFunctionOuter->SetParameters(params);
      mPadResponseFunctionOuter->SetParNames("PadWidth","Anode-Cathode gap","wire spacing","K3OP","CrossTalkOuter","PadPitch");
      mPadResponseFunctionOuter->SetTitle(mPadResponseFunctionOuter->GetName());
      mPadResponseFunctionOuter->GetXaxis()->SetTitle("pads");
      mPadResponseFunctionOuter->GetYaxis()->SetTitle("Signal");
      mPadResponseFunctionOuter->Save(xminP,xmaxP,0,0,0,0);
    }
    if (! mChargeFractionOuter) {
      xmaxP = 2.5;//5*gStTpcDb->PadPlaneGeometry()->outerSectorPadLength(); // 1.26
      xminP = - xmaxP;
      mChargeFractionOuter = new TF1("ChargeFractionOuter",
				     StTpcRSMaker::PadResponseFunc,xminP,xmaxP,6);
      params[0] = gStTpcDb->PadPlaneGeometry()->outerSectorPadLength();
      params[3] = St_TpcResponseSimulatorC::instance()->K3OR(); 
      params[4] = 0;
      params[5] = 1.; 
      mChargeFractionOuter->SetParameters(params);
      mChargeFractionOuter->SetParNames("PadLength","Anode-Cathode gap","wire spacing","K3IR","CrossTalkOuter","RowPitch");
      mChargeFractionOuter->SetTitle(mChargeFractionOuter->GetName());
      mChargeFractionOuter->GetXaxis()->SetTitle("Distance (cm)");
      mChargeFractionOuter->GetYaxis()->SetTitle("Signal");
      mChargeFractionOuter->Save(xminP,xmaxP,0,0,0,0);
    }
  }
  //  TF1 *func = new TF1("funcP","x*sqrt(x)/exp(2.5*x)",0,10);
  // see http://www4.rcf.bnl.gov/~lebedev/tec/polya.html
  // Gain fluctuation in proportional counters follows Polya distribution. 
  // x = G/G_0
  // P(m) = m(m(x)**(m-1)*exp(-m*x)/Gamma(m);
  // original Polya  m = 1.5 (R.Bellazzini and M.A.Spezziga, INFN PI/AE-94/02). 
  // Valeri Cherniatin (cherniat@bnlarm.bnl.gov) recomends m=1.38
  // Trs uses x**1.5/exp(x)
  // tss used x**0.5/exp(1.5*x)
  //  mPolya = new TF1("Polya;x = G/G_0;signal","sqrt(x)/exp(1.5*x)",0,10); // original Polya 
  mPolya = new TF1("Polya;x = G/G_0;signal","pow(x,0.38)*exp(-1.38*x)",0,10); //  Valeri Cherniatin
  //   mPoly = new TH1D("Poly","polyaAvalanche",100,0,10);
  //   mPoly->FillRandom("funcP",100000);
  //   delete func;
  // tss
  mGG = new TF1("GaitingGridTransperency","1-6.27594134307865925e+00*TMath::Exp(-2.87987e-01*(x-1.46222e+01))",21,56);
  
  if (Debug()) Print();
#ifdef __ClusterProfile__
  memset (hist, 0, sizeof(hist));
  if (GetTFile()) {
    GetTFile()->cd();
    Int_t color = 1;
    struct Name_t {
      const Char_t *Name;
      const Char_t *Title;
    };
    const Name_t InOut[4] = {
      {"Inner","Inner old electronics"},
      {"Outer","Outer old electronics"},
      {"InnerX","Inner new electronics"},
      {"OuterX","Outer new electronics"}
    };
    const Name_t PadTime[2] = {
      {"Pad","Pad"},
      {"Time","Time"}
    };
    for (Int_t io = 0; io < 4; io++) {
      for (Int_t pt = 0; pt < 2; pt++) {
	TString Name(InOut[io].Name); Name += PadTime[pt].Name; Name += "Mc";
	TString Title(InOut[io].Title); Title += PadTime[pt].Title; Title += "Mc";
	hist[io][pt] = (TProfile2D *) gDirectory->Get(Name);
	if (! hist[io][pt]) {
	  hist[io][pt] = new TProfile2D(Name,Title,nx[pt],xmin[pt],xmax[pt],nz,zmin,zmax,""); 
	  hist[io][pt]->SetMarkerStyle(20);
	  hist[io][pt]->SetMarkerColor(color++);
	}
      }
    }
  }
#endif
  return kStOK;
}
//________________________________________________________________________________
Int_t StTpcRSMaker::Make(){  //  PrintInfo();
  // constants
  static Int_t iBreak = 0;
  static Int_t iSec  = 0, iRow = 0;
  static StTpcCoordinateTransform transform(gStTpcDb);
  Int_t Ndebug = 0, Idebug; // debug printout depth
  if (Debug()%10) {
    if (Debug()%10 > 1) Ndebug = 10;
    gBenchmark->Reset();
    gBenchmark->Start("TpcRS");
    LOG_INFO << "\n -- Begin TpcRS Processing -- \n";
  }
  St_g2t_tpc_hit *g2t_tpc_hit = (St_g2t_tpc_hit *) GetDataSet("geant/g2t_tpc_hit");
  if (!g2t_tpc_hit) return kStWarn;
  Int_t no_tpc_hits       = g2t_tpc_hit->GetNRows();               if (no_tpc_hits<1) return kStOK;
  if (Debug() > 1) g2t_tpc_hit->Print(0,10);
  St_g2t_track *g2t_track = (St_g2t_track *) GetDataSet("geant/g2t_track"); //  if (!g2t_track)    return kStWarn;
  g2t_track_st *tpc_track = 0;
  if (g2t_track) tpc_track = g2t_track->GetTable();
  St_g2t_vertex  *g2t_ver = (St_g2t_vertex *) GetDataSet("geant/g2t_vertex");// if (!g2t_ver)      return kStWarn;
  g2t_vertex_st     *gver = 0;
  if (g2t_ver) gver = g2t_ver->GetTable();
  g2t_tpc_hit_st *tpc_hit_begin = g2t_tpc_hit->GetTable();
  g2t_tpc_hit_st *tpc_hit = tpc_hit_begin;
  // sort 
  TTableSorter sorter(g2t_tpc_hit,&SearchT,&CompareT);//, 0, no_tpc_hits);
  //Float_t amass, charge, tlife; 
  Int_t sortedIndex = 0;
  tpc_hit = tpc_hit_begin;
  for (Int_t sector = 1; sector <= NoOfSectors; sector++) {
    Idebug = 0;
    Int_t NoHitsInTheSector = 0;
    SignalSum_t *SignalSum = ResetSignalSum();
    //    for (; i<=no_tpc_hits; i++,tpc_hit++){// it is assumed that hit are oreder by sector
    while (sortedIndex < no_tpc_hits) {
      Int_t indx = sorter.GetIndex(sortedIndex);
      if (indx < 0) break;
      tpc_hit = tpc_hit_begin + indx;
      Int_t volId = tpc_hit->volume_id%100000;
      Int_t iSector = volId/100;
      if (iSector != sector) {
	if (! ( iSector > sector ) )
	  LOG_ERROR << "StTpcRSMaker::Make: g2t_tpc_hit table has not been ordered by sector no. !" << endm;
	assert( iSector > sector );
	break;
      }
      sortedIndex++;
      if (Debug() && iSec && iSec != sector) continue;
      if (tpc_hit->volume_id <= 0 || tpc_hit->volume_id > 1000000) continue;
      do {
	Int_t iPadrow = volId%100;
	if (Debug() && iRow && iRow != iPadrow) continue;
	Int_t iRdo    = StDetectorDbTpcRDOMasks::instance()->rdoForPadrow(iPadrow);
	
	if ( ! StDetectorDbTpcRDOMasks::instance()->isOn(iSector,iRdo))    continue;
	if ( ! St_tpcAnodeHVavgC::instance()->livePadrow(iSector,iPadrow)) continue;
	Int_t Id         = tpc_hit->track_p;
	Int_t TrackId    = Id;
	Int_t id3 = 0, ipart = 8, charge = 1;
	if (tpc_track) {
	  id3        = tpc_track[Id-1].start_vertex_p;
	  ipart      = tpc_track[Id-1].ge_pid;
	  charge     = (Int_t) tpc_track[Id-1].charge;
	  if (tpc_track[Id-1].next_parent_p && ipart == 3) { // delta electrons ?
	    Id = tpc_track[Id-1].next_parent_p;
	    ipart      = tpc_track[Id-1].ge_pid;
	  }
	}
	if (ipart == Laserino || ipart == Chasrino) {
	  charge = 0;
	} else {
	  if (charge == 0) continue;
	}
	Double_t dStep =  TMath::Abs(tpc_hit->ds);
	Int_t io = 0;
	if (iPadrow > 13) io = 1;
	Int_t ioH = io;
	if (St_TpcAltroParametersC::instance()->N(sector-1) >= 0) ioH += 2;
	Double_t TotalSignal  = 0;
	Double_t tof = 0;
	if (gver) tof = gver[id3-1].ge_tof;
	if (! TESTBIT(m_Mode, kNoToflight)) tof += tpc_hit->tof;
	Double_t lgam = tpc_hit->lgam;
	Double_t gamma = TMath::Power(10.,lgam) + 1;
	Double_t betaGamma = TMath::Sqrt(gamma*gamma - 1.);
	// move up, calculate field at center of TPC
	static Float_t BFieldG[3]; 
	StMagF::Agufld(tpc_hit->x,BFieldG);
	// distortion and misalignment 
	StGlobalCoordinate   xyzG(tpc_hit->x[0],tpc_hit->x[1],tpc_hit->x[2]);
	// replace pxy => direction and try linear extrapolation
	StThreeVectorD       pxyzG(tpc_hit->p[0],tpc_hit->p[1],tpc_hit->p[2]);

	StGlobalDirection    dirG(pxyzG.unit());
	StGlobalDirection    BG(BFieldG[0],BFieldG[1],BFieldG[2]);
	static StTpcLocalCoordinate xyzL;
	transform(xyzG,xyzL,sector,iPadrow);
	static StTpcLocalSectorAlignedCoordinate xyzA;
	static StTpcLocalSectorCoordinate xyzLocalSector;
	transform(xyzL,xyzA);
	transform(xyzA,xyzLocalSector);
	Double_t driftLength = xyzLocalSector.position().z() + tof*gStTpcDb->DriftVelocity(sector); 
	if (driftLength > 250. || driftLength < -1.0) continue;
	xyzLocalSector.position().setZ(driftLength);
	transform(xyzLocalSector,xyzA);
	transform(xyzA,xyzL);
	// Distortions 
	if (TESTBIT(m_Mode, kDistortion) && StMagUtilities::Instance()) {
	  Float_t pos[3] = {xyzL.position().x(), xyzL.position().y(), xyzL.position().z()};
	  Float_t posMoved[3];
	  StMagUtilities::Instance()->DoDistortion(pos,posMoved);   // input pos[], returns posMoved[]
	  StThreeVector<double> postion(posMoved[0],posMoved[1],posMoved[2]);
	  xyzL.setPosition(postion);
	  transform(xyzL,xyzG);
	  tpc_hit->x[0] = xyzG.position().x();
	  tpc_hit->x[1] = xyzG.position().y();
	  tpc_hit->x[2] = xyzG.position().z();
	}
	transform(xyzL,xyzA);
	transform(xyzA,xyzLocalSector);
	// end of distortion
	static StTpcLocalDirection  dirL;
	transform(dirG,dirL,sector,iPadrow);
	static StTpcLocalDirection  BL;
	transform(  BG,  BL,sector,iPadrow);
	static StTpcLocalSectorAlignedDirection  dirA;
	transform(dirL,dirA);
	static StTpcLocalSectorDirection dirLocalSector;
	transform(dirA,dirLocalSector);
	static StTpcLocalSectorAlignedDirection  BA;
	transform(BL,BA);
	static StTpcLocalSectorDirection BLocalSector;
	transform(BA,BLocalSector);
	// Generate signal 
	Double_t padlength = gStTpcDb->PadPlaneGeometry()->innerSectorPadLength();
	Double_t Gain = 0;
	Int_t rowMin = iPadrow;
	Int_t rowMax = iPadrow;
	TF1 *PadResponseFunction = mPadResponseFunctionInner;
	TF1 *ChargeFraction      = mChargeFractionInner;
	Double_t PadPitch        = gStTpcDb->PadPlaneGeometry()->innerSectorPadPitch();
	Double_t sigmaJitterT     = St_TpcResponseSimulatorC::instance()->SigmaJitterTI();
	if(iPadrow > NoOfInnerRows) { // Outer
	  Gain                   = St_tss_tssparC::instance()->gain_out(sector,iPadrow); 
	  rowMin                 = TMath::Max(iPadrow - 1, NoOfInnerRows+1);
	  rowMax                 = TMath::Min(iPadrow + 1, NoOfRows);
	  PadResponseFunction    = mPadResponseFunctionOuter;
	  ChargeFraction         = mChargeFractionOuter;
	  PadPitch               = gStTpcDb->PadPlaneGeometry()->outerSectorPadPitch();
	  padlength              = gStTpcDb->PadPlaneGeometry()->outerSectorPadLength();
	  sigmaJitterT           = St_TpcResponseSimulatorC::instance()->SigmaJitterTO();
	} else {                     // Inner
	  Gain                   = St_tss_tssparC::instance()->gain_in(sector,iPadrow);
	}
	// Extra correction for simulation with respect to data
	Int_t iowe = 0;
	if (sector  > 12) iowe += 4;
	if (iPadrow > 13) iowe += 2;
	Float_t  *Cor = St_TpcResponseSimulatorC::instance()->SecRowCor();
	Double_t GainXCorrectionL = Cor[iowe] + iPadrow*Cor[iowe+1];
	Gain *= TMath::Exp(-GainXCorrectionL);
	static StTpcPadCoordinate Pad;
	transform(xyzLocalSector,Pad,kTRUE,kFALSE); // use T0, don't use Tau
	if (Debug()%10 > 1) {
	  LOG_INFO << "--> tpc_hit: " << indx << "\t"
	       << tpc_hit->volume_id   << "\t"
	       << tpc_hit->de          << "\t"
	       << tpc_hit->ds          << "\t" << endm;
	  PrPP(Make,xyzG); PrPP(Make,xyzL); PrPP(Make,xyzA); PrPP(Make,xyzLocalSector);
	  PrPP(Make,dirG); PrPP(Make,dirL); PrPP(Make,dirA); PrPP(Make,dirLocalSector);
	  PrPP(Make,BG); PrPP(Make,BL); PrPP(Make,BA); PrPP(Make,BLocalSector);
	  PrPP(Make,Pad);
	  PrPP(Make,dirLocalSector); PrPP(Make,dirLocalSector.position()); PrPP(Make,betaGamma);
	}
	// dE/dx correction
	Double_t dEdxCor = 1;
	
	if (m_TpcdEdxCorrection) {
	  dEdxY2_t CdEdx;
	  memset (&CdEdx, 0, sizeof(dEdxY2_t));
	  CdEdx.sector = Pad.sector(); 
	  CdEdx.row    = Pad.row();
	  CdEdx.pad    = TMath::Nint(Pad.pad());
	  Double_t edge = CdEdx.pad;
	  if (edge > 0.5*gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(CdEdx.row)) 
	    edge -= gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(CdEdx.row) + 1;
	  CdEdx.edge = edge;
	  CdEdx.dE     = 1;
	  CdEdx.dx     = dStep;
	  CdEdx.xyz[0] = xyzG.position().x();
	  CdEdx.xyz[1] = xyzG.position().y();
	  CdEdx.xyz[2] = xyzG.position().z();
	  CdEdx.ZdriftDistance = driftLength;
	  St_tpcGas *tpcGas = m_TpcdEdxCorrection->tpcGas();
	  if (tpcGas)
	    CdEdx.ZdriftDistanceO2 = CdEdx.ZdriftDistance*(*tpcGas)[0].ppmOxygenIn;
	  if (! m_TpcdEdxCorrection->dEdxCorrection(CdEdx)) {
	    dEdxCor = CdEdx.dE;
	  }
	  if (dEdxCor <= 0.) continue;
	}
	// Apply Gating Grid
	if (Pad.timeBucket() > mGG->GetXmin() && Pad.timeBucket() < mGG->GetXmax()) {
	  dEdxCor *= mGG->Eval(Pad.timeBucket());
	}
	if (dEdxCor < minSignal) continue;
#ifdef __ClusterProfile__
	Double_t padH = Pad.pad();        
	Double_t tbkH = Pad.timeBucket(); 
        Int_t pad0 = TMath::Nint(padH + xmin[0]);
	Int_t tbk0 = TMath::Nint(tbkH + xmin[1]);
#endif
	Float_t BField[3] = {BLocalSector.position().x(), BLocalSector.position().y(), BLocalSector.position().z()};
	Double_t OmegaTau =St_TpcResponseSimulatorC::instance()->OmegaTau()*BField[2]/5.0;// from diffusion 586 um / 106 um at B = 0/ 5kG
	StPhysicalHelixD track(dirLocalSector.position(),
			       xyzLocalSector.position(),
			       BField[2]*kilogauss,charge);  if (Debug() % 10 > 1) PrPP(Make,track);
	Double_t NP = TMath::Abs(tpc_hit->de/tpc_hit->ds)/(St_TpcResponseSimulatorC::instance()->W()*eV*
							   St_TpcResponseSimulatorC::instance()->Cluster()); // from GEANT
	Double_t s_low = - TMath::Abs(dStep)/2;
	Double_t s_upper = - s_low;
	Double_t dSD    = s_upper - s_low;
	if (driftLength <= 0) {
	  if ((Pad.row() >  13 && driftLength > -gStTpcDb->WirePlaneGeometry()->outerSectorAnodeWirePadPlaneSeparation()) ||
	      (Pad.row() <= 13 && driftLength > -gStTpcDb->WirePlaneGeometry()->innerSectorAnodeWirePadPlaneSeparation())) 
	    driftLength = TMath::Abs(driftLength);
	  else continue; 
	}
	Double_t D = 1. + OmegaTau*OmegaTau;
	Double_t SigmaT = St_TpcResponseSimulatorC::instance()->transverseDiffusion()*  TMath::Sqrt(   driftLength/D);
	//	Double_t SigmaL = St_TpcResponseSimulatorC::instance()->longitudinalDiffusion()*TMath::Sqrt(2*driftLength  );
	Double_t SigmaL = St_TpcResponseSimulatorC::instance()->longitudinalDiffusion()*TMath::Sqrt(   driftLength  );
	if (Debug()%10 > 1) { 	
	  LOG_INFO << "s_low/s_upper/dSD\t" << s_low << "/\t" << s_upper << "/\t" << dSD << endm;
	}
	Double_t GainLocal = Gain/dEdxCor/St_TpcResponseSimulatorC::instance()->NoElPerAdc(); // Account dE/dx calibration
	// end of dE/dx correction
	// generate electrons: No. of primary clusters per cm
	if (TESTBIT(m_Mode, kBICHSEL)) {
	  NP = GetNoPrimaryClusters(betaGamma); // per cm
	  if (NP <= 0.0) {
	    iBreak++; continue;
	  }
	}
	Int_t nP = 0; 
	Double_t xOnWire, yOnWire, zOnWire;
	Double_t newPosition = s_low;
	Double_t dESum = 0;
	Double_t dSSum = 0;
#ifdef __ClusterProfile__
	Double_t padsdE[32]; memset (padsdE, 0, sizeof(padsdE));
	Double_t tbksdE[64]; memset (tbksdE,  0, sizeof(tbksdE));
#endif
	Double_t dESumC = 0;
	Double_t dSSumC = 0;
	Double_t QAvC = 0;
	do {// Clusters
	  Float_t dS = 0;
	  Float_t dE = 0;
	  if (charge) {
	    if (TESTBIT(m_Mode, kPAI)) {
	      mPAI->xNext(betaGamma,dS,dE);
	    }
	    else {
	      dS = - TMath::Log(gRandom->Rndm())/NP;
	      if (TESTBIT(m_Mode, kBICHSEL)) dE = mdNdE->GetRandom();
	      else                           dE = St_TpcResponseSimulatorC::instance()->W()*
		gRandom->Poisson(St_TpcResponseSimulatorC::instance()->Cluster());
	    }
	    dE *= charge*charge;
	  }
	  else { // charge == 0 geantino
	    // for Laserino assume dE/dx = 25 keV/cm;
	    dE = 10; // eV
	    dS = dE*eV/(TMath::Abs(mLaserScale*tpc_hit->de/tpc_hit->ds));
	  }	  
	  dESum += dE;
	  dSSum += dS;
	  dESumC += dE;
	  dSSumC += dS;
	  Double_t xRange = ElectronRange*TMath::Power(dE/ElectronRangeEnergy,ElectronRangePower);
	  Double_t phiXY = 2*TMath::Pi()*gRandom->Rndm();
	  Double_t rX = TMath::Cos(phiXY);
	  Double_t rY = TMath::Sin(phiXY);
	  TRVector xyzRangeL(3, xRange*rX, xRange*rY, 0.);
	  Double_t cxyz[3] = {dirLocalSector.position().x(),dirLocalSector.position().y(),dirLocalSector.position().z()};
	  TRMatrix L2L(3,3, 
		       cxyz[2], - cxyz[0]*cxyz[2], cxyz[0],
		       cxyz[0], - cxyz[1]*cxyz[2], cxyz[1],
		       0.0    , cxyz[0]*cxyz[0] + cxyz[1]*cxyz[1], cxyz[2]);
	  TRVector xyzR(L2L,TRArray::kAxB,xyzRangeL);
	  if (Debug()%10 > 2) {
	    LOG_INFO << "xyzRangeL: " << xyzRangeL << endm;
	    LOG_INFO << "L2L: " << L2L << endm;
	    LOG_INFO << "xyzR: " << xyzR << endm;
	  }
	  Double_t E = dE*eV;
	  if (E > mCutEle) continue; // account delta-electrons
	  nP++;
	  newPosition += dS;
	  if (newPosition > s_upper) break;
	  StThreeVectorD xyzC = track.at(newPosition);
	  if (Debug() > 1) {
	    StThreeVectorD xyzCP = xyzLocalSector.position() + newPosition*dirLocalSector.position();
	    StThreeVectorD diff   = xyzC - xyzCP;
	    if (diff.mag() > 1e-2) {
	      LOG_INFO << "helix approximation: " << xyzC << endm;
	      LOG_INFO << "straight line appro: " << xyzCP << " are different" << endm;
	    }
	    //	if (Debug()) {PrPP(Make,xyzC);}
	  }
	  Int_t n0 = TMath::Nint((dE - St_TpcResponseSimulatorC::instance()->I0())/
				 St_TpcResponseSimulatorC::instance()->W()/(1. - St_TpcResponseSimulatorC::instance()->FanoFactor()));
	  Int_t Nt = gRandom->Binomial(n0, 1. - St_TpcResponseSimulatorC::instance()->FanoFactor()) + 1;
	  Double_t sigmaT = SigmaT;
	  Double_t sigmaL = SigmaL;
	  Double_t QAv = GainLocal*mPolya->GetRandom();
	  QAvC = QAv;
	  for (Int_t ie = 0; ie < Nt; ie++) {
	    if (ie)  QAv = GainLocal*mPolya->GetRandom();
	    // transport to wire
	    Double_t rX, rY;
	    gRandom->Rannor(rX,rY);
	    StTpcLocalSectorCoordinate xyzE(xyzC.x()+xyzR[0]+rX*sigmaT,
					    xyzC.y()+xyzR[1]+rY*sigmaT,
					    xyzC.z()+xyzR[2]+gRandom->Gaus(0,sigmaL), sector, iPadrow);
	    Double_t y = xyzE.position().y();
	    Double_t alphaVariation = InnerAlphaVariation;
	    // Transport to wire
	    if (y < firstInnerSectorAnodeWire || y >  lastOuterSectorAnodeWire) continue;
	    if (y > lastInnerSectorAnodeWire  && y < firstOuterSectorAnodeWire) continue;
	    if (y < lastInnerSectorAnodeWire) {
	      Int_t WireIndex = TMath::Nint((y - firstInnerSectorAnodeWire)/anodeWirePitch);
	      yOnWire = firstInnerSectorAnodeWire + WireIndex*anodeWirePitch;
	    }
	    else {
	      Int_t WireIndex = TMath::Nint((y - firstOuterSectorAnodeWire)/anodeWirePitch);
	      yOnWire = firstOuterSectorAnodeWire + WireIndex*anodeWirePitch;
	      alphaVariation = OuterAlphaVariation;
	    }
	    Double_t distanceToWire = y - yOnWire; // Calculated effective distance to wire affected by Lorentz shift 
	    xOnWire = xyzE.position().x();
	    zOnWire = xyzE.position().z();
	    // Grid plane (1 mm spacing) focusing effect + Lorentz angle in drift volume 
	    Int_t iGridWire = (Int_t ) TMath::Abs(10.*distanceToWire);
	    Double_t dist2Grid = TMath::Sign(0.05 + 0.1*iGridWire, distanceToWire); // [cm]
#if 0
	    // Focusing on Grid wires  => Effectively accounted by focusing on Anode wires
	    Double_t yShift   = dist2Grid - distanceToWire;
	    xOnWire -= yShift*OmegaTau;
	    zOnWire += TMath::Abs(yShift);
#endif
	    // Ground plane (1 mm spacing) focusing effect
	    Int_t iGroundWire = (Int_t ) TMath::Abs(10.*dist2Grid);
	    Double_t distFocused = TMath::Sign(0.05 + 0.1*iGroundWire, dist2Grid);
	    // OmegaTau near wires taken from comparison with data
	    Double_t tanLorentz = OmegaTau/St_TpcResponseSimulatorC::instance()->OmegaTauScaleO(); 
	    if (y < firstOuterSectorAnodeWire) tanLorentz = OmegaTau/St_TpcResponseSimulatorC::instance()->OmegaTauScaleI(); 
	    xOnWire += distFocused*tanLorentz; // tanLorentz near wires taken from comparison with data
	    zOnWire += TMath::Abs(distFocused);
	    if (! iGroundWire ) QAv *= TMath::Exp( alphaVariation);
	    else                QAv *= TMath::Exp(-alphaVariation);

	    for(Int_t row = rowMin; row <= rowMax; row++) {              
	      StTpcLocalSectorCoordinate xyzW(xOnWire, yOnWire, zOnWire, sector, row);
	      transform(xyzW,Pad,kTRUE,kFALSE); // use T0, don't use Tau
	      Float_t bin = Pad.timeBucket();
	      Int_t binT = TMath::Nint(bin);
	      if (binT < 0 || binT >= NoOfTimeBins) continue;
	      Double_t dT = bin -  binT;
	      if (sigmaJitterT) dT += gRandom->Gaus(0,sigmaJitterT);
	      Double_t dely[1]           = {transform.yFromRow(row)-yOnWire};            
	      Double_t localYDirectionCoupling = ChargeFraction->GetSave(dely);
	      if(localYDirectionCoupling < minSignal) continue;
	      // pre calculate time bin response
	      TF1 *mShaperResponse = mShaperResponses[io][sector-1];
	      Double_t tbksdEonPad[64]; memset (tbksdEonPad,  0, sizeof(tbksdEonPad));
	      Double_t dt = dT;
	      Int_t bin_low  = TMath::Max(0             ,binT + TMath::Nint(dt+mShaperResponse->GetXmin()-0.5));
	      Int_t bin_high = TMath::Min(NoOfTimeBins-1,binT + TMath::Nint(dt+mShaperResponse->GetXmax()+0.5));
	      Int_t NTimeBins = 0;
	      for(Int_t itbin = 0; itbin <= bin_high-bin_low; itbin++){
		Double_t t = -dt + itbin;
		tbksdEonPad[itbin] = QAv*localYDirectionCoupling*mShaperResponse->GetSave(&t);
		NTimeBins++;
		if (itbin > binT - bin_low && tbksdEonPad[itbin] < minSignal) break;
	      }
	      Float_t padX = Pad.pad();
	      Int_t CentralPad = TMath::Nint(padX);
	      if (CentralPad < 1) continue;
	      Int_t PadsAtRow = gStTpcDb->PadPlaneGeometry()->numberOfPadsAtRow(row);
	      if(CentralPad > PadsAtRow) continue;
	      Int_t DeltaPad = TMath::Nint(PadResponseFunction->GetXmax());
	      Int_t padMin = TMath::Max(CentralPad - DeltaPad ,1);
	      Int_t padMax = TMath::Min(CentralPad + DeltaPad ,PadsAtRow);
	      //	      Double_t xPad = padMin - padX;
	      for(Int_t pad = padMin; pad <= padMax; pad++) {
		Double_t gain = 1;
		if (! TESTBIT(m_Mode, kGAINOAtALL)) { 
		  Double_t gain   = St_tpcPadGainT0C::instance()->Gain(sector,row,pad);
		  if (gain <= 0.0) continue;
		}
		Double_t xPad = pad - padX;
		Double_t xpad[1] = {xPad};
		Double_t localXDirectionCoupling = gain*PadResponseFunction->GetSave(xpad);
		if (localXDirectionCoupling < minSignal) continue;
		Double_t XYcoupling = localYDirectionCoupling*localXDirectionCoupling;
		if(XYcoupling < minSignal)  continue;
		Int_t index = NoOfTimeBins*((row-1)*NoOfPads+pad-1)+bin_low;
		for(Int_t itbin = 0; itbin <= NTimeBins; itbin++, index++){
		  Double_t signal = localXDirectionCoupling*tbksdEonPad[itbin];
		  TotalSignal += signal;
		  SignalSum[index].Sum += signal;
#ifdef __ClusterProfile__
		  Int_t Itbin = itbin + bin_low - tbk0;
		  Int_t Ipad  = pad - pad0;
		  if (Ipad  >= 0 && Ipad < 32 && 
		      Itbin >= 0 && Itbin < 64) {
		    padsdE[Ipad]  += signal;
		    tbksdE[Itbin] += signal;
		  }
#endif
		  if ( TrackId ) {
		    if (! SignalSum[index].TrackId ) SignalSum[index].TrackId = TrackId;
		    else  // switch TrackId, works only for 2 tracks, more tracks ?
		      if ( SignalSum[index].TrackId != TrackId && SignalSum[index].Sum < 2*signal) 
			SignalSum[index].TrackId = TrackId;
		  }
#if 1
		  if (Debug() > 1) {
		    LOG_INFO << "simu R/P/T/I = " << row << " /\t" << pad << " /\t" << itbin << " /\t" << index 
			 << "\tSum/Adc/TrackId = " << SignalSum[index].Sum << " /\t" 
			 << SignalSum[index].Adc << " /\t" << SignalSum[index].TrackId << endm;
		  }
#endif
		} // time 
	      } // pad limits
	    } // row limits
	  }  // electrons in Cluster
	} while (kTRUE); // Clusters
	tpc_hit->lgam = -99;
	if (dESum > 0 && dSSum) {
	  tpc_hit->de = dESum*eV; tpc_hit->ds = dSSum; tpc_hit->lgam = TotalSignal;
#ifdef __ClusterProfile__
	  if (hist[ioH][0]) {
	    Double_t S = 0;
	    Double_t pax = 0;
	    Int_t  nActivePads = 0;
	    for (Int_t p = 0; p < 32; p++) {
	      if (padsdE[p] < minSignal) continue;
	      nActivePads++;
	      S += padsdE[p]; pax += (p+pad0)*padsdE[p];
	    }
	    if (S > 0) {
	      for (Int_t p = 0; p < 32; p++) hist[ioH][0]->Fill((p+pad0)-padH,xyzL.position().z(),padsdE[p]/S);
	      pax /= S;
	      pax -= padH;
	      //	      if (Debug() > 1 && (TMath::Abs(pax) > 0.5 || nActivePads <= 2)) {
	      if (Debug() > 1 && (nActivePads <= 2)) {
		LOG_INFO << "padH/pax = " << padH << "/" << pax << endm;
		for (Int_t p = 0; p < 32; p++) {
		  if (padsdE[p] > 0) {
		    LOG_INFO << "\t" << p << "\t" << padsdE[p]/S;
		  }
		}
		LOG_INFO << endm;
		iBreak++;
	      }
	    }
	  }								                          
	  if (hist[ioH][1]) {						                          
	    Double_t S = 0;
	    for (Int_t t = 0; t < 64; t++) S += tbksdE[t];
	    if (S > 0) {
	      for (Int_t t = 0; t < 64; t++) hist[ioH][1]->Fill((t+tbk0+0.5)-tbkH,xyzL.position().z(),tbksdE[t]/S);
	    }
	  }
#endif
	}
	NoHitsInTheSector++;
      } while (0); // end do loop
    } // hit in the sector
    if (NoHitsInTheSector) {
      DigitizeSector(sector);   
      if (Debug()) LOG_INFO << "StTpcRSMaker: Done with sector\t" << sector << " total no. of hit = " << NoHitsInTheSector << endm;
    }
  } // sector
  if (Debug()%10) gBenchmark->Show("TpcRS");
  return kStOK;
}
//________________________________________________________________________________
Double_t StTpcRSMaker::GetNoPrimaryClusters(Double_t betaGamma) {
  if (! mdNdx) return 0;
  Int_t bin = mdNdx->FindBin(betaGamma);
  if (bin <= 0) bin = 1;
  if (bin > mdNdx->GetNbinsX()) bin = mdNdx->GetNbinsX();
  return mdNdx->GetBinContent(bin);
}
//________________________________________________________________________________
Double_t StTpcRSMaker::ShaperFunc(Double_t *x, Double_t *par) {
  Double_t tau = par[0];
  Double_t width = par[1];
  Double_t p = par[2];
  Double_t t = x[0]*width/tau;
  Double_t Delta = width/tau;
  Double_t t1 = t - Delta/2.;
  Double_t t2 = t1 + Delta;
  Double_t val = TMath::Gamma(p,t2) - TMath::Gamma(p,t1);
  return val;
}
//________________________________________________________________________________
Double_t StTpcRSMaker::PadResponseFunc(Double_t *x, Double_t *par) {
  Double_t CrossTalk = 0;
  Double_t Value = 0;
  Double_t X = par[5]*x[0];
  if (CrossTalk > 0) {
    for (Int_t i = -1; i <= 1; i++) {
      Double_t xx = X + par[5]*i;
      if (i == 0) Value += (1. - 2.*CrossTalk)*Gatti(&xx,par);
      else        Value +=          CrossTalk *Gatti(&xx,par);
    }
  } else   Value = Gatti(&X,par);
  return Value;
}
//________________________________________________________________________________
Double_t StTpcRSMaker::Gatti(Double_t *x, Double_t *par) {
  /************************************************************************
   *  Function    : generates the cathode signal using                    *
   *                the single-parameter Gatti formula:                   *
   *                              1 - tanh(K2 * lambda)**2                *
   *     GFunc(lambda) = K1 * -------------------------------             *
   *                           1 + K3 * tanh (K2 *lambda)**2              *
   *     lambda = x/h, h is anode cathode spacing                         *
   *                                                                      *
   *     K2 = pi/2*(1 - 0.5*sqrt(K3))                                     *
   *                                                                      *
   *              K2*sqrt(K3)                                             *
   *     K1 = -------------------                                         *
   *            4 * atan(sqrt(K3))                                        *
   *                                                                      *
   *  References  : E.Gatti, A.Longoni, NIM 163 (1979) 82-93.             *
   *  Authors : V.Balagura,V.Cherniatin,A.Chikanian                       *
   ************************************************************************/
  Double_t y = x[0];   // distance to center of strip [cm]
  Double_t w = par[0]; // w = width of pad       
  Double_t h = par[1]; // h = Anode-Cathode gap  
  Double_t K3  = par[3];
  Double_t lambda = y/h;
  Double_t K2 = TMath::PiOver2()*(1. - 0.5*TMath::Sqrt(K3));  
  //  Double_t K1 = K2*TMath::Sqrt(K3)/(2*TMath::ATan(TMath::Sqrt(K3)));
  Double_t sqK3 = TMath::Sqrt(K3);
  Double_t ATsqK3 = 0.5/TMath::ATan(sqK3);
  Double_t Y1 = lambda - w/h/2;
  Double_t Y2 = Y1 + w/h;
  Double_t X1 = K2*Y1;
  Double_t X2 = K2*Y2;
  Double_t Z1 = sqK3*TMath::TanH(X1);
  Double_t Z2 = sqK3*TMath::TanH(X2);
  Double_t val = ATsqK3*(TMath::ATan(Z2) - TMath::ATan(Z1));
  return val;
}
//________________________________________________________________________________
void  StTpcRSMaker::Print(Option_t *option) const {
  PrPP(Print, NoOfSectors);
  PrPP(Print, NoOfRows);
  PrPP(Print, NoOfInnerRows);
  PrPP(Print, NoOfPads);
  PrPP(Print, St_TpcResponseSimulatorC::instance()->W());// = 26.2);//*eV
  PrPP(Print, St_TpcResponseSimulatorC::instance()->Cluster());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->longitudinalDiffusion());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->transverseDiffusion());
  //  PrPP(Print, Gain);
  PrPP(Print, NoOfTimeBins);
  PrPP(Print, numberOfInnerSectorAnodeWires); 
  PrPP(Print, firstInnerSectorAnodeWire);
  PrPP(Print, lastInnerSectorAnodeWire);
  PrPP(Print, numberOfOuterSectorAnodeWires);
  PrPP(Print, firstOuterSectorAnodeWire);
  PrPP(Print, lastOuterSectorAnodeWire);
  PrPP(Print, anodeWirePitch);
  PrPP(Print,St_TpcResponseSimulatorC::instance()->OmegaTau()); // tan of Lorentz angle
  PrPP(Print, St_TpcResponseSimulatorC::instance()->NoElPerAdc());
  PrPP(Print, anodeWireRadius);
  PrPP(Print, St_TpcResponseSimulatorC::instance()->AveragePedestal());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->AveragePedestalRMS());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->AveragePedestalRMSX());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->FanoFactor());
  PrPP(Print, innerSectorAnodeVoltage);
  PrPP(Print, outerSectorAnodeVoltage);
  PrPP(Print, St_TpcResponseSimulatorC::instance()->K3IP());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->K3IR());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->K3OP());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->K3OR());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->SigmaJitterTI());
  PrPP(Print, St_TpcResponseSimulatorC::instance()->SigmaJitterTO());
}
//________________________________________________________________________________
void  StTpcRSMaker::DigitizeSector(Int_t sector){
  //  static Int_t PedestalMem[__MaxNumberOfTimeBins__];
  TDataSet *event = GetData("Event");
  StTpcRawData *data = 0;
  if (! event) {
    data = new StTpcRawData(NoOfSectors);
    event = new TObjectSet("Event", data);
    AddData(event);
  } else data = (StTpcRawData *) event->GetObject();
  assert(data);
  SignalSum_t *SignalSum = GetSignalSum();
  Double_t ped    = 0; 
  Double_t pedRMS = St_TpcResponseSimulatorC::instance()->AveragePedestalRMS();
  Int_t itpc = 0;
  if (St_TpcAltroParametersC::instance()->N(sector-1) >= 0) {
    pedRMS = St_TpcResponseSimulatorC::instance()->AveragePedestalRMSX();
    itpc = 1;
  }
  Int_t adc = 0;
  Int_t index = 0;
  Double_t gain = 1;
  Int_t row, pad, bin;
  Int_t Sector = TMath::Abs(sector);
  StTpcDigitalSector *digitalSector = data->GetSector(Sector);
  if (! digitalSector) {
    digitalSector = new StTpcDigitalSector();
    data->setSector(Sector,digitalSector);
  } else 
    digitalSector->clear();
  for (row = 1;  row <= NoOfRows; row++) {
    Int_t NoOfPadsAtRow = StTpcDigitalSector::numberOfPadsAtRow(row);
    Int_t io = 0;
    if (row > 13) io = 1;
    for (pad = 1; pad <= NoOfPadsAtRow; pad++) {
      gain = St_tpcPadGainT0C::instance()->Gain(Sector,row,pad);
      if (gain <= 0.0) continue;
      ped    = St_TpcResponseSimulatorC::instance()->AveragePedestal();
      static  Short_t ADCs[__MaxNumberOfTimeBins__];
      static UShort_t IDTs[__MaxNumberOfTimeBins__];
      memset(ADCs, 0, sizeof(ADCs));
      memset(IDTs, 0, sizeof(IDTs));
      Int_t NoTB = 0;
      index = NoOfTimeBins*((row-1)*NoOfPads+pad-1);
      for (bin = 0; bin < NoOfTimeBins; bin++,index++) {
	//	Int_t index= NoOfTimeBins*((row-1)*NoOfPads+pad-1)+bin;
	// Digits : gain + ped
	//  GG TF1 *ff = new TF1("ff","TMath::Sqrt(4.76658e+01*TMath::Exp(-2.87987e-01*(x-1.46222e+01)))",21,56)
	Double_t pRMS = pedRMS;
#if 0
	if (bin >= 21 && bin <= 56) {
	  pRMS = TMath::Sqrt(pedRMS*pedRMS + 4.76658e+01*TMath::Exp(-2.87987e-01*(bin-1.46222e+01)));
	}
#endif
	if (pRMS > 0) {
	  adc = (Int_t) (SignalSum[index].Sum/gain + gRandom->Gaus(ped,pRMS));
	  adc = adc - (int) ped;
	}
	else            adc = (Int_t) (SignalSum[index].Sum/gain);
	if (adc > 1023) adc = 1023;
	if (adc < 1) continue;
	SignalSum[index].Adc = adc;
	NoTB++;
	ADCs[bin] = adc;
	IDTs[bin] = SignalSum[index].TrackId;
#if 0
	if (Debug() > 1) {
	  LOG_INFO << "digi R/P/T/I = " << row << " /\t" << pad << " /\t" << bin << " /\t" << index 
	       << "\tSum/Adc/TrackId = " << SignalSum[index].Sum << " /\t" 
	       << SignalSum[index].Adc << " /\t" << SignalSum[index].TrackId << endm;
	}
#endif
      }
      if (! NoTB) continue;
      if (St_TpcAltroParametersC::instance()->N(sector-1) >= 0 && ! mAltro) {
	mAltro = new Altro(__MaxNumberOfTimeBins__,ADCs);
	if (St_TpcAltroParametersC::instance()->N(sector-1) > 0) {// Tonko 06/25/08
	  //      ConfigAltro(ONBaselineCorrection1, ONTailcancellation, ONBaselineCorrection2, ONClipping, ONZerosuppression)
	  mAltro->ConfigAltro(                    0,                  1,                     0,          1,                 1); 
	  //       ConfigBaselineCorrection_1(int mode, int ValuePeDestal, int *PedestalMem, int polarity)
	  //altro->ConfigBaselineCorrection_1(4, 0, PedestalMem, 0);  // Tonko 06/25/08
	  Int_t *altro_reg = St_TpcAltroParametersC::instance()->altro_reg(sector-1);
	  mAltro->ConfigTailCancellationFilter(altro_reg[0],altro_reg[1],altro_reg[2], // K1-3
					       altro_reg[3],altro_reg[4],altro_reg[5]);// L1-3
	} else {
	  mAltro->ConfigAltro(0,0,0,1,1); 
	}
	mAltro->ConfigZerosuppression(St_asic_thresholds_tpxC::instance()->thresh_lo(),
				      St_asic_thresholds_tpxC::instance()->n_seq_lo(),
				      0,0);
	mAltro->PrintParameters();
      }
      if (mAltro) {
	//#define PixelDUMP
#ifdef PixelDUMP
	static Short_t ADCsSaved[__MaxNumberOfTimeBins__];
	memcpy(ADCsSaved, ADCs,sizeof(ADCsSaved));
#endif
	mAltro->RunEmulation();
#ifdef PixelDUMP
	ofstream *out = new ofstream("digi.dump",ios_base::app);
	for (Int_t i = 0; i < __MaxNumberOfTimeBins__; i++) {
	  if (ADCsSaved[i] > 0 || ADCs[i] > 0) {
	    LOG_INFO << Form("s %2i r %i p %3i t %3i: %10i => %10i keep %10i",sector,row,pad,i,ADCsSaved[i],ADCs[i],mAltro->ADCkeep[i]) << endm;
	    *out << Form("s %2i r %i p %3i t %3i: %10i => %10i keep %10i",sector,row,pad,i,ADCsSaved[i],ADCs[i],mAltro->ADCkeep[i]) << endm;
	  }
	}
	delete out;
#endif
      }
      else {
	if (St_TpcAltroParametersC::instance()->N(sector-1) < 0) NoTB = AsicThresholds(ADCs);
      }
      if (NoTB && digitalSector) {
	digitalSector->putTimeAdc(row,pad,ADCs,IDTs);
      }
    }
  }
}
//________________________________________________________________________________
Int_t StTpcRSMaker::AsicThresholds(Short_t ADCs[__MaxNumberOfTimeBins__]) {
  Int_t t1 = 0;
  Int_t nSeqLo = 0;
  Int_t nSeqHi = 0;
  Int_t noTbleft = 0;
  for (UInt_t tb = 0; tb < __MaxNumberOfTimeBins__; tb++) {
    if (ADCs[tb] <= St_asic_thresholdsC::instance()->thresh_lo()) {
      if (! t1) ADCs[tb] = 0;
      else {
	if (nSeqLo <= St_asic_thresholdsC::instance()->n_seq_lo() ||
	    nSeqHi <= St_asic_thresholdsC::instance()->n_seq_hi()) 
	  for (UInt_t t = t1; t <= tb; t++) ADCs[t] = 0;
	else noTbleft += nSeqLo;
      }
      t1 = nSeqLo = nSeqHi = 0;
    }
    nSeqLo++; 
    if (! t1) t1 = tb;
    if (ADCs[tb] > St_asic_thresholdsC::instance()->thresh_hi()) {nSeqHi++;}
  }
  return noTbleft;
}
//________________________________________________________________________________
Double_t StTpcRSMaker::InducedCharge(Double_t s, Double_t h, Double_t ra, Double_t Va, Double_t &t0) {
  // Calculate variation of induced charge due to different arrived angles 
  // alpha = -26 and -70 degrees
  LOG_INFO << "wire spacing = " << s << " cm"
       << "\tcathode anode gap = " << h << " cm"
       << "\tanode wire radius = " << ra << " cm"
       << "\tpotential on anode wire = " << Va << " V" << endm;
  const Double_t B  = 30e-3; // 1/V
  const Double_t E0 = 20e3; // V/cm
  const Double_t mu = 2.26; // cm**2/V/sec CH4+ mobility 
  // const Double_t mu = 1.87; // cm**2/V/sec Ar+ mobility 
  Double_t alpha[2] = {-26., -70.};
  Double_t pi = TMath::Pi();
  // E.Mathieson (3.2b), V.Chernyatin said that it should be used this (Weber ) approximation 07/09/08
  Double_t rc = s/(2*pi)*TMath::Exp(pi*h/s); LOG_INFO << "rc(Cylinder approx) = " << rc << " cm" << endm; 
  //  Double_t rc = 4*h/pi; LOG_INFO << "rc = " << rc << " cm" << endm;   // E.Mathieson (4.3), no valid for our case
  Double_t C  = 1./(2*TMath::Log(rc/ra)); LOG_INFO << "C = " << C << endm;
  Double_t E  = 2*pi*C*Va/s; LOG_INFO << "E = " << E << " V/cm" << endm;
  // Gain variation: M = M0*(1 - k*cos(2*alpha))
  Double_t k = 2*B/3.*TMath::Power((pi/E0/s),2)*TMath::Power(C*Va,3); LOG_INFO << "k = " << k << endm;
  // Induced charge variation
  t0 = ra*ra/(4*mu*C*Va); 
#if 0
  if (h/s < 0.8) t0 *= 0.5;
#endif
  LOG_INFO << "t0 = " << 1e9*t0 << " ns" << endm;                                     // E.Mathieson (2.10)
  Double_t Tav = t0*h/s/(2*pi*C);  LOG_INFO << "Tav = " << 1e9*Tav << " ns" << endm;
  //  Double_t t = 5*55e-9;             LOG_INFO << "t = " << 1e9*t << " ns" << endm;
  Double_t t = 180e-9;             LOG_INFO << "t = " << 1e9*t << " ns" << endm; 
  Double_t rp = TMath::Sqrt(1. + t/t0); LOG_INFO << "r' = " << rp << endm;
  // qc = rp*ra*sin(alpha)/(2*h) + C/2*log(1 + t/t0) = A*sin(alpha) + B
  Double_t Aconstant = rp*ra/(2*h);        LOG_INFO << "Aconstant = " << Aconstant << endm;
  Double_t Bconstant = C/2*TMath::Log(1 + t/t0); LOG_INFO << "Bconstant = " << Bconstant << endm;
  Double_t Gains[2];
  for (Int_t i = 0; i < 2; i++) {
    Gains[i] = Aconstant*TMath::Sin(pi/180*alpha[i]) + Bconstant; 
    LOG_INFO << "Gain = " << Gains[i] << " at alpha = " << alpha[i] << " degree" << endm;
  }
  Double_t GainsAv = TMath::Sqrt(Gains[0]*Gains[1]);
  Double_t r = 0;
  for (Int_t i = 0; i < 2; i++) {
    r = TMath::Log(Gains[i]/GainsAv); LOG_INFO << "Relative gain " << r << " at alpha = " << alpha[i] << endm;
  }
  return r;
}
//________________________________________________________________________________
Int_t StTpcRSMaker::SearchT(const void *elem1, const void **elem2) { 
  g2t_tpc_hit_st *value1 = (g2t_tpc_hit_st *) elem1;    
  g2t_tpc_hit_st *value2 = (g2t_tpc_hit_st *) *elem2;   
  if (value1->volume_id%100000 != value2->volume_id%100000) return value1->volume_id%100000 - value2->volume_id%100000;
  if (value1->track_p          != value2->track_p) return value1->track_p - value2->track_p;
  return (Int_t) ((value1->x[2] - value2->x[2])/4.); // within 4 cm
}
//________________________________________________________________________________
Int_t StTpcRSMaker::CompareT(const void **elem1, const void **elem2) {
  return SearchT(*elem1, elem2);
}                                                     
#if 0
//________________________________________________________________________________
Double_t StTpcRSMaker::DriftLength(Double_t x, Double_t y) {
  static const Double_t Step = 5e-2;
  Double_t r = TMath::Sqrt(x*x + y*y);
  if (r < 0.25) return r;
  x = TMath::Abs(x);
  y = TMath::Abs(y);
  Int_t Nstep = 0;
  while (x > Step || y > Step) {
    Double_t Slope = TMath:SinH(TMath::Pi()*y/s)/TMath:Sin(TMath::Pi()*x/s);
    Double_t Co2 = 1./(1. + Slope*Slope);
    Double_t Si  = TMath::Sqrt(1. - Co2);
    Double_t Co  = TMath::Sqrt(Co2);
    x = TMath::Abs(x - Step*Co);
    y = TMath::Abs(y - Step*Si);
    NStep++
  }
  return NStep*Step;
}
#endif
//________________________________________________________________________________
Double_t StTpcRSMaker::fei(Double_t t, Double_t t0, Double_t T) {
  static const Double_t xmaxt = 708.39641853226408;
  static const Double_t xmax  = xmaxt - TMath::Log(xmaxt);
  Double_t t01 = xmax, t11 = xmax;
  if (T > 0) {t11 = (t+t0)/T;}
  if (t11 > xmax) t11 = xmax;
  if (T > 0) {t01 = t0/T;}
  if (t01 > xmax) t01  = xmax;
  return TMath::Exp(-t11)*(ROOT::Math::expint(t11) - ROOT::Math::expint(t01));
}
//________________________________________________________________________________
Double_t StTpcRSMaker::shapeEI(Double_t *x, Double_t *par) {// does not work. It is needed to 1/s
  Double_t t  = x[0];
  Double_t value = 0;
  if (t <= 0) return value;
  Double_t t0    = par[0];
  Double_t T1 = par[1]; // tau_I
  Double_t T2 = par[3]; // tau_C
  if (TMath::Abs((T1-T2)/(T1+T2)) < 1e-7) {
    return TMath::Max(0.,(t + t0)/T1*fei(t,t0,T1) + TMath::Exp(-t/T1) - 1);
  } 
  if (T2 <= 0) return fei(t,t0,T1);
  if (T1 <= 0) return 0;
  return T1/(T1 - T2)*(fei(t,t0,T1) - fei(t,t0,T2));
}
//________________________________________________________________________________
Double_t StTpcRSMaker::shapeEI3(Double_t *x, Double_t *par) {// does not work. It is needed to 1/s
  Double_t t  = x[0];
  Double_t value = 0;
  if (t <= 0) return value;
  Double_t t0    = par[0];
  Double_t tau_F = par[1];
  Double_t tau_P = par[2];
  Double_t tau_I = par[3];
  Double_t tau_C = par[5];
  Double_t d =   1./tau_P;
  Double_t a[3] = {- 1./tau_I, - 1./tau_F, 0};
  Double_t A[3] = {(a[0]+d)/(a[0]-a[1]), (a[1]+d)/(a[1]-a[0]), 0};
  Int_t N = 2;
  if (tau_C > 0) {
    N = 3;
    a[2] = -1./tau_C;
    A[0] = (a[0] + d)/a[0]/(a[0] - a[1])/(a[0] - a[2]);
    A[1] = (a[1] + d)/a[1]/(a[1] - a[0])/(a[1] - a[2]);
    A[2] = (a[2] + d)/a[2]/(a[2] - a[0])/(a[2] - a[1]); 
  }
  for (Int_t i = 0; i < N; i++) {
    value += A[i]*TMath::Exp(a[i]*(t+t0))*(ROOT::Math::expint(-a[i]*(t+t0))-ROOT::Math::expint(-a[i]*t0));
  }
  return value;
}
//________________________________________________________________________________
Double_t StTpcRSMaker::shapeEI_I(Double_t *x, Double_t *par) { //Integral of shape over time bin
  static Double_t sqrt2 = TMath::Sqrt(2.);
  Double_t TimeBinWidth = par[2];
  Double_t norm = par[3];
  Double_t t1 = TimeBinWidth*(x[0] - 0.5);
  Double_t t2 = t1 + TimeBinWidth;
  Int_t io = (Int_t) par[4];
  assert(io >= 0 && io <= 1);
  return sqrt2*fgTimeShape0[io]->Integral(t1,t2)/norm;
}
//________________________________________________________________________________
Double_t StTpcRSMaker::shapeEI3_I(Double_t *x, Double_t *par) { //Integral of shape over time bin
  static Double_t sqrt2 = TMath::Sqrt(2.);
  Double_t TimeBinWidth = par[4];
  Double_t norm = par[5];
  Double_t t1 = TimeBinWidth*(x[0] - 0.5);
  Double_t t2 = t1 + TimeBinWidth;
  Int_t io = (Int_t) par[6];
  assert(io >= 0 && io <= 1);
  return sqrt2*fgTimeShape3[io]->Integral(t1,t2)/norm;
}
//________________________________________________________________________________
SignalSum_t  *StTpcRSMaker::GetSignalSum() {
  if (! m_SignalSum) 
    m_SignalSum = (SignalSum_t  *) malloc(NoOfRows*NoOfPads*NoOfTimeBins*sizeof(SignalSum_t)); 
  return m_SignalSum;
}
//________________________________________________________________________________
SignalSum_t  *StTpcRSMaker::ResetSignalSum() {
  GetSignalSum();
  memset (m_SignalSum, 0, NoOfRows*NoOfPads*NoOfTimeBins*sizeof(SignalSum_t));
  return m_SignalSum;
}
#undef PrPP
//________________________________________________________________________________
// $Id: StTpcRSMaker.cxx,v 1.36 2010/04/20 13:56:24 fisyak Exp $
// $Log: StTpcRSMaker.cxx,v $
// Revision 1.36  2010/04/20 13:56:24  fisyak
// Switch off __ClusterProfile__
//
// Revision 1.35  2010/04/16 19:29:35  fisyak
// W is in eV now
//
// Revision 1.34  2010/04/01 22:17:06  fisyak
// Add checking for TPC is switched off at all and stop if so
//
// Revision 1.33  2010/03/22 23:45:05  fisyak
// Freeze version with new parameters table
//
// Revision 1.32  2010/03/16 19:41:46  fisyak
// Move diffusion and sec/row correction in DB, clean up
//
// Revision 1.31  2010/03/02 21:10:27  fisyak
// Make aware about TpcRDOMasks
//
// Revision 1.30  2010/02/26 18:53:33  fisyak
// Take longitudinal Diffusion from Laser track fit, add Gating Grid
//
// Revision 1.29  2010/02/16 00:21:23  fisyak
// Speed up by a factor 3.5 by ignoring individual pad T0
//
// Revision 1.28  2010/01/26 19:47:25  fisyak
// Include dE/dx calibration and distortions in the simulation
//
// Revision 1.27  2009/11/25 21:32:52  fisyak
// Comment out cluster profile histograms
//
// Revision 1.26  2009/11/03 22:38:53  fisyak
// Freeze version rcf9108.J
//
// Revision 1.25  2009/10/30 21:12:00  fisyak
// Freeze version rcf9108.F, Clean up
//
// Revision 1.24  2009/10/26 18:50:58  fisyak
// Clean up from Bichel's stuff
//
// Revision 1.23  2009/10/12 23:54:12  fisyak
// Restore T0Jitter, remove differential in Tpx signal
//
// Revision 1.22  2009/10/03 21:29:09  fisyak
// Clean up, move all TpcT related macro into StTpcMcAnalysisMaker
//
// Revision 1.21  2009/10/01 14:53:06  fisyak
// Add T0Jitter
//
// Revision 1.20  2009/09/27 01:30:48  fisyak
// Restate T0Jitter
//
// Revision 1.19  2009/09/27 01:24:58  fisyak
// Restate T0Jitter
//
// Revision 1.18  2009/09/21 13:20:39  fisyak
// Variant O4, no mSigmaJitter, 100 keV
//
// Revision 1.17  2009/09/01 15:06:44  fisyak
// Version N
//
// Revision 1.16  2009/08/25 20:39:40  fisyak
// Variant K
//
// Revision 1.15  2009/08/25 15:45:58  fisyak
// Version J
//
// Revision 1.14  2009/08/24 20:16:41  fisyak
// Freeze with new Altro parameters
//
// Revision 1.13  2008/12/29 15:24:54  fisyak
// Freeze ~/WWW/star/Tpc/TpcRS/ComparisonMIP31
//
// Revision 1.12  2008/12/18 23:06:37  fisyak
// Take care about references to TGiant
//
// Revision 1.11  2008/12/12 21:41:41  fisyak
// Freeze
//
// Revision 1.10  2008/10/06 19:10:23  fisyak
// BichlePPMIP3
//
// Revision 1.9  2008/10/03 20:25:29  fisyak
// Version BichselMIP2
//
// Revision 1.8  2008/08/19 16:01:15  fisyak
// Version 21
//
// Revision 1.7  2008/08/18 15:54:25  fisyak
// Version 20
//
// Revision 1.6  2008/07/30 23:53:19  fisyak
// Freeze
//
// Revision 1.5  2008/07/18 16:22:50  fisyak
// put a factor 2.5 for tauIntegration
//
// Revision 1.3  2008/06/25 20:02:32  fisyak
// The first set of parametrs for Altro, Remove gains for the moment
//
// Revision 1.2  2008/06/19 22:45:43  fisyak
// Freeze problem with TPX parameterization
//
// Revision 1.1.1.1  2008/04/28 14:39:47  fisyak
// Start new Tpc Response Simulator
//
// Revision 1.20  2008/04/24 10:42:03  fisyak
// Fix binning issues
//
// Revision 1.19  2008/04/04 15:00:11  fisyak
// Freeze before shaper modifications
//
// Revision 1.18  2005/02/07 21:40:31  fisyak
// rename antique TGeant3 to TGiant3
//
// Revision 1.17  2005/01/26 23:28:38  fisyak
// Check boundary for sorted tpc_hit array
//
// Revision 1.16  2005/01/26 21:45:31  fisyak
// Freeze correction made in June
//
// Revision 1.14  2004/06/04 17:09:02  fisyak
// Change tau in Chaper and OmegaTau for gas
//
// Revision 1.13  2004/05/29 21:16:27  fisyak
// Fix pad direction, add sorting for ADC/cluster nonlinearity, replace product by sum of logs
//
// Revision 1.12  2004/05/17 19:45:08  fisyak
// Clean up, add pseudo padrows
//
// Revision 1.11  2004/05/05 17:41:52  fisyak
// Take K3 from E.Mathieson book
//
// Revision 1.10  2004/05/04 13:39:06  fisyak
// Add TF1
//
// Revision 1.9  2004/05/02 20:54:18  fisyak
// fix t0 offset
//
// Revision 1.8  2004/04/22 01:05:03  fisyak
// Freeze the version before modification parametrization for K3
//
// Revision 1.7  2004/04/12 14:30:07  fisyak
// Propagate cluster as a whole
//
// Revision 1.6  2004/04/06 01:50:13  fisyak
// Switch from Double_t to Float_t for sum
//
// Revision 1.5  2004/03/30 19:30:04  fisyak
// Add Laser
//
// Revision 1.4  2004/03/21 19:00:43  fisyak
// switch to GEANT step length
//
// Revision 1.3  2004/03/20 17:57:15  fisyak
// Freeze the version of PAI model
//
// Revision 1.2  2004/03/17 20:47:43  fisyak
// Add version with TrsCluster TTree
//
// Revision 1.1.1.1  2004/03/05 20:51:25  fisyak
// replacement for Trs
//
