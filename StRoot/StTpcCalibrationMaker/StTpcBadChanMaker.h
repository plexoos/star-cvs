// $Id: StTpcBadChanMaker.h,v 1.4 2003/09/10 19:47:39 perev Exp $
// $Log: StTpcBadChanMaker.h,v $
// Revision 1.4  2003/09/10 19:47:39  perev
// ansi corrs
//
// Revision 1.3  2000/08/04 21:03:53  perev
// Leaks + Clear() cleanup
//
// Revision 1.2  1999/10/11 08:04:09  fretiere
// Fix bugg + add README, LOG and ID
//
///////////////////////////////////////////////////////////////////////////////
#ifndef STAR_StTpcBadChanMaker
#define STAR_StTpcBadChanMaker

#ifndef StMaker_H
#include "StMaker.h"
#endif
// Local
class StTpcCalibSetup;
class StTpcCalibSector;

class StTpcBadChanMaker : public StMaker {
private:

  const StTpcCalibSetup* mSetup; //!
  StTpcCalibSector** mTpcCalibSector; //!

  void CalcElectronicConvertor(int** aPadToFeeConvertor, 
			       int** aPadToRDOConvertor);
protected:
public: 
  StTpcBadChanMaker(const char *aMakerName,
		 const StTpcCalibSetup* aSetup);
		 //		 const StTpcDb* aTpcDb);
  virtual       ~StTpcBadChanMaker();
  virtual Int_t  Init();
  virtual Int_t  Make();
  virtual Int_t  Finish();
  virtual void   PrintInfo();
 ClassDef(StTpcBadChanMaker,0)   //StAF chain virtual base class for Makers
};

#endif
