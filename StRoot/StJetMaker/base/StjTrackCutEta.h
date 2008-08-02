// -*- mode: c++;-*-
// $Id: StjTrackCutEta.h,v 1.3 2008/08/02 22:43:21 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef STJTRACKCUTETA_H
#define STJTRACKCUTETA_H

#include "StjTrackCut.h"

namespace StJetTrackCut {

class StjTrackCutEta : public StjTrackCut {

public:
  StjTrackCutEta(double min = -2.0, double max = 2.0) :_min(min), _max(max) { }
  virtual ~StjTrackCutEta() { }

  bool operator()(const StSpinJet::StjTrack& track)
  {
    if(track.eta < _min) return true;

    if(track.eta > _max) return true;

    return false;
  }

private:

  double  _min;
  double  _max;

};

}

#endif // STJTRACKCUTETA_H
