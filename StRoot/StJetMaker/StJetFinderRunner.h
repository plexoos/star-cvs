// -*- mode: c++;-*-
// $Id: StJetFinderRunner.h,v 1.6 2008/05/02 17:07:11 tai Exp $
#ifndef STJETFINDERRUNNER_HH
#define STJETFINDERRUNNER_HH

#include <StJetFinder/StProtoJet.h>

#include <list>

class StJetPars;
class StJetFinder;

namespace StSpinJet {

class StJetFinderRunner {

public:

  typedef std::list<StProtoJet> ProtoJetList;
  typedef std::vector<const AbstractFourVec*> ParticleList;

  StJetFinderRunner(StJetPars* jp, ParticleList& particleList, ProtoJetList& protoJets);

  virtual ~StJetFinderRunner();

  void Init();

  void Run();

private:

  StJetFinder* _jetFinder;

  ParticleList& _particleList;

  ProtoJetList& _protoJetList;

};

}

#endif // STJETFINDERRUNNER_HH

