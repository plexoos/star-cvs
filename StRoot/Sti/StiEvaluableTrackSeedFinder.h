//StiEvaluableTrackSeedFinder.h
//M.L. Miller (Yale Software)
//04/01

/*! \class StiEvaluableTrackSeedFinder.
  StiEvaluableTrackSeedFinder is used to provide candidate seeds for the track finder
  when running on simulated data.  These seeds are of type StiEvaluableTrack, and
  encapsulate information regarding the monte-carlo track and the StGlobalTrack found by
  existing STAR tracking.  The association is performed via StAssociationMaker.  The
  best match is chosen from all possible matches, and this match must pass a cut on the
  number of matched points.  This cut is explained in makeTrack().
  <p>
  StiEvaluableTrackSeedFinder is a StiSeedFinder, so it supports the enforced user interface,
  namely hasMore() and next().  Additionaly, one can control the nature of the generated
  seeds.  StiEvaluableTrackSeedFinder is, like all StiSeedFinder objects, dynamically buildable.
  Currently this is done by parsing a text file.  However, it is understood that the
  information necessary for a dynamic build must eventually come from a data base.  For more
  information, see the documentation for setBuildPath() and build() methods.
  <p>
  StiEvaluableTrackSeedFinder requires a valid pointer to an instance of type
  StAssociationMaker in the constructor call.  Contsturctors of other types are
  explicitly prohibited.

  \author M.L. Miller (Yale Software)
*/


#ifndef StiEvaluableTrackSeedFinder_HH
#define StiEvaluableTrackSeedFinder_HH

#include <vector>
using std::vector;
#include <map>
using std::map;
#include <string>
using std::string;

#include "StiSeedFinder.h"
#include "StiObjectFactoryInterface.h"

class StiKalmanTrack;
class StMcEvent;
class StMcTrack;
class StAssociationMaker;
class StTrackPairInfo;
class StTpcHitFilter;

//Stl utility functor
class BestCommonHits
{
public:
    typedef pair<StMcTrack*, StTrackPairInfo*> McToStPair_t;

    ///Default Constructor
    BestCommonHits();
    
    ///Reset all internal members to either zero or 'null'.
    void reset() {
	mMostCommon=0;
	mPair=0;
    }

    ///Set required lower bound for the number of common hits required.
    void setLowerBound(unsigned int val ) {mMostCommon=val;}

    void operator()(const McToStPair_t& rhs);
    
    StTrackPairInfo* pair() const {return mPair;}
    
private:
    unsigned int mMostCommon;
    StTrackPairInfo* mPair;
};
    
class StiEvaluableTrackSeedFinder : public StiSeedFinder
{
public:
    
    ///This is the only constructor available.
    StiEvaluableTrackSeedFinder(StAssociationMaker*);

    ///Default destructor.
    virtual ~StiEvaluableTrackSeedFinder();
    
    //Sets

    void setEvent(StMcEvent* mcevt=0);
    virtual bool hasMore();
    virtual StiKalmanTrack* next();
    virtual void build();
    virtual void reset();

protected:
    ///Construct an evaluable track from a m.c. track
    StiEvaluableTrack* makeTrack(StMcTrack*);
    
private:
    //Not implemented, gotta have association maker
    StiEvaluableTrackSeedFinder(); 
    
    StAssociationMaker* mAssociationMaker;
    StMcEvent* mMcEvent;
    //deep memeber, requires non-defualt assignment and copy
    StTpcHitFilter* mTpcHitFilter;

    unsigned int mLowerBound;
    unsigned int mMaxHits;
    //Association filter.
    BestCommonHits mBestCommon;
    
    vector<StMcTrack*>::iterator mCurrentMc;
    vector<StMcTrack*>::iterator mBeginMc;
    vector<StMcTrack*>::iterator mEndMc;
};

#endif


