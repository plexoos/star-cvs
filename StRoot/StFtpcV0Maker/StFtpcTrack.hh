///////////////////////////////////////////
//  This is the track class.
//
// Writen by Mike Heffner 10/30/98
///////////////////////////////////////////

#ifndef ST_FTPC_TRACK
#define ST_FTPC_TRACK

#include <iostream>

#include "StPhysicalHelix.hh"
#include "StThreeVector.hh"
#include "StLorentzVector.hh"
#include "StGlobals.hh"
#include "V0PhysicalConstants.hh"

class StFtpcTrack: public StPhysicalHelix
{
 protected:
  StDouble mB;

 public:
  StFtpcTrack(StThreeVector<double> momentum,
	      StThreeVector<double> origin,
	      double magField, double charge);

  StFtpcTrack();
 ~StFtpcTrack();

  double GetMagField() const {return mB;}

  void display() const;

};


#endif  //ST_FTPC_TRACK
