//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StMuDstVtxSeedMaker class                                            //
// Author: G. Van Buren, BNL                                            //
// Description: calculates mean primary vertex positions from           //
//              suitable events to use as seeds in finding better       //
//              primary vertex positions (helpful for low               //
//              multiplicity events like pp collisions)                 //
//              using MuDst                                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "StMuDstVtxSeedMaker.h"
#include "StMuDSTMaker/COMMON/StMuDstMaker.h"
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuPrimaryVertex.h"
#include "StMessMgr.h"


ClassImp(StMuDstVtxSeedMaker)
//_____________________________________________________________________________
StMuDstVtxSeedMaker::StMuDstVtxSeedMaker(const char *name):
  StVertexSeedMaker(name), mudst(0), event(0) {
}
//_____________________________________________________________________________
Int_t StMuDstVtxSeedMaker::Make() {
  StMuDstMaker* mudstMaker = (StMuDstMaker*) GetMaker("MuDst");
  if (!mudstMaker) {
    gMessMgr->Error("StMuDstVtxSeedMaker: No StMuDstMaker(\"MuDst\") found!");
    return kStErr;
  }
  mudst = mudstMaker->muDst();
  if (!mudst) {
    gMessMgr->Error("StMuDstVtxSeedMaker: No MuDst found!");
    return kStErr;
  }
  event = mudst->event();
  if (!event) {
    gMessMgr->Error("StMuDstVtxSeedMaker: No MuEvent found!");
    return kStErr;
  }
 
  Int_t result = kStOk;
  for (pvn=0; pvn<mudst->numberOfPrimaryVertices(); pvn++) {
    result = StVertexSeedMaker::Make();
    if (result != kStOk) break;
  }

  return result;
}
//_____________________________________________________________________________
Bool_t StMuDstVtxSeedMaker::CheckTriggers() {
  Bool_t notTrig = kTRUE;

  // Check trigger ids
  StMuTriggerIdCollection& trigIdColl = event->triggerIdCollection();
  const StTriggerId& tr = trigIdColl.nominal();
  vector<unsigned int> idVec = tr.triggerIds();
  for (unsigned int iTrg = 0;
       (notTrig) && (iTrg < idVec.size()) ; iTrg++) {
    if (ValidTrigger(idVec[iTrg])) notTrig = kFALSE;
  }
  return notTrig;
}
//_____________________________________________________________________________
Int_t StMuDstVtxSeedMaker::GetEventData() {
  // Get primary vertex from MuEvent
  StMuPrimaryVertex* primVtx = mudst->primaryVertex(pvn);
  if (!primVtx) {
    gMessMgr->Error("StMuDstVtxSeedMaker: No primary vertex from StMuDst!");
    return kStErr;
  }
  StRunInfo& runInfo = event->runInfo();
  zdc = (float) (runInfo.zdcWestRate() + runInfo.zdcEastRate());
  fill = (int) (runInfo.beamFillNumber(blue));
  run = runInfo.runId();

  StThreeVectorF pvert = primVtx->position();
  zvertex = pvert.z();
  yvertex = pvert.y();
  xvertex = pvert.x();

  mult = (float) (primVtx->nTracksUsed());
  rank = primVtx->ranking();

  // hits not saved in MuDst
  itpc = 0; otpc = 0;

  return kStOk;
}
//_____________________________________________________________________________
void StMuDstVtxSeedMaker::PrintInfo() {
  LOG_INFO << "\n**************************************************************"
           << "\n* $Id: StMuDstVtxSeedMaker.cxx,v 1.5 2007/05/16 02:59:25 genevb Exp $"
           << "\n**************************************************************" << endm;

  if (Debug()) StVertexSeedMaker::PrintInfo();
}
//_____________________________________________________________________________
// $Id: StMuDstVtxSeedMaker.cxx,v 1.5 2007/05/16 02:59:25 genevb Exp $
// $Log: StMuDstVtxSeedMaker.cxx,v $
// Revision 1.5  2007/05/16 02:59:25  genevb
// printf => LOG_INFO
//
// Revision 1.4  2006/09/01 22:27:16  genevb
// More detailed info in ntuple
//
// Revision 1.3  2005/07/01 21:46:59  genevb
// Remove extraneous print statement
//
// Revision 1.2  2005/07/01 21:46:01  genevb
// Specify output directory
//
// Revision 1.1  2005/06/14 18:52:20  genevb
// Introduction of code to use MuDst for beamline constraint
//
//
