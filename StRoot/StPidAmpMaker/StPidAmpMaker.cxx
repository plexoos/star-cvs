/***************************************************************************
 *
 * $Id: StPidAmpMaker.cxx,v 1.4 2000/04/11 15:45:25 aihong Exp $
 *
 * Author: Aihong Tang & Richard Witt (FORTRAN Version),Kent State U.
 *         Send questions to aihong@cnr.physics.kent.edu
 ***************************************************************************
 *
 * Description:part of StPidAmpMaker package
 *             StPidAmpMaker is a mediator between StEvent and StPidAmpManager
 ***************************************************************************
 *
 * $Log: StPidAmpMaker.cxx,v $
 * Revision 1.4  2000/04/11 15:45:25  aihong
 * change to adapt dividing trks by channel for faster filling
 *
 * Revision 1.3  2000/04/09 18:50:47  aihong
 * change Make() to read directly from dst tables instead of StEvent
 *
 * Revision 1.2  2000/04/09 16:36:43  aihong
 * change for adapting NHitDcaNet added
 *
 * Revision 1.1.1.1  2000/03/09 17:48:33  aihong
 * Installation of package
 *
 **************************************************************************/


#include "StPidAmpMaker/StPidAmpMaker.h"
#include "StChain.h"
#include "St_Table.h"
#include "St_DataSetIter.h"
#include "StMessMgr.h"

#include "StPidAmpMaker/Include/StPidAmpConst.hh"



void fillStPidAmpTrks(St_Table* theTrackTable, St_Table* theDedxTable, St_Table* theVertexTable, StPidAmpTrkVector* trks,TH3D* histo);
void readDataFromDisk(StPidAmpTrkVector* trks,TH3D* histo);//read trks from disk.
void writeTrks(St_Table* theTrackTable, St_Table* theDedxTable, St_Table* theVertexTable);//write trks to disk for quik reading.vi readDataFromDisk.

ClassImp(StPidAmpMaker)

StPidAmpMaker::StPidAmpMaker(const Char_t *name) : StMaker(name)
{
    theManager    =new StPidAmpManager(); 

    ampTrks     =new StPidAmpTrkVector();
    dependHisto =new TH3D("histo of dependencies","histo of dependencies",NBinNHits,0,NMaxHits,NBinPt,0,PtUpLimit,NBinX,0,XUpLimit);
    //                                                                    nhits,                pt,           x.

    mNHits4BG=0;
    theManager->passTrksAddress(ampTrks);

}

StPidAmpMaker::~StPidAmpMaker() { /* noop */ }

Int_t
StPidAmpMaker::Init()
{


  ampTrks->reserve(8000000);

  return StMaker::Init();
}

void
StPidAmpMaker::Clear(Option_t *opt)
{
    StMaker::Clear();
}

Int_t
StPidAmpMaker::Finish()
{
  // readDataFromDisk(ampTrks,dependHisto);

  //  dependHisto->Project3D("x")->Draw();

        

    //release unused space back to memory.
        StPidAmpTrkVector tmpVector=*ampTrks;
         ampTrks->swap(tmpVector);




     
    //run...
        if (theManager->netSets()->size()==0) 
        theManager->bookADefaultChannelCollection("BAR","B");


       theManager->process(dependHisto);
 //    dependHisto->Draw();

    return kStOK;
}

Int_t
StPidAmpMaker::Make()
{
  St_DataSet *dst_data = GetInputDS("dst");
  if (!dst_data) return 0;

  St_DataSetIter  local(dst_data);

  St_Table* globalTable   =0;
  St_Table* dst_dedxTable =0;
  St_Table* vertexTable   =0;

  globalTable   =(St_Table *)local["globtrk"];
  dst_dedxTable =(St_Table *)local["dst_dedx"];
  vertexTable   =(St_Table* )local["vertex"];

    // OK, we've got the tables. Pass them and process them.

     if (globalTable && dst_dedxTable && vertexTable) {
    fillStPidAmpTrks(globalTable, dst_dedxTable,vertexTable, ampTrks,dependHisto);
    //    writeTrks(globalTable, dst_dedxTable,vertexTable);
    return kStOK;
     } else return 0;

}

void
StPidAmpMaker::SetNHitsFilter2LastCollection(Int_t nhits){
       theManager->setNHits4BGNet(nhits);
}



void 
StPidAmpMaker::AddDefaultChannelCollection(TString fitOpt, TString drawOpt){
    theManager->bookADefaultChannelCollection(fitOpt,drawOpt);
    gMessMgr->Info()<<"a default ChannelCollection is registered in NetSet Store"<<endm;
}





void 
StPidAmpMaker::AddNHitsChannelCollection(Int_t x1, Int_t x2,TString fitOpt, TString drawOpt){
    theManager->bookADefaultChannelCollection(fitOpt,drawOpt);
    gMessMgr->Info()<<"ignored two inputs "<<x1<<" "<<x2<<endm;
    gMessMgr->Info()<<"two inputs is for default option, the default NetSet is registered in NetSet Store"<<endm;
}


void 
StPidAmpMaker::AddNHitsChannelCollection(Int_t x1, Int_t x2, Int_t x3,TString fitOpt, TString drawOpt){
    theManager->bookANHitsChannelCollection(x1,x2,x3,fitOpt,drawOpt);
    gMessMgr->Info()<<"a nhits("<<x1<<" "<<x2<<" "<<x3<<") NetSet is registered in NetSet Store "<<endm;
}

void 
StPidAmpMaker::AddNHitsChannelCollection(Int_t x1, Int_t x2,Int_t x3, Int_t x4,TString fitOpt, TString drawOpt){
    theManager->bookANHitsChannelCollection(x1,x2,x3,x4,fitOpt,drawOpt);
    gMessMgr->Info()<<"a nhits("<<x1<<" "<<x2<<" "<<x3<<" "<<x4<<") NetSet is registered in NetSet Store "<<endm;
}



