/********************************************************
 * $Id: StRichAnalogToDigitalConverter.cxx,v 2.2 2003/09/02 17:58:55 perev Exp $
 *
 * Description:
 *  StRichAnalogToDigitalConverter takes an analog signal
 *  and converts it to an ADC count, depending on the
 *  given adc factor, pedestal and adc range. 
 *
 *  In this implementation, 
 *    all the computations are done in femtocoulombs. 
 *
 ******************************************************
 * $Log: StRichAnalogToDigitalConverter.cxx,v $
 * Revision 2.2  2003/09/02 17:58:55  perev
 * gcc 3.2 updates + WarnOff
 *
 * Revision 2.1  2002/04/01 21:55:22  lasiuk
 * pedestal convolution
 *
 * Revision 2.0  2000/08/09 16:16:59  gans
 * Readded Files That were not added in last CVS.
 *  Cosmetic Changes, naming convention
 * for StRichDrawableT(foo)
 *
 * Revision 1.5  2000/05/17 22:26:24  lasiuk
 * charge is float (cast to avoid warnings)
 *
 * Revision 1.4  2000/03/12 23:56:33  lasiuk
 * new coordinate system
 * exchange MyRound with inline templated funtion
 *
 * Revision 1.3  2000/02/08 16:22:19  lasiuk
 * use dbs
 * systemOfUnits now used
 *
 * Revision 1.2  2000/01/25 22:02:19  lasiuk
 * Second Revision
 *
 * Revision 1.1  2000/01/18 21:32:00  lasiuk
 * Initial Revision
 *
 ******************************************************/

#include <algorithm>

#include "SystemOfUnits.h"

#ifndef ST_NO_NAMESPACES
using std::min;
using std::max;
using namespace units;
#endif

#ifndef ST_NO_NAMESPACES
//namespace StRichRawData {
#endif  

#include "StRichAnalogToDigitalConverter.h"
#include "StRichPhysicsDb.h"
#include "StRichGeometryDb.h"
#include "StRichOtherAlgorithms.h"

StRichAnalogToDigitalConverter::StRichAnalogToDigitalConverter()
    : mAddPedestal(0)
{
    mPhysicsDb      = StRichPhysicsDb::getDb();
    //mMaxADC     = static_cast<int>(::pow(2.,mPhysicsDb->channel_width));
    mMaxADC         = static_cast<int>(::pow(2.,mPhysicsDb->adcChannelWidth()));
    mPedestal       = mPhysicsDb->averagePedestal();
    mAdcConversion  = mPhysicsDb->adcConversion();// fC/ADC_channels

    //mElectronCharge = e_SI*1.0e15;// #electrons -> fC conversion
}

StRichAnalogToDigitalConverter::~StRichAnalogToDigitalConverter() { /* nopt */ }
    
int StRichAnalogToDigitalConverter::operator()(double signal) const
{
    //    
    // signal is in [electrons], e_charge in [fC] and adc_factor in [fC/channel]

  double pedestal = mPedestal + mGenerator.Gauss(0,2);
  //    cout << "signal  (ADC) " << (signal/mAdcConversion) << endl;
  //    cout << "pedestal(ADC) " << pedestal << endl;
  
  double qMeasured = signal+pedestal*mAdcConversion;

  int Q = static_cast<int>( floor(qMeasured/mAdcConversion) );
  //    cout << "Q(ADC)=" << Q << endl;

  int threshold = Q-(mPedestal+6);
  //    cout << "threshold=" << threshold << endl;

  if(Q>=mMaxADC) {
    Q = mMaxADC;
  }
  else if(threshold<0) {
    Q = 0;
  }
  else {
    Q = threshold;
  }

  //    cout << "Qf= " << Q << endl;


  //
  // old code
  //
//        int Q = static_cast<int>(floor( signal  /  mAdcConversion ));
//      if(mAddPedestal)
//  	Q += mPedestal;
    
//      // check underflow or overflow ( saturation )
	
//        Q=min(max(0,Q),mMaxADC);
    
    return Q;
}

#ifndef ST_NO_NAMESPACES
//}
#endif
