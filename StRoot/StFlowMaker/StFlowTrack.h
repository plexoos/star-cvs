//////////////////////////////////////////////////////////////////////
//
// $Id: StFlowTrack.h,v 1.13 2000/09/16 22:20:35 snelling Exp $
//
// Author: Raimond Snellings and Art Poskanzer
//
// Description: part of StFlowTrackCollection
//
//////////////////////////////////////////////////////////////////////
//
// $Log: StFlowTrack.h,v $
// Revision 1.13  2000/09/16 22:20:35  snelling
// Added selection on P and global DCA and fixed rapidity calulation
//
// Revision 1.12  2000/09/15 22:51:35  posk
// Added pt weighting for event plane calcualtion.
//
// Revision 1.11  2000/09/15 01:20:04  snelling
// Added methods for P and Y and added selection on Y
//
// Revision 1.10  2000/09/05 17:57:13  snelling
// Solaris needs math.h for fabs
//
// Revision 1.9  2000/09/05 16:11:39  snelling
// Added global DCA, electron and positron
//
// Revision 1.8  2000/08/09 21:38:23  snelling
// PID added
//
// Revision 1.7  2000/06/01 18:26:41  posk
// Increased precision of Track integer data members.
//
// Revision 1.6  2000/05/26 21:29:34  posk
// Protected Track data members from overflow.
//
// Revision 1.5  2000/05/20 00:55:20  posk
// Condensed flownanoevent.root somewhat.
//
// Revision 1.4  2000/05/16 20:59:35  posk
// Voloshin's flownanoevent.root added.
//
// Revision 1.3  2000/05/12 22:42:05  snelling
// Additions for persistency and minor fix
//
// Revision 1.1  2000/03/02 23:02:57  posk
// Changed extensions from .hh and .cc to .h and .cxx .
//
// Revision 1.10  2000/02/29 22:00:56  posk
// Made SetPhiWeight inline, changed ImpactPar to Dca, etc.
//
// Revision 1.9  2000/02/18 22:49:57  posk
// Added PID and centrality.
//
// Revision 1.7  1999/12/21 01:11:02  posk
// Added more quantities to StFlowEvent.
//
// Revision 1.6  1999/12/16 18:05:25  posk
// Fixed Linux compatability again.
//
// Revision 1.5  1999/12/15 22:01:29  posk
// Added StFlowConstants.hh
//
// Revision 1.4  1999/12/04 00:10:35  posk
// Works with the new StEvent
//
// Revision 1.3  1999/11/30 18:52:55  snelling
// First modification for the new StEvent
//
// Revision 1.2  1999/11/24 18:17:16  posk
// Put the methods which act on the data in with the data in StFlowEvent.
//
// Revision 1.1  1999/11/11 23:08:58  posk
// Rearrangement of files.
//
// Revision 1.1  1999/11/04 19:02:08  snelling
// First check in of StFlowMaker. It contains the common code from
// StFlowTagMaker and StFlowAnalysisMaker.
//
//////////////////////////////////////////////////////////////////////

#ifndef StFlowTrack_h
#define StFlowTrack_h
#include <string.h>
#include <math.h>
#include "Rtypes.h"
#include "StObject.h"
#include "StFlowConstants.h"

class StFlowTrack : public StObject {

public:

                StFlowTrack();
  virtual       ~StFlowTrack();

  Float_t       PidPiPlus()       const;
  Float_t       PidPiMinus()      const;
  Float_t       PidProton()       const;
  Float_t       PidKaonMinus()    const;
  Float_t       PidKaonPlus()     const;
  Float_t       PidAntiProton()   const;
  Float_t       PidDeuteron()     const;
  Float_t       PidAntiDeuteron() const;
  Float_t       PidElectron()     const;
  Float_t       PidPositron()     const;

  const Char_t* Pid()        const;
  Float_t       Phi()        const;
  Float_t       Eta()        const;
  Float_t       Dedx()       const;
  Float_t       Pt()         const;
  Float_t       P()          const;
  Float_t       Y()          const;
  Short_t       Charge()     const;
  Float_t       Dca()        const;
  Float_t       DcaGlobal()  const;
  Float_t       Chi2()       const;
  Int_t         FitPts()     const;
  Int_t         MaxPts()     const;
  Int_t Select(Int_t harmonic, Int_t selection, Int_t subevent= -1) const;

