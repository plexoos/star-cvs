#include <math.h>
#include "StiPlacement.h"

StiPlacement::StiPlacement(){
  setCenterRep(0., 0., 0.);
}// StiPlacement()

void StiPlacement::setCenterRep(float refAngle_, float radius_, 
                                float orientation_){

  while(refAngle_ < 0.){ refAngle_ += 2.*M_PI; }
  while(refAngle_ >= 2.*M_PI){ refAngle_ -= 2.*M_PI; }
  centerRefAngle = refAngle_;

  if(radius_ >= 0.){ centerRadius = radius_; }
  
  while(orientation_ < -M_PI/2.){ orientation_ += M_PI; }
  while(orientation_ >= M_PI/2.){ orientation_ -= M_PI; }
  centerOrientation = orientation_;

  normalRefAngle = centerRefAngle + centerOrientation;
  while(normalRefAngle < 0.){ normalRefAngle += 2.*M_PI; }
  while(normalRefAngle >= 2.*M_PI){ normalRefAngle -= 2.*M_PI; }
  normalRadius = centerRadius*cos(centerOrientation);
  normalXoffset = centerRadius*sin(centerOrientation);

}// setCenterRep()

void StiPlacement::setNormalRep(float refAngle_, float radius_, 
                                float xOffset_){

  while(refAngle_ < 0.){ refAngle_ += 2.*M_PI; }
  while(refAngle_ >= 2.*M_PI){ refAngle_ -= 2.*M_PI; }
  normalRefAngle = refAngle_;

  if(radius_ >= 0.){ normalRadius = radius_; }
  
  normalXoffset = xOffset_;

  // the checking above makes these values within bounds, also
  centerRadius = sqrt(normalRadius*normalRadius + normalXoffset*normalXoffset);
  centerOrientation = atan(normalXoffset/normalRadius);
  centerRefAngle = normalRefAngle - centerOrientation;
  while(centerRefAngle < 0.){ centerRefAngle += 2.*M_PI; }
  while(centerRefAngle >= 2.*M_PI){ centerRefAngle -= 2.*M_PI; }

//  cout << "normal(" << normalRefAngle << ", " << normalRadius
//       << ", " << normalXoffset << ") == center(" << centerRefAngle
//       << ", " << centerRadius << ", " << centerOrientation << ")" << endl;
}// setNormalRep()
