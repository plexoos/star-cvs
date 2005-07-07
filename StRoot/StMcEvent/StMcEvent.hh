/***************************************************************************
 *
 * $Id: StMcEvent.hh,v 2.19 2005/07/07 18:20:49 calderon Exp $
 * $Log: StMcEvent.hh,v $
 * Revision 2.19  2005/07/07 18:20:49  calderon
 * Added support for IGT detector.
 *
 * Revision 2.18  2005/05/27 23:37:25  calderon
 * Update for EEMC, add eprs, esmdu esdmv hits to StMcEvent.
 *
 * Revision 2.17  2005/04/18 20:11:32  calderon
 * Addition of Fgt and Fst files.  Modified other files to accomodate changes.
 *
 * Revision 2.16  2005/01/27 23:40:47  calderon
 * Adding persistency to StMcEvent as a step for Virtual MonteCarlo.
 *
 * Revision 2.15  2004/09/14 05:00:29  calderon
 * Added support for Ist, Ssd and changes to Pixel, from "El Kai".
 *
 * Revision 2.14  2004/01/13 21:03:34  fisyak
 * Replace iostream by Stiostream.h (for icc)
 *
 * Revision 2.13  2003/12/04 05:56:47  calderon
 * Inclusion of Endcap EMC hit collection in StMcEvent and
 * of the Endcap hit vector in StMcTrack.
 * fix const of StMcVertex::parent() to avoid warnings in user code
 *
 * Revision 2.12  2003/10/08 20:17:55  calderon
 * -using <iostream>, std::cout, std::ostream.
 * -changes in FTPC volume Id.
 *   o Causes changes in decoding of plane().
 *   o sector() is added.
 *   o print volumeId and sector() in the operator<<.
 *
 * Revision 2.11  2003/09/02 17:58:41  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.10  2003/08/20 18:50:21  calderon
 * Addition of Tof classes and Pixel classes.  Modified track, event, and
 * container code to reflect this.
 * Fix bug in StMcVertex and in clearing of some hit collections.
 *
 * Revision 2.9  2003/05/15 18:28:47  calderon
 * Added data members from modified g2t_event table:
 * Event Generator Final State Tracks, N Binary Collisions,
 * N Wounded Nucleons East and West, N Jets.
 *
 * Revision 2.8  2003/03/18 22:37:43  calderon
 * Added member mSubProcessId which is used for Pythia events.
 * Only is set from constructor from g2t_event table.
 *
 * Revision 2.7  2003/02/19 03:16:05  calderon
 * Introduction of Ctb Hit Class and Ctb Hit Collection class, modified
 * StMcTrack, and StMcEvent accordingly.  Clearing of hits in StMcSvtWaferHitCollection.
 *
 * Revision 2.6  2000/06/06 02:58:40  calderon
 * Introduction of Calorimeter classes.  Modified several classes
 * accordingly.
 *
 * Revision 2.5  2000/04/17 23:01:15  calderon
 * Added local momentum to hits as per Lee's request
 *
 * Revision 2.4  2000/03/06 18:05:21  calderon
 * 1) Modified SVT Hits storage scheme from layer-ladder-wafer to
 * barrel-ladder-wafer.
 * 2) Added Rich Hit class and collection, and links to them in other
 * classes.
 *
 * Revision 2.3  2000/01/18 20:52:31  calderon
 * Works with CC5
 *
 * Revision 2.2  1999/12/03 00:51:52  calderon
 * Tested with new StMcEventMaker.  Added messages for
 * diagnostics.
 *
 * Revision 2.1  1999/11/19 19:06:31  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:12:15  calderon
 * Completely revised for new StEvent
 *
 * Revision 1.4  1999/09/24 01:23:15  fisyak
 * Reduced Include Path
 *
 * Revision 1.3  1999/09/23 21:25:50  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 * Revision 1.2  1999/07/28 20:27:32  calderon
 * Version with SL99f libraries
 *
 * 
 **************************************************************************/
