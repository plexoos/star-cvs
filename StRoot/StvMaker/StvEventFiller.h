//StvEventFiller.h
/***************************************************************************
 *
 * $Id: StvEventFiller.h,v 1.1 2010/07/06 20:27:53 perev Exp $
 * It is base class for Transfer internal Stv structures into 
 * current experiment ones
 * Author: Victor Perev, Jun 2010
 *
 ***************************************************************************
 */
#ifndef StvEventFiller_HH
#define StvEventFiller_HH

class StEvent;
class StvEventFiller
{
public:
    StvEventFiller(){mEvent=0;mTracks=0;mPullEvent=0;}
    virtual ~StvEventFiller(){;}
        void Set(StEvent* e,StvTracks* tk) 	{ mEvent=e; mTracks = tk;}
        void Set(StvPullEvent* pull)		{ mPullEvent=pull;}
	     virtual void fillEvent()=0;
virtual void fillEventPrimaries()=0;
protected:
    StEvent* 		mEvent;
    StvTracks* 		mTracks;
    StvPullEvent* 	mPullEvent;

};

#endif
