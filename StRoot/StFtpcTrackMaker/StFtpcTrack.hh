// $Id: StFtpcTrack.hh,v 1.13 2002/10/24 16:37:45 oldi Exp $
// $Log: StFtpcTrack.hh,v $
// Revision 1.13  2002/10/24 16:37:45  oldi
// dca (impact parameter) is calculated using StHelix::distance(vertexPos), now.
// Therefore it is the smallest three dimensional distance of the helix to the
// primary vertex.
// dca for primary tracks is filled correctly, now. (Even though this value
// shouldn't be of great use.)
// Code clean-ups.
//
// Revision 1.12  2002/10/11 15:45:19  oldi
// Get FTPC geometry and dimensions from database.
// No field fit activated: Returns momentum = 0 but fits a helix.
// Bug in TrackMaker fixed (events with z_vertex > outer_ftpc_radius were cut).
// QA histograms corrected (0 was supressed).
// Code cleanup (several lines of code changed due to *params -> Instance()).
// cout -> gMessMgr.
//
// Revision 1.11  2002/04/29 15:50:05  oldi
// All tracking parameters moved to StFtpcTrackingParameters.cc/hh.
// In a future version the actual values should be moved to an .idl file (the
// interface should be kept as is in StFtpcTrackingParameters.cc/hh).
//
// Revision 1.10  2002/04/05 16:50:44  oldi
// Cleanup of MomentumFit (StFtpcMomentumFit is now part of StFtpcTrack).
// Each Track inherits from StHelix, now.
// Therefore it is possible to calculate, now:
//  - residuals
//  - vertex estimations obtained by back extrapolations of FTPC tracks
// Chi2 was fixed.
// Many additional minor (and major) changes.
//
// Revision 1.9  2002/01/29 11:08:16  oldi
// Write() renamed to WriteCluster() resp. WriteTrack() to avoid compiler warnings.
// As a result the functions TObject::Write() are available again (directly).
//
// Revision 1.8  2001/01/25 15:22:02  oldi
// Review of the complete code.
// Fix of several bugs which caused memory leaks:
//  - Tracks were not allocated properly.
//  - Tracks (especially split tracks) were not deleted properly.
//  - TClonesArray seems to have a problem (it could be that I used it in a
//    wrong way). I changed all occurences to TObjArray which makes the
//    program slightly slower but much more save (in terms of memory usage).
// Speed up of HandleSplitTracks() which is now 12.5 times faster than before.
// Cleanup.
//
// Revision 1.7  2000/11/10 18:38:02  oldi
// Changes due to replacement of StThreeVector by TVector3.
// Points can be added to a track on either end now.
// New data members for dE/dx information.
// Cleanup.
//
// Revision 1.6  2000/07/18 21:22:16  oldi
// Changes due to be able to find laser tracks.
// Cleanup: - new functions in StFtpcConfMapper, StFtpcTrack, and StFtpcPoint
//            to bundle often called functions
//          - short functions inlined
//          - formulas of StFormulary made static
//          - avoid streaming of objects of unknown size
//            (removes the bunch of CINT warnings during compile time)
//          - two or three minor bugs cured
//
// Revision 1.5  2000/07/12 11:58:43  jcs
// calculate and save FTPC track parameters for unconstrained fit
//
// Revision 1.4  2000/07/03 12:42:57  jcs
// save (pre)Vertex id and unconstrained fit results
//
// Revision 1.3  2000/06/07 11:43:30  oldi
// New data members added: mRowsWithPoints, mChi2Circle, mChi2Length, mRFirst, mRLast, mAlphaFirst, mAlphaLast.
// Added the getters and setters for the new data members.
// Added GetEta().
//
// Revision 1.2  2000/05/11 15:14:51  oldi
// Changed class names *Hit.* due to already existing class StFtpcHit.cxx in StEvent
//
// Revision 1.1  2000/05/10 13:39:23  oldi
// Initial version of StFtpcTrackMaker
//

/////////////////////////////////////////////////////////////////////////////////
//                                                                             //
// StFtpcTrack class - representation of one FTPC track for the FTPC trackers. //
//                                                                             //
/////////////////////////////////////////////////////////////////////////////////

#ifndef STAR_StFtpcTrack
#define STAR_StFtpcTrack

#include "TObjArray.h"
#include "TVector3.h"

#include "StPhysicalHelix.hh"
#include "StThreeVector.hh"

#include "MIntArray.h"
#include "StFtpcPoint.hh"
#include "StFtpcVertex.hh"
#include "StFtpcTrackingParams.hh"

#include "fpt_fptrack.h"
#include "phys_constants.h"

class StFtpcPoint;
class StFtpcVertex;

class StFtpcTrack : public StPhysicalHelix, public TObject {
  
private:
  
  // data from tracker
  TObjArray *mPoints;         // Array of pointers to clusters of track
  MIntArray *mPointNumbers;   // Array of numbers of clusters
  Int_t  mRowsWithPoints;     // Binary pattern to know in which row a point is found

