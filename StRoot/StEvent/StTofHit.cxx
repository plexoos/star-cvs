/***************************************************************************
 *
 * $Id: StTofHit.cxx,v 2.3 2003/05/21 18:22:46 ullrich Exp $
 *
 * Author: Wei-Ming Zhang, Dec 2000
 ***************************************************************************
 *
 * Description:
 *
 ***************************************************************************
 *
 * $Log: StTofHit.cxx,v $
 * Revision 2.3  2003/05/21 18:22:46  ullrich
 * Major Revision of ToF classes (F. Geurts)
 *
 * Revision 2.2  2001/03/24 03:34:59  perev
 * clone() -> clone() const
 *
 * Revision 2.1  2000/12/21 23:52:24  ullrich
 * Initial Revision.
 *
 **************************************************************************/
#include "StTofHit.h"
#include "StTrack.h"
#include "StParticleDefinition.hh"

ClassImp(StTofHit)

StTofHit::StTofHit()
{
    mTrayIndex = 0;
    mModuleIndex = 0;
    mDaqIndex = 255; 
    mTimeOfFlight = 0;
    mPathLength = 0;
    mBeta = 0;
    mTOFExpectedAsElectron = 0;
    mTOFExpectedAsPion = 0; 
    mTOFExpectedAsKaon = 0;
    mTOFExpectedAsProton = 0; 
    mSigmaElectron = 999.; 
    mSigmaPion = 999.; 
    mSigmaKaon = 999.; 
    mSigmaProton = 999.;
    
    mAssociatedTrack = 0;
    mParticleHypothesis = 0;
}

StTofHit::~StTofHit() {/* noop */}

int StTofHit::trayIndex() const { return mTrayIndex; }

int StTofHit::moduleIndex() const { return mModuleIndex; }

int StTofHit::cellIndex() const { return mCellIndex; }

int StTofHit::daqIndex() const { return mDaqIndex; }

float StTofHit::timeOfFlight() const { return mTimeOfFlight; }

float StTofHit::pathLength() const { return mPathLength; }

float StTofHit::beta() const { return mBeta; }

StTrack* StTofHit::associatedTrack() { return mAssociatedTrack; }

const StTrack* StTofHit::associatedTrack() const { return mAssociatedTrack; }

float StTofHit::tofExpectedAsElectron() const { return mTOFExpectedAsElectron; }

float StTofHit::tofExpectedAsPion() const { return mTOFExpectedAsPion; }

float StTofHit::tofExpectedAsKaon() const { return mTOFExpectedAsKaon; }

float StTofHit::tofExpectedAsProton() const { return mTOFExpectedAsProton; }

float StTofHit::sigmaElectron() const { return mSigmaElectron; }

float StTofHit::sigmaPion() const { return mSigmaPion; }

float StTofHit::sigmaKaon() const { return mSigmaKaon; }

float StTofHit::sigmaProton() const { return mSigmaProton; }

StParticleDefinition* StTofHit::particleHypothesis() { return mParticleHypothesis; }

const StParticleDefinition* StTofHit::particleHypothesis() const { return mParticleHypothesis; }

void StTofHit::setTrayIndex(int trayId) { mTrayIndex = trayId; }

void StTofHit::setModuleIndex(int moduleId) { mModuleIndex = moduleId; }

void StTofHit::setCellIndex(int cellId) { mCellIndex = cellId; }

void StTofHit::setDaqIndex(int daqId) { mDaqIndex = daqId; }

void StTofHit::setTimeOfFlight(float tof) { mTimeOfFlight = tof; }

void StTofHit::setPathLength(float length) { mPathLength = length; }

void StTofHit::setBeta(float b) { mBeta = b; }

void StTofHit::setAssociatedTrack(StTrack* val)
{
    mAssociatedTrack = val;
}

void StTofHit::settofExpectedAsElectron(float tofexp) { mTOFExpectedAsElectron = tofexp; }

void StTofHit::settofExpectedAsPion(float tofexp) { mTOFExpectedAsPion = tofexp; }

void StTofHit::settofExpectedAsKaon(float tofexp) { mTOFExpectedAsKaon = tofexp; }

void StTofHit::settofExpectedAsProton(float tofexp) { mTOFExpectedAsProton = tofexp; }

void StTofHit::setsigmaElectron(float sigma) { mSigmaElectron = sigma; }

void StTofHit::setsigmaPion(float sigma) { mSigmaPion = sigma; }

void StTofHit::setsigmaKaon(float sigma) { mSigmaKaon = sigma; }

void StTofHit::setsigmaProton(float sigma) { mSigmaProton = sigma; }

void StTofHit::setparticleHypothesis(StParticleDefinition* val)
{
    mParticleHypothesis = val;
}
