/***************************************************************************
 *
 * $Id: StMcSvtHit.hh,v 2.11 2005/09/28 21:30:15 fisyak Exp $
 * $Log: StMcSvtHit.hh,v $
 * Revision 2.11  2005/09/28 21:30:15  fisyak
 * Persistent StMcEvent
 *
 * Revision 2.10  2005/01/27 23:40:48  calderon
 * Adding persistency to StMcEvent as a step for Virtual MonteCarlo.
 *
 * Revision 2.9  2000/06/06 02:58:41  calderon
 * Introduction of Calorimeter classes.  Modified several classes
 * accordingly.
 *
 * Revision 2.8  2000/05/05 15:25:44  calderon
 * Reduced dependencies and made constructors more efficient
 *
 * Revision 2.7  2000/04/19 14:34:48  calderon
 * More corrections for the SSD, thanks Helen
 *
 * Revision 2.6  2000/04/18 23:46:12  calderon
 * Fix bug in reurning barrel number
 * Enumerations for the Max barrels, ladders & wafers modified for
 * SSD inclusion in current scheme.
 *
 * Revision 2.5  2000/04/18 22:55:28  calderon
 * Functions to access the volume Id
 * Added volume Id to output of operator<<
 *
 * Revision 2.4  2000/01/18 20:52:31  calderon
 * Works with CC5
 *
 * Revision 2.3  1999/12/14 07:04:49  calderon
 * Numbering scheme as per SVT request.
 *
 * Revision 2.2  1999/12/03 00:51:52  calderon
 * Tested with new StMcEventMaker.  Added messages for
 * diagnostics.
 *
 * Revision 2.1  1999/11/19 19:06:33  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:12:16  calderon
 * Completely revised for new StEvent
 *
 * Revision 1.4  1999/09/24 01:23:16  fisyak
 * Reduced Include Path
 *
 * Revision 1.3  1999/09/23 21:25:52  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 *
 **************************************************************************/
#ifndef StMcSvtHit_hh
#define StMcSvtHit_hh

#include "StMcHit.hh"
#include "StMemoryPool.hh"

class StMcTrack;
class g2t_svt_hit_st;

#if !defined(ST_NO_NAMESPACES)
#endif

class StMcSvtHit : public StMcHit {
public:
    StMcSvtHit();
    StMcSvtHit(const StThreeVectorF&,const StThreeVectorF&,
	     const float, const float, const long, const long, 
	     StMcTrack*);
    StMcSvtHit(g2t_svt_hit_st*);
    ~StMcSvtHit();
#ifdef POOL
    void* operator new(size_t)     { return mPool.alloc(); }
    void  operator delete(void* p) { mPool.free(p); }
#endif
    unsigned long layer() const;      // layer=[1,6] with SSD [1-8]
    unsigned long ladder() const;     // ladder=[1-8] with SSD [1-20]
    unsigned long wafer() const;      // wafer=[1-7] with SSD [1-16]
    unsigned long barrel() const;     // barrel=[1-3] with SSD [1-4]
    unsigned long hybrid() const;

protected:
#ifdef POOL
    static StMemoryPool mPool;  
#endif
    ClassDef(StMcSvtHit,1)
};

ostream&  operator<<(ostream& os, const StMcSvtHit&);

inline unsigned long
StMcSvtHit::layer() const
{
    // Volume Id: 1000*layer + 100*wafer + ladder (Helen, Nov 99)
    return (mVolumeId)/1000;
}

inline unsigned long
StMcSvtHit::ladder() const
{
    // Volume Id: 1000*layer + 100*wafer + ladder (Helen, Nov 99)
    return (mVolumeId)%100 ;
}

inline unsigned long
StMcSvtHit::wafer() const
{
    // Volume Id: 1000*layer + 100*wafer + ladder (Helen, Nov 99)

    if (mVolumeId < 7101) // SVT
	return ((mVolumeId)%1000)/100;
    else // SSD
	return ((mVolumeId/100)-70);
}

inline unsigned long
StMcSvtHit::barrel() const { return (layer()+1)/2; }

inline unsigned long
StMcSvtHit::hybrid() const { return 0; } // to be implemented

#endif