  void SetPidPiPlus(Float_t);
  void SetPidPiMinus(Float_t);
  void SetPidProton(Float_t);
  void SetPidKaonMinus(Float_t);
  void SetPidKaonPlus(Float_t);
  void SetPidAntiProton(Float_t);
  void SetPidDeuteron(Float_t);
  void SetPidAntiDeuteron(Float_t);
  void SetPidElectron(Float_t);
  void SetPidPositron(Float_t);
  void SetPid(const Char_t*);
  void SetPhi(Float_t);
  void SetEta(Float_t);
  void SetDedx(Float_t);
  void SetPt(Float_t);
  void SetCharge(Short_t);
  void SetDca(Float_t);
  void SetDcaGlobal(Float_t);
  void SetChi2(Float_t);
  void SetFitPts(Int_t);
  void SetMaxPts(Int_t);
  void SetSelect(Int_t harmonic, Int_t selection);
  void SetSubevent(Int_t harmonic, Int_t selection, Int_t subevent);

private:

  Int_t   mPidPiPlus;
  Int_t   mPidPiMinus;
  Int_t   mPidProton;
  Int_t   mPidKaonPlus;
  Int_t   mPidKaonMinus;
  Int_t   mPidAntiProton;
  Int_t   mPidDeuteron;
  Int_t   mPidAntiDeuteron;
  Int_t   mPidElectron;
  Int_t   mPidPositron;
  Char_t  mPid[10];
  Float_t mPhi;
  Float_t mEta;
  Float_t mDedx;
  Float_t mPt;
  Short_t mCharge;
  Float_t mDca;
  Float_t mDcaGlobal;
  Float_t mChi2;
  Int_t   mFitPts;
  Int_t   mMaxPts;
  Int_t   mSelection;
  Short_t mSubevent[Flow::nHars][Flow::nSels];
  static  Float_t maxInt;

  ClassDef(StFlowTrack, 1)                     // macro for rootcint
};

inline Float_t  StFlowTrack::PidPiPlus()   const { return mPidPiPlus/1000.; }
inline Float_t  StFlowTrack::PidPiMinus()  const { return mPidPiMinus/1000.; }
inline Float_t  StFlowTrack::PidProton()   const { return mPidProton/1000.; }
inline Float_t  StFlowTrack::PidKaonMinus() const { return mPidKaonMinus/1000.; }
inline Float_t  StFlowTrack::PidKaonPlus() const { return mPidKaonPlus/1000.; }
inline Float_t  StFlowTrack::PidAntiProton() const { return mPidAntiProton/1000.; }
inline Float_t  StFlowTrack::PidDeuteron() const { return mPidDeuteron/1000.; }
inline Float_t  StFlowTrack::PidAntiDeuteron() const { return mPidAntiDeuteron/1000.; }
inline Float_t  StFlowTrack::PidElectron() const { return mPidElectron/1000.; }
inline Float_t  StFlowTrack::PidPositron() const { return mPidPositron/1000.; }
inline const Char_t* StFlowTrack::Pid()    const { return mPid; }
inline Float_t  StFlowTrack::Phi()         const { return mPhi; }   
inline Float_t  StFlowTrack::Eta()         const { return mEta; }     
inline Float_t  StFlowTrack::Dedx()        const { return mDedx; }     
inline Float_t  StFlowTrack::Pt()          const { return mPt; }                
inline Short_t  StFlowTrack::Charge()      const { return mCharge; }   
inline Float_t  StFlowTrack::Dca()         const { return mDca; }
inline Float_t  StFlowTrack::DcaGlobal()   const { return mDcaGlobal; }
inline Float_t  StFlowTrack::Chi2()        const { return mChi2; } 
inline Int_t    StFlowTrack::FitPts()      const { return mFitPts; }  
inline Int_t    StFlowTrack::MaxPts()      const { return mMaxPts; }  

inline Float_t StFlowTrack::P()            const { 
  float momentum = mPt/sqrt(1-(tanh(mEta)*tanh(mEta)));
  return momentum; }

