/*!
 * \class  StPrepEmbedMaker
 * \brief  Prepare GEANT Maker with input from embedding settings and DAQ event
 * \author A.Rose, Y.Fisyak
 * \date   May 2007
 *
 * This maker prepares the GEANT Maker (St_geant_Maker) for running
 * in the embedding chain. There are several values which need to be
 * set so that the data and simulation have similar environments,
 * including magnetic field, detector geometry and primary vertex. It
 * does so by re-setting the default values for the GEANT maker,
 * St_geant_Maker, event by event.
 *
 * The Make method for this maker must be called in each event before
 * the Make method of the St_geant_Maker, or the simulated and real
 * event will not be appropriately matched.
 *
 * $Id: StPrepEmbedMaker.cxx,v 1.12 2010/02/05 23:01:04 andrewar Exp $
 *
 */

#include "TFile.h"
#include "StMessMgr.h"
#include "StPrepEmbedMaker.h"
#include "StEvtHddr.h"
#include "TTree.h"
#include "StGenericVertexMaker/StGenericVertexMaker.h"
#include "StGenericVertexMaker/StFixedVertexFinder.h"

#include "tables/St_vertexSeed_Table.h"
#include "TString.h"
#include "TSystem.h"

#include <unistd.h>

ClassImp(StPrepEmbedMaker)
struct embedSettings{
  Double_t mult;
  Int_t pid;
  Double_t ptlow;
  Double_t pthigh;
  Double_t etalow;
  Double_t etahigh;
  Double_t philow;
  Double_t phihigh;
  Double_t temperature;
  Int_t rnd1;
  Int_t rnd2;
  Double_t vzlow;
  Double_t vzhigh;
  Int_t NReqTrg;
  Int_t ReqTrgId[32];
  TString mode;
};

static embedSettings  *mSettings = 0;
//________________________________________________________________________________
StPrepEmbedMaker::StPrepEmbedMaker(const Char_t *name) : StMaker(name) {
  mEventCounter = 0;
  mGeant3=0;

  if( !mSettings ){
    mSettings = new embedSettings();
    mSettings->mult=0.;
  }
  mFile = 0;
  mTree = 0;
  mSkipMode = kTRUE;
}
//________________________________________________________________________________
StPrepEmbedMaker::~StPrepEmbedMaker() { 
  SafeDelete(mFile);
}
//________________________________________________________________________________
Int_t StPrepEmbedMaker::Init() {

	srand((unsigned)time(0));
        mSettings->rnd1 = abs(int(rand()*10000)+getpid());
        mSettings->rnd2 = abs(int(rand()*10000)+getpid());


    return StMaker::Init();
}

