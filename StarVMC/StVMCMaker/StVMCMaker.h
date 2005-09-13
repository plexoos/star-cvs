// $Id: StVMCMaker.h,v 1.4 2005/09/13 21:34:29 fisyak Exp $

#ifndef STAR_StVMCMaker
#define STAR_StVMCMaker

/*!
 *                                                                     
 * \class  StVMCMaker
 * \author fisyak
 * \date   2005/04/22
 * \brief  virtual base class for Maker
 *
 * This commented block at the top of the header file is considered as
 * the class description to be present on the this class Web page. 
 *
 */                                                                      

#ifndef StMaker_H
#include "StMaker.h"
#endif
#include "StarVMCApplication.h"
#include "TGeant3TGeo.h"

class StVMCMaker : public StMaker {
 public: 
  StVMCMaker(const char *name="geant") : StMaker(name),fEventNo(0), fRunNo(1), fEvtHddr(0), fInputFile(""), fInitRun(0), fVolume(0) {}
  virtual       ~StVMCMaker() {}
  virtual Int_t  Init() {return StMaker::Init();}
  virtual Int_t  Make();
  virtual void   Clear(Option_t *option="");
  virtual Int_t  Finish();
  virtual Int_t  InitRun  (Int_t runumber = 0);
  virtual Int_t  FinishRun(Int_t runumber){return 0;}; 
  virtual void   SetDateTime(Int_t idat=0,Int_t itim=0);
  virtual void   SetRunNo(Int_t m ) {fRunNo = m < 1 || m >= 1000000 ? 1 : m;}
  virtual void   Skip(Int_t nskip);
  virtual void   SetInputFile(const Char_t *fileName) {fInputFile = fileName;}
  const Char_t  *InputFile() const {return fInputFile.Data();}
  static StarVMCApplication* GetStarVMCApplication() {return fgStarVMCApplication;}
  static TGeant3TGeo*        GetGeant3()             {return fgGeant3;}
  TDataSet* GetVolume() { return fVolume; }
 private:
  // Private method declaration if any
 
 protected:
  // Protected method if any
  virtual TDataSet  *FindDataSet (const char* logInput,
				  const StMaker *uppMk=0,
				  const StMaker *dowMk=0) const ;
  static StarVMCApplication* fgStarVMCApplication;
  static TGeant3TGeo*        fgGeant3;
  Int_t                      fEventNo;
  Int_t                      fRunNo;
  StEvtHddr                 *fEvtHddr;//! pointer to Event Header
  TString                    fInputFile;
  Int_t                      fInitRun;
  TDataSet*                  fVolume;   //!
 public:
  virtual const char *GetCVS() const {
    static const char cvs[]="Tag $Name:  $ $Id: StVMCMaker.h,v 1.4 2005/09/13 21:34:29 fisyak Exp $ built "__DATE__" "__TIME__ ; 
    return cvs;
  }

  ClassDef(StVMCMaker,0)   //StAF chain virtual base class for Makers
};

#endif


// $Log: StVMCMaker.h,v $
// Revision 1.4  2005/09/13 21:34:29  fisyak
// Move initialization from Init to InitRun, add conversion TGeoVolume to TVolume for StEventDisplayMaker and TofMatchers
//
// Revision 1.3  2005/08/29 22:49:31  fisyak
// add check for run no.
//
// Revision 1.2  2005/06/09 20:14:40  fisyak
// Set Run number (=1 D)
//
// Revision 1.1  2005/05/24 22:58:08  fisyak
// The first version
//
