/***********************************************************************
 *
 * $Id: StStrangeEvMuDst.cc,v 3.3 2001/08/23 13:20:54 genevb Exp $
 *
 * Authors: Gene Van Buren, UCLA, 24-Mar-2000
 *          Peter G. Jones, University of Birmingham, 19-Aug-1999
 *
 ***********************************************************************
 *
 * Description: Strangeness event micro dst class
 *
 ***********************************************************************
 *
 * $Log: StStrangeEvMuDst.cc,v $
 * Revision 3.3  2001/08/23 13:20:54  genevb
 * Many bug workarounds...
 *
 * Revision 3.2  2001/05/04 20:15:14  genevb
 * Common interfaces and reorganization of components, add MC event info
 *
 * Revision 3.1  2000/09/07 02:22:56  genevb
 * Now using STAR standard uncorrected primary track multiplicity
 *
 * Revision 3.0  2000/07/14 12:56:49  genevb
 * Revision 3 has event multiplicities and dedx information for vertex tracks
 *
 * Revision 2.0  2000/06/02 22:11:54  genevb
 * New version of Strangeness micro DST package
 *
 * Revision 1.3  2000/03/31 03:20:24  jones
 * Added topology map to V0/Xi; access funcs for each data member
 *
 * Revision 1.2  2000/03/29 20:52:13  genevb
 * Added StKinkMuDst, replaced arrays
 *
 * Revision 1.1  2000/03/29 03:10:07  genevb
 * Introduction of Strangeness Micro DST package
 *
 *
 ***********************************************************************/
#include "StStrangeEvMuDst.hh"
#include "StEventTypes.h"
#include "StMcEventTypes.hh"
#include "StuRefMult.hh"
#include "StMessMgr.h"
#include "TArrayF.h"


static TArrayF uncorBinMean, corBinMean, corError;
static TArrayF fracTracks, fracSigma, fracError;

ClassImp(StStrangeEvMuDst)

StStrangeEvMuDst::StStrangeEvMuDst() { 
}

void StStrangeEvMuDst::Fill(StEvent& event) {

  mRun = event.runId();
  mEvent = event.id();
  
  mGlobalTracks = 0;
  StSPtrVecTrackNode& theNodes = event.trackNodes();
  for (unsigned int i=0; i<theNodes.size(); i++) {
    for (unsigned int j=0; j<theNodes[i]->entries(global); j++) {
      if (theNodes[i]->track(global,j)->flag() > 0)
        mGlobalTracks++;
    }
  }

  StPrimaryVertex* primaryVertex = event.primaryVertex();
  if( primaryVertex ) {
    mPrimaryVertexX = primaryVertex->position().x();
    mPrimaryVertexY = primaryVertex->position().y();
    mPrimaryVertexZ = primaryVertex->position().z();
    mPrimaryNegTracks = uncorrectedNumberOfNegativePrimaries(event);
    mPrimaryTracks    = uncorrectedNumberOfPositivePrimaries(event) +
                                                  mPrimaryNegTracks;
  } else {
    mPrimaryVertexX = 0.;
    mPrimaryVertexY = 0.;
    mPrimaryVertexZ = 0.;
    mPrimaryNegTracks = 0;
    mPrimaryTracks = 0;
  }
}

void StStrangeEvMuDst::Fill(StMcEvent& event) {

  mRun = event.runNumber();
  mEvent = event.eventNumber();
  
  mGlobalTracks = event.tracks().size();
  mPrimaryNegTracks = 0;

  StMcVertex* primaryVertex = event.primaryVertex();
  if( primaryVertex ) {
    mPrimaryVertexX = primaryVertex->position().x();
    mPrimaryVertexY = primaryVertex->position().y();
    mPrimaryVertexZ = primaryVertex->position().z();
    StPtrVecMcTrack& daughters = primaryVertex->daughters();
    mPrimaryTracks  = daughters.size();
    for (int i=0; i<mPrimaryTracks; i++) {
      StMcTrack* tri = daughters[i];
      if (tri) {
        StParticleDefinition* pdef = tri->particleDefinition();
	if ((pdef) && (pdef->charge() < 0)) mPrimaryNegTracks++;
      }
    }
  } else {
    mPrimaryVertexX = 0.;
    mPrimaryVertexY = 0.;
    mPrimaryVertexZ = 0.;
    mPrimaryTracks  = 0;
  }
}

