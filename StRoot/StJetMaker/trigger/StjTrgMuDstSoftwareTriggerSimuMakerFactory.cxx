// $Id: StjTrgMuDstSoftwareTriggerSimuMakerFactory.cxx,v 1.1 2008/08/08 23:12:27 tai Exp $
#include "StjTrgMuDstSoftwareTriggerSimuMakerFactory.h"

#include "StjTrgMuDstSoftwareTriggerSimuMaker.h"

StjTrgMuDstSoftware* StjTrgMuDstSoftwareTriggerSimuMakerFactory::create()
{
  return new StjTrgMuDstSoftwareTriggerSimuMaker(_simuTrig);
}

