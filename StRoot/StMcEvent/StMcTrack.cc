/***************************************************************************
 *
 * $Id: StMcTrack.cc,v 2.14 2003/02/19 03:16:05 calderon Exp $
 * $Log: StMcTrack.cc,v $
 * Revision 2.14  2003/02/19 03:16:05  calderon
 * Introduction of Ctb Hit Class and Ctb Hit Collection class, modified
 * StMcTrack, and StMcEvent accordingly.  Clearing of hits in StMcSvtWaferHitCollection.
 *
 * Revision 2.2  2003/02/18 00:00:00  gans
 * Introduction of Ctb classes.  Modified several classes
 * accordingly.

 * $Id: StMcTrack.cc,v 2.14 2003/02/19 03:16:05 calderon Exp $
 * $Log: StMcTrack.cc,v $
 * Revision 2.14  2003/02/19 03:16:05  calderon
 * Introduction of Ctb Hit Class and Ctb Hit Collection class, modified
 * StMcTrack, and StMcEvent accordingly.  Clearing of hits in StMcSvtWaferHitCollection.
 *
 * Revision 2.13  2000/06/22 23:52:27  calderon
 * Alexei fixed typo in ostream<< for Bsdme
 *
 * Revision 2.12  2000/06/06 02:58:42  calderon
 * Introduction of Calorimeter classes.  Modified several classes
 * accordingly.
 *
 * Revision 2.11  2000/05/05 15:25:44  calderon
 * Reduced dependencies and made constructors more efficient
 *
 * Revision 2.10  2000/05/04 22:46:01  calderon
 * read the pdg Id ALSO from the g2t_track table.  This caused problems
 * for embedded tracks.
 *
 * Revision 2.9  2000/04/18 00:55:14  calderon
 * added printout of local momentum to operator<<
 *
 * Revision 2.8  2000/04/12 21:32:05  calderon
 * Chech particle definition pointer before writing name
 *
 * Revision 2.7  2000/04/06 23:29:10  calderon
 * Store the parent track for all tracks.
 *
 * Revision 2.6  2000/04/06 08:34:56  calderon
 * Version using the particle table:
 * 1) Constructor for particle_st*
 * 2) Pointer to parent track from particle table
 * 3) PDG encoding when track is from particle table
 * 4) Generator label, used to index entries in the table for debugging
 *
 * Revision 2.5  2000/03/06 18:05:23  calderon
 * 1) Modified SVT Hits storage scheme from layer-ladder-wafer to
 * barrel-ladder-wafer.
 * 2) Added Rich Hit class and collection, and links to them in other
 * classes.
 *
 * Revision 2.4  2000/01/18 20:52:31  calderon
 * Works with CC5
 *
 * Revision 2.3  1999/12/14 07:04:50  calderon
 * Numbering scheme as per SVT request.
 *
 * Revision 2.2  1999/12/03 00:51:53  calderon
 * Tested with new StMcEventMaker.  Added messages for
 * diagnostics.
 *
 * Revision 2.1  1999/11/19 19:06:34  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:12:16  calderon
 * Completely revised for new StEvent
 *
 * Revision 1.3  1999/09/23 21:25:54  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 *
 **************************************************************************/
#include <algorithm>
#ifndef ST_NO_NAMESPACES
using std::find;
#endif

#include "StMcTrack.hh"

#include "StMcContainers.hh"
#include "StParticleTable.hh"
#include "StParticleDefinition.hh"

#include "tables/St_g2t_track_Table.h"
#include "tables/St_particle_Table.h"

static const char rcsid[] = "$Id: StMcTrack.cc,v 2.14 2003/02/19 03:16:05 calderon Exp $";

StMcTrack::StMcTrack() 
{
    initToZero();
}



StMcTrack::StMcTrack(g2t_track_st* trk) {
    initToZero();
    mFourMomentum.setPx(trk->p[0]);
    mFourMomentum.setPy(trk->p[1]);
    mFourMomentum.setPz(trk->p[2]);
    mFourMomentum.setE(trk->e);
    mIsShower = trk->is_shower;
    mGeantId = trk->ge_pid;
    mPdgId   = trk->eg_pid;
    mKey     = trk->id;
    mParticleDefinition = StParticleTable::instance()->findParticleByGeantId(trk->ge_pid);
    mEventGenLabel = trk->eg_label;
    
    // The information to fill the collections 
    // is not available directly from the tables.  
    // We need to decode from trk->hit_tpc_p, 
    // trk->hit_svt_p, trk->hit_ftp_p, and trk-next_vtx_trk_p 
    // the actual Collection objects, not just the id's stored in the table.
}

