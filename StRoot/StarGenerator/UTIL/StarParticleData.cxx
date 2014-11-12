#include "StarParticleData.h"
ClassImp(StarParticleData);

#include "TDatabasePDG.h"
#include "TCollection.h"
#include "THashList.h"
#include "assert.h"
#include <iostream>

using namespace std;

/// Helper function to define PDG ids for heavy ions
/// @param z Charge of the heavy ion
/// @param a Atomic number of the heavy ion
/// @param l Number of lambdas in a hypernucleus
Int_t hid( Int_t z, Int_t a, Int_t l=0 )
{
  //         10LZZZAAAI
  return (   1000000000
	 +     10000000*l					       
	 +        10000*z					
         +           10*a );
}


// Functor class which converts PDG code to STAR definition.
class StarTrackingCode { 
public: 
  virtual Int_t operator()(Int_t pdgcode)=0; 
};
class G3TrackingCode : public StarTrackingCode { 
public:
  virtual Int_t operator()( Int_t ipdg ){ return TDatabasePDG::Instance()->ConvertPdgToGeant3(ipdg); }
};



  StarParticleData StarParticleData::sInstance;
// ---------------------------------------------------------------------------------------------
StarParticleData::~StarParticleData()
{

}
// ---------------------------------------------------------------------------------------------
StarParticleData::StarParticleData( const Char_t *name, TDataSet *parent ) :
  TObjectSet(name)
{

  if ( parent )                Shunt(parent); 

  // Particle list will own the particles added to it, and is responsible for
  // cleaning up their memory.
  mParticleList.SetOwner();

  //
  // Intiailze the particle data from TDatabasePDG
  //
  TDatabasePDG *pdg = TDatabasePDG::Instance();
  pdg -> ReadPDGTable(); // because it's too much to expect from a singleton


  //
  // Iterate over all particles in the PDG database, and add them to the local list.
  // Set the tracking code according to the G3 standard.  TODO:  Allow user to select
  // differnt tracking code standard.
  //
  TIter Next( pdg->ParticleList() );

  G3TrackingCode G3ID;

  TParticlePDG *particle = 0;

  Int_t g3new = 100;
  while(  (particle=(TParticlePDG *)Next())  )
    {

      //
      // Clone the particle and add in the tracking code, which maps the PDG ID to the ID 
      // used in the simulation package.
      //
      TString   name   = particle->GetName();
      TString   title  = particle->GetTitle();
      Double_t  mass   = particle->Mass();
      Bool_t    stable = particle->Stable();
      Double_t  width  = particle->Width();
      Double_t  charge = particle->Charge();
      TString   class_ = particle->ParticleClass();

      Int_t   code = particle->PdgCode();
      Int_t   anti = 0; if ( particle->AntiParticle() == particle ) anti = -code;

      Int_t g3id = G3ID( code );
      
      TParticlePDG *myparticle = new TParticlePDG( name, title, mass, stable, width, charge, class_, code, anti, g3id );
      
      mParticleList. Add( myparticle );   
      mParticleNameMap[ name ] = myparticle;
      mParticleIdMap[ code ]   = myparticle; 
      mParticleG3IdMap[ g3id ] = myparticle;

    }
	

 
  //
  // Register the object array with the dataset
  //
  AddObject( &mParticleList, false );

  //
  // Create aliases for typical beam particles to handle differences
  // between event generator names and TDatabasePDG names... shouldn't
  // need e+ or pbar, but who knows...
  //
  AddAlias("electron",   "e-");
  AddAlias("positron",   "e+");
  AddAlias("p",    "proton");
  AddAlias("pbar", "antiproton");

  //
  // Next create typical heavy ions used in beams at RHIC... "hid" is defined
  // to help define PDG ids for the heavy ions
  //

  TParticlePDG *D     = new TParticlePDG( "D",     "Deuteron", /* mass */ 0.0,  true,  0., 1.0, "heavyion", hid(1,2,0),   0, 45 );
  TParticlePDG *He3   = new TParticlePDG( "He3",   "Helium-3", /* mass */ 0.0,  true,  0., 2.0, "heavyion", hid(2,1,0),   0, 49 );
  TParticlePDG *Cu    = new TParticlePDG( "Cu",    "Copper",   /* mass */ 0.0,  true,  0., 29,  "heavyion", hid(29,64,0),  0, 0 );
  TParticlePDG *Au    = new TParticlePDG( "Au",    "Gold",     /* mass */ 0.0,  true,  0., 79,  "heavyion", hid(79,197,0), 0, 0 );
  TParticlePDG *U     = new TParticlePDG( "U",     "Uranium",  /* mass */ 0.0,  true,  0., 92,  "heavyion", hid(92,238,0), 0, 0 );

  AddParticle("D",   D);
  AddParticle("He3", He3);
  AddParticle("Cu",  Cu);
  AddParticle("Au",  Au);
  AddParticle("U",   U);

  //
  // TODO: Add in the hypertriton and its antiparticles
  //

#undef hid

  

}
// ---------------------------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------------------------
TParticlePDG *StarParticleData::GetParticle( const Char_t *name )
{
  return mParticleNameMap[ name ];
}
// ---------------------------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------------------------
TParticlePDG *StarParticleData::GetParticle( const Int_t id )
{
  return mParticleIdMap[id];
}
TParticlePDG *StarParticleData::GetParticleG3( const Int_t id )
{
  return mParticleG3IdMap[id];
}
// ---------------------------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------------------------
void StarParticleData::AddParticle( const Char_t *name, TParticlePDG *particle )
{
  Int_t   code = particle->PdgCode();
  mParticleList.Add( particle );
  if ( mParticleNameMap[ name ] ) {    Warning( "AddParticle()", Form("Overwriting entry %s",name) );  }
  mParticleNameMap[ name ] = particle;
  if ( mParticleIdMap[ code ] ) {    Warning( "AddParticle()", Form("Overwriting entry %i",code) );  }
  mParticleIdMap[code]     = particle;
  G3TrackingCode G3ID; 
  mParticleG3IdMap[ G3ID(code) ] = particle;
  return;
}
// ---------------------------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------------------------
TParticlePDG *StarParticleData::AddParticle( const Char_t *name, const Char_t *title, Double_t mass, 
Bool_t stable, Double_t width, Double_t charge3, const char* particleClass, Int_t PdgCode, Int_t Anti, Int_t geantCode )
{
  // Create the particle
  TParticlePDG *part = new TParticlePDG(name,title,mass,stable,width,charge3,particleClass,PdgCode,Anti,geantCode); 
  // Register the particle
  AddParticle( name, part );
  // And return a pointer to it
  return part;
}
// ---------------------------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------------------------
void StarParticleData::AddAlias( const Char_t *alias, const Char_t *name )
{
  TParticlePDG *particle = GetParticle(name);
  AddParticle( alias, particle );
}
// ---------------------------------------------------------------------------------------------
//
// ---------------------------------------------------------------------------------------------
