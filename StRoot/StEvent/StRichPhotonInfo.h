/***************************************************************************
 *
 * $Id: StRichPhotonInfo.h,v 2.3 2001/04/05 04:00:40 ullrich Exp $
 *
 * Author: Brian Lasiuk, Nov 2000
 ***************************************************************************
 *
 * Description: Definition of the persistent Photon Info object
 *
 ***************************************************************************
 *
 * $Log: StRichPhotonInfo.h,v $
 * Revision 2.3  2001/04/05 04:00:40  ullrich
 * Replaced all (U)Long_t by (U)Int_t and all redundant ROOT typedefs.
 *
 * Revision 2.2  2001/03/24 03:34:56  perev
 * clone() -> clone() const
 *
 * Revision 2.1  2000/11/25 11:51:01  lasiuk
 * Initial Revision
 *
 **************************************************************************/
#ifndef StRichPhotonInfo_hh
#define StRichPhotonInfo_hh

#include <iostream.h>

#include "StObject.h"

#include "StEnumerations.h"

class StRichPhotonInfo : public StObject {
public:
    StRichPhotonInfo();
    StRichPhotonInfo(double, double, double);
    
    virtual ~StRichPhotonInfo();
    //StRichPhotonInfo(const StRichPhotonInfo&){ /* nopt */ }
    //StRichPhotonInfo& operator=(const StRichPhotonInfo&){/* nopt */}

    void    setD(float);
    void    setSigma(float);
    void    setAzimuth(float);
    
    float d()       const;
    float sigma()   const;
    float azimuth() const;
    
protected:
    StObject* clone() const;

    Float_t    mD;
    Float_t    mSigma;
    Float_t    mAzimuth;
    
    ClassDef(StRichPhotonInfo,1)
};

inline void StRichPhotonInfo::setD(float d) {mD = d;}
inline void StRichPhotonInfo::setSigma(float s) {mSigma = s;}
inline void StRichPhotonInfo::setAzimuth(float psi) {mAzimuth = psi;}
    
inline float StRichPhotonInfo::d() const {return mD;}
inline float StRichPhotonInfo::sigma() const {return mSigma;}
inline float StRichPhotonInfo::azimuth() const {return mAzimuth;}

//non-members
ostream& operator<<(ostream& os, const StRichPhotonInfo& hit);

#endif
