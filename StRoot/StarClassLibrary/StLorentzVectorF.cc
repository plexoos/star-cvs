/***************************************************************************
 *
 * $Id: StLorentzVectorF.cc,v 1.5 2000/11/27 17:33:49 ullrich Exp $
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
 * $Log: StLorentzVectorF.cc,v $
 * Revision 1.5  2000/11/27 17:33:49  ullrich
 * Enclosed streamer in macro __ROOT__.
 *
 * Revision 1.4  2000/09/28 02:06:10  perev
 * non automatic streamer added
 *
 * Revision 1.3  1999/10/15 21:55:10  didenko
 * fixed type of x,y,z,t T-> double
 *
 * Revision 1.2  1999/10/15 15:56:34  ullrich
 * Changed output format in operator<<, added operator>>
 *
 * Revision 1.1  1999/01/30 03:59:03  fisyak
 * Root Version of StarClassLibrary
 *
 * Revision 1.1  1999/01/23 00:29:18  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "StLorentzVectorF.hh"
#include "StLorentzVectorD.hh"

#ifdef __ROOT__
#include "TBuffer.h"
ClassImp(StLorentzVectorF)
#endif

StLorentzVectorF::StLorentzVectorF(const StLorentzVectorD &vec)
    : mThreeVector(vec.vect()), mX4(vec.t()) { /* nop */ }

StLorentzVectorF
StLorentzVectorF::boost(const StLorentzVectorF& pframe) const
{
    double mass        = abs(pframe);
    StThreeVectorD eta = (-1./mass)*pframe.vect();            // gamma*beta
    double gamma       = pframe.e()/mass;
    StThreeVectorD pl  = ((this->vect()*eta)/(eta*eta))*eta;  // longitudinal momentum
    return StLorentzVectorF(gamma*this->e() - this->vect()*eta,
			    this->vect() + (gamma-1.)*pl - this->e()*eta);
}

StLorentzVectorF
StLorentzVectorF::boost(const StLorentzVectorD& pframe) const
{
    double mass               = abs(pframe);
    StThreeVectorD eta        = (-1./mass)*pframe.vect();            // gamma*beta
    double gamma              = pframe.e()/mass;
    StThreeVectorD pl         = ((this->vect()*eta)/(eta*eta))*eta;  // longitudinal momentum
    return StLorentzVectorF(gamma*this->e() - this->vect()*eta,
			    this->vect() + (gamma-1.)*pl - this->e()*eta);
}

StLorentzVectorF&
StLorentzVectorF::operator=(const StLorentzVectorD& vec)
{
    mThreeVector = vec.vect();
    mX4 = vec.t();
    return *this;
}

int
StLorentzVectorF::operator== (const StLorentzVectorD& v) const
{
    return (mThreeVector == v.vect()) && (mX4 == v.t());
}

int
StLorentzVectorF::operator!= (const StLorentzVectorD& v) const
{
    return !(*this == v);
}

StLorentzVectorF&
StLorentzVectorF::operator+= (const StLorentzVectorD& v)
{
    mThreeVector += v.vect();
    mX4 += v.t();
    return *this;
}

StLorentzVectorF&
StLorentzVectorF::operator-= (const StLorentzVectorD& v)
{
    mThreeVector -= v.vect();
    mX4 -= v.t();
    return *this;
}

ostream& operator<< (ostream& os, const StLorentzVectorF& v)
{
    return os << v.vect() << "\t\t" << v.t();
}

istream&  operator>>(istream& is, StLorentzVectorF& v)
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
void StLorentzVectorF::Streamer(TBuffer &R__b)
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

