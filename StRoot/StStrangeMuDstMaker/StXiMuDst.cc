/***********************************************************************
 *
 * $Id: StXiMuDst.cc,v 3.2 2001/05/04 20:15:15 genevb Exp $
 *
 * Authors: Gene Van Buren, UCLA, 24-Mar-2000
 *          Peter G. Jones, University of Birmingham, 30-Mar-1999
 *
 ***********************************************************************
 *
 * Description: Xi (cascade) micro dst class
 *
 ***********************************************************************
 *
 * $Log: StXiMuDst.cc,v $
 * Revision 3.2  2001/05/04 20:15:15  genevb
 * Common interfaces and reorganization of components, add MC event info
 *
 * Revision 3.1  2000/08/10 01:16:25  genevb
 * Added number of dedx points
 *
 * Revision 3.0  2000/07/14 12:56:50  genevb
 * Revision 3 has event multiplicities and dedx information for vertex tracks
 *
 * Revision 2.0  2000/06/02 22:11:55  genevb
 * New version of Strangeness micro DST package
 *
 * Revision 1.3  2000/03/31 03:20:24  jones
 * Added topology map to V0/Xi; access funcs for each data member
 *
 * Revision 1.2  2000/03/29 20:52:14  genevb
 * Added StKinkMuDst, replaced arrays
 *
 * Revision 1.1  2000/03/29 03:10:08  genevb
 * Introduction of Strangeness Micro DST package
 *
 *
 ***********************************************************************/
#include "StXiMuDst.hh"
#include "StXiVertex.h"
#include "StTrack.h"
#include "StTrackFitTraits.h"
#include "StStrangeEvMuDst.hh"
#include "StDedxPidTraits.h"

ClassImp(StXiMuDst)

StXiMuDst::StXiMuDst() : StXiI() { 
}

void StXiMuDst::Fill(StXiVertex* xiVertex, 
		       StV0Vertex* v0Vertex,
		       StStrangeEvMuDst* event) {
  StV0MuDst::Fill(v0Vertex, event);
  FillXi(xiVertex);
}

void StXiMuDst::FillXi(StXiVertex* xiVertex) {
  mCharge = (Int_t) (xiVertex->chargeOfBachelor());
  mDecayVertexXiX = xiVertex->position().x();
  mDecayVertexXiY = xiVertex->position().y();
  mDecayVertexXiZ = xiVertex->position().z();
  mDcaXiDaughters = xiVertex->dcaDaughters();
  mDcaBachelorToPrimVertex = xiVertex->dcaBachelorToPrimaryVertex();
  mDcaXiToPrimVertex = xiVertex->dcaParentToPrimaryVertex();
  mMomBachelorX = xiVertex->momentumOfBachelor().x();
  mMomBachelorY = xiVertex->momentumOfBachelor().y();
  mMomBachelorZ = xiVertex->momentumOfBachelor().z();
  mChi2Xi = xiVertex->chiSquared();
  mClXi = xiVertex->probChiSquared();

  StTrack* trk = xiVertex->bachelor();
  mKeyBachelor  = trk->key();
  mTopologyMapBachelor = trk->topologyMap();
  mChi2Bachelor = trk->fitTraits().chi2(0);
  mClBachelor = trk->fitTraits().chi2(1);
  // For now, get the truncated mean dE/dX from the TPC
  StPtrVecTrackPidTraits pidBachelor = trk->pidTraits(kTpcId);
  for (UInt_t i=0; i<pidBachelor.size(); i++) {
    StDedxPidTraits* pid = (StDedxPidTraits*) pidBachelor[i];
    if (pid->method() == kTruncatedMeanId) {
      mDedxBachelor = pid->mean();
      mNumDedxBachelor = pid->numberOfPoints();
      break;
    }
  }
}

StXiMuDst::~StXiMuDst() {
}

Long_t StXiMuDst::detectorIdXi() {
  return ((100*detectorIdV0())+
               detectorIdTrack(mTopologyMapBachelor));
}

Long_t StXiMuDst::detectorIdPars() {
  return 1;  // Currently, only one set of parameters actually used
}
