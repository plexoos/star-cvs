//StppJetAnalyzer.cxx
//M.L. Miller (Yale Software)
//07/02
//Modified by Thomas Henry
//08/02 StppJetAnalyzer converted to an interface class

//std
#include <list>
#include <time.h>
#include <algorithm>
#include "Stiostream.h"
#include <math.h>
using namespace std;

//StJetFinder
#include "StJetFinder/FourVec.h"
#include "StJetFinder/StProtoJet.h"
#include "StJetFinder/StJetFinder.h"
#include "StJetFinder/StKtCluJetFinder.h"
#include "StJetFinder/StConeJetFinder.h"
#include "StJetFinder/StCdfChargedConeJetFinder.h"

//StMuDst
#include "StMuDSTMaker/COMMON/StMuDst.h"
#include "StMuDSTMaker/COMMON/StMuEvent.h"
#include "StMuDSTMaker/COMMON/StMuTrack.h"

//StJetMaker
#include "StMuTrackFourVec.h"
#include "StJet.h"
#include "StppJetAnalyzer.h"

ClassImp(StppJetAnalyzer)
ClassImp(StppAnaPars)
    
//cstr
    
//StppJetAnalyzer::StppJetAnalyzer(const StJetPars* pars)
    StppJetAnalyzer::StppJetAnalyzer(const StppAnaPars* ap, const StJetPars* pars, StFourPMaker* fp) : mFourPMaker(fp), mPars(*ap)
{
    cout <<"StppJetAnalyzer::StppJetAnalyzer()"<<endl;
    
    //Set the finder!
    
    if (dynamic_cast<const StKtCluPars*>(pars)) {
	const StKtCluPars* temp = dynamic_cast<const StKtCluPars*>(pars);
	cout <<"StppJetAnalyzer, instantiate StKtCluJetFinder"<<endl;
	StKtCluJetFinder* jf = new StKtCluJetFinder(*temp);
	mFinder = jf;
    }
    else if (dynamic_cast<const StCdfChargedConePars*>(pars)) { //note, this if MUST come before the StConePars test
	cout <<"StppJetAnalyzer, instantiate StCdfChargedConeJetFinder"<<endl;
	const StCdfChargedConePars* temp = dynamic_cast<const StCdfChargedConePars*>(pars);
	StCdfChargedConeJetFinder* jf = new StCdfChargedConeJetFinder(*temp);
	mFinder = jf;
	jf->print();
    }
    else if (dynamic_cast<const StConePars*>(pars)) {	
	cout <<"StppJetAnalyzer, instantiate StConeJetFinder"<<endl;
	const StConePars* temp = dynamic_cast<const StConePars*>(pars);
	StConeJetFinder* jf = new StConeJetFinder(*temp);
	mFinder = jf;
	jf->print();
    }
    else {
	cout <<"StppJetAnalyzer, unkown algorithm"<<endl;
	abort();
    }

    //setup the tree
    muDstJets = new StJets();
    muDstJets->Clear();
}

StppJetAnalyzer::~StppJetAnalyzer()
{
    cout <<"StppJetAnalyzer::~StppJetAnalyzer()"<<endl;
    delete muDstJets;
}

void StppJetAnalyzer::clear()
{
    /*
    //clearAndDestroy four list
    int ngone = 0;
    for (FourList::iterator it=mFourList.begin(); it!=mFourList.end(); ++it) {
	AbstractFourVec* temp = *it;
        if(temp != NULL) {
	    delete temp;
	    temp=0;
	    ++ngone;
	}
    }
    cout <<"StppJetAnzlyzer::clear().  Deleted:\t"<<ngone<<"\tobjects"<<endl;
    */
    
    mFourList.clear();
    mProtoJets.clear();
}

void StppJetAnalyzer::print()
{
    cout <<"\t---   Contents of mProtoJets ---"<<endl;
    for (JetList::const_iterator it2=mProtoJets.begin(); it2!=mProtoJets.end(); ++it2) {
	cout <<*it2<<endl;
    }
}