inline Float_t StFlowTrack::Y()            const { 
  float M = 0.139; 
  if (strcmp(mPid, "none") == 0)     M = 0.139;
  if (strcmp(mPid, "pi+") == 0)      M = 0.139;
  if (strcmp(mPid, "pi-") == 0)      M = 0.139;
  if (strcmp(mPid, "proton") == 0)   M = 0.938;
  if (strcmp(mPid, "pbar") == 0)     M = 0.938;
  if (strcmp(mPid, "k+") == 0)       M = 0.494;
  if (strcmp(mPid, "k-") == 0)       M = 0.494;
  if (strcmp(mPid, "d") == 0)        M = 1.876;
  if (strcmp(mPid, "dbar") == 0)     M = 1.876;
  if (strcmp(mPid, "e-") == 0)       M = 0.0005;
  if (strcmp(mPid, "e+") == 0)       M = 0.0005;
  float Pz = sqrt(this->P()*this->P() - mPt*mPt); 
  if (mEta < 0) {Pz = -Pz;}
  float E = sqrt(this->P()*this->P() + M*M);
  float rapidity = 0.5*log((E + Pz)/(E - Pz));
  return rapidity; }

inline Int_t StFlowTrack::Select(Int_t harmonic, Int_t selection,
 Int_t subevent) const {
  if (subevent == -1 || subevent == mSubevent[harmonic][selection]) {
    int bitShift = harmonic + Flow::nHars * selection;
    return (mSelection & (1 << bitShift)) ? kTRUE : kFALSE;
  }
  return kFALSE;         
}

inline void StFlowTrack::SetPidPiPlus(Float_t pid)  {
  if (fabs(pid) > maxInt) pid = maxInt; mPidPiPlus = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPidPiMinus(Float_t pid) {
  if (fabs(pid) > maxInt) pid = maxInt; mPidPiMinus = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPidProton(Float_t pid)  {
  if (fabs(pid) > maxInt) pid = maxInt; mPidProton = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPidKaonMinus(Float_t pid)  {
  if (fabs(pid) > maxInt) pid = maxInt; mPidKaonMinus = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPidKaonPlus(Float_t pid)  {
  if (fabs(pid) > maxInt) pid = maxInt; mPidKaonPlus = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPidAntiProton(Float_t pid)  {
  if (fabs(pid) > maxInt) pid = maxInt; mPidAntiProton = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPidDeuteron(Float_t pid)  {
  if (fabs(pid) > maxInt) pid = maxInt; mPidDeuteron = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPidAntiDeuteron(Float_t pid)  {
  if (fabs(pid) > maxInt) pid = maxInt; mPidAntiDeuteron = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPidElectron(Float_t pid)  {
  if (fabs(pid) > maxInt) pid = maxInt; mPidElectron = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPidPositron(Float_t pid)  {
  if (fabs(pid) > maxInt) pid = maxInt; mPidPositron = (Int_t)(pid*1000.); }

inline void StFlowTrack::SetPid(const Char_t* pid)  { strncpy(mPid, pid, 9);
                                                         mPid[9] = '\0'; }
inline void StFlowTrack::SetPhi(Float_t phi)        { mPhi = phi; }      

inline void StFlowTrack::SetEta(Float_t eta)        { mEta = eta; }       

inline void StFlowTrack::SetDedx(Float_t dedx)      { mDedx = dedx; }       

inline void StFlowTrack::SetPt(Float_t pt)          { mPt = pt; }              

inline void StFlowTrack::SetCharge(Short_t charge)  { mCharge = charge; }     

inline void StFlowTrack::SetDca(Float_t dca)        { mDca = dca; }

inline void StFlowTrack::SetDcaGlobal(Float_t gdca) { mDcaGlobal = gdca; }

inline void StFlowTrack::SetChi2(Float_t chi2)      { mChi2 = chi2; }

inline void StFlowTrack::SetFitPts(Int_t fitPts)    { mFitPts = fitPts; }

inline void StFlowTrack::SetMaxPts(Int_t maxPts)    { mMaxPts = maxPts; }

inline void StFlowTrack::SetSelect(Int_t harmonic, Int_t selection) {
  int bitShift = harmonic + Flow::nHars * selection;
  mSelection |= 1 << bitShift; }

inline void StFlowTrack::SetSubevent(Int_t harmonic, Int_t selection,
 Int_t subevent) { mSubevent[harmonic][selection] = subevent; }

#endif

