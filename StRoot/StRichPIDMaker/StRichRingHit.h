/**********************************************************
 * $Id: StRichRingHit.h,v 2.1 2000/09/29 01:35:37 horsley Exp $
 *
 * Description:
 *   
 *  Revision 1.1  2000/04/03 19:36:09  horsley
 *  initial revision 
 **********************************************************/

#ifndef StRichRingHit_H
#define StRichRingHit_H

#include "StRichHit.h"

class StRichRingHit {

public:
  StRichRingHit(StRichHit* , double,double,double,double);  
  ~StRichRingHit();
  
  StRichHit* getHit();
  double getAngle();
  double getDist();
  double getMeanPathInRadiator();
  double getMeanPathInQuartz();

private:
  StRichHit* mHit;
  double mAngle,mDist;
  double mPathInRadiator,mPathInQuartz;
};

#endif











