/***************************************************************************
 *
 * $Id: StDAQReader.cxx,v 1.27 2001/07/10 18:13:04 jeromel Exp $
 *
 * Author: Victor Perev
 ***************************************************************************
 *
 * Description: Offline Wrapper for DAQ reader classes
 *
 ***************************************************************************
 *
 * $Log: StDAQReader.cxx,v $
 * Revision 1.27  2001/07/10 18:13:04  jeromel
 * Changes commited for Frank Geurts (TOF) after approval from Herb Ward
 * on Tue, 10 Jul 2001 11:19:48 and review by Victor.
 * Changes implements TOF DAQ Reader.
 *
 * Revision 1.26  2001/06/19 21:10:26  jeromel
 * Changes for FTPCReader (Janet S.)
 *
 * Revision 1.25  2001/03/20 01:43:14  perev
 * first event skip fixed
 *
 * Revision 1.24  2001/01/03 23:08:35  perev
 * Skip over EOF added
 *
 * Revision 1.23  2000/09/13 17:02:09  perev
 * add delete of L3Reader in close
 *
 * Revision 1.22  2000/08/28 22:19:10  ward
 * Skip corrupted events. StDaqLib/GENERIC/EventReader.cxx & StDAQMaker/StDAQReader.cxx.
 *
 * Revision 1.21  2000/07/13 22:29:52  perev
 * Return kStErr when TPC data is not in event.
 *
 * Revision 1.20  2000/07/08 22:53:39  perev
 * next offset before error return
 *
 * Revision 1.19  2000/07/06 20:39:28  fisyak
 * Herb correction for Token '0' events
 *
 * Revision 1.18  2000/06/30 21:53:40  perev
 * L3 stuff added
 *
 * Revision 1.17  2000/06/27 23:56:47  perev
 * Helen DAQ SVT update
 *
 * Revision 1.16  2000/06/12 15:04:02  perev
 * SVT + cleanup
 *
 * Revision 1.15  2000/05/25 20:01:58  perev
 * EventSize added
 *
 * Revision 1.14  2000/04/07 15:43:18  perev
 * SetVerbose method added
 *
 * Revision 1.13  2000/02/03 23:19:11  fisyak
 * Reduce file map size
 *
 * Revision 1.12  2000/01/24 20:35:37  ward
 * Access trigger data.
 *
 * Revision 1.11  2000/01/24 14:39:27  perev
 * FTPC (HolmMade) is added
 *
 * Revision 1.10  1999/12/15 18:40:59  perev
 * Keep undeleted all DAQ wrapper classes
 *
 * Revision 1.9  1999/09/24 01:22:52  fisyak
 * Reduced Include Path
 *
 * Revision 1.8  1999/09/10 16:35:38  fine
 * The oreder of deleting object has been changed to avoid crash in StDaqLib
 *
 * Revision 1.7  1999/08/19 22:28:40  perev
 * fix Skip & EventNumber
 *
 * Revision 1.6  1999/08/10 00:28:50  fisyak
 * Herb/Victor corrects for errors
 *
 * Revision 1.5  1999/08/06 16:20:58  perev
 * RICHReader added
 *
 * Revision 1.4  1999/08/01 00:14:49  perev
 * leak removed author added
 *
 * Revision 1.3  1999/08/01 00:09:07  perev
 * leak removed author added
 *
 **************************************************************************/
#include "Stypes.h"

//	non standard open,close,read
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "Stypes.h"

#include "StDaqLib/GENERIC/EventReader.hh"
#include "StDAQReader.h"
#include "StTPCReader.h"
#include "StFTPCReader.h"
#include "StTRGReader.h"
#include "StSVTReader.h"
//


