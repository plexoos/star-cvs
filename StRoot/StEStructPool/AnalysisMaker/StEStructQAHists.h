/**********************************************************************
 *
 * $Id: StEStructQAHists.h,v 1.2 2006/04/27 22:20:12 prindle Exp $
 *
 * Author: Jeff Porter 
 *
 **********************************************************************
 *
 * Description:  base class for QA histogramming
 *
 ***********************************************************************/
#ifndef _STEStructQAHists_H
#define _STEStructQAHists_H

#include "StEStructEventReader.h"
#include "TROOT.h"

class TH1D;
class TH1F;
class TH2F;
class StEStructTrack;

class StEStructQAHists : public TObject {

 protected:

  int  mEType; //event type: 0=data, 1=hij, 2=pyth, 3... organize as you need

  TH1D * mCents[2];
  TH1D * mptCents[3];

  // --- a base set for aa event generators (e.g. hijing)

  TH1D * aaGenImpact;
  TH2F * aaGen[2];
  TH1D ** aaGenBin;
  TH1D ** aaGenPart;

  // --- a base set for aa event generators (e.g. hijing)

  TH1F ** ppELines;
  TH1F ** ppALines;

  //--- a basic track set

  bool mhasAIndex;
  int mntBins;
  TH1F ** mHEta;
  TH1F ** mHPhi;
  TH1F ** mHPt;
  TH1F ** mHYt;
  TH2F ** mHdEdxPtot;

  void initBaseHistograms();
  void fillBaseHistograms(StEStructEvent* event, StEStructEventReader* reader);
  void writeBaseHistograms(TFile* tf);

 public:

  StEStructQAHists(int itype=0);
  virtual ~StEStructQAHists();

  virtual void initHistograms();
  virtual void fillHistograms(StEStructEvent* event, StEStructEventReader* reader);
  virtual void writeHistograms(TFile* tf);

  virtual void initTrackHistograms(int numBins, int analysisIndex=-1);
  virtual void fillTrackHistograms(StEStructTrack* track, int ibin);
  virtual void writeTrackHistograms(TFile* tf);

  ClassDef(StEStructQAHists,1)
};

#endif

/**********************************************************************
 *
 * $Log: StEStructQAHists.h,v $
 * Revision 1.2  2006/04/27 22:20:12  prindle
 * Some changes in trigger names for run periods.
 * Changed a couple of the Hijing QA histograms.
 *
 * Revision 1.1  2006/04/04 22:05:06  porter
 * a handful of changes:
 *  - changed the StEStructAnalysisMaker to contain 1 reader not a list of readers
 *  - added StEStructQAHists object to contain histograms that did exist in macros or elsewhere
 *  - made centrality event cut taken from StEStructCentrality singleton
 *  - put in  ability to get any max,min val from the cut class - one must call setRange in class
 *
 *
 *********************************************************************/
