/***************************************************************************
 * Author: christelle roy
 * Description: SSD DB access Maker
 **************************************************************************/

#ifndef STSSDDBMAKER_H
#define STSSDDBMAKER_H

#ifndef StMaker_H
#include "StMaker.h"
#endif
#include "StDbLib/StDbDefs.hh"                         //

class StChain;
class StDbManager;
class StDbConfigNode;


class St_SsdDb_Reader;
class StSsdDbReader; 
class StSsdHybridCollection;
class StSsdGeometry;
class StSsdDbWriter;

class StSsdDbMaker : public StMaker {
 private:
  Text_t *mTimeStamp;            //!
  Int_t   mUnixTimeStamp;        //!
  St_SsdDb_Reader *m_Reader;     //!
  StSsdDbWriter *mWriter;        //!
  
  StDbManager* mDbMgr;           //!
  StDbConfigNode* mConfigGeom;   //!
  StDbConfigNode* mGeom;         //!

 protected:

 public: 
  StSsdDbMaker(const char *name="SsdDb");
  virtual       ~StSsdDbMaker();
  virtual Int_t  Init();
  virtual Int_t  InitRun(int runumber);
  virtual Int_t  Make();
  virtual Int_t  Finish();
  virtual void   Clear(const char *opt);

  void setTimeStamp(Text_t *timestamp) {mTimeStamp = timestamp;}
  void setUnixTimeStamp(Int_t timestamp) {mUnixTimeStamp = timestamp;}
  void setSsdDb_Reader();

  void setSsdDirectDb_Reader();

  void setSsdConfig();
  void readSsdConfig();
  void setSsdDimensions();
  void readSsdDimensions();

  void setSsdGeometry();
  void readSsdGeometry();
  void setSsdPedestals();
  void readSsdPedestals();

  void setSsdDbWriter(int unixTime);

  St_SsdDb_Reader* get_SsdDb_Reader(){return m_Reader;}

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StSsdDbMaker.h,v 1.4 2005/05/10 12:41:42 reinnart Exp $ built "__DATE__" "__TIME__ ; return cvs;}
  ClassDef(StSsdDbMaker,0)   //StAF chain virtual base class for Makers
};

// Global pointers:
R__EXTERN StSsdDbMaker* gStSsdDbMaker;

#endif


