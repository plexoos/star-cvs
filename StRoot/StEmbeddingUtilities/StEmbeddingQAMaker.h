//****************************************************************************************************
//  Class StEmbeddingQAMaker
//
//    Produce QA histograms for embedding and real data
//    - For embedding outputs, we currently include the histograms for 
//      MC, MATCHED, GHOST, CONTAM, MATGLOB pairs 
//      (NOTE: MATGLOB pairs don't have a function to get the number of tracks
//       we have to implement that function in StMiniMcEvent later
//       in order to obtain the QA histograms)
//
//    - For real data outputs, both primary and global tracks will be analyzed
//
//
//    To get the QA histograms, you can run the macro "doEmbeddingQAMaker.C" under StRoot/macros/embedding
//
//    For example,
//    root4star -b -q doEmbeddingQAMaker.C'(2007, "P08ic", "PiPlus", "minimc.list", "embedding.root")'
//    
//    or
//
//    roo4star -b
//    > .L doEmbeddingQAMaker.C
//    > doEmbeddingQA(2007, "P08ic", "PiPlus", "minimc.list");
//    > ...
//    > .q
//
//    The details of arguments can be found in the doEmbeddingQAMaker.C
//
//    NOTE:
//     - Particle name (3rd argument, in this case "PiPlus") is case INSENSITIVE. 
//       You can use PiPlus, piPlus, piplus or whatever you want. 
//       Please look at the StRoot/macros/embedding/StEmbeddingQAUtilities.cxx 
//       for more details about particle name.
//
//     - Output file name (5th argument, in this case "embedding.root") will be 
//       automatically detemined according to the "year", "production" 
//       and "particle name" if you leave it blank.
//----------------------------------------------------------------------------------------------------
//   Revised history
//
//   Oct/23/2009 : Add pid cuts (nsigma <2) for e/pi/K/p for real data
//   Sep/20/2009 : Fix title for Nhit histograms. Wider eta/y range (-2.5,2.5)
//   Sep/18/2009 : Added QA histograms for decay daughters (H. Masui)
//   Sep/09/2009 : 1st version checked in   (H. Masui)
//****************************************************************************************************

#ifndef __StEmbeddingQAMaker_h__
#define __StEmbeddingQAMaker_h__

#include "TMath.h"
#include "TString.h"

#include "StEmbeddingQAUtilities.h"

class TFile ;
class TH1 ;
class TH2 ;
class TH3 ;
class TObject ;

class StContamPair ;
class StMiniMcEvent ;
class StEmbeddingQAPair ;
class StEmbeddingQATrack ;
class StMuDstMaker ;
class StMuTrack ;
class StTinyMcTrack ;

class StEmbeddingQAParticleCollection ;

//____________________________________________________________________________________________________
//  Analyze either minimc trees for embedding QA or microDST for real data QA
class StEmbeddingQAMaker {
  public:
    StEmbeddingQAMaker() ; // Default is 2007, P08ic, pi+
    StEmbeddingQAMaker(const Int_t year, const TString production, const Int_t particleId, 
        const Bool_t isSimulation = kTRUE); // specify year, production, particle id (default is embedding QA)
    StEmbeddingQAMaker(const Int_t year, const TString production, const TString name,
        const Bool_t isSimulation = kTRUE); // specify year, production, particle name (default is embedding QA)
    virtual ~StEmbeddingQAMaker();

    Bool_t book(const TString outputFileName = ""); // Default output is "ana_embedding_{year}_{production}_{particleId}.root" for embedding QA
    Bool_t make(const TString inputFileName, const Bool_t isSimulation = kTRUE);

    Bool_t run(const TString inputFileList) ; // Either RunRealData or RunEmbedding according to the kIsSimulation flag
    Bool_t runRealData(const TString inputFileList) ;
    Bool_t runEmbedding(const TString inputFileList) ;
    Bool_t end(); // Close output file

    void setDebug(const Int_t val) ;

