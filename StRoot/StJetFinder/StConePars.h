// -*- mode: c++;-*-
// $Id: StConePars.h,v 1.7 2008/04/25 01:50:59 tai Exp $
#ifndef STCONEPARS_H
#define STCONEPARS_H

#include "StJetPars.h"

#include <cmath>

class StConePars : public StJetPars {

public:

  virtual StJetFinder* constructJetFinder();

  ///Set the grid spacing:
  void setGridSpacing(int nEta, double etaMin, double etaMax,
		      int nPhi, double phiMin, double phiMax)
  {
    mNeta = nEta; mEtaMin = etaMin; mEtaMax = etaMax;
    mNphi = nPhi; mPhiMin = phiMin; mPhiMax = phiMax;
  }
    
  ///minimum et threshold to be considered a seed
  void setSeedEtMin(double v) { mSeedEtMin = v; }

  ///minimum et threshold to be considered for addition to the seed
  void setAssocEtMin(double v) { mAssocEtMin = v; }

  ///split jets if E_shared/E_neighbor>splitFraction
  void setSplitFraction(double v) { mSplitFraction = v; }

  ///Let jet wander to minimum?
  void setPerformMinimization(bool v) {mDoMinimization=v;}

  ///Add seeds at midpoints?
  void setAddMidpoints(bool v) {mAddMidpoints=v;}

  ///Do Split/Merge step?
  void setDoSplitMerge(bool v) {mDoSplitMerge=v;}

  ///Require stable midpoints?
  void setRequireStableMidpoints(bool v) {mRequireStableMidpoints=v;}

  ///Set cone radius:
  void setConeRadius(double v) {mR = v;}

  ///Toggle debug streams on/off
  void setDebug(bool v) {mDebug = v;}

  int    Neta()   const { return mNeta; }
  int    Nphi()	  const { return mNphi; } 
  double EtaMin() const { return mEtaMin; }
  double EtaMax() const { return mEtaMax; }
  double PhiMin() const { return mPhiMin; }
  double PhiMax() const { return mPhiMax; }

  double coneRadius() const {return mR;}

  double seedEtMin() const { return mSeedEtMin; }
  double assocEtMin() const { return mAssocEtMin; }

  double splitFraction() const { return mSplitFraction; }

  bool performMinimization() const {return mDoMinimization;}

  bool addMidpoints() const {return mAddMidpoints;}

  bool requiredStableMidpoints() const {return mRequireStableMidpoints;}

  bool doSplitMerge() const {return mDoSplitMerge;}

  bool debug() const {return mDebug;}
    
  double phiWidth() const { return (PhiMax() - PhiMin())/static_cast<double>(Nphi()); }
  double etaWidth() const { return (EtaMax() - EtaMin())/static_cast<double>(Neta()); }
  int    deltaPhi() const { return static_cast<int>(floor(coneRadius()/phiWidth())) + 1; }
  int    deltaEta() const { return static_cast<int>(floor(coneRadius()/etaWidth())) + 1; }


private:

  int mNeta;
  int mNphi;
  double mEtaMin;
  double mEtaMax;
  double mPhiMin;
  double mPhiMax;
  double mR;
  double mSeedEtMin;
  double mAssocEtMin;
  double mSplitFraction;
  bool mDoMinimization;
  bool mAddMidpoints;
  bool mRequireStableMidpoints;
  bool mDoSplitMerge;
  bool mDebug;

  ClassDef(StConePars,1)
};

#endif // STCONEPARS_H
