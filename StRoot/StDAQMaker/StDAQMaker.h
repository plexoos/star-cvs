/*!
 * \class  StDAQMaker 
 * \author Victor Perev and Herb Ward
 *
 */
#ifndef STAR_StDAQMaker
#define STAR_StDAQMaker

#ifndef StMaker_H
#include "StMaker.h"
#endif
#include "StIOInterFace.h"

//	Forward declarations
class StDAQReader;
class TObjectSet;

class StDAQMaker : public StIOInterFace {
 private:
  StEvtHddr 	*fEvtHddr;	//! pointer to Event Header
  StDAQReader 	*fDAQReader;	//! pointer to StDAQReader
  TObjectSet 	*fDAQReaderSet;	//! pointer to StDAQReader DataSet

 public:
  StDAQMaker(const char *name="DAQInput",const char *inputFile=0);
  virtual       ~StDAQMaker();
  virtual Int_t  Init();
  virtual Int_t  Open(const char *file=0);
  virtual void   Close(Option_t *opt=0);
  virtual Int_t  Make();
  virtual Int_t  MakeRead(){return Make();};
  virtual Int_t  MakeRead(const StUKey &){return MakeRead();}
  virtual Int_t  Skip(Int_t Nskip=1);
          void   Clear(const char *opt="");
  ///	for compatability with StIOInterFace
  void SetBranch(const Char_t*,const Char_t*,const Char_t*, const Option_t*){};

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StDAQMaker.h,v 1.10 2004/08/07 02:39:00 perev Exp $ built "__DATE__" "__TIME__ ; return cvs;}

  ClassDef(StDAQMaker, 0)   //
};

#endif