  private:
    static const Float_t kVertexCut ; // z-vertex cut (Default is 30 cm)
    const Int_t kYear ;               // Year
    const TString kProduction ;       // Production
    const Int_t kParticleId ;         // Input particle id
    const Bool_t kIsSimulation ;      // kTRUE : embedding QA,  kFALSE : real data QA

    // Initialization
    void init() ;

    // Fill
    Bool_t fillEmbedding(const TString inputFileName) ; // Fill embedding histograms
    Bool_t fillRealData(const TString inputFileName) ;  // Fill real data histograms

    void fillMcTracks(const StMiniMcEvent& mcevent, const Int_t trackid, const Int_t itrk);
    void fillMatchedPairs(const StMiniMcEvent& mcevent, const Int_t trackid, const Int_t itrk);
    void fillGhostPairs(const StMiniMcEvent& mcevent, const Int_t trackid, const Int_t itrk);
    void fillContamPairs(const StMiniMcEvent& mcevent, const Int_t trackid, const Int_t itrk);
    void fillMatGlobPairs(const StMiniMcEvent& mcevent, const Int_t trackid, const Int_t itrk);
    void fillRealTracks(const StMuTrack& track, const Int_t trackid, const Int_t itrk);
    void fillHistograms(const StEmbeddingQATrack& track, const Int_t trackid, const Int_t iparticle, const Int_t geantId);

    // Cuts
    Bool_t isZVertexOk(const StMiniMcEvent& mcevent, const Float_t vertexCut = 30.0) const ;

    // Number of histograms for each branch
    Int_t getNumberOfHistograms(const Int_t categoryId) const ;

    Int_t mDebug ;
    StMuDstMaker* mMuDstMaker ;

    StEmbeddingQAParticleCollection* mParticles ;

    // Output histograms
    TFile* mOutput ;

    // Event-wise histograms
    Double_t mVz ;
    TH1* hVz ;         // z-vertex
    TH1* hVzAccepted ; // z-vertex (with z-vertex cut)

    TH2* hVyVx ; // y vs x vertices
    TH1* hdVx ; // vx(real) - vx(MC)
    TH1* hdVy ; // vy(real) - vy(MC)
    TH1* hdVz ; // vz(real) - vz(MC)

    // Tracks
    //  Fill Daughters for contaminated pairs
    TH1** hGeantId[StEmbeddingQAUtilities::kNCategory];         // Geant id
    TH3** hNHit[StEmbeddingQAUtilities::kNCategory];            // Nhit distribution vs eta vs pt
    TH3** hDca[StEmbeddingQAUtilities::kNCategory];             // Dca vs eta vs pt
    TH2** hPtVsEta[StEmbeddingQAUtilities::kNCategory];         // pt vs pseudo-rapidity
    TH2** hPtVsY[StEmbeddingQAUtilities::kNCategory];           // pt vs rapidity
    TH2** hPtVsPhi[StEmbeddingQAUtilities::kNCategory];         // pt vs phi
    TH2** hPtVsMom[StEmbeddingQAUtilities::kNCategory];         // pt vs momentum
    TH2** hdPtVsPt[StEmbeddingQAUtilities::kNCategory];         // pt - pt(MC) vs pt
    TH2** hMomVsEta[StEmbeddingQAUtilities::kNCategory];        // momentum vs eta
    TH2** hdEdxVsMom[StEmbeddingQAUtilities::kNCategory];       // dE/dx vs momentum (no PID cut)
    TH2** hdEdxVsMomPidCut[StEmbeddingQAUtilities::kNCategory]; // dE/dx vs momentum (with PID cut, 2 sigma)

    TH2** hEtaVsPhi[StEmbeddingQAUtilities::kNCategory];  // pseudo-rapidity vs phi
    TH2** hEtaVsVz[StEmbeddingQAUtilities::kNCategory];   // pseudo-rapidity vs vz
    TH2** hYVsVz[StEmbeddingQAUtilities::kNCategory];     // rapidity vs vz

    ClassDef(StEmbeddingQAMaker, 1);
};

inline void StEmbeddingQAMaker::setDebug(const Int_t val) { mDebug = val ; }

#endif

