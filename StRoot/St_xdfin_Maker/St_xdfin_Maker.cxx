//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_xdfin_Maker class for Makers                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#include <iostream.h>
#include "StChain.h"
#include "St_DataSetIter.h"
#include "St_xdfin_Maker.h"
#include "St_particle_Table.h"
#include "St_XDFFile.h"

ClassImp(St_xdfin_Maker)

//_____________________________________________________________________________
St_xdfin_Maker::St_xdfin_Maker(const char *name, const char *title):StMaker(name,title){}
//_____________________________________________________________________________
St_xdfin_Maker::~St_xdfin_Maker(){
  m_Init_Done = kFALSE;
}
//_____________________________________________________________________________
Int_t St_xdfin_Maker::Init(){
// Get run parameters from input file
  Int_t res;
  while (! m_Init_Done) res = St_xdfin_Maker::Make();
  return res;
}
//_____________________________________________________________________________
Int_t St_xdfin_Maker::Make(){
  PrintInfo();
  St_DataSet *set;
  if (!m_DataSet) m_DataSet = new St_DataSet(GetName()); 
  St_DataSetIter local(m_DataSet);
  Int_t  ret = kStErr;
  while(set = gStChain->XDFFile()->NextEventGet()) {
    if (set) {
      if (strcmp(set->GetName(),"run")==0){ 
        local.Mkdir("run");
        St_DataSet *run = local("run");
        run->Update(set); SafeDelete(set);
        run->Purge();
        m_Init_Done = kTRUE;
        ret = kStOK;
        break;  
      }
      if (!m_Init_Done && strcmp(set->GetName(),"dst")==0){ // run summary
        local.Mkdir("run/dst");
        St_DataSet *dst = local("run/dst");
        dst->Update(set); SafeDelete(set);
        dst->Purge();
        m_Init_Done = kTRUE;
        ret = kStOK;
        break;  
      }
      if (strcmp(set->GetName(),"dst") == 0){// dst
        local.Mkdir("event/data/global/dst");
        St_DataSet *topset = local("event/data/global/dst");
        topset->Update(set); SafeDelete(set);
        topset->Purge();
        ret = kStOK;
        break;
      }
      if (strcmp(set->GetName(),"Run")==0){
        local.Mkdir("run/geant/Run");
        St_DataSet *geant = local("run/geant/Run");
        geant->Update(set); SafeDelete(set);
        geant->Purge();
        m_Init_Done = kTRUE;
        ret = kStOK;
        break;
      } 
      if (strcmp(set->GetName(),"BEGIN_RUN")==0){
	cout << "St_xdfin_Maker::Init found BEGIN_RUN dataset" << endl;
        StMaker *params = gStChain->Maker("params");
        if (! params) {
	  cout << "St_params_Maker has not been created. No Place to put BEGIN_RUN data" << endl;
	  continue;
        }
	const Char_t *maker_title = gStChain->Maker("params")->GetTitle();
        if (maker_title) {
	  local.Mkdir(maker_title);
	  local.Cd(maker_title);
	}
	St_DataSet *tpc = local("tpc");
	if (!tpc) tpc = local.Mkdir("tpc");
        local.Cd("tpc");
        St_DataSet *BEGIN_RUN = local("BEGIN_RUN");
        if (! BEGIN_RUN) BEGIN_RUN = local.Mkdir("BEGIN_RUN");
        BEGIN_RUN->Update(set);
        SafeDelete(set);
        m_Init_Done = kTRUE;
        ret = kStOK;
        break;
      }   
      if (strcmp(set->GetName(),"TPC_DATA")==0){
        StMaker *tpc_raw = gStChain->Maker("tpc_raw");
        if (! tpc_raw) {
	  cout << "tpc_raw Maker has not been created. No Place to put TPC_DATA data" << endl;
	  continue;
        }
	const Char_t *maker_title = gStChain->Maker("tpc_raw")->GetTitle();
        if (maker_title) {
	  local.Mkdir(maker_title);
	  local.Cd(maker_title);
	}
        St_DataSet *TPC_DATA = local("TPC_DATA");
        if (! TPC_DATA) TPC_DATA = local.Mkdir("TPC_DATA");
        TPC_DATA->Update(set);
        SafeDelete(set);
        ret = kStOK;
        break;
      }   
     // Drop any ROSIE_RESET or SLOW_CONTROL records.
      if (strcmp(set->GetName(),"ROSIE_RESET")==0){
        SafeDelete(set);
        cout << " St_xdfin_Maker::Make drop ROSIE_RESET dataset" <<endl;
	continue;
      }
      if (strcmp(set->GetName(),"SLOW_CONTROL")==0){
          SafeDelete(set);
	  cout << " St_xdfin_Maker::Make drop SLOW_CONTROL dataset" <<endl;
	  continue;
      }
      if (strcmp(set->GetName(),"event") == 0){// full event
        local.Mkdir("event"); 
        St_DataSet *event = local("event");
        event->Update(set); SafeDelete(set);
        event->Purge();
        ret = kStOK;
        break;
      }
      if (strcmp(set->GetName(),"Event") == 0){// GEANT event
        local.Mkdir("event/geant/Event");
        St_DataSet *topset = local("event/geant/Event");
        topset->Update(set); SafeDelete(set);
        topset->Purge();
        ret = kStOK;
        break;
      }
      m_DataSet->Shunt(set);
      m_DataSet->Purge();
      ret = kStOK;
      break;
#if 0
      const Char_t *makertype = GetTitle();
      if (makertype && strlen(makertype)) {
        local.Mkdir(makertype);
        St_DataSet       *topset = local(makertype);
        topset->Update(set); SafeDelete(set);
        topset->Purge();
        ret = kStOK;
        break;
      }
#endif
    }
  }
  Split();
  return ret;
}
//_____________________________________________________________________________
void St_xdfin_Maker::PrintInfo(){
  if (gStChain->Debug()) printf("St_xdfin_Maker\n"); //  %s %s \n",GetName(), GetTitle());
}
//_____________________________________________________________________________
void St_xdfin_Maker::Split(){
//   Loop on all makers and Split their data sets from xdfin 
  TList *m_Makers = gStChain->Makers();
  TIter next(m_Makers, kIterBackward);
  if (m_DataSet){
    St_DataSetIter local(m_DataSet); local.Cd(GetName());
    St_DataSet *makerset = 0;
    StMaker *maker;
    while (maker = (StMaker *) next()){
      if (maker != this) {
        const Char_t *makertype = maker->GetTitle();
        if (makertype && strlen(makertype)) {
          St_DataSet *set = local(makertype);
          if (set) {
            St_DataSet *Set = maker->DataSet();
            if (! Set) {
              Set = new St_DataSet(set->GetName());
              maker->SetDataSet(Set); 
            }
            Set->Update(set);
            St_DataSetIter ll(Set);
            ll.Du();
          }
        }
      } 
    }
    SafeDelete(m_DataSet);
  }
}
