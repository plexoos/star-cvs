//
// $Id: StPreEclMaker.cxx,v 1.16 2001/07/13 23:47:07 suaide Exp $
//
// $Log: StPreEclMaker.cxx,v $
// Revision 1.16  2001/07/13 23:47:07  suaide
// small modification
//
// Revision 1.14  2001/05/02 15:28:19  suaide
// the default option is not to clear the old emc info. use it only
// if you want to re-run cluster finder
//
// Revision 1.13  2001/05/01 18:17:13  suaide
// modified to erase old EMC points and clusters if they exist, so it
// is possible to run cluster maker again with different thresholds
//
// Revision 1.12  2001/04/20 22:23:44  pavlinov
// Clean up
//
// Revision 1.11  2001/04/17 23:51:50  pavlinov
// Clean up before MDC4
//
// Revision 1.10  2001/02/06 18:23:58  pavlinov
// Changed the algorithm of finding of EMC's collection in StPreEclMaker
//
// Revision 1.9  2001/02/01 22:23:13  suaide
// Fixed some memory leaks
//
// Revision 1.8  2000/12/01 21:15:43  suaide
//
//
// Small fixes in StPreEclMaker::Make()
//       if some detector fails to find clusters it was aborting the chain
//
// Small fixes in StEmcPreClusterCollection::findClustersInModule()
// Small fixes in StEmcPreClusterCollection::testOnNeighbor()
//
// Small fixes in StEmcPreCluster::calcMeanAndRms()
//
// Revision 1.7  2000/09/08 22:55:06  suaide
//
//
//
// some modifications to compile on Solaris
//
// Revision 1.6  2000/09/08 21:48:00  suaide
//
//
// See README for details
//
// Revision 1.5  2000/08/24 22:11:35  suaide
//
//
// restored some files for background compatibility
//
// Revision 1.4  2000/08/24 19:45:37  suaide
//
//
// small modifications: some cout has been removed
//
// Revision 1.3  2000/08/24 11:26:48  suaide
//
//
//
// by A. A. P. Suaide - 2000/08/24 07:25:00
//
// Notes:
//
// 1. Full StEvent Compatible
// 2. Read hits from StEvent object
// 3. Write clusters in StEvent format and old format to keep background
//    compatibility
// 4. Do clustering in bemc, bprs, bsmde, bsmdp
// 5. Included method StPreEclMaker::SetClusterCollection
//
// Revision 1.2  2000/07/04 02:36:52  perev
// formal corrections, gStChain removed
//
// Revision 1.1  2000/05/15 21:24:01  subhasis
// initial version
//
//
// Authors: Alexandre A. P. Suaide (version 1.3)
//          Subhasis Chattopadhyay,
//          Aleksei Pavlinov , July 1999.
//          initial version from Akio Ogawa    
//    

//////////////////////////////////////////////////////////////////////////
//                                                             
// StPreEclMaker class  
//                                                             
//////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <cmath>
#include "StChain.h"
#include "TDataSetIter.h"
#include "StPreEclMaker.h"
#include "StEmcSimulatorMaker/StEmcSimulatorMaker.h"
#include "St_ems_Maker/St_ems_Maker.h"
#include "StEmcUtil/StEmcMath.h"
#include "tables/St_emcClusterParam_Table.h"

// added for StEvent

#include "StEvent/StEvent.h" 
#include "StEvent/StEventTypes.h"

ClassImp(StPreEclMaker)


int nModule[8] = {120, 120, 120, 120, 24, 24, 24, 24}; // temp.->database
#include "StEmcUtil/emcDetectorName.h"

emcClusterParam_st *parTable=0;
Bool_t        kCheckClustersOkConf[8];
StEmcCollection* ecmpreecl;

