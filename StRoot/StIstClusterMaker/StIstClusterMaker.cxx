#include "StIstClusterMaker.h"
#include "StEvent.h"
#include "StEvent/StEnumerations.h"
#include "StRoot/StIstUtil/StIstCollection.h"
#include "StRoot/StIstUtil/StIstRawHit.h"
#include "StRoot/StIstUtil/StIstRawHitCollection.h"
#include "StRoot/StIstUtil/StIstCluster.h"
#include "StRoot/StIstUtil/StIstClusterCollection.h"
#include "StRoot/StIstUtil/StIstConsts.h"
#include "StRoot/StIstClusterMaker/StIstIClusterAlgo.h"
#include "StRoot/StIstClusterMaker/StIstScanClusterAlgo.h"

StIstClusterMaker::StIstClusterMaker( const char *name ) : StMaker(name), mIstCollectionPtr(0), mClusterAlgoPtr(0), mTimeBin(-1), mSplitCluster(true)
{
   /* nothing to do */
};

StIstClusterMaker::~StIstClusterMaker()
{
   delete mIstCollectionPtr;
   delete mClusterAlgoPtr;
};

void StIstClusterMaker::Clear( Option_t *opts )
{
   if ( mIstCollectionPtr ) {
      for ( unsigned char i = 0; i < kIstNumLadders; ++i ) {
         mIstCollectionPtr->getRawHitCollection(i)->Clear( "" );
         mIstCollectionPtr->getClusterCollection(i)->Clear( "" );
      }
   }
};

Int_t StIstClusterMaker::Make()
{
   //input data
   TObjectSet *istDataSet = (TObjectSet *)GetDataSet("istRawHitAndCluster");

   if (! istDataSet) {
      LOG_WARN << "Make() - there is no istDataSet (raw hit and cluster) " << endm;
      return kStWarn;
   }

   mIstCollectionPtr = (StIstCollection *)istDataSet->GetObject();

   if (!mIstCollectionPtr) {
      LOG_WARN << "Make() - no istCollection." << endm;
      return kStWarn;
   }

   mClusterAlgoPtr->setUsedTimeBin(mTimeBin);
   mClusterAlgoPtr->setSplitFlag(mSplitCluster);
   mClusterAlgoPtr->doClustering(*mIstCollectionPtr);

   LOG_DEBUG << "End of StIstClusterMaker, print all raw hits & clusters: " << endm
             << "Total raw hits=" << mIstCollectionPtr->getNumRawHits()
             << ", total Clusters=" <<  mIstCollectionPtr->getNumClusters() << endm;

   if (Debug() > 2) {
      for (unsigned char iLadder = 0; iLadder < kIstNumLadders; iLadder++)
      {
         LOG_DEBUG << "Content: iLadder=" << (short) iLadder + 1
                   << " # of : raw hits=" << mIstCollectionPtr->getNumRawHits(iLadder)
                   << "  clusters=" << mIstCollectionPtr->getNumClusters( iLadder) << endm;

         // ..... print all raw hits ....
         StIstRawHitCollection *rawHitPtr = mIstCollectionPtr->getRawHitCollection(iLadder);
         size_t nTimeBins = mIstCollectionPtr->getNumTimeBins();
         rawHitPtr->Print(nTimeBins);

         // ..... print all 1D clusters  ....
         StIstClusterCollection *clustPtr = mIstCollectionPtr->getClusterCollection(iLadder);
         clustPtr->Print();
      }
   }

   return kStOk;
}

void StIstClusterMaker::setClusterAlgo(StIstIClusterAlgo *algo)
{
   mClusterAlgoPtr = algo;
}

Int_t StIstClusterMaker::Init()
{
   Int_t ierr = kStOk;

   if ( !mClusterAlgoPtr ) {
      LOG_INFO << "IST clustering algorithm: Scanning algorithm" << endm;
      mClusterAlgoPtr = new StIstScanClusterAlgo();
   }

   return ierr;
};

ClassImp(StIstClusterMaker);


