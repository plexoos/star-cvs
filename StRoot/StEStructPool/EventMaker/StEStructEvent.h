/**********************************************************************
 *
 * $Id: StEStructEvent.h,v 1.4 2006/02/22 22:06:06 prindle Exp $
 *
 * Author: Jeff Porter as rewrite of Ebye code by Jeff Reid
 *
 **********************************************************************
 *
 * Description:  Event quantities + list of (primary) tracks
 *
 **********************************************************************/

#ifndef _StEStructEvent
#define _StEStructEvent

#include "Stiostream.h"
#include "TObject.h"
#include "TClonesArray.h"
#include "StEStructTrackCollection.h"


class StEStructEvent : public TObject {

 private:

  Int_t mNtrack;                     // track number

  Int_t mEventID;                    // event ID
  Int_t mRunID;                      // run ID
  Int_t mEventTime;                  // event (unix) timestamp
  Float_t mVx;                       // primary vertex position
  Float_t mVy;
  Float_t mVz;
  Float_t mBField;                   // magnetic field (kilogauss as MuDst)

  //  Int_t mCTBarray[32];               // coarse CTB array

  Float_t mZDCe;                     // ZDC East
  Float_t mZDCw;                     // ZDC West

  TClonesArray *fTracks; //->

  // non-persistent data to merge old event and 2ptevent classes

  Int_t mCentrality;               //! centrality measure (depends on analysis)
  Int_t mPtCentrality;             //! centrality measure (depends on analysis)
  StEStructTrackCollection * mTrackCollectionM; //! negative charge list
  StEStructTrackCollection * mTrackCollectionP; //! positive charge list

    
 public:
  StEStructEvent();
  StEStructEvent(StEStructEvent& e);
  virtual ~StEStructEvent();


  void Clear(Option_t *option ="");

  Int_t EventID() const { return mEventID; }; 
  Int_t RunID() const { return mRunID; };
  Int_t EventTime() const { return mEventTime; }

  Int_t Centrality() const { return mCentrality; };
  Int_t PtCentrality() const { return mPtCentrality; };

  Float_t Vx() const { return mVx; }
  Float_t Vy() const { return mVy; }
  Float_t Vz() const { return mVz; }
  Float_t VertexZ() const { return mVz; } 
  Float_t BField() const { return mBField; };

  Float_t ZDCe() const { return mZDCe; }
  Float_t ZDCw() const { return mZDCw; }
  
  void AddTrack(StEStructTrack* pEStructTrack);

  void SetEventID(const Int_t id) { mEventID = id; }
  void SetRunID(const Int_t id) { mRunID = id; }

  void SetVx(const Float_t vx) { mVx = vx; }
  void SetVy(const Float_t vy) { mVy = vy; }
  void SetVz(const Float_t vz) { mVz = vz; }

  void SetVertex(const Float_t vx, const Float_t vy, const Float_t vz) {
    mVx = vx; mVy = vy; mVz = vz; }
  void SetBField(const float bfield){ mBField=bfield; };

  void SetZDCe(const Float_t zdce) { mZDCe = zdce; }
  void SetZDCw(const Float_t zdcw) { mZDCw = zdcw; }

  Int_t Ntrack() { return mNtrack; }
  TClonesArray *Tracks() { return fTracks; }

  virtual StEStructTrackCollection * TrackCollectionM() const; 
  virtual StEStructTrackCollection * TrackCollectionP() const;
  void SetCentrality(const Double_t N);

  virtual void FillChargeCollections();
  
  ClassDef(StEStructEvent,1)
};



#endif


/**********************************************************************
 *
 * $Log: StEStructEvent.h,v $
 * Revision 1.4  2006/02/22 22:06:06  prindle
 * Removed all references to multRef (?)
 *
 * Revision 1.3  2004/06/09 22:39:10  prindle
 * Expanded centrality class.
 * Call to set centrality from event reader.
 *
 *
 * CVS :nded ----------------------------------------------------------------------
 *
 * Revision 1.2  2004/02/27 02:28:04  prindle
 *
 * Small modification to StEStructCentrality in EventMaker branch.
 * Many modifications to Fluctuations branch, although that branch is not
 * stable yet.
 *
 * Revision 1.1  2003/10/15 18:20:51  porter
 * initial check in of Estruct Analysis maker codes.
 *
 *
 *********************************************************************/
