/***************************************************************************
 *
 * $Id: StMcTofHit.cc,v 2.3 2005/01/27 23:40:48 calderon Exp $
 * $Log: StMcTofHit.cc,v $
 * Revision 2.3  2005/01/27 23:40:48  calderon
 * Adding persistency to StMcEvent as a step for Virtual MonteCarlo.
 *
 * Revision 2.2  2003/12/02 21:22:03  calderon
 * remove unnecessary #include "StMcTrack.hh"
 *
 * Revision 2.1  2003/08/20 18:50:21  calderon
 * Addition of Tof classes and Pixel classes.  Modified track, event, and
 * container code to reflect this.
 * Fix bug in StMcVertex and in clearing of some hit collections.
 *
 ***************************************************************************/
#include "StMcTofHit.hh"
#include "tables/St_g2t_ctf_hit_Table.h"
static const char rcsid[] = "$Id: StMcTofHit.cc,v 2.3 2005/01/27 23:40:48 calderon Exp $";
#ifdef POOL
StMemoryPool StMcTofHit::mPool(sizeof(StMcTofHit));
#endif
ClassImp(StMcTofHit);

StMcTofHit::StMcTofHit() { /* noop */ };

StMcTofHit::StMcTofHit(const StThreeVectorF& x,const StThreeVectorF& p,
 		       const float de, const float ds, const long key,
		       const long id,
		       StMcTrack* parent)  : StMcHit(x, p, de, ds, key, id, parent)
{ /* noop */ }


StMcTofHit::StMcTofHit(g2t_ctf_hit_st* pt)
    : StMcHit(StThreeVectorF(pt->x[0], pt->x[1], pt->x[2]),
	      StThreeVectorF(pt->p[0], pt->p[1], pt->p[2]),
	      pt->de,
	      pt->ds,
	      pt->id,
	      pt->volume_id,0),
      mTof(pt->tof),
      mStrack(pt->s_track)
{/* noop */ }

StMcTofHit::~StMcTofHit() {/* noop */}

ostream&  operator<<(ostream& os, const StMcTofHit& h) {
    os << "Position       : " << h.position() << endl; 
    os << "Local Momentum : " << h.localMomentum()    << endl;
    os << "volume-id      : " << h.volumeId() << endl;
    os << "T. of Flight   : " << h.tof()   << endl;
    os << "path length    : " << h.sTrack()   << endl;
    return os;
}
