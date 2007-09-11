// $Id: StEmcSimpleSimulator.cxx,v 1.12 2007/09/11 21:49:13 kocolosk Exp $

#include "StEmcSimpleSimulator.h"

#include "TMath.h"

#include "StMessMgr.h"
#include "StEvent/StEnumerations.h"
#include "StEvent/StEmcRawHit.h"
#include "StMcEvent/StMcCalorimeterHit.hh"
#include "StEmcUtil/geometry/StEmcGeom.h"
#include "StEmcUtil/database/StBemcTables.h"

ClassImp(StEmcSimpleSimulator)

StEmcSimpleSimulator::StEmcSimpleSimulator(StDetectorId det, StEmcSimulatorMode mode):StEmcVirtualSimulator() {
    mDetectorId = det;
    mMode = mode;
    
    // set some default values depending on the type of detector being simulated
    switch(mDetectorId) {
        case kBarrelEmcTowerId:
            mMaxADC = 4095;     
            mSF[0]  = 14.69;
            mSF[1]  = -0.1022;
            mSF[2]  = 0.7484;
            break;
        
        case kBarrelEmcPreShowerId:
            mMaxADC = 1023;
            mSF[0]  = 559.7;
            mSF[1]  = -109.9;
            mSF[2]  = -97.81;
            break;
            
        case kBarrelSmdEtaStripId:
            mMaxADC = 1023;
            mSF[0]  = 118500.0;
            mSF[1]  = -32920.0;
            mSF[2]  = 31130.0;
            break;
        
        case kBarrelSmdPhiStripId:
            mMaxADC = 1023;
            mSF[0]  = 126000.0;
            mSF[1]  = -13950.0;
            mSF[2]  = 19710.0;
            break;
            
        default:
            LOG_ERROR << det << " is not an OK value for detector (see StEnumerations.h).  BEMC simulations will be JUNK!" << endm;
    }
    
    mGeom = StEmcGeom::instance(det - 8);

    mEmbeddingMode = false;

    mTables = NULL;    
    
    mCalibScale = 1.0;
    mCalibSpread = 0.0;
}

StEmcRawHit* StEmcSimpleSimulator::makeRawHit(const StMcCalorimeterHit *mcHit) {
    // remember the problem with negative sub -- let's be careful
    if(mcHit->module() <= 0 || mcHit->eta() <= 0 || mcHit->sub() <= 0) { 
        LOG_ERROR << "These quantities must all be positive: module=" << mcHit->module() 
                  << " eta=" << mcHit->eta() << " sub=" << mcHit->sub() << endm;
        return NULL;
    }
    
    StEmcRawHit *rawHit = new StEmcRawHit(mDetectorId, mcHit->module(), mcHit->eta(), mcHit->sub(), 0);
    
    float pseudoRapidity; mGeom->getEta(mcHit->module(), mcHit->eta(), pseudoRapidity);
    int softId; mGeom->getId(mcHit->module(), mcHit->eta(), mcHit->sub(), softId);
    
    switch(mMode) {
        case kTestMode: {
            rawHit->setEnergy(mcHit->dE() * samplingFraction(pseudoRapidity));

            LOG_DEBUG << Form("det=%2d  softId=%5d  dE=%e  sF=%.1f  ADC=%4d  ped=%6.2f  rms=%4.2f  energy=%.4f",
                mDetectorId, softId, mcHit->dE(), samplingFraction(pseudoRapidity), rawHit->adc(), 
                0.0, 0.0, rawHit->energy()) << endm;
                
            return rawHit;
        }
            
        case kSimpleMode: {
            float calib = mTables->calib(mDetectorId-8, softId);    
            
            double ADC = mcHit->dE() * samplingFraction(pseudoRapidity) / calib;
            
            // add pedestal noise unless we're doing embedding (when noise is already in the data)
            float pedMean(0.0), pedRMS(0.0);
            if(!mEmbeddingMode) {
                pedMean = mTables->pedestal(mDetectorId-8, softId);
                pedRMS  = mTables->pedestalRMS(mDetectorId-8, softId);
                ADC += mRandom.Gaus(pedMean, pedRMS);
            }
            
            // finally smear with any specified calibration jitter
            ADC *= mRandom.Gaus(mCalibScale, mCalibSpread);
            
            // check for a valid ADC range
            if(ADC < 0)         ADC = 0.0;
            if(ADC > mMaxADC)   ADC = mMaxADC;
            
            rawHit->setAdc(static_cast<unsigned int>(ADC));
            
            float energy = (rawHit->adc() - pedMean) * calib;
            rawHit->setEnergy(energy);
            
            LOG_DEBUG << Form("det=%2d  softId=%5d  dE=%e  sF=%.1f  ADC=%4d  ped=%6.2f  rms=%4.2f  energy=%.4f",
                mDetectorId, softId, mcHit->dE(), samplingFraction(pseudoRapidity), rawHit->adc(), 
                pedMean, pedRMS, rawHit->energy()) << endm;
                
            return rawHit;
        }
            
        default: { 
            LOG_ERROR << "StEmcSimpleSimulator is not configured for " << mMode << " so raw hit is NULL" << endm;
            delete rawHit;
            return NULL;
        }
    }
}

double StEmcSimpleSimulator::samplingFraction(double eta) {
    Double_t x = TMath::Abs(eta);
    return mSF[0]+mSF[1]*x+mSF[2]*x*x;
}

/*****************************************************************************
 *  $Log: StEmcSimpleSimulator.cxx,v $
 *  Revision 1.12  2007/09/11 21:49:13  kocolosk
 *  complete overhaul of the BEMC simulator
 *  http://www.star.bnl.gov/HyperNews-star/get/emc2/2486.html
 *
 *  Revision 1.11  2007/01/22 19:13:40  kocolosk
 *  use STAR logger for all output
 *
 *  Revision 1.10  2005/03/21 21:36:39  suaide
 *  fixed problem with chain
 *
 *  Revision 1.9  2004/08/06 13:24:47  suaide
 *  New features added and fixed some bugs in the database
 *
 *  Revision 1.8  2003/09/23 15:19:48  suaide
 *  fixed bugs and modifications for embedding
 *
 *  Revision 1.7  2003/01/23 03:09:02  jeromel
 *  Include modif
 *
 *  Revision 1.6  2002/10/17 21:17:01  pavlinov
 *  default - no pedestal for all detectors
 *
 *  Revision 1.5  2002/09/10 16:51:30  pavlinov
 *  Discard line with mDbMaker->SetDateTime
 *
 *  Revision 1.4  2002/06/04 16:09:35  pavlinov
 *  added option with DB(pedestal ans calibration  coefficients
 *
 *  Revision 1.3  2001/05/14 01:30:13  pavlinov
 *  Cleanup
 *
 *  Revision 1.2  2001/04/25 17:24:33  perev
 *  HPcorrs
 *
 *  Revision 1.1  2000/10/23 22:53:14  pavlinov
 *  First working C++ version
 *****************************************************************************/
