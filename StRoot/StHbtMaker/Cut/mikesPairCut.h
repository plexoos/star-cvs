/***************************************************************************
 *
 * $Id: mikesPairCut.h,v 1.2 1999/07/06 22:33:21 lisa Exp $
 *
 * Author: Mike Lisa, Ohio State, lisa@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: StHbtMaker package
 *   a do-nothing pair cut that simply says "true" to every pair           
 *
 ***************************************************************************
 *
 * $Log: mikesPairCut.h,v $
 * Revision 1.2  1999/07/06 22:33:21  lisa
 * Adjusted all to work in pro and new - dev itself is broken
 *
 * Revision 1.1.1.1  1999/06/29 16:02:56  lisa
 * Installation of StHbtMaker
 *
 **************************************************************************/


#ifndef mikesPairCut_hh
#define mikesPairCut_hh

#ifndef StMaker_H
#include "StMaker.h"
#endif

#include "StHbtMaker/Base/StHbtPairCut.hh"

class mikesPairCut : public StHbtPairCut{
public:
  mikesPairCut();
  //~mikesPairCut();

  virtual bool Pass(const StHbtPair*);

  virtual StHbtString Report();


private:
  long mNPairsPassed;
  long mNPairsFailed;

  ClassDef(mikesPairCut, 1)

};

#endif
