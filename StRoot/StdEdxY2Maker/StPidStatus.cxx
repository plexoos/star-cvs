#include "StPidStatus.h"
#include "StBichsel/Bichsel.h"
#include "StTrackGeometry.h"
#include "TMath.h"
//________________________________________________________________________________
StPidStatus::StPidStatus(StGlobalTrack *Track) : PiDStatus(-1), gTrack(Track) {
  Clear();
  if (! gTrack) return;
  StSPtrVecTrackPidTraits &traits = gTrack->pidTraits();
  if (! traits.size()) return;
  for (UInt_t i = 0; i < traits.size(); i++) {
    if (! traits[i]) continue;
    if ( traits[i]->IsZombie()) continue;
    Short_t id = traits[i]->detector();
    if (id == kTofId) {
      StBTofPidTraits* pid = dynamic_cast<StBTofPidTraits*>(traits[i]);
      if (! pid) continue;
      if (TMath::Abs(pid->yLocal()) < 1.8) fToF = StToFStatus(pid);
    } else if (id == kTpcId) {
      StDedxPidTraits* pid = dynamic_cast<StDedxPidTraits*>(traits[i]);
      if (pid) {
	switch (pid->method()) {
	case kTruncatedMeanId: fI70 = StdEdxStatus(pid); break;
	case kLikelihoodFitId: fFit = StdEdxStatus(pid); break;
	case kEnsembleTruncatedMeanId: fI70U = StdEdxStatus(pid); break;// == kTruncatedMeanId+1 uncorrected
	case kWeightedTruncatedMeanId: fFitU = StdEdxStatus(pid); break;  // == kLikelihoodFitId+1; uncorrected
	default: break;
	}
      } else {
	StProbPidTraits *pidprob = dynamic_cast<StProbPidTraits*>(traits[i]);
	if (pidprob) fProb = pidprob;
      }
    }
  }
  if (! fI70.fPiD || ! fFit.fPiD) return;
  PiDStatus = 0;
  StThreeVectorD g3 = gTrack->geometry()->momentum(); // p of global track
  Double_t pMomentum = g3.mag();
  //  Double_t bg = TMath::Log10(pMomentum/StProbPidTraits::mPidParticleDefinitions[kPidPion]->mass());
  Int_t l;
  PredBMN[0] = Pred70BMN[0] =  1;
  PredBMN[1] = Pred70BMN[1] = -1;
  Bichsel *b = Bichsel::Instance();
  for (l = kPidElectron; l < KPidParticles; l += 1) {
    bghyp[l] = TMath::Log10(pMomentum*TMath::Abs(StProbPidTraits::mPidParticleDefinitions[l]->charge())/StProbPidTraits::mPidParticleDefinitions[l]->mass());
    PredB[l]   = 1.e-6*StProbPidTraits::mPidParticleDefinitions[l]->charge()*StProbPidTraits::mPidParticleDefinitions[l]->charge()*
      TMath::Exp(b->GetMostProbableZ(bghyp[l],fFit.fPiD->log2dX())); 
    PredBT[l]   = 1.e-6*StProbPidTraits::mPidParticleDefinitions[l]->charge()*StProbPidTraits::mPidParticleDefinitions[l]->charge()*
      TMath::Exp(b->GetMostProbableZ(bghyp[l]));
    if (PredBT[l] < PredBMN[0]) PredBMN[0] = PredBT[l];
    if (PredBT[l] > PredBMN[1]) PredBMN[1] = PredBT[l];
    Pred70B[l] = 1.e-6*StProbPidTraits::mPidParticleDefinitions[l]->charge()*StProbPidTraits::mPidParticleDefinitions[l]->charge()*
      b->GetI70M(bghyp[l],fI70.fPiD->log2dX()); 
    Pred70BT[l] = 1.e-6*StProbPidTraits::mPidParticleDefinitions[l]->charge()*StProbPidTraits::mPidParticleDefinitions[l]->charge()*
      b->GetI70M(bghyp[l]); 
    if (Pred70B[l] < Pred70BMN[0]) Pred70BMN[0] = Pred70BT[l];
    if (Pred70B[l] > Pred70BMN[1]) Pred70BMN[1] = Pred70BT[l];
    if (fI70.fPiD) {
      devZ[l]  = TMath::Log(fI70.I()/Pred70BT[l]);
      devZs[l] = TMath::Abs(devZ[l])/fI70.D();
    }
  }
  PiDkey    = -1; // best
  PiDkeyU   = -1; // only one with devZs<3, 
  PiDkeyU3  = -1; // -"- and devZs > 5 for all others 
  lBest     = -1;
  Int_t lBestToF = -1;
  // use ToF 
  Double_t devZmin = 999;
  if (fToF.fPiD) {
    for (l = kPidElectron; l < KPidParticles; l++) {
      if      (l == kPidMuon    ) devToF[l] = devToF[kPidPion];
      else if (l >= kPidDeuteron) devToF[l] = devToF[kPidProton];
      else                        devToF[l] = TMath::Abs(fToF.Sigma(l));
    }
    for (l = kPidElectron; l < KPidParticles; l++) {
      if (devToF[l] < 3.0) PiDStatus |= 1<<l;
      if (devToF[l] < devZmin) {devZmin = devToF[l]; lBestToF = l;}
    }
    if (devZmin > 5) {lBestToF = -1;}
  }
  devZmin = 999;
  for (l = kPidElectron; l < KPidParticles; l++) {
    if (devZs[l] < 3.0) PiDStatus |= 1<<l;
    Double_t dev = devZs[l]*devZs[l];
    if (lBestToF >= 0) dev += devToF[l]*devToF[l];
    if (dev < devZmin) {
      lBest = l;
      devZmin = dev;
    }
  }
  if (lBest >=0) {
    if (devZs[lBest] < 3.0) {
      PiDkey = lBest;
      Int_t lNext = -1;
      devZmin = 999;
      for (l = kPidElectron; l < KPidParticles; l++) {
	if (l == lBest) continue;
	Double_t dev = devZs[l]*devZs[l];
	if (lBestToF >= 0) dev += devToF[l]*devToF[l];
	if (dev < devZmin) {devZmin = dev; lNext = l;}
      }
      if (lNext >= 0) {
	Double_t dev = devZs[lNext]*devZs[lNext];
	if (lBestToF >= 0) dev += devToF[lNext]*devToF[lNext];
	if (dev > 9.) {
	  PiDkeyU = PiDkey;
	  if (dev > 25.) PiDkeyU3 = PiDkeyU;
	}
      }
    }
  }
}
//________________________________________________________________________________
