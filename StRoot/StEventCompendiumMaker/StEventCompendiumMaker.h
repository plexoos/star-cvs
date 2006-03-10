/*!
// $Id: StEventCompendiumMaker.h,v 1.2 2006/03/10 00:55:00 jeromel Exp $
//
// \class StEventCompendiumMaker
// \brief Class for Making a Compendium of the information in StEvent.
//
// The information of various tables filled into StEvent by StEventMaker 
// will not be propagated in the ITTF chain because these tables are
// missing.  For example, StEventSummary comes from the st_event_summary
// table, which is filled in St_dst_Maker with the dst_vertex and dst_track
// tables as input.  We need to substitute this in the ITTF chain with
// filling these directly.  The purpose of this package is then to
// collect together the code, in the form of small utility functions
// that will fill the relevant information into StEvent from these
// missing tables, which will run in bfc during production.
// As such, it doesn't store any information (no need to keep data members),
// it will put all the results of the functions into StEvent.
//
// \author Manuel Calderon de la Barca Sanchez
// \date   June 2004
//
// $Log $
*/

#ifndef STAR_StEventCompendiumMaker
#define STAR_StEventCompendiumMaker

#ifndef StMaker_H
#include "StMaker.h"
#endif
class StEventCompendiumMaker : public StMaker {

public: 
    StEventCompendiumMaker(const char *name="StEventCompendiumMaker");
    ~StEventCompendiumMaker();
    Int_t  Init();
    virtual void Clear(const char* opt="");
    Int_t  Make();
    Int_t  Finish();
    virtual const char *GetCVS() const {
      static const char cvs[]= "Tag $Name:  $ $Id: StEventCompendiumMaker.h,v 1.2 2006/03/10 00:55:00 jeromel Exp $ built __DATE__ __TIME__" ; 
      return cvs;
    }
private:
    ClassDef(StEventCompendiumMaker, 1)
    
};

#endif
