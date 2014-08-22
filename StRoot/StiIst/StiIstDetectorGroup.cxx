
#include "StEvent.h"
#include "StiIstDetectorGroup.h"
#include "StiIstDetectorBuilder.h"
#include "StiIstHitLoader.h"


StiIstDetectorGroup::StiIstDetectorGroup(bool active)
   : StiDetectorGroup<StEvent>("Ist",
                               active ? new StiIstHitLoader() : 0,
                               new StiIstDetectorBuilder(active))
{}

StiIstDetectorGroup::~StiIstDetectorGroup()
{}
