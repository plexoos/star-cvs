/***************************************************************************
 *
 * $Id: RandEngine.cc,v 1.1 1999/01/30 03:58:59 fisyak Exp $
 *
 * Author: Gabriele Cosmo - Created: 5th September 1995
 *         modified for SCL bl
 ***************************************************************************
 *
 * Description:
 *             RandEngine.cc,v 1.6 1998/01/23 08:19:53
 * -----------------------------------------------------------------------
 *                             HEP Random
 *                         --- RandEngine ---
 *                      class implementation file
 * -----------------------------------------------------------------------
 * This file is part of Geant4 (simulation toolkit for HEP).
 *
 ***************************************************************************
 *
 * $Log: RandEngine.cc,v $
 * Revision 1.1  1999/01/30 03:58:59  fisyak
 * Root Version of StarClassLibrary
 *
 * Revision 1.1  1999/01/23 00:29:05  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include "RandEngine.h"
#include <stdlib.h>     // for RAND_MAX, tu


RandEngine::RandEngine(long seed) : mx(RAND_MAX)
{
   setSeed(seed,0);
   setSeeds(&theSeed,0);
   seq = 0;
}

RandEngine::~RandEngine() {}

RandEngine::RandEngine(const RandEngine &p) : mx(RAND_MAX)
{
  // This copy constructor uses "seq" to make the physical copy
  // of the object preserving its original status.

  if ((this != &p) && (&p)) {
    theSeed = p.theSeed;
    seq = 0;
    for (HepInt i=0; i<p.seq; ++i) flat();
    setSeeds(&theSeed,0);
  }
}

RandEngine & RandEngine::operator = (const RandEngine &p)
{
  // This operator uses "seq" to make the physical copy
  // of the object preserving its original status.

  if ((this != &p) && (&p)) {
    theSeed = p.theSeed;
    seq = 0;
    for (HepInt i=0; i<p.seq; ++i) flat();
    setSeeds(&theSeed,0);
  }
  return *this;
}

void RandEngine::setSeed(long seed, HepInt)
{
   theSeed = seed;
   srand( HepInt(seed) );
   seq = 0;
}

void RandEngine::setSeeds(const long* seeds, HepInt)
{
  setSeed(seeds ? *seeds : 19780503, 0);
  theSeeds = seeds;
}

void RandEngine::saveStatus() const
{
   ofstream outFile("Rand.conf", ios::out ) ;

   if (!outFile.bad()) {
     outFile << theSeed << endl;
     outFile << seq << endl;
   }
}

void RandEngine::restoreStatus()
{
   // The only way to restore the status of RandEngine is to
   // keep track of the number of shooted random sequences, reset
   // the engine and re-shoot them again. The Rand algorithm does
   // not provide any way of getting its internal status.

   ifstream inFile("Rand.conf", ios::in);
   long count;

   if (!inFile.bad() && !inFile.eof()) {
     inFile >> theSeed;
     inFile >> count;
     setSeed(theSeed,0);
     for (HepInt i=0; i<count; ++i) flat();
   }
}

void RandEngine::showStatus() const
{
   cout << endl;
   cout << "---------- Rand engine status ----------" << endl;
   cout << " Initial seed  = " << theSeed << endl;
   cout << " Shooted sequences = " << seq << endl;
   cout << "----------------------------------------" << endl;
}

HepDouble RandEngine::flat()
{
   register HepDouble num = 0.;

   while (num == 0.)
     num = rand()/(mx+1);
   seq++;
   return num;
}

void RandEngine::flatArray(const HepInt size, HepDouble* vect)
{
   register HepInt i;

   for (i=0; i<size; ++i)
     vect[i]=flat();
}

#ifndef ST_NO_TEMPLATE_DEF_ARGS
void RandEngine::flatArray(vector<HepDouble>& vec)
#else
void RandEngine::flatArray(vector<HepDouble,allocator<HepDouble> >& vec)
#endif
{
   register HepInt i;

   for (i=0; i<vec.size(); ++i)
     vec[i]=flat();
}
