/***************************************************************************
 *
 * $Id: StTriggerDetectorCollection.cxx,v 2.0 1999/10/12 18:43:18 ullrich Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTriggerDetectorCollection.cxx,v $
 * Revision 2.0  1999/10/12 18:43:18  ullrich
 * Completely Revised for New Version
 *
 * Revision 2.0  1999/10/12 18:43:18  ullrich
 * Completely Revised for New Version
 *
 **************************************************************************/
#include "StTriggerDetectorCollection.h"
#include "tables/dst_TrgDet.h"

static const char rcsid[] = "$Id: StTriggerDetectorCollection.cxx,v 2.0 1999/10/12 18:43:18 ullrich Exp $";

ClassImp(StTriggerDetectorCollection)

StTriggerDetectorCollection::StTriggerDetectorCollection() {/* noop */}

StTriggerDetectorCollection::StTriggerDetectorCollection(const dst_TrgDet_st& t) :
    mCtb(t), mMwc(t), mVpd(t), mZdc(t) {/* noop */}

StTriggerDetectorCollection::~StTriggerDetectorCollection() {/* noop */}

StCtbTriggerDetector&
StTriggerDetectorCollection::ctb() { return mCtb; }

const StCtbTriggerDetector&
StTriggerDetectorCollection::ctb() const { return mCtb; }

StMwcTriggerDetector&
StTriggerDetectorCollection::mwc() { return mMwc; }

const StMwcTriggerDetector&
StTriggerDetectorCollection::mwc() const { return mMwc; }

StVpdTriggerDetector&
StTriggerDetectorCollection::vpd() { return mVpd; }

const StVpdTriggerDetector&
StTriggerDetectorCollection::vpd() const { return mVpd; }

StZdcTriggerDetector&
StTriggerDetectorCollection::zdc() { return mZdc; }

const StZdcTriggerDetector&
StTriggerDetectorCollection::zdc() const { return mZdc; }

