// $Id: StSpectraMaker.cxx,v 1.14 2003/09/02 17:58:59 perev Exp $
// $Log: StSpectraMaker.cxx,v $
// Revision 1.14  2003/09/02 17:58:59  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.13  2001/09/14 18:00:24  perev
// Removed references to StRun.
//
// Revision 1.12  2000/03/28 03:20:06  munhoz
// correcting normalization of weighted histogram
//
// Revision 1.11  2000/03/23 03:21:49  munhoz
// added V0 classes
//
// Revision 1.10  2000/03/12 19:28:28  ogilvie
// added new analysis class, StNoPidSpectraAnalysis, for inclusive spectra
//
// Revision 1.9  2000/03/10 19:54:07  ogilvie
// bug fix in name of histograms
//
// Revision 1.8  2000/03/08 02:30:20  ogilvie
// individual output .root files per analysis, prep. for user choice of axes, (y,eta) (pperp,mperp)
//
// Revision 1.7  2000/03/03 03:30:32  ogilvie
// major infra. change to read in/use efficiency histos
//
// Revision 1.6  2000/01/11 19:09:12  ogilvie
// compiles on sun CC5, linux, but not sun cc4
//
// Revision 1.5  1999/11/28 20:22:06  ogilvie
// updated to work with new StEvent
//
// Revision 1.4  1999/11/22 01:54:58  ogilvie
// generalised analysis containers to beany object that inherits from StSpectraAnalysis
//
// Revision 1.3  1999/11/08 17:07:32  ogilvie
// *** empty log message ***
//
// Revision 1.2  1999/11/05 18:58:49  ogilvie
// general tidy up following Mike Lisa's review. List of analyses conntrolled via
// analysis.dat, rather than hardcoded into StSpectraMaker.cxx
//
// Revision 1.1  1999/11/03 21:22:41  ogilvie
// initial version
//
//
///////////////////////////////////////////////////////////////////////////////
//
///////////////////////////////////////////////////////////////////////////////
#include "StSpectraMaker.h"
#include "StChain.h"
//-tu #include "StRun.h"
#include "StEventTypes.h"
#include "StSpectraAnalysis.h"
#include "StTpcDeviantSpectraAnalysis.h"
#include "StNoPidSpectraAnalysis.h"
#include "StV0SpectraAnalysis.h"
#include "StMessMgr.h"
#include "StEfficiency.h"
#include "StSpectraAxesEnumeration.h"

#include "Stiostream.h"

string readString(ifstream& ifs) {
  string line;
  #if defined(__SUNPRO_CC)
        char c; line.erase();
        while ((c = ifs.get()) && c != '\n' && !ifs.eof()) line += c;
  #else
        getline(ifs,line,'\n');
  #endif
  cout << line.c_str() << endl;
  return line;
}

static const char rcsid[] = "$Id: StSpectraMaker.cxx,v 1.14 2003/09/02 17:58:59 perev Exp $";

StSpectraMaker::StSpectraMaker(const Char_t *name) : StMaker(name) {
}

StSpectraMaker::~StSpectraMaker() {
}

