/***************************************************************************
 *
 * $Id: StThreeVectorF.hh,v 1.13 2005/03/28 06:04:25 perev Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:  
 *
 * Remarks:   This is a 'handmade' specialisation of StThreeVector<T>
 *            for T=float. This code contains no templates.
 *
 ***************************************************************************
 *
 * $Log: StThreeVectorF.hh,v $
 * Revision 1.13  2005/03/28 06:04:25  perev
 * Defence FPE added
 *
 * Revision 1.12  2005/02/05 01:15:19  perev
 * test for zero mag added
 *
 * Revision 1.11  2004/10/26 17:33:16  perev
 * check for x,y=0 added
 *
 * Revision 1.10  2004/10/17 03:20:41  perev
 * Error check improved
 *
 * Revision 1.9  2003/10/30 20:06:47  perev
 * Check of quality added
 *
 * Revision 1.8  2003/09/02 17:59:35  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 1.7  2003/05/07 20:34:12  perev
 * functions for CINT added
 *
 * Revision 1.6  2002/06/21 17:47:37  genevb
 * Added pseudoProduct
 *
 * Revision 1.5  2000/09/25 20:23:08  ullrich
 * Removed inheritance from TObject.
 *
 * Revision 1.4  1999/12/21 15:14:33  ullrich
 * Modified to cope with new compiler version on Sun (CC5.0).
 *
 * Revision 1.3  1999/06/04 18:00:14  ullrich
 * Added new constructor which takes C-style array as argument.
 * New operators operator() and operator[] which can be used
 * as lvalues.
 *
 * Revision 1.2  1999/02/14 23:11:51  fisyak
 * Fixes for Rootcint
 *
 * Revision 1.1  1999/01/30 03:59:06  fisyak
 * Root Version of StarClassLibrary
 *
 * Revision 1.1  1999/01/23 00:28:07  ullrich
 * Initial Revision
 *
 **************************************************************************/
#ifndef ST_THREE_VECTOR_F_HH
#define ST_THREE_VECTOR_F_HH
#include <Stiostream.h>
#include <math.h>
#ifdef __ROOT__
#include "Rtypes.h"
#endif

class StThreeVectorD;

class StThreeVectorF
{
public:    
    StThreeVectorF(float = 0, float = 0, float = 0);
    virtual ~StThreeVectorF();

    StThreeVectorF(const StThreeVectorF&);
    StThreeVectorF(const StThreeVectorD&);

    StThreeVectorF(const float*);
    StThreeVectorF(const double*);
    
    StThreeVectorF& operator=(const StThreeVectorF&);
    StThreeVectorF& operator=(const StThreeVectorD&);
    
    void setX(float);
    void setY(float);
    void setZ(float);

    void setPhi(float);
    void setTheta(float);
    void setMag(float);
    void setMagnitude(float);
    
    float   x()                        const;
    float   y()                        const;
    float   z()                        const;
    float   theta()                    const;
    float   cosTheta()                 const;
    float   phi()                      const;
    float   perp()                     const;
    float   perp2()                    const;
    float   magnitude()                const;
    float   mag()                      const;
    float   mag2()                     const;
    float   pseudoRapidity()           const;
    float   operator() (size_t)        const;
    float   operator[] (size_t)        const;
    float&  operator() (size_t);
    float&  operator[] (size_t);

    float   massHypothesis(float mass) const;
    
    StThreeVectorF  unit()       const;
    StThreeVectorF  orthogonal() const;

    void  rotateX(float);
    void  rotateY(float);
    void  rotateZ(float);
    
    StThreeVectorF  operator- ();
    StThreeVectorF  operator+ ();
    StThreeVectorF& operator*= (double);
    StThreeVectorF& operator/= (double);
 
    float          angle(const StThreeVectorF&) const;
    StThreeVectorF cross(const StThreeVectorF&) const;
    float          dot  (const StThreeVectorF&) const;
    StThreeVectorF pseudoProduct(const StThreeVectorF&) const;
    StThreeVectorF pseudoProduct(double,double,double) const;
 
