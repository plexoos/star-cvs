/***************************************************************************
 *
 * $Id: StDAQReader.h,v 1.18 2001/07/16 21:38:44 perev Exp $
 *
 * Author: Victor Perev
 ***************************************************************************
 *
 * Description: Offline Wrapper for DAQ reader classes
 *
 ***************************************************************************
 *
 * $Log: StDAQReader.h,v $
 * Revision 1.18  2001/07/16 21:38:44  perev
 * EMC added
 *
 * Revision 1.17  2001/07/10 18:13:04  jeromel
 * Changes commited for Frank Geurts (TOF) after approval from Herb Ward
 * on Tue, 10 Jul 2001 11:19:48 and review by Victor.
 * Changes implements TOF DAQ Reader.
 *
 * Revision 1.16  2001/06/29 17:08:24  perev
 * Hide DaqLib includes from CINT
 *
 * Revision 1.15  2001/06/19 21:10:49  jeromel
 * Changes for FTPCReader (Janet S.)
 *
 * Revision 1.14  2000/07/13 22:35:10  perev
 * More accurate __CINT__ flags added
 *
 * Revision 1.13  2000/06/30 21:53:40  perev
 * L3 stuff added
 *
 * Revision 1.12  2000/06/12 15:41:19  perev
 * StTPCRead.h added into StDAQReader.h
 *
 * Revision 1.11  2000/06/12 15:04:02  perev
 * SVT + cleanup
 *
 * Revision 1.10  2000/05/25 20:01:58  perev
 * EventSize added
 *
 * Revision 1.9  2000/04/07 15:43:19  perev
 * SetVerbose method added
 *
 * Revision 1.8  2000/01/24 20:35:37  ward
 * Access trigger data.
 *
 * Revision 1.7  2000/01/24 14:39:33  perev
 * FTPC (HolmMade) is added
 *
 * Revision 1.6  1999/12/15 18:40:59  perev
 * Keep undeleted all DAQ wrapper classes
 *
 * Revision 1.5  1999/08/06 16:20:59  perev
 * RICHReader added
 *
 * Revision 1.4  1999/08/01 00:14:50  perev
 * leak removed author added
 *
 * Revision 1.3  1999/08/01 00:10:57  perev
 * leak removed author added
 *
 *
 **************************************************************************/
#ifndef _StDAQReader_
#define _StDAQReader_



//		Forward declarations
struct  EventInfo;
typedef  EventInfo DAQEventInfo;
class EventReader ;  
class StTPCReader ;  
class StEMCReader ;  
class StFTPCReader;  
class StTRGReader ;
class StSVTReader ;
#ifndef __CINT__

#include "StDaqLib/RICH/RICH_Reader.hh"
#include "StDaqLib/L3/L3_Reader.hh"
#include "StDaqLib/TOF/TOF_Reader.hh"
typedef RICH_Reader StRICHReader;
typedef L3_Reader   StL3Reader;
typedef TOF_Reader  StTOFReader;

#endif /*__CINT__*/

#ifdef __CINT__

class StRICHReader;
class StL3Reader;
class StTOFReader;

#endif /*__CINT__*/


//

 
class StDAQReader 
{
public:
  StDAQReader(const char *file=0);
  virtual ~StDAQReader();

  virtual void setVerbose(int ver=1){fVerbose=ver;};
  virtual int open(const char *file);
  virtual int close();
  virtual int isOpened(){ return (fFd != (-1));};
  virtual int readEvent();
  virtual int skipEvent(int nskip);
  virtual int getRunNumber() const;
  virtual int getEventNumber() const;
  virtual unsigned int getUnixTime() const;
  virtual unsigned int getTrigWord() const;
  virtual unsigned int getTrigInputWord() const;
  
  virtual int TPCPresent () const;  
  virtual int SVTPresent () const; 
  virtual int TOFPresent () const;
  virtual int EMCPresent () const;
  virtual int SMDPresent () const;
  virtual int FTPCPresent() const;
  virtual int RICHPresent() const;
  virtual int TRGDetectorsPresent() const;
  virtual int L3Present()   const;

  virtual void setTPCVersion(const char* vers = "TPCV2P0"); 
  virtual void setFTPCVersion(const char* vers = "FTPV1P0"); 
  virtual const char *getTPCVersion()  const {return fTPCVersion ;} 
  virtual const char *getFTPCVersion() const {return fFTPCVersion;} 
  StTPCReader  *getTPCReader (); 
  StEMCReader  *getEMCReader (); 
  StRICHReader *getRICHReader(); 
  StFTPCReader *getFTPCReader(); 
  StTRGReader  *getTRGReader ();
  StSVTReader  *getSVTReader ();
  StL3Reader   *getL3Reader  ();
  StTOFReader  *getTOFReader ();
  virtual void printEventInfo();
  virtual int  getEventSize() const;
  virtual EventReader *getEventReader() const {return fEventReader;}  

protected:
  int fFd;	//File descriptor
  int fVerbose;
  EventReader  *fEventReader;  
  StTPCReader  *fTPCReader;  
  StEMCReader  *fEMCReader;  
  StFTPCReader *fFTPCReader;  
  StRICHReader *fRICHReader;
  StTRGReader  *fTRGReader;
  StSVTReader  *fSVTReader;
  StL3Reader   *fL3Reader;
  StTOFReader  *fTOFReader;
  long fOffset;
  DAQEventInfo *fEventInfo;
  char *fFile;
  char fTPCVersion[12];
  char fFTPCVersion[12];

};
#ifndef __CINT__
#include "StTPCReader.h"
#include "StFTPCReader.h"
#include "StEMCReader.h"
#endif /*__CINT__*/
#endif /*end*/