#ifndef StMcEvent_hh
#define StMcEvent_hh

#include "Stiostream.h"
#include "StMcContainers.hh" 
#include "TString.h"
#include "TDataSet.h"
class StMcTpcHitCollection;
class StMcFtpcHitCollection;
class StMcRichHitCollection;
class StMcCtbHitCollection;
class StMcSvtHitCollection;
class StMcSsdHitCollection;
class StMcEmcHitCollection;
class StMcTofHitCollection;
class StMcPixelHitCollection;
class StMcIstHitCollection;
class StMcIgtHitCollection;
class StMcFstHitCollection;
class StMcFgtHitCollection;
class StMcVertex;
class g2t_event_st;

class StMcEvent : public TDataSet {
    
public:
    StMcEvent();  
    StMcEvent(g2t_event_st*);
    virtual ~StMcEvent();
    
    int operator==(const StMcEvent&) const;
    int operator!=(const StMcEvent&) const;
    
    void initToZero();


    //"Get" Methods
  
    // The following stuff will be read directly from g2t_event table
    static const TString&        cvsTag() {return mCvsTag; }   
    
    unsigned long                eventGeneratorEventLabel() const;
    unsigned long                eventNumber() const;
    unsigned long                runNumber() const;              
    unsigned long                type() const;
    unsigned long                zWest() const;
    unsigned long                nWest() const;
    unsigned long                zEast() const;
    unsigned long                nEast() const;
    unsigned long                eventGeneratorFinalStateTracks() const;
    unsigned long                numberOfPrimaryTracks() const;
    unsigned long                subProcessId() const;  
    float                        impactParameter() const;
    float                        phiReactionPlane() const;
    float                        triggerTimeOffset() const;
    unsigned long                nBinary() const;
    unsigned long                nWoundedEast() const;
    unsigned long                nWoundedWest() const;
    unsigned long                nJets() const;
    
    
    StMcVertex*                    primaryVertex();
    const StMcVertex*              primaryVertex() const;
    StSPtrVecMcVertex&             vertices();
    const StSPtrVecMcVertex&       vertices() const;
    StSPtrVecMcTrack&              tracks();
    const StSPtrVecMcTrack&        tracks() const;
    StMcTpcHitCollection*          tpcHitCollection();
    const StMcTpcHitCollection*    tpcHitCollection()const;
    StMcSvtHitCollection*          svtHitCollection();
    const StMcSvtHitCollection*    svtHitCollection() const;
    StMcSsdHitCollection*          ssdHitCollection();
    const StMcSsdHitCollection*    ssdHitCollection() const;
    StMcFtpcHitCollection*         ftpcHitCollection();
    const StMcFtpcHitCollection*   ftpcHitCollection() const;
    StMcRichHitCollection*         richHitCollection();
    const StMcRichHitCollection*   richHitCollection() const;
    StMcCtbHitCollection*          ctbHitCollection();
    const StMcCtbHitCollection*    ctbHitCollection() const;
    
    StMcEmcHitCollection*          bemcHitCollection();
    const StMcEmcHitCollection*    bemcHitCollection() const;
    StMcEmcHitCollection*          bprsHitCollection();
    const StMcEmcHitCollection*    bprsHitCollection() const;

    StMcEmcHitCollection*          bsmdeHitCollection();
    const StMcEmcHitCollection*    bsmdeHitCollection() const;
    StMcEmcHitCollection*          bsmdpHitCollection();
    const StMcEmcHitCollection*    bsmdpHitCollection() const;

    StMcTofHitCollection*          tofHitCollection();
    const StMcTofHitCollection*    tofHitCollection() const;

    StMcEmcHitCollection*          eemcHitCollection();
    const StMcEmcHitCollection*    eemcHitCollection() const;
    StMcEmcHitCollection*          eprsHitCollection();
    const StMcEmcHitCollection*    eprsHitCollection() const;
    StMcEmcHitCollection*          esmduHitCollection();
    const StMcEmcHitCollection*    esmduHitCollection() const;
    StMcEmcHitCollection*          esmdvHitCollection();
    const StMcEmcHitCollection*    esmdvHitCollection() const;
    
