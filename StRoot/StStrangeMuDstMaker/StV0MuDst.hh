/***********************************************************************
 *
 * $Id: StV0MuDst.hh,v 3.4 2001/05/04 20:15:15 genevb Exp $
 *
 * Authors: Gene Van Buren, UCLA, 24-Mar-2000
 *          Peter G. Jones, University of Birmingham, 04-Jun-1999
 *
 ***********************************************************************
 *
 * Description: V0 micro dst class
 *
 ***********************************************************************
 *
 * $Log: StV0MuDst.hh,v $
 * Revision 3.4  2001/05/04 20:15:15  genevb
 * Common interfaces and reorganization of components, add MC event info
 *
 * Revision 3.3  2000/08/24 20:29:00  genevb
 * Fixed a typo
 *
 * Revision 3.2  2000/08/24 20:02:59  genevb
 * Fixed a typo
 *
 * Revision 3.1  2000/08/10 01:16:24  genevb
 * Added number of dedx points
 *
 * Revision 3.0  2000/07/14 12:56:50  genevb
 * Revision 3 has event multiplicities and dedx information for vertex tracks
 *
 * Revision 2.0  2000/06/02 22:11:54  genevb
 * New version of Strangeness micro DST package
 *
 * Revision 1.4  2000/03/31 03:20:24  jones
 * Added topology map to V0/Xi; access funcs for each data member
 *
 * Revision 1.3  2000/03/29 20:52:13  genevb
 * Added StKinkMuDst, replaced arrays
 *
 * Revision 1.2  2000/03/29 14:42:40  genevb
 * Removed StV0MiniMiniDst
 *
 * Revision 1.1  2000/03/29 03:10:08  genevb
 * Introduction of Strangeness Micro DST package
 *
 *
 ***********************************************************************/
#ifndef StV0MuDst_hh
#define StV0MuDst_hh
#include "StV0I.hh"
#include "StStrangeMuDst.hh"

class StVertex;
class StV0Vertex;

class StV0MuDst : public virtual StV0I, public StStrangeMuDst {
public:
  StV0MuDst();
  ~StV0MuDst();
  StV0MuDst(StV0Vertex*,StStrangeEvMuDst*);
  void    Fill(StV0Vertex*,StStrangeEvMuDst*);

  Float_t decayVertexV0X() const;      // Coordinates of decay vertex
  Float_t decayVertexV0Y() const;
  Float_t decayVertexV0Z() const;
  Float_t dcaV0Daughters() const;      // DCA of v0 daughters at decay vertex
  Float_t dcaV0ToPrimVertex()  const;  // DCA of v0 to primary vertex
  Float_t dcaPosToPrimVertex() const;  // DCA of pos v0 daughter to pri vertex
  Float_t dcaNegToPrimVertex() const;  // DCA of neg v0 daughter to pri vertex
  Float_t momPosX() const;             // Momentum components of pos. daughter
  Float_t momPosY() const;
  Float_t momPosZ() const;
  Float_t momNegX() const;             // Momentum components of neg. daughter
  Float_t momNegY() const;
  Float_t momNegZ() const;
  StTrackTopologyMap& topologyMapPos();
  StTrackTopologyMap& topologyMapNeg();
  UShort_t keyPos() const;             // Track id v0 daughters
  UShort_t keyNeg() const;             // Track id v0 daughters

  Float_t momV0X() const;         // Momentum components of V0
  Float_t momV0Y() const;
  Float_t momV0Z() const;

  Float_t chi2V0()  const;        // Chi square of V0
  Float_t clV0()    const;        // Confidence level of V0
  Float_t chi2Pos() const;        // Chi square of pos. daughter
  Float_t clPos()   const;        // Confidence level of pos. daughter
  Float_t chi2Neg() const;        // Chi square of neg. daughter
  Float_t clNeg()   const;        // Confidence level of neg. daughter
  Long_t  detectorIdV0();         // Detector ID for V0 Vertex
  virtual Long_t detectorIdPars();// Detector ID for pars used in V0 finder
  Float_t dedxPos() const;        // dE/dX of pos. daughter
  Float_t dedxNeg() const;        // dE/dX of neg. daughter
  UShort_t numDedxPos() const;    // Number of dE/dX points for pos. daughter
  UShort_t numDedxNeg() const;    // Number of dE/dX points for neg. daughter

protected:

