// $Id: StPeCMaker.h,v 1.9 2000/12/22 22:38:17 akio Exp $
//
// $Log: StPeCMaker.h,v $
// Revision 1.9  2000/12/22 22:38:17  akio
// bug fix for solaris
//
// Revision 1.8  2000/12/13 00:08:59  akio
// Added trigger sim and histograms
//
// Revision 1.7  2000/04/21 19:09:42  nystrand
// Update StPeCPair class, new histograms
//
// Revision 1.6  2000/03/24 22:36:16  nystrand
// First version with StPeCEvent
//
// Revision 1.5  2000/01/20 23:03:15  nystrand
// First Version of StPeCMaker with new StEvent
//
// Revision 1.3  1999/07/15 13:57:21  perev
// cleanup
//
// Revision 1.2  1999/04/08 16:37:27  nystrand
// MakeBranch,SetBranch removed
//
// Revision 1.1  1999/04/06 20:47:35  akio
// The first version
//
// Revision 1.0  1999/03/05 11:00:00  Nystrand
// First Version
//
///////////////////////////////////////////////////////////////////////////////
//
// StPeCMaker
//
// Description: 
//  Sample maker to access and analyze Peripheral Collisions through StEvent
//
// Environment:
//  Software developed for the STAR Detector at Brookhaven National Laboratory
//
// Author List: 
//  Joakim Nystrand, LBNL
//
// History:
//
///////////////////////////////////////////////////////////////////////////////
#ifndef StPeCMaker_HH
#define StPeCMaker_HH
#include "StMaker.h"
#include "StPeCEvent.h"
#include "TH1.h"
#include "TH2.h"
#include "TFile.h"

class StEvent;
class StPeCEvent;
class StRun;
class TH1F;
class TH2F;

class StPeCMaker : public StMaker {

protected:
  TFile *m_outfile;
  TH1F *m_hstat;
  TH1F *m_hntrk; 
  TH1F *m_hnvtxtrk;
  TH1F *m_hnmwchts;
  TH1F *m_hnctbhts;
  TH1F *m_hsumq;
  TH1F *m_hsumpt;
  TH1F *m_hxvert;
  TH1F *m_hyvert;
  TH2F *m_hxyvert;
  TH1F *m_hzvert;
  TH1F *m_hminvpi;
  TH1F *m_hminvk;
  TH1F *m_hrappi;
  TH1F *m_hrapka;
  TH1F *m_hopnangle;
  TH1F *m_hcostheta;
  TH2F *m_hdedx;
  TH2F *m_hdedxpos;
  TH2F *m_hdedxneg;
  TH1F *m_ctbsingle;
  TH1F *m_ctbsum;
  TH1F *m_ctbsumped;
  TH1F *m_ctbtrg;
  TH1F *m_zdcwest;
  TH1F *m_zdceast;
  TH1F *m_zdcsum;
  TH2F *m_ctbvstrk;
  TH2F *m_ctbslat;

  TH1F *m_hnpair;   
  TH1F *m_hpairsumq;   
  TH1F *m_hpairoa;  
  TH1F *m_hpaircostpi; 
  TH1F *m_hpaircostk;  
  TH1F *m_hpairsumpt;  
  TH1F *m_hpairminvpi; 
  TH1F *m_hpairminvk;  
  TH2F *m_hpairdedx;
		
  TH1F *m_hcpairsumq; 
  TH1F *m_hlpairsumq; 

  TH1F *m_hcpairsumpt; 
  TH1F *m_hcpairminvpi;
  TH1F *m_hcpairminvk; 
  TH2F *m_hcpairdedx;
		
  TH1F *m_hrhoptall;  
		      
  TH1F *m_hmass;   
  TH2F *m_hmasszdc;   
		      
  TH1F *m_hrhonpair;   
  TH1F *m_hrhoxvert;  
  TH1F *m_hrhoyvert;  
  TH1F *m_hrhozvert;  
  TH1F *m_hrhocost; 
  TH1F *m_hrhopt;     
  TH1F *m_hrhorapidity;     
  TH1F *m_hrhodndpt2;     
  TH2F *m_hrhodedx;
  TH1F *m_hrhozdcsum;
  TH2F *m_hrhozdcew;

  TH1F *m_hlowmasspt;       
  TH1F *m_hlowmasszdcsum;

  TH1F *m_hdedx1;
  TH1F *m_hrhodedx1;
  TH2F *m_hrhodedx2;
  TH1F *m_hlowmassdedx1;
  TH1F *m_rhozdcsum;

public:

  StPeCMaker(const Char_t *name="analysis");
  virtual ~StPeCMaker();
  //  virtual void Clear(Option_t *option="");
  virtual Int_t  Init();
  virtual Int_t  Make();
  virtual Int_t  Finish();

private:

  Int_t FillStPeCEvent(StEvent *event, StPeCEvent *pevent);
  Int_t Cuts(StEvent *event, StPeCEvent *pevent);
  Int_t FillHistograms(StPeCEvent *pevent);
  Int_t ExampleAnalysis(StPeCEvent *pevent);
  Int_t triggerSim(StEvent *);

  virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StPeCMaker.h,v 1.9 2000/12/22 22:38:17 akio Exp $ built "__DATE__" "__TIME__ ; return cvs;}

  ClassDef(StPeCMaker, 1)
};

#endif