    StMcPixelHitCollection*        pixelHitCollection();
    const StMcPixelHitCollection*  pixelHitCollection() const;
    StMcIstHitCollection*          istHitCollection();
    const StMcIstHitCollection*    istHitCollection() const;
    StMcIgtHitCollection*          igtHitCollection();
    const StMcIgtHitCollection*    igtHitCollection() const;
    StMcFstHitCollection*          fstHitCollection();
    const StMcFstHitCollection*    fstHitCollection() const;
    StMcFgtHitCollection*          fgtHitCollection();
    const StMcFgtHitCollection*    fgtHitCollection() const;

    // "Set" Methods
    
    void setEventGeneratorEventLabel(unsigned long);
    void setEventNumber(unsigned long);
    void setRunNumber(unsigned long);
    void setType(unsigned long);
    void setZWest(unsigned long);
    void setNWest(unsigned long);
    void setZEast(unsigned long);
    void setNEast(unsigned long);
    void setEventGeneratorFinalStateTracks(unsigned long);
    void setNumberOfPrimaryTracks(unsigned long);
    void setImpactParameter(float);
    void setPhiReactionPlane(float);
    void setTriggerTimeOffset(float);
    void setNBinary(unsigned long);
    void setNWoundedEast(unsigned long);
    void setNWoundedWest(unsigned long);
    void setNJets(unsigned long);
    void setPrimaryVertex(StMcVertex*);  
    void setTpcHitCollection(StMcTpcHitCollection*);               
    void setSvtHitCollection(StMcSvtHitCollection*);               
    void setSsdHitCollection(StMcSsdHitCollection*);               
    void setFtpcHitCollection(StMcFtpcHitCollection*);              
    void setRichHitCollection(StMcRichHitCollection*);
    void setCtbHitCollection(StMcCtbHitCollection*);              
    void setBemcHitCollection(StMcEmcHitCollection*);              
    void setBprsHitCollection(StMcEmcHitCollection*);              
    void setBsmdeHitCollection(StMcEmcHitCollection*);              
    void setBsmdpHitCollection(StMcEmcHitCollection*);              
    void setTofHitCollection(StMcTofHitCollection*);
    void setEemcHitCollection(StMcEmcHitCollection*);              
    void setEprsHitCollection(StMcEmcHitCollection*);              
    void setEsmduHitCollection(StMcEmcHitCollection*);              
    void setEsmdvHitCollection(StMcEmcHitCollection*);
    void setPixelHitCollection(StMcPixelHitCollection*);       
    void setIstHitCollection(StMcIstHitCollection*);       
    void setIgtHitCollection(StMcIgtHitCollection*);       
    void setFstHitCollection(StMcFstHitCollection*);       
    void setFgtHitCollection(StMcFgtHitCollection*);       

protected:
    unsigned long                  mEventGeneratorEventLabel;
    unsigned long                  mEventNumber;
    unsigned long                  mRunNumber;
    unsigned long                  mType;  
    unsigned long                  mZWest;
    unsigned long                  mNWest;
    unsigned long                  mZEast;
    unsigned long                  mNEast;
    unsigned long                  mEvGenFSTracks; // Number of final state event generator tracks
    unsigned long                  mPrimaryTracks;
    unsigned long                  mSubProcessId; // Pythia subprocess Id
    float                          mImpactParameter;
    float                          mPhiReactionPlane;
    float                          mTriggerTimeOffset; // time offset wrt trigger event
    unsigned long                  mNBinary;           // Number of Binary Collisions
    unsigned long                  mNWoundedEast;      // Number of Wounded Nucleons East
    unsigned long                  mNWoundedWest;      // Number of Wounded Nucleons West
    unsigned long                  mNJets;             // Number of Jets
    StMcVertex*                    mPrimaryVertex;
    StSPtrVecMcVertex              mVertices;
    StSPtrVecMcTrack               mTracks;
    StMcTpcHitCollection*          mTpcHits;
    StMcSvtHitCollection*          mSvtHits;
    StMcSsdHitCollection*          mSsdHits;
    StMcFtpcHitCollection*         mFtpcHits;
    StMcRichHitCollection*         mRichHits;
    StMcCtbHitCollection*          mCtbHits;
    StMcEmcHitCollection*          mBemcHits;
    StMcEmcHitCollection*          mBprsHits;
    StMcEmcHitCollection*          mBsmdeHits;
    StMcEmcHitCollection*          mBsmdpHits;
    StMcTofHitCollection*          mTofHits;
    StMcEmcHitCollection*          mEemcHits;
    StMcEmcHitCollection*          mEprsHits;
    StMcEmcHitCollection*          mEsmduHits;
    StMcEmcHitCollection*          mEsmdvHits;
    StMcPixelHitCollection*        mPixelHits;
    StMcIstHitCollection*          mIstHits;
    StMcIgtHitCollection*          mIgtHits;
    StMcFstHitCollection*          mFstHits;
    StMcFgtHitCollection*          mFgtHits;
    static TString                 mCvsTag;
private:
    const StMcEvent& operator=(const StMcEvent&);
    StMcEvent(const StMcEvent&);
    ClassDef(StMcEvent,1)
};