//----
Int_t StPrepEmbedMaker::InitRun(int runnum)
{
  //Call geant maker, set defaults
  if (! mGeant3) {
    mGeant3 = TGiant3::Geant3();
    if( ! mGeant3)
      {
	LOG_ERROR << "Geant3 pointer not found. exiting."<<endm;
	return kStErr;
      }
    if (mTagFile == "") {
      LOG_ERROR << "TagFile has not been defined" << endm;
      return kStErr;
    }
    mFile = new TFile(mTagFile);
    if (! mFile ) {
      LOG_ERROR << "TagFile : " << mTagFile << " cannot be opened" << endm;
      return kStErr;
    }
    mTree = (TTree *) mFile->Get("Tag");
    if (! mTree ) {
      LOG_ERROR << "In TagFile : " << mTagFile << " cannot find TTree \"Tag\"" << endm;
      return kStErr;
    }

    // Check mode type, select settings
   if (mSpreadMode){
         mMoreTagsFile = mTagFile;
         int indx1 = mMoreTagsFile.Index(".tags",0);
         int indx2 = mMoreTagsFile.Last('.');
         if (indx1!=indx2) mMoreTagsFile.Remove(indx1+1,(indx2-indx1));
         mMoreTagsFile.Insert(indx1+1,"moretags.");
         mMoreFile = new TFile(mMoreTagsFile);
         if (! mMoreFile ) {
            LOG_ERROR << "MoreTagsFile : " << mMoreTagsFile << " cannot be opened" << endm;
            return kStErr;
         }
         mMoreTree = (TTree *) mMoreFile->Get("MoreTags");
         if (! mMoreTree ) {
            LOG_ERROR << "In MoreTagsFile : " << mMoreTagsFile << " cannot find TTree \"MoreTags\"" << endm;
            return kStErr;
         }
    }

   
   if(mSettings->mode.Contains("Spectrum"))
     {

      // Call the old gentx binary for this request. 
      // We need to port this code into the StPrepEmbedMaker,
      // but the underlying functions don't exist in St_geant_Maker 
      // at this time (11-25-09).
      // magic numbers: 1000 events, temp.fz need to be resolved
       TString cmd;
       cmd = Form("root4star -q \'~starofl/embedding/getVerticiesFromTags.C\(1000,\"./\",\"%s\")\'", 
		  mTagFile.Data());
       cmd = Form("~starofl/embedding/GENTX/gentx %s %s %i %i %f %f %f %i %f %f %f %i",
		  mTagFile.Data(),"temp.fz",
		  1000,mSettings->mult,mSettings->etalow, mSettings->etahigh, 
		  0.0,0,mSettings->ptlow, mSettings->pthigh,mSettings->temperature,
		  mSettings->rnd1);
       gSystem->Exec(cmd.Data());


       Do("gfile p temp.fz");
     }//end if Spectrum

   else{
//    Do("detp  hadr_on");
 //   Do("make gstar");
       gSystem->Load("libgstar");
   

    
    Do("detp  hadr_on");
    TString cmd("rndm ");
    cmd+=mSettings->rnd1; cmd+=" "; cmd+=mSettings->rnd2;
    Do(cmd.Data());
    
    Do("vec/cr BUFF(1) r 0");
    Do("vec/cr BR2(6) R 100. 0. 0. 0. 0. 0.");
    Do("vec/cr MODE2(6) I 1412 0 0 0 0 0");
    Do("spart 995 'LAMBDASTAR' 3 1.5195  0  4.22e-23 BUFF 0 BR2 MODE2");
    Do("user/output o temp.fz");

   }// end default type selection (FlatPt)
  }//end if !mGeant3
  return 0;
}


