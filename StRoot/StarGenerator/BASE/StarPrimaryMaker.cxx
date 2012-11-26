#include "StarPrimaryMaker.h"
ClassImp(StarPrimaryMaker);
//#include "StarGenerator.h"

#include "g2t/St_g2t_particle_Module.h"
#include "tables/St_g2t_event_Table.h"
#include "tables/St_g2t_gepart_Table.h"
#include "tables/St_g2t_vertex_Table.h"
#include "tables/St_g2t_event_Table.h"

#include "StarGenerator.h"
#include "StarCallf77.h" 
#include <iostream>
#include "St_geant_Maker/St_geant_Maker.h"
#include "TGiant3.h"
#include <map>
#include "TString.h"
#include "TSystem.h"

#include "StBFChain.h"
#include "TFile.h"
#include "TTree.h"
#include "TClass.h"


#include "StarGenerator/EVENT/StarGenEvent.h"
#include "StarGenerator/EVENT/StarGenParticle.h"
#include "AgStarReader.h"
#include "StarGenerator/UTIL/StarRandom.h"

#include "TMCProcess.h"

using namespace std;

StarPrimaryMaker *fgPrimary      = 0;
// --------------------------------------------------------------------------------------------------------------
StarPrimaryMaker::StarPrimaryMaker()  : 
  StMaker("PrimaryMaker"),
  mNumParticles(0),
  mTree(0),
  mFile(0),
  mTreeName("none"),
  mFileName("none"),
  mStack(0),
  mPrimaryEvent(0),
  mVx(0), mVy(0), mVz(0), mSx(0.1), mSy(0.1), mSz(30.0), mRho(0),
  mPtMin(0), mPtMax(-1), mRapidityMin(0), mRapidityMax(-1), mPhiMin(0), mPhiMax(-1), mZMin(0), mZMax(-1)
{
  assert(fgPrimary == 0); // cannot create more than one primary generator
  fgPrimary = this;
  mStack = new StarParticleStack();
  AgStarReader::Instance().SetStack(mStack);

  // Register the particle database with this maker
  StarParticleData &pdb = StarParticleData::instance();
  Shunt( &pdb );

}
// --------------------------------------------------------------------------------------------------------------
StarPrimaryMaker::~StarPrimaryMaker()
{
  fgPrimary = 0;       // Cleanup
  if ( mStack )        delete mStack;
  if ( mFile )         delete mFile;
  /* deleting mTree and mPrimaryEvent cause seg violation here... why? */
}
// --------------------------------------------------------------------------------------------------------------
TParticlePDG *StarPrimaryMaker::pdg( Int_t id ){

  return StarParticleData::instance().GetParticle(id);

}
// --------------------------------------------------------------------------------------------------------------
Int_t StarPrimaryMaker::Init()
{
  
  //
  // Initialize all submakers first
  //
  Int_t result =  StMaker::Init();

  //
  // Intialize the TTree with one event branch for each sub generator
  // and one branch for the primary event
  //
  if ( mFileName == "" ) {
    mFileName =   ((StBFChain*)StMaker::GetTopChain())->GetFileOut();
    mFileName.ReplaceAll(".root",".gener.root");
  }
  
  mFile = TFile::Open( mFileName, "recreate" );
  if ( !mFile ) result = (result<kStWarn)? kStWarn : result;

  mTree = new TTree( "genevents", "TTree containing event generator information" );
  


  mPrimaryEvent = new StarGenEvent("primaryEvent","Primary Event... particle-wise information from all event generators");
  mTree->Branch("primaryEvent","StarGenEvent",&mPrimaryEvent,64000,99);

  TIter Next( GetMakeList() );
  StarGenerator *generator = 0; 
  StMaker *_maker = 0;

  Int_t id = 0;

  while ( (_maker=(StMaker *)Next()) )
    {

      //
      // Dynamic cast to StarGenerator and punt if it's not one
      //
      generator = dynamic_cast<StarGenerator *>(_maker);
      if ( !generator ) 
	{
	  continue;
	}

      // Set ID of the event generator
      generator -> SetId( id++ );
      

      //
      // By default, connect generator to output tree.  It the IO mode
      // of the generator has been set, skip this step.
      //
      if ( generator->IOmode()==0 ) 
	{
	  generator -> SetOutputTree( mTree );
	}

    }

  return result;
}
// --------------------------------------------------------------------------------------------------------------
Int_t StarPrimaryMaker::Finish()
{
  if (mFile) 
    { 
      mFile -> Write();
      mFile -> Close();
    }
  else
    {
      Warning(GetName(),"Could not write to unopened file");
    }
  return StMaker::Finish();
}
// --------------------------------------------------------------------------------------------------------------
Int_t StarPrimaryMaker::Make()
{


  /// Iterate over all generators and execute  PreGenerate()
  PreGenerate();
  /// Iterate over all generators and execute     Generate()
  Generate();
  /// Iterate over all generators and execute PostGenerate()
  PostGenerate();

  /// Register g2t tables
  BuildTables();

  /// Finalize the event
  Finalize();

  /// Fill the tree
  mTree -> Fill();

  return kStOK;
}
// --------------------------------------------------------------------------------------------------------------
Int_t StarPrimaryMaker::InitRun( Int_t runnumber )
{
  mPrimaryEvent->SetRunNumber(runnumber);
  return StMaker::InitRun( runnumber );
}

