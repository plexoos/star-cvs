#ifndef __StarPythia8Decayer_h__
#define __StarPythia8Decayer_h__

#include "TVirtualMCDecayer.h"
#include "TClonesArray.h"
#include "Pythia.h"

class TLorentzVector;

class StarPythia8Decayer : public TVirtualMCDecayer
{
public:

  /// Initializes the decayer
  void Init();
  /// Decays the particle specified by PDG id and lorentz vector 
  void Decay( int pdg, TLorentzVector *p=0 );
  /// Returns the decay products in a TClonesArray of TParticle
  int ImportParticles( TClonesArray *array = 0 );
  /// 
  void SetForceDecay( int type );
  /// 
  void ForceDecay();
  /// Return the branching ratio for the spdcified PDG ID
  float GetPartialBranchingRatio( int pdgid );
  /// Return teh lifetime in seconds for the specified particle
  float GetLifetime( int pdgid );
  ///
  void ReadDecayTable();
  /// Add a particle with specified PDG ID to the stack
  void AppendParticle( int pdgid, TLorentzVector *p=0 );
  /// Clear the event
  void ClearEvent();
  
  /// Set the debug level
  void SetDebug( int dbg=1 ){ mDebug = dbg; }

  /// Modify pythia8 behavior
  void Set( const char *cmd ){ mPythia->readString(cmd); }

  /// Class constructor
  StarPythia8Decayer( Pythia8::Pythia *pythia = 0 );

  /// Class destructor
  ~StarPythia8Decayer();

private:
protected:

  Pythia8::Pythia *mPythia;
  Bool_t           mOwner;
  int            mDebug;

  ClassDef(StarPythia8Decayer,1);
};

#endif