    float          angle(const StThreeVectorD&) const;
    float          dot  (const StThreeVectorD&) const;
    StThreeVectorF cross(const StThreeVectorD&) const;
    StThreeVectorF pseudoProduct(const StThreeVectorD&) const;
      
    int operator == (const StThreeVectorF& v) const;
    int operator != (const StThreeVectorF& v) const;
    StThreeVectorF& operator+= (const StThreeVectorF&);
    StThreeVectorF& operator-= (const StThreeVectorF&);
    
    int operator == (const StThreeVectorD& v) const;
    int operator != (const StThreeVectorD& v) const;
    StThreeVectorF& operator+= (const StThreeVectorD&);
    StThreeVectorF& operator-= (const StThreeVectorD&);
    int             valid(double world = 1.e+5) const;
    int             bad  (double world = 1.e+5) const;

protected:
    float    mX1, mX2, mX3;
#ifdef __ROOT__
    ClassDef(StThreeVectorF,2)
#endif
};
//VP#ifndef __CINT__
//
//        Declaration of non-member functions and operators
//
ostream&              operator<<(ostream& os, const StThreeVectorF& v);
istream&              operator>>(istream& is, StThreeVectorF& v);
inline float          abs(const StThreeVectorF& v);
inline StThreeVectorF cross_product(const StThreeVectorF& v1, const StThreeVectorF& v2);
inline StThreeVectorF cross_product(const StThreeVectorF& v1, const StThreeVectorD& v2);
inline StThreeVectorF operator+ (const StThreeVectorF& v1, const StThreeVectorF& v2);
inline StThreeVectorF operator+ (const StThreeVectorF& v1, const StThreeVectorD& v2);
inline StThreeVectorF operator- (const StThreeVectorF& v1, const StThreeVectorF& v2);
inline StThreeVectorF operator- (const StThreeVectorF& v1, const StThreeVectorD& v2);
inline float          operator* (const StThreeVectorF& v1, const StThreeVectorF& v2);
inline float          operator* (const StThreeVectorF& v1, const StThreeVectorD& v2);
inline StThreeVectorF operator* (const StThreeVectorF& v, double c);
inline StThreeVectorF operator* (double c, const StThreeVectorF& v);
inline StThreeVectorF operator/ (const StThreeVectorF& v, double c);                          

//
//        Implementation of inline member functions
//
inline StThreeVectorF::StThreeVectorF(float x, float y, float z)
    : mX1(x), mX2(y), mX3(z) {/* nop */}

inline StThreeVectorF::~StThreeVectorF() {/* nop */}

inline void StThreeVectorF::setX(float x) {mX1 = x;}

inline void StThreeVectorF::setY(float y) {mX2 = y;}

inline void StThreeVectorF::setZ(float z) {mX3 = z;}

inline void StThreeVectorF::setPhi(float angle)
{
    double  r = magnitude();
    double th = theta();
    
    mX1 = r*sin(th)*cos(angle);
    mX2 = r*sin(th)*sin(angle);
}

inline void StThreeVectorF::setTheta(float angle)
{
    double r  = magnitude();
    double ph = phi();

    mX1 = r*sin(angle)*cos(ph);
    mX2 = r*sin(angle)*sin(ph);
    mX3 = r*cos(angle);
}

inline void StThreeVectorF::setMagnitude(float r)
{
    double th = theta();
    double ph = phi();
    
    mX1 = r*sin(th)*cos(ph);
    mX2 = r*sin(th)*sin(ph);
    mX3 = r*cos(th);
}

inline void StThreeVectorF::setMag(float mag)
{
    setMagnitude(mag);
}

inline float StThreeVectorF::x() const {return mX1;}

inline float StThreeVectorF::y() const {return mX2;}

inline float StThreeVectorF::z() const {return mX3;}

inline float StThreeVectorF::theta() const
{
    return acos(mX3/(this->mag()+1.e-10));
}

inline float StThreeVectorF::cosTheta() const
{
    return mX3/(mag()+1.e-10);
}

inline float StThreeVectorF::phi() const
{
    if (fabs(mX1)+fabs(mX2)<=1.e-12) return 0;
    return atan2(mX2, mX1);
}

