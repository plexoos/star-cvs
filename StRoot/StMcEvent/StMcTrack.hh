/***************************************************************************
 *
 * $Id: StMcTrack.hh,v 2.13 2003/12/04 05:56:47 calderon Exp $
 * $Log: StMcTrack.hh,v $
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
 * Revision 2.9  2003/02/19 03:16:05  calderon
 * Introduction of Ctb Hit Class and Ctb Hit Collection class, modified
 * StMcTrack, and StMcEvent accordingly.  Clearing of hits in StMcSvtWaferHitCollection.
 *
 * Revision 2.9  2003/02/18 00:00:00  gans
 * Introduction of Ctb classes.  Modified several classes
 * accordingly.
 *
 * $Id: StMcTrack.hh,v 2.13 2003/12/04 05:56:47 calderon Exp $
 * $Log: StMcTrack.hh,v $
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
 * Revision 2.9  2003/02/19 03:16:05  calderon
 * Introduction of Ctb Hit Class and Ctb Hit Collection class, modified
 * StMcTrack, and StMcEvent accordingly.  Clearing of hits in StMcSvtWaferHitCollection.
 *
 * Revision 2.8  2000/06/06 02:58:42  calderon
 * Introduction of Calorimeter classes.  Modified several classes
 * accordingly.
 *
 * Revision 2.7  2000/04/06 23:29:10  calderon
 * Store the parent track for all tracks.
 *
 * Revision 2.6  2000/04/06 08:34:56  calderon
 * Version using the particle table:
 * 1) Constructor for particle_st*
 * 2) Pointer to parent track from particle table
 * 3) PDG encoding when track is from particle table
 * 4) Generator label, used to index entries in the table for debugging
 *
 * Revision 2.5  2000/04/04 22:25:24  calderon
 * add inline function to return primary key from g2t table
 *
 * Revision 2.4  2000/03/06 18:05:23  calderon
 * 1) Modified SVT Hits storage scheme from layer-ladder-wafer to
 * barrel-ladder-wafer.
 * 2) Added Rich Hit class and collection, and links to them in other
 * classes.
 *
 * Revision 2.3  2000/01/18 20:52:31  calderon
 * Works with CC5
 *
 * Revision 2.2  1999/12/03 00:51:53  calderon
 * Tested with new StMcEventMaker.  Added messages for
 * diagnostics.
 *
 * Revision 2.1  1999/11/19 19:06:34  calderon
 * Recommit after redoing the files.
 *
 * Revision 2.0  1999/11/17 02:12:16  calderon
 * Completely revised for new StEvent
 *
 * Revision 1.4  1999/09/24 01:23:17  fisyak
 * Reduced Include Path
 *
 * Revision 1.3  1999/09/23 21:25:54  calderon
 * Added Log & Id
 * Modified includes according to Yuri
 *
 *
 **************************************************************************/
#ifndef StMcTrack_hh
#define StMcTrack_hh 
#include <iostream>
#ifndef ST_NO_NAMESPACES
using std::ostream;
using std::cout;
#endif
#include "StLorentzVectorF.hh"
#include "StMcContainers.hh"
//#include "StTrackTopologyMap.h"
class StParticleDefinition;
class g2t_track_st;
class particle_st;
class StMcTrack;

class StMcTrack {
public:
    StMcTrack();
    virtual ~StMcTrack();
    StMcTrack(g2t_track_st* trk);
    StMcTrack(particle_st* trk);
    
    // StMcTrack(const StMcTrack&);                     use default
    // const StMcTrack & operator=(const StMcTrack&);   use default
    
    int operator==(const StMcTrack&) const;
    int operator!=(const StMcTrack&) const;

    void initToZero();
  // "Get" Methods
    const StLorentzVectorF&         fourMomentum() const; //!
    const StThreeVectorF&           momentum() const; //!
    float                           energy() const; //!
    float                           pt() const; //!
    float                           rapidity() const; //!
    float                           pseudoRapidity() const; //!
    StMcVertex*                     startVertex(); //!
    const StMcVertex*               startVertex() const; //!
    StMcVertex*                     stopVertex(); //!
    const StMcVertex*               stopVertex() const; //!
    StMcTrack*                      parent(); //!
    const StMcTrack*                parent() const; //!
    StPtrVecMcVertex&               intermediateVertices(); //!
    const StPtrVecMcVertex&         intermediateVertices() const; //!
    StPtrVecMcTpcHit&               tpcHits(); //!
    const StPtrVecMcTpcHit&         tpcHits() const; //!
    StPtrVecMcSvtHit&               svtHits(); //!
    const StPtrVecMcSvtHit&         svtHits() const; //!
    StPtrVecMcFtpcHit&              ftpcHits(); //!
    const StPtrVecMcFtpcHit&        ftpcHits() const; //!
    StPtrVecMcRichHit&              richHits(); //!
    const StPtrVecMcRichHit&        richHits() const; //!
    StPtrVecMcCtbHit&               ctbHits(); //!
    const StPtrVecMcCtbHit&         ctbHits() const; //!
    StPtrVecMcCalorimeterHit&       bemcHits(); //!
    const StPtrVecMcCalorimeterHit& bemcHits() const; //!
    StPtrVecMcCalorimeterHit&       bprsHits(); //!
    const StPtrVecMcCalorimeterHit& bprsHits() const; //!
    StPtrVecMcCalorimeterHit&       bsmdeHits(); //!
    const StPtrVecMcCalorimeterHit& bsmdeHits() const; //!
    StPtrVecMcCalorimeterHit&       bsmdpHits(); //!
    const StPtrVecMcCalorimeterHit& bsmdpHits() const; //!
    StPtrVecMcTofHit&               tofHits(); //!
    const StPtrVecMcTofHit&         tofHits() const; //!
    StPtrVecMcCalorimeterHit&       eemcHits(); //!
    const StPtrVecMcCalorimeterHit& eemcHits() const; //!
    StPtrVecMcPixelHit&             pixelHits(); //!
    const StPtrVecMcPixelHit&       pixelHits() const; //!
    StParticleDefinition*           particleDefinition(); //!
    const StParticleDefinition*     particleDefinition() const; //!
    int                             isShower() const; //! 1 = yes, 0 = no
    long                            geantId() const;  //!
    long                            pdgId()   const;  //!
    long                            key()     const;  //!
    long                            eventGenLabel() const; //!