// --------------------------------------------------------------------------------------------------------------
void StarPrimaryMaker::Clear( const Option_t *opts )
{
  mNumParticles = 0;
  mStack->Clear();
  mPrimaryEvent->Clear();
  StMaker::Clear(opts);
  TIter Next( GetMakeList() );
  StarGenerator *generator = 0;
  while ( (generator=(StarGenerator *)Next()) )
    {
      generator->Clear();
    }
}

// --------------------------------------------------------------------------------------------------------------
void StarPrimaryMaker::AddGenerator( StarGenerator *gener )
{
  static Int_t id = 0;
  gener->mId = ++id;
  AddMaker(gener);
}

// --------------------------------------------------------------------------------------------------------------
Int_t StarPrimaryMaker::PreGenerate()
{ 
  
  TIter Next( GetMakeList() );
  StarGenerator *generator = 0;
  while ( (generator=(StarGenerator *)Next()) )
    {
      generator -> PreGenerateHook();
    }
  
  return kStOK;
}
// --------------------------------------------------------------------------------------------------------------
Int_t StarPrimaryMaker::PostGenerate()
{ 
  
  TIter Next( GetMakeList() );
  StarGenerator *generator = 0;
  while ( (generator=(StarGenerator *)Next()) )
    {
      generator -> PostGenerateHook();
    }
  
  return kStOK;
}
// --------------------------------------------------------------------------------------------------------------
Int_t StarPrimaryMaker::Generate()
{
  TIter Next( GetMakeList() );
  StarGenerator *generator = 0; 
  while ( (generator=(StarGenerator *)Next()) )
    {

      // Generate the event with probability equal to the pileup
      // probability
      Double_t prob = generator -> GetPileup();
      if ( StarRandom::Instance().flat() < prob )
	generator -> Generate(); 

      // Accumulate total number of particles
      mNumParticles += generator->GetNumberOfParticles();

    }  
  return kStOK;
}
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
void StarPrimaryMaker::SetCuts( Double_t ptmin,  Double_t ptmax, 
				    Double_t ymin,   Double_t ymax,
				    Double_t phimin, Double_t phimax,
				    Double_t zmin,   Double_t zmax )
{
  mPtMin = ptmin; mPtMax = ptmax;
  mRapidityMin = ymin; mRapidityMax = ymax;
  mPhiMin = phimin; mPhiMax = phimax;
  mZMin = zmin; mZMax = zmax;
}
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
Bool_t StarPrimaryMaker::Simulate( StarGenParticle *particle )
{

  Bool_t go = particle->Simulate(); if (!go) return go;

  Double_t px = particle -> GetPx();
  Double_t py = particle -> GetPy();
  Double_t pz = particle -> GetPz();
  Double_t pt = TMath::Sqrt(px*px + py*py);
  TVector3 p( px, py, pz );

  // Check PT range
  if ( pt < mPtMin ) return false;
  if ( mPtMin < mPtMax )
    {
      if ( pt > mPtMax ) return false;
    }

  if ( pt > 0 )
    if ( mRapidityMin < mRapidityMax )
      {
	if ( p.Eta() < mRapidityMin ) return false;
	if ( p.Eta() > mRapidityMax ) return true;
      }

  // Extend this ... add phi and z-vertex range cuts.

  return true;

}
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
// --------------------------------------------------------------------------------------------------------------
Int_t StarPrimaryMaker::Finalize()
{

  TIter Next( GetMakeList() );
  StarGenerator *generator = 0; 
  Int_t offset = 0;

  //
  // Loop over all generators and call finalize on them.
  //
  while ( (generator=(StarGenerator *)Next()) )
    {
      generator -> Finalize();
    }
  Next.Reset();

  //
  // Retrieve the particle_st table
  //
  St_particle *table = (St_particle*) GetDataSet("particle");
  if ( !table ) 
    {
      //assert(!mNumParticles);
      Error(GetName(),"No particle table found, but we have particles.");
      return kStFatal;
    }

  //
  // Generate the primary vertex
  //
  TLorentzVector primary = Vertex(); 
  mPrimaryVertex = primary;



  //
  // Next loop over all generators and push their tracks onto the particle stack.
  // Add them to the local event record and establish the connection between the
  // particle index on the stack and the particle index in the event record.
  //
  Int_t nstack = 0;
  while ( (generator=(StarGenerator *)Next()) )
    {

      // Obtain the vertex for this event.  If the generator is marked as
      // pileup, sample a new vertex.  Otherwise use the primary vertex
      TLorentzVector vertex = (generator->IsPileup())?	Vertex() : primary;

      StarGenEvent *event = generator->Event();
      Int_t npart = event->GetNumberOfParticles();

      // Set the offset on the event
      event -> SetOffset( offset );

      // cout << "Pushing particles to particle stack" << endl;
      // event->Print();

      Int_t ntrack; // set by stack
      Int_t ndone = 0;

      for ( Int_t i=0; i<npart; i++ )
	{

	  StarGenParticle *particle = (*event)[i];
	  assert(particle);

	  Int_t    toDo = Simulate(particle); //particle->Simulate();

	  Double_t px   = particle->GetPx();
	  Double_t py   = particle->GetPy();
	  Double_t pz   = particle->GetPz();
	  Double_t E    = particle->GetEnergy();
	  Double_t M    = particle->GetMass();
	  Double_t vx   = particle->GetVx();
	  Double_t vy   = particle->GetVy();
	  Double_t vz   = particle->GetVz();
	  Double_t vt   = particle->GetTof();

	  /**
	   *
	   * NOTE: Insert rotation along the beamline constraint at this point
	   *
	   */

	  Double_t polx=0, poly=0, polz=0;
	  
	  Int_t    parent  = particle->GetFirstMother();
	  Int_t    parent2 = particle->GetLastMother();
	  Int_t    kid1    = particle->GetFirstDaughter();
	  Int_t    kid2    = particle->GetLastDaughter();
	  Int_t    id      = particle->GetId();
	  Int_t    status  = particle->GetStatus();
	  Double_t weight  = 1.0;

	  //
	  // Except for the first event, offset the particle's start vertex and update the
	  // particle ids
	  //
	  if ( i )
	    {
	      vx += vertex.X();
	      vy += vertex.Y();
	      vz += vertex.Z();
	      vt += vertex.T();
	      
	      parent += offset;
	      parent2 += offset;
	      kid1 += offset;
	      kid2 += offset;
	    }


	  // Add the track to the particle stack and register the
	  // track number with the particle
	  mStack -> PushTrack( toDo, parent, id, px, py, pz, E, vx, vy, vz, vt, polx, poly, polz, kPNoProcess, ntrack, weight, status );
	  //	  particle->SetStack( ntrack ); //?????

	  // Add the track to the g2t table iff it is pushed out to the simulator
	  if ( toDo ) table -> AddAt( particle->get_hepevt_address(), nstack );

	  // Add it to the primary event record
	  StarGenParticle *p = mPrimaryEvent->AddParticle( status, id, parent, parent2, kid1, kid2, px, py, pz, E, M, vx, vy, vz, vt );

	  // Associate this event record entry with the particle stack
	  p->SetIndex( particle -> GetIndex() );
	  // Set the primary key
	  p->SetPrimaryKey( i + offset );
	  
	  // If the particle is to be simulated, increment the stack ID
	  if ( toDo ) p->SetStack(  1 + nstack );
	  else        p->SetStack( -1 );

	  if ( toDo ) { nstack++; ndone++; }

	}

      
      // Add the total number of particles 
      // (npart includes the 0th entry 
      // generator record).
      offset += npart;


      //
      // Clear the generator's particle record so that only one
      // such record appears for the user
      //
      event->Clear("part");

    }
  
  //  St_particle *particle_st = (St_particle *)GetData("particle");
  //  assert(particle_st); // Hey!  Did we create the particle DS?

  return kStOK;
}

