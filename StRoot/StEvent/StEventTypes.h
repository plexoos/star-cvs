/***************************************************************************
 *
 * $Id: StEventTypes.h,v 2.22 2002/04/18 23:38:21 jeromel Exp $
 *
 * Author: Thomas Ullrich, Sep 1999
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StEventTypes.h,v $
 * Revision 2.22  2002/04/18 23:38:21  jeromel
 * Implementation of the SVT 2 tables scheme ...
 *
 * Revision 2.21  2002/02/19 04:24:03  lasiuk
 * addition of StRichSpectra information for uDST purposes
 *
 * Revision 2.20  2002/01/03 20:59:33  ullrich
 * Added BBC and FPD.
 *
 * Revision 2.19  2001/12/01 15:42:26  ullrich
 * Added StDetectorState.h
 *
 * Revision 2.18  2001/11/10 23:53:23  ullrich
 * Added calibration vertices.
 *
 * Revision 2.17  2001/11/07 21:19:43  ullrich
 * Added L1 trigger.
 *
 * Revision 2.16  2001/10/01 19:40:31  ullrich
 * Added StTofData.
 *
 * Revision 2.15  2001/09/18 00:14:55  ullrich
 * Added StRunInfo.
 *
 * Revision 2.14  2001/09/13 03:04:21  ullrich
 * Removed StRun and StRunSummary.
 *
 * Revision 2.13  2001/08/20 21:22:46  ullrich
 * L3 classes added.
 *
 * Revision 2.12  2001/04/27 02:37:46  ullrich
 * Added ToF include files.
 *
 * Revision 2.11  2001/03/14 03:50:42  ullrich
 * Added StPsd.h
 *
 * Revision 2.10  2000/08/17 00:12:55  ullrich
 * Added StTptTrack.h
 *
 * Revision 2.9  2000/06/19 01:32:16  perev
 *  Thomas StEvent branches added
 *
 * Revision 2.8  2000/05/22 21:43:06  ullrich
 * Add RICH related containers.
 *
 * Revision 2.7  2000/04/26 20:29:50  ullrich
 * Removed obsolete StBrowsableEvent.
 *
 * Revision 2.6  2000/03/29 16:54:19  ullrich
 * Added L3 trigger.
 *
 * Revision 2.5  2000/02/23 17:36:05  ullrich
 * Changes due to the addition of the EMC to StEvent
 *
 * Revision 2.4  2000/02/17 18:13:09  ullrich
 * Changed the SVT hit storage model. Hits are now stored according
 * to barrel/ladder/wafer not by layer/ladder/wafer.
 *
 * Revision 2.3  2000/01/14 13:48:52  ullrich
 * Added RICH pixel collection.
 *
 * Revision 2.2  2000/01/05 16:02:30  ullrich
 * SSD hits added to StEvent.
 *
 * Revision 2.1  1999/10/13 19:43:04  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef StEventTypes_hh
#define StEventTypes_hh
 
#include "StBbcTriggerDetector.h"
#include "StCalibrationVertex.h"
#include "StCtbSoftwareMonitor.h"
#include "StCtbTriggerDetector.h"
#include "StDedxPidTraits.h"
#include "StDetectorState.h"
#include "StEmcCluster.h"
#include "StEmcClusterCollection.h"
#include "StEmcCollection.h"
#include "StEmcDetector.h"
#include "StEmcModule.h"
#include "StEmcPoint.h"
#include "StEmcRawHit.h"
#include "StEmcSoftwareMonitor.h"
#include "StEnumerations.h"
#include "StEvent.h"
#include "StEventClusteringHints.h"
#include "StEventInfo.h"
#include "StEventSummary.h"
#include "StEstGlobalTrack.h"
#include "StEstPrimaryTrack.h"
#include "StFpdCollection.h"
#include "StFtpcHit.h"
#include "StFtpcHitCollection.h"
#include "StFtpcPlaneHitCollection.h"
#include "StFtpcSectorHitCollection.h"
#include "StFtpcSoftwareMonitor.h"
#include "StFunctional.h"
#include "StGlobalSoftwareMonitor.h"
#include "StGlobalTrack.h"
#include "StHelixModel.h"
#include "StHit.h"
#include "StKinkVertex.h"
#include "StL0Trigger.h"
#include "StL1Trigger.h"
#include "StL3Trigger.h"
#include "StL3EventSummary.h"
#include "StL3AlgorithmInfo.h"
#include "StL3SoftwareMonitor.h"
#include "StMeasuredPoint.h"
#include "StMwcTriggerDetector.h"
#include "StPrimaryTrack.h"
#include "StPrimaryVertex.h"
#include "StPsd.h"
#include "StRichCluster.h"
#include "StRichCollection.h"
#include "StRichHit.h"
#include "StRichMCHit.h"
#include "StRichMCInfo.h"
#include "StRichMCPixel.h"
#include "StRichPhotonInfo.h"
#include "StRichPid.h"
#include "StRichPidTraits.h"
#include "StRichSpectra.h"
#include "StRichSoftwareMonitor.h"
#include "StRichPixel.h"
#include "StRunInfo.h"
#include "StSoftwareMonitor.h"
#include "StSsdHit.h"
#include "StSsdHitCollection.h"
#include "StSsdLadderHitCollection.h"
#include "StSsdWaferHitCollection.h"
#include "StSvtHit.h"
#include "StSvtHitCollection.h"
#include "StSvtLadderHitCollection.h"
#include "StSvtBarrelHitCollection.h"
#include "StSvtSoftwareMonitor.h"
#include "StSvtWaferHitCollection.h"
#include "StTofCollection.h"
#include "StTofHit.h"
#include "StTofMCHit.h"
#include "StTofMCSlat.h"
#include "StTofPidTraits.h"
#include "StTofSlat.h"
#include "StTofData.h"
#include "StTofSoftwareMonitor.h"
#include "StTpcHit.h"
#include "StTpcHitCollection.h"
#include "StTpcPadrowHitCollection.h"
#include "StTpcPixel.h"
#include "StTpcSectorHitCollection.h"
#include "StTpcSoftwareMonitor.h"
#include "StTptTrack.h"
#include "StTrack.h"
#include "StTrackDetectorInfo.h"
#include "StTrackFitTraits.h"
#include "StTrackGeometry.h"
#include "StTrackNode.h"
#include "StTrackPidTraits.h"
#include "StTrackTopologyMap.h"
#include "StTrigger.h"
#include "StTriggerDetectorCollection.h"
#include "StV0Vertex.h"
#include "StVertex.h"
#include "StVpdTriggerDetector.h"
#include "StXiVertex.h"
#include "StZdcTriggerDetector.h"
 
#endif
