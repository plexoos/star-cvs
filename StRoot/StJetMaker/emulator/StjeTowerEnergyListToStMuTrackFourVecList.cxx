// $Id: StjeTowerEnergyListToStMuTrackFourVecList.cxx,v 1.4 2009/12/09 05:12:12 pibero Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#include "StjeTowerEnergyListToStMuTrackFourVecList.h"

#include "StjTowerEnergyToTLorentzVector.h"

#include "../emulator/StMuTrackFourVec.h"
#include "../emulator/StMuTowerEmu.h"

StjeTowerEnergyListToStMuTrackFourVecList::StjeTowerEnergyListToStMuTrackFourVecList()
  : _energyTo4p(*(new StjTowerEnergyToTLorentzVector)) { }

FourList StjeTowerEnergyListToStMuTrackFourVecList::operator()(const StjTowerEnergyList& energyDepositList)
{
  FourList ret;

  for(StjTowerEnergyList::const_iterator tower = energyDepositList.begin(); tower != energyDepositList.end(); ++tower) {

    TLorentzVector p4 = _energyTo4p(*tower);

    StMuTrackFourVec* pmu = new StMuTrackFourVec(0, createTowerEmu(*tower), 0, p4, 0, tower->towerId, tower->detectorId);

    ret.push_back(pmu);
  }

  return ret;
}

StMuTowerEmu* StjeTowerEnergyListToStMuTrackFourVecList::createTowerEmu(const StjTowerEnergy& tower)
{
  StMuTowerEmu* ret = new StMuTowerEmu;

  TVector3 mom;
  mom.SetPtEtaPhi(tower.towerR, tower.towerEta, tower.towerPhi);
  mom.SetMag(tower.energy);

  ret->_px         = mom.Px();
  ret->_py         = mom.Py();
  ret->_pz         = mom.Pz();
  ret->_adc        = tower.adc;
  ret->_pedestal   = tower.pedestal;
  ret->_rms        = tower.rms;
  ret->_status     = tower.status;
  ret->_id         = tower.towerId;
  ret->_detectorId = tower.detectorId;

  return ret;
}