//_____________________________________________________________________________
StPreEclMaker::StPreEclMaker(const char *name, const char *title):StMaker(name,title){
//  drawinit=kFALSE;
}
//_____________________________________________________________________________
StPreEclMaker::~StPreEclMaker()
{
}
//_____________________________________________________________________________
Int_t StPreEclMaker::Init()
{
  
  doClearEmc=kFALSE;
  
  //Setting default cluster conditions ... for pi0 business
  mParam = new St_emcClusterParam("emcClustParam", BSMDP);
  AddRunco((TDataSet*)mParam);
  parTable = mParam->GetTable();

  SetClusterConditions("bemc",  4, 0.4, 0.001, 0.7,kFALSE);
  SetClusterConditions("bprs",  1, 0.1, 0.001, 0.1,kFALSE); // Cluster is one hit
  SetClusterConditions("bsmde", 5, 0.08,0.001, 0.4,kFALSE);
  SetClusterConditions("bsmdp", 5, 0.08,0.001, 0.4,kFALSE);
  mParam->Purge();
  parTable = mParam->GetTable();

  Int_t greta[4]={40,40,300,20};   // eta bins
  Int_t grphi[4]={120,120,60,900}; // phi bins  => 16-apr by PAI
  Float_t myPI = M_PI + 0.0001;
  
  //Making QA histgrams
  m_ncl  = new TH2F("Ncluster","Number of cluster(log) .vs. Detector #",40,0.0,4.0, 4,0.5,4.5);
  m_etot = new TH2F("Ecluster" ,"Total PreCluster Energy(log) .vs. Detector #", 60,-2.0,4.0, 4,0.5,4.5);
  Float_t rmsMax=0.026;
  Int_t   rmsN=52;
  m_sig_e= new TH2F("RMS(eta)" ,"Sigma(eta) .vs. Detector #",rmsN,0.0,rmsMax,4,0.5,4.5);
  m_sig_p= new TH2F("RMS(phi)" ,"Sigma(phi) .vs. Detector #",rmsN,0.0,rmsMax,4,0.5,4.5);
  for (Int_t i=0; i<4; i++)
  {
    TString name_h = detname[i] + "_cluster";
    TString name_e = detname[i] + "_cluster_energy";
    TString tit_h  = detname[i] + " cluster";
    TString tit_e  = detname[i] + " energy of cluster";
    if(i==2) {
      m_cl[i]     = new TH2F(name_h,tit_h,greta[i],-1.0,1.0,grphi[i],-M_PI*1.015, M_PI*0.985);
      m_energy[i] = new TH2F(name_e,tit_e,greta[i],-1.0,1.0,grphi[i],-M_PI*1.015, M_PI*0.985);
    }
    else{
      m_cl[i]     = new TH2F(name_h,tit_h,greta[i],-1.0,1.0,grphi[i],-myPI, myPI);
      m_energy[i] = new TH2F(name_e,tit_e,greta[i],-1.0,1.0,grphi[i],-myPI, myPI);
    }
    

    TString name_m  = detname[i] + "ClNum";
    TString tit_m   = "Number hits in cluster for " + detname[i];
    m_HitsInCl[i]   = new TH1F(name_m, tit_m, 21, -0.5, 20.5);

    TString name_en  = detname[i] + "ClEnergy";
    TString tit_en   = "Energy of cluster for " + detname[i];
    m_EnergyCl[i]    = new TH1F(name_en, tit_en, 2000, 0.0, 20.0);

    TString name_eta  = detname[i] + "Eta";
    TString tit_eta   = "Eta of clusters for " + detname[i];
    TString name_phi  = detname[i] + "Phi";
    TString tit_phi   = "Phi of clusters for " + detname[i];
    if(i==2) {
      TArrayD *xb  = StEmcMath::binForSmde();
      if(xb){
        m_EtaInCl[i] = new TH1F(name_eta, tit_eta, xb->GetSize()-1, xb->GetArray());
        delete xb;
      }
      m_PhiInCl[i]   = new TH1F(name_phi, tit_phi, grphi[i], -M_PI*1.015, M_PI*0.985);
    }
    else{ 
      m_EtaInCl[i]   = new TH1F(name_eta, tit_eta, greta[i], -1., 1.);
      m_PhiInCl[i]   = new TH1F(name_phi, tit_phi, grphi[i], -myPI, myPI);
    }
  }
  Histograms()->SetName("PreClustHist");  
  return StMaker::Init();
}  
//_____________________________________________________________________________
Int_t StPreEclMaker::Make()
{
  cout << "\n************************* Entering PreEclMaker Make() \n\n";
//
// This maker must be run after StEventMaker => StEvent must be !!!!!
//
   StEmcSimulatorMaker* simnew;
   St_ems_Maker* simold;
// First of all, try to get StEvent Pointer
  StEvent *currevent = (StEvent*)GetInputDS("StEvent");
  if(!currevent){
    cout << "***** Can not get StEvent pointer .. sorry \n";
    return kStErr;
  }
  else{
    cout << "***** StEvent pointer Ok\n";
    ecmpreecl = currevent->emcCollection();

    if(ecmpreecl == 0){
    // Try to get from simulator(s) ==============================================
      simnew = (StEmcSimulatorMaker*)GetMaker("emcRaw");
      if(simnew) {
        ecmpreecl = (StEmcCollection*)simnew->getEmcCollection();
        cout <<"***** New simulator in chain";
      }
      else {
        simold = (St_ems_Maker*)GetMaker("emc_raw");;
        if(simold){
          ecmpreecl = (StEmcCollection*)simold->getEmcCollection();
          cout <<"***** Old simulator in chain\n";
        }
      }
      if(ecmpreecl){ // Emc from simulator(s)  
        cout<<" => Get EmcCollection from simulator\n";
	currevent->setEmcCollection(ecmpreecl);
	if(simnew) simnew->clearStEventStaf();  // We move StEmcCollection from new simulator to StEvent 
      }
      else cout<<" => No EmcCollection from simulator\n";
    }
    else cout<<"***** Get EmcCollection from StEvent\n";

    if(ecmpreecl == 0) {
    // Try to get from calibration (will be define later)
    }

    if(ecmpreecl==0) return kStWarn;

  }
  
  if (doClearEmc) ClearEmc(); // clear old emc info
//
// At this point, StEmcCollection should be Ok.
//

// Starting clustering....

  for(Int_t i=0; i<4; i++)  // Loop over the BEMC detectors
  {  
    cout <<"***** Doing clustering on detector "<<detname[i].Data()<<"\n";
    StDetectorId id = static_cast<StDetectorId>(i+kBarrelEmcTowerId);
    StEmcDetector* mDet=ecmpreecl->detector(id); // getting detector pointer
    if(mDet)
    {
      StEmcPreClusterCollection* cc=new StEmcPreClusterCollection(detname[i].Data(),mDet);
      if(Debug()>=2) AddData(cc);  // 17-apr-2001 by PAI
      if(cc->IsOk())
      {
        cc->setSizeMax(Int_t(parTable[i].sizeMax));
        cc->setEnergySeed(Float_t(parTable[i].energySeed));
        cc->setEnergyAdd(Float_t(parTable[i].energyAdd));
        cc->setEnergyThresholdAll(Float_t(parTable[i].energyThreshold));
        cc->setCheckClusters(kCheckClustersOkConf[i]);
//      cc->printConf();
  	    if(cc->findClusters()!= kStOK) cout<<"***** ERR: StEmcClusterCollection: No hits\n";
        MakeHistograms(i,cc); // Fill QA histgrams
        fillStEvent(i,cc);
      }      
      if(Debug()<2) delete cc;
    }  
  }
  
  AddData(new St_ObjectSet("PreEclEmcCollection",ecmpreecl));  // for what ??
  cout <<"***** New EmcCollection on local .data\n";
    
  return kStOK;
}
//_____________________________________________________________________________
Int_t StPreEclMaker::ClearEmc()
{
  cout <<"Deleting old emc info ...\n";
  StSPtrVecEmcPoint& pvec = ecmpreecl->barrelPoints();
  if(pvec.size()>0) 
  {
    cout <<"Deleting old EMC points...\n"; 
    pvec.clear(); 
  }
 
  for(Int_t i=0; i<4; i++)
  {
    StDetectorId id = static_cast<StDetectorId>(i+kBarrelEmcTowerId);
    StEmcDetector* detector=ecmpreecl->detector(id);
    if(detector)
    {
      StSPtrVecEmcCluster& cluster=detector->cluster()->clusters();
      if(cluster.size()>0) 
      {
        cout <<"Deleting old clusters for "<<detname[i].Data()<<"\n";
        cluster.clear();  
      }
    }
  }
  return kStOk;
}
//_____________________________________________________________________________
void StPreEclMaker::MakeHistograms(Int_t idet,StEmcPreClusterCollection* cluster)
{

  Int_t n,det;
    if(cluster > 0)
    {
      n = cluster->Nclusters();
      Float_t Etot=0.0;
      if(n>0)
      {
	      det = cluster->Detector();
        m_ncl->Fill(log10((Float_t)n),(Float_t)det);

        TIter next(cluster->Clusters());
        StEmcPreCluster *cl;
        for(Int_t i=0; i<n; i++)
        {
          cl=(StEmcPreCluster*)next();
          Float_t eta=cl->Eta();
          Float_t phi=cl->Phi();
          Float_t energy=cl->Energy();
          Float_t sigmaeta=cl->SigmaEta();
          Float_t sigmaphi=cl->SigmaPhi();
          Int_t   nhits=cl->Nhits();

          if(sigmaeta > 0) m_sig_e->Fill(sigmaeta, Axis_t(det));          
          if(sigmaphi > 0.0) m_sig_p->Fill(sigmaphi, Axis_t(det));
          
          m_cl[det-1]->Fill(Axis_t(eta), Axis_t(phi));
          m_energy[det-1]->Fill(Axis_t(eta), Axis_t(phi), energy);
          m_HitsInCl[det-1]->Fill(Axis_t(nhits));
          m_EnergyCl[det-1]->Fill(Axis_t(energy));
          m_EtaInCl[det-1]->Fill(Axis_t(eta));
          m_PhiInCl[det-1]->Fill(Axis_t(phi));

          Etot+=energy;         
        }
        m_etot->Fill(log10(Etot), Axis_t(det));
      }
    }
  
}

