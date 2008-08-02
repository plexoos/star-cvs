// -*- mode: c++;-*-
// $Id: StJetScratch.h,v 1.6 2008/08/02 22:43:05 tai Exp $
#ifndef STJETSCRATCH_H
#define STJETSCRATCH_H

#include "StMaker.h"
#include <Rtypes.h>

class TDirectory;
class TTree;

class StjTrgWriter;

class StMuDstMaker;

namespace StSpinJet {

}

class StJetScratch : public StMaker {

public:

  StJetScratch(const Char_t *name, TDirectory* file, StMuDstMaker* uDstMaker);
  virtual ~StJetScratch() { }

  Int_t Init();
  Int_t Make();
  Int_t Finish();
    
  const char* GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StJetScratch.h,v 1.6 2008/08/02 22:43:05 tai Exp $ built "__DATE__" "__TIME__; return cvs;}

private:

  TDirectory* _file;

  StMuDstMaker* _uDstMaker;


  ClassDef(StJetScratch, 0)

};

#endif // STJETSCRATCH_H
