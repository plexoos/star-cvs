/***************************************************************************
 *
 * $Id: StMcPixelHit.hh,v 2.2 2004/09/14 05:00:30 calderon Exp $
 * $Log: StMcPixelHit.hh,v $
 * Revision 2.2  2004/09/14 05:00:30  calderon
 * Added support for Ist, Ssd and changes to Pixel, from "El Kai".
 *
 * Revision 2.1  2003/08/20 18:50:21  calderon
 * Addition of Tof classes and Pixel classes.  Modified track, event, and
 * container code to reflect this.
 * Fix bug in StMcVertex and in clearing of some hit collections.
 *
 *
 **************************************************************************/
#ifndef StMcPixelHit_hh
#define StMcPixelHit_hh

#include "StMcHit.hh"
#include "StMemoryPool.hh"

class StMcTrack;
class StThreeVectorF;
class g2t_pix_hit_st;

#if !defined(ST_NO_NAMESPACES)
#endif

class StMcPixelHit : public StMcHit {
public:
    StMcPixelHit();
  StMcPixelHit(const StThreeVectorF&,const StThreeVectorF&,
	       const float, const float, const long, const long, StMcTrack*);
    StMcPixelHit(g2t_pix_hit_st*);
    ~StMcPixelHit();

    void* operator new(size_t)     { return mPool.alloc(); }
    void  operator delete(void* p) { mPool.free(p); }

    unsigned long layer() const; // 1-2
    unsigned long ladder() const; // 1-6, 1-18
    
private:

    static StMemoryPool mPool; //!
};

ostream&  operator<<(ostream& os, const StMcPixelHit&);


#endif
