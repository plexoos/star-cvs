/***************************************************************************
 *
 * $Id: StEmcCluster.cxx,v 2.5 2001/04/05 04:00:48 ullrich Exp $
 *
 * Author: Akio Ogawa, Jan 2000
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StEmcCluster.cxx,v $
 * Revision 2.5  2001/04/05 04:00:48  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.4  2001/03/24 03:34:44  perev
 * clone() -> clone() const
 *
 * Revision 2.3  2000/07/31 22:12:22  akio
 * eliminate print() for L3(?)
 *
 * Revision 2.2  2000/03/23 22:24:06  akio
 * Initial version of Emc Point, and Inclusion of track pointers
 *
 * Revision 2.1  2000/02/23 17:34:01  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "StEmcCluster.h"

ClassImp(StEmcCluster)
    
static const char rcsid[] = "$Id: StEmcCluster.cxx,v 2.5 2001/04/05 04:00:48 ullrich Exp $";

StEmcCluster::StEmcCluster() {/* noop*/};

StEmcCluster::~StEmcCluster() {/* noop */};

float
StEmcCluster::eta() const {return mEta;}

float
StEmcCluster::phi() const {return mPhi;}

float
StEmcCluster::sigmaEta() const {return mSigmaEta;}

float
StEmcCluster::sigmaPhi() const {return mSigmaPhi;}

float
StEmcCluster::energy() const {return mEnergy;}

int
StEmcCluster::nHits() const {return mHits.size();}

int
StEmcCluster::nNeighbors() const {return mNeighbors.size();}

int
StEmcCluster::nTracks() const {return mTracks.size();}

StPtrVecEmcRawHit&
StEmcCluster::hit() {return mHits;}

const StPtrVecEmcRawHit&
StEmcCluster::hit() const {return mHits;}

StPtrVecEmcCluster&
StEmcCluster::neighbor() {return mNeighbors;}

const StPtrVecEmcCluster&
StEmcCluster::neighbor() const {return mNeighbors;}

StPtrVecTrack&
StEmcCluster::track() {return mTracks;}

const StPtrVecTrack&
StEmcCluster::track() const {return mTracks;}

void
StEmcCluster::setEta(float ver)      {mEta=ver;}

void
StEmcCluster::setPhi(float ver)      {mPhi=ver;}

void
StEmcCluster::setSigmaEta(float ver) {mSigmaEta=ver;}

void
StEmcCluster::setSigmaPhi(float ver) {mSigmaPhi=ver;}

void
StEmcCluster::setEnergy(float ver)   {mEnergy=ver;}

void
StEmcCluster::addHit(StEmcRawHit* hit) {mHits.push_back(hit);}

void
StEmcCluster::addNeighbor(StEmcCluster* cluster) {mNeighbors.push_back(cluster);}

void
StEmcCluster::addTrack(StTrack* track) {mTracks.push_back(track);}

StObject*
StEmcCluster::clone() const {return new StEmcCluster(*this);}

ostream&
operator<<(ostream &os, const StEmcCluster& cl)
{
  return (os << " Energy " << cl.energy() << endl
          << " Eta "    << cl.eta() << "+/-" << cl.sigmaEta() << endl
          << " Phi "    << cl.phi() << "+/-" << cl.sigmaPhi() << endl
          << " # of hits " << cl.nHits() << ",# of neighbor " << cl.nNeighbors() << endl);
}

