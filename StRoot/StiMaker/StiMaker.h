//StiMaker.h
//M.L. Miller (Yale Software)
//04/01

#ifndef StiMaker_HH
#define StiMaker_HH

#include "StMaker.h"
#include "StEvent/StEnumerations.h"
#include "Sti/StiHit.h" //For StiHitFactory
#include "Sti/StiCompositeTreeNode.h" //For data_node_factory
#include "Sti/StiTrackNode.h"
#include "Sti/StiKalmanTrackNode.h"

#include "StiGui/StiRootDrawableStiEvaluableTrack.h" //For EvaluableTrackFactory
#include "StiGui/StiRootDrawableDetector.h"

class StEvent;
class StiHitContainer;
class StiHitFiller;
class StiDisplayManager;
class StiDetectorContainer;
class StiTrackContainer;
class StiDrawableHits;
class StiEvaluableTrackSeedFinder;
class StiTrackSeedFinder;
class StiTrackFinder;
class StiKalmanTrackFinder;
class StiCompositeSeedFinder;

class StiMaker : public StMaker {
 public:
    typedef StiObjectFactory<StiKalmanTrack> StiKalmanTrackFactory;
    
    virtual ~StiMaker();

    virtual void  Clear(const char* opt="");
    virtual Int_t Init();
    virtual Int_t Make();
    virtual Int_t Finish();

    virtual const char* GetCVS() const
    {static const char cvs[]="Tag $Name:  $ $Id: StiMaker.h,v 1.18 2001/08/30 18:26:54 mmiller Exp $ built "__DATE__" "__TIME__; return cvs;}	

public:

    //Singleton access
    static StiMaker* instance();
    static void kill();

    //Gets/sets
    void setMaterialBuildPath(char* val);
    void setDetectorBuildPath(char* val);
    void setDrawableStTrackType(StTrackType);

    void printStatistics() const;
    
    //Used for stepping to next action (via StiControlPad)
    void doNextAction();
    
protected:
    StiMaker(const char* name = "StiMaker");

private:

    //Containers
    StiHitContainer* mhitstore; //!
    StiDetectorContainer* mdetector; //!
    StiTrackContainer* mtrackstore; //!

    //Factories
    StiHitFactory* mhitfactory; //!
    StiEvaluableTrackFactory* mtrackfactory; //!
    StiKalmanTrackNodeFactory* mktracknodefactory; //!
    detector_factory* mdetectorfactory; //!
    data_node_factory* mdatanodefactory; //!
    StiKalmanTrackFactory* mkalmantrackfactory; //!

    //Display
    StiDisplayManager* mdisplay; //!
    StiDrawableHits* mdrawablehits; //!
    
    //Utilites
    StiHitFiller* mhitfiller; //!
    StiEvaluableTrackSeedFinder* mtrackseedfinder; //!
    StiTrackSeedFinder* mkalmanseedfinder; //!

    //TEST!!!!!!!
    StiTrackSeedFinder* mtempseedfinder; //!
    StiCompositeSeedFinder* mcompseedfinder; //!

    //Tracker
    StiKalmanTrackFinder* mtracker; //!
    
    char* mmaterialbuildpath; //!
    char* mdetectorbuildpath; //!

    StTrackType mStTrackType;
    
    static StiMaker* sinstance; //!

private:
    StEvent* mevent; //!
    ClassDef(StiMaker, 1)

};

//inlines

inline void StiMaker::setDrawableStTrackType(StTrackType val)
{
    mStTrackType = val;
}

#endif