inline float StThreeVectorF::pseudoRapidity() const
{
    return -::log(tan(theta()/2.)+1e-20);
}

inline StThreeVectorF StThreeVectorF::unit() const
{
    return *this/(mag()+1.e-10);
}

inline float StThreeVectorF::massHypothesis(float mass) const
{
    return ::sqrt((*this)*(*this) + mass*mass);
}

inline StThreeVectorF StThreeVectorF::orthogonal() const
{
    // Direct copy from CLHEP--it is probably better to
    // use your own dot/cross product code...
    double x = (mX1 < 0.0) ? -mX1 : mX1;
    double y = (mX2 < 0.0) ? -mX2 : mX2;
    double z = (mX3 < 0.0) ? -mX3 : mX3;
    
    if(x<y)
	return x < z ? StThreeVectorF(0,mX3,-mX2) :  StThreeVectorF(mX2,-mX1,0);
    else
	return  mX2 < mX3 ? StThreeVectorF(-mX3,0,mX1) :  StThreeVectorF(mX2,-mX1,0);
}

inline void StThreeVectorF::rotateX(float angle)
{
    // may in the future make use of the StRotation class!
    double yPrime = cos(angle)*mX2 - sin(angle)*mX3;
    double zPrime = sin(angle)*mX2 + cos(angle)*mX3;

    mX2 = yPrime;
    mX3 = zPrime;
}

inline void StThreeVectorF::rotateY(float angle)
{
    // may in the future make use of the StRotation class!
    double zPrime = cos(angle)*mX3 - sin(angle)*mX1;
    double xPrime = sin(angle)*mX3 + cos(angle)*mX1;

    mX1 = xPrime;
    mX3 = zPrime;
}

inline void StThreeVectorF::rotateZ(float angle)
{
    // may in the future make use of the StRotation class!
    double xPrime = cos(angle)*mX1 - sin(angle)*mX2;
    double yPrime = sin(angle)*mX1 + cos(angle)*mX2;

    mX1 = xPrime;
    mX2 = yPrime;
}

inline float StThreeVectorF::perp() const
{
    return ::sqrt(mX1*mX1+mX2*mX2);
}

inline float StThreeVectorF::perp2() const
{
    return mX1*mX1+mX2*mX2;
}

inline float StThreeVectorF::magnitude() const
{
    return mag();
}

inline float StThreeVectorF::mag() const
{
    return ::sqrt(mX1*mX1+mX2*mX2+mX3*mX3);
}

inline float StThreeVectorF::mag2() const
{
    return mX1*mX1+mX2*mX2+mX3*mX3;
}

inline float StThreeVectorF::operator() (size_t i) const
{
    if (i == 0)
        return mX1;
    else if (i == 1)
        return mX2;
    else if (i == 2)
        return mX3;
    else {
	cerr << "StThreeVectorF::operator(): bad index" << endl;
	return 0;
    }
}

inline float& StThreeVectorF::operator() (size_t i)
{
    if (i == 0)
        return mX1;
    else if (i == 1)
        return mX2;
    else if (i == 2)
        return mX3;
    else {
	cerr << "StThreeVectorF::operator(): bad index" << endl;
	return mX3;
    }
}

inline float StThreeVectorF::operator[] (size_t i) const
{
    if (i == 0)
        return mX1;
    else if (i == 1)
        return mX2;
    else if (i == 2)
        return mX3;
    else {
	cerr << "StThreeVectorF::operator[]: bad index" << endl;
	return 0;
    }
}

inline float& StThreeVectorF::operator[] (size_t i)
{
    if (i == 0)
        return mX1;
    else if (i == 1)
        return mX2;
    else if (i == 2)
        return mX3;
    else {
	cerr << "StThreeVectorF::operator[]: bad index" << endl;
	return mX3;
    }
}

inline StThreeVectorF& StThreeVectorF::operator*= (double c)
{
    mX1 *= c; mX2 *= c; mX3 *= c;
    return *this;
}

inline StThreeVectorF& StThreeVectorF::operator/= (double c)
{
    mX1 /= c; mX2 /= c; mX3 /= c;
    return *this;
}