//________________________________________________________________________________
Int_t StPrepEmbedMaker::Make() {
  mEventCounter++;  // increase counter
  StEvtHddr* EvtHddr = (StEvtHddr*) GetDataSet("EvtHddr");
  if (! EvtHddr) {
    LOG_ERROR << "StPrepEmbedMaker::Make EvtHddr has not been found" << endm;
    return kStErr;
  }
  Int_t nFound = mTree->Draw("uncorrectedNumberOfPrimaries:primaryVertexFlag",
			     Form("mRunNumber==%i&&mEventNumber==%i",EvtHddr->GetRunNumber(),EvtHddr->GetEventNumber()),
			     "goff");
  if (nFound != 1) {
    LOG_ERROR << "StPrepEmbedMaker::Make Run/Event = " << EvtHddr->GetRunNumber() << "/" << EvtHddr->GetEventNumber() 
	      << " has been found in tag file" << nFound << " times" <<  endm;
    return kStErr;
  }
  LOG_INFO << "StPrepEmbedMaker::Make Run/Event = " << EvtHddr->GetRunNumber()
	   << "/" << EvtHddr->GetEventNumber() 
	   << " has been found with uncorrectedNumberOfPrimaries = " <<  mTree->GetV1()[0] 
	   << " and primaryVertexFlag = " << mTree->GetV2()[0]  <<  endm;
   if (mTree->GetV1()[0] <= 0 || mTree->GetV2()[0] )
   {
     LOG_ERROR << "StPrepEmbedMaker::Make reject this event" << endm;
     return kStErr;
   }
 
  Int_t numberOfPrimaryTracks = (Int_t) mTree->GetV1()[0];
  // Extract info for mult for this event
  Int_t npart;
  if(mSettings->mult < 1.) 
    {
      npart=int(mSettings->mult * numberOfPrimaryTracks);
      if (! npart)
      {
	LOG_INFO << "StPrepEmbedMaker::Event " << EvtHddr->GetEventNumber() 
	      << " has too small numberOfPrimaryTracks " << numberOfPrimaryTracks << " for the mult fraction requested. Forcing npart to 1." << endm; 
	npart=1;
      }
  
    }
  else
    {
      npart = int (mSettings->mult);
    }


  nFound = (Int_t) mTree->Draw("primaryVertexX:primaryVertexY:primaryVertexZ",
			       Form("mRunNumber==%i&&mEventNumber==%i",
				    EvtHddr->GetRunNumber(),
				    EvtHddr->GetEventNumber()),
			       "goff");
  Double_t xyz[3] = {mTree->GetV1()[0],mTree->GetV2()[0],mTree->GetV3()[0]};
  // Skip event if no primary vertex - effectively if tags say it is 0,0,0
  if (mSkipMode == kTRUE){ 
    if (fabs(xyz[0])<1e-7 && fabs(xyz[1])<1e-7 && fabs(xyz[2])<1e-7 ){
      LOG_INFO << "StPrepEmbedMaker::Event " << EvtHddr->GetEventNumber()
	       << " has tags with vertex approx at (0,0,0) - probably no PV, skipping."
	       << endm;
      return kStSKIP;
    }
  }
  //Done set up for event.

  //Setup embedded particle
  // gkine      npart ID        PTLOW,   PTHIGH,   YLOW,   YHIGH,   PHILOW,   PHIHIGH,   ZLOW,   ZHIGH
  //make sure zlow!=zhigh in particle definition - not sure of result. 
  //Z vertex will be forced in vxyz statement.

       Double_t xyzerr[3] = {0.,0.,0.};
          Double_t vzlow = xyz[2];
          Double_t vzhigh = xyz[2];
   
  if(mSettings->mode.Contains("strange"))
    {
     //get primary vertex errors from moretags.root
     nFound = (Int_t) mMoreTree->Draw("VXERR:VYERR:VZERR",
             Form("RunId==%i&&EvtId==%i",
                  EvtHddr->GetRunNumber(),
                  EvtHddr->GetEventNumber()),
             "goff");
     if (nFound != 1) {
          LOG_ERROR << "StPrepEmbedMaker::Make Run/Event = " << EvtHddr->GetRunNumber() << "/" << EvtHddr->GetEventNumber()
             << " has been found in moretags file" << nFound << " times" <<  endm;
          return kStErr;
     }
     //xyzerr[0] = mMoreTree->GetV1()[0];
     //xyzerr[1] = mMoreTree->GetV2()[0];

     xyzerr[0] = 0;
     xyzerr[1] = 0;
     xyzerr[2] = mMoreTree->GetV3()[0];
     LOG_INFO << xyzerr[0] << " " << xyzerr[1] << " " << xyzerr[2] << endm;
     vzlow = -100.0;
     vzhigh = 100.0;

     //Set the vertex for StEvent with StGenericVertexMaker
     StGenericVertexMaker * vmaker = (StGenericVertexMaker*) GetMaker("GenericVertex");
     StFixedVertexFinder * vfinder = (StFixedVertexFinder *) vmaker->GetGenericFinder();
     vfinder->SetVertexPosition(xyz[0],xyz[1],xyz[2]);
  }



  TString cmd;
#if 1 
   cmd = Form("gkine %i %i %f %f %f %f %f %f %f %f;",
            npart, mSettings->pid,
            mSettings->ptlow, mSettings->pthigh,
            mSettings->etalow, mSettings->etahigh,
            mSettings->philow, mSettings->phihigh, xyz[2], xyz[2]);
            //mSettings->philow, mSettings->phihigh, vzlow, vzhigh);
   Do(cmd.Data());
#endif
  cmd = Form("phasespace %i %i %f %f %f %f;",
	     npart, mSettings->pid,
	     mSettings->ptlow, mSettings->pthigh,
	     mSettings->etalow, mSettings->etahigh);
  
  Do(cmd.Data());
  Do(Form("gvertex %f %f %f",xyz[0],xyz[1],xyz[2]));
  if( mSettings->mode.Contains("strange") )
    {
	Do(Form("gspread %f %f %f", xyzerr[0],xyzerr[1],xyzerr[2]));
    }
  else
    { 
 	Do("vsig 0 0;");
    }

  Do("trig 1");

  return kStOK;
}

