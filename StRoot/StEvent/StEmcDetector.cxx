/***************************************************************************
 *
 * $Id: StEmcDetector.cxx,v 2.6 2000/12/08 03:39:50 ullrich Exp $
 *
 * Author: Akio Ogawa, Jan 2000
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StEmcDetector.cxx,v $
 * Revision 2.6  2000/12/08 03:39:50  ullrich
 * Fixed warning because of signed/unsigned comparison.
 *
 * Revision 2.5  2000/10/26 00:02:21  ullrich
 * Fixed various problems causing I/O failures.
 *
 * Revision 2.4  2000/07/28 19:49:27  akio
 * Change in Detector Id for Endcap SMD
 *
 * Revision 2.3  2000/06/30 17:23:19  akio
 * minor bug fix for return kFalse
 *
 * Revision 2.2  2000/05/22 19:21:53  akio
 * Bug fix, add delta into EMcPoint, wider bits for Eta in RawHit
 *
 * Revision 2.1  2000/02/23 17:34:08  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "StEmcDetector.h"
#include "StEmcRawHit.h"
#include "StEmcModule.h"
#include "StEmcClusterCollection.h"

static const char rcsid[] = "$Id: StEmcDetector.cxx,v 2.6 2000/12/08 03:39:50 ullrich Exp $";

ClassImp(StEmcDetector)

StEmcDetector::StEmcDetector() { /* noop */ }

StEmcDetector::StEmcDetector(StDetectorId id, UInt_t n)
{
    mDetectorId = id;
    mNumberOfModules = n;
    for(Int_t i=0; i<120;i++)
    {
      StEmcModule * module = new StEmcModule();
      this->setModule(module,i);
    }

}

StEmcDetector::~StEmcDetector()
{
    for(Int_t i=0; i<120;i++) if(mModules[i]) delete mModules[i];
    if (mClusters) delete mClusters;
}

Bool_t
StEmcDetector::addHit(StEmcRawHit* hit)
{
    if (hit){
      UInt_t m = hit->module();
      if (m > 0 && m <= mNumberOfModules)
      {
	      mModules[m-1]->hits().push_back(hit);
	      return kTRUE;
      }
    }
    return kFALSE;
}

StDetectorId
StEmcDetector::detectorId() const { return mDetectorId; }

UInt_t
StEmcDetector::numberOfModules() const { return mNumberOfModules; }

UInt_t
StEmcDetector::numberOfHits() const
{
    UInt_t sum = 0;
    for (UInt_t m=0;m<mNumberOfModules;m++) sum+= mModules[m]->hits().size();
    return sum;
}

StEmcModule*
StEmcDetector::module(UInt_t i)
{
    if (i > 0 && i <= mNumberOfModules) return (mModules[i-1]);
    else return 0;
}

const StEmcModule*
StEmcDetector::module(UInt_t i) const
{
    if (i > 0 && i <= mNumberOfModules) return (mModules[i-1]);
    else return 0;
}

StEmcClusterCollection*
StEmcDetector::cluster() {return mClusters;}

const StEmcClusterCollection*
StEmcDetector::cluster() const {return mClusters;}

void
StEmcDetector::setCluster(StEmcClusterCollection* val)
{
    if (mClusters) delete mClusters;
    mClusters = val;
}

void
StEmcDetector::setModule(StEmcModule* val,Int_t IdMod)
{
    if (val)
    { 
      if (IdMod >= 0 && IdMod < static_cast<int>(mNumberOfModules)) 
      {
        if (mModules[IdMod]) delete mModules[IdMod];
        mModules[IdMod] = val;
      }
   }
}     
