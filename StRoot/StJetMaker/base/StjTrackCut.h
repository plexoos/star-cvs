// -*- mode: c++;-*-
// $Id: StjTrackCut.h,v 1.5 2008/08/04 02:37:22 tai Exp $
// Copyright (C) 2008 Tai Sakuma <sakuma@bnl.gov>
#ifndef STJTRACKCUT_H
#define STJTRACKCUT_H

#include <TObject.h>

#include "StjTrackList.h"

class StjTrackCut : public TObject {

public:
  StjTrackCut() { }
  virtual ~StjTrackCut() { }

  virtual bool operator()(const StjTrack& track) = 0;

  ClassDef(StjTrackCut, 1)

};

#endif // STJTRACKCUT_H