void 
StPidAmpMaker::AddNHitsChannelCollection(Int_t x1, Int_t x2, Int_t x3, Int_t x4, Int_t x5,TString fitOpt, TString drawOpt){
    theManager->bookANHitsChannelCollection(x1,x2,x3,x4,x5,fitOpt,drawOpt);
    gMessMgr->Info()<<"a nhits("<<x1<<" "<<x2<<" "<<x3<<" "<<x4<<x5<<" "<<") NetSet is registered in NetSet Store "<<endm;
}


void 
StPidAmpMaker::AddNHitsDcaChannelCollection(Int_t x1, Int_t x2,TString fitOpt, TString drawOpt,Double_t d1, Double_t d2, Double_t d3){
    theManager->bookADefaultChannelCollection(fitOpt,drawOpt);
    gMessMgr->Info()<<"ignored two inputs "<<x1<<" "<<x2<<endm;
    gMessMgr->Info()<<"two inputs is for default option, the default NetSet is registered in NetSet Store"<<endm;
}


void 
StPidAmpMaker::AddNHitsDcaChannelCollection(Int_t x1, Int_t x2, Int_t x3,TString fitOpt, TString drawOpt,Double_t d1, Double_t d2,  Double_t d3){
    theManager->bookANHitsDcaChannelCollection(x1,x2,x3,fitOpt,drawOpt,d1,d2,d3);
    gMessMgr->Info()<<"a nhits("<<x1<<" "<<x2<<" "<<x3<<") dca("<<d1<<" "<<d2<<" "<<d3<<")  NetSet is registered in NetSet Store "<<endm;
}

void 
StPidAmpMaker::AddNHitsDcaChannelCollection(Int_t x1, Int_t x2,Int_t x3, Int_t x4,TString fitOpt, TString drawOpt,Double_t d1,  Double_t d2, Double_t d3){

    theManager->bookANHitsDcaChannelCollection(x1,x2,x3,x4,fitOpt,drawOpt,d1,d2,d3);
    gMessMgr->Info()<<"a nhits("<<x1<<" "<<x2<<" "<<x3<<" "<<x4<<") dca("<<d1<<" "<<d2<<" "<<d3<<") NetSet is registered in NetSet Store "<<endm;
}



void 
StPidAmpMaker::AddNHitsDcaChannelCollection(Int_t x1, Int_t x2, Int_t x3, Int_t x4, Int_t x5,TString fitOpt, TString drawOpt, Double_t d1,  Double_t d2, Double_t  d3){
    theManager->bookANHitsDcaChannelCollection(x1,x2,x3,x4,x5,fitOpt,drawOpt,d1,d2,d3);
    gMessMgr->Info()<<"a nhits("<<x1<<" "<<x2<<" "<<x3<<" "<<x4<<x5<<" "<<") dca("<<d1<<" "<<d2<<" "<<d3<<") NetSet is registered in NetSet Store "<<endm;
}



void 
StPidAmpMaker::AddPtChannelCollection(Double_t x1, Double_t x2,TString fitOpt, TString drawOpt){
    theManager->bookADefaultChannelCollection(fitOpt,drawOpt);
    gMessMgr->Info()<<"ignored two inputs "<<x1<<" "<<x2<<endm;
    gMessMgr->Info()<<"two inputs is for default option, the default NetSet is registered in NetSet Store"<<endm;
}


void 
StPidAmpMaker::AddPtChannelCollection(Double_t x1, Double_t x2, Double_t x3,TString fitOpt, TString drawOpt){
    theManager->bookAPtChannelCollection(x1,x2,x3,fitOpt,drawOpt);
    gMessMgr->Info()<<"a pt("<<x1<<" "<<x2<<" "<<x3<<") NetSet is registered in NetSet Store "<<endm;
}

void 
StPidAmpMaker::AddPtChannelCollection(Double_t x1, Double_t x2,Double_t x3, Double_t x4,TString fitOpt, TString drawOpt){
    theManager->bookAPtChannelCollection(x1,x2,x3,x4,fitOpt,drawOpt);
    gMessMgr->Info()<<"a pt("<<x1<<" "<<x2<<" "<<x3<<" "<<x4<<") NetSet is registered in NetSet Store "<<endm;
}

void 
StPidAmpMaker::AddPtChannelCollection(Double_t x1, Double_t x2, Double_t x3, Double_t x4, Double_t x5,TString fitOpt, TString drawOpt){
    theManager->bookAPtChannelCollection(x1,x2,x3,x4,x5,fitOpt,drawOpt);
    gMessMgr->Info()<<"a Pt("<<x1<<" "<<x2<<" "<<x3<<" "<<x4<<x5<<" "<<") NetSet is registered in NetSet Store "<<endm;
}

    

void 
StPidAmpMaker::AddPtNHitsChannelCollection(Int_t n, Int_t* nhitsAry,Int_t p, Double_t* ptAry,TString fitOpt, TString drawOpt){
  
    Int_t j;

    theManager->bookAPtNHitsChannelCollection(n, nhitsAry, p, ptAry,fitOpt,drawOpt);
    gMessMgr->Info()<<"a Pt( ";
    for ( j=0; j<n; j++) gMessMgr->Info()<<nhitsAry[j]<<" ";
    gMessMgr->Info()<<")&NHits( ";
    for ( j=0; j<p; j++) gMessMgr->Info()<<ptAry[j]<<" ";
    gMessMgr->Info()<<") is registered in NetSet Store "<<endm;
}



    



