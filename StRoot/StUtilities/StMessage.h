// $Id: StMessage.h,v 1.10 2001/05/16 15:18:50 genevb Exp $
// $Log: StMessage.h,v $
// Revision 1.10  2001/05/16 15:18:50  genevb
// Need include for TROOT
//
// Revision 1.9  2001/05/14 20:53:20  genevb
// Add features to examine memory use, switch from TDatime to time_t
//
// Revision 1.8  2000/06/10 21:19:02  genevb
// Remove use of virtual functions
//
// Revision 1.7  2000/01/05 19:53:46  genevb
// Fixed CC5 warnings, and several other small improvements under the hood
//
// Revision 1.6  1999/08/18 18:28:33  fine
// Various bugs have been fixed. share lib was not loaded under HP
//
// Revision 1.5  1999/07/08 22:58:18  genevb
// Created an abstract interface with StMessMgr.h hiding template implementation from others, a few other small fixes
//
// Revision 1.4  1999/06/30 17:24:50  genevb
// Better limit management, remove Bool_t
//
// Revision 1.3  1999/06/29 17:37:31  genevb
// Lots of fixes...
//
// Revision 1.2  1999/06/24 16:30:41  genevb
// Fixed some memory leaks
//
// Revision 1.1  1999/06/23 15:17:47  genevb
// Introduction of StMessageManager
//
//
// Revision 1.1 1999/01/27 10:28:29 genevb
//
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StMessage                                                            //
//                                                                      //
// This is the class of messages used by StMessageManager in STAR.      //
// Messages have a type and message specified at instantiation,         //
// and also include a time-date stamp and options for printing.         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ClassStMessage
#define ClassStMessage

#ifdef __ROOT__
#include "TROOT.h"
#endif
#include <time.h>

class StMessage {

 private:

 protected:
   char type[2];
//   char* location;                    //!
//   unsigned long runNumber;
//   pair<long, long> eventId;
   char* option;                      //!
   time_t messTime;
   char* message;                     //!

 public:
   StMessage(const char* mess="", const char* ty="I", const char* opt="O");
   StMessage(const StMessage&){;}
   virtual ~StMessage();
          void PrintInfo();
           int Print(int nChars=-1);
       time_t& GetTime() {return messTime;}
   const char* GetType() const {return type;}
         char* GetMessage() {return message;}
         char* GetOptions() {return option;}
          void SetOption(char* opt) {option = opt;}
        size_t GetMemoryUsage();
#ifdef __ROOT__
   ClassDef(StMessage,1)
#endif
};

#endif
