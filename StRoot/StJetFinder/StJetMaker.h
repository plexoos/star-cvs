/***************************************************************************
 *
 * $Id: StJetMaker.h,v 1.2 2003/04/04 21:26:42 thenry Exp $
 * $Log: StJetMaker.h,v $
 * Revision 1.2  2003/04/04 21:26:42  thenry
 * Repeated jet bug fix + debugging output not removed
 *
 * Revision 1.1  2003/02/27 21:38:10  thenry
 * Created by Thomas Henry
 *
 * Revision 1.0  2003/02/20 thenry
 * StJetMaker was created to allow multiple jet analysis modules to be
 * run simultaneosly with various parameters while the Maker loads the events
 * and analyses them.  Four different jet analyzers exist:
 *
 * Konstanin's Analyzers:
 *     Kt type: StppKonstKtJetAnalyzer
 *     Cone type: StppKonstConeJetAnalyzer
 *
 * Mike's Analyzers:
 *     Kt type: StppMikeKtJetAnalyzer
 *     Cone type: StppMikeConeJetAnalyzer
 *
 * These modules all require the StJetFinder modules.
 *
 * Author: Thomas Henry February 2003
 ***************************************************************************
 *
 * Description:  TTree Jet nano-Dst
 *
 ***************************************************************************/
#ifndef StJetMaker_h
#define StJetMaker_h
#include "StMaker.h"
#include "../StSpinMaker/StppJetAnalyzer.h"

//#define _GEANT_
#define _BBC_data_
#define _FPD_data_
#define MAXANALYZERS 12
#define DMAXEVENTSPERFILE 1000

class TFile;
class TTree;
class StppEvent;
class StppGeant;
class StBbcTriggerDetector;
class StFpdCollection;
class StMuDst;
class StMuEmcCollection;
class StMuDstMaker;
class StFourPMaker;

class StJetMaker : public StMaker {
public:
    StJetMaker(const Char_t *name, StFourPMaker* fPMaker, 
        StMuDstMaker* uDstMaker, const char *outputName);
    Int_t Init();
    Int_t Make();
    Int_t Finish();
    void setMuDst(StMuDst* p) {mudst=p;};
    StppEvent* event() {return jetEvent;} //added by MLM
    TTree* tree() {return jetTree;}  //added by MLM
    void SetMaxEventsPerFile(int newMax) { maxEventsPerFile = newMax; };
    void SetStoreEMC(bool doStore) { saveEMC = doStore; };     

    void SetSaveEventWithNoJets(bool saveIt);
    void addAnalyzer(StppJetAnalyzer* a, const char *);

protected:
    void InitFile(void);
    void FinishFile(void);

    StMuEmcCollection* muEmcCol; //!
    StppJetAnalyzer **jetBranches; //!
    char** names;                  //!
    Int_t numJetBranches;          
    bool saveEventWithNoJets;
    int maxEventsPerFile;	   //!      
    int eventFileCounter;	   //!
    int fileCounter;		   //!
    bool saveEMC;		   //!

private:
    StFourPMaker*   fourPMaker;   //!
    StMuDstMaker*   muDstMaker;   //!
    const char*     outName;      //!
    StMuDst*        mudst;        //!
    size_t          mGoodCounter; //!
    size_t          mBadCounter;  //!
    TFile           *m_outfile;   //!
    TTree           *jetTree;      //!
    StppEvent       *jetEvent;     //!
#ifdef _GEANT_
    StppGeant       *ppGeant;     //!
#endif
#ifdef _BBC_data_
    StBbcTriggerDetector *bbc;    //!
#endif
#ifdef _FPD_data_
    StFpdCollection *fpd;         //!
#endif
    Int_t            infoLevel;
  
    ClassDef(StJetMaker,1)
	};
#endif