ostream&  operator<<(ostream& os, const StMcEvent&);

// Definition of "Get" methods

inline unsigned long StMcEvent::eventGeneratorEventLabel() const { return mEventGeneratorEventLabel; }

inline unsigned long StMcEvent::eventNumber() const { return mEventNumber; }

inline unsigned long StMcEvent::runNumber() const { return mRunNumber;}             

inline unsigned long StMcEvent::type() const { return mType;}             

inline unsigned long StMcEvent::zWest() const { return mZWest;}

inline unsigned long StMcEvent::nWest() const { return mNWest;}

inline unsigned long StMcEvent::zEast() const { return mZEast;}

inline unsigned long StMcEvent::nEast() const { return mNEast;}

inline unsigned long StMcEvent::eventGeneratorFinalStateTracks() const { return mEvGenFSTracks;}

inline unsigned long StMcEvent::numberOfPrimaryTracks() const { return mPrimaryTracks;}

inline unsigned long StMcEvent::subProcessId() const { return mSubProcessId;}

inline float StMcEvent::impactParameter() const { return mImpactParameter; }

inline float StMcEvent::phiReactionPlane() const { return mPhiReactionPlane; }

inline float StMcEvent::triggerTimeOffset() const { return mTriggerTimeOffset;}

inline unsigned long StMcEvent::nBinary() const { return mNBinary;}
				
inline unsigned long StMcEvent::nWoundedEast() const { return mNWoundedEast;}
				
inline unsigned long StMcEvent::nWoundedWest() const { return mNWoundedWest;}
				
inline unsigned long StMcEvent::nJets() const { return mNJets;}

inline StMcVertex* StMcEvent::primaryVertex() { return mPrimaryVertex;}

inline const StMcVertex* StMcEvent::primaryVertex() const { return mPrimaryVertex;}

inline StSPtrVecMcVertex& StMcEvent::vertices() { return mVertices;}

inline const StSPtrVecMcVertex& StMcEvent::vertices() const { return mVertices;}

inline StSPtrVecMcTrack& StMcEvent::tracks() { return mTracks;}

inline const StSPtrVecMcTrack& StMcEvent::tracks() const { return mTracks;}

inline StMcTpcHitCollection* StMcEvent::tpcHitCollection() { return mTpcHits;}

inline const StMcTpcHitCollection* StMcEvent::tpcHitCollection() const { return mTpcHits;}

inline StMcSvtHitCollection* StMcEvent::svtHitCollection() { return mSvtHits;}

