// -*- mode: c++;-*-
// $Id: StjTowerEnergyCutBemcWestOnly.h,v 1.5 2008/08/04 02:48:46 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef STJTOWERENERGYCUTBEMCWESTONLY_H
#define STJTOWERENERGYCUTBEMCWESTONLY_H

#include "StjTowerEnergyCut.h"

class StjTowerEnergyCutBemcWestOnly : public StjTowerEnergyCut {

public:
  StjTowerEnergyCutBemcWestOnly() { }
  virtual ~StjTowerEnergyCutBemcWestOnly() { }

  bool operator()(const StjTowerEnergy& tower)
  {
    if(tower.detectorId != 9) return true;

    if(tower.towerId > 2400) return true;

    return false;
  }

private:
  
  ClassDef(StjTowerEnergyCutBemcWestOnly, 1)

};

#endif // STJTOWERENERGYCUTBEMCWESTONLY_H
