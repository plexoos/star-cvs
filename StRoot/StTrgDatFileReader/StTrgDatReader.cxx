// $Id: StTrgDatReader.cxx,v 1.1 2009/10/13 15:53:31 fine Exp $
//
// $Log: StTrgDatReader.cxx,v $
// Revision 1.1  2009/10/13 15:53:31  fine
// Akio\'s DAT file format reader
//
// 

/***************************************************************************
 * Author: akio ogawa
 * Description: Trigger Data file (run*.*.dat) reader
 **************************************************************************/

#include "StTrgDatReader.h"
#include "StArchInfo.h"
#include <iostream>

namespace {
//__________________________________________________________________________
inline unsigned int swapI(unsigned int var){
  return
    (var & 0xff000000) >> 24 |
    (var & 0x00ff0000) >> 8  |
    (var & 0x0000ff00) << 8  |
    (var & 0x000000ff) << 24 ;
}
}
const int StTrgDatReader::mLheader=8;
//__________________________________________________________________________
StTrgDatReader::StTrgDatReader():StStreamFile(), mLength(0),mVersion(0), mData(0),mAllocated(0) 
{
   Buffer(mLheader);
}

//__________________________________________________________________________
StTrgDatReader::StTrgDatReader(const char *fileName, ios_base::openmode mode)
      :StStreamFile(fileName,mode), mLength(0), mVersion(0),mData(0),mAllocated(0)
{
   Buffer(mLheader);
}


//__________________________________________________________________________
StTrgDatReader::~StTrgDatReader() { Buffer(0) ; }

//__________________________________________________________________________
char *StTrgDatReader::Buffer(streamsize n)
{
   if ( (n >mAllocated) || (n==0)) {
      mData = (char *)realloc(mData,n);
      assert(!n || (mData && "There is no memory to allocate the I/O buffer"));
      mAllocated=n;
   }
   return mData;
}

//__________________________________________________________________________
fstream &StTrgDatReader::Read(){
  if (is_open() ) {
     read(Buffer(mLheader), mLheader);
     if (good()) {
        mVersion = swapI(*(unsigned int*)mData);
        mLength  = swapI(*(unsigned int*)(mData+4));
        assert( (mLength - mLheader > 0) && "The file is too short!!!");
        if(Debug() >0) { 
           printf("Version = %x\nLength  = %d; data=%p\n",Version(), Length(),mData);
        }
        read(Buffer(mLength)+mLheader, mLength - mLheader);
    }
    if (!good()) {
       mLength = 0;
       if (!eof()) Perror("*********** Attention DAT errors:");
    }
  }
  return stream();
}

//__________________________________________________________________________
int StTrgDatReader::Length() const   { return mLength;  }

//__________________________________________________________________________
int  StTrgDatReader::Version() const { return  mVersion;} 

//__________________________________________________________________________
char* StTrgDatReader::Record()       { return mData;    }