  Float_t mDecayVertexV0X;        // These are written out
  Float_t mDecayVertexV0Y;
  Float_t mDecayVertexV0Z;
  Float_t mDcaV0Daughters;
  Float_t mDcaV0ToPrimVertex;
  Float_t mDcaPosToPrimVertex;
  Float_t mDcaNegToPrimVertex;
  Float_t mMomPosX;
  Float_t mMomPosY;
  Float_t mMomPosZ;
  Float_t mMomNegX;
  Float_t mMomNegY;
  Float_t mMomNegZ;

  UShort_t  mKeyPos;
  UShort_t  mKeyNeg;

  StTrackTopologyMap mTopologyMapPos;
  StTrackTopologyMap mTopologyMapNeg;

  Float_t mChi2V0;
  Float_t mClV0;
  Float_t mChi2Pos;
  Float_t mClPos;
  Float_t mChi2Neg;
  Float_t mClNeg;

  Long_t detectorIdTrack(StTrackTopologyMap&);

  Float_t mDedxPos;
  Float_t mDedxNeg;
  UShort_t mNumDedxPos;
  UShort_t mNumDedxNeg;

  ClassDef(StV0MuDst,4)
};

inline StV0MuDst::StV0MuDst(StV0Vertex* v1,StStrangeEvMuDst* e1) : StV0I()
             { Fill(v1,e1); }
inline Float_t StV0MuDst::decayVertexV0X() const { return mDecayVertexV0X; }
inline Float_t StV0MuDst::decayVertexV0Y() const { return mDecayVertexV0Y; }
inline Float_t StV0MuDst::decayVertexV0Z() const { return mDecayVertexV0Z; }
inline Float_t StV0MuDst::dcaV0Daughters() const 
             { return mDcaV0Daughters; }
inline Float_t StV0MuDst::dcaV0ToPrimVertex() const 
             { return mDcaV0ToPrimVertex; }
inline Float_t StV0MuDst::dcaPosToPrimVertex() const 
             { return mDcaPosToPrimVertex; }
inline Float_t StV0MuDst::dcaNegToPrimVertex() const 
             { return mDcaNegToPrimVertex; }
inline Float_t StV0MuDst::momPosX() const { return mMomPosX; }
inline Float_t StV0MuDst::momPosY() const { return mMomPosY; }
inline Float_t StV0MuDst::momPosZ() const { return mMomPosZ; }
inline Float_t StV0MuDst::momNegX() const { return mMomNegX; }
inline Float_t StV0MuDst::momNegY() const { return mMomNegY; }
inline Float_t StV0MuDst::momNegZ() const { return mMomNegZ; }
inline StTrackTopologyMap& StV0MuDst::topologyMapPos()
             { return mTopologyMapPos; }
inline StTrackTopologyMap& StV0MuDst::topologyMapNeg()
             { return mTopologyMapNeg; }
inline UShort_t StV0MuDst::keyPos() const { return mKeyPos; } 
inline UShort_t StV0MuDst::keyNeg() const { return mKeyNeg; } 
inline Float_t StV0MuDst::momV0X()  const { return mMomPosX + mMomNegX; }
inline Float_t StV0MuDst::momV0Y()  const { return mMomPosY + mMomNegY; }
inline Float_t StV0MuDst::momV0Z()  const { return mMomPosZ + mMomNegZ; }
inline Float_t StV0MuDst::chi2V0()  const { return mChi2V0; }
inline Float_t StV0MuDst::clV0()    const { return mClV0; }
inline Float_t StV0MuDst::chi2Pos() const { return mChi2Pos; }
inline Float_t StV0MuDst::clPos()   const { return mClPos; }
inline Float_t StV0MuDst::chi2Neg() const { return mChi2Neg; }
inline Float_t StV0MuDst::clNeg()   const { return mClNeg; }
inline Float_t StV0MuDst::dedxPos() const { return mDedxPos; }
inline Float_t StV0MuDst::dedxNeg() const { return mDedxNeg; }
inline UShort_t StV0MuDst::numDedxPos() const { return mNumDedxPos; }
inline UShort_t StV0MuDst::numDedxNeg() const { return mNumDedxNeg; }
#endif
