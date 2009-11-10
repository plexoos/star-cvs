//
//  This is a STAR typical comment header. You should modify
//  it to reflect your changes.
//  As a minimum it should contain the name of the author, the
//  date it was written/modified, and a short description of what
//  the class is meant to do. The cvs strings $X$ (where X=Id, Log)
//  are not needed when you do not intend to put the file under
//  cvs control. Remove them.
//  
/*!
 * \class  StAnalysisMaker
 * \brief  A typical Analysis Class
 * \author Torre Wenaus, BNL, Thomas Ullrich
 * \date   Nov 1999
 *
 *
 * This is an example of a maker to perform analysis using StEvent.
 * Use this as a template and customize it for your studies.
 *
 * $Id: StAnalysisMaker.h,v 2.7 2009/11/10 20:17:59 fisyak Exp $
 *
 */
/* -------------------------------------------------------------------------
 * $Log: StAnalysisMaker.h,v $
 * Revision 2.7  2009/11/10 20:17:59  fisyak
 * Add print out for StEvent track and hits
 *
 * Revision 2.6  2009/11/03 15:13:22  fisyak
 * Comment print out, wait till StEvent will be mofidied
 *
 * Revision 2.5  2009/11/03 15:03:56  fisyak
 * Add static method to print StEvent
 *
 * Revision 2.4  2003/09/10 19:47:02  perev
 * ansi corrs
 *
 * Revision 2.3  2002/04/28 00:10:28  jeromel
 * doxygen basic dox added. GetCVS() had wrong signature : corrected to avoid
 * propagation of this typo in new makers.
 *
 * Revision 2.2  2000/07/12 05:23:28  ullrich
 * Updated for better use as template for actual analysis.
 *
 * Revision 2.1  1999/12/30 01:54:57  ogilvie
 * added countPrimaryPions as example how to use PID
 *
 * Revision 2.0  1999/11/04 16:10:03  ullrich
 * Revision for new StEvent
 *
 * -------------------------------------------------------------------------
 */

//
// Every header file should have these macros to protect
// from being included multiple times in the same scope.
// If you change the name of the class change the name
// of the macro.
//
#ifndef StAnalysisMaker_hh     
#define StAnalysisMaker_hh

//
//  Include files. StMaker.h is needed since your maker
//  inherits from StMaker. 
//  <string> contains the STL string class. It's a system
//  header therefore it is enclosed in <> and not in double
//  quotes.
//
#include "StMaker.h"
#include <string>

//
//  Forward declarations.
//  It is always a good idea to reduce the dependencies
//  to other header files. This can be achieved by
//  forward declaring classes which are only referenced
//  but not contained (by value) in the class decleration.
//  In the implementation then one onviously has to include
//  the referring header. Another advantage of this
//  technique is that the these classes do not get passed
//  through rootcint.
//
class StEvent;
class StTrack;
class TFile;
class TNtuple;

//
//  On some systems (e.g. Sun) the STL is contained in
//  namespace 'std'. We have to tell the compiler where
//  to look for string. Since not all compilers
//  use namespaces we have to protects is by using the
//  ST_NO_NAMESPACES macro which is automatically set
//  (or unset) when compiling the code with 'cons'.
//
#ifndef ST_NO_NAMESPACES
using std::string;
#endif

//
//  The class declaration. Every maker has to
//  inherit from StMaker.
//
class StAnalysisMaker : public StMaker {
public:

    StAnalysisMaker(const Char_t *name="analysis");     // constructor
    ~StAnalysisMaker();                                 // destructor
    
    void Clear(Option_t *option="");    // called after every event to cleanup 
    Int_t  Init();                      // called once at the beginning of your job
    Int_t  Make();                      // invoked for every event
    Int_t  Finish();                    // called once at the end
    static void PrintStEvent(Int_t k = 0);
    static void PrintTpcHits(Int_t sector = 0, Int_t row = 0, Bool_t plot = kFALSE);
    static void PrintSvtHits();
    static void PrintSsdHits();
    static void PrintRnDHits();
    virtual const char *GetCVS() const {
      static const char cvs[]="Tag $Name:  $ $Id: StAnalysisMaker.h,v 2.7 2009/11/10 20:17:59 fisyak Exp $ built "__DATE__" "__TIME__ ; 
      return cvs;
    }

private:
    //
    //  Add your data member and new methods here.
    //  The "//!" means that rootcint is not adding
    //  the data member to the streamer. Don't worry
    //  if you don't know what this means.
    //  In general it is a good idea in analysis makers
    //  to always add the //! after a member.
    //

    //
    //  Methods (== member functions)
    //  Remember: these are just examples!
    //
    bool accept(StEvent*);            // this method serves as an event filter
    bool accept(StTrack*);            // and this is used to select tracks

    //
    //  Data members
    //  Note, that it is recommended to start all member names with
    //  an 'm'. This makes it easier to read the code later.
    //
    int        mEventCounter;  //!
    string     mFileName;      //!
    TFile      *mFile;         //!
    TNtuple    *mTuple;        //!

    //
    //  This is needed to make your maker known to root4star.
    //  It must be always the last statement in the class.
    //  Note that this is a macro, that's why the ';' is missing.
    //
    ClassDef(StAnalysisMaker,0)
};
#endif
