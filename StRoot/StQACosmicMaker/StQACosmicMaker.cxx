/***************************************************************************
 *
 * $Id: StQACosmicMaker.cxx,v 1.21 2003/09/02 17:58:50 perev Exp $
 *
 * Author: Raimond Snellings, LBNL, Jun 1999
 * Description:  Maker to QA the data from the tables (hitfinding, tracking etc.)
 *
 * $Log: StQACosmicMaker.cxx,v $
 * Revision 1.21  2003/09/02 17:58:50  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 1.20  2000/08/18 20:34:46  snelling
 * Added hit errors to ntuple
 *
 * Revision 1.19  2000/07/14 23:18:24  snelling
 * Added the z-residual histograms
 *
 * Revision 1.18  2000/07/04 01:15:08  snelling
 * change to make ntuple write out even if tables last event are empty
 *
 * Revision 1.17  2000/07/03 04:10:28  snelling
 * Fixed bug in ntuple (changed :: to :)
 *
 * Revision 1.16  2000/06/23 00:29:52  snelling
 * Added hit charge to the ntuple
 *
 * Revision 1.15  2000/02/05 01:26:10  snelling
 * Fixed multiple declaration loop variable (SUN compiler does not like it)
 *
 * Revision 1.14  2000/02/03 23:51:38  snelling
 * Removed empty histograms
 *
 * Revision 1.13  2000/01/20 23:14:59  snelling
 * Made writing histograms default
 *
 **************************************************************************/
#include <Stiostream.h>
#include <stdlib.h>
#include <math.h>
#include "StQACosmicMaker.h"
#include "StChain.h"
#include "St_DataSet.h"
#include "St_DataSetIter.h"
#include "TFile.h"
#include "TF1.h"
//-----------------------------------------------------------------------

ClassImp(StQACosmicMaker)

StQACosmicMaker::StQACosmicMaker(const char *name):
  StMaker(name),
  bSectorSelectionOn(kFALSE),
  bWriteTNtupleOn(kTRUE),
  bWritePostscriptOn(kFALSE),
  bWriteHistogramsOn(kTRUE),
  nXBins(50),
  MakerName(name) {
  
}

//-----------------------------------------------------------------------

StQACosmicMaker::~StQACosmicMaker() {

}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::Init() {

  if (bWriteTNtupleOn) {initTNtuple();}
  initClusHistograms();
  initMorphHistograms();
  initResHistograms();
  initChargeHistograms();

  return StMaker::Init();
}
//-----------------------------------------------------------------------

Int_t StQACosmicMaker::Make() {

  fillTablePointers();
  if (bWriteTNtupleOn && btptrack) {fillTrackTNtuple();}
  if (bWriteTNtupleOn && brestpt && btphit && btptrack) {fillHitTNtuple();}
  if (bWriteTNtupleOn && bmorph && btphit && btptrack) {fillMorphTNtuple();}
  if (bWriteTNtupleOn && brestpt && btphit && btptrack) {fillHitTNtupleNoTrk();}
  if (bWriteTNtupleOn && bmorph && btphit && btptrack) {fillMorphTNtupleNoTrk();}
  if (btphit && btptrack) {fillClusHistograms();}
  if (bmorph && btphit && btptrack) {fillMorphHistograms();}
  if (brestpt&& btphit && btptrack) {fillResHistograms();}
  if (btphit && btptrack) {fillChargeHistograms();}
  cleanUpTableSorters();

  return kStOK;
}

//-----------------------------------------------------------------------

