/***************************************************************************
 *
 * $Id: StMcTofHit.hh,v 2.2 2005/01/27 23:40:48 calderon Exp $
 * $Log: StMcTofHit.hh,v $
 * Revision 2.2  2005/01/27 23:40:48  calderon
 * Adding persistency to StMcEvent as a step for Virtual MonteCarlo.
 *
 * Revision 2.1  2003/08/20 18:50:21  calderon
 * Addition of Tof classes and Pixel classes.  Modified track, event, and
 * container code to reflect this.
 * Fix bug in StMcVertex and in clearing of some hit collections.
 *
 */
#ifndef StMcTofHit_hh
#define StMcTofHit_hh

#include "StMcHit.hh"
#include "StMemoryPool.hh"

class StMcTrack;
class StThreeVectorF;
class g2t_ctf_hit_st;

class StMcTofHit : public StMcHit {
public:
  StMcTofHit();
  StMcTofHit(const StThreeVectorF&,const StThreeVectorF&,
	     const float, const float,  const long, const long, StMcTrack*);
  StMcTofHit(g2t_ctf_hit_st*);
  virtual ~StMcTofHit();
  int operator==(const StMcTofHit&) const;
  int operator!=(const StMcTofHit&) const;
  float tof() const;
  float sTrack() const;
#ifdef POOL
  void* operator new(size_t)     { return mPool.alloc(); }
  void  operator delete(void* p) { mPool.free(p); }
#endif
private:
#ifdef POOL
  static StMemoryPool mPool; //!
#endif
  float               mTof; //!
  float               mStrack; //!
  ClassDef(StMcTofHit,1)
};

ostream&  operator<<(ostream& os, const StMcTofHit&);

inline float StMcTofHit::tof()  const {return mTof;}
inline float StMcTofHit::sTrack() const {return mStrack;}

#endif
