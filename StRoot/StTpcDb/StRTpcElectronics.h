/***************************************************************************
 *
 * $Id: StRTpcElectronics.h,v 1.7 1999/12/16 22:00:53 hardtke Exp $
 *
 * Author:  David Hardtke
 ***************************************************************************
 *
 * Description: Root implementation of Tpc Electronics interface  
 *
 ***************************************************************************
 *
 * $Log: StRTpcElectronics.h,v $
 * Revision 1.7  1999/12/16 22:00:53  hardtke
 * add CVS tags
 *
 **************************************************************************/
#ifndef __STRTPCELECTRONICS__
#define __STRTPCELECTRONICS__
#include "StTpcElectronicsI.h"
#include "tables/St_tpcElectronics_Table.h"

class StRTpcElectronics : public StTpcElectronicsI {

private:
    St_tpcElectronics* mElec;

public:
    StRTpcElectronics(St_tpcElectronics* In=0){AddData(In);}
    ~StRTpcElectronics(){}
    void AddData(St_tpcElectronics* In){
     mElec = In;
    }
 
    int    numberOfTimeBins()                   const;
    double nominalGain()                        const;
    double samplingFrequency()                  const;
    double tZero()                              const;
    double adcCharge()                          const;
    double adcConversion()                      const;
    double averagePedestal()                    const;
    double shapingTime()                        const;
    double tau()                                const;

ClassDef(StRTpcElectronics,0)

};

inline int StRTpcElectronics::numberOfTimeBins() const {
   return (*mElec)[0].numberOfTimeBins;
}

inline double StRTpcElectronics::nominalGain() const {
   return (*mElec)[0].nominalGain;
}

inline double StRTpcElectronics::samplingFrequency() const {
   return (*mElec)[0].samplingFrequency;
}

inline double StRTpcElectronics::tZero() const {
   return (*mElec)[0].tZero;
}

inline double StRTpcElectronics::adcCharge() const {
   return (*mElec)[0].adcCharge;
}

inline double StRTpcElectronics::adcConversion() const {
   return (*mElec)[0].adcConversion;
}

inline double StRTpcElectronics::averagePedestal() const {
   return (*mElec)[0].averagePedestal;
}

inline double StRTpcElectronics::shapingTime() const {
   return (*mElec)[0].shapingTime;
}

inline double StRTpcElectronics::tau() const {
   return (*mElec)[0].tau;
}

#endif