typedef EventInfo DAQEventInfo;
//_____________________________________________________________________________
StDAQReader::StDAQReader(const char *file)
{
  fFd = -1;
  fVerbose = 0;
  fEventReader	= 0;
  fTPCReader 	= 0;
  fRICHReader 	= 0;
  fL3Reader 	= 0;
  fSVTReader 	= 0;
  fFTPCReader   = 0;
  fTOFReader    = 0;
  fOffset = 0;
  fFile = 0;
  fEventInfo = new DAQEventInfo;
  memset(fEventInfo,0,sizeof(DAQEventInfo));
  assert(sizeof(DAQEventInfo)==sizeof(EventInfo));
  setTPCVersion();
  setFTPCVersion();

  if(file && file[0]) open(file);
}

//_____________________________________________________________________________
int StDAQReader::open(const char *file)
{
  assert(file);
  if (fFd!=(-1) && fFile && strcmp(file,fFile)==0) return 0;
  close();
  fFile = new char[strlen(file)+1];  strcpy(fFile,file);

  fFd = ::open(fFile,O_RDONLY);
   if (fFd==-1) { 
     printf("<StDAQReader::open>  %s %s ",fFile, strerror( errno ) );
     return kStErr;
  }
  fOffset =0;   
  return 0;  
}
//_____________________________________________________________________________
int StDAQReader::close()
{
  delete [] fFile; fFile=0;
  if (fFd != (-1)) ::close(fFd);
  fFd = -1;
  delete fEventReader;	fEventReader 	= 0;  

  if(fTPCReader) 	fTPCReader ->close();  
  if(fSVTReader) 	fSVTReader ->close();  
//if (fRICHReader) 	fRICHReader->close();  
  if(fFTPCReader)       fFTPCReader->close();
  delete fL3Reader; 	fL3Reader  = 0;  
  fOffset = -1;
  return 0;
}
//_____________________________________________________________________________
StDAQReader::~StDAQReader()
{
  close();
}
//_____________________________________________________________________________
int StDAQReader::readEvent()
{  
  delete fEventReader;	fEventReader=0;
  delete fRICHReader; 	fRICHReader = 0;
  delete fL3Reader; 	fL3Reader   = 0;
  delete fTOFReader;    fTOFReader  = 0;
  if (fOffset == -1) return kStEOF;
  fEventReader = new EventReader();
  fEventReader->setVerbose(fVerbose);
  //  fEventReader->InitEventReader(fFd, fOffset, 0);
  fEventReader->InitEventReader(fFd, fOffset);
  int oldOffset = fOffset;
  fOffset = fEventReader->NextEventOffset();
  if(fEventReader->eventIsCorrupted(fFd,oldOffset)) return kStErr; // Herb, Aug 28 2000
  if(fEventReader->errorNo()) return kStErr;  
  *fEventInfo = fEventReader->getEventInfo();
  if(fEventInfo->Token==0) return kStErr;  // Herb, July 5 2000

  if (fTPCReader) { if(fTPCReader ->Update()) return kStErr; }
  if (fFTPCReader)	fFTPCReader->Update();  
  if (fTRGReader) 	fTRGReader ->Update();
  if (fSVTReader) 	fSVTReader ->Update();
  return 0;
}
//_____________________________________________________________________________
int StDAQReader::skipEvent(int nskip)
{
  for (int isk=0; nskip; nskip--,isk++) 
  {
    delete fEventReader;
    if (fOffset == -1) {
      printf("<Warning: StDAQReader::skipEvent> EOF after record %d\n",isk);
      break;}  
      
    fEventReader = new EventReader();
    //    fEventReader->InitEventReader(fFd, fOffset, 0);
    fEventReader->InitEventReader(fFd, fOffset);
    if(fEventReader->errorNo()) {
      printf("<Warning: StDAQReader::skipEvent> ReadError on record %d\n",isk);
      fOffset = -1; break;}  
    fOffset = fEventReader->NextEventOffset();
  }
  return  nskip;
}


