//StJetReader.h
//M.L. Miller
//MIT Software
//6/04

#ifndef StJetReader_HH
#define StJetReader_HH

#include "StMaker.h"
#include "StJetMaker.h"

class TTree;
class TFile;
class StMuDstMaker;

/*!
  \class StJetReader
  \author M.L. Miller (MIT Software)
  StJetReader is a utility maker that reads the jet tree from file.  Currently it only supports
  single file reading, but additions to multiple files and chain reading are planned.  Also,
  we are still debugging the InitTree() method, which would allow for the jet tree to be
  a "friend" of the StMuDst, and would therefore make the StMuDstMaker responsible for the reading
  from file (a great savings in effort and bookkeeping!).
  An example block of analysis code is given in the exampleEventAna() method.
 */
class StJetReader : public StMaker
{
public:

    ///A useful typedef for the StJets map
    typedef map<string, StJets*, less<string> > JetBranchesMap;

    ///The constructor requires a valid instance of StMuDstMaker
    StJetReader(const char* name, StMuDstMaker* uDstMaker);
    virtual ~StJetReader();
    
    virtual Int_t Init();
    virtual Int_t Make();
    virtual Int_t Finish();
    
    ///Recover the TTree from file and prepare for reading
    virtual void InitFile(const char* file);
    
    ///Prepare for reading the TTree, but as a "friend" of the StMuDst tree
    virtual void InitTree(TTree* tree);
    
    ///Access to the StJetsMap
    JetBranchesMap& jetsMap();
    
    ///An example analysis method, look here for a demonstration of jet/track histogramming
    void exampleEventAna();

private:
    JetBranchesMap mStJetsMap;
    TFile* mFile;
    TTree* mTree;
    StMuDstMaker* mDstMaker;
    int mCounter;
    
    ClassDef(StJetReader,1)
};

//inlines ---

inline StJetReader::JetBranchesMap& StJetReader::jetsMap()
{
    return mStJetsMap;
}


#endif
