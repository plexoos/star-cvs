//StiHit.cxx
//M.L. Miller (Yale Software)
//04/01

//STD
#include <iostream.h>

//SCL
#include "StThreeVectorD.hh"
#include "StThreeVectorF.hh"

//StEvent
#include "StEventTypes.h"

//Sti
#include "StiHit.h"

StiHit::StiHit()
{
    reset();
}

StiHit::~StiHit()
{
}

StiHit::StiHit(const StiHit& rhs)
{
    if (*this==rhs) return;
    copyToThis(rhs);
    return;
}

StThreeVectorD StiHit::globalPosition() const
{
    return msthit->position();
}

void StiHit::setError(const StMatrixF& matrix)
{
    enum Labels {x=1, y=2, z=3};

    //Set Diagonal elements
    msxx = matrix(x,x);
    msyy = matrix(y,y);
    mszz = matrix(z,z);

    //Off Diagonal
    msxy = matrix(x,y);
    msxz = matrix(x,z);
    msyz = matrix(y,z);
    
    return;
}

ostream& operator<<(ostream& os, const StiHit& hit)
{
    return os <<hit.refangle()<<" "<<hit.position()<<"\t\t" //Key
	      <<hit.x()<<" "<<hit.y()<<" "<<hit.z()<<"\t\t" //Position
	      <<hit.sxx()<<" "<<hit.syy()<<" "<<hit.szz()<<"\t" //Diagonal Error
	      <<hit.sxy()<<" "<<hit.sxz()<<" "<<hit.syz()<<"\t" //Off-diagonal error
	      <<hit.detector(); //pointer to detector
}