  // "Set" Methods
    void setFourMomentum(const StLorentzVectorF&); //!
    void setStartVertex(StMcVertex*); //!
    void setStopVertex(StMcVertex*); //!
    void setIntermediateVertices(StPtrVecMcVertex&); //!
    void setTpcHits(StPtrVecMcTpcHit&); //!
    void setSvtHits(StPtrVecMcSvtHit&); //!
    void setFtpcHits(StPtrVecMcFtpcHit&); //!
    void setRichHits(StPtrVecMcRichHit&); //!
    void setCtbHits(StPtrVecMcCtbHit&); //!
    void setBemcHits(StPtrVecMcCalorimeterHit&); //!
    void setBprsHits(StPtrVecMcCalorimeterHit&); //!
    void setBsmdeHits(StPtrVecMcCalorimeterHit&); //!
    void setBsmdpHits(StPtrVecMcCalorimeterHit&); //!
    void setTofHits(StPtrVecMcTofHit&); //!
    void setEemcHits(StPtrVecMcCalorimeterHit&); //!
    void setPixelHits(StPtrVecMcPixelHit&); //!

    void setShower(char); //!
    void setGeantId(long); //!
    void setPdgId(long); //!
    void setKey(long);     //!
    void setEventGenLabel(long);     //!
    void setParent(StMcTrack*);     //!

    void addTpcHit(StMcTpcHit*); //!
    void addSvtHit(StMcSvtHit*); //!
    void addFtpcHit(StMcFtpcHit*); //!
    void addRichHit(StMcRichHit*); //!
    void addCtbHit(StMcCtbHit*); //!
    void addBemcHit(StMcCalorimeterHit*); //!
    void addBprsHit(StMcCalorimeterHit*); //!
    void addBsmdeHit(StMcCalorimeterHit*); //!
    void addBsmdpHit(StMcCalorimeterHit*); //!
    void addTofHit(StMcTofHit*); //!
    void addEemcHit(StMcCalorimeterHit*); //!
    void addPixelHit(StMcPixelHit*); //!
    void removeTpcHit(StMcTpcHit*); //!
    void removeSvtHit(StMcSvtHit*); //!
    void removeFtpcHit(StMcFtpcHit*); //!
    void removeRichHit(StMcRichHit*); //!
    void removeCtbHit(StMcCtbHit*); //!
    void removeCalorimeterHit(StPtrVecMcCalorimeterHit&, StMcCalorimeterHit*); //!
    void removeBemcHit(StMcCalorimeterHit*); //!
    void removeBprsHit(StMcCalorimeterHit*); //!
    void removeBsmdeHit(StMcCalorimeterHit*); //!
    void removeBsmdpHit(StMcCalorimeterHit*); //!
    void removeTofHit(StMcTofHit*); //!
    void removeEemcHit(StMcCalorimeterHit*); //!
    void removePixelHit(StMcPixelHit*); //!

    //    void setTopologyMap(StTrackTopologyMap&); //!
    
protected:
    StLorentzVectorF         mFourMomentum; //! 
    StMcVertex*              mStartVertex; //!
    StMcVertex*              mStopVertex; //!
    StPtrVecMcVertex         mIntermediateVertices; //!
    StPtrVecMcTpcHit         mTpcHits; //!
    StPtrVecMcSvtHit         mSvtHits; //!
    StPtrVecMcFtpcHit        mFtpcHits; //!
    StPtrVecMcRichHit        mRichHits; //!
    StPtrVecMcCtbHit         mCtbHits; //!
    StPtrVecMcCalorimeterHit mBemcHits; //!
    StPtrVecMcCalorimeterHit mBprsHits; //!
    StPtrVecMcCalorimeterHit mBsmdeHits; //!
    StPtrVecMcCalorimeterHit mBsmdpHits; //!
    StPtrVecMcTofHit         mTofHits; //!
    StPtrVecMcCalorimeterHit mEemcHits; //!
    StPtrVecMcPixelHit       mPixelHits; //!
    StParticleDefinition*    mParticleDefinition; //!
    StMcTrack*               mParent;
    char                     mIsShower; //!
    long                     mGeantId; //!
    long                     mPdgId; //!
    long                     mKey;     //!
    long                     mEventGenLabel; //!
    //    StTrackTopologyMap     mTopologyMap; //!
};

