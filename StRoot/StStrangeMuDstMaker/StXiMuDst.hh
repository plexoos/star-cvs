/***********************************************************************
 *
 * $Id: StXiMuDst.hh,v 3.3 2001/05/04 20:15:15 genevb Exp $
 *
 * Authors: Gene Van Buren, UCLA, 24-Mar-2000
 *          Peter G. Jones, University of Birmingham, 30-Mar-1999
 *
 ***********************************************************************
 *
 * Description: Xi (cascade) micro dst class
 *
 ***********************************************************************
 *
 * $Log: StXiMuDst.hh,v $
 * Revision 3.3  2001/05/04 20:15:15  genevb
 * Common interfaces and reorganization of components, add MC event info
 *
 * Revision 3.2  2000/08/10 01:16:25  genevb
 * Added number of dedx points
 *
 * Revision 3.1  2000/07/14 14:09:11  genevb
 * Fixed small typo
 *
 * Revision 3.0  2000/07/14 12:56:51  genevb
 * Revision 3 has event multiplicities and dedx information for vertex tracks
 *
 * Revision 2.0  2000/06/02 22:11:55  genevb
 * New version of Strangeness micro DST package
 *
 * Revision 1.3  2000/03/31 03:20:25  jones
 * Added topology map to V0/Xi; access funcs for each data member
 *
 * Revision 1.2  2000/03/29 20:52:14  genevb
 * Added StKinkMuDst, replaced arrays
 *
 * Revision 1.1  2000/03/29 03:10:08  genevb
 * Introduction of Strangeness Micro DST package
 *
 *
 ***********************************************************************/
#ifndef StXiMuDst_hh
#define StXiMuDst_hh
#include "StV0MuDst.hh"
#include "StXiI.hh"

class StXiVertex;

class StXiMuDst : public virtual StXiI, public StV0MuDst {
public:
  StXiMuDst();
  ~StXiMuDst();
  StXiMuDst(StXiVertex* x1,StV0Vertex* v1,StStrangeEvMuDst* e1);
  void    Fill(StXiVertex*,StV0Vertex*,StStrangeEvMuDst*);

  Int_t   charge() const;              // Particle charge
  Float_t decayVertexXiX() const;      // Coordinate of decay vertex
  Float_t decayVertexXiY() const;
  Float_t decayVertexXiZ() const;
  Float_t dcaXiDaughters() const;      // DCA of xi daughters at decay vertex
  Float_t dcaBachelorToPrimVertex() const; // DCA of bachelor to primary vertex
  Float_t dcaXiToPrimVertex() const;   // DCA of xi to primary vertex
  Float_t momBachelorX() const;        // Momentum components of bachelor
  Float_t momBachelorY() const;
  Float_t momBachelorZ() const;
  UShort_t keyBachelor() const;
  StTrackTopologyMap& topologyMapBachelor();

  Float_t momXiX() const;              // Momentum components of Xi/Omega
  Float_t momXiY() const;
  Float_t momXiZ() const;

  Float_t chi2Xi() const;              // Chi square of Xi
  Float_t clXi()   const;              // Confidence level of Xi
  Float_t chi2Bachelor() const;        // Chi square of bachelor
  Float_t clBachelor()   const;        // Confidence level of bachelor
  Long_t  detectorIdXi();              // Detector ID for Xi vertex
  virtual Long_t detectorIdPars();     // Detector ID for pars used in Xi finder
  Float_t dedxBachelor() const;        // dE/dX of bachelor
  UShort_t numDedxBachelor() const;    // Number of dE/dX points for bachelor

protected:
  Int_t   mCharge;                     // Written out
  Float_t mDecayVertexXiX;
  Float_t mDecayVertexXiY;
  Float_t mDecayVertexXiZ;

  Float_t mDcaXiDaughters;
  Float_t mDcaBachelorToPrimVertex;
  Float_t mDcaXiToPrimVertex;
  Float_t mMomBachelorX;
  Float_t mMomBachelorY;
  Float_t mMomBachelorZ;

  UShort_t mKeyBachelor;

  StTrackTopologyMap mTopologyMapBachelor;

  Float_t mChi2Xi;
  Float_t mClXi;
  Float_t mChi2Bachelor;
  Float_t mClBachelor;

  void    FillXi(StXiVertex*);

  Float_t mDedxBachelor;
  UShort_t mNumDedxBachelor;

  ClassDef(StXiMuDst,4)
};

inline StXiMuDst::StXiMuDst(StXiVertex* x1,StV0Vertex* v1,StStrangeEvMuDst* e1):
             StXiI(), StV0MuDst(v1,e1)
             { FillXi(x1); }

inline Int_t   StXiMuDst::charge() const
             { return mCharge; }
inline Float_t StXiMuDst::decayVertexXiX() const { return mDecayVertexXiX; }
inline Float_t StXiMuDst::decayVertexXiY() const { return mDecayVertexXiY; }
inline Float_t StXiMuDst::decayVertexXiZ() const { return mDecayVertexXiZ; }
inline Float_t StXiMuDst::dcaXiDaughters() const 
             { return mDcaXiDaughters; }
inline Float_t StXiMuDst::dcaXiToPrimVertex() const 
             { return mDcaXiToPrimVertex; }
inline Float_t StXiMuDst::dcaBachelorToPrimVertex() const 
             { return mDcaBachelorToPrimVertex; }
inline Float_t StXiMuDst::momBachelorX() const { return mMomBachelorX; }
inline Float_t StXiMuDst::momBachelorY() const { return mMomBachelorY; }
inline Float_t StXiMuDst::momBachelorZ() const { return mMomBachelorZ; }
inline Float_t StXiMuDst::momXiX() const { return mMomBachelorX + momV0X(); }
inline Float_t StXiMuDst::momXiY() const { return mMomBachelorY + momV0Y(); }
inline Float_t StXiMuDst::momXiZ() const { return mMomBachelorZ + momV0Z(); }
inline UShort_t StXiMuDst::keyBachelor() const { return mKeyBachelor; }
inline StTrackTopologyMap& StXiMuDst::topologyMapBachelor()
             { return mTopologyMapBachelor; }
inline Float_t StXiMuDst::chi2Xi() const { return mChi2Xi; }
inline Float_t StXiMuDst::clXi()   const { return mClXi; }
inline Float_t StXiMuDst::chi2Bachelor() const { return mChi2Bachelor; }
inline Float_t StXiMuDst::clBachelor()   const { return mClBachelor; }
inline Float_t StXiMuDst::dedxBachelor() const { return mDedxBachelor; }
inline UShort_t StXiMuDst::numDedxBachelor() const { return mNumDedxBachelor; }

#endif