//------------------------------------------------------------------------
Int_t StPreEclMaker::fillStEvent(Int_t idet,StEmcPreClusterCollection* cluster)
{
       
    if(cluster> 0)
    {
      StDetectorId id = static_cast<StDetectorId>(idet+kBarrelEmcTowerId); 
      StEmcDetector* detector = ecmpreecl->detector(id);           

      StEmcClusterCollection* cluscoll = new StEmcClusterCollection();
      cluscoll->setDetector(id);
      cluscoll->setClusterFinderId(1);
      cluscoll->setClusterFinderParamVersion(1); 

      detector->setCluster(cluscoll);
 
      Int_t n = cluster->Nclusters();
      if(n>0)
      {
        Int_t det = cluster->Detector(); 
        if(det){}
 
        TIter next(cluster->Clusters());
        StEmcPreCluster *cl;
        for(Int_t i=0; i<n; i++)
        {
          cl=(StEmcPreCluster*)next();
          Int_t module=cl->Module();
          StSPtrVecEmcRawHit& RawHit = detector->module((UInt_t)module)->hits();
          Float_t eta=cl->Eta();
          Float_t phi=cl->Phi();
          Float_t energy=cl->Energy();
          Float_t sigmaeta=cl->SigmaEta();
          Float_t sigmaphi=cl->SigmaPhi();
          StEmcCluster* clus = new StEmcCluster();
          clus->setEta(eta);
          clus->setPhi(phi);
          clus->setEnergy(energy);
          clus->setSigmaEta(sigmaeta);
          clus->setSigmaPhi(sigmaphi);
            
          for(Int_t kk=0;kk<cl->Nhits();kk++)
          {
            clus->addHit(RawHit[cl->ID(kk)]);
          }   
          
          cluscoll->addCluster(clus);
        } 
      } 
    } 
  return kStOK;
}                                                                               

