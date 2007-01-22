#include "StEmcSimpleSimulator.h"

#include <assert.h>
#include "TMath.h"
#include "StEmcUtil/others/emcInternalDef.h"
#include "Stiostream.h"
#include "StMessMgr.h"

ClassImp(StEmcSimpleSimulator)

StEmcSimpleSimulator::StEmcSimpleSimulator(UInt_t det=1):StEmcVirtualSimulator()
{
    setControlDefault(det);
}

void StEmcSimpleSimulator::setControlDefault(UInt_t det=1)
{
    //
    // Define default value for Simple simulator
    // Value of sfCoeff see in pams/emc/inc/samplefrac_def.h
    // Also  see pams/emc/kumac/init_ems.kumac
    //
    mDetector     = det;
    mPedType      = 0;   // no pedestal
    mPedMean      = 0.0; // for sure
    mPedRMS       = 1.0;
    mKeySet       = 0;   // calib. coefficient the same for all cells, smoothing on Et !!!

    mControl.Set(1);
    controlEmcPmtSimulator_st controlW;

    controlW.mode = 1;
    controlW.pedDistribution  = 0;    // no pedestal distribution in default -- 17-0ct-2002
    switch (det)
    {
    case BEMC:
        controlW.maxAdc     = 3500;   // 12 bit (max 4096)
        controlW.maxEnergy  = 60.0;   // in GeV
        controlW.sfCoeff[0] = 14.69;
        controlW.sfCoeff[1] = -0.1022;
        controlW.sfCoeff[2] = 0.7484;
        controlW.pedMean          = 10.; // for 2001 year
        controlW.pedRMS           = 2.;
        break;
    case BPRS:
        controlW.maxAdc     = 220;    // 8 bit (max 256)
        controlW.maxEnergy  = 1.0;    // in
        controlW.sfCoeff[0] = 14.69;  // The same as for BEMC
        controlW.sfCoeff[1] = -0.1022;
        controlW.sfCoeff[2] = 0.7484;
        controlW.pedMean          = 10.; // for 2001 year
        controlW.pedRMS           = 2.;
        break;
    case BSMDE:
        controlW.maxAdc     = 900;    // 10 bit (max 1024)
        controlW.maxEnergy  = 25.0;   // in GeV
        controlW.sfCoeff[0] = 0.1185e+6;
        controlW.sfCoeff[1] = -0.3292e+5;
        controlW.sfCoeff[2] = 0.3113e+5;
        controlW.pedMean          = 40.; // for 2001 year
        controlW.pedRMS           = 4.;
        break;
    case BSMDP:
        controlW.maxAdc     = 900;    // 10 bit (max 1024)
        controlW.maxEnergy  = 25.0;   // in GeV
        controlW.sfCoeff[0] = 0.1260e+6;
        controlW.sfCoeff[1] = -0.1395e+5;
        controlW.sfCoeff[2] = 0.1971e+5;
        controlW.pedMean          = 40.; // for 2001 year
        controlW.pedRMS           = 4.;
        break;
    default:
        LOG_WARN << Form("Wrong value of #det %i", mDetector) << endm;
    }
    mControl.AddAt(&controlW,0);
    init();
}

void StEmcSimpleSimulator::setControl(controlEmcPmtSimulator_st* var)
{
    if(var)
    {
        mControl.AddAt(var,0);
        init();
    }
}

void StEmcSimpleSimulator::init()
{
    //
    // mC1 - coefficient for transition from energy to adc
    //       (reverse for calibration coefficient).
    //
    if(mControl[0].maxEnergy > 0.0 && (mControl[0].mode==0||mControl[0].mode==1))
    {
        mMode      = mControl[0].mode;
        mMaxAdc    = mControl[0].maxAdc;
        mMaxEnergy = mControl[0].maxEnergy;
        mC1        = mMaxAdc / mMaxEnergy;
        mC4        = 1;
        for(Int_t i=0; i<=2; i++)
            mSF[i] = mControl[0].sfCoeff[i];
        // 16-oct-202 - must be set in any case
        setPedestal(mControl[0].pedDistribution, mControl[0].pedMean, mControl[0].pedRMS);
    }
    else
    {
        LOG_FATAL << Form("StEmcSimpleSimulator::init() -> wrong parameter(s)") << endm;
        assert(0);
    }
}

void StEmcSimpleSimulator::setParameters(const Float_t calibCoeff, const UInt_t type, const Float_t pedMean, const Float_t pedRMS, float gainUnc)
{
    if(calibCoeff <= 1.e-10)
    {
        mKeySet    = -1;    // bad case
        return;
    }
    mKeySet    = 1; // individual calib. coefficient
    mC1        = 1./Double_t(calibCoeff);
    mC4        = gainUnc;
    mMaxEnergy = Double_t(mMaxAdc) * calibCoeff;
    setPedestal(type, pedMean, pedRMS);
}
void StEmcSimpleSimulator::setPedestal(const UInt_t type, const Float_t pedMean, const Float_t pedRMS)
{
    mPedType = type;
    mPedMean = pedMean;
    mPedRMS  = pedRMS;
}
void StEmcSimpleSimulator::print()
{
    Char_t* tit[2] = {"No transition; keep deposit energy for energy",
                      "Simple transition with sampling function"};

    if(mMode==0 || mMode==1)
    {
        LOG_INFO << Form("Simple Simulator for detector %i", mDetector) << endm;
        LOG_INFO << Form(" Mode = %1i -> %s", mMode, tit[mMode]) << endm;
    }
    switch (mKeySet)
    {
    case  0:
        LOG_INFO << Form(" == No DB, ideal calibration , smoothing on eta ==") << endm;
        LOG_INFO << Form("     Max Energy  %5.1f GeV (eta=0)", mMaxEnergy) << endm;
        break;
    case  1:
        LOG_INFO << Form(" ==        DB in action    ==") << endm;
        LOG_INFO << Form("     Max Energy  %5.1f GeV", mMaxEnergy) << endm;
        break;
    default:
		LOG_INFO << Form(" ==  Bad case : cell is bad   ==") << endm;
    }
    LOG_INFO << Form("     Max Adc     %i", mMaxAdc) << endm;
    LOG_INFO << Form("     reverse calibration coefficient %f -> %f", mC1, 1./mC1) << endm;
    LOG_INFO << Form("     sample fraction function => %10.2f  %10.2f*x + %10.2f*x*x", mSF[0], mSF[1], mSF[2]) << endm;
    switch (mPedType)
    {
    case 1:
        LOG_INFO << Form("     Pedestal distribution is GAUSS -> mean %7.2f rms %7.2f",  mPedMean, mPedRMS) << endm;
        break;
    default:
        LOG_INFO << Form(" No pedestal") << endm;
    }
}

