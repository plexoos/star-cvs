//////////////////////////////////////////////////////////////////////
//
// $Id: StJets.h,v 1.7 2005/01/27 18:39:03 mmiller Exp $
// $Log: StJets.h,v $
// Revision 1.7  2005/01/27 18:39:03  mmiller
// Added some extra accessors to StJet object to keep track of Et from TPC, BTOW, ETOW, etc.
//
// Revision 1.6  2004/12/07 20:03:35  mmiller
// Fixed the tracking of the software-id (tower index) of barrel towers in the jet.
//
// Revision 1.5  2004/11/30 19:01:38  mmiller
// Back compatibility for pre P04k bemc corrupt events
//
// Revision 1.4  2004/09/22 15:46:21  mmiller
// Added a double check to verify that jet 4p is equal to the vector sum of
// the particles 4-p.  Removed troublesome access methods to StJets.  See
// StJetReader::exampleEventAna() for access to jet-particles.
//
// Revision 1.3  2004/09/20 23:15:52  mmiller
// Fixed bug in retreiving emc towers for jet, introduced
// TrackToJetIndex inherits from TLorentzVector now.  See StJetReader::exampleAna
// for example of how to retreive the corrected 4-momenta used for barrel towers.
//
// Revision 1.2  2004/09/10 18:13:53  mmiller
// Two fixes:
// 1) add StDetectorId to the TTree to allow sorting of jet particles into
// StMuTrack and BemcTowers.  See StJetReader::exampleEventAna() for usage
//
// 2) removed a continue line in StJetMaker::Make that created a non-synch between
// the jet tree and the MuDst
//
// Revision 1.1  2004/07/08 15:41:04  mmiller
// First release of StJetMaker.  Mike's commit of full clean of StJetFinder, StJetMaker, and StSpinMaker.  builds and runs in dev.
//
// Revision 1.5  2003/09/02 17:59:01  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.4  2003/05/20 20:22:45  thenry
// Moved body of jetTrackIndices to cxx.
//
// Revision 1.3  2003/05/15 17:48:27  thenry
// Previous versions of StJets expected only primary TPC tracks to be used by
// the jet maker.  That changed with the introduction of EMC points.
// Therefore, a bug existed in jetParticles, because this function
// assumed that all the TrackToJetIndices were valid primary TPC track indices.
// This bug has been fixed, so that if the TrackToJetIndex is greater than
// the number of primary tracks, that index is skipped in the construction
// of the StJets::TrackVec.  Therefore, the StJets::jetParticles function NOW
// does exactly what it did before, completely ignoring EMC Points, even when
// they contribute to the jet.
//
// In addition, a new function was added: jetTrackIndices(), which returns a
// vector of integers corresponding to TPC track indices with the addition of
// (EMC Point index + number TPC primary tracks)).  This function then allows
// us to determine which tracks and which points (their indexes at least) are
// part of each jet, even if we do not have a correctly filled StppEvent*.
//
// Revision 1.2  2003/04/01 23:45:04  thenry
// Added jet track accessor functions:
// numTracks, tracksPt, tracksPhi, tracksEta
//
// Revision 1.1  2002/12/04 20:28:08  thenry
// StppuDstMaker was modified to allow multiple jet analysis modules to be
// run simultaneosly with various parameters while the Maker loads the events
// and analyses them.  Four different jet analyzers exist:
//
// Konstanin's Analyzers:
//     Kt type: StppKonstKtJetAnalyzer
//     Cone type: StppKonstConeJetAnalyzer
//
// Mike's Analyzers:
//     Kt type: StppMikeKtJetAnalyzer
//     Cone type: StppMikeConeJetAnalyzer
//
// These modules all require the StJetFinder modules.
//
//
// Revision 1.0  2002/02/11 20:30:48  Henry
// Adapted from StJet.h by Akio Ogawa
//
//////////////////////////////////////////////////////////////////////
//
// StJets
//
// Branch for multiple jets and an array of track->jet indices
//
//////////////////////////////////////////////////////////////////////
#ifndef StJets_h
#define StJets_h

