/***************************************************************************
 *
 * $Id: mikesEventCut.cxx,v 1.5 1999/10/15 01:57:03 lisa Exp $
 *
 * Author: Mike Lisa, Ohio State, lisa@mps.ohio-state.edu
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: StHbtMaker package
 *   A simple event-wise cut that selects on multiplicity and z-position
 *   of primary vertex           
 *
 ***************************************************************************
 *
 * $Log: mikesEventCut.cxx,v $
 * Revision 1.5  1999/10/15 01:57:03  lisa
 * Important enhancement of StHbtMaker - implement Franks CutMonitors
 * ----------------------------------------------------------
 * This means 3 new files in Infrastructure area (CutMonitor),
 * several specific CutMonitor classes in the Cut area
 * and a new base class in the Base area (StHbtCutMonitor).
 * This means also changing all Cut Base class header files from .hh to .h
 * so we have access to CutMonitor methods from Cint command line.
 * This last means
 * 1) files which include these header files are slightly modified
 * 2) a side benefit: the TrackCuts and V0Cuts no longer need
 * a SetMass() implementation in each Cut class, which was stupid.
 * Also:
 * -----
 * Include Franks StHbtAssociationReader
 * ** None of these changes should affect any user **
 *
 * Revision 1.4  1999/07/24 16:24:20  lisa
 * adapt StHbtMaker to dev version of library - solaris still gives problems with strings
 *
 * Revision 1.3  1999/07/19 14:24:04  hardtke
 * modifications to implement uDST
 *
 * Revision 1.2  1999/07/06 22:33:21  lisa
 * Adjusted all to work in pro and new - dev itself is broken
 *
 * Revision 1.1.1.1  1999/06/29 16:02:56  lisa
 * Installation of StHbtMaker
 *
 **************************************************************************/

#include "StHbtMaker/Cut/mikesEventCut.h"
#include <cstdio>

ClassImp(mikesEventCut)

mikesEventCut::mikesEventCut(){
  mNEventsPassed =  mNEventsFailed = 0;
}
//------------------------------
//mikesEventCut::~mikesEventCut(){
//  /* noop */
//}
//------------------------------
bool mikesEventCut::Pass(const StHbtEvent* event){
  int mult =  event->NumberOfTracks();
  double VertexZPos = event->PrimVertPos().z();
  cout << "mikesEventCut:: mult:       " << mEventMult[0] << " < " << mult << " < " << mEventMult[1] << endl;
  cout << "mikesEventCut:: VertexZPos: " << mVertZPos[0] << " < " << VertexZPos << " < " << mVertZPos[1] << endl;
  bool goodEvent =
    ((mult > mEventMult[0]) && 
     (mult < mEventMult[1]) && 
     (VertexZPos > mVertZPos[0]) &&
     (VertexZPos < mVertZPos[1]));
  goodEvent ? mNEventsPassed++ : mNEventsFailed++ ;
  cout << "mikesEventCut:: return : " << goodEvent << endl;
  return (goodEvent);
}
//------------------------------
StHbtString mikesEventCut::Report(){
  string Stemp;
  char Ctemp[100];
  sprintf(Ctemp,"Multiplicity:\t %d-%d\n",mEventMult[0],mEventMult[1]);
  Stemp = Ctemp;
  sprintf(Ctemp,"Vertex Z-position:\t %E-%E\n",mVertZPos[0],mVertZPos[1]);
  Stemp += Ctemp;
  sprintf(Ctemp,"Number of events which passed:\t%ld  Number which failed:\t%ld\n",mNEventsPassed,mNEventsFailed);
  Stemp += Ctemp;
  StHbtString returnThis = Stemp;
  return returnThis;
}