inline StThreeVectorF StThreeVectorF::operator- ()
{
    return StThreeVectorF(-mX1, -mX2, -mX3);
}

inline StThreeVectorF StThreeVectorF::operator+ ()
{
    return *this;
}

inline StThreeVectorF::StThreeVectorF(const StThreeVectorF& v)
    : mX1(v.x()), mX2(v.y()), mX3(v.z()) {/* nop */}

inline StThreeVectorF&
StThreeVectorF::operator=(const StThreeVectorF& v)
{
    mX1 = v.x();  mX2 = v.y();  mX3 = v.z();
    return *this;
}

inline int
StThreeVectorF::operator== (const StThreeVectorF& v) const
{
    return mX1 == v.x() && mX2 == v.y() && mX3 == v.z();
}

inline int
StThreeVectorF::operator!= (const StThreeVectorF& v) const
{
    return !(*this == v);
}

inline StThreeVectorF&
StThreeVectorF::operator+= (const StThreeVectorF& v)
{
    mX1 += v.x(); mX2 += v.y(); mX3 += v.z();
    return *this;
}

inline StThreeVectorF&
StThreeVectorF::operator-= (const StThreeVectorF& v)
{
    mX1 -= v.x(); mX2 -= v.y(); mX3 -= v.z();
    return *this;
}

inline float StThreeVectorF::dot(const StThreeVectorF& v) const
{
    return mX1*v.x() + mX2*v.y() + mX3*v.z();
}

inline StThreeVectorF
StThreeVectorF::cross(const StThreeVectorF& v) const
{
    return StThreeVectorF(mX2*v.z() - mX3*v.y(),
			    mX3*v.x() - mX1*v.z(),
			    mX1*v.y() - mX2*v.x());
}

inline float StThreeVectorF::angle(const StThreeVectorF& v) const
{
    return acos(this->dot(v)/(this->mag()*v.mag()+1e-10));
}

inline StThreeVectorF
StThreeVectorF::pseudoProduct(const StThreeVectorF& v) const
{
    return this->pseudoProduct(v.x(),v.y(),v.z());
}

inline StThreeVectorF
StThreeVectorF::pseudoProduct(double x,double y,double z) const
{
    return StThreeVectorF(mX1*x,mX2*y,mX3*z);
}


//
//        Inline non-member functions
//
inline float abs(const StThreeVectorF& v) {return v.mag();}

inline StThreeVectorF
cross_product(const StThreeVectorF& v1, const StThreeVectorF& v2)
{
    return v1.cross(v2);
}

inline StThreeVectorF
cross_product(const StThreeVectorF& v1, const StThreeVectorD& v2)
{
    return v1.cross(v2);
}


//
//        Inline non-member operators
//
inline StThreeVectorF
operator+ (const StThreeVectorF& v1, const StThreeVectorF& v2)
{
    return StThreeVectorF(v1) += v2;
}

inline StThreeVectorF
operator+ (const StThreeVectorF& v1, const StThreeVectorD& v2)
{
    return StThreeVectorF(v1) += v2;
}

inline StThreeVectorF
operator- (const StThreeVectorF& v1, const StThreeVectorF& v2)
{
    return StThreeVectorF(v1) -= v2;
}

inline StThreeVectorF
operator- (const StThreeVectorF& v1, const StThreeVectorD& v2)
{
    return StThreeVectorF(v1) -= v2;
}

inline float operator* (const StThreeVectorF& v1, const StThreeVectorF& v2)
{
    return StThreeVectorF(v1).dot(v2);
}

inline float operator* (const StThreeVectorF& v1, const StThreeVectorD& v2)
{
    return StThreeVectorF(v1).dot(v2);
}

inline StThreeVectorF operator* (const StThreeVectorF& v, double c)
{
    return StThreeVectorF(v) *= c;
}

inline StThreeVectorF operator* (double c, const StThreeVectorF& v)
{
    return StThreeVectorF(v) *= c;
}

inline StThreeVectorF operator/ (const StThreeVectorF& v, double c)
{
    return StThreeVectorF(v) /= c;
}
//VP#endif
#endif