#include <vector>
#include <iostream>
#include <string>
using std::vector;
#include <cmath>
#include "TObject.h"
#include "TClonesArray.h"
#include "TLorentzVector.h"
#include "StDetectorId.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"

class StProtoJet;
class StJet;
class StMuDst;

//!class TrackToJetIndex : public TObject
class TrackToJetIndex : public TLorentzVector
{
public:
    TrackToJetIndex(int ji=-1, int ti=-1, StDetectorId id=kUnknownId) : mJetIndex(ji), mTrackIndex(ti) , mDetId(id) {};
    virtual ~TrackToJetIndex() {};
    
    void setJetIndex(int n) {mJetIndex=n;}
    int jetIndex() const {return mJetIndex;}

    ///Note, trackIndex is the index of the track in the primaryTracks array, if detectorId==kTpcId.
    ///If detectorId==kBemcTowerId, it is the tower index (actually software id)
    void setTrackIndex(int n) {mTrackIndex=n;}
    int trackIndex() const {return mTrackIndex;}

    void setDetectorId(StDetectorId v) {mDetId=v;}
    StDetectorId detectorId() const {return mDetId;}
    
private:
    int mJetIndex;
    int mTrackIndex;
    StDetectorId mDetId;
    
    ClassDef(TrackToJetIndex,1)
};

inline ostream& operator<<(ostream& os, const TrackToJetIndex& t)
{
    string idstring;
    StDetectorId mDetId = t.detectorId();
    if (mDetId==kTpcId) {
	idstring = "kTpcId";
    }
    else if (mDetId==kBarrelEmcTowerId) {
	idstring = "kBarrelEmcTowerId";
    }
    else if (mDetId==kEndcapEmcTowerId) {
	idstring = "kEndcapEmcTowerId";
    }
    else {
	idstring = "kUnknown";
    }
    
    return os <<"jetIndex:\t"<<t.jetIndex()<<"\ttrackIndex:\t"<<t.trackIndex()<<"\tdetId:\t"<<t.detectorId()<<"\t"<<idstring;
}

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
    typedef vector<StMuTrack*> TrackVec;

    StJets();
    virtual ~StJets();
    
    void Clear(bool clearTracks = false);
    void Clear(const char *opt);
        
    ///add a jet to the container
    void addProtoJet(StProtoJet& pj);

    ///Set event-wise information:
    void setMuDst(const StMuDst*);

    ///Set the BEMC corrupt flag.  true --> event is corrupt, no jet finding was performed
    void setBemcCorrupt(bool v);
    bool bemcCorrupt() const;

    ///The number of jets found in this event
    int nJets() {return mJets->GetLast()+1;}

    ///Access to the jets in this event.
    TClonesArray* jets() {return mJets;}

    ///The track to jet indices TClonesArray: this contains _all_ the 4momenta contained in jets for jet finding!  This is for expert use only
    TClonesArray* indices() {return mTrackToJetIndices;}
    
    ///Here's how you get the 4-momenta of a particles in a given jet.  This contains tracks and energy-corrected-towers.  Use this for Frag. Function
    vector<TrackToJetIndex*> particles(int jetIndex);
    
    ///Access to a container of the charged-tracks associated with a jet
    TrackVec jetParticles(StMuDst*, int jetIndex);
    
    ///access to event numbers, used to synchronize with StMuDstMaker for simultaneous reading
    int eventId();
    int eventNumber();
    int runId();
    int runNumber();

    ///A double check, used to synchronize with StMuDstMaker for simultaneous reading
    bool isSameEvent(const StMuDst*);

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
inline int StJets::eventId()
{
    return mEventId;
}

inline int StJets::eventNumber()
{
    return mEventNumber;
}

inline int StJets::runId()
{
    return mRunId;
}

inline int StJets::runNumber()
{
    return mRunNumber;
}

inline void StJets::Clear(const char *opt)
{
    TObject::Clear(opt);
    mEventId = mEventNumber = mRunId = mRunNumber = 0;
    mCorrupt = false;
}

inline void StJets::setBemcCorrupt(bool v)
{
    mCorrupt = v;
}

inline bool StJets::bemcCorrupt() const
{
    return mCorrupt;
}


//non-members ---------------------

#endif