Int_t StPrepEmbedMaker::Finish()
{
  TString cmd("user/output c temp.fz");
  Do(cmd.Data());
  return 0;
}


//_____________________________________________________________________________
void StPrepEmbedMaker::Do(const Char_t *job)
{  
  Int_t l=strlen(job);
  if (l) {
    LOG_INFO << "StPrepEmbedMaker::Do(" << job << ");" << endm;
    mGeant3->Kuexel(job);
  }
}
//________________________________________________________________________________
void StPrepEmbedMaker::SetPartOpt(Int_t pid, Double_t mult)  
{ 
  mSettings->mult=mult; mSettings->pid=pid; 
  LOG_INFO << "StPrepEmbedMaker::SetPartOpt mult = " << mSettings->mult
	   << " pid = " << mSettings->pid << endm;
}
//________________________________________________________________________________
void StPrepEmbedMaker::SetOpt(Double_t ptlow, Double_t pthigh,
			      Double_t etalow, Double_t etahigh, Double_t philow,
			      Double_t phihigh, TString type) {
  mSettings->ptlow=ptlow;   mSettings->pthigh=pthigh; 
  mSettings->etalow=etalow; mSettings->etahigh=etahigh;
  mSettings->philow=philow;  mSettings->phihigh=phihigh;
  mSettings->mode=type;
  LOG_INFO << "StPrepEmbedMaker::SetOpt ptlow = " << mSettings->ptlow << " pthigh = " << mSettings->pthigh
	   << " etalow = " << mSettings->etalow << " etahigh = " << mSettings->etahigh
	   << " philow = " << mSettings->philow << " phihigh = " << mSettings->phihigh
	   <<"Mode: "<< type.Data() << endm;
}
void StPrepEmbedMaker::SetTemp(double t)
{
  mSettings->temperature=t;
}
/* -------------------------------------------------------------------------
 * $Log: StPrepEmbedMaker.cxx,v $
 * Revision 1.12  2010/02/05 23:01:04  andrewar
 * Update with spectra embedding mode.
 *
 * Revision 1.11  2009/07/01 23:20:34  andrewar
 * Updated with codes for Strangeness embedding (taken from Xianglei's code,
 * Feb 09)
 *
 * Revision 1.9  2008/09/04 00:07:27  fisyak
 * Change default from gkine to phasespace
 *
 * Revision 1.8  2008/08/19 23:11:27  andrewar
 * Added initialization for RNDM seeds. Seeding now from the clock and the UNIX
 * process ID (as suggested by Marco).
 *
 * Revision 1.7  2008/08/15 15:09:37  lbarnby
 * Skip embedding events without primary vertex + flag for this behaviour (default is to skip)
 *
 * Revision 1.6  2008/07/30 14:11:55  lbarnby
 * Changed tags used to get multiplicity for calculating how many particle to embed from numberOfPrimaryTracks to uncorrectedNumberOfPrimaries because former is created by StPCollTagMaker which was not used in P08ic Au+Au 9 GeV production whereas latter is from StTagsMaker
 *
 * Revision 1.5  2008/06/17 16:08:56  fisyak
 * Move access to TGiant into InitRun
 *
 * Revision 1.4  2008/01/21 01:23:37  perev
 * WarnOff
 *
 * Revision 1.3  2007/09/18 21:53:45  fisyak
 * Don't use field set from GEANT
 *
 * Revision 1.2  2007/08/29 22:59:33  andrewar
 * Added some calls for GEANT simulation of embedded particles.
 *
 * Revision 1.1  2007/07/12 20:34:35  fisyak
 * Add StPrepEmbedMaker
 *
 *
 * -------------------------------------------------------------------------
 */