ostream&  operator<<(ostream& os, const StMcTrack&);

inline const StLorentzVectorF& StMcTrack::fourMomentum() const { return mFourMomentum; }

inline const StThreeVectorF& StMcTrack::momentum() const { return mFourMomentum.vect(); }

inline float StMcTrack::energy() const { return mFourMomentum.e(); }

inline float StMcTrack::pt() const { return mFourMomentum.perp(); }

inline float StMcTrack::rapidity() const { return mFourMomentum.rapidity(); }

inline float StMcTrack::pseudoRapidity() const { return mFourMomentum.pseudoRapidity(); }

inline StMcVertex* StMcTrack::startVertex() { return mStartVertex; }

inline const StMcVertex* StMcTrack::startVertex() const { return mStartVertex; }

inline StMcVertex* StMcTrack::stopVertex() { return mStopVertex; }

inline const StMcVertex* StMcTrack::stopVertex() const { return mStopVertex; }

inline StMcTrack* StMcTrack::parent() { return mParent; }

inline const StMcTrack* StMcTrack::parent() const { return mParent; }

inline StPtrVecMcVertex& StMcTrack::intermediateVertices() { return mIntermediateVertices; }

inline const StPtrVecMcVertex& StMcTrack::intermediateVertices() const { return mIntermediateVertices; }

inline StPtrVecMcTpcHit& StMcTrack::tpcHits() { return mTpcHits; }

inline const StPtrVecMcTpcHit& StMcTrack::tpcHits() const { return mTpcHits; }

inline StPtrVecMcSvtHit& StMcTrack::svtHits() { return mSvtHits; }

inline const StPtrVecMcSvtHit& StMcTrack::svtHits() const { return mSvtHits; }

inline StPtrVecMcFtpcHit& StMcTrack::ftpcHits() { return mFtpcHits; }

inline const StPtrVecMcFtpcHit& StMcTrack::ftpcHits() const { return mFtpcHits; }

inline StPtrVecMcRichHit& StMcTrack::richHits() { return mRichHits; }

inline const StPtrVecMcRichHit& StMcTrack::richHits() const { return mRichHits; }

inline StPtrVecMcCtbHit& StMcTrack::ctbHits() { return mCtbHits; }

inline const StPtrVecMcCtbHit& StMcTrack::ctbHits() const { return mCtbHits; }

inline StPtrVecMcCalorimeterHit& StMcTrack::bemcHits() { return mBemcHits; }

inline const StPtrVecMcCalorimeterHit& StMcTrack::bemcHits() const { return mBemcHits; }

inline StPtrVecMcCalorimeterHit& StMcTrack::bprsHits() { return mBprsHits; }

inline const StPtrVecMcCalorimeterHit& StMcTrack::bprsHits() const { return mBprsHits; }

inline StPtrVecMcCalorimeterHit& StMcTrack::bsmdeHits() { return mBsmdeHits; }

inline const StPtrVecMcCalorimeterHit& StMcTrack::bsmdeHits() const { return mBsmdeHits; }

inline StPtrVecMcCalorimeterHit& StMcTrack::bsmdpHits() { return mBsmdpHits; }

inline const StPtrVecMcCalorimeterHit& StMcTrack::bsmdpHits() const { return mBsmdpHits; }

inline StPtrVecMcTofHit& StMcTrack::tofHits() { return mTofHits; }

inline const StPtrVecMcTofHit& StMcTrack::tofHits() const { return mTofHits; }

inline StPtrVecMcCalorimeterHit& StMcTrack::eemcHits() { return mEemcHits; }

inline const StPtrVecMcCalorimeterHit& StMcTrack::eemcHits() const { return mEemcHits; }

inline StPtrVecMcPixelHit& StMcTrack::pixelHits() { return mPixelHits; }

inline const StPtrVecMcPixelHit& StMcTrack::pixelHits() const { return mPixelHits; }

inline StParticleDefinition* StMcTrack::particleDefinition() { return mParticleDefinition; }

inline const StParticleDefinition* StMcTrack::particleDefinition() const { return mParticleDefinition; }

inline int StMcTrack::isShower() const { return mIsShower; }

inline long StMcTrack::geantId() const { return mGeantId; }

inline long StMcTrack::pdgId() const { return mPdgId; }

inline long StMcTrack::key() const { return mKey; }

inline long StMcTrack::eventGenLabel() const { return mEventGenLabel; }

#endif




