// for each little section here, do your own "ifndef....endif"
// the reason for that is because this file may be processed more
// than once (by cint "preprocessor" and by C++ preprocessor)

//#include "StHbtMaker/Infrastructure/franks1Histo.hh"  
//#include "StHbtMaker/Infrastructure/franks2Histo.hh"  
#ifndef __ROOT__
//#include "franks1Histo.hh"  
//#include "franks2Histo.hh"  
#include "franks1HistoD.hh"  
#include "franks2HistoD.hh"  
#endif

/*------------------------------
 * Enumerations                */
#ifndef HBT_Enumarations
#define HBT_Enumarations
#ifdef __CINT__
#pragma link C++ enum StHbtParticleType;
#endif
enum StHbtParticleType {hbtUndefined, hbtTrack, hbtV0};
#endif

/*------------------------------------
 * Histograms
 */
#ifndef StHbt_Histos
#define StHbt_Histos

#ifdef __ROOT__

#ifndef ROOT_TH1
#include "TH1.h"
#include "TF1.h"
#endif

#ifndef ROOT_TH2
#include "TH2.h"
#include "TF2.h"
#endif

#ifndef ROOT_TH3
#include "TH3.h"
#include "TF3.h"
#endif

#include "TTree.h"




typedef TH1D StHbt1DHisto;
typedef TH2D StHbt2DHisto;
typedef TH3D StHbt3DHisto;
typedef TF1 StHbtTF1;
typedef TF2 StHbtTF2;
typedef TF3 StHbtTF3;
typedef TTree StHbtTree;

#else 

//typedef franks1Histo<> StHbt1DHisto;
//typedef franks2Histo<> StHbt2DHisto;
typedef franks1HistoD StHbt1DHisto;
typedef franks2HistoD StHbt2DHisto;

#endif // __ROOT__

#endif


/*----------------------------------------------------------------
 * Strings
 * This insane little bit is because Cint, which is supposed to be
 * so much like C++, doesn't even recognize "string" as a type!
 */

#ifndef __CINT__
#ifndef StHbtString_noCint
#define StHbtString_noCint
#include <string>
#if !defined(ST_NO_NAMESPACES)
using std::string;
#endif
typedef string StHbtString; //!
#endif

#else
#ifndef StHbtString_yesCint
#define StHbtString_yesCint
class StHbtString; //!
#endif
#endif

/*----------------------------------------------------------------
 * Vectors:
 * Even the STAR Class Library keeps changing in terms of what will
 * work!  Unfortunately, we have to abstract even the ThreeVectors
 * and LorentzVectors to here, so that we can roll with the punches
 * from sofi more easily.
 */

#ifndef StHbtVectors_hh
#define StHbtVectors_hh
//#include "StThreeVector.hh"
//typedef StThreeVector<double> StHbtThreeVector;//!
#include "StThreeVectorD.hh"
typedef StThreeVectorD StHbtThreeVector;//!
//#include "StLorentzVector.hh"
//typedef StLorentzVector<double> StHbtLorentzVector;//!
#include "StLorentzVectorD.hh"
typedef StLorentzVectorD StHbtLorentzVector;//!
#include "StHelix.hh"
typedef StHelix StHbtHelix;//!
#include "StPhysicalHelixD.hh"
typedef StPhysicalHelixD StHbtPhysicalHelix;//!

#endif
