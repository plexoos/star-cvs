//////////////////////////////////////////////////////////////////////////
// 
// $Id: StFlowPicoEvent.h,v 1.5 2000/08/31 18:58:25 posk Exp $
//
// Author: Sergei Voloshin and Raimond Snellings, March 2000
//
// Description:  A persistent Flow Pico DST
// 
//////////////////////////////////////////////////////////////////////////
//
// $Log: StFlowPicoEvent.h,v $
// Revision 1.5  2000/08/31 18:58:25  posk
// For picoDST, added version number, runID, and multEta for centrality.
// Added centrality cut when reading picoDST.
// Added pt and eta selections for particles corr. wrt event plane.
//
// Revision 1.4  2000/08/09 21:38:23  snelling
// PID added
//
// Revision 1.3  2000/06/01 18:26:39  posk
// Increased precision of Track integer data members.
//
// Revision 1.2  2000/05/26 21:29:32  posk
// Protected Track data members from overflow.
//
// Revision 1.1  2000/05/23 20:09:50  voloshin
// added StFlowPicoEvent, persistent FlowEvent as plain root TTree
//
// Revision 1.5  2000/05/16 20:59:34  posk
// Voloshin's flowPicoevent.root added.
//
// 
//////////////////////////////////////////////////////////////////////////

#ifndef StFlowPicoEvent__h
#define StFlowPicoEvent__h
#include <iostream.h>
#include "TObject.h"
#include "TClonesArray.h"

//-----------------------------------------------------------

class StFlowPicoEvent : public TObject {
  
 public:

                StFlowPicoEvent();
  virtual       ~StFlowPicoEvent() { Clear(); }
  void          Clear(Option_t *option ="");
  TClonesArray* Tracks()     const { return fTracks; }
  Int_t         GetNtrack()  const { return mNtrack; }

  Int_t         Version()    const { return mVersion; }
  UInt_t        OrigMult()   const { return mOrigMult; }
  UInt_t        MultEta()    const { return mMultEta; }

  UInt_t        Centrality() const { return mCentrality; }
  Float_t       VertexX()    const { return mVertexX; }
  Float_t       VertexY()    const { return mVertexY; }
  Float_t       VertexZ()    const { return mVertexZ; }
  Int_t         EventID()    const { return mEventID; }
  Int_t         RunID()      const { return mRunID; }
  Float_t       CTB()        const { return mCTB; }
  Float_t       ZDCe()       const { return mZDCe; }
  Float_t       ZDCw()       const { return mZDCw; }

  
  void SetVersion(const Int_t ver)      { mVersion = ver; }
  void SetEventID(const Int_t id)       { mEventID = id; }
  void SetRunID(const Int_t id)         { mRunID = id; }
  void SetNtrack(const Int_t ntrk)      { mNtrack = ntrk; }
  void SetOrigMult(const UInt_t mult)   { mOrigMult = mult; }
  void SetMultEta(const UInt_t goodtracks) { mMultEta = goodtracks; }
  void SetCentrality(const UInt_t cent) { mCentrality = cent; }
  void SetVertexPos(const Float_t x, const Float_t y, const Float_t z) { 
    mVertexX=x; mVertexY=y; mVertexZ=z; }
  void SetCTB(const Float_t ctb)  {mCTB = ctb; }
  void SetZDCe(const Float_t zdce) {mZDCe = zdce; }
  void SetZDCw(const Float_t zdcw) {mZDCw = zdcw; }

 private:

  Int_t          mVersion;              // pico version
  Int_t          mNtrack;               // track number
  Int_t          mEventID;              // event ID
  Int_t          mRunID;                // run ID
  UInt_t         mOrigMult;             // number of tracks
  UInt_t         mMultEta;              // number of tracks with 
                                        // positive flag in 1.5 units of eta
  UInt_t         mCentrality;           // centrality bin
  Float_t        mVertexX;              // primary vertex position
  Float_t        mVertexY;              // primary vertex position
  Float_t        mVertexZ;              // primary vertex position
  Float_t        mCTB;                  // CTB value sum
  Float_t        mZDCe;                 // ZDC east
  Float_t        mZDCw;                 // ZDC west
  
  TClonesArray*        fTracks;
  static TClonesArray* fgTracks;
  
  ClassDef(StFlowPicoEvent,1)
};


//-----------------------------------------------------------

class StFlowPicoTrack : public TObject {

public:

            StFlowPicoTrack() { }
            StFlowPicoTrack(Float_t pt, 
                            Float_t eta, 
                            Float_t Dedx, 
                            Float_t phi,
          		    Short_t charge,
                            Float_t dca,
                            Float_t chi2,
                            Int_t   fitPts,
                            Int_t   maxPts,
                            Float_t pidPiPlus,
                            Float_t pidPiMinus,
                            Float_t pidProton,
			    Float_t pidKaonPlus,
			    Float_t pidKaonMinus,
			    Float_t pidAntiProton,
                            Float_t pidDeuteron
			    );
   virtual  ~StFlowPicoTrack() { }

   Float_t  Pt()         const { return mPt; }
   Float_t  Eta()        const { return mEta; }
   Float_t  Phi()        const { return mPhi; }
   Short_t  Charge()     const { return (Short_t)mCharge; }
   Float_t  Dca()        const { return mDca/10000.; }
   Float_t  Chi2()       const { return mChi2/10000.; }
   Int_t    FitPts()     const { return (Int_t)mFitPts; }
   Int_t    MaxPts()     const { return (Int_t)mMaxPts; }
   Float_t  PidPiPlus()  const { return mPidPiPlus/1000.; }
   Float_t  PidPiMinus() const { return mPidPiMinus/1000.; }
   Float_t  PidProton()  const { return mPidProton/1000.; }
   Float_t  PidKaonMinus()  const { return mPidKaonMinus/1000.; }
   Float_t  PidKaonPlus()   const { return mPidKaonPlus/1000.; }
   Float_t  PidAntiProton() const { return mPidAntiProton/1000.; }
   Float_t  PidDeuteron()   const { return mPidDeuteron/1000.; }
   Float_t  Dedx()          const { return mDedx; }

private:
   Float_t   mPt;
   Float_t   mEta;
   Float_t   mDedx;
   Float_t   mPhi;
   Char_t    mCharge;
   UShort_t  mDca;
   UShort_t  mChi2;
   UChar_t   mFitPts;
   UChar_t   mMaxPts;
   Short_t   mPidPiPlus;      
   Short_t   mPidPiMinus;
   Short_t   mPidProton;
   Short_t   mPidKaonPlus;
   Short_t   mPidKaonMinus;
   Short_t   mPidAntiProton;
   Short_t   mPidDeuteron;

   ClassDef(StFlowPicoTrack,1)
};

#endif