     Int_t   mTrackNumber;    // number of track
  Double_t   mChi2Circle;     // Chi squared of circle fit
  Double_t   mChi2Length;     // Chi squared of length fit
  Double_t   mTrackLength;    // Length of track helix from first to last point
  Double_t   mRadius;         // Radius of the helix (projected to a circle)
  Double_t   mCenterX;        // x coordinate of the center of the helix (projected to a circle)
  Double_t   mCenterY;        // y coordinate of the center of the helix (projected to a circle)
  Double_t   mAlpha0;         // angle between first point of track (may be the vertex) and x axis as seen from center of circle projetion
     Int_t   mPid;            // particle id
   Short_t   mNMax;           // number of possible hits on this track
  Double_t   mRFirst;         // radius of (virtual) trackpoint in the first (inner) pad row
  Double_t   mRLast;          // radius of (virtual) trackpoint in the last (outer) pad row
  Double_t   mAlphaFirst;     // angle of (virtual) trackpoint in the first (inner) pad row
  Double_t   mAlphaLast;      // angle of (virtual) trackpoint in the last (outer) pad row
    Bool_t   mFromMainVertex; // true if tracks origin is the main vertex, otherwise false
  
  // data from momentum fit
  TVector3   mP;              // ThreeVector of track momentum
  TVector3   mV;              // ThreeVector of vertex used in fit
     Int_t   mQ;              // charge measured in fit 
  Double_t   mChiSq[2];       // Chi2 of momentum fit
  Double_t   mTheta;          // theta value of momentum fit
  Double_t   mDca;            // radial impact parameter to main vertex

  // dE/dx information
  Double_t   mdEdx;           // Mean nergy loss per length
     Int_t   mNumdEdxHits;    // Number of hits accepted for dE/dx


public:
  
             StFtpcTrack();                                                       // constructor
             StFtpcTrack(Int_t tracknumber);                                      // constructor which fills tracknumber (all other members are set to default values)
             StFtpcTrack(fpt_fptrack_st *track_st, TObjArray *hits = 0, 
	                Int_t tracknumber = 0);                                  // constructor if STAF track is given
   virtual  ~StFtpcTrack();                                                       // destructor

      void   SetDefaults();                                                       // performs the default setup for the track
      void   AddPoint(StFtpcPoint *point);                                        // adds a point to the track
      void   AddForwardPoint(StFtpcPoint* point);                                 // adds a point after all shifting all existing points by one slot
      void   Fit();                                                               // momentum fit
      void   Fit(StFtpcVertex *vertex, Double_t max_Dca, Int_t id_start_vertex);  // momentum fit with vertex
      void   CalculateNMax();                                                     // calculates the max. possible number of points
  Double_t   CalcDca(StFtpcVertex *vertex, Bool_t primaryFit);                    // calculation of distance of closest approach (dca) to main vertex
  Double_t   CalcAlpha0();                                                        // calculation of the angle of xt with respect to the x axis
      void   CalcAndSetAlpha0() { this->SetAlpha0(this->CalcAlpha0()); }          // calculates and sets the angle of xt with respect to the x axis
      void   CalcResiduals();                                                     // calulates the residuals for each point on track
     Int_t   WriteTrack(fpt_fptrack_st *trackTableEntry, Int_t id_start_vertex);  // writes track to table

  // momentum fit
  void MomentumFit(StFtpcVertex *vertex = 0);

  StThreeVector<Double_t> helixMomentum() const;
  StThreeVector<Double_t> momentum() const;
  StThreeVector<Double_t> localMomentum(Double_t s);
               Double_t chi2Rad() const;
               Double_t chi2Lin() const;
  
  // getter
  TObjArray  *GetHits()             const { return mPoints;                          }
  MIntArray  *GetHitNumbers()       const { return mPointNumbers;                    }
  Int_t       GetRowsWithPoints()   const { return mRowsWithPoints;                  }
  Int_t       GetTrackNumber()      const { return mTrackNumber;                     }
  Double_t    GetChi2Circle()       const { return mChi2Circle;                      }
  Double_t    GetChi2Length()       const { return mChi2Length;                      }
  Double_t    GetRadius()           const { return mRadius;                          }
  Double_t    GetCenterX()          const { return mCenterX;                         }
  Double_t    GetCenterY()          const { return mCenterY;                         }
  Double_t    GetAlpha0()           const { return mAlpha0;                          }
  Double_t    GetPid()              const { return mPid;                             }
  Short_t     GetNMax()             const { return mNMax;                            }
  Double_t    GetRFirst()           const { return mRFirst;                          }
  Double_t    GetRLast()            const { return mRLast;                           }
  Double_t    GetAlphaFirst()       const { return mAlphaFirst;                      }
  Double_t    GetAlphaLast()        const { return mAlphaLast;                       }
  Int_t       GetNumberOfPoints()   const { return mPoints->GetEntriesFast();        }
  Bool_t      ComesFromMainVertex() const { return mFromMainVertex;                  }
  TVector3    GetMomentum()         const { return mP;                               }
  Double_t    GetPx()               const { return mP.X();                           }
  Double_t    GetPy()               const { return mP.Y();                           }
  Double_t    GetPz()               const { return mP.Z();                           }
  Double_t    GetP() const;
  Double_t    GetPt() const;
  Double_t    GetPseudoRapidity()   const;
  Double_t    GetEta() const; 
  Double_t    GetRapidity() const;
  Int_t       GetHemisphere() const;

