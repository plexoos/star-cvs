// -*- mode: c++;-*-
// $Id: StJetTowerEnergyListReader.h,v 1.1 2008/07/24 20:57:13 tai Exp $
#ifndef STJETTOWERENERGYLISTREADER_H
#define STJETTOWERENERGYLISTREADER_H

#include "TowerEnergyList.h"
#include <Rtypes.h>

class TTree;

class StJetTowerEnergyListReader {

public:
  StJetTowerEnergyListReader(TTree *tree);
  virtual ~StJetTowerEnergyListReader() { }

  StSpinJet::TowerEnergyList GetEntry(Long64_t entry);

private:

  TTree* _tree;

  Int_t    _runNumber;
  Int_t    _eventId;
  Int_t    _detectorId; // 9: BEMC, 13: EEMC
  Int_t    _nTowers;
  Int_t    _towerId[4800];
  Double_t _towerR[4800];
  Double_t _towerEta[4800];
  Double_t _towerPhi[4800];
  Double_t _vertexX[4800];
  Double_t _vertexY[4800];
  Double_t _vertexZ[4800];
  Double_t _energy[4800];
  UInt_t   _adc[4800];
  Double_t _pedestal[4800];
  Double_t _rms[4800];
  Int_t    _status[4800];     // 1 is good for BEMC. 0 is good for EEMC

};

#endif // STJETTOWERENERGYLISTREADER_H