void 
StPreEclMaker::SetClusterConditions(char *cdet,Int_t sizeMax,
                                            Float_t energySeed,
                                            Float_t energyAdd,
                                            Float_t energyThresholdAll)
{
  for(Int_t i=0;i<8;i++)
  {
    if(!strcmp(cdet,detname[i].Data()))
    {
      parTable[i].detector        = i + 1;
      parTable[i].sizeMax         = sizeMax;
      parTable[i].energySeed      = energySeed;
      parTable[i].energyAdd       = energyAdd;
      parTable[i].energyThreshold = energyThresholdAll;
      if(parTable[i].energyThreshold < parTable[i].energySeed) // check parameter adjustment 
      parTable[i].energyThreshold = parTable[i].energySeed;
      if(i <= mParam->GetNRows()) mParam->SetNRows(i+1);
      break;
    }
  }
}

void 
StPreEclMaker::SetClusterConditions(char *cdet,Int_t sizeMax,
                                            Float_t energySeed,
                                            Float_t energyAdd,
                                            Float_t energyThresholdAll,
                                            Bool_t  kCheckClustersOk)
{
  SetClusterConditions(cdet,sizeMax,energySeed,energyAdd,energyThresholdAll);
  for(Int_t i=0;i<8;i++)
  {
    if(!strcmp(cdet,detname[i].Data()))
    {
      kCheckClustersOkConf[i]=kCheckClustersOk;
      break;
    }
  }
}

void 
StPreEclMaker::PrintInfo(){
  printf("**************************************************************\n");
  printf("* $Id: StPreEclMaker.cxx,v 1.16 2001/07/13 23:47:07 suaide Exp $   \n");
  printf("**************************************************************\n");
  if (Debug()) StMaker::PrintInfo();
}

