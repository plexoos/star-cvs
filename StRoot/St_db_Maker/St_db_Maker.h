// $Id: St_db_Maker.h,v 1.32 2008/04/02 20:22:33 perev Exp $
// $Log: St_db_Maker.h,v $
// Revision 1.32  2008/04/02 20:22:33  perev
// WarnOff
//
// Revision 1.31  2008/01/09 20:44:52  perev
// Improve printout in Finish()
//
// Revision 1.30  2007/12/29 01:43:24  perev
// More dbStat
//
// Revision 1.29  2007/09/10 02:20:10  perev
// StDbBroker::Release used
//
// Revision 1.28  2007/07/02 19:36:40  fisyak
// Add parameter currenTime (== requested time) in FindLeft
//
// Revision 1.27  2007/03/09 20:01:03  perev
// Request by user defined time now allowed
//
// Revision 1.26  2006/08/15 21:42:21  jeromel
// Fix rhic -> rhic.bnl.gov
//
// Revision 1.25  2005/07/20 17:41:44  perev
// Cleanup
//
// Revision 1.24  2004/08/18 20:33:56  perev
// Timers added for MySQL and maker itself
//
// Revision 1.23  2004/07/23 17:06:58  perev
// AliasDate & AliasTime moved fro db maker to StMaker
//
// Revision 1.22  2004/04/08 00:28:53  perev
// AliasDate & AliasTime now static methods
//
// Revision 1.21  2002/12/20 03:28:37  perev
// Save method improved
//
// Revision 1.20  2002/11/26 02:24:45  perev
// new ROOT adoptation
//
// Revision 1.19  2001/10/27 21:48:32  perev
// SetRunNumber added
//
// Revision 1.18  2001/10/13 20:23:25  perev
// SetFlavor  working before and after Init()
//
// Revision 1.17  2001/09/26 23:24:05  perev
// SetFlavor for table added
//
// Revision 1.16  2001/04/13 21:21:34  perev
// small fix (fine) + cons's
//
// Revision 1.15  2000/07/14 02:39:21  perev
// SetMaxEntryTime method added
//
// Revision 1.14  2000/06/26 20:58:41  perev
// multiple DBs
//
// Revision 1.13  2000/05/20 01:00:43  perev
// SetFlavor() added
//
// Revision 1.12  2000/04/13 02:58:47  perev
// Method Save is added & default CintDB loaded if exists
//
// Revision 1.11  2000/03/23 14:55:56  fine
// Adjusted to libSTAR and ROOT 2.24
//
// Revision 1.10  2000/02/26 01:45:04  fine
// CVS id has been introduced
//
//
#ifndef STAR_St_db_Maker
#define STAR_St_db_Maker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_db_Maker virtual base class for Maker                          //
//                                                                      //
// This class is C++ implementation of the Begin_html <a href="http://www.rhic.bnl.gov/afs/rhic.bnl.gov/star/doc/www/packages_l/pro/pams/db/sdb/doc/index.html">Simple Database Manager</a> End_html    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif
#include "TTable.h"

#include "TDatime.h"
#include "StDbBroker/dbConfig.h"

class TFileSet;
class TList;
class TBrowser;
class StDbBroker;
class St_dbConfig;
class StValiSet;

class St_db_Maker : public StMaker {
private:

  TDataSet    *fDataBase;       //! DB structure
  StDbBroker  *fDBBroker;       //!MySql broker
  St_dbConfig *fHierarchy;      //!
  TString     fDirs[10];        //! Array of dirs with DBs
  Int_t       fIsDBTime;        //! flag to use own time stamp
  TDatime     fDBTime;          //! Own DB time stamp
  Int_t       fUpdateMode;      //!
  UInt_t      fMaxEntryTime;    //! MaxEntryTime accepted from DB
  TStopwatch  fTimer[6];        //!Timer object
  int         fEvents[2];       // [0]=nEvents [1]=events with mysql request
  int         fDataSize[2];     // [0]=mysql data this event; [1]=total

//  static Char_t fVersionCVS = "$Id: St_db_Maker.h,v 1.32 2008/04/02 20:22:33 perev Exp $";
 protected:
 public:
                   St_db_Maker(const char *name
                              ,const char *dir0
                              ,const char *dir1 = ""
                              ,const char *dir2 = ""
                              ,const char *dir3 = ""
                   );
   virtual        ~St_db_Maker();
   virtual TDataSet *GetDataBase(const char* logInput, const TDatime *td=0);
   virtual TDatime GetDateTime() const;
   virtual Int_t   GetValidity(const TTable *tb, TDatime *val) const;
   virtual void    SetDateTime(int idat,int itim);
   virtual void    SetDateTime(const char *datalias);
   virtual Int_t   InitRun(int runumber);
   virtual Int_t   Init();
   virtual Int_t   Make();
   virtual Int_t   Save(const char *path,const TDatime *newtime=0);
   virtual void    SetOff(const Char_t *path);
   virtual void    SetOn (const Char_t *path);
   virtual void    SetFlavor(const char *flav,const char *tabname=".all");
   virtual void    OnOff();
   virtual void    Clear(Option_t *opt="");
   virtual Int_t   Finish();
           void    SetMaxEntryTime(Int_t idate,Int_t itime);
private:
   virtual TDataSet* UpdateDB (TDataSet* ds);
   virtual int UpdateTable(UInt_t parId, TTable* dat, const TDatime &req, TDatime val[2]);
   virtual TDataSet *LoadTable(TDataSet* left);
   virtual TDataSet *FindLeft(StValiSet *val, TDatime vals[2], const TDatime &currenTime);
   virtual TDataSet *OpenMySQL(const char* dbname);
           int       Snapshot (int flag);

   static EDataSetPass UpdateDB (TDataSet* ds,void *user );
   static EDataSetPass PrepareDB(TDataSet* ds,void *user );
          int UpdateValiSet(StValiSet *val,const TDatime &currenTime);
public:
   static TDatime  Time(const char *filename);
   static int      Kind(const char *filename);

   virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: St_db_Maker.h,v 1.32 2008/04/02 20:22:33 perev Exp $ built "__DATE__" "__TIME__ ; return cvs;}

   ClassDef(St_db_Maker, 0)
};

class St_dbConfig : public TTable
{
  ClassDefTable(St_dbConfig,dbConfig_st)
  ClassDef(St_dbConfig,2)
};

#endif
