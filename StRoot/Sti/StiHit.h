/*! \class StiHit
  StiHit is a simple class that encapsulates a three dimensional position
  measurement.  The measurement is represented in a frame
  that is 'local' to the detector plane from which it arose.
  <p>
  It is assumed
  that all hits come from a detector that can be composed of discrete planes.
  Each plane is characterized by two values: position and refAngle.  In the
  mid-rapidity region of STAR (SVT, TPC, EMC, etc) the position corresponds to
  the magnitude of a vector pointing from the origin to the center of the plane.
  The refAngle is the azimuthal defined by the aforemention vector and the
  STAR global x-axis.  All hits store the position and refAngle of the detector
  plane from which they came.
  <p>
  Within each detector plane, the hit is characterized by two more numbers:
  y and z.  The y value corresponds to the distance along the plane (e.g.,
  padrow) w.r.t. the center of the plane.  The z value corresponds to the STAR
  global coordinate.
  <p>
  While it only takes two values (y and z) two specify the hit location within
  a plane (once the location of the plane is known) StiHit stores one more
  value: x.  This value of x corresponds to the magnitude of a vector pointing
  from the origin to the detector plane <b>perpendicular to the plane</b>.  So,
  for planes with no tilt w.r.t. the origin (e.g., TPC pad-planes), x will be
  identical to position.  Actually, this is not even quite true for tpc hits, as
  distortion corrections can make x slightly different than position.  However,
  by storing both x and position, we allow for the separation of information
  that depends only on material location (track fitting) from that which
  depends only on hit location (track finding).
  <p>
  StiHit stores information that represents a full error matrix.  For efficiency
  purposes this is stored as a collection of discreet floats instead of a
  matrix.  Because the error matrix is always symmetric, we must store only six
  values.  These are denoted by s_ij, where s_ij corresponds to the (i,j)
  component of the matrix.
  <p>
  StiHit also stores a pointer to an StHit that it corresponds to.
  Additionally, StiHit stores a pointer to the StDetector object from which its
  measurement arose.

  \author M.L. Miller (Yale Software)
 */

#ifndef StiHit_HH
#define StiHit_HH
#include "StiDetector.h"
#include "StiPlacement.h"
#include "StMeasuredPoint.h"
#include "StThreeVectorF.hh"
#include "StMatrixF.hh"
#define HITMESSENGER *(Messenger::instance(MessageType::kHitMessage))



class StiHit 
{
public: 

  enum StiHitProperty { kR,
			kZ,
			kPseudoRapidity,
			kPhi};
    ///Default constructor.
    StiHit();
    StiHit(const StiHit&);
    const StiHit& operator=(const StiHit& );
    ///Default destructor.
    ~StiHit();

    ///Return the local x value.
    float x() const;
    ///Return the local y value.
    float y() const;
    ///Return the local/global z value.
    float z() const;
    ///Return the local x value.
    float x_g() const;
    ///Return the local y value.
    float y_g() const;
    ///Return the local/global z value.
    float z_g() const;
    ///Return the (x,x) component of the error matrix.
    float sxx() const;
    ///Return the (y,y) component of the error matrix.
    float syy() const;
    ///Return the (z,z) component of the error matrix.
    float szz() const;
    ///Return the (x,y) component of the error matrix.
    float sxy() const;
    ///Return the (x,z) component of the error matrix.
    float sxz() const;
    ///Return the (y,z) component of the error matrix.
    float syz() const;
    ///Return the energy deposition associated with this point 
    float getEloss();
    ///Return the refAngle of the detector plane from which the hit arose.
    float refangle() const;
    ///Return the position of the detector plane from whcih the hit arose.
    float position() const;
    ///Return a const pointer to the StiDetector object from which the hit
    ///arose.
    const StiDetector* detector() const;

    ///Return a const pointer to the StHit object corresponding to this StiHit
    ///instance
    //const StHit* stHit() const;
    const StMeasuredPoint * stHit() const;

    ///If we are running in simulated mode, return a const pointer to the
    /// StMcHit associated with this StiHit.
    //const StMcHit* stMcHit() const;

    ///Return the number of times this hit was assigned to a track
    unsigned int timesUsed() const;
    