//_____________________________________________________________________________
void StDAQReader::setTPCVersion(const char* vers)
{strcpy(fTPCVersion,vers);} 
//_____________________________________________________________________________
void StDAQReader::setFTPCVersion(const char* vers)
{strcpy(fFTPCVersion,vers);} 

//_____________________________________________________________________________
  int StDAQReader::getRunNumber()   const { return fEventReader->runno();}
//_____________________________________________________________________________
  int StDAQReader::getEventNumber() const {return fEventInfo->EventSeqNo;}
//_____________________________________________________________________________
  unsigned int StDAQReader::getUnixTime() const {return fEventInfo->UnixTime;}
//_____________________________________________________________________________
  unsigned int StDAQReader::getTrigWord() const {return fEventInfo->TrigWord;}
//_____________________________________________________________________________
  unsigned int StDAQReader::getTrigInputWord() const {return fEventInfo->TrigInputWord;}
//_____________________________________________________________________________
   int StDAQReader::TPCPresent()  const {return  fEventInfo->TPCPresent;}
//_____________________________________________________________________________
   int StDAQReader::SVTPresent()  const {return  fEventInfo->SVTPresent;}
//_____________________________________________________________________________
   int StDAQReader::TOFPresent()  const {return  fEventInfo->TOFPresent;}
//_____________________________________________________________________________
   int StDAQReader::EMCPresent()  const {return  fEventInfo->EMCPresent;}
//_____________________________________________________________________________
   int StDAQReader::SMDPresent()  const {return  fEventInfo->SMDPresent;}
//_____________________________________________________________________________
   int StDAQReader::FTPCPresent() const {return  fEventInfo->FTPCPresent;}
//_____________________________________________________________________________
   int StDAQReader::RICHPresent() const {return  fEventInfo->RICHPresent;}
//_____________________________________________________________________________
   int StDAQReader::TRGDetectorsPresent() const{return  fEventInfo->TRGDetectorsPresent;}
//_____________________________________________________________________________
   int StDAQReader::L3Present()   const {return  fEventInfo->L3Present;}
//_____________________________________________________________________________
   int StDAQReader::getEventSize()const {return  fEventInfo->EventLength;}
//_____________________________________________________________________________
StTPCReader *StDAQReader::getTPCReader() 
{
  if (!fTPCReader) {
    fTPCReader = new StTPCReader(this);
    fTPCReader->Update();
  }
  return fTPCReader;
}
//_____________________________________________________________________________
StRICHReader *StDAQReader::getRICHReader() 
{
  if (!fRICHReader) {
    fRICHReader = ::getRICHReader(fEventReader);
  }
  return fRICHReader;
}
//_____________________________________________________________________________
StL3Reader *StDAQReader::getL3Reader() 
{
  if (!fL3Reader) {
    fL3Reader = ::getL3Reader(fEventReader);
  }
  return fL3Reader;
}
//_____________________________________________________________________________
StFTPCReader *StDAQReader::getFTPCReader() 
{
  if (!fFTPCReader) {
    fFTPCReader = new StFTPCReader(this);
    fFTPCReader->Update();
  }
  return fFTPCReader;
}
//_____________________________________________________________________________
StTOFReader *StDAQReader::getTOFReader()
{
  if (!fTOFReader) {
    fTOFReader = ::getTOFReader(fEventReader);
  }
  return fTOFReader;
}
//_____________________________________________________________________________
StTRGReader *StDAQReader::getTRGReader()
{
  if (!fTRGReader) {
    fTRGReader = new StTRGReader(this);
    fTRGReader->Update();
  }
  return fTRGReader;
}
//_____________________________________________________________________________
StSVTReader *StDAQReader::getSVTReader()
{
  if (!fSVTReader) {
    fSVTReader = new StSVTReader(this);
    fSVTReader->Update();
  }
  return fSVTReader;
}
//_____________________________________________________________________________
void StDAQReader::printEventInfo()
{fEventReader->printEventInfo();}