void StQACosmicMaker::PrintInfo() {
  printf("**************************************************************\n");
  printf("* $Id: StQACosmicMaker.cxx,v 1.21 2003/09/02 17:58:50 perev Exp $\n");
  printf("**************************************************************\n");

  if (Debug()) StMaker::PrintInfo();
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::Finish() {

  if (bWriteHistogramsOn) {calcResHistograms();}
  if (bWriteHistogramsOn) {calcChargeHistograms();}
  if (bWriteTNtupleOn) {writeOutTNtuple();}
  if (bWriteHistogramsOn) {writeOutHistograms();}
  
  return StMaker::Finish();
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillTablePointers() {

  /*
    // get pointers to raw adc xyz table
    St_DataSetIter Itpc_raw(GetDataSet("tpc_raw"));
    phtfc = 0;
    ptadcxyz = 0;
    phtfc = (St_tfc_adcxyz *) Itpc_raw.Find("adcxyz");
    if (phtfc) {ptadcxyz = phtfc->GetTable();}
    else { cout << "Warning: adcxyz table header does not exist " << endl; return kStWarn; }
    if (!ptadcxyz) { cout << "Warning: adcxyz table does not exist " << endl; return kStWarn; }
  */

  St_DataSetIter Itpc_hits(GetDataSet("tpc_hits"));

  // get pointers to tpc hit table
  btphit = kTRUE;
  phtcl = NULL;
  pttphit = NULL;
  phtcl = (St_tcl_tphit *) Itpc_hits.Find("tphit");
  if (phtcl) {pttphit = phtcl->GetTable();}
  else { cout << "Warning: tphit table header does not exist "   << endl; btphit = kFALSE;}
  if (!pttphit) { cout << "Warning: tphit table does not exist " << endl; btphit = kFALSE;}

  // Tom's Table
  bmorph = kTRUE;
  phmorph = NULL;
  ptmorph = NULL;
  phmorph = (St_tcc_morphology *) Itpc_hits.Find("morph");
  if (phmorph) {ptmorph = phmorph->GetTable();}
  else { cout << "Warning: morphology table header does not exist "   << endl; bmorph = kFALSE;}
  if (!ptmorph) { cout << "Warning: morphology table does not exist " << endl; bmorph = kFALSE;}

  // get pointers to tpc residuals table and track table
  St_DataSetIter Itpc_trk(GetDataSet("tpc_tracks"));

  brestpt = kTRUE;
  phres = NULL;
  ptres = NULL;
  phres = (St_tpt_res *) Itpc_trk.Find("restpt");
  if (phres) {ptres = phres->GetTable();}
  else { cout << "Warning: restpt table header does not exist " << endl; brestpt = kFALSE;}
  if (!ptres) { cout << "Warning: restpt table does not exist " << endl; brestpt = kFALSE;}

  btptrack = kTRUE;
  phtrk = NULL;
  pttrk = NULL;
  phtrk = (St_tpt_track *) Itpc_trk.Find("tptrack");
  if (phtrk) {pttrk = phtrk->GetTable();}
  else { cout << "Warning: tptrack table header does not exist " << endl; btptrack = kFALSE;}
  if (!pttrk) { cout << "Warning: tptrack table does not exist " << endl; btptrack = kFALSE;}


  TString colName;
  Int_t nrows;

  if (brestpt) {
    nrows = phres->GetNRows();
    if (nrows == 0) {
      cout << "Warning: residual table contains zero rows "   << endl; 
      brestpt = kFALSE;
    }
    else {
      colName = "hit";
      ressorter = new St_TableSorter(*phres,colName,0,nrows-1);
    }
  }

  if (btptrack) {
    nrows = phtrk->GetNRows();
    if (nrows == 0) {
      cout << "Warning: track table contains zero rows " << endl; 
      btptrack = kFALSE;
    }
    else {
      colName = "id";
      trksorter = new St_TableSorter(*phtrk,colName,0,nrows-1);
    }
  }

  if (bmorph) {
    nrows = phmorph->GetNRows();
    if (nrows == 0) {
      cout << "Warning: morphology table contains zero rows " << endl; 
      bmorph = kFALSE;
    }
    else {
      colName = "clusterId";
      morphsorter = new St_TableSorter(*phmorph,colName,0,nrows-1);
    }
  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::cleanUpTableSorters() {

  if (brestpt)  {delete ressorter;}
  if (bmorph)   {delete morphsorter;}
  if (btptrack) {delete trksorter;}

  return kStOK;
}

//-----------------------------------------------------------------------

void StQACosmicMaker::setSector(const Int_t sectorNumber) {

  SectorSelectionOn();

  if (sectorNumber > 0 && sectorNumber <= 24) {
  SelectedSector = sectorNumber;
  cout << "Sector: " << SelectedSector << " selected" << endl;
  }
  else {
    cout << "error sector selected which does not exist" << endl;
  }
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::writeOutHistograms() {

  TString *mHistFileName = new TString(MakerName.Data());
  mHistFileName->Append(".hists.root");

  TFile outHistFile(mHistFileName->Data(),"RECREATE"); 
  this->GetHistList()->Write();
  outHistFile.Close();

  delete mHistFileName;

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::initTNtuple() {

  char* ntVars = "hflag:hrow:hsector:hx:hy:hz:hdx:hdy:hdz:hq:dedx:halpha:hlambda:hdalpha:hdlambda:resy:resz:trknfit:trkp";  
  mTNtupleTPC = new TNtuple("nttpc","TPC TNtuple",ntVars);

  mTNtupleMorph = new TNtuple("ntmorph",
			    "Morph TNtuple",
			    "hx:hy:hz:cnseq:cnpix:cnpads");

  mTNtupleTPCNoTrk = new TNtuple("nttpcnotrk",
			    "TPC TNtuple Hits not on track",
			    "hflag:hrow:hx:hy:hz:hq:dedx");

  mTNtupleMorphNoTrk = new TNtuple("ntmorphnotrk",
			    "Morph TNtuple Hits not on track",
			    "hx:hy:hz:cnseq:cnpix:cnpads");

  mTNtupleTrk = new TNtuple("nttrk",
			    "TPC TNtuple Track parameters",
			    "flag:r0:z0:tanl:phi0:trkcalcp");
  
  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillTrackTNtuple() {

  for (Int_t i=0; i<phtrk->GetNRows();i++) {

    Float_t trkcalcp = ::sqrt((pttrk[i].tanl * pttrk[i].tanl + 1.) /
			    (pttrk[i].invp * pttrk[i].invp));
    if (Debug()) {cout << "track momentum" << trkcalcp << endl;}
    
    mTNtupleTrk->Fill(
		      (Float_t)(pttrk[i].flag),
		      (Float_t)(pttrk[i].r0),
		      (Float_t)(pttrk[i].z0),
		      (Float_t)(pttrk[i].tanl),
		      (Float_t)(pttrk[i].phi0),
		      (Float_t)(trkcalcp)
		      );
  }
  
  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillHitTNtuple() {

  for(Int_t i=0; i<phtcl->GetNRows();i++) {
    Int_t irow_res = ressorter->operator[]((Int_t)(pttphit[i].id));
    // track in row table is 1000*id + position on track
    Int_t irow_trk = trksorter->operator[]((Int_t)(pttphit[i].track/1000.));
    
    // global cuts and only particles belonging to track
    if (irow_trk >= 0 && irow_res >= 0) {
      if (pttrk[irow_trk].flag >= 0) {
	// calculate total momentum of the track where the hit belongs to
	Float_t trkcalcp = ::sqrt((pttrk[irow_trk].tanl * pttrk[irow_trk].tanl + 1.) /
				(pttrk[irow_trk].invp * pttrk[irow_trk].invp));
	if (Debug()) {cout << "track momentum" << trkcalcp << endl;}
	if (Debug()) {cout << "z residual" << ptres[irow_res].resz << endl;}

	// row in tphit table 100*sector + row
	Int_t isector = (Int_t)(floor(pttphit[i].row / 100.));
	Int_t irowsector = pttphit[i].row - 100 * isector;

	ntEntries[hflag]    = pttphit[i].flag;
	ntEntries[hrow]     = (Float_t)(irowsector);
	ntEntries[hsector]  = (Float_t)(isector);
	ntEntries[hx]       = pttphit[i].x;
	ntEntries[hy]       = pttphit[i].y;
	ntEntries[hz]       = pttphit[i].z;
	ntEntries[hdx]      = pttphit[i].dx;
	ntEntries[hdy]      = pttphit[i].dy;
	ntEntries[hdz]      = pttphit[i].dz;
	ntEntries[hq]       = pttphit[i].q;
	ntEntries[dedx]     = pttphit[i].dedx;
	ntEntries[halpha]   = pttphit[i].alpha;
	ntEntries[hlambda]  = pttphit[i].lambda;
	ntEntries[hdalpha]  = pttphit[i].dalpha;
	ntEntries[hdlambda] = pttphit[i].dlambda;
	ntEntries[resy]     = ptres[irow_res].resy;
	ntEntries[resz]     = ptres[irow_res].resz;
	ntEntries[trknfit]  = (Float_t)(pttrk[irow_trk].nfit);
	ntEntries[trkp]     = trkcalcp;

	mTNtupleTPC->Fill(ntEntries);
	
	if (Debug()) {cout << "dip angle " <<pttphit[i].lambda << endl;}
      }
    }
  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillMorphTNtuple() {

  for(Int_t i=0; i<phtcl->GetNRows();i++) {
    // track in row table is 1000*id + position on track
    Int_t irow_trk = trksorter->operator[]((Int_t)(pttphit[i].track/1000.));
    // cluster information
    Int_t irow_morph = morphsorter->operator[]((Int_t)(pttphit[i].cluster));
    
    // global cuts and only particles belonging to track
    if (irow_trk >= 0 && irow_morph >= 0) {
      if (pttrk[irow_trk].flag >= 0) {
	mTNtupleMorph->Fill(
			    (Float_t)(pttphit[i].x),
			    (Float_t)(pttphit[i].y),
			    (Float_t)(pttphit[i].z),
			    (Float_t)(ptmorph[irow_morph].numberOfSequences),
			    (Float_t)(ptmorph[irow_morph].numberOfPixels),
			    (Float_t)(ptmorph[irow_morph].numberOfPads)
			    );
	if (Debug()) {cout << "number of Pixels: " 
			   << ptmorph[irow_morph].numberOfSequences 
			   << endl;}
      }
    }
  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillHitTNtupleNoTrk() {

  for(Int_t i=0; i<phtcl->GetNRows();i++) {
    // track in row table is 1000*id + position on track
    Int_t irow_trk = trksorter->operator[]((Int_t)(pttphit[i].track/1000.));
    
    // global cuts and only particles belonging to track
    if (irow_trk < 0) {
      // row in tphit table 100*sector + row
      Int_t isector = (Int_t)(floor(pttphit[i].row / 100.));
      Int_t irowsector = pttphit[i].row - 100 * isector;

      mTNtupleTPCNoTrk->Fill(
			(Float_t)(pttphit[i].flag),
			(Float_t)(irowsector),
			(Float_t)(pttphit[i].x),
			(Float_t)(pttphit[i].y),
			(Float_t)(pttphit[i].z),
			(Float_t)(pttphit[i].q),
			(Float_t)(pttphit[i].dedx)
			);

      if (Debug()) {cout << "x " << pttphit[i].x << endl;}
    }

    if (irow_trk >= 0) {
      if (pttrk[irow_trk].flag < 0) {
	// row in tphit table 100*sector + row
	Int_t isector = (Int_t)(floor(pttphit[i].row / 100.));
	Int_t irowsector = pttphit[i].row - 100 * isector;

	mTNtupleTPCNoTrk->Fill(
			       (Float_t)(pttphit[i].flag),
			       (Float_t)(irowsector),
			       (Float_t)(pttphit[i].x),
			       (Float_t)(pttphit[i].y),
			       (Float_t)(pttphit[i].z),
			       (Float_t)(pttphit[i].q),
			       (Float_t)(pttphit[i].dedx)
			       );

	if (Debug()) {cout << "x " << pttphit[i].x << endl;}
      }
    }
  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillMorphTNtupleNoTrk() {

  for(Int_t i=0; i<phtcl->GetNRows();i++) {
    // track in row table is 1000*id + position on track
    Int_t irow_trk = trksorter->operator[]((Int_t)(pttphit[i].track/1000.));
    // cluster information
    Int_t irow_morph = morphsorter->operator[]((Int_t)(pttphit[i].cluster));
    
    // global cuts and only particles belonging to track
    if (irow_trk < 0 && irow_morph >= 0) {

      mTNtupleMorphNoTrk->Fill(
			       (Float_t)(pttphit[i].x),
			       (Float_t)(pttphit[i].y),
			       (Float_t)(pttphit[i].z),
			       (Float_t)(ptmorph[irow_morph].numberOfSequences),
			       (Float_t)(ptmorph[irow_morph].numberOfPixels),
			       (Float_t)(ptmorph[irow_morph].numberOfPads)
			       );
      if (Debug()) {cout << "number of Pixels: " << 
		      ptmorph[irow_morph].numberOfSequences  << endl;}
    }

    if (irow_trk >= 0 && irow_morph >= 0) {
      if (pttrk[irow_trk].flag < 0) {
	mTNtupleMorphNoTrk->Fill(
				 (Float_t)(pttphit[i].x),
				 (Float_t)(pttphit[i].y),
				 (Float_t)(pttphit[i].z),
				 (Float_t)(ptmorph[irow_morph].numberOfSequences),
				 (Float_t)(ptmorph[irow_morph].numberOfPixels),
				 (Float_t)(ptmorph[irow_morph].numberOfPads)
				 );
	if (Debug()) {cout << "number of Pixels: " << 
			ptmorph[irow_morph].numberOfSequences  << endl;}
      }
    }
  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::writeOutTNtuple() {


  TString *mTNtupleFileName = new TString(MakerName.Data());
  mTNtupleFileName->Append(".ntuple.root");

  TFile outTNtupleFile(mTNtupleFileName->Data(),"RECREATE"); 
  mTNtupleTPC->Write();
  mTNtupleMorph->Write();
  mTNtupleTPCNoTrk->Write();
  mTNtupleMorphNoTrk->Write();
  mTNtupleTrk->Write();
  outTNtupleFile.Close();

  delete mTNtupleFileName;
  
  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::writeOutPostscript() {
  // not implemented yet

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::initResHistograms() {

  int i;
  TString *mHistTitle;
  TString *mHistName;
  char mCount[2];
  char *mIndexName[nChargeHist] = {" inner p > .3 GeV"," inner p < .3 GeV",
				    " outer p > .3 GeV"," outer p < .3 GeV"};

  char mSector[3];
  if (bSectorSelectionOn) {sprintf(mSector,"%2d",SelectedSector);}

  Float_t xMin = -100.;
  Float_t xMax = 100.;
  Float_t yMin = -1.;
  Float_t yMax = 1.;
  Int_t nYBins = 200;
  
  for (i = 0; i < nResHist; i++) {

    sprintf(mCount,"%1d",i);


    // XY residual versus crossing angle


    mHistTitle = new TString("xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalpha");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusAlpha =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusAlpha->Sumw2();
    ResidualHists[i].mXYResVersusAlpha->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mXYResVersusAlpha->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsalpha");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsalpha");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlpha_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlpha_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsalpha");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlpha_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlpha_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsalpha");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlpha_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlpha_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("xy residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusAlphaZ1 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusAlphaZ1->Sumw2();
    ResidualHists[i].mXYResVersusAlphaZ1->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mXYResVersusAlphaZ1->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsalphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsalphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsalphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsalphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("xy residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusAlphaZ2 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusAlphaZ2->Sumw2();
    ResidualHists[i].mXYResVersusAlphaZ2->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mXYResVersusAlphaZ2->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsalphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsalphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsalphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsalphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("xy residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusAlphaZ3 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusAlphaZ3->Sumw2();
    ResidualHists[i].mXYResVersusAlphaZ3->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mXYResVersusAlphaZ3->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsalphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsalphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsalphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsalphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("xy residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusAlphaZ4 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusAlphaZ4->Sumw2();
    ResidualHists[i].mXYResVersusAlphaZ4->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mXYResVersusAlphaZ4->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsalphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsalphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsalphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsalphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;



    // XY residual versus dip angle


    mHistTitle = new TString("xy residual vs dip angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusDip =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusDip->Sumw2();
    ResidualHists[i].mXYResVersusDip->SetXTitle("dip angle (degree)");
    ResidualHists[i].mXYResVersusDip->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs dip angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDip_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDip_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs dip angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDip_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDip_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs dip angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDip_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDip_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs dip angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDip_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDip_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("xy residual vs dip angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusDipZ1 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusDipZ1->Sumw2();
    ResidualHists[i].mXYResVersusDipZ1->SetXTitle("dip angle (degree)");
    ResidualHists[i].mXYResVersusDipZ1->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs dip angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs dip angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs dip angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs dip angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("xy residual vs dip angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusDipZ2 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusDipZ2->Sumw2();
    ResidualHists[i].mXYResVersusDipZ2->SetXTitle("dip angle (degree)");
    ResidualHists[i].mXYResVersusDipZ2->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs dip angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs dip angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs dip angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs dip angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("xy residual vs dip angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusDipZ3 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusDipZ3->Sumw2();
    ResidualHists[i].mXYResVersusDipZ3->SetXTitle("dip angle (degree)");
    ResidualHists[i].mXYResVersusDipZ3->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs dip angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs dip angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs dip angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs dip angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("xy residual vs dip angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].mXYResVersusDipZ4 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mXYResVersusDipZ4->Sumw2();
    ResidualHists[i].mXYResVersusDipZ4->SetXTitle("dip angle (degree)");
    ResidualHists[i].mXYResVersusDipZ4->SetYTitle("xy residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean xy residual vs dip angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanxyresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma xy residual vs dip angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaxyresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude xy residual vs dip angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magxyresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared xy residual vs dip angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chixyresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;



    // Z residual versus crossing angle


    mHistTitle = new TString("Z residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlpha");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusAlpha =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusAlpha->Sumw2();
    ResidualHists[i].mZResVersusAlpha->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mZResVersusAlpha->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsAlpha");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlpha_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsAlpha");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlpha_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlpha_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsAlpha");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlpha_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlpha_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsAlpha");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlpha_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlpha_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Z residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusAlphaZ1 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusAlphaZ1->Sumw2();
    ResidualHists[i].mZResVersusAlphaZ1->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mZResVersusAlphaZ1->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsAlphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsAlphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsAlphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs crossing angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsAlphaZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Z residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusAlphaZ2 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusAlphaZ2->Sumw2();
    ResidualHists[i].mZResVersusAlphaZ2->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mZResVersusAlphaZ2->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsAlphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsAlphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsAlphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs crossing angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsAlphaZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Z residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusAlphaZ3 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusAlphaZ3->Sumw2();
    ResidualHists[i].mZResVersusAlphaZ3->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mZResVersusAlphaZ3->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsAlphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsAlphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsAlphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs crossing angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsAlphaZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Z residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusAlphaZ4 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusAlphaZ4->Sumw2();
    ResidualHists[i].mZResVersusAlphaZ4->SetXTitle("crossing angle (degree)");
    ResidualHists[i].mZResVersusAlphaZ4->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsAlphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsAlphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsAlphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs crossing angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsAlphaZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    // Z residual versus dip angle

    mHistTitle = new TString("Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusDip =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusDip->Sumw2();
    ResidualHists[i].mZResVersusDip->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].mZResVersusDip->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDip_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDip_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDip_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDip_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDip_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDip_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsDip");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDip_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDip_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Z residual vs Dip Angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusDipZ1 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusDipZ1->Sumw2();
    ResidualHists[i].mZResVersusDipZ1->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].mZResVersusDipZ1->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs Dip Angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs Dip Angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ1_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ1_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ1_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ1_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs Dip Angle 0 < z <= 50");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsDipZ1");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ1_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ1_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Z residual vs Dip Angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusDipZ2 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusDipZ2->Sumw2();
    ResidualHists[i].mZResVersusDipZ2->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].mZResVersusDipZ2->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs Dip Angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs Dip Angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ2_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ2_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ2_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ2_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs Dip Angle 50 < z <= 100");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsDipZ2");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ2_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ2_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Z residual vs Dip Angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusDipZ3 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusDipZ3->Sumw2();
    ResidualHists[i].mZResVersusDipZ3->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].mZResVersusDipZ3->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs Dip Angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs Dip Angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ3_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ3_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ3_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ3_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs Dip Angle 100 < z <= 150");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsDipZ3");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ3_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ3_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Z residual vs Dip Angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].mZResVersusDipZ4 =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    ResidualHists[i].mZResVersusDipZ4->Sumw2();
    ResidualHists[i].mZResVersusDipZ4->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].mZResVersusDipZ4->SetYTitle("Z residuals");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean Z residual vs Dip Angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanZresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma Z residual vs Dip Angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmaZresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ4_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ4_sigma->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magZresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ4_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ4_mag->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared Z residual vs Dip Angle 150 < z <= 200");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chiZresvsDipZ4");
    mHistName->Append(*mCount);
    ResidualHists[i].FitHists.mZResVersusDipZ4_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ResidualHists[i].FitHists.mZResVersusDipZ4_chi->Sumw2();
    delete mHistTitle;
    delete mHistName;


  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillResHistograms() {
  
  int i;

  // fill histograms  
  for (i=0; i<phtcl->GetNRows(); i++) {
    Int_t irow_res = ressorter->operator[]((Int_t)(pttphit[i].id));
    // track in tphit table is 1000*id + position on track
    Int_t irow_trk = trksorter->operator[]((Int_t)(pttphit[i].track/1000.));
    // row in tphit table 100*sector + row
    Int_t isector = (Int_t)(pttphit[i].row/100.);
    Int_t irowsector = pttphit[i].row - 100 * isector;   

    // global cuts and only particles belonging to track
    if (ptres[irow_res].resy != 0. && pttphit[i].alpha != 0. && irow_trk >= 0) {
      if (pttrk[irow_trk].flag >= 0) {
	// calculate total momentum of the track where the hit belongs to
	Float_t trkcalcp = ::sqrt((pttrk[irow_trk].tanl * pttrk[irow_trk].tanl + 1) /
				(pttrk[irow_trk].invp * pttrk[irow_trk].invp));

	//  no specific sector selected 
	if (!bSectorSelectionOn) {
	  // inner sector
	  if (irowsector <= 13) {
	    if (trkcalcp >= 0.3) {
	      ResidualHists[0].mXYResVersusAlpha->
		Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
	      ResidualHists[0].mXYResVersusDip->
		Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
	      ResidualHists[0].mZResVersusAlpha->
		Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
	      ResidualHists[0].mZResVersusDip->
		Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      if (fabs(pttphit[i].z) <= 50.) {
		ResidualHists[0].mXYResVersusAlphaZ1->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[0].mXYResVersusDipZ1->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[0].mZResVersusAlphaZ1->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[0].mZResVersusDipZ1->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 50. && fabs(pttphit[i].z) <= 100.) {
		ResidualHists[0].mXYResVersusAlphaZ2->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[0].mXYResVersusDipZ2->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[0].mZResVersusAlphaZ2->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[0].mZResVersusDipZ2->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 100. && fabs(pttphit[i].z) <= 150.) {
		ResidualHists[0].mXYResVersusAlphaZ3->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[0].mXYResVersusDipZ3->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[0].mZResVersusAlphaZ3->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[0].mZResVersusDipZ3->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 150. && fabs(pttphit[i].z) <= 200.) {
		ResidualHists[0].mXYResVersusAlphaZ4->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[0].mXYResVersusDipZ4->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[0].mZResVersusAlphaZ4->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[0].mZResVersusDipZ4->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	    }
	    else {
	      ResidualHists[1].mXYResVersusAlpha->
		Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
	      ResidualHists[1].mXYResVersusDip->
		Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
	      ResidualHists[1].mZResVersusAlpha->
		Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
	      ResidualHists[1].mZResVersusDip->
		Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      if (fabs(pttphit[i].z) <= 50.) {
		ResidualHists[1].mXYResVersusAlphaZ1->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[1].mXYResVersusDipZ1->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[1].mZResVersusAlphaZ1->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[1].mZResVersusDipZ1->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 50. && fabs(pttphit[i].z) <= 100.) {
		ResidualHists[1].mXYResVersusAlphaZ2->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[1].mXYResVersusDipZ2->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[1].mZResVersusAlphaZ2->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[1].mZResVersusDipZ2->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 100. && fabs(pttphit[i].z) <= 150.) {
		ResidualHists[1].mXYResVersusAlphaZ3->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[1].mXYResVersusDipZ3->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[1].mZResVersusAlphaZ3->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[1].mZResVersusDipZ3->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 150. && fabs(pttphit[i].z) <= 200.) {
		ResidualHists[1].mXYResVersusAlphaZ4->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[1].mXYResVersusDipZ4->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[1].mZResVersusAlphaZ4->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[1].mZResVersusDipZ4->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	    }
	  }
	  // outer sector
	  else {
	    if (trkcalcp >= 0.3) {
	      ResidualHists[2].mXYResVersusAlpha->
		Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
	      ResidualHists[2].mXYResVersusDip->
		Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
	      ResidualHists[2].mZResVersusAlpha->
		Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
	      ResidualHists[2].mZResVersusDip->
		Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      if (fabs(pttphit[i].z) <= 50.) {
		ResidualHists[2].mXYResVersusAlphaZ1->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[2].mXYResVersusDipZ1->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[2].mZResVersusAlphaZ1->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[2].mZResVersusDipZ1->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 50. && fabs(pttphit[i].z) <= 100.) {
		ResidualHists[2].mXYResVersusAlphaZ2->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[2].mXYResVersusDipZ2->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[2].mZResVersusAlphaZ2->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[2].mZResVersusDipZ2->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 100. && fabs(pttphit[i].z) <= 150.) {
		ResidualHists[2].mXYResVersusAlphaZ3->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[2].mXYResVersusDipZ3->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[2].mZResVersusAlphaZ3->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[2].mZResVersusDipZ3->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 150. && fabs(pttphit[i].z) <= 200.) {
		ResidualHists[2].mXYResVersusAlphaZ4->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[2].mXYResVersusDipZ4->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[2].mZResVersusAlphaZ4->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[2].mZResVersusDipZ4->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	    }
	    else {
	      ResidualHists[3].mXYResVersusAlpha->
		Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
	      ResidualHists[3].mXYResVersusDip->
		Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
	      ResidualHists[3].mZResVersusAlpha->
		Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
	      ResidualHists[3].mZResVersusDip->
		Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      if (fabs(pttphit[i].z) <= 50.) {
		ResidualHists[3].mXYResVersusAlphaZ1->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[3].mXYResVersusDipZ1->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[3].mZResVersusAlphaZ1->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[3].mZResVersusDipZ1->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 50. && fabs(pttphit[i].z) <= 100.) {
		ResidualHists[3].mXYResVersusAlphaZ2->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[3].mXYResVersusDipZ2->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[3].mZResVersusAlphaZ2->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[3].mZResVersusDipZ2->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 100. && fabs(pttphit[i].z) <= 150.) {
		ResidualHists[3].mXYResVersusAlphaZ3->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[3].mXYResVersusDipZ3->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[3].mZResVersusAlphaZ3->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[3].mZResVersusDipZ3->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	      if (fabs(pttphit[i].z) > 150. && fabs(pttphit[i].z) <= 200.) {
		ResidualHists[3].mXYResVersusAlphaZ4->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[3].mXYResVersusDipZ4->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resy) );
		ResidualHists[3].mZResVersusAlphaZ4->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resz) );
		ResidualHists[3].mZResVersusDipZ4->
		  Fill((Float_t)(pttphit[i].lambda),(Float_t)(ptres[irow_res].resz) );
	      }
	    }
	  }
	}
	// fill histograms only for selected sector, low momentum in hist1 rest in hist0
	else {
	  if (isector == SelectedSector) {
	    // inner sector
	    if (irowsector <= 13) {
	      if (trkcalcp >= 0.3) {
		ResidualHists[0].mXYResVersusAlpha->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		if (fabs(pttphit[i].z) <= 50.) {
		  ResidualHists[0].mXYResVersusAlphaZ1->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 50. && fabs(pttphit[i].z) <= 100.) {
		  ResidualHists[0].mXYResVersusAlphaZ2->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 100. && fabs(pttphit[i].z) <= 150.) {
		  ResidualHists[0].mXYResVersusAlphaZ3->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 150. && fabs(pttphit[i].z) <= 200.) {
		  ResidualHists[0].mXYResVersusAlphaZ4->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
	      }
	      else {
		ResidualHists[1].mXYResVersusAlpha->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy));
		if (fabs(pttphit[i].z) <= 50.) {
		  ResidualHists[1].mXYResVersusAlphaZ1->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 50. && fabs(pttphit[i].z) <= 100.) {
		  ResidualHists[1].mXYResVersusAlphaZ2->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 100. && fabs(pttphit[i].z) <= 150.) {
		  ResidualHists[1].mXYResVersusAlphaZ3->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 150. && fabs(pttphit[i].z) <= 200.) {
		  ResidualHists[1].mXYResVersusAlphaZ4->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
	      }
	    }
	    // outer sector
	    else {
	      if (trkcalcp >= 0.3) {
		ResidualHists[2].mXYResVersusAlpha->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		if (fabs(pttphit[i].z) <= 50.) {
		  ResidualHists[2].mXYResVersusAlphaZ1->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 50. && fabs(pttphit[i].z) <= 100.) {
		  ResidualHists[2].mXYResVersusAlphaZ2->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 100. && fabs(pttphit[i].z) <= 150.) {
		  ResidualHists[2].mXYResVersusAlphaZ3->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 150. && fabs(pttphit[i].z) <= 200.) {
		  ResidualHists[2].mXYResVersusAlphaZ4->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
	      }
	      else {
		ResidualHists[3].mXYResVersusAlpha->
		  Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy));
		if (fabs(pttphit[i].z) <= 50.) {
		  ResidualHists[3].mXYResVersusAlphaZ1->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 50. && fabs(pttphit[i].z) <= 100.) {
		  ResidualHists[3].mXYResVersusAlphaZ2->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 100. && fabs(pttphit[i].z) <= 150.) {
		  ResidualHists[3].mXYResVersusAlphaZ3->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
		if (fabs(pttphit[i].z) > 150. && fabs(pttphit[i].z) <= 200.) {
		  ResidualHists[3].mXYResVersusAlphaZ4->
		    Fill((Float_t)(pttphit[i].alpha),(Float_t)(ptres[irow_res].resy) );
		}
	      }
	    }
	  }
	}
      }
    }
  }
  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::calcResHistograms() {

  int i;
  int j;
  TString *mHistTitle;
  TString *mHistName;
  char mCount[2];
  char *mIndexName[nChargeHist] = {" inner p > .3 GeV"," inner p < .3 GeV",
				   " outer p > .3 GeV"," outer p < .3 GeV"};
  char mSector[3];
  if (bSectorSelectionOn) {sprintf(mSector,"%2d",SelectedSector);}

  for (i = 0; i < nResHist; i++) {

    sprintf(mCount,"%1d",i);

    // XY residual versus crossing angle

    mHistTitle = new TString("xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusAlpha->FitSlicesY();
    ResidualHists[i].mXYResVersusAlpha->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusAlpha->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalpha");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlpha_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlpha_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlpha_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlpha_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlpha_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalpha");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlpha_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusAlpha_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalpha");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlpha_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlpha_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlpha_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlpha_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlpha_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs crossing angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalpha");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlpha_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlpha_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlpha_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlpha_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlpha_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusAlpha_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusAlpha_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusAlpha_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 0 - 50 cm

    mHistTitle = new TString("xy residual vs crossing angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusAlphaZ1->FitSlicesY();
    ResidualHists[i].mXYResVersusAlphaZ1->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusAlphaZ1->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs crossing angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs crossing angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs crossing angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ1_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ1_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusAlphaZ1_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 50 - 100 cm

    mHistTitle = new TString("xy residual vs crossing angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusAlphaZ2->FitSlicesY();
    ResidualHists[i].mXYResVersusAlphaZ2->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusAlphaZ2->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs crossing angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs crossing angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs crossing angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ2_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ2_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusAlphaZ2_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 100 - 150 cm

    mHistTitle = new TString("xy residual vs crossing angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusAlphaZ3->FitSlicesY();
    ResidualHists[i].mXYResVersusAlphaZ3->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusAlphaZ3->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs crossing angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs crossing angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs crossing angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ3_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ3_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusAlphaZ3_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 150 - 200 cm

    mHistTitle = new TString("xy residual vs crossing angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusAlphaZ4->FitSlicesY();
    ResidualHists[i].mXYResVersusAlphaZ4->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusAlphaZ4->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs crossing angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs crossing angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs crossing angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs crossing angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsalphaZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusAlphaZ4_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusAlphaZ4_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusAlphaZ4_sigma->fArray[j]);
    }

    // XY residual versus dip angle

    mHistTitle = new TString("xy residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusDip->FitSlicesY();
    ResidualHists[i].mXYResVersusDip->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusDip->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDip");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDip_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDip_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDip_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDip_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDip_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDip");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDip_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDip_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDip_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDip_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusDip_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusDip_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDip_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusDip_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDip_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDip_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusDip_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDip");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDip_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDip_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDip_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDip_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDip_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDip");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDip_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDip_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDip_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDip_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDip_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusDip_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusDip_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusDip_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 0 - 50 cm

    mHistTitle = new TString("xy residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusDipZ1->FitSlicesY();
    ResidualHists[i].mXYResVersusDipZ1->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusDipZ1->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ1_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ1_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusDipZ1_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ1_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ1_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ1_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ1_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ1_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusDipZ1_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 50 - 100 cm

    mHistTitle = new TString("xy residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusDipZ2->FitSlicesY();
    ResidualHists[i].mXYResVersusDipZ2->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusDipZ2->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ2_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ2_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusDipZ2_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ2_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ2_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ2_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ2_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ2_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusDipZ2_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 100 - 150 cm

    mHistTitle = new TString("xy residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusDipZ3->FitSlicesY();
    ResidualHists[i].mXYResVersusDipZ3->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusDipZ3->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ3_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ3_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusDipZ3_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ3_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ3_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ3_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ3_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ3_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusDipZ3_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 150 - 200 cm

    mHistTitle = new TString("xy residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mXYResVersusDipZ4->FitSlicesY();
    ResidualHists[i].mXYResVersusDipZ4->SetName(mHistTitle->Data());
    ResidualHists[i].mXYResVersusDipZ4->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude xy residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ4_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean xy residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ4_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->SetYTitle("Mean xy residuals");
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mXYResVersusDipZ4_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma xy residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 xy residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("xyresvsDipZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mXYResVersusDipZ4_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mXYResVersusDipZ4_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mXYResVersusDipZ4_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mXYResVersusDipZ4_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mXYResVersusDipZ4_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mXYResVersusDipZ4_sigma->fArray[j]);
    }

    // Z residual versus dip angle

    mHistTitle = new TString("Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusDip->FitSlicesY();
    ResidualHists[i].mZResVersusDip->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusDip->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDip");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDip_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDip_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDip_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDip_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDip_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDip");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDip_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDip_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDip_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDip_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusDip_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusDip_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDip_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusDip_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDip_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDip_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusDip_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDip");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDip_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDip_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDip_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDip_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDip_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Dip Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDip");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDip_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDip_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDip_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDip_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDip_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusDip_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusDip_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusDip_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 0 - 50 cm

    mHistTitle = new TString("Z residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusDipZ1->FitSlicesY();
    ResidualHists[i].mZResVersusDipZ1->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusDipZ1->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ1_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ1_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ1_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ1_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ1_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ1_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusDipZ1_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ1_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ1_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ1_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ1_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ1_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Dip Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ1_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ1_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ1_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ1_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ1_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusDipZ1_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusDipZ1_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusDipZ1_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 50 - 100 cm

    mHistTitle = new TString("Z residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusDipZ2->FitSlicesY();
    ResidualHists[i].mZResVersusDipZ2->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusDipZ2->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ2_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ2_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ2_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ2_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ2_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ2_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusDipZ2_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ2_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ2_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ2_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ2_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ2_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Dip Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ2_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ2_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ2_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ2_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ2_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusDipZ2_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusDipZ2_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusDipZ2_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 100 - 150 cm

    mHistTitle = new TString("Z residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusDipZ3->FitSlicesY();
    ResidualHists[i].mZResVersusDipZ3->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusDipZ3->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ3_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ3_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ3_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ3_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ3_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ3_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusDipZ3_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ3_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ3_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ3_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ3_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ3_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Dip Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ3_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ3_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ3_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ3_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ3_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusDipZ3_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusDipZ3_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusDipZ3_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 150 - 200 cm

    mHistTitle = new TString("Z residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusDipZ4->FitSlicesY();
    ResidualHists[i].mZResVersusDipZ4->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusDipZ4->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ4_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ4_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ4_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ4_mag->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ4_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ4_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusDipZ4_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ4_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ4_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ4_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ4_sigma->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ4_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Dip Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsDipZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusDipZ4_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusDipZ4_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusDipZ4_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusDipZ4_chi->SetXTitle("Dip Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusDipZ4_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusDipZ4_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusDipZ4_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusDipZ4_sigma->fArray[j]);
    }


    // Z residual versus crossing angle

    mHistTitle = new TString("Z residual vs Crossing Angle");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusAlpha->FitSlicesY();
    ResidualHists[i].mZResVersusAlpha->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusAlpha->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Crossing Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlpha");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlpha_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlpha_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlpha_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlpha_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlpha_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Crossing Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlpha");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlpha_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusAlpha_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Crossing Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlpha");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlpha_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlpha_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlpha_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlpha_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlpha_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Crossing Angle");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlpha");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlpha_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlpha_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlpha_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlpha_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlpha_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusAlpha_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusAlpha_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusAlpha_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 0 - 50 cm

    mHistTitle = new TString("Z residual vs Crossing Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusAlphaZ1->FitSlicesY();
    ResidualHists[i].mZResVersusAlphaZ1->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusAlphaZ1->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Crossing Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ1_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Crossing Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Crossing Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Crossing Angle (0 < z <= 50)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ1");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ1_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ1_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusAlphaZ1_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 50 - 100 cm

    mHistTitle = new TString("Z residual vs Crossing Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusAlphaZ2->FitSlicesY();
    ResidualHists[i].mZResVersusAlphaZ2->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusAlphaZ2->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Crossing Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ2_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Crossing Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Crossing Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Crossing Angle (50 < z <= 100)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ2");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ2_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ2_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusAlphaZ2_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 100 - 150 cm

    mHistTitle = new TString("Z residual vs Crossing Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusAlphaZ3->FitSlicesY();
    ResidualHists[i].mZResVersusAlphaZ3->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusAlphaZ3->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Crossing Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ3_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Crossing Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Crossing Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Crossing Angle (100 < z <= 150)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ3");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ3_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ3_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusAlphaZ3_sigma->fArray[j]);
    }

    //-----------------------------------------------------------------------
    // z dependence residuals 150 - 200 cm

    mHistTitle = new TString("Z residual vs Crossing Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    ResidualHists[i].mZResVersusAlphaZ4->FitSlicesY();
    ResidualHists[i].mZResVersusAlphaZ4->SetName(mHistTitle->Data());
    ResidualHists[i].mZResVersusAlphaZ4->SetTitle(mHistTitle->Data());
    delete mHistTitle;

    mHistTitle = new TString("Magnitude Z residual vs Crossing Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ4_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mag->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mag->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Z residual vs Crossing Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->SetMaximum(.4);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->SetMinimum(-.4);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->SetYTitle("Mean Z residuals");
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->GetXaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->GetYaxis()->SetLabelSize(0.04);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->SetMarkerColor(kBlue);
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Z residual vs Crossing Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma->SetYTitle("Sigma");
    
    mHistTitle = new TString("Chi2 Z residual vs Crossing Angle (150 < z <= 200)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ZresvsAlphaZ4");
    mHistName->Append(*mCount);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ResidualHists[i].FitHists.mZResVersusAlphaZ4_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_chi->SetName(mHistTitle->Data());
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_chi->SetXTitle("Crossing Angle (degree)");
    ResidualHists[i].FitHists.mZResVersusAlphaZ4_chi->SetYTitle("chi2");
    
    for (j=0; j<ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma->fN; j++) { 
      ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma->fArray[j] = 
    	fabs(ResidualHists[i].FitHists.mZResVersusAlphaZ4_sigma->fArray[j]);
    }

  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::initChargeHistograms() {

  int i;
  char *mIndexName[nChargeHist]={"x","y","z","row"};
  TString *mHistTitle;
  TString *mHistName;

  char mSector[3];
  if (bSectorSelectionOn) {sprintf(mSector,"%2d",SelectedSector);}

  Float_t xMin = -250.;
  Float_t xMax = 250.;
  Float_t yMin = 0.;
  Float_t yMax = 0.00001;
  Int_t nYBins = 2000;


  // define the histograms for q versus x,y,z  
  for (i = 0; i < nChargeHist; i++) {

    mHistTitle = new TString("Q distribution versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chargevs");
    mHistName->Append(mIndexName[i]);
    ChargeHists[i].mQdist =
      new TH2F(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, nYBins, yMin, yMax);
    if (mIndexName[i] == "row") {ChargeHists[i].mQdist->SetBins(50, 0., 50.,2000, 0., 0.00001);}
    ChargeHists[i].mQdist->Sumw2();
    ChargeHists[i].mQdist->SetXTitle(mIndexName[i]);
    ChargeHists[i].mQdist->SetYTitle("Q");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Q profile versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chargeprofvs");
    mHistName->Append(mIndexName[i]);
    ChargeHists[i].mQprof =
      new TProfile(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax, yMin, yMax);
    ChargeHists[i].mQprof->Sumw2();
    ChargeHists[i].mQprof->SetName(mHistTitle->Data());
    if (mIndexName[i] == "row") {ChargeHists[i].mQprof->SetBins(50, 0., 50.);}
    ChargeHists[i].mQprof->SetXTitle(mIndexName[i]);
    ChargeHists[i].mQprof->SetYTitle("Q profile");
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Mean charge versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("meanchargevs");
    mHistName->Append(mIndexName[i]);
    ChargeHists[i].FitQHists.mQ_mean =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ChargeHists[i].FitQHists.mQ_mean->Sumw2();
    if (mIndexName[i] == "row") {ChargeHists[i].FitQHists.mQ_mean->SetBins(50, 0., 50.);}
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Sigma charge versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("sigmachargevs");
    mHistName->Append(mIndexName[i]);
    ChargeHists[i].FitQHists.mQ_sigma =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ChargeHists[i].FitQHists.mQ_sigma->Sumw2();
    if (mIndexName[i] == "row") {ChargeHists[i].FitQHists.mQ_sigma->SetBins(50, 0., 50.);}
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Magnitude charge versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("magchargevs");
    mHistName->Append(mIndexName[i]);
    ChargeHists[i].FitQHists.mQ_mag =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ChargeHists[i].FitQHists.mQ_mag->Sumw2();
    if (mIndexName[i] == "row") {ChargeHists[i].FitQHists.mQ_mag->SetBins(50, 0., 50.);}
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("ChiSquared charge versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chichargevs");
    mHistName->Append(mIndexName[i]);
    ChargeHists[i].FitQHists.mQ_chi =
      new TH1D(mHistName->Data(), mHistTitle->Data(), nXBins, xMin, xMax);
    ChargeHists[i].FitQHists.mQ_chi->Sumw2();
    if (mIndexName[i] == "row") {ChargeHists[i].FitQHists.mQ_chi->SetBins(50, 0., 50.);}
    delete mHistTitle;
    delete mHistName;
  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillChargeHistograms() {
  
  int i;
  
  for (i=0; i<phtcl->GetNRows(); i++) {
    // track in row table is 1000*id + position on track
    Int_t irow_trk = trksorter->operator[]((Int_t)(pttphit[i].track/1000.));
    Int_t isector = (Int_t)(pttphit[i].row/100.);
    Int_t irowsector = pttphit[i].row - 100 * isector;   
    
    // global cuts and only particles belonging to track
    if (pttphit[i].q != 0. && irow_trk >= 0) {
      if (pttrk[irow_trk].flag >= 0) {
	// calculate total momentum of the track where the hit belongs to
	Float_t trkcalcp = ::sqrt((pttrk[irow_trk].tanl * pttrk[irow_trk].tanl + 1) /
				(pttrk[irow_trk].invp * pttrk[irow_trk].invp));
	
	if (Debug()) {
	  cout << "trk row nr in table: " << irow_trk << endl;
	}
	
	if (trkcalcp >= 0.3) {
	  //  no specific sector selected 
	  if (!bSectorSelectionOn) {
	    ChargeHists[0].mQdist->
	      Fill((Float_t)(pttphit[i].x),(Float_t)(pttphit[i].q) );
	    ChargeHists[1].mQdist->
	      Fill((Float_t)(pttphit[i].y),(Float_t)(pttphit[i].q) );
	    ChargeHists[2].mQdist->
	      Fill((Float_t)(pttphit[i].z),(Float_t)(pttphit[i].q) );
	    ChargeHists[3].mQdist->
	      Fill((Float_t)(irowsector),(Float_t)(pttphit[i].q) );
	    // profile histogram
	    ChargeHists[0].mQprof->
	      Fill((Float_t)(pttphit[i].x),(Float_t)(pttphit[i].q) );
	    ChargeHists[1].mQprof->
	      Fill((Float_t)(pttphit[i].y),(Float_t)(pttphit[i].q) );
	    ChargeHists[2].mQprof->
	      Fill((Float_t)(pttphit[i].z),(Float_t)(pttphit[i].q) );
	    ChargeHists[3].mQprof->
	      Fill((Float_t)(irowsector),(Float_t)(pttphit[i].q) );
	  }
	  // fill histograms only for selected sector
	  else {
	    if (isector == SelectedSector) {
	      ChargeHists[0].mQdist->
		Fill((Float_t)(pttphit[i].x),(Float_t)(pttphit[i].q) );
	      ChargeHists[1].mQdist->
		Fill((Float_t)(pttphit[i].y),(Float_t)(pttphit[i].q) );
	      ChargeHists[2].mQdist->
		Fill((Float_t)(pttphit[i].z),(Float_t)(pttphit[i].q) );
	      ChargeHists[3].mQdist->
		Fill((Float_t)(irowsector),(Float_t)(pttphit[i].q) );
	      // profile histograms
	      ChargeHists[0].mQprof->
		Fill((Float_t)(pttphit[i].x),(Float_t)(pttphit[i].q) );
	      ChargeHists[1].mQprof->
		Fill((Float_t)(pttphit[i].y),(Float_t)(pttphit[i].q) );
	      ChargeHists[2].mQprof->
		Fill((Float_t)(pttphit[i].z),(Float_t)(pttphit[i].q) );
	      ChargeHists[3].mQprof->
		Fill((Float_t)(irowsector),(Float_t)(pttphit[i].q) );
	    }
	  }
	}
      }
    }
  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::calcChargeHistograms() {

  int i;
  char *mIndexName[nChargeHist]={"x","y","z","row"};
  TString *mHistTitle;
  TString *mHistName;

  char mSector[3];
  if (bSectorSelectionOn) {sprintf(mSector, "%2d", SelectedSector);}

  for (i = 0; i < nChargeHist; i++) {

    mHistTitle = new TString("Q distribution versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    TF1 *mLandau = new TF1("Landau","landau",0.,1.);
    ChargeHists[i].mQdist->FitSlicesY(mLandau);
    ChargeHists[i].mQdist->SetXTitle(mIndexName[i]);
    ChargeHists[i].mQdist->SetName(mHistTitle->Data());
    ChargeHists[i].mQdist->SetTitle(mHistTitle->Data());
    delete mHistTitle;
    delete mLandau;
    
    mHistTitle = new TString("Magnitude Q distribution versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chargevs");
    mHistName->Append(mIndexName[i]);
    mHistName->Append("_0");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ChargeHists[i].FitQHists.mQ_mag);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ChargeHists[i].FitQHists.mQ_mag->SetName(mHistTitle->Data());
    ChargeHists[i].FitQHists.mQ_mag->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ChargeHists[i].FitQHists.mQ_mag->SetXTitle(mIndexName[i]);
    ChargeHists[i].FitQHists.mQ_mag->SetYTitle("Magnitude");

    mHistTitle = new TString("Mean Q distribution versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chargevs");
    mHistName->Append(mIndexName[i]);
    mHistName->Append("_1");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ChargeHists[i].FitQHists.mQ_mean);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ChargeHists[i].FitQHists.mQ_mean->SetName(mHistTitle->Data());
    ChargeHists[i].FitQHists.mQ_mean->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ChargeHists[i].FitQHists.mQ_mean->SetXTitle(mIndexName[i]);
    ChargeHists[i].FitQHists.mQ_mean->SetYTitle("Mean Q");
    ChargeHists[i].FitQHists.mQ_mean->GetXaxis()->SetLabelSize(0.04);
    ChargeHists[i].FitQHists.mQ_mean->GetYaxis()->SetLabelSize(0.04);
    ChargeHists[i].FitQHists.mQ_mean->SetMarkerColor(kBlue);
    ChargeHists[i].FitQHists.mQ_mean->SetMarkerStyle(20);

    mHistTitle = new TString("Sigma Q distribution versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chargevs");
    mHistName->Append(mIndexName[i]);
    mHistName->Append("_2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ChargeHists[i].FitQHists.mQ_sigma);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ChargeHists[i].FitQHists.mQ_sigma->SetName(mHistTitle->Data());
    ChargeHists[i].FitQHists.mQ_sigma->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ChargeHists[i].FitQHists.mQ_sigma->SetXTitle(mIndexName[i]);
    ChargeHists[i].FitQHists.mQ_sigma->SetYTitle("Sigma Q");
    
    mHistTitle = new TString("Chi2 Q distribution versus ");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("chargevs");
    mHistName->Append(mIndexName[i]);
    mHistName->Append("_chi2");
    ((TH1D *) gDirectory->Get(mHistName->Data()))->Copy(*ChargeHists[i].FitQHists.mQ_chi);
    delete ((TH1D *) gDirectory->Get(mHistName->Data()));
    ChargeHists[i].FitQHists.mQ_chi->SetName(mHistTitle->Data());
    ChargeHists[i].FitQHists.mQ_chi->SetTitle(mHistTitle->Data());
    delete mHistName;
    delete mHistTitle;
    ChargeHists[i].FitQHists.mQ_chi->SetXTitle(mIndexName[i]);
    ChargeHists[i].FitQHists.mQ_chi->SetYTitle("chi2 Q");
    
    for (int j=0; j<ChargeHists[i].FitQHists.mQ_sigma->fN; j++) { 
      ChargeHists[i].FitQHists.mQ_sigma->fArray[j] = 
    	fabs(ChargeHists[i].FitQHists.mQ_sigma->fArray[j]);
    }
  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::initClusHistograms() {

  int i;
  TString *mHistTitle;
  TString *mHistName;
  char mCount[2];
  char *mIndexName[nChargeHist] = {" inner p > .3 GeV"," inner p < .3 GeV",
				    " outer p > .3 GeV"," outer p < .3 GeV"};

  char mSector[3];
  if (bSectorSelectionOn) {sprintf(mSector,"%2d",SelectedSector);}

  Float_t xMin = -1.;
  Float_t xMax = 20.;
  Int_t nBins = 21;

  for (i = 0; i < nClusterHist; i++) {

    sprintf(mCount,"%1d",i);

    mHistTitle = new TString("Number of Pads per Hit");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("npadsperhit");
    mHistName->Append(*mCount);
    ClusterHists[i].mNPadsPerHit =
      new TH1F(mHistName->Data(), mHistTitle->Data(), nBins, xMin, xMax);
    ClusterHists[i].mNPadsPerHit->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Number of Time buckets per Hit");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("ntmbkperhit");
    mHistName->Append(*mCount);
    ClusterHists[i].mNTimeBucketsPerHit =
      new TH1F(mHistName->Data(), mHistTitle->Data(), nBins, xMin, xMax);
    ClusterHists[i].mNTimeBucketsPerHit->Sumw2();
    delete mHistTitle;
    delete mHistName;

  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillClusHistograms() {
  int i;

  // fill histograms  
  for (i=0; i<phtcl->GetNRows(); i++) {
    // track in tphit table is 1000*id + position on track
    Int_t irow_trk = trksorter->operator[]((Int_t)(pttphit[i].track/1000.));
    // row in tphit table 100*sector + row
    Int_t isector = (Int_t)(pttphit[i].row/100.);
    Int_t irowsector = pttphit[i].row - 100 * isector;   

    // global cuts and only particles belonging to track
    if (irow_trk >= 0) {
      if (pttrk[irow_trk].flag >= 0) {
	// calculate total momentum of the track where the hit belongs to
	Float_t trkcalcp = ::sqrt((pttrk[irow_trk].tanl * pttrk[irow_trk].tanl + 1) /
				(pttrk[irow_trk].invp * pttrk[irow_trk].invp));
	
	//  no specific sector selected 
	if (!bSectorSelectionOn) {
	  // inner sector
	  if (irowsector <= 13) {
	    if (trkcalcp >= 0.3) {
	      ClusterHists[0].mNPadsPerHit->Fill((Float_t)(pttphit[i].npads));
	      ClusterHists[0].mNTimeBucketsPerHit->Fill((Float_t)(pttphit[i].ntmbk));
	    }
	    else {
	      ClusterHists[1].mNPadsPerHit->Fill((Float_t)(pttphit[i].npads));
	      ClusterHists[1].mNTimeBucketsPerHit->Fill((Float_t)(pttphit[i].ntmbk));
	    }
	  }
	  // outer sector
	  else {
	    if (trkcalcp >= 0.3) {
	      ClusterHists[2].mNPadsPerHit->Fill((Float_t)(pttphit[i].npads));
	      ClusterHists[2].mNTimeBucketsPerHit->Fill((Float_t)(pttphit[i].ntmbk));
	    }
	    else {
	      ClusterHists[3].mNPadsPerHit->Fill((Float_t)(pttphit[i].npads));
	      ClusterHists[3].mNTimeBucketsPerHit->Fill((Float_t)(pttphit[i].ntmbk));
	    }
	  }
	}
	// fill histograms only for selected sector, low momentum in hist1 rest in hist0
	else {
	  if (isector == SelectedSector) {
	    // inner sector
	    if (irowsector <= 13) {
	      if (trkcalcp >= 0.3) {
		ClusterHists[0].mNPadsPerHit->Fill((Float_t)(pttphit[i].npads));
		ClusterHists[0].mNTimeBucketsPerHit->Fill((Float_t)(pttphit[i].ntmbk));
	      }
	      else {
		ClusterHists[1].mNPadsPerHit->Fill((Float_t)(pttphit[i].npads));
		ClusterHists[1].mNTimeBucketsPerHit->Fill((Float_t)(pttphit[i].ntmbk));
	      }
	    }
	    // outer sector
	    else {
	      if (trkcalcp >= 0.3) {
		ClusterHists[2].mNPadsPerHit->Fill((Float_t)(pttphit[i].npads));
		ClusterHists[2].mNTimeBucketsPerHit->Fill((Float_t)(pttphit[i].ntmbk));
	      }
	      else {
		ClusterHists[3].mNPadsPerHit->Fill((Float_t)(pttphit[i].npads));
		ClusterHists[3].mNTimeBucketsPerHit->Fill((Float_t)(pttphit[i].ntmbk));
	      }
	    }
	  }
	}
      }
    }
  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::initMorphHistograms() {

  int i;
  TString *mHistTitle;
  TString *mHistName;
  char mCount[2];
  char *mIndexName[nChargeHist] = {" inner p > .3 GeV"," inner p < .3 GeV",
				    " outer p > .3 GeV"," outer p < .3 GeV"};

  char mSector[3];
  if (bSectorSelectionOn) {sprintf(mSector,"%2d",SelectedSector);}

  Float_t xMin = -1.;
  Float_t xMax = 20.;
  Int_t nBins = 21;  
  for (i = 0; i < nResHist; i++) {

    sprintf(mCount,"%1d",i);

    mHistTitle = new TString("Number Of Sequences (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("NumberOfSequences");
    mHistName->Append(*mCount);
    MorphHists[i].mNumberOfSequences =
      new TH1F(mHistName->Data(), mHistTitle->Data(), nBins, xMin, xMax);
    MorphHists[i].mNumberOfSequences->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Number Of Pixels (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("NumberOfPixels");
    mHistName->Append(*mCount);
    MorphHists[i].mNumberOfPixels =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 41, -1., 40.);
    MorphHists[i].mNumberOfPixels->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Number Of Pads (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("NumberOfPads");
    mHistName->Append(*mCount);
    MorphHists[i].mNumberOfPads =
      new TH1F(mHistName->Data(), mHistTitle->Data(), nBins, xMin, xMax);
    MorphHists[i].mNumberOfPads->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Number Of Hits (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("NumberOfHits");
    mHistName->Append(*mCount);
    MorphHists[i].mNumberOfHits =
      new TH1F(mHistName->Data(), mHistTitle->Data(), nBins, xMin, xMax);
    MorphHists[i].mNumberOfHits->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Total Charge (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("TotalCharge");
    mHistName->Append(*mCount);
    MorphHists[i].mTotalCharge =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 200, 0., 2000.);
    MorphHists[i].mTotalCharge->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Max Charge (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("MaxCharge");
    mHistName->Append(*mCount);
    MorphHists[i].mMaxCharge =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 200, 0., 200.);
    MorphHists[i].mMaxCharge->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Average Charge (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("AverageCharge");
    mHistName->Append(*mCount);
    MorphHists[i].mAverageCharge =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., 100.);
    MorphHists[i].mAverageCharge->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Pad Sigma 1 (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("PadSigma1");
    mHistName->Append(*mCount);
    MorphHists[i].mPadSigma1 =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., 1.);
    MorphHists[i].mPadSigma1->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Time Sigma 1 (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("TimeSigma1");
    mHistName->Append(*mCount);
    MorphHists[i].mTimeSigma1 =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., 1.);
    MorphHists[i].mTimeSigma1->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Pad Time Sigma 1 Sq (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("PadTimeSigma1Sq");
    mHistName->Append(*mCount);
    MorphHists[i].mPadTimeSigma1Sq =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., .2);
    MorphHists[i].mPadTimeSigma1Sq->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Ecc1 (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("Ecc1");
    mHistName->Append(*mCount);
    MorphHists[i].mEcc1 =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., .1);
    MorphHists[i].mEcc1->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Lin Ecc 1 (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("LinEcc1");
    mHistName->Append(*mCount);
    MorphHists[i].mLinEcc1 =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., .1);
    MorphHists[i].mLinEcc1->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Pad Sigma 2 (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("PadSigma2");
    mHistName->Append(*mCount);
    MorphHists[i].mPadSigma2 =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., 1.);
    MorphHists[i].mPadSigma2->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Time Sigma 2 (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("TimeSigma2");
    mHistName->Append(*mCount);
    MorphHists[i].mTimeSigma2 =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., 1.);
    MorphHists[i].mTimeSigma2->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Pad Time Sigma 2 Sq (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("PadTimeSigma2Sq");
    mHistName->Append(*mCount);
    MorphHists[i].mPadTimeSigma2Sq =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., .2);
    MorphHists[i].mPadTimeSigma2Sq->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Ecc2 (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("Ecc2");
    mHistName->Append(*mCount);
    MorphHists[i].mEcc2 =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., .1);
    MorphHists[i].mEcc2->Sumw2();
    delete mHistTitle;
    delete mHistName;

    mHistTitle = new TString("Lin Ecc 2 (Morphology table)");
    mHistTitle->Append(mIndexName[i]);
    if (bSectorSelectionOn) {mHistTitle->Append(" Sector "); mHistTitle->Append(*mSector);}
    mHistName  = new TString("LinEcc2");
    mHistName->Append(*mCount);
    MorphHists[i].mLinEcc2 =
      new TH1F(mHistName->Data(), mHistTitle->Data(), 100, 0., .1);
    MorphHists[i].mLinEcc2->Sumw2();
    delete mHistTitle;
    delete mHistName;

  }

  return kStOK;
}

//-----------------------------------------------------------------------

Int_t StQACosmicMaker::fillMorphHistograms() {

  int i;

  // fill histograms  
  for (i=0; i<phtcl->GetNRows(); i++) {
    // track in tphit table is 1000*id + position on track
    Int_t irow_trk = trksorter->operator[]((Int_t)(pttphit[i].track/1000.));
    // row in tphit table 100*sector + row
    Int_t isector = (Int_t)(pttphit[i].row/100.);
    Int_t irowsector = pttphit[i].row - 100 * isector;   
    
    Int_t irow_morph = morphsorter->operator[]((Int_t)(pttphit[i].cluster));
    
    // global cuts and only particles belonging to track
    if ( irow_trk >= 0 && irow_morph >= 0 ) {
      if (pttrk[irow_trk].flag >= 0) {
	// calculate total momentum of the track where the hit belongs to
	Float_t trkcalcp = ::sqrt((pttrk[irow_trk].tanl * pttrk[irow_trk].tanl + 1) /
				(pttrk[irow_trk].invp * pttrk[irow_trk].invp));
	
	//  no specific sector selected 
	if (!bSectorSelectionOn) {
	  // inner sector
	  if (irowsector <= 13) {
	    if (trkcalcp >= 0.3) {
	      MorphHists[0].mNumberOfSequences->Fill((Float_t)(ptmorph[irow_morph].numberOfSequences));
	      MorphHists[0].mNumberOfPixels->Fill((Float_t)(ptmorph[irow_morph].numberOfPixels));
	      MorphHists[0].mNumberOfPads->Fill((Float_t)(ptmorph[irow_morph].numberOfPads));
	      MorphHists[0].mNumberOfHits->Fill((Float_t)(ptmorph[irow_morph].numberOfHits));
	      MorphHists[0].mTotalCharge->Fill((Float_t)(ptmorph[irow_morph].totalCharge));
	      MorphHists[0].mMaxCharge->Fill((Float_t)(ptmorph[irow_morph].maxCharge));
	      MorphHists[0].mAverageCharge->Fill((Float_t)(ptmorph[irow_morph].averageCharge));
	      MorphHists[0].mPadSigma1->Fill((Float_t)(ptmorph[irow_morph].padSigma1));
	      MorphHists[0].mTimeSigma1->Fill((Float_t)(ptmorph[irow_morph].timeSigma1));
	      MorphHists[0].mPadTimeSigma1Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma1Sq ));
	      MorphHists[0].mEcc1->Fill((Float_t)(ptmorph[irow_morph].ecc1));
	      MorphHists[0].mLinEcc1->Fill((Float_t)(ptmorph[irow_morph].linEcc1));
	      MorphHists[0].mPadSigma2->Fill((Float_t)(ptmorph[irow_morph].padSigma2));
	      MorphHists[0].mTimeSigma2->Fill((Float_t)(ptmorph[irow_morph].timeSigma2));
	      MorphHists[0].mPadTimeSigma2Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma2Sq ));
	      MorphHists[0].mEcc2->Fill((Float_t)(ptmorph[irow_morph].ecc2));
	      MorphHists[0].mLinEcc2->Fill((Float_t)(ptmorph[irow_morph].linEcc2));
	    }
	    else {
	      MorphHists[1].mNumberOfSequences->Fill((Float_t)(ptmorph[irow_morph].numberOfSequences));
	      MorphHists[1].mNumberOfPixels->Fill((Float_t)(ptmorph[irow_morph].numberOfPixels));
	      MorphHists[1].mNumberOfPads->Fill((Float_t)(ptmorph[irow_morph].numberOfPads));
	      MorphHists[1].mNumberOfHits->Fill((Float_t)(ptmorph[irow_morph].numberOfHits));
	      MorphHists[1].mTotalCharge->Fill((Float_t)(ptmorph[irow_morph].totalCharge));
	      MorphHists[1].mMaxCharge->Fill((Float_t)(ptmorph[irow_morph].maxCharge));
	      MorphHists[1].mAverageCharge->Fill((Float_t)(ptmorph[irow_morph].averageCharge));
	      MorphHists[1].mPadSigma1->Fill((Float_t)(ptmorph[irow_morph].padSigma1));
	      MorphHists[1].mTimeSigma1->Fill((Float_t)(ptmorph[irow_morph].timeSigma1));
	      MorphHists[1].mPadTimeSigma1Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma1Sq ));
	      MorphHists[1].mEcc1->Fill((Float_t)(ptmorph[irow_morph].ecc1));
	      MorphHists[1].mLinEcc1->Fill((Float_t)(ptmorph[irow_morph].linEcc1));
	      MorphHists[1].mPadSigma2->Fill((Float_t)(ptmorph[irow_morph].padSigma2));
	      MorphHists[1].mTimeSigma2->Fill((Float_t)(ptmorph[irow_morph].timeSigma2));
	      MorphHists[1].mPadTimeSigma2Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma2Sq ));
	      MorphHists[1].mEcc2->Fill((Float_t)(ptmorph[irow_morph].ecc2));
	      MorphHists[1].mLinEcc2->Fill((Float_t)(ptmorph[irow_morph].linEcc2));
	    }
	  }
	  // outer sector
	  else {
	    if (trkcalcp >= 0.3) {
	      MorphHists[2].mNumberOfSequences->Fill((Float_t)(ptmorph[irow_morph].numberOfSequences));
	      MorphHists[2].mNumberOfPixels->Fill((Float_t)(ptmorph[irow_morph].numberOfPixels));
	      MorphHists[2].mNumberOfPads->Fill((Float_t)(ptmorph[irow_morph].numberOfPads));
	      MorphHists[2].mNumberOfHits->Fill((Float_t)(ptmorph[irow_morph].numberOfHits));
	      MorphHists[2].mTotalCharge->Fill((Float_t)(ptmorph[irow_morph].totalCharge));
	      MorphHists[2].mMaxCharge->Fill((Float_t)(ptmorph[irow_morph].maxCharge));
	      MorphHists[2].mAverageCharge->Fill((Float_t)(ptmorph[irow_morph].averageCharge));
	      MorphHists[2].mPadSigma1->Fill((Float_t)(ptmorph[irow_morph].padSigma1));
	      MorphHists[2].mTimeSigma1->Fill((Float_t)(ptmorph[irow_morph].timeSigma1));
	      MorphHists[2].mPadTimeSigma1Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma1Sq ));
	      MorphHists[2].mEcc1->Fill((Float_t)(ptmorph[irow_morph].ecc1));
	      MorphHists[2].mLinEcc1->Fill((Float_t)(ptmorph[irow_morph].linEcc1));
	      MorphHists[2].mPadSigma2->Fill((Float_t)(ptmorph[irow_morph].padSigma2));
	      MorphHists[2].mTimeSigma2->Fill((Float_t)(ptmorph[irow_morph].timeSigma2));
	      MorphHists[2].mPadTimeSigma2Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma2Sq ));
	      MorphHists[2].mEcc2->Fill((Float_t)(ptmorph[irow_morph].ecc2));
	      MorphHists[2].mLinEcc2->Fill((Float_t)(ptmorph[irow_morph].linEcc2));
	    }
	    else {
	      MorphHists[3].mNumberOfSequences->Fill((Float_t)(ptmorph[irow_morph].numberOfSequences));
	      MorphHists[3].mNumberOfPixels->Fill((Float_t)(ptmorph[irow_morph].numberOfPixels));
	      MorphHists[3].mNumberOfPads->Fill((Float_t)(ptmorph[irow_morph].numberOfPads));
	      MorphHists[3].mNumberOfHits->Fill((Float_t)(ptmorph[irow_morph].numberOfHits));
	      MorphHists[3].mTotalCharge->Fill((Float_t)(ptmorph[irow_morph].totalCharge));
	      MorphHists[3].mMaxCharge->Fill((Float_t)(ptmorph[irow_morph].maxCharge));
	      MorphHists[3].mAverageCharge->Fill((Float_t)(ptmorph[irow_morph].averageCharge));
	      MorphHists[3].mPadSigma1->Fill((Float_t)(ptmorph[irow_morph].padSigma1));
	      MorphHists[3].mTimeSigma1->Fill((Float_t)(ptmorph[irow_morph].timeSigma1));
	      MorphHists[3].mPadTimeSigma1Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma1Sq ));
	      MorphHists[3].mEcc1->Fill((Float_t)(ptmorph[irow_morph].ecc1));
	      MorphHists[3].mLinEcc1->Fill((Float_t)(ptmorph[irow_morph].linEcc1));
	      MorphHists[3].mPadSigma2->Fill((Float_t)(ptmorph[irow_morph].padSigma2));
	      MorphHists[3].mTimeSigma2->Fill((Float_t)(ptmorph[irow_morph].timeSigma2));
	      MorphHists[3].mPadTimeSigma2Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma2Sq ));
	      MorphHists[3].mEcc2->Fill((Float_t)(ptmorph[irow_morph].ecc2));
	      MorphHists[3].mLinEcc2->Fill((Float_t)(ptmorph[irow_morph].linEcc2));
	    }
	  }
	}
	// fill histograms only for selected sector, low momentum in hist1 rest in hist0
	else {
	  if (isector == SelectedSector) {
	    // inner sector
	    if (irowsector <= 13) {
	      if (trkcalcp >= 0.3) {
		MorphHists[0].mNumberOfSequences->Fill((Float_t)(ptmorph[irow_morph].numberOfSequences));
		MorphHists[0].mNumberOfPixels->Fill((Float_t)(ptmorph[irow_morph].numberOfPixels));
		MorphHists[0].mNumberOfPads->Fill((Float_t)(ptmorph[irow_morph].numberOfPads));
		MorphHists[0].mNumberOfHits->Fill((Float_t)(ptmorph[irow_morph].numberOfHits));
		MorphHists[0].mTotalCharge->Fill((Float_t)(ptmorph[irow_morph].totalCharge));
		MorphHists[0].mMaxCharge->Fill((Float_t)(ptmorph[irow_morph].maxCharge));
		MorphHists[0].mAverageCharge->Fill((Float_t)(ptmorph[irow_morph].averageCharge));
		MorphHists[0].mPadSigma1->Fill((Float_t)(ptmorph[irow_morph].padSigma1));
		MorphHists[0].mTimeSigma1->Fill((Float_t)(ptmorph[irow_morph].timeSigma1));
		MorphHists[0].mPadTimeSigma1Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma1Sq ));
		MorphHists[0].mEcc1->Fill((Float_t)(ptmorph[irow_morph].ecc1));
		MorphHists[0].mLinEcc1->Fill((Float_t)(ptmorph[irow_morph].linEcc1));
		MorphHists[0].mPadSigma2->Fill((Float_t)(ptmorph[irow_morph].padSigma2));
		MorphHists[0].mTimeSigma2->Fill((Float_t)(ptmorph[irow_morph].timeSigma2));
		MorphHists[0].mPadTimeSigma2Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma2Sq ));
		MorphHists[0].mEcc2->Fill((Float_t)(ptmorph[irow_morph].ecc2));
		MorphHists[0].mLinEcc2->Fill((Float_t)(ptmorph[irow_morph].linEcc2));
	      }
	      else {
		MorphHists[1].mNumberOfSequences->Fill((Float_t)(ptmorph[irow_morph].numberOfSequences));
		MorphHists[1].mNumberOfPixels->Fill((Float_t)(ptmorph[irow_morph].numberOfPixels));
		MorphHists[1].mNumberOfPads->Fill((Float_t)(ptmorph[irow_morph].numberOfPads));
		MorphHists[1].mNumberOfHits->Fill((Float_t)(ptmorph[irow_morph].numberOfHits));
		MorphHists[1].mTotalCharge->Fill((Float_t)(ptmorph[irow_morph].totalCharge));
		MorphHists[1].mMaxCharge->Fill((Float_t)(ptmorph[irow_morph].maxCharge));
		MorphHists[1].mAverageCharge->Fill((Float_t)(ptmorph[irow_morph].averageCharge));
		MorphHists[1].mPadSigma1->Fill((Float_t)(ptmorph[irow_morph].padSigma1));
		MorphHists[1].mTimeSigma1->Fill((Float_t)(ptmorph[irow_morph].timeSigma1));
		MorphHists[1].mPadTimeSigma1Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma1Sq ));
		MorphHists[1].mEcc1->Fill((Float_t)(ptmorph[irow_morph].ecc1));
		MorphHists[1].mLinEcc1->Fill((Float_t)(ptmorph[irow_morph].linEcc1));
		MorphHists[1].mPadSigma2->Fill((Float_t)(ptmorph[irow_morph].padSigma2));
		MorphHists[1].mTimeSigma2->Fill((Float_t)(ptmorph[irow_morph].timeSigma2));
		MorphHists[1].mPadTimeSigma2Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma2Sq ));
		MorphHists[1].mEcc2->Fill((Float_t)(ptmorph[irow_morph].ecc2));
		MorphHists[1].mLinEcc2->Fill((Float_t)(ptmorph[irow_morph].linEcc2));
	      }
	    }
	    // outer sector
	    else {
	      if (trkcalcp >= 0.3) {
		MorphHists[2].mNumberOfSequences->Fill((Float_t)(ptmorph[irow_morph].numberOfSequences));
		MorphHists[2].mNumberOfPixels->Fill((Float_t)(ptmorph[irow_morph].numberOfPixels));
		MorphHists[2].mNumberOfPads->Fill((Float_t)(ptmorph[irow_morph].numberOfPads));
		MorphHists[2].mNumberOfHits->Fill((Float_t)(ptmorph[irow_morph].numberOfHits));
		MorphHists[2].mTotalCharge->Fill((Float_t)(ptmorph[irow_morph].totalCharge));
		MorphHists[2].mMaxCharge->Fill((Float_t)(ptmorph[irow_morph].maxCharge));
		MorphHists[2].mAverageCharge->Fill((Float_t)(ptmorph[irow_morph].averageCharge));
		MorphHists[2].mPadSigma1->Fill((Float_t)(ptmorph[irow_morph].padSigma1));
		MorphHists[2].mTimeSigma1->Fill((Float_t)(ptmorph[irow_morph].timeSigma1));
		MorphHists[2].mPadTimeSigma1Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma1Sq ));
		MorphHists[2].mEcc1->Fill((Float_t)(ptmorph[irow_morph].ecc1));
		MorphHists[2].mLinEcc1->Fill((Float_t)(ptmorph[irow_morph].linEcc1));
		MorphHists[2].mPadSigma2->Fill((Float_t)(ptmorph[irow_morph].padSigma2));
		MorphHists[2].mTimeSigma2->Fill((Float_t)(ptmorph[irow_morph].timeSigma2));
		MorphHists[2].mPadTimeSigma2Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma2Sq ));
		MorphHists[2].mEcc2->Fill((Float_t)(ptmorph[irow_morph].ecc2));
		MorphHists[2].mLinEcc2->Fill((Float_t)(ptmorph[irow_morph].linEcc2));
	      }
	      else {
		MorphHists[3].mNumberOfSequences->Fill((Float_t)(ptmorph[irow_morph].numberOfSequences));
		MorphHists[3].mNumberOfPixels->Fill((Float_t)(ptmorph[irow_morph].numberOfPixels));
		MorphHists[3].mNumberOfPads->Fill((Float_t)(ptmorph[irow_morph].numberOfPads));
		MorphHists[3].mNumberOfHits->Fill((Float_t)(ptmorph[irow_morph].numberOfHits));
		MorphHists[3].mTotalCharge->Fill((Float_t)(ptmorph[irow_morph].totalCharge));
		MorphHists[3].mMaxCharge->Fill((Float_t)(ptmorph[irow_morph].maxCharge));
		MorphHists[3].mAverageCharge->Fill((Float_t)(ptmorph[irow_morph].averageCharge));
		MorphHists[3].mPadSigma1->Fill((Float_t)(ptmorph[irow_morph].padSigma1));
		MorphHists[3].mTimeSigma1->Fill((Float_t)(ptmorph[irow_morph].timeSigma1));
		MorphHists[3].mPadTimeSigma1Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma1Sq ));
		MorphHists[3].mEcc1->Fill((Float_t)(ptmorph[irow_morph].ecc1));
		MorphHists[3].mLinEcc1->Fill((Float_t)(ptmorph[irow_morph].linEcc1));
		MorphHists[3].mPadSigma2->Fill((Float_t)(ptmorph[irow_morph].padSigma2));
		MorphHists[3].mTimeSigma2->Fill((Float_t)(ptmorph[irow_morph].timeSigma2));
		MorphHists[3].mPadTimeSigma2Sq->Fill((Float_t)(ptmorph[irow_morph].padTimeSigma2Sq ));
		MorphHists[3].mEcc2->Fill((Float_t)(ptmorph[irow_morph].ecc2));
		MorphHists[3].mLinEcc2->Fill((Float_t)(ptmorph[irow_morph].linEcc2));
	      }
	    }
	  }
	}
      }
    }
  }
  
  return kStOK;
}

//-----------------------------------------------------------------------