bool StppJetAnalyzer::accept(StMuTrack* p)
{
    if (!p) {
	cout <<"StppJetAnalyzer::accept(StMuTrack*) ERROR:\t p==0"<<endl;
    }
    bool passedP = (p!=0) ? true : false;
    bool passedFlag = p->flag() > mPars.mFlagMin;
    bool passedPt = p->pt() > mPars.mPtMin;
    bool passedEta = fabs(p->eta()) < mPars.mEtaMax;
    bool passedNHits = p->nHits() > mPars.mNhits;
    //cout <<"p:\t"<<passedP<<"\tflag:\t"<<passedFlag<<"\tpt:\t"<<passedPt<<"\teta:\t"<<passedEta
    //<<"\tnhits:\t"<<passedNHits<<endl;
    return passedP && passedFlag && passedPt && passedEta && passedNHits;

    /*
      return (p
      && p->flag()> mPars.mFlagMin
      && p->pt()>mPars.mPtMin
      && fabs(p->eta())<mPars.mEtaMax
      && p->nHits()>mPars.mNhits
      );
    */
}

bool StppJetAnalyzer::accept(StMuTrackFourVec* p)
{
    if(p->particle()) 
	return accept(p->particle());

    return (p
	    && p->pt()>mPars.mPtMin
	    && fabs(p->eta())<mPars.mEtaMax
	    );
}
	

bool StppJetAnalyzer::accept(const StProtoJet& pj)
{
    return (
	    pj.pt() > mPars.mPtMin
	    && fabs(pj.eta()) < mPars.mEtaMax
	    );
}

void StppJetAnalyzer::acceptJets(void)
{
    JetList newList;
    for (JetList::iterator it=mProtoJets.begin(); it!=mProtoJets.end(); ++it)
	{
	    newList.push_back(*it);
	}
    mProtoJets.clear();
    //loop on jets
    for (JetList::iterator it=newList.begin(); it!=newList.end(); ++it) 
	{
	    if(acceptJet(*it)) 
		mProtoJets.push_back(*it);
	}
}

bool StppJetAnalyzer::acceptJet(StProtoJet &pj)
{
    return (
	    (pj.pt() > mPars.mJetPtMin)
	    && (fabs(pj.eta()) < mPars.mJetEtaMax)
	    && (fabs(pj.eta()) > mPars.mJetEtaMin)
            && ((int)pj.numberOfParticles() >= mPars.mJetNmin)
	    );
}

void StppJetAnalyzer::fillLists(FourList &tracks)
{
    cout <<"StppJetAnalyzer::fillList()";
    int nTracks=0;
    int nAcceptedTracks=0;
    int nAcceptedProtoJets=0;
    for (FourList::iterator i=tracks.begin(); i != tracks.end(); i++) {
	++nTracks;
	if (accept( ((StMuTrackFourVec*) *i) )  ) {
	    ++nAcceptedTracks;
	    mFourList.push_back(*i) ; //for ownership
	    StProtoJet tempPj(*i);
	    //This is a dangerous call!!!  Commented out, MLM 6/04
	    //if (accept(tempPj)) { //check to make sure that nothing is wrong w/ protojet, as well!!!
	    ++nAcceptedProtoJets;
	    mProtoJets.push_back( tempPj ); //for jet finding
	    //}
	}
    }
    cout <<"\tnTracks:\t"<<nTracks<<"\tnAcceptedTracks:\t"<<nAcceptedTracks
	 <<"\tnAcceptedProtoJets:\t"<<nAcceptedProtoJets<<endl;
}

void StppJetAnalyzer::setFourVec(FourList &tracks)
{
    mFourList.clear();
    mProtoJets.clear();

    fillLists(tracks);
}

void StppJetAnalyzer::findJets()
{
    cout <<"StppJetAnalyzer::findJets().  Clustering will begin with:\t"<<mProtoJets.size()<<"\tprotoJets"<<endl;
    clock_t start = clock();
    mFinder->findJets(mProtoJets);
    clock_t stop = clock();
    double time = (double)(stop-start)/(double)(CLOCKS_PER_SEC);
    cout <<"\ttime to find jets:\t"<<time<<endl;
    acceptJets();
}

void StppJetAnalyzer::addBranch(const char *name, void *stppudst) 
{
    TTree* ppuDst = (TTree *) stppudst;
    ppuDst->Branch (name, "StJets", &muDstJets, 64000, 99);
}
