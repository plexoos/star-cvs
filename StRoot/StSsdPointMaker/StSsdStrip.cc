#include "StSsdStrip.hh"

StSsdStrip::StSsdStrip(int rNStrip, int rDigitSig, float rSigma, int rPedestal, int *rIdMcHit)
{
  mIdMcHit   = new int[SSD_MAXIDMCHIT];

  mNStrip    = rNStrip;
  mDigitSig  = rDigitSig;
  mSigma     = rSigma;
  mPedestal  = rPedestal;
  for(int e=0;e<SSD_MAXIDMCHIT;e++) mIdMcHit[e] = rIdMcHit[e];
  mPrevStrip = 0;
  mNextStrip = 0;
}

StSsdStrip::StSsdStrip(int rNStrip, int rDigitSig, float rSigma, int rPedestal)
{
  mIdMcHit   = new int[SSD_MAXIDMCHIT];

  mNStrip    = rNStrip;
  mDigitSig  = rDigitSig;  
  mSigma     = rSigma;
  mPedestal  = rPedestal;
  for(int e=0;e<SSD_MAXIDMCHIT;e++) mIdMcHit[e] = 0;
  mPrevStrip = 0;
  mNextStrip = 0;
}

StSsdStrip::~StSsdStrip()
{
  delete [] mIdMcHit;
}

void StSsdStrip::setPrevStrip(StSsdStrip *rPrevStrip)
{  this->mPrevStrip = rPrevStrip; }

void StSsdStrip::setNextStrip(StSsdStrip *rNextStrip)
{  this->mNextStrip = rNextStrip; }

void StSsdStrip::setSigma(float rSigma)
{  this->mSigma = rSigma; }

void StSsdStrip::setPedestal(int iPedestal)
{  this->mPedestal = iPedestal; }

void StSsdStrip::setNStrip(int rNStrip)
{  this->mNStrip = rNStrip; }

void StSsdStrip::setDigitSig(int rDigitSig)
{  this->mDigitSig = rDigitSig; }

void StSsdStrip::setIdMcHit(int rIdMcHit, int iR)
{  this->mIdMcHit[iR] = rIdMcHit; }

int StSsdStrip::getNStrip()
{  return this->mNStrip; }

int StSsdStrip::getDigitSig()
{  return this->mDigitSig; }

StSsdStrip* StSsdStrip::getPrevStrip()
{  return this->mPrevStrip; }

StSsdStrip* StSsdStrip::getNextStrip()
{  return this->mNextStrip; }

float StSsdStrip::getSigma()
{  return this->mSigma; }

int StSsdStrip::getPedestal()
{  return this->mPedestal; }

int StSsdStrip::getIdMcHit(int iR)
{  return this->mIdMcHit[iR]; }

void StSsdStrip::copyTo(StSsdStrip *ptrClone)
{
  ptrClone->mNStrip    = this->mNStrip;
  ptrClone->mDigitSig  = this->mDigitSig;
  ptrClone->mSigma     = this->mSigma;
  ptrClone->mPedestal  = this->mPedestal;
  for(int e=0;e<SSD_MAXIDMCHIT;e++) ptrClone->mIdMcHit[e] = this->mIdMcHit[e];
}