Int_t StSpectraMaker::Init() {

  //
  // create the analyses that are stored in the file analysis.dat, 
  // 

  ifstream from("analysis.dat");
  StSpectraAnalysisType analysisType;
  string analysisSType = readString(from);
  while (!from.eof()) {
    //

    if (analysisSType == "TpcDeviant") {
      analysisType= kTpcDeviant;
    } else if (analysisSType == "TpcDedx") {
      analysisType= kTpcDedx;
    } else if (analysisSType == "V0") {
      analysisType= kV0;
    } else if (analysisSType == "NoPid") {
      analysisType= kNoPid;
    } else {
      // protection?
      gMessMgr->Message("There is no type specified here!","E");
      return kStErr;
    }
    cout << "particle name " ;
    string particleName = readString(from);
    string analysisTitle = readString(from); 
    char* efficiencyFile = new char[100];
    from >> efficiencyFile; 
    cout << efficiencyFile << endl;
    StEfficiency effic(efficiencyFile);
    delete efficiencyFile; 

    StSpectraOrdinate ordinateType = effic.getOrdinate();
    StSpectraAbscissa abscissaType = effic.getAbscissa();

    effic.setParticle(particleName);
    
    float lbinAbscissa = effic.getLowEdge('x');
    float ubinAbscissa = effic.getUpEdge('x');
    int   nbinAbscissa = effic.getNbin('x');
    cout <<"abscissa range and bins "<< lbinAbscissa << " "
	 << ubinAbscissa << " " 
	 << nbinAbscissa << endl;  
    float lbinOrdinate = effic.getLowEdge('y');
    float ubinOrdinate = effic.getUpEdge('y');
    int   nbinOrdinate = effic.getNbin('y');
    cout <<"ordinate range and bins "<< lbinOrdinate << " "
	 << ubinOrdinate << " " 
	 << nbinOrdinate << endl; 
    StSpectraAnalysis* anal;
    if (analysisType == kTpcDeviant) {       
       StTpcDeviantSpectraAnalysis* local_anal = new StTpcDeviantSpectraAnalysis;
       anal = local_anal;
    } else if (analysisType == kNoPid) {       
       StNoPidSpectraAnalysis* local_anal = new StNoPidSpectraAnalysis;
       anal = local_anal;
    } else if (analysisType == kV0) { 
       StV0SpectraAnalysis* local_anal = new StV0SpectraAnalysis;
       local_anal->setParticle(particleName);
       anal = local_anal;
    } else {
      continue;
    }
    anal->setParticle(particleName);
    anal->setTitle(analysisTitle);
    anal->setEfficiency(effic);   
    anal->setAbscissa(abscissaType,lbinAbscissa,ubinAbscissa,nbinAbscissa); 
    anal->setOrdinate(ordinateType,lbinOrdinate,ubinOrdinate,nbinOrdinate);
 
    mSpectraAnalysisContainer.push_back(anal);

    string comment = readString(from);
    analysisSType = readString(from);
   }
  from.close();
 
  //
  //
  // loop through the analyses and book histograms
  // 

  vector<StSpectraAnalysis*>::const_iterator analysisIter;

  for (analysisIter = mSpectraAnalysisContainer.begin();
	 analysisIter != mSpectraAnalysisContainer.end();
	 analysisIter++) {
    (*analysisIter)->bookHistograms();
  }
  return StMaker::Init();
}

Int_t StSpectraMaker::Make() {

  StEvent* mEvent;
  mEvent = (StEvent *) GetInputDS("StEvent");
  if (! mEvent) return kStOK; // If no event, we're done
  StEvent& ev = *mEvent;
  //
  // for each analysis, call fill histograms
  //
  vector<StSpectraAnalysis*>::const_iterator analysisIter;

  for (analysisIter = mSpectraAnalysisContainer.begin();
	 analysisIter != mSpectraAnalysisContainer.end();
	 analysisIter++) {
    (*analysisIter)->fillHistograms(ev);
  }  
  return kStOK;
}

void StSpectraMaker::Clear(Option_t *opt) {

  StMaker::Clear();
}

Int_t StSpectraMaker::Finish() {

  vector<StSpectraAnalysis*>::const_iterator analysisIter;

  for (analysisIter = mSpectraAnalysisContainer.begin();
	 analysisIter != mSpectraAnalysisContainer.end();
	 analysisIter++) {
    (*analysisIter)->projectHistograms();
  }

  for (analysisIter = mSpectraAnalysisContainer.begin();
	 analysisIter != mSpectraAnalysisContainer.end();
	 analysisIter++) {
    (*analysisIter)->writeHistograms();
  }

  // delete analyses in container

  for (analysisIter = mSpectraAnalysisContainer.begin();
	 analysisIter != mSpectraAnalysisContainer.end();
	 analysisIter++) {
    delete (*analysisIter);
  }

  return kStOK;
}


ClassImp(StSpectraMaker)