StMcTrack::StMcTrack(particle_st* trk) {
    initToZero();
    mFourMomentum.setPx(trk->phep[0]);
    mFourMomentum.setPy(trk->phep[1]);
    mFourMomentum.setPz(trk->phep[2]);
    mFourMomentum.setE(trk->phep[3]);
    mParticleDefinition = StParticleTable::instance()->findParticle(trk->idhep);
    mPdgId = trk->idhep;
    // This constructor is used for particles coming from the
    // particle table.
}



StMcTrack::~StMcTrack() {
    // Not owner, so we don't have to delete.
    mIntermediateVertices.clear();
    mTpcHits.clear();
    mSvtHits.clear();
    mFtpcHits.clear();
}

void StMcTrack::initToZero()
{
    
    mStartVertex  = 0;
    mStopVertex   = 0;
    mParticleDefinition = 0;
    mParent = 0;
    mIsShower        = 0;
    mGeantId         = 0;
    mPdgId           = 0;
    mKey             = 0;
    mEventGenLabel   = 0;

}

int StMcTrack::operator==(const StMcTrack& t) const
{
    return (t.mFourMomentum == mFourMomentum &&
	    t.mStartVertex  == mStartVertex   &&
	    t.mStopVertex   == mStopVertex );
}

int StMcTrack::operator!=(const StMcTrack& t) const
{
    return !(t == *this);
}

ostream&  operator<<(ostream& os, const StMcTrack& t)
{
    if (t.particleDefinition())
	os << "Particle      : " << t.particleDefinition()->name().c_str() << endl;
    os << "Four Momentum : " << t.fourMomentum() << endl; 
    os << "Pt            : " << t.pt() << endl;
    os << "Rapidity      : " << t.rapidity() << endl;
    os << "PseudoRapidity: " << t.pseudoRapidity()   << endl;
    os << "No. Tpc   Hits: " << t.tpcHits().size()   << endl;
    os << "No. Svt   Hits: " << t.svtHits().size()   << endl;
    os << "No. Ftpc  Hits: " << t.ftpcHits().size()  << endl;
    os << "No. Rich  Hits: " << t.richHits().size()  << endl;
    os << "No. Ctb   Hits: " << t.ctbHits().size()  << endl;
    os << "No. Bemc  Hits: " << t.bemcHits().size()  << endl;
    os << "No. Bprs  Hits: " << t.bprsHits().size()  << endl;
    os << "No. Bsmde Hits: " << t.bsmdeHits().size() << endl;
    os << "No. Bsmdp Hits: " << t.bsmdpHits().size() << endl;
    os << "Is Shower     : " << t.isShower() << endl;
    os << "Geant Id      : " << t.geantId()  << endl;
    os << "Pdg Code      : " << t.pdgId()  << endl;
    os << "Event Gen. Lab: " << t.eventGenLabel()  << endl;
    os << "Key from g2t  : " << t.key()  << endl;
    
    return os;
}

void StMcTrack::setFourMomentum(const StLorentzVectorF& val) { mFourMomentum = val; }

void StMcTrack::setStartVertex(StMcVertex* val) { mStartVertex = val; }

void StMcTrack::setStopVertex(StMcVertex* val) { mStopVertex = val; }

void StMcTrack::setIntermediateVertices(StPtrVecMcVertex& val) { mIntermediateVertices = val; }

void StMcTrack::setTpcHits(StPtrVecMcTpcHit& val) { mTpcHits = val; }

void StMcTrack::setSvtHits(StPtrVecMcSvtHit& val) { mSvtHits = val; }

void StMcTrack::setFtpcHits(StPtrVecMcFtpcHit& val) { mFtpcHits = val; }

void StMcTrack::setRichHits(StPtrVecMcRichHit& val) { mRichHits = val; }

void StMcTrack::setBemcHits(StPtrVecMcCalorimeterHit& val) { mBemcHits = val; }

void StMcTrack::setBprsHits(StPtrVecMcCalorimeterHit& val) { mBprsHits = val; }

