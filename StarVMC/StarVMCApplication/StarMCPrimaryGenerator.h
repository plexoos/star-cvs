// $Id: StarMCPrimaryGenerator.h,v 1.3 2005/06/09 20:13:47 fisyak Exp $
// $Log: StarMCPrimaryGenerator.h,v $
// Revision 1.3  2005/06/09 20:13:47  fisyak
// It looks like that all hits in place (calorimeters have to be check for volumeid)
//
// Revision 1.2  2005/05/03 15:42:14  fisyak
// Adjust for bfc
//
// Revision 1.1  2005/04/25 20:44:28  fisyak
// StarVMCApplication with example in macros/starVMC.C
//

#ifndef Star_PRIMARY_GENERATOR_H
#define Star_PRIMARY_GENERATOR_H
#include <assert.h>
#include <stdio.h> 
#include "Stiostream.h"
#include "TString.h"
#include "TMath.h"
#include "TVector3.h"
#include "StarMCStack.h"
#include "TRandom.h"
#include "TPDGCode.h"
#include "TDatabasePDG.h"

class StarMCPrimaryGenerator : public TObject {
 public:
  StarMCPrimaryGenerator(TVirtualMCStack* stack = 0) : TObject(), fStack(stack), fIsRandom(false), fNofPrimaries(0), 
				   fOption(""), fDebug(0), fId(0), fOrigin() {fgInstance = this;}
  virtual ~StarMCPrimaryGenerator() {}
  static StarMCPrimaryGenerator* Instance() {return fgInstance;}
  void  SetIsRandom(Bool_t isRandomGenerator) { fIsRandom = isRandomGenerator; }
  void  SetNofPrimaries(Int_t nofPrimaries)   { fNofPrimaries = nofPrimaries; }
  void  SetStack(TVirtualMCStack *stack)      { fStack = stack;}
  void  SetOption(const Char_t *opt)          { fOption = opt;}
  void  SetDebug(Int_t m)                     { fDebug = m;}
  void  SetOrigin(Double_t x, Double_t y, Double_t z) {fOrigin = TVector3(x,y,z);}
  void  SetOrigin(const TVector3 &xyz)        { fOrigin = xyz;}
  Int_t GetNofPrimaries()                     { return fNofPrimaries;}
  const Option_t* GetOption() const           { return fOption.Data();}
  TVirtualMCStack *GetStack()                 { return fStack;}
  Int_t Debug()                               { return fDebug;}
  TVector3 &GetOrigin()                       { return fOrigin;}
  virtual void GeneratePrimaries() {}
  virtual void GeneratePrimaries(const TVector3& origin) {}

 protected:

  static StarMCPrimaryGenerator *fgInstance;
  TVirtualMCStack  *fStack;    
  Bool_t            fIsRandom;
  Int_t             fNofPrimaries;
  TString           fOption;  
  Int_t             fDebug;
  Int_t             fId;
  TVector3          fOrigin;

  ClassDef(StarMCPrimaryGenerator,1)  //StarMCPrimaryGenerator
};
#endif //Star_PRIMARY_GENERATOR_H

