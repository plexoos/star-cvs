/***************************************************************************
 *
 * $Id: StMcEventTypes.hh,v 2.5 2003/02/19 03:16:05 calderon Exp $
 *
 * Author: Manuel Calderon de la Barca
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StMcEventTypes.hh,v $
 * Revision 2.5  2003/02/19 03:16:05  calderon
 * Introduction of Ctb Hit Class and Ctb Hit Collection class, modified
 * StMcTrack, and StMcEvent accordingly.  Clearing of hits in StMcSvtWaferHitCollection.
 *
 * Revision 2.4  2000/06/06 02:58:41  calderon
 * Introduction of Calorimeter classes.  Modified several classes
 * accordingly.
 *
 * Revision 2.3  2000/03/06 20:33:08  calderon
 * svt layer -> svt barrel
 *
 * Revision 2.2  2000/03/06 18:05:21  calderon
 * 1) Modified SVT Hits storage scheme from layer-ladder-wafer to
 * barrel-ladder-wafer.
 * 2) Added Rich Hit class and collection, and links to them in other
 * classes.
 *
 * Revision 2.1  1999/12/03 21:31:19  calderon
 * Initial Revision
 *
 *
 **************************************************************************/
#ifndef StMcEventTypes_hh
#define StMcEventTypes_hh

#include "StMcContainers.hh"
#include "StMcCalorimeterHit.hh"
#include "StMcCtbHit.hh"
#include "StMcCtbHitCollection.hh"
#include "StMcEmcModuleHitCollection.hh"
#include "StMcEmcHitCollection.hh"
#include "StMcEvent.hh"
#include "StMcFtpcHit.hh"
#include "StMcFtpcHitCollection.hh"
#include "StMcFtpcPlaneHitCollection.hh"
#include "StMcHit.hh"
#include "StMcHitComparisons.hh"
#include "StMcRichHit.hh"
#include "StMcRichHitCollection.hh"
#include "StMcSvtHit.hh"
#include "StMcSvtHitCollection.hh"
#include "StMcSvtBarrelHitCollection.hh"
#include "StMcSvtLadderHitCollection.hh"
#include "StMcSvtWaferHitCollection.hh"
#include "StMcTpcHit.hh"
#include "StMcTpcHitCollection.hh"
#include "StMcTpcPadrowHitCollection.hh"
#include "StMcTpcSectorHitCollection.hh"
#include "StMcTrack.hh"
#include "StMcVertex.hh"

#endif
