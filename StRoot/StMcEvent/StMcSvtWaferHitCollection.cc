/***************************************************************************
 *
 * $Id: StMcSvtWaferHitCollection.cc,v 2.4 2005/01/27 23:40:48 calderon Exp $
 *
 * Author: Manuel Calderon de la Barca Sanchez, Oct 1999
 ***************************************************************************
 *
 * Description: Monte Carlo Svt Wafer Hit Collection class
 *
 ***************************************************************************
 *
 * $Log: StMcSvtWaferHitCollection.cc,v $
 * Revision 2.4  2005/01/27 23:40:48  calderon
 * Adding persistency to StMcEvent as a step for Virtual MonteCarlo.
 *
 * Revision 2.3  2003/02/19 03:16:05  calderon
 * Introduction of Ctb Hit Class and Ctb Hit Collection class, modified
 * StMcTrack, and StMcEvent accordingly.  Clearing of hits in StMcSvtWaferHitCollection.
 *
 * Revision 2.2  2000/03/06 18:05:23  calderon
 * 1) Modified SVT Hits storage scheme from layer-ladder-wafer to
 * barrel-ladder-wafer.
 * 2) Added Rich Hit class and collection, and links to them in other
 * classes.
 *
 * Revision 2.1  1999/11/19 19:06:33  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:01:00  calderon
 * Completely revised for new StEvent
 *
 *
 **************************************************************************/
#include "StMcSvtWaferHitCollection.hh"
#include "StMcSvtHit.hh"

static const char rcsid[] = "$Id: StMcSvtWaferHitCollection.cc,v 2.4 2005/01/27 23:40:48 calderon Exp $";
ClassImp(StMcSvtWaferHitCollection);
StMcSvtWaferHitCollection::StMcSvtWaferHitCollection() { /* noop */ }

StMcSvtWaferHitCollection::~StMcSvtWaferHitCollection()
{
  // StMcSvtHit provides its own new/delete operator, and
  // mHits is a polymorphic container, so we need to do this.
  for (unsigned int i=0; i<mHits.size(); i++) {
    delete mHits[i];
    mHits[i] = 0;
  }
  mHits.clear();
}

const StSPtrVecMcSvtHit&
StMcSvtWaferHitCollection::hits() const { return mHits; }

StSPtrVecMcSvtHit&
StMcSvtWaferHitCollection::hits() { return mHits; }
