/***************************************************************************
 *
 * $Id: StRunInfo.h,v 2.1 2001/09/18 00:14:17 ullrich Exp $
 *
 * Author: Thomas Ullrich, Sep 2001
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StRunInfo.h,v $
 * Revision 2.1  2001/09/18 00:14:17  ullrich
 * Initial Revision.
 *
 **************************************************************************/
#ifndef StRunInfo_hh
#define StRunInfo_hh

#include <ctime>
#include "StObject.h"
#include "TString.h"
#include "StEnumerations.h"

class StRunInfo : public StObject {
public:
    StRunInfo();
    // StRunInfo(const StRunInfo&);            use default
    // StRunInfo& operator=(const StRunInfo&); use default
    virtual ~StRunInfo();

    int      runId() const;
    time_t   productionTime() const;
    TString  productionVersion() const;
    double   centerOfMassEnergy() const;
    int      beamMassNumber(StBeamDirection) const;
    int      beamCharge(StBeamDirection) const;
    double   magneticField() const;
    double   tpcDriftVelocity(StBeamDirection) const;

    void     setRunId(int);
    void     setProductionTime(time_t);                 
    void     setProductionVersion(const char*);              
    void     setCenterOfMassEnergy(double);             
    void     setBeamMassNumber(StBeamDirection, int);  
    void     setBeamCharge(StBeamDirection, int);      
    void     setMagneticField(double);                  
    void     setTpcDriftVelocity(StBeamDirection, double);
    
protected:
    Int_t	mRunId;
    
    UInt_t      mProductionTime;
    TString     mProductionVersion;
    
    Float_t     mCenterOfMassEnergy;
    Int_t       mBeamMassNumber[2];
    Int_t       mBeamCharge[2];
    
    Double_t    mMagneticFieldZ;
    Float_t     mTpcDriftVelocity[2];
    
    ClassDef(StRunInfo,1)
};
#endif
