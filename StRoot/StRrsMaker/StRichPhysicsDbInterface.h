/**************************************************************************
 * $Id: StRichPhysicsDbInterface.h,v 2.0 2000/08/09 16:17:03 gans Exp $
 *
 * Description:
 *
 * $Log: StRichPhysicsDbInterface.h,v $
 * Revision 2.0  2000/08/09 16:17:03  gans
 * Readded Files That were not added in last CVS. Cosmetic Changes, naming convention
 * for StRichDrawableT(foo)
 *
 * Revision 1.3  2000/02/11 21:10:54  lasiuk
 * maximum energy probability in access function
 * change electrons/cm and gas gain to 10 pwer 5
 *
 * Revision 1.2  2000/02/08 16:29:37  lasiuk
 * include gasGainAmplification factor here instead of geometry
 *
 * Revision 1.1  2000/01/25 22:02:22  lasiuk
 * Second Revision
 *
 **************************************************************************/
#ifndef ST_RICH_PHYSICS_INTERFACE_H
#define ST_RICH_PHYSICS_INTERFACE_H

#include <iostream.h>

class StRichPhysicsDbInterface {
public:
    
    virtual ~StRichPhysicsDbInterface() {}
    
    //StRichPhysicsInterface(const StRichPhysicsInterface&);
    //StRichPhysicsInterface&(const StRichPhysicsInterface&);

    virtual double version() const = 0;

    // Efficiency
    virtual double photonToPadEfficiency()       const = 0;
    virtual double photoConversionEfficiency()   const = 0;
    virtual double feedBackPhotonProbability()   const = 0;

    // Ionization
    virtual double polia() const = 0;
    virtual double averageNumberOfInteractions() const = 0;
    virtual double electronDistribution(int)     const = 0;    
    virtual double maximumElectronEnergyProbability()       const = 0;
    virtual double gasGainAmplification()        const = 0;
    
    // Electronics
    virtual int averagePedestal()                const = 0;
    virtual double adcConversion()               const = 0;	
    virtual int adcThreshold()                   const = 0;
    virtual int adcChannelWidth()                const = 0;
    virtual double electronicNoiseLevel()        const = 0;    
    //virtual double electronCharge()              const = 0;    

    virtual void   print(ostream& os = cout) const = 0;
};
#endif
