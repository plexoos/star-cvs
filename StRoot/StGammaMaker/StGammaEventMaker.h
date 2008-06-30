#ifndef __StGammaEventMaker_h__
#define __StGammaEventMaker_h__

/**!
 *
 * \class StGammaEventMaker
 * 
 * First maker in the chain for gamma analysis.  Responsible for createing
 * StGammaEvent and filling with run number, event number and vertex.  Other
 * makers in the chain will get pointers to StGammaEvent and fill with other
 * information.  StGammaTreeMaker is responsible for writing the event class
 * to a ttree.
 *
 */

#include <StMaker.h>

class StGammaEvent;

class StGammaTower;
class StGammaStrip;

class StGammaPythiaEvent;
class StGammaPythiaEventMaker;

class StMuDstMaker;

#include "StEEmcUtil/EEmcGeom/EEmcGeomDefs.h"

class StGammaEventMaker : public StMaker
{

 public:
  StGammaEventMaker( const Char_t *name="gemaker" );
  ~StGammaEventMaker(){ /* nada */ }

  Int_t Init();
  Int_t Make();
  void  Clear( Option_t *opts= "" );
    
  StGammaEvent *event(){ return mGammaEvent; }

  virtual const char* GetCVS() const
    {static const char cvs[]="Tag $Name:  $ $Id: StGammaEventMaker.h,v 1.5 2008/06/30 14:58:39 jwebb Exp $ built "__DATE__" "__TIME__; return cvs;}


 private:
 protected:

  StGammaEvent *mGammaEvent;
  StGammaPythiaEvent* mPythia;
  StGammaPythiaEventMaker* mPythiaMaker;
  StMuDstMaker *muDstMaker;

  ClassDef(StGammaEventMaker,1);
};

#endif