    ///Return a boolean that marks whether or not this hit is assigned to a
    ///track.
    //bool   isUsed() const;

    ///Return a const reference to a StThreeVectorF that denotes the position
    ///of the hit in global STAR coordinates.
    const StThreeVectorF globalPosition() const;

    void set(float position,  float angle, float y, float z);

    ///Set the local position and error in one function call
    void set(const StiDetector * detector,
	     const StMeasuredPoint * stHit,
	     float energy,
	     float x, float y, float z, 
	     float sxx=1, float sxy=1, float sxz=1, float syy=1, float syz=1, float szz=1);
    ///Set the global position and error in one function call 
    ///A transformation is performed internally from global to local coordinates
    ///according to the detector information.
    void setGlobal(const StiDetector * detector,
		   const StMeasuredPoint * stHit,
		   float x, float y, float z,
		   float energy);
    
    ///Set the position error matrix for the measurement from an StMatrixF
    ///object.
    void setError(const StMatrixF&);
    ///Scale all errors by the given factor
    void scaleError(float);
    ///Set the pointer to the StiDetector from which the hit arose.
    void setDetector(const StiDetector*det) {mdetector=det;};
    ///Set the pointer to the corresponding StHit object.
    void setStHit(const StMeasuredPoint*hit){msthit=hit;}
    ///Set the number of times used
    void setTimesUsed(unsigned int);
    void reset();
    void rotate(double angle);
    double getValue(int key) const;
    double getPseudoRapidity() const;
    friend ostream& operator<<(ostream& os, const StiHit& h);
    void Break(int i);
private:
    float mrefangle;
    float mposition;
    float mx;
    float my;
    float mz; 
    float msxx;
    float msyy;
    float mszz;
    float msxy;
    float msxz;
    float msyz;
    // global position
    float _xg,_yg,_zg;
    unsigned int mTimesUsed;
    const StiDetector* mdetector;
    const StMeasuredPoint * msthit;
    float _energy;
public:
    int mCount;
};


inline void StiHit::scaleError(float scale)
{
  msxx*=scale;
  msyy*=scale;
  mszz*=scale;
  msxy*=scale;
  msxz*=scale;
  msyz*=scale;
}
inline float StiHit::x() const {return mx;}
inline float StiHit::y() const {return my;}
inline float StiHit::z() const {return mz;}
//inline float StiHit::x_g() const {return msthit->position().x();}
//inline float StiHit::y_g() const {return msthit->position().y();}
//inline float StiHit::z_g() const {return msthit->position().z();}
inline float StiHit::x_g() const {return _xg;}
inline float StiHit::y_g() const {return _yg;}
inline float StiHit::z_g() const {return _zg;}
inline float StiHit::sxx() const {return msxx;}
inline float StiHit::syy() const {return msyy;}
inline float StiHit::szz() const {return mszz;}
inline float StiHit::sxy() const {return msxy;}
inline float StiHit::sxz() const {return msxz;}
inline float StiHit::syz() const {return msyz;}
inline float StiHit::refangle() const {return mrefangle;}
inline float StiHit::position() const {return mposition;}
inline const StMeasuredPoint* StiHit::stHit() const {return msthit;}
inline const StiDetector* StiHit::detector() const {return mdetector;}
inline unsigned int StiHit::timesUsed() const { return mTimesUsed;}


//Functors for ordering hits
struct StiHitRadiusLessThan
{
  bool operator() (const StiHit*h1, const StiHit*h2) const
  {
    double x1 = h1->x_g();
    double y1 = h1->y_g();
    double r1 = sqrt(x1*x1+y1*y1);
    double x2 = h2->x_g();
    double y2 = h2->y_g();
    double r2 = sqrt(x2*x2+y2*y2);
    return r1<r2;
  }
};

struct StidHitLessThan
{
  bool operator() (const StiHit*lhs, const StiHit*rhs) const
    {
      return (lhs->y() < rhs->y()) ? true : false;
    }
};

struct StizHitLessThan
{
  bool operator() (const StiHit*lhs, const StiHit*rhs) const
  {
    return (lhs->z() < rhs->z()) ? true : false;
  }
};

struct StiHitIsUsed
{
  bool operator() (const StiHit*hit) const
  {
    return (hit->timesUsed()==0);
  }
};


#endif