inline const StMcSvtHitCollection* StMcEvent::svtHitCollection() const { return mSvtHits;}

inline StMcSsdHitCollection* StMcEvent::ssdHitCollection() { return mSsdHits;}

inline const StMcSsdHitCollection* StMcEvent::ssdHitCollection() const { return mSsdHits;}

inline StMcFtpcHitCollection* StMcEvent::ftpcHitCollection() { return mFtpcHits;}

inline const StMcFtpcHitCollection* StMcEvent::ftpcHitCollection() const { return mFtpcHits;}

inline StMcRichHitCollection* StMcEvent::richHitCollection() { return mRichHits;}

inline const StMcRichHitCollection* StMcEvent::richHitCollection() const { return mRichHits;}

inline StMcCtbHitCollection* StMcEvent::ctbHitCollection() { return mCtbHits;}

inline const StMcCtbHitCollection* StMcEvent::ctbHitCollection() const { return mCtbHits;}

inline StMcEmcHitCollection* StMcEvent::bemcHitCollection() { return mBemcHits;}

inline const StMcEmcHitCollection* StMcEvent::bemcHitCollection() const {return mBemcHits;}

inline StMcEmcHitCollection* StMcEvent::bprsHitCollection() { return mBprsHits;}

inline const StMcEmcHitCollection* StMcEvent::bprsHitCollection() const {return mBprsHits;}

inline StMcEmcHitCollection* StMcEvent::bsmdeHitCollection() { return mBsmdeHits;}

inline const StMcEmcHitCollection* StMcEvent::bsmdeHitCollection() const {return mBsmdeHits;}

inline StMcEmcHitCollection* StMcEvent::bsmdpHitCollection() { return mBsmdpHits;}

inline const StMcEmcHitCollection* StMcEvent::bsmdpHitCollection() const {return mBsmdpHits;}

inline StMcTofHitCollection* StMcEvent::tofHitCollection() { return mTofHits;}

inline const StMcTofHitCollection* StMcEvent::tofHitCollection() const { return mTofHits;}

inline StMcEmcHitCollection* StMcEvent::eemcHitCollection() { return mEemcHits;}

inline const StMcEmcHitCollection* StMcEvent::eemcHitCollection() const {return mEemcHits;}

inline StMcEmcHitCollection* StMcEvent::eprsHitCollection() { return mEprsHits;}

inline const StMcEmcHitCollection* StMcEvent::eprsHitCollection() const {return mEprsHits;}

inline StMcEmcHitCollection* StMcEvent::esmduHitCollection() { return mEsmduHits;}

inline const StMcEmcHitCollection* StMcEvent::esmduHitCollection() const {return mEsmduHits;}

inline StMcEmcHitCollection* StMcEvent::esmdvHitCollection() { return mEsmdvHits;}

inline const StMcEmcHitCollection* StMcEvent::esmdvHitCollection() const {return mEsmdvHits;}

inline StMcPixelHitCollection* StMcEvent::pixelHitCollection() { return mPixelHits;}

inline const StMcPixelHitCollection* StMcEvent::pixelHitCollection() const { return mPixelHits;}

inline StMcIstHitCollection* StMcEvent::istHitCollection() { return mIstHits;}

inline const StMcIstHitCollection* StMcEvent::istHitCollection() const { return mIstHits;}

inline StMcIgtHitCollection* StMcEvent::igtHitCollection() { return mIgtHits;}

inline const StMcIgtHitCollection* StMcEvent::igtHitCollection() const { return mIgtHits;}

inline StMcFstHitCollection* StMcEvent::fstHitCollection() { return mFstHits;}

inline const StMcFstHitCollection* StMcEvent::fstHitCollection() const { return mFstHits;}

inline StMcFgtHitCollection* StMcEvent::fgtHitCollection() { return mFgtHits;}

inline const StMcFgtHitCollection* StMcEvent::fgtHitCollection() const { return mFgtHits;}
#endif

