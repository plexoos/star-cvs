// -*- mode: c++;-*-
// $Id: StjMCParticleListCut.h,v 1.5 2008/08/04 06:10:25 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef STJMCPARTICLELISTCUT_H
#define STJMCPARTICLELISTCUT_H

#include <TObject.h>

#include "StjMCParticleCut.h"

#include "StjMCParticleList.h"

class StjMCParticleListCut : public TObject {

public:
  StjMCParticleListCut() { }
  virtual ~StjMCParticleListCut() { }
  
  StjMCParticleList operator()(const StjMCParticleList& aList);

  void addCut(StjMCParticleCut* cut) {
    _cutList.push_back(cut);
  }

  typedef std::vector<StjMCParticleCut*> CutList;
  CutList getCutList() { return _cutList; }

private:

  bool shouldNotKeep(const StjMCParticle& p);

  CutList _cutList;

  ClassDef(StjMCParticleListCut, 1)

};

#endif // STJMCPARTICLELISTCUT_H