StStrangeEvMuDst::~StStrangeEvMuDst() {
}

void StStrangeEvMuDst::SetCorrectionFile(char* fname) {
  FILE* fp = fopen(fname,"r");
  if (fp) {
    int asize=64;
    int count=0;
    float uBM,cBM,cE;
    while (fscanf(fp,"%f %f +/- %f",&uBM,&cBM,&cE) != EOF) {
      if ((count + 1) > corBinMean.GetSize()) {
        uncorBinMean.Set(asize);
        corBinMean.Set(asize);
        corError.Set(asize);
        asize *= 2;
      }
      uncorBinMean.AddAt(uBM,count);
      corBinMean.AddAt(cBM,count);
      corError.AddAt(cE,count);
      count++;
    }
    uncorBinMean.Set(count);
    corBinMean.Set(count);
    corError.Set(count);
    fclose(fp);
  } else {
    gMessMgr->Warning() << "StStrangeEvMuDst: Failed to find correction file: "
      << fname << "\n            No corrections available!" << endm;
  }
}

void StStrangeEvMuDst::SetFractionFile(char* fname) {
  FILE* fp = fopen(fname,"r");
  if (fp) {
    int asize=64;
    int count=0;
    float fT,fS,fE;
    while (fscanf(fp,"%f %f +/- %f",&fT,&fS,&fE) != EOF) {
      if ((count + 1) > fracTracks.GetSize()) {
        fracTracks.Set(asize);
        fracSigma.Set(asize);
        fracError.Set(asize);
        asize *= 2;
      }
      fracTracks.AddAt(fT,count);
      fracSigma.AddAt(fS,count);
      fracError.AddAt(fE,count);
      count++;
    }
    fracTracks.Set(count);
    fracSigma.Set(count);
    fracError.Set(count);
    fclose(fp);
  } else {
    gMessMgr->Warning() << "StStrangeEvMuDst: Failed to find fraction file: "
      << fname << "\n            No fractions available!" << endm;
  }
}

Float_t StStrangeEvMuDst::primaryCorrectedTracks() const {
  int count = corBinMean.GetSize();
  if (count) {
    count--;
    int i,j=1;
    for (i=0; i<count; i++) {
      if (uncorBinMean.At(i) > mPrimaryTracks) break;
    }
    if (i != 0) j = i - 1;
    Float_t wt1 = 1.0/(uncorBinMean.At(i) - mPrimaryTracks);
    Float_t wt2 = 1.0/(uncorBinMean.At(j) - mPrimaryTracks);
    if ((corError.At(i)) && (corError.At(j))) {
      wt1 /= corError.At(i);
      wt2 /= corError.At(j);
    }
    return ((wt1*corBinMean.At(i) - wt2*corBinMean.At(j))/(wt1 - wt2));
  }
  gMessMgr->Warning() << "StStrangeEvMuDst: No corrections entered!\n" <<
    "    Use SetCorrectionFile(char*) to select a valid correction map file"
    << endm;
  return -1.0;
}

Float_t StStrangeEvMuDst::fractionSigma() const {
  Float_t cTracks = primaryCorrectedTracks();
  int count = fracTracks.GetSize();
  if (count) {
    count--;
    int i,j=1;
    for (i=0; i<count; i++) {
      if (fracTracks.At(i) > cTracks) break;
    }
    if (i != 0) j = i - 1;
    Float_t wt1 = 1.0/(fracTracks.At(i) - cTracks);
    Float_t wt2 = 1.0/(fracTracks.At(j) - cTracks);
    if ((fracError.At(i)) && (fracError.At(j))) {
      wt1 /= fracError.At(i);
      wt2 /= fracError.At(j);
    }
    return ((wt1*fracSigma.At(i) - wt2*fracSigma.At(j))/(wt1 - wt2));
  }
  gMessMgr->Warning() << "StStrangeEvMuDst: No fractions entered!\n" <<
    "    Use SetFractionFile(char*) to select a valid fraction map file"
    << endm;
  return -1.0;
}

