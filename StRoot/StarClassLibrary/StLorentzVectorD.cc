/***************************************************************************
 *
 * $Id: StLorentzVectorD.cc,v 1.6 2003/05/01 19:24:31 ullrich Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 * Remarks:   This is a 'handmade' specialisation of StLorentzVector<T>
 *            for T=float. This code contains no templates.
 *
 ***************************************************************************
 *
 * $Log: StLorentzVectorD.cc,v $
 * Revision 1.6  2003/05/01 19:24:31  ullrich
 * Corrected problem in boost().
 *
 * Revision 1.5  2000/11/27 17:33:46  ullrich
 * Enclosed streamer in macro __ROOT__.
 *
 * Revision 1.4  2000/09/28 02:06:10  perev
 * non automatic streamer added
 *
 * Revision 1.3  1999/10/15 21:55:09  didenko
 * fixed type of x,y,z,t T-> double
 *
 * Revision 1.2  1999/10/15 15:56:30  ullrich
 * Changed output format in operator<<, added operator>>
 *
 * Revision 1.1  1999/01/30 03:59:02  fisyak
 * Root Version of StarClassLibrary
 *
 * Revision 1.1  1999/01/23 00:29:17  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "StLorentzVectorF.hh"
#include "StLorentzVectorD.hh"

#ifdef __ROOT__
#include "TBuffer.h"
ClassImp(StLorentzVectorD)
#endif

StLorentzVectorD::StLorentzVectorD(const StLorentzVectorF &vec)
    : mThreeVector(vec.vect()), mX4(vec.t()) { /* nop */ }

StLorentzVectorD
StLorentzVectorD::boost(const StLorentzVectorD& pframe) const
{
    double mass        = abs(pframe);
    StThreeVectorF eta = (-1./mass)*pframe.vect();            // gamma*beta
    double gamma       = fabs(pframe.e())/mass;
    StThreeVectorF pl  = ((this->vect()*eta)/(eta*eta))*eta;  // longitudinal momentum
    return StLorentzVectorD(gamma*this->e() - this->vect()*eta,
			    this->vect() + (gamma-1.)*pl - this->e()*eta);
}

StLorentzVectorD
StLorentzVectorD::boost(const StLorentzVectorF& pframe) const
{
    double mass               = abs(pframe);
    StThreeVectorF eta        = (-1./mass)*pframe.vect();            // gamma*beta
    double gamma              = fabs(pframe.e())/mass;
    StThreeVectorF pl         = ((this->vect()*eta)/(eta*eta))*eta;  // longitudinal momentum
    return StLorentzVectorD(gamma*this->e() - this->vect()*eta,
			    this->vect() + (gamma-1.)*pl - this->e()*eta);
}

StLorentzVectorD&
StLorentzVectorD::operator=(const StLorentzVectorF& vec)
{
    mThreeVector = vec.vect();
    mX4 = vec.t();
    return *this;
}

int
StLorentzVectorD::operator== (const StLorentzVectorF& v) const
{
    return (mThreeVector == v.vect()) && (mX4 == v.t());
}

int
StLorentzVectorD::operator!= (const StLorentzVectorF& v) const
{
    return !(*this == v);
}

StLorentzVectorD&
StLorentzVectorD::operator+= (const StLorentzVectorF& v)
{
    mThreeVector += v.vect();
    mX4 += v.t();
    return *this;
}

StLorentzVectorD&
StLorentzVectorD::operator-= (const StLorentzVectorF& v)
{
    mThreeVector -= v.vect();
    mX4 -= v.t();
    return *this;
}

double
operator* (const StLorentzVectorF& v1, const StLorentzVectorD& v2)
{
    return v1.t()*v2.t() - v1.vect()*v2.vect();
}

double
operator* (const StLorentzVectorD& v1, const StLorentzVectorF& v2)
{
    return v1.t()*v2.t() - v1.vect()*v2.vect();
}

ostream& operator<< (ostream& os, const StLorentzVectorD& v)
{
    return os << v.vect() << "\t\t" << v.t();
}

istream&  operator>>(istream& is, StLorentzVectorD& v)
{
    double  x, y, z, t;
    is >> x >> y >> z >> t;
    v.setX(x);
    v.setY(y);
    v.setZ(z);
    v.setT(t);
    return is;
}

#ifdef __ROOT__
void StLorentzVectorD::Streamer(TBuffer &R__b)
{
//	Stream an object of class StLorentzVectorF.
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      if (R__v==1) R__b.SetBufferOffset(R__b.Length()+10);	//skip TObject
      mThreeVector.Streamer(R__b);
      R__b >> mX4;
   } else {
      R__b.WriteVersion(Class());
      mThreeVector.Streamer(R__b);
      R__b << mX4;
   }
}
#endif

