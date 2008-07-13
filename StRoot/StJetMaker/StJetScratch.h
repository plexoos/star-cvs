// -*- mode: c++;-*-
// $Id: StJetScratch.h,v 1.3 2008/07/13 00:05:25 tai Exp $
#ifndef STJETSCRATCH_HH
#define STJETSCRATCH_HH

#include "StMaker.h"
#include <Rtypes.h>

class TDirectory;
class TTree;

class StJetTrgWriter;

class StMuDstMaker;

namespace StSpinJet {
  class StJetTPC;
  class StJetBEMC;
  class StJetEEMC;
}

class StJetScratch : public StMaker {

public:

  StJetScratch(const Char_t *name, TDirectory* file, StMuDstMaker* uDstMaker);
  virtual ~StJetScratch() { }

  Int_t Init();
  Int_t Make();
  Int_t Finish();
    
  const char* GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StJetScratch.h,v 1.3 2008/07/13 00:05:25 tai Exp $ built "__DATE__" "__TIME__; return cvs;}

private:

  TDirectory* _file;

  StMuDstMaker* _uDstMaker;

  StSpinJet::StJetBEMC* _bemc;

  TTree* _tree;

  Int_t _runNumber;
  Int_t _eventId;
  Int_t _nTowers;
  Int_t    _detectorId[4800]; // 9: BEMC, 13: EEMC
  Int_t    _towerId[4800];
  Double_t _towerX[4800];
  Double_t _towerY[4800];
  Double_t _towerZ[4800];
  Double_t _vertexX[4800];
  Double_t _vertexY[4800];
  Double_t _vertexZ[4800];
  Double_t _energy[4800];
  UInt_t   _adc[4800];
  Double_t _pedestal[4800];
  Double_t _rms[4800];
  Int_t    _status[4800];     // 1 is good for BEMC. 0 is good for EEMC

  ClassDef(StJetScratch, 0)

};

#endif // STJETSCRATCH_HH
