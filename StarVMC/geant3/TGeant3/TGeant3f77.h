#ifndef ROOT_TGeant3f77
#define ROOT_TGeant3f77 
/* $Id: TGeant3f77.h,v 1.1.1.1 2005/05/25 22:36:36 fisyak Exp $ */

//////////////////////////////////////////////////// 
//  C++/f77 interface to Geant3 basic routines    // 
////////////////////////////////////////////////////

#include "TGeant3.h" 

class TGeant3f77 : public TGeant3 { 

public: 
  TGeant3f77(); 
  TGeant3f77(const char *title, Int_t nwgeant=0); 
  virtual ~TGeant3f77() {}
 

private:
  TGeant3f77(const TGeant3f77 &) {}
  TGeant3f77 & operator=(const TGeant3f77&) {return *this;}
  
  ClassDef(TGeant3f77,1)  //C++/f77 interface to Geant basic routines 
}; 
#endif //ROOT_TGeant3f77