// --------------------------------------------------------------------------------------------------------------
TLorentzVector StarPrimaryMaker::Vertex()
{  
  Double_t x=0,y=0,z=0,t=0;

  TVector2 xy = StarRandom::Instance().gauss2d( mSx, mSy, mRho );
  x = mVx + xy.X();
  y = mVy + xy.Y();
  z = mVz + StarRandom::Instance().gauss( mSz );
  Double_t dist = TMath::Sqrt(x*x+y*y+z*z);
  t = dist / TMath::Ccgs();

  return TLorentzVector(x,y,z,t);
}
// --------------------------------------------------------------------------------------------------------------
void StarPrimaryMaker::BuildTables()
{

  // Loop over all generators and total the number of particles
  TIter Next( GetMakeList() );
  StarGenerator *generator = 0; 
  Int_t sum = 0;

  while ( (generator=(StarGenerator *)Next()) )
    {
      sum += generator -> Event() -> GetNumberOfParticles();
    }
  mNumParticles = sum;

  // Create g2t_event and g2t_particle tables for geant maker.  
  St_g2t_event    *g2t_event    = new St_g2t_event( "event", 1 );
  m_DataSet -> Add( g2t_event );
  if ( mNumParticles ) {
    St_particle *g2t_particle = new St_particle( "particle", mNumParticles );
    m_DataSet -> Add( g2t_particle );
  }
  // note: m_DataSet owns the new object(s) and is responsible for cleanup
   
}
// --------------------------------------------------------------------------------------------------------------
