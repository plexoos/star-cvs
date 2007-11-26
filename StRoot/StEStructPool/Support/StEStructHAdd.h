/**********************************************************************
 *
 * $Id: StEStructHAdd.h,v 1.3 2007/11/26 20:07:17 prindle Exp $
 *
 * Author: Jeff Porter 
 *
 **********************************************************************
 *
 * Description: Simple helper class for adding hists from cut selectoins
  *
 ***********************************************************************/
#ifndef __STESTRUCTHADD_H
#define __STESTRUCTHADD_H


#include "TROOT.h"
class TFile;
class TH2;

class StEStructHAdd : public TObject {

 protected:

 public:

  StEStructHAdd(){};
  ~StEStructHAdd(){};

  void addCuts(const char* outfile, TFile * inFile,
               int* nlist, int num, int parentDist[][2], int nParentDist, int symmXX=0);
  void addCuts(const char* outfile, const char* infile,
               int* nlist, int num, int parentDist[][2], int nParentDist, int symmXX=0);
  void symmetrizeXX(TH2 *hist);
  void combineUS(TFile * modFile);


  ClassDef(StEStructHAdd,1)

};


#endif
/***********************************************************************
 *
 * $Log: StEStructHAdd.h,v $
 * Revision 1.3  2007/11/26 20:07:17  prindle
 * Modified to average \Delta\rho/sqrt(\rho) over z-bins (if more than one z-bin
 * present for given centrality. Note: I weight by number of tracks, not number of
 * pairs. This is important when we are also combining different centralities (which
 * I do by combining centrality tag with z-bin tag in macro/addCentralities.)
 *
 * Scale mixed histograms by number of events. Integral of \Delta\rho need not be 0.
 *
 * delete items that are created and valgrind complained were lost. (Not a big deal
 * since macro is run once)
 *
 * [Still need to commit StEStructHAdd.cxx which cvs complained that check-update failed.]
 *
 * Revision 1.2  2006/04/06 01:09:47  prindle
 *   Calculating pt for each cut bin caused changes in HAdd.
 * The splitting of +- into +- and -+ caused changes in Support.
 *
 * Revision 1.1  2004/07/01 00:37:17  porter
 * new code previously my StEStructHelper. Takes hists from correltation
 * pass and builds final ressults.  Also the StEStructHAdd.h is a simple
 * replacemnt for my sumyt.C macro which could be expanded later as needed.
 *
 *
 *
 *********************************************************************/

