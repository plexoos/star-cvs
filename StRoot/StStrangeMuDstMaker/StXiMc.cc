/***********************************************************************
 *
 * $Id: StXiMc.cc,v 3.0 2000/07/14 12:56:50 genevb Exp $
 * $Log: StXiMc.cc,v $
 * Revision 3.0  2000/07/14 12:56:50  genevb
 * Revision 3 has event multiplicities and dedx information for vertex tracks
 *
 * Revision 2.0  2000/06/05 05:19:46  genevb
 * New version of Strangeness micro DST package
 *
 *
 ***********************************************************************
 *
 * Description: Monte Carlo Xi micro dst class
 *
 ***********************************************************************/
#include "StXiMc.hh"

ClassImp(StXiMc)

StXiMc::StXiMc() : StODMc()
{}
  
StXiMc::StXiMc(StMcVertex* mcVertex, StMcTrack* mcDaughterTrack) :
         StODMc(mcVertex, mcDaughterTrack)
{}

StXiMc::~StXiMc()
{}
  
  