/***************************************************************************
*
* $Log: StIstClusterMaker.cxx,v $
* Revision 1.27  2014/10/14 21:05:54  smirnovd
* Don't wait until the end of routine to return error codes. Leads to somewhat cleaner code and eliminates if statements
*
* Revision 1.26  2014/09/17 20:36:26  smirnovd
* StIstClusterMaker: Use the updated public interface of StIstIClusterAlgo. The
* previous functionality has been moved to the base class
*
* Revision 1.25  2014/09/17 20:33:31  smirnovd
* Squashed commit of the following:
*
* commit 72dc19a6663ea31c719c1a61f6d2b4752dd766aa
* Author: Dmitri Smirnov <d.s@plexoos.com>
* Date:   Wed Sep 17 12:34:42 2014 -0400
*
*     Minor code refactoring, clean up
*
* commit e083a10a9fb60b7dcce692ef8043b9227c12768b
* Author: Dmitri Smirnov <d.s@plexoos.com>
* Date:   Wed Sep 17 12:18:16 2014 -0400
*
*     Removed pointless comments
*
* commit 88d51857362c91c954704cec4a31a0b0fa7fccc5
* Author: Dmitri Smirnov <d.s@plexoos.com>
* Date:   Wed Sep 17 12:17:26 2014 -0400
*
*     Updated description in doxygen comments
*
* commit eb09527489179fc7dab6aa7f23fd132b25185bb1
* Author: Dmitri Smirnov <d.s@plexoos.com>
* Date:   Tue Sep 9 15:15:56 2014 -0400
*
*     StIstScanClusterAlgo: Removed unused variable
*
* commit 1a8df63533c71a0e2ba4d8275ebf89f4e3004765
* Author: Dmitri Smirnov <d.s@plexoos.com>
* Date:   Fri Aug 22 16:04:47 2014 -0400
*
*     Neatened headers: Removed unused, spelled paths in includes explicitly as it slightly helps in identifying dependencies
*
* commit 972e8ed41403bd680ade5ecc509f8bca004e86ee
* Author: Dmitri Smirnov <d.s@plexoos.com>
* Date:   Wed Sep 17 12:34:20 2014 -0400
*
*     Minor stylistic changes
*
* commit 57daf5a1e0b3246fd12f1dd1c2ca089b62930c83
* Author: Dmitri Smirnov <d.s@plexoos.com>
* Date:   Tue Sep 16 16:29:14 2014 -0400
*
*     Improved doxygen comments
*
* Revision 1.24  2014/09/09 15:50:09  smirnovd
* StIstClusterMaker: Refactored conditional statements and added a formal warning for missing collections. Adjusted white space indentation
*
* Revision 1.23  2014/09/09 05:42:38  ypwang
* minor update the data type for temporary variable numRawHits from UShort_t to Int_t
*
* Revision 1.22  2014/09/08 19:29:31  smirnovd
* StIstClusterMaker: Use Print() methods of respective collections for debugging
*
* Revision 1.21  2014/09/08 14:45:17  smirnovd
* StIstClusterMaker: No need to check for a null pointer before deleting the object
*
* Revision 1.20  2014/09/07 11:44:52  ypwang
* remove Init() function for clustering algorithms
*
* Revision 1.19  2014/09/07 11:34:42  ypwang
* update the setClusterAlgo() returning void instead of Int_t type
*
* Revision 1.18  2014/09/07 11:31:29  ypwang
* update the setClusterAlgo() returning void instead of Int_t type
*
* Revision 1.17  2014/09/07 08:15:18  ypwang
* destructor was added for the mIstCollectionPtr and mClusterAlgoPtr objects killing
*
* Revision 1.16  2014/08/22 15:55:15  smirnovd
* Fixed style with astyle -s3 -p -H -A3 -k3 -O -o -y -Y -f
*
* Revision 1.15  2014/08/21 17:51:08  smirnovd
* Moved CVS history to the end of file
*
* Revision 1.14  2014/08/12 23:04:53  ypwang
* remove the raw hit number cut per ladder before doing clustering, due to chip occupancy cut was added in raw hit maker which can do the bad column rejection; simplfy the code by removing the InitRun() function
*
* Revision 1.13  2014/08/06 18:56:52  ypwang
* minor update due to coding style update of the StIstDb method
*
* Revision 1.12  2014/07/29 20:13:31  ypwang
* update the IST DB obtain method
*
* Revision 1.11  2014/04/15 06:46:59  ypwang
* updates for collections clear due to Clear() function removed from StIstCollection
*
* Revision 1.10  2014/03/25 03:06:52  ypwang
* updates on Db table accessory method
*
* Revision 1.9  2014/03/24 15:55:07  ypwang
* minor updates due to returned const pointers in StIstDbMaker
*
* Revision 1.8  2014/03/17 21:51:56  ypwang
* minor update due to some IST constants moved to StEnumurations.h
*
* Revision 1.7  2014/02/16 21:42:54  ypwang
* getting number of time bins used in current event by StIstCollection::getNumTimeBins() function
*
* Revision 1.6  2014/02/15 20:02:37  ypwang
* Clear() member function added, and mIstCollectionPtr data member defined
*
* Revision 1.4  2014/02/08 03:34:16  ypwang
* updating scripts
*
*
****************************************************************************
* StIstClusterMaker.cxx,v 1.0
* Revision 1.0 2013/11/04 15:55:30 Yaping
* Initial version
****************************************************************************/