Double_t StEmcSimpleSimulator::sampleFraction(const Double_t eta)
{
    //
    // See  pams/emc/util/samplefraction.c
    //
    Double_t x = TMath::Abs(eta);
    return mSF[0]+mSF[1]*x+mSF[2]*x*x;
}

void StEmcSimpleSimulator::checkAdc()
{
    mAdc  = (Int_t)mRadc;
    mAdc  = (mAdc>0) ? mAdc : 0;
    mAdc  = (mAdc<mMaxAdc) ? mAdc: mMaxAdc;
}

Int_t StEmcSimpleSimulator::getAdc(const Double_t de, const Double_t eta)
{
    mDe = de;
    switch (mMode)
    {
    case 0:
        mAdc = -999; // No transition; keep deposit energy for energy;
        break;
    case 1:
        mSinTheta  = getSinTheta(eta); // depend from mKeySet
        mRadc      = de*sampleFraction(eta)*mSinTheta*mC1;
        if(mPedType)
            mRadc += getPedestal(mPedType, mPedMean, mPedRMS);
        checkAdc();
        Float_t ADC = (Float_t) mAdc;
        ADC*= mC4;
        mAdc=(Int_t) ADC; // add gain uncertainty in the simulation. This is not considered when converting to energy
        break;
    }
    return mAdc;
}
Double_t StEmcSimpleSimulator::getPedestal(const Int_t type, const Double_t pedMean, const Double_t pedRMS)
{
    switch(type)
    {
    case 1:
        return mRandom.Gaus(pedMean, pedRMS);
        break;
    default:
        return 0.;
    }
}
Double_t StEmcSimpleSimulator::deductPedestal(const Int_t type, const Int_t adc, const Double_t pedMean)
{
    switch(type)
    {
    case 1:
        return ((Double_t)adc - pedMean); // could be negative
        break;
    default:
        return (Double_t)adc;             // >= 0.0
    }
}
Float_t StEmcSimpleSimulator::getEnergy()
{
    //
    // Calculate energy
    //
    static Float_t e;
    static Double_t adcTmp;


    switch (mMode)
    {
    case 0:
        e = (Float_t)mDe;
        break;
    case 1:
    case 2:
    case 3:
    case 4:
        adcTmp = deductPedestal(mPedType, mAdc, mPedMean);
        switch (mKeySet)
        {
        case  0:
        case  1:
            e = (Float_t)(adcTmp/(mC1*mSinTheta));
            break;
        default:
            e = 0.0;
        }
        break;
    }
    return e;
}

Double_t StEmcSimpleSimulator::getSinTheta(Double_t eta)
{
    // 1./cosh(eta) = sin(theta)
    static Double_t sinTheta;
    switch (mKeySet)
    {
    case  0:
        sinTheta  = 1./TMath::CosH(eta);
        break;
    case  1:
        sinTheta  = 1.; // db case
        break;
    default:
        sinTheta  = 0.;
    }
    return sinTheta;
}

//////////////////////////////////////////////////////////////////////////
//  $Id: StEmcSimpleSimulator.cxx,v 1.11 2007/01/22 19:13:40 kocolosk Exp $
//  $Log: StEmcSimpleSimulator.cxx,v $
//  Revision 1.11  2007/01/22 19:13:40  kocolosk
//  use STAR logger for all output
//
//  Revision 1.10  2005/03/21 21:36:39  suaide
//  fixed problem with chain
//
//  Revision 1.9  2004/08/06 13:24:47  suaide
//  New features added and fixed some bugs in the database
//
//  Revision 1.8  2003/09/23 15:19:48  suaide
//  fixed bugs and modifications for embedding
//
//  Revision 1.7  2003/01/23 03:09:02  jeromel
//  Include modif
//
//  Revision 1.6  2002/10/17 21:17:01  pavlinov
//  default - no pedestal for all detectors
//
//  Revision 1.5  2002/09/10 16:51:30  pavlinov
//  Discard line with mDbMaker->SetDateTime
//
//  Revision 1.4  2002/06/04 16:09:35  pavlinov
//  added option with DB(pedestal ans calibration  coefficients
//
//  Revision 1.3  2001/05/14 01:30:13  pavlinov
//  Cleanup
//
//  Revision 1.2  2001/04/25 17:24:33  perev
//  HPcorrs
//
//  Revision 1.1  2000/10/23 22:53:14  pavlinov
//  First working C++ version
//
//////////////////////////////////////////////////////////////////////////
