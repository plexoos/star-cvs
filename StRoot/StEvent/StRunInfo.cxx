/***************************************************************************
 *
 * $Id: StRunInfo.cxx,v 2.1 2001/09/18 00:14:17 ullrich Exp $
 *
 * Author: Thomas Ullrich, Sep 2001
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StRunInfo.cxx,v $
 * Revision 2.1  2001/09/18 00:14:17  ullrich
 * Initial Revision.
 *
 **************************************************************************/
#include "StRunInfo.h"

static const char rcsid[] = "$Id: StRunInfo.cxx,v 2.1 2001/09/18 00:14:17 ullrich Exp $";

ClassImp(StRunInfo)

StRunInfo::StRunInfo()
{
    mRunId = 0;
    mProductionTime = 0;
    mCenterOfMassEnergy = 0;
    mBeamMassNumber[east] = 0;
    mBeamMassNumber[west] = 0;
    mBeamCharge[east] = 0;
    mBeamCharge[west] = 0;
    mMagneticFieldZ = 0;
    mTpcDriftVelocity[east] = 0;
    mTpcDriftVelocity[west] = 0;
}

StRunInfo::~StRunInfo() {/* noop */}

int
StRunInfo::runId() const
{return mRunId;}

time_t
StRunInfo::productionTime() const
{return mProductionTime;}

TString
StRunInfo::productionVersion() const
{return mProductionVersion;}

double
StRunInfo::centerOfMassEnergy() const
{return mCenterOfMassEnergy;}

int
StRunInfo::beamMassNumber(StBeamDirection dir) const
{return mBeamMassNumber[dir];}

int
StRunInfo::beamCharge(StBeamDirection dir) const
{return mBeamCharge[dir];}

double
StRunInfo::magneticField() const
{return mMagneticFieldZ;}

double
StRunInfo::tpcDriftVelocity(StBeamDirection dir) const
{return mTpcDriftVelocity[dir];}

void
StRunInfo::setRunId(int val) {mRunId = val;}

void
StRunInfo::setProductionTime(time_t val)
{mProductionTime = val;}               

void
StRunInfo::setProductionVersion(const char* val)
{mProductionVersion = TString(val);}            

void
StRunInfo::setCenterOfMassEnergy(double val)
{mCenterOfMassEnergy = val;}            

void
StRunInfo::setBeamMassNumber(StBeamDirection dir, int val)
{mBeamMassNumber[dir] = val;}

void
StRunInfo::setBeamCharge(StBeamDirection dir, int val)
{mBeamCharge[dir] = val;}    

void
StRunInfo::setMagneticField(double val)
{mMagneticFieldZ = val;}                

void
StRunInfo::setTpcDriftVelocity(StBeamDirection dir, double val)
{mTpcDriftVelocity[dir] = val;}  
