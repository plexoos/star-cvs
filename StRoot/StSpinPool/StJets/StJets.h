// -*- mode: c++;-*-
// $Id: StJets.h,v 1.1 2008/06/01 03:41:45 tai Exp $
#ifndef StJets_h
#define StJets_h

#include "TObject.h"
#include "TClonesArray.h"

#include <vector>


#include "TrackToJetIndex.h"

class StJet;

/*!
  \class StJets
  \authro T.Henry (Texas A&M)
  StJets persistently encapsulates the event-wise results of a given jet algorithm.  That is,
  it stores a container of StJet objects.  Additionally, it also stores some information
  to persistently store  the parent-daughter relationsip between jets and tracks.
 */
class StJets : public TObject
{
public:

    StJets();
    virtual ~StJets();
    
    void Clear(bool clearTracks = false);
    void Clear(const char *opt);
        
    ///Set the BEMC corrupt flag.  true --> event is corrupt, no jet finding was performed
  void setBemcCorrupt(bool v) { mCorrupt = v; }
  bool bemcCorrupt() const { return mCorrupt; }

    ///The number of jets found in this event
    int nJets() {return mJets->GetLast()+1;}

    ///Access to the jets in this event.
    TClonesArray* jets() {return mJets;}

  void addJet(StJet& jet);

    ///The track to jet indices TClonesArray: this contains _all_ the 4momenta contained in jets for jet finding!  This is for expert use only
    TClonesArray* indices() {return mTrackToJetIndices;}

  void addTrackToIndex(TrackToJetIndex &t2j);
    
    ///Here's how you get the 4-momenta of a particles in a given jet.  This contains tracks and energy-corrected-towers.  Use this for Frag. Function
  std::vector<TrackToJetIndex*> particles(int jetIndex);

    // don't use this. this is for test.
    TObjArray particles_(int jetIndex);
    
    ///Access to a container of the charged-tracks associated with a jet
  //    TrackVec jetParticles(StMuDst*, int jetIndex);
    
    ///access to event numbers, used to synchronize with StMuDstMaker for simultaneous reading
  int eventId() { return mEventId; }
  int eventNumber() {return mEventNumber; }
  int runId() { return mRunId; }
  int runNumber() { return mRunNumber; }

  void seteventId(int v) { mEventId = v; }
  void seteventNumber(int v) { mEventNumber = v; }
  void setrunId(int v) { mRunId = v; }
  void setrunNumber(int v) { mRunNumber = v; }

    ///A double check, used to synchronize with StMuDstMaker for simultaneous reading
  //    bool isSameEvent(const StMuDst*);

    ///Number of towers with e>0.4 GeV (after status check)
    int nDylanPoints() const {return mDylanPoints;}
    void setDylanPoints(int v) {mDylanPoints = v;}

    ///Summed energy of towers with e>0.4 (after status check)
    double sumEmcE() const {return mSumEmcE;}
    void setSumEmcE(double v) {mSumEmcE = v;}

public:
    ///User Interface as per Thomas H's request.  Access jet kinematics based on index:
    
    double e(int) ;
    double et(int) ;
    double p(int) ;
    double pt(int) ;
    double phi(int) ;
    double eta(int) ;
    int nCell(int) ;
    int charge(int) ;

private:

    int mDylanPoints;
    double mSumEmcE;
    int mEventId;
    int mEventNumber;
    int mRunId;
    int mRunNumber;
    bool mCorrupt;

    bool inBounds(int);
    StJet* jet(int);
    
    TClonesArray* mJets;
    TClonesArray* mTrackToJetIndices;
    
    ClassDef(StJets,1)
};

//inlines
inline void StJets::Clear(const char *opt)
{
    TObject::Clear(opt);
    mEventId = mEventNumber = mRunId = mRunNumber = 0;
    mCorrupt = false;
}

#endif // StJets_h

