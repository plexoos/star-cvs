// StPCollTagMaker class for PColl Reconstruction Tags                  //

#include "StPCollTagMaker.h"
#include "StChain.h"
#include "tables/St_PCollTag_Table.h"
#include "StEventTypes.h"
#include "Stypes.h"
#include "StMessMgr.h"
#include "tables/St_dst_TrgDet_Table.h"
#include "tables/St_dst_event_summary_Table.h"


ClassImp(StPCollTagMaker)

StPCollTagMaker::StPCollTagMaker(const char *name):StMaker(name){
 //  StPCollTagMaker constructor
}

StPCollTagMaker::~StPCollTagMaker(){
  // StPCollTagMaker destructor
}

Int_t StPCollTagMaker::Init(){
 //  Init - is a first method the top level StChain calls to initialize all its makers
   return StMaker::Init();
}

Int_t StPCollTagMaker::Make(){


 //  Make - this method is called in loop for each event

  // Find St_dst_TrgDet
    St_dst_TrgDet *TrgDet = (St_dst_TrgDet *) GetInputDS("dst/TrgDet");
  if(!TrgDet) return 0;
  dst_TrgDet_st *tt = (dst_TrgDet_st *) TrgDet->GetTable();

  // Create a data set and add the table to it.
  St_PCollTag *tagtab= new St_PCollTag("PCollTag",1); m_DataSet->Add(tagtab);
//VPunused  PCollTag_st *tagtab_st = (PCollTag_st *) tagtab->GetTable();  
 
  PCollTag_st row;
  //fill default values for the PColl Tags
  row.chargedMultiplicity = 0;
  row.numberOfPhotonClusters = 0;
  row.numberOfElectronsInEmc = 0;
  row.numberOfMwpcHits = 0;
  row.numberOfPrimaryTracks= 0;
  row.vectorSumOfPt = -9999.;
  row.totalCharge = -9999.;
  row.pseudorapidityOfEvent = -9999.0;
  row.primaryVertexX = -9999.;
  row.primaryVertexY = -9999.;
  row.primaryVertexZ = -9999.;
  row.zdc1Energy = -9999.0;
  row.zdc2Energy = -9999.0;

  tagtab->AddAt(&row,0);

 // Find St_dst_event_summary
   St_dst_event_summary *event_summary =  (St_dst_event_summary *) GetInputDS("dst/event_summary");
  if (!event_summary) return 0;
  dst_event_summary_st *event_summary_st =
    (dst_event_summary_st *)event_summary->GetTable();

  long   trk_tot =  0;
  if (event_summary_st) {
    trk_tot =   event_summary_st->glb_trk_tot;
    row.chargedMultiplicity = trk_tot;
  }

  StEvent* event = (StEvent *)GetInputDS("StEvent");
  if (!event) {
    tagtab->AddAt(&row,0);
    return kStErr;
  }

  StPrimaryVertex* vtx = event->primaryVertex();
  if(vtx) {
    row.primaryVertexX = vtx->position().x();
    row.primaryVertexY = vtx->position().y();
    row.primaryVertexZ = vtx->position().z();
  }

  if(tt) { 
  row.zdc1Energy = tt->adcZDCEast;
  row.zdc2Energy = tt->adcZDCWest;
  }
 
  Int_t  NPrimaryTracks=0;
  Float_t SumQ = 0.0;
  Float_t SumPx=0.0;
  Float_t SumPy=0.0;

  StSPtrVecTrackNode& exnode = event->trackNodes();
  Int_t node = exnode.size();
  for(Int_t i=0; i<node; i++){
    UInt_t nprim = exnode[i]->entries(primary);
    if(nprim == 1) {
      NPrimaryTracks++;
      StTrack *tp = exnode[i]->track(primary);
      float px = tp->geometry()->momentum().x();
      float py = tp->geometry()->momentum().y();
      SumPx = SumPx + px;
      SumPy = SumPy + py;
      SumQ = SumQ + tp->geometry()->charge();
    }
  } 
 
  row.numberOfPrimaryTracks=NPrimaryTracks;
  row.vectorSumOfPt = ::sqrt(SumPx*SumPx+SumPy*SumPy);
  tagtab->AddAt(&row,0);

 return kStOK;

}