void StMcTrack::setBsmdeHits(StPtrVecMcCalorimeterHit& val) { mBsmdeHits = val; }

void StMcTrack::setBsmdpHits(StPtrVecMcCalorimeterHit& val) { mBsmdpHits = val; }

void StMcTrack::setShower(char val) { mIsShower = val; }

void StMcTrack::setGeantId(long val) { mGeantId = val; }

void StMcTrack::setPdgId(long val) { mPdgId = val; }

void StMcTrack::setKey(long val) { mKey = val; }

void StMcTrack::setEventGenLabel(long val) { mEventGenLabel = val; }

void StMcTrack::setParent(StMcTrack* val) { mParent = val; }

void StMcTrack::addTpcHit(StMcTpcHit* hit)
{
  mTpcHits.push_back(hit);
}

void StMcTrack::addFtpcHit(StMcFtpcHit* hit)
{
  mFtpcHits.push_back(hit);
}

void StMcTrack::addSvtHit(StMcSvtHit* hit)
{
  mSvtHits.push_back(hit);
}

void StMcTrack::addRichHit(StMcRichHit* hit)
{
  mRichHits.push_back(hit);
}

void StMcTrack::addCtbHit(StMcCtbHit* hit)
{
  mCtbHits.push_back(hit);
}

void StMcTrack::addBemcHit(StMcCalorimeterHit* hit)
{
  mBemcHits.push_back(hit);
}

void StMcTrack::addBprsHit(StMcCalorimeterHit* hit)
{
  mBprsHits.push_back(hit);
}

void StMcTrack::addBsmdeHit(StMcCalorimeterHit* hit)
{
  mBsmdeHits.push_back(hit);
}

void StMcTrack::addBsmdpHit(StMcCalorimeterHit* hit)
{
  mBsmdpHits.push_back(hit);
}
// Not very elegant.  Maybe should have kept all collections as
// vector<StMcHit*> so that then we could have used the same
// routine for all of them... 
void StMcTrack::removeTpcHit(StMcTpcHit* hit)
{
    StMcTpcHitIterator iter = find (mTpcHits.begin(), mTpcHits.end(), hit);
    if (iter != mTpcHits.end()) {
	mTpcHits.erase(iter);
    }
}

void StMcTrack::removeFtpcHit(StMcFtpcHit* hit)
{
    StMcFtpcHitIterator iter = find (mFtpcHits.begin(), mFtpcHits.end(),hit);
    if (iter != mFtpcHits.end()) {
	mFtpcHits.erase(iter);
    }
}

void StMcTrack::removeSvtHit(StMcSvtHit* hit)
{
    StMcSvtHitIterator iter = find(mSvtHits.begin(), mSvtHits.end(), hit);
    if (iter != mSvtHits.end()) {
	mSvtHits.erase(iter);
    }
}

void StMcTrack::removeRichHit(StMcRichHit* hit)
{
    StMcRichHitIterator iter = find(mRichHits.begin(), mRichHits.end(), hit);
    if (iter != mRichHits.end()){
	mRichHits.erase(iter);
    }
}

void StMcTrack::removeCtbHit(StMcCtbHit* hit)
{
    StMcCtbHitIterator iter = find(mCtbHits.begin(), mCtbHits.end(), hit);
    if (iter != mCtbHits.end()){
	mCtbHits.erase(iter);
    }
}

void StMcTrack::removeCalorimeterHit(StPtrVecMcCalorimeterHit& vch, StMcCalorimeterHit* hit)
{
    StMcCalorimeterHitIterator iter = find(vch.begin(), vch.end(), hit);
    if (iter != vch.end()) {
	vch.erase(iter);
    }
    
}
void StMcTrack::removeBemcHit(StMcCalorimeterHit* hit)
{
    removeCalorimeterHit(mBemcHits, hit);
}
void StMcTrack::removeBprsHit(StMcCalorimeterHit* hit)
{
    removeCalorimeterHit(mBprsHits, hit);
}
void StMcTrack::removeBsmdeHit(StMcCalorimeterHit* hit)
{
    removeCalorimeterHit(mBsmdeHits, hit);
}
void StMcTrack::removeBsmdpHit(StMcCalorimeterHit* hit)
{
    removeCalorimeterHit(mBsmdpHits, hit);
}
//void StMcTrack::setTopologyMap(StTrackTopologyMap& val) { mTopologyMap = val; }