  TVector3    GetVertex()           const { return mV;                               }
  Int_t       GetCharge()           const { return mQ;                               }
  Double_t const  *GetChiSq()       const { return mChiSq;                           }
  Double_t    GetTheta()            const { return mTheta;                           }
  Double_t    GetDca()              const { return mDca;                             }

  Double_t    GetdEdx() const             { return mdEdx;                            }
  Int_t       GetNumdEdxHits() const      { return mNumdEdxHits;                     }
  
  // setter   
  void   SetTrackNumber(Int_t number);
  void   SetRowsWithPoints(Int_t f)    { mRowsWithPoints = f; }
	    
  void   SetPx(Double_t f)             {          mP.SetX(f); }
  void   SetPy(Double_t f)             {          mP.SetY(f); }
  void   SetPz(Double_t f)             {          mP.SetZ(f); }

  void   SetdEdx(Double_t f)           {           mdEdx = f; }
  void   SetNumdEdxHits(Int_t f)       {    mNumdEdxHits = f; }

  void   SetChi2Circle(Double_t f)     {     mChi2Circle = f; }
  void   SetChi2Length(Double_t f)     {     mChi2Length = f; }
  void   SetRadius(Double_t f)         {         mRadius = f; }
  void   SetCenterX(Double_t f)        {        mCenterX = f; }
  void   SetCenterY(Double_t f)        {        mCenterY = f; }
  void   SetAlpha0(Double_t f)         {         mAlpha0 = f; }
  void   SetCharge(Int_t f)            {              mQ = f; }
  void   SetPid(Int_t f)               {            mPid = f; }
  void   SetRLast(Double_t f)          {          mRLast = f; }
  void   SetRFirst(Double_t f)         {         mRFirst = f; }
  void   SetAlphaLast(Double_t f)      {      mAlphaLast = f; }
  void   SetAlphaFirst(Double_t f)     {     mAlphaFirst = f; }

  void   SetDca(Double_t f)            {            mDca = f; }
  void   SetNMax(Short_t f)            {           mNMax = f; }
  void   ComesFromMainVertex(Bool_t f) { mFromMainVertex = f; }

  void   SetProperties(Bool_t fUsage, Int_t mTrackNumber);  
  void   SetPointDependencies();

protected:

  Int_t  CircleFit(Double_t x[],Double_t y[], Double_t xw[], Double_t yw[], Int_t num);
  void   LineFit(Double_t *x, Double_t *y, Double_t *z, Double_t *xw, Double_t *yw, Int_t num);
  
  StThreeVector<Double_t> mHelixMomentum;
  StThreeVector<Double_t> mFullMomentum;
  StThreeVector<Double_t> mVertex;
  Double_t              mXCenter, mYCenter, mFitRadius, mChi2Rad;
  Double_t              mArcOffset, mArcSlope, mChi2Lin;
  Double_t              mZField;
  Int_t                 mIterSteps;
  Int_t                 mVertexPointOffset;

  ClassDef(StFtpcTrack, 1)    // Ftpc track class  
};


inline Double_t StFtpcTrack::GetPt() const
{
  // Returns transverse momentum.

  return TMath::Sqrt(mP.X() * mP.X() + mP.Y() * mP.Y());
}


inline Double_t StFtpcTrack::GetP() const
{
  // Returns total momentum.

  return TMath::Sqrt(mP.X() * mP.X() + mP.Y() * mP.Y() + mP.Z() * mP.Z());
}


inline Double_t StFtpcTrack::GetPseudoRapidity() const
{
  // Returns the pseudo rapidity of the particle.

  return 0.5 * TMath::Log((GetP() + GetPz()) / (GetP() - GetPz()));  
}


inline Double_t StFtpcTrack::GetEta() const
{
  // This function returns the value of GetPseudoRapidity().
  
  return GetPseudoRapidity();
}


inline Double_t StFtpcTrack::GetRapidity() const
{
  // Returns the rapidity of the particle with the assumption that the particle is a pion (+/-).

  return 0.5 * TMath::Log((M_PION_PLUS + GetPz()) / (M_PION_PLUS - GetPz()));
}


// momentum fit functions

inline StThreeVector<Double_t> StFtpcTrack::helixMomentum() const
{
  return mHelixMomentum;
}


inline StThreeVector<Double_t> StFtpcTrack::momentum() const
{
  return mFullMomentum;
}


inline Double_t StFtpcTrack::chi2Rad() const
{
  return mChi2Rad;
}


inline Double_t StFtpcTrack::chi2Lin() const
{
  return mChi2Lin;
}


inline StThreeVector<Double_t> StFtpcTrack::localMomentum(Double_t s)
{
  return momentumAt(s, mZField);
}


#endif
