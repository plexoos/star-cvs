#include "StRareTrack.h"
#include "StEventTypes.h"
#include "StMessMgr.h"
double dEdx_formula(double momentum, double mass);
ClassImp(StRareTrack)

StRareTrack::StRareTrack(){}
StRareTrack::StRareTrack(StPrimaryTrack* track){
  StGlobalTrack* gtrack = static_cast<StGlobalTrack*>((StTrack*)track);
  fdca = gtrack->impactParameter();
  fnpntpossible = track->numberOfPossiblePoints();
  ftracknumber = track->key();
  fiflag = track->flag();
  fchisqxy = track->fitTraits().chi2(0);
  fchisqz = track->fitTraits().chi2(1);
  fnpntfit = track->fitTraits().numberOfFitPoints();
  fpx = track->geometry()->momentum().x();
  fpy = track->geometry()->momentum().y();
  fpz = track->geometry()->momentum().z();
  fdedxPion = dedxPi();
  fdedxProton = dedxProton();
  fdedxDeuteron = dedxDeuteron();
  fdedxTriton = dedxTriton();
  fdedxHe3 = dedxHe3();
  fdedxHe4 = dedxHe4();
  fchargesign = track->geometry()->charge();
  ftrigtype = 0;
  fdedx = 0.0;
  fndedx = 0;
  StSPtrVecTrackPidTraits& traits=track->pidTraits();
  StDedxPidTraits* dedxPidTr;
   for (int itrait = 0; itrait < traits.size(); itrait++){
     dedxPidTr = 0;
      if (traits[itrait]->detector() == kTpcId) {
             StTrackPidTraits* thisTrait = traits[itrait];
  	     dedxPidTr = dynamic_cast<StDedxPidTraits*>(thisTrait);
	     if (dedxPidTr && dedxPidTr->method() == kTruncatedMeanId) {
              fdedx = 1e6*dedxPidTr->mean();
              fndedx = dedxPidTr->numberOfPoints();
             }
      }
   }

}

float StRareTrack::dedxExpected(float mass, float charge = 1) const {
  float real_mom = p()*charge;
  float dedx = charge*charge*dEdx_formula(real_mom,mass);
  return dedx;
} 

void  StRareTrack::SetTrigType(int type) {ftrigtype = type;}
