//StvStEventFiller.h
/***************************************************************************
 *
 * $Id: StvStEventFiller.h,v 1.1 2010/07/06 20:27:53 perev Exp $
 *
 * Author: Manuel Calderon de la Barca Sanchez, Mar 2002
 * Author: Victor Perev, Jun 2010
 ***************************************************************************
 *
 * $Log: StvStEventFiller.h,v $
 * Revision 1.1  2010/07/06 20:27:53  perev
 * Alpha version of Stv (Star Tracker Virtual)
 *
 * Revision 1.2  2010/07/03 16:27:15  perev
 * Last time name Stv
 *
 * Revision 1.1  2010/06/22 19:34:28  perev
 * EventFiller added
 *
 * Revision 2.24  2006/12/18 01:30:52  perev
 * fillPulls reorganized
 *
 *
 **************************************************************************/
#ifndef StvStEventFiller_HH
#define StvStEventFiller_HH
#include "StDetectorId.h"
class StEvent;
class StTrackNode;
class StTrackDetectorInfo;
class StvNode;
class StvHit;
class StTrack;
class StvPullEvent;
#include "StvEventFiller.h"
#include "StThreeVectorD.hh"
//class StHelix;
//class StHelixModel;
#include "StPhysicalHelixD.hh"

/*! \class StvStEventFiller
    StvStEventFiller is a utilitity class meant to properly convert StvTrack
    objects into StTrack (Global/Primary) objects and hang these on the StEvent
    Track-node.

    \author Manuel Calderon de la Barca Sanchez (Yale Software)
    Rewritten for StvVmc 
    \author Victor Perev (BNL)

 */
class StvStEventFiller : public StvEventFiller
{
public:
    StvStEventFiller();
    void setUseAux(int aux=1)		{mUseAux=aux;}
    virtual ~StvStEventFiller();
    void fillEvent();
    void fillEventPrimaries();
    void fillDetectorInfo(StTrackDetectorInfo* detInfo, const StvTrack* kTrack,bool refCountIncr);
    void fillGeometry(StTrack* track, const StvTrack* kTrack, bool outer);
    //void fillTopologyMap(StTrack* track, const StvTrack* kTrack);
    void fillFitTraits(StTrack* track, const StvTrack* kTrack);
    void fillTrack(StTrack* track, const StvTrack* kTrack,StTrackDetectorInfo* detInfo );
    void fillDca(StTrack* track, const StvTrack* kTrack);
    void fillFlags(StTrack* track);
    double impactParameter(StTrack* strack , StThreeVectorD &vertexPosition);
    void setPullEvent(StvPullEvent *pe) 		{mPullEvent=pe;}
    void getAllPointCount(const StvTrack *track,int count[1][3]);
private:
 void fillResHack(StHit *hh,const StvHit *stiHit, const StvNode *node);
 void fillPulls  (StHit *hh,const StvHit *stiHit
                 ,const StvNode  *node
		 ,const StvTrack *track
		 ,int dets[1][3],int gloPri);
 void fillPulls  (const StvTrack *ktrack,int gloPri);
 bool accept(const StvTrack* kTrack);
 void FillStHitErr(StHit *hh,const StvNode *node);

private:
    int mUseAux;
    int mGloPri;		//0=filing global,1=filing primary
    int mTrackNumber;

    unsigned short mStvEncoded;


};

#endif
