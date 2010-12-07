/** 
 * @file  StvToolkit.h
 */
#ifndef StvToolkit_H
#define StvToolkit_H 1

/** 
 * @class StvToolkit
 * @brief Definition of toolkit
 */
class StvGeoLoader;
class StvHitLoader;
class StvSeedFinder;
class StvTrackFinder;
class StvEventFiller;
class StvHit;
class StvHitFactory;
class StvVertexFactory;
class StvNode;
class StvNodeFactory;
class StvTrack;
class StvTrackFactory;
class StvTracks;

class StvToolkit 
{
protected:
  StvToolkit(); 
public:
void Clear(const char* opt="");
void Reset();

void Init ();
void Finish();

StvGeoLoader 	*GeoLoader();
StvHitLoader 	*HitLoader()  const	{return mHitLoader ;}
StvSeedFinder   *SeedFinder() const	{return mSeedFinder;}
StvTrackFinder  *TrackFinder() const	{return mTrakFinder;}
StvEventFiller  *EventFiller() const	{return mEventFiller;}
double          GetHz(const double *x) const;
double          GetHz(const float  *x) const;

StvTracks      &GetTracks();
void            Show() const;


//		Factories for Stv objects
StvHit          *GetHit();       
StvHit          *GetVertex();       
void            FreeHit(StvHit*     &stiHit );       
StvNode         *GetNode();       
void            FreeNode(StvNode*   &stiNode);       
StvTrack       *GetTrack();       
void            FreeTrack(StvTrack* &stiTrak);       


void SetHitLoader  (StvHitLoader   *loadHits   ){ mHitLoader   = loadHits   ;}
void SetSeedFinder (StvSeedFinder  *seedFinder ){ mSeedFinder  = seedFinder ;}
void SetTrackFinder(StvTrackFinder *trackFinder){ mTrakFinder  = trackFinder;}
void SetEventFiller(StvEventFiller *eventFiller){ mEventFiller = eventFiller;}

public:
static StvToolkit* Inst(); 

protected:
char           	mBeg[1];
StvGeoLoader   	*mGeoLoader;
StvHitLoader   	*mHitLoader;
StvSeedFinder  	*mSeedFinder;
StvTrackFinder 	*mTrakFinder;
StvTracks 	*mTraks;
StvEventFiller  *mEventFiller;
StvHitFactory  	*mHitFactory;
StvNodeFactory  *mNodeFactory;
StvTrackFactory *mTrackFactory;
StvVertexFactory 	*mVertexFactory;
char            mEnd[1];

protected:
static StvToolkit* mgInstance;
};

#endif

