//*-- Author :    Valery Fine(fine@bnl.gov)   11/07/99  
// $Id: StEventDisplayMaker.cxx,v 1.66 2000/08/16 20:50:55 fine Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StEventDisplayMaker class for Makers                                 //
//                                                                      //
//  To mix and display the "detector" object defined by  GEANT geometry //
//  and "event" object defined by TTable objects                        //
//                                                                      //
//  The "regular" ROOT viewers, namely TPad, X3D and OpenGL can be used //
//                                                                      //
//  Example provided by default filter (StVirtualEventFilter class):    //
//  begin_html <P ALIGN=CENTER> <IMG SRC="gif/EventDisplay.gif" width=100%> </P> end_html   //
//                                                                      //
// - "doEvents" - defines the "bfc" should make NO reconstruction       //
//                and get the information from the begin_html <a href="http://www.rhic.bnl.gov/STAR/html/comp_l/ofl/dst_table_model.html">DST</a> end_html file directly    //
// - "y1h geant" - define the source of the detector geometry           //
// - "noevent"   - defines no StEvent output is required                //
//                                                                      //
// - green dots represent the hits                                      //
//   from begin_html <a href="dst_point_st.html">dst/point</a> end_html  table with no track associated                      //
//                                                                      //
// - "colored" dots represent the hits associated with begin_html <a href="dst_track_st.html">dst/globtrk</a> end_html //
//   (The color is used to distinguish the hits of one track from others)//
//                                                                      //
// - "colored" lines represent the begin_html <a href="dst_track_st.html">dst/primtrk</a> end_html          //
//                                                                      //
// - "detector geometry" is defined by STAR geant definition (see: St_geant_Maker) //
//                                                                      //
// - the default 3D viewer is ROOT TPad object. That can be switched to either //
//   X3D or OpenGL view TPad "view" menu (see picture attached)         //
//                                                                      //
// - The custom view can be provided by user code as well. The user should provide 
//   his/her own selection with the selection class-filter.             //
//                                                                      //
//  Submit any problem with this code via begin_html <A HREF="http://www.star.bnl.gov/STARAFS/comp/sofi/bugs/send-pr.html"><B><I>"STAR Problem Report Form"</I></B></A> end_html   //
//                                                                      //
//                                                                      //
//             How to create the custom filter                          //
//                                                                      //
// 1. Choose name for your filter                                       //
// 2. Pick the pattern:                                                 //
//    cvs co StRoot/macros/graphics/StSectorHitFilter                   //
// 3. Copy it to your filter directory:                                 //
//    cp StRoot/macros/graphics/StSectorHitFilter/* StRoot/MyFilter     //
//    where "MyFilter" is the name of the filter you choose             //
// 3. Replace all "SectorHit" with the name of the filter you've chosen //
// 4. Edit St<your_name>Filer.cxx to introduce your own version of      //
//    St<your_name>EventFiler::Channel methods                          //
// 5. Create the "regular" STAR share library for yor filter class      //
// 6. Call TurnDisplay.C("MyFiler") to include StEventDisplayMaker with //
//    your custom filter in the "regular" chain: "bfc.C" or "doEvents"  //
// 7. Call .x PadControlPanel.C                                         //
//                                                                      //
// 8. Push either "ReDraw" to redraw the current event                  //
//                "NextEvent" to see next event                         //                                                                    //
//////////////////////////////////////////////////////////////////////////

#include "TROOT.h"
#include "TCanvas.h"
#include "TColor.h"
#include "TStyle.h"
#include "TTUBS.h"

#include "StEventDisplayMaker.h"
#include "StChain.h"
#include "TDataSetIter.h"
#include "TVolume.h"
#include "TVolumeView.h"
#include "TVolumePosition.h"
#include "St_PolyLine3D.h"
#include "TPoints3D.h"
#include "StHits3DPoints.h"
#include "StVertices3DPoints.h"
#include "StHelix3DPoints.h"
#include "TTable3Points.h"
#include "St_Table3PackedPoints.h"
#include "StVirtualEventFilter.h"
#include "TTable.h"
#include "TTableSorter.h"
#include "StArray.h"
#include "tables/St_tpt_track_Table.h"

// #include "StThreeVector.hh"
// #include "StHelixD.hh"
#include "StTrackChair.h"

// #include "StEvent.h"
#include <TCanvas.h>
#include <TGeometry.h>
#include <TWebFile.h>
#include <TBRIK.h>
#include <TH2.h>
#include <TMath.h>
#include <TObjString.h>

// StVirtualEventFilter hitsOffFilter(0);
// StVirtualEventFilter hitsOnFilter(1);
StVirtualEventFilter StEventDisplayMaker::m_DefaultFilters[StEventDisplayMaker::kEndOfEventList];

//_____________________________________________________________________________
//
//                         StEventDisplayMaker
//_____________________________________________________________________________

ClassImp(StEventDisplayMaker)

//_____________________________________________________________________________
StEventDisplayMaker::StEventDisplayMaker(const char *name):StMaker(name)
{
  m_Hall          =  0;  
  m_FullView      =  0;  
  m_ShortView     =  0; 
  m_Sensible      =  0; 
  m_EventsNode    =  0;

  m_HitCollector  = new TList;
  m_TrackCollector= new TList;
  m_TableCollector= new TList;

  m_PadBrowserCanvas = 0;

  m_ListDataSetNames = 0;

  m_FilterArray   = new TObjArray(kEndOfEventList);
  Int_t i; 
  for (i =0;i<kEndOfEventList;i++) {
   m_FilterArray->AddAt(&m_DefaultFilters[i],i);
//    m_FilterArray->AddAt(&hitsOffFilter,i);
  }
#ifdef STEVENT
  ((StVirtualEventFilter *)m_FilterArray->At(kVertices))->TurnOn();
  ((StVirtualEventFilter *)m_FilterArray->At(kGlobalTracks))->TurnOn();
  ((StVirtualEventFilter *)m_FilterArray->At(kTrack))->TurnOn();
#endif
  ((StVirtualEventFilter *)m_FilterArray->At(kTptTrack))->TurnOn();
  ((StVirtualEventFilter *)m_FilterArray->At(kTable))->TurnOn();

  gROOT->GetListOfBrowsables()->Add(this,GetName());
}
//_____________________________________________________________________________
StEventDisplayMaker::~StEventDisplayMaker(){
  gROOT->GetListOfBrowsables()->RecursiveRemove(this);
  ClearEvents();
  delete m_FilterArray;
  delete m_EventsNode;
  delete m_HitCollector;
  delete m_TrackCollector;
  delete m_TableCollector;
  if (m_ListDataSetNames) {
     m_ListDataSetNames->Delete();
     delete m_ListDataSetNames;
     m_ListDataSetNames = 0;
  }
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::Init(){
   // Create geometry 
   BuildGeometry();
     // Create a special node to keep "tracks" and "hits"
   CreateTrackNodes();
     // define the custom palette (may affect other pictures)
#ifdef PALETTE
   palette();    
#endif
   // Call the "standard" Init()
   
   return StMaker::Init();
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::BuildGeometry()
{
  // Create STAR sub-detector definition

  m_Hall = (TVolume *)GetDataSet("HALL");
  if (!m_Hall) return kStErr;
  //
  // Create an iterator to navigate STAR geometry
  TDataSetIter volume(m_Hall,0);
// ---  Create "standard" TPC and SVT views ----
  TVolume *sector = 0;
//  const Char_t *volueNames[] = {"TPSS","STSI"}; // STLI"};
//  const Char_t *volueNames[] = {"TPSS","STLI","ECAL","CALB"}; // STSI"};
  const Char_t *volueNames[] = {"TPSS","STLI","ECAL","CALB","BTOF"}; // STSI"};
  const Int_t lvolueNames = sizeof(volueNames)/sizeof(Char_t *);
  while ( (sector = ( TVolume *)volume()) ){
    Bool_t found = kFALSE;
    Int_t i;
    for (i =0; i < lvolueNames; i++) 
    if (strcmp(sector->GetName(),volueNames[i]) == 0 ) {found = kTRUE; break; }
    if (found) {
      sector->SetVisibility(TVolume::kBothVisible);
      sector->Mark();
      if (!i) {  // special case for TPSS sectors
        TTUBE *tubs = (TTUBE *)sector->GetShape();
        tubs->SetNumberOfDivisions(1);
      }
    } else { 
      sector->UnMark();
      sector->SetVisibility(TVolume::kThisUnvisible);
    }
  }
  m_Hall->SetVisibility(TVolume::kBothVisible);
  m_ShortView = new TVolumeView(*m_Hall,2); 
//  Begin_Html <P ALIGN=CENTER> <IMG SRC="gif/HitsDrawFullView.gif"> </P> End_Html // 
//    if (strcmp(tpssNode->GetName(),"TPGV") && strcmp(tpssNode->GetName(),"TPSS")) continue;
  return 0;
}

//______________________________________________________________________________
void StEventDisplayMaker::AddName(const Char_t *name)
{
  //  "name" - StEvent
  //  "g2t_tpc_hit(track_id,x[0]:x[1]:x[2])"
  //   Attention:     NO EXPRESSION, yet !!!

  if (!m_ListDataSetNames) m_ListDataSetNames = new TList;
  // check name
  TIter next(m_ListDataSetNames);
  TObjString *str = 0;
  while ((str = (TObjString *)next()) && str->String() != name);
  if (!str) m_ListDataSetNames->Add(new TObjString(name));
}
//______________________________________________________________________________
void StEventDisplayMaker::ClearEvents()
{
  if (m_Mode == 2) return;
  // Clear picture
  if (m_EventsNode) {
    if (!GetEventPad()) CreateCanvas();
    else                m_PadBrowserCanvas->Clear();

    delete m_EventsView;
    m_EventsView = 0;
    TVolume *node = 0;
    TDataSetIter nextNode(m_EventsNode);
    while ( (node = (TVolume *)nextNode() )) {
      TShape *shape = node->GetShape();
      if (shape) delete shape;   
      m_EventsNode->Delete();
      m_HitCollector->Delete();
      m_TrackCollector->Delete();
   }
  }
}
//_____________________________________________________________________________
void StEventDisplayMaker::ClearCanvas()
{
  // Clear canvas from the TBrowser Context Menu
  if (m_PadBrowserCanvas) {
    Clear();
    m_PadBrowserCanvas->Modified();
    m_PadBrowserCanvas->Update();
  }
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::CreateTrackNodes()
{
 //  Create TVolume dataset
  if (m_EventsNode) delete m_EventsNode;
  m_EventsNode  = new TVolume(".track",".track",(TShape *)0);
  m_EventsNode->Mark();
  m_EventsNode->SetVisibility();
  AddConst(m_EventsNode);
  return 0;
}
//______________________________________________________________________________
void StEventDisplayMaker::Clear(Option_t *)
{
  ClearEvents();
//  StMaker::Clear();
}

//_____________________________________________________________________________
TVirtualPad *StEventDisplayMaker::CreateCanvas()
{
  if (!GetEventPad()) {
   // Attention !!! The name of TCanvas MUST unique across all ROOT
   // objects otherwise those will be destroyed by TCanvas ctor !!!
     m_PadBrowserCanvas = new TCanvas("STARMonitor","Event Display",10,600,400,400);
  }
  if (m_ShortView) m_ShortView->Draw();
  m_PadBrowserCanvas->Modified();
  m_PadBrowserCanvas->Update();
  return m_PadBrowserCanvas;
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::MakeGlobalTracks()
{
  Int_t trackCounter  = 0;
  Int_t hitCounter    = 0;
#ifdef STEVENT
  const Int_t maxTrackCounter = 9999999;
  Size_t size;
  Style_t style;
  StTrackCollection *tracks = m_Event->trackCollection();
  if (tracks) {
     StGlobalTrackIterator next(tracks);
     StGlobalTrack *globTrack = 0;
     while ( ( globTrack = (StGlobalTrack *)next() ) && trackCounter < maxTrackCounter) {
        StVirtualEventFilter *filter = (StVirtualEventFilter *)m_FilterArray->At(kGlobalTracks);
        if (filter && (filter->IsOff() || ( filter->Channel(globTrack,size,style)==0 ) ) ) continue;
        // ------------------------   Tracks   ------------------------- //
        filter = (StVirtualEventFilter *)m_FilterArray->At(kTrack);
        if (!filter || filter->IsOn() ) 
              trackCounter +=  MakeTracks(globTrack,filter);

        // -------------------------   Hits  --------------------------- //
        filter = (StVirtualEventFilter *)m_FilterArray->At(kTrackTpcHits);
        if (!filter || filter->IsOn() ) {
           const StVecPtrTpcHit &hits   = globTrack->tpcHits();
           hitCounter += MakeHits(&hits,filter);
        }

        filter = (StVirtualEventFilter *)m_FilterArray->At(kTrackSvtHits);
        if (!filter || filter->IsOn() ) {
           const StVecPtrSvtHit &hits   = globTrack->svtHits();
           hitCounter += MakeHits(&hits,filter);
        }

        filter = (StVirtualEventFilter *)m_FilterArray->At(kTrackFtpcHits);
        if (!filter || filter->IsOn() ) {
           const StVecPtrFtpcHit &hits   = globTrack->ftpcHits();
           hitCounter += MakeHits(&hits,filter);
        }
     }
     printf(" %d tracks %d hits have been found\n",trackCounter, hitCounter);
   }   
#endif
   return trackCounter+hitCounter;
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::Make()
{
//  const Int_t maxTrackCounter = 9;
  if (!m_EventsNode) return kStErr;
  if (m_Mode == 1)   return  kStOK;
  CreateCanvas();
  if (Debug()) PrintInfo();
  //_______________________________________
  //
  // Reset all filters
  //_______________________________________
 
  TIter nextFilter(m_FilterArray);
  StVirtualEventFilter *f = 0;
  while ( (f = (StVirtualEventFilter *)nextFilter()) ) f->Reset(); 

 //---  Temporary ---
 //   ClearCanvas();
 //_______________________________________
 //
 //   Creating tracks and hits shapes
 //_______________________________________
  Int_t totalCounter = 0;
  if (m_ShortView){
    if (!m_ListDataSetNames || m_ListDataSetNames->GetSize() == 0) 
    {
    // Add default names
#ifdef STEVENT
      m_Event  = (StEvent *) GetDataSet("StEvent");       
      if (m_Event)  AddName("StEvent");
      else 
#endif
      {
#if TPC
        TDataSet *dshits = GetDataSet("tphit");
        if (dshits)   {
           AddName("tphit(id_globtrk,x:y:z)");
           printf(" tphit found !!!\n");  
          ((TTable *)dshits)->Print(0,10);   
        }
         TDataSet *dstracks = GetDataSet("tptrack");
         if (dstracks) {
            AddName(dstracks->GetName());
            printf(" tptrack found !!!\n"); 
           ((TTable *)dstracks)->Print(0,1);
         }
#else
        TDataSet *dshits = GetDataSet("dst/point");
        // if (dshits)   
        {
           AddName("dst/point(id_track,position[0]:position[1]:charge)");
           if (Debug()) {
              printf(" tphit found !!!\n");  
             ((TTable *)dshits)->Print(0,10);   
           }
        }
        const char *track2Draw = "dst/primtrk";
        TDataSet *dstracks = GetDataSet(track2Draw);
        // if (dstracks) 
        {
           AddName(track2Draw);
           printf(" %s found !!!\n",track2Draw); 
           if (Debug()) {
             printf(" %s found !!!\n",track2Draw); 
            ((TTable *)dstracks)->Print(0,1);
           }
        }
        // printf(" no %s found !!!\n",track2Draw); 

#endif
      }
    }
    TIter nextNames(m_ListDataSetNames);
    TObjString *eventName = 0;
     while ( (eventName = (TObjString *)nextNames()) ) 
    {
      m_Event = 0;
      m_Table = 0;
  
      Char_t *nextObjectName = StrDup(eventName->String().Data());
      Char_t *positions[] = {0,0,0,0,0,0};
      Int_t type = ParseName(nextObjectName, positions);
      if (Debug()) { printf(" The type of the current parameter is %d \n", type); }
      if (!type) { delete [] nextObjectName; continue; }
      const Char_t *foundName = positions[0];
      TDataSet *event = GetDataSet(foundName);
      if (!event) {
         if (Debug()) Warning("Make","No object \"%s\" found",foundName);
         continue;
      }
      if (event->InheritsFrom("TTable") && 
          (( type == 5) || (type == 1) || (type == 2)) ) 
      {
        //  ----- Draw "table" events -------------------------- //
          m_Table = (TTable *)event;                           //
          totalCounter += MakeTable((const Char_t **)positions); //
        //  ---------------------------------------------------- //
      }
      else if (event->InheritsFrom("StEvent") && type == 1) {
         //  ---- Draw "StEvent" events ---- //
           m_Event = (StEvent *)event;       //
           totalCounter += MakeEvent();      //
         //  ------------------------------- //
      }
      else if (Debug()) Warning("Make","Can not draw the object \"%s\"",nextObjectName); 
      delete [] nextObjectName;
     }
     if (totalCounter) {
       m_EventsView = new TVolumeView(*m_EventsNode);
       m_ShortView->Add(m_EventsView);
     }
     printf(" updating view of %d objects\n",totalCounter);
     m_PadBrowserCanvas->Modified();
     m_PadBrowserCanvas->Update();
   }
   return kStOK;
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::ParseName(Char_t *inName, Char_t *positions[])
{ 
  // returns  the number of the tokens found:
  //
  //              "1" - assuming StEvent name defined by position[0]
  //              "5" - assuming TTable columns definitions
  //              "0" - syntax error
  //  
  //  "name" - StEvent
  //  "g2t_tpc_hit(track_id,x[0]:x[1]:x[2])"
  //   Attention:     NO EXPRESSION, yet !!!
  //

  Int_t nParsed = 0;
  if (inName && inName[0]) {
    Char_t *pos = 0;
    const Char_t *errorMessages[] = {  "the open bracket missed"
                                     , "first comma missed"
                                     , "first collon missed"
                                     , "second collon missed"
                                     , "the closed bracket missed"
                                    };
    const Int_t lenExpr = sizeof(errorMessages)/sizeof(Char_t *);
    const Char_t *openBracket  = "(";
    const Char_t *closeBracket = ")";
    const Char_t *comma        = ",)";
    const Char_t *collon       = ":";
    const Char_t *delimiters[] = {openBracket,comma,collon,collon,closeBracket };
    pos = positions[nParsed] = inName;
    UInt_t idx = 0;
    nParsed = 1;
    Bool_t hasClosed = kFALSE;
    for (nParsed = 1; nParsed <= lenExpr; nParsed ++ ) 
    {
      if  ( (idx = strcspn(pos+1,delimiters[nParsed-1])) >=  strlen(pos+1) ) break;
      pos = pos+idx+1;
      positions[nParsed] = pos+1;
      // Is it close bracket
      if (*pos == *closeBracket) { *pos = 0 ; hasClosed = kTRUE; break; }
      *pos = 0;
    }
    if (nParsed > 1) {
      for (Int_t i=1;i<nParsed-1;i++) 
         if (!positions[i]) cerr << "StEventDisplayMaker::ParseName" << errorMessages[i-1] << endl;
         if ( (nParsed <= lenExpr) && !hasClosed ) nParsed = 0;
    }
  }
  return nParsed;
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::MakeEvent()
{
  if (!m_Event) return 0;

  Int_t total      = 0;
  //----------------------------//
    total = MakeGlobalTracks(); //
  //----------------------------//
#ifdef STEVENT
  Int_t hitCounter = 0;
  StVirtualEventFilter *filter = 0;
  filter = (StVirtualEventFilter *)m_FilterArray->At(kTpcHit);
  if (!filter || filter->IsOn() ) {
  StTpcHitCollection *hits   = m_Event->tpcHitCollection();
     hitCounter = MakeHits(hits,filter);
     if (Debug()) printf(" TpcHitCollection: %d hits\n", hitCounter);
     total += hitCounter;
  }

  filter = (StVirtualEventFilter *)m_FilterArray->At(kFtpcHit);
  if (!filter || filter->IsOn() ) {
     StFtpcHitCollection *hits   = m_Event->ftpcHitCollection();
     hitCounter = MakeHits(hits,filter);
     if (Debug()) printf(" FtpcHitCollection: %d hits\n", hitCounter);
     total += hitCounter;
  }

  filter = (StVirtualEventFilter *)m_FilterArray->At(kSvtHit);
  if (!filter || filter->IsOn() ) {
     StSvtHitCollection *hits   = m_Event->svtHitCollection();
     hitCounter = MakeHits(hits,filter);
     if (Debug()) printf(" SvtHitCollection: %d hits\n", hitCounter);
     total += hitCounter;
  }

  filter = (StVirtualEventFilter *)m_FilterArray->At(kEmcTowerHit);
  if (!filter || filter->IsOn() ) {
    StEmcTowerHitCollection *hits   = m_Event->emcTowerHitCollection();
    hitCounter = MakeHits(hits,filter);
    if (Debug()) printf(" EmcTowerHitCollection: %d hits\n", hitCounter);
    total += hitCounter;
  }

  filter = (StVirtualEventFilter *)m_FilterArray->At(kEmcPreShowerHit);
  if (!filter || filter->IsOn() ) {
    StEmcPreShowerHitCollection *hits   = m_Event->emcPreShowerHitCollection();
    hitCounter = MakeHits(hits,filter);
    if (Debug()) printf(" EmcPreShowerHitCollection: %d hits\n", hitCounter);
    total += hitCounter;
  } 

  filter = (StVirtualEventFilter *)m_FilterArray->At(kSmdPhiHit);
  if (!filter || filter->IsOn() ) {
    StSmdPhiHitCollection *hits   = m_Event->smdPhiHitCollection();
    hitCounter = MakeHits(hits,filter);
    if (Debug()) printf(" SmdPhiHitCollection: %d hits\n", hitCounter);
    total += hitCounter;
  }

  filter = (StVirtualEventFilter *)m_FilterArray->At(kSmdEtaHit);
  if (!filter || filter->IsOn() ) {
    StSmdEtaHitCollection *hits   = m_Event->smdEtaHitCollection();
    hitCounter = MakeHits(hits,filter);
    if (Debug()) printf(" SmdEtaHitCollection: %d hits\n", hitCounter);
    total += hitCounter;
  }

  filter = (StVirtualEventFilter *)m_FilterArray->At(kVertices);
  if (!filter || filter->IsOn() ) {
    StVertexCollection *vertices   = m_Event->vertexCollection();
    hitCounter =  MakeVertices(vertices,filter);
    if (Debug()) printf(" VertexCollection: %d vertices\n", hitCounter);
    total += hitCounter;
  }

  filter = (StVirtualEventFilter *)m_FilterArray->At(kPrimaryVertex);
  if (!filter || filter->IsOn() ) {
    StVertex  *vertex   = m_Event->primaryVertex();
    hitCounter =  MakeVertex(vertex,filter);
    if (Debug()) printf(" Primary Vertex: %d vertex\n", hitCounter);
    total += hitCounter;
  }
#endif
  return total;
}
//_____________________________________________________________________________
Color_t StEventDisplayMaker::GetColorAttribute(Int_t adc)
{
  // Convert the inpput signal amplitude into color index
//  return Color_t(10-(adc?TMath::Log2(adc):10));
//  return Color_t(adc?TMath::Log2(adc)+50:10);
  return Color_t(50 + (adc/256));
}


//_____________________________________________________________________________
Int_t StEventDisplayMaker::MakeHits(const StObjArray *eventCollection,StVirtualEventFilter *filter)
{
  if (eventCollection && eventCollection->size() ) {
    Color_t hitColor = kYellow;
    Style_t hitStyle = 1;
    Size_t hitSize  = 2;

    // ---------------------------- hits filter ----------------------------- //
    if (filter) hitColor =  filter->Channel(eventCollection,hitSize,hitStyle);//
    // ---------------------------------------------------------------------- //
#ifdef STEVENT
    Int_t   hitCounter = 0;
    if (hitColor > 0) {
       StHits3DPoints   *hitsPoints  = new StHits3DPoints((StObjArray *)eventCollection);
       m_HitCollector->Add(hitsPoints);    // Collect to remove  
       TPolyLineShape *hitsShape   = new TPolyLineShape(hitsPoints);
         hitsShape->SetVisibility(1);        hitsShape->SetColorAttribute(hitColor);
         hitsShape->SetStyleAttribute(hitStyle);  hitsShape->SetSizeAttribute(hitSize);
       // Create a node to hold it
       TVolume *thisHit = new TVolume("hits",eventCollection->GetName(),hitsShape);
         thisHit->Mark();
         thisHit->SetVisibility();
       TVolumePosition *pp = m_EventsNode->Add(thisHit); 
       if (!pp && hitCounter) {
          printf(" no track position %d\n",hitCounter);
       }
       return hitsPoints->Size(); // hitColor;
    }
#endif
  }
  return 0;
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::MakeVertex(const StVertex *vertex,StVirtualEventFilter *filter)
{
  if (vertex) {

#ifdef STEVENT
    Int_t   vertexCounter = 0;
    Color_t vertexColor = kBlue;
    Style_t vertexStyle = 3;
    Size_t vertexSize  = 1;
    // ---------------------------- hits filter ----------------------------- //
    if (filter) vertexColor =  filter->Channel(vertex,vertexSize,vertexStyle);//
    // ---------------------------------------------------------------------- //
    if (vertexColor > 0) {
       const StThreeVectorF &vertexVector = ((StVertex *)vertex)->position();
       Float_t x = vertexVector[0];
       Float_t y = vertexVector[1];
       Float_t z = vertexVector[2];
       TPoints3D *vertexPoint =  new TPoints3D(1, &x, &y, &z);
       m_HitCollector->Add(vertexPoint);    // Collect to remove  
       TPolyLineShape *vertexShape   = new TPolyLineShape(vertexPoint);
         vertexShape->SetVisibility(1);                vertexShape->SetColorAttribute(vertexColor);
         vertexShape->SetStyleAttribute(vertexStyle);  vertexShape->SetSizeAttribute(vertexSize);
       // Create a node to hold it
       TVolume *thisVertex = new TVolume("vertex",vertex->GetName(),vertexShape);
         thisVertex->Mark();
         thisVertex->SetVisibility();
       TVolumePosition *pp = m_EventsNode->Add(thisVertex); 
       if (!pp) 
          printf(" no track position \n");
       vertexCounter = 1;
       return vertexCounter;
    }
#endif
  }
  return 0;
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::MakeVertices(const StObjArray *eventCollection,StVirtualEventFilter *filter)
{
  if (eventCollection && eventCollection->size() ) {
#ifdef STEVENT
    Int_t   hitCounter = 0;
    Color_t hitColor = kYellow;
    Style_t hitStyle = 1;
    Size_t hitSize  = 2;
    // ---------------------------- hits filter ----------------------------- //
    if (filter) hitColor =  filter->Channel(eventCollection,hitSize,hitStyle); //
    // ---------------------------------------------------------------------- //
    if (hitColor > 0) {
      StVertices3DPoints *hitsPoints  = new StVertices3DPoints((StObjArray *)eventCollection);
       m_HitCollector->Add(hitsPoints);    // Collect to remove  
       TPolyLineShape *hitsShape   = new TPolyLineShape(hitsPoints);
         hitsShape->SetVisibility(1);        hitsShape->SetLineColor(hitColor);
         hitsShape->SetLineStyle(hitStyle);  hitsShape->SetLineWidth(hitSize);
       // Create a node to hold it
       TVolume *thisHit = new TVolume("hits",eventCollection->GetName(),hitsShape);
         thisHit->Mark();
         thisHit->SetVisibility();
       TVolumePosition *pp = m_EventsNode->Add(thisHit); 
       if (!pp && hitCounter) {
          printf(" no track position %d\n",hitCounter);
       }
       return hitsPoints->Size(); // hitColor;
    }
#endif
  }
  return 0;
}
//_____________________________________________________________________________
Int_t StEventDisplayMaker::MakeTracks( StGlobalTrack *globTrack,StVirtualEventFilter *filter)
{
  if (globTrack) {
#ifdef STEVENT
    Int_t   trackCounter = 0;
    Color_t trackColor = kRed;
    Style_t trackStyle = 1;
    Size_t trackSize  = 2;
    // --------------------- tracks filter ---------------------------------- //
    if (filter) trackColor =  filter->Channel(globTrack,trackSize,trackStyle);//
    // ---------------------------------------------------------------------- //
    if (trackColor > 0) {
       StHelix3DPoints *tracksPoints  = new StHelix3DPoints(globTrack);
       m_TrackCollector->Add(tracksPoints);    // Collect to remove  
       TPolyLineShape *tracksShape   = new TPolyLineShape(tracksPoints,"L");
         tracksShape->SetVisibility(1);         tracksShape->SetColorAttribute(trackColor);
         tracksShape->SetLineStyle(trackStyle); tracksShape->SetSizeAttribute(trackSize);
       // Create a node to hold it
       TVolume *thisTrack = new TVolume("tracks",globTrack->GetName(),tracksShape);
         thisTrack->Mark();   thisTrack->SetVisibility();
         trackCounter++;
       TVolumePosition *pp = m_EventsNode->Add(thisTrack); 
       if (!pp && trackCounter) {
          printf(" no track position %d\n",trackCounter);
       }
       return trackCounter;
    }
#endif
  }
  return 0;
}

//_____________________________________________________________________________
Int_t StEventDisplayMaker::MakeTable(const Char_t **positions)
{
  StVirtualEventFilter *filter = 0;
  Int_t tableCounter = 0;
//  StTrackChair &tracks = *StTrackChair::Instance(m_Table);
//  if (!(&tracks)) {
  if (StTrackChair::IsTrack(m_Table)==-1) {
    filter = (StVirtualEventFilter *)m_FilterArray->At(kTable);
    if (!filter || filter->IsOn() ) {
       tableCounter += MakeTableHits(m_Table,filter,positions[1],&positions[2]);
       if (Debug()) printf(" TTable: %d \n", tableCounter);
    }
  }
  else {
    filter = (StVirtualEventFilter *)m_FilterArray->At(kTptTrack);
    if (!filter || filter->IsOn() ) {
       StTrackChair tracks(m_Table);
       tableCounter += MakeTableTracks(&tracks,filter);
       if (Debug()) printf(" TTable:tpc_Track %d \n", tableCounter);
    }
  }
  return tableCounter;
}
//_____________________________________________________________________________
Int_t StEventDisplayMaker::MakeTableTracks(const StTrackChair *points,StVirtualEventFilter *filter)
{
  Int_t i = 0;
  Int_t trackCounter = 0;
  Int_t nRows = 0;
  if (points && (nRows = points->GetNRows()) ) {
    Color_t trackColor = kRed;
    Style_t trackStyle = 1;
    Size_t trackSize  = 1;
    for (i = 0; i < nRows; i++ ){
      filter = (StVirtualEventFilter *)m_FilterArray->At(kTptTrack);
      if (!filter || filter->IsOn() ) {
        // ------------------------------ tracks filter ------------------------------------ //
        if (filter) trackColor =  filter->Channel(points->Table(),i,trackSize,trackStyle);//
        // ----------------------------------------------------------------------------------//
        if (trackColor > 0) {
           StHelixD *helix  = points->MakeHelix(i);
           Float_t      len = points->Length(i);
    	   Int_t nSteps = Int_t(28*len*points->Curvature(i) + 1); 
	       Float_t step = len / nSteps;
           StHelix3DPoints *tracksPoints  = new StHelix3DPoints(helix,step,nSteps);
           m_TrackCollector->Add(tracksPoints);    // Collect to remove  
           TPolyLineShape *tracksShape   = new TPolyLineShape(tracksPoints,"L");
             tracksShape->SetVisibility(1);         tracksShape->SetColorAttribute(trackColor);
             tracksShape->SetLineStyle(trackStyle); tracksShape->SetSizeAttribute(trackSize);
           // Create a node to hold it
           TVolume *thisTrack = new TVolume("tracks",points->GetName(),tracksShape);
             thisTrack->Mark();   thisTrack->SetVisibility();
             trackCounter++;
           TVolumePosition *pp = m_EventsNode->Add(thisTrack); 
           if (!pp && trackCounter) {
              printf(" no track position %d\n",trackCounter);
           }
        }
        else if (trackColor == -1) break;
      }
    }
  }
  return trackCounter;
}
//_____________________________________________________________________________
Int_t StEventDisplayMaker::MakeTableHits(const TTable *points,StVirtualEventFilter *filter
                                        ,const Char_t *keyColumn,const Char_t *keyPositions[]
)
{
  Int_t totalHits = 0;
  TTable &ttt = *((TTable *)points);
  TString tr = keyColumn; 
  const Char_t *packedList[] = {"dst_point_st"};
  const Int_t lPackedList = sizeof(packedList)/sizeof(Char_t *);
  Bool_t packed = kFALSE;
  Int_t i = 0;
  for (i = 0; i< lPackedList && !packed ;i++) {
    if (!strcmp(ttt.GetType(),packedList[i])) packed = kTRUE;
  }
  if (ttt.GetNRows() ) {
    TTableSorter *track2Line = new TTableSorter (ttt,tr);
    m_TableCollector->Add(track2Line);    // Collect to remove  
    Color_t hitColor = kGreen;
    Style_t hitStyle = packed ?   8 : 5;
    Size_t  hitSize  = packed ? 0.6 : 0.9;
    i = 0;
    Int_t nextKeyIndx = 0;
    Int_t maxTrackCounter = track2Line->CountKeys();
    for (i=0;i<maxTrackCounter;i++) 
    { 
       // -------------------------- hits filter -------------------------------------- //
       if (filter) hitColor =  filter->Channel(track2Line,nextKeyIndx,hitSize,hitStyle);//
       if (filter->IsOff() ) break;                                                     //
       // ----------------------------------------------------------------------------- //
       if (hitColor > 0 && keyPositions[0]) {
         TTable3Points *hitsPoints = 0;
         if (packed)  
             hitsPoints = new St_Table3PackedPoints(track2Line,
                                              nextKeyIndx,
                                              keyPositions[0]);
         else if (keyPositions[1] && keyPositions[2])
             hitsPoints = new TTable3Points(track2Line,
                                              nextKeyIndx,
                                              keyPositions[0],keyPositions[1],keyPositions[2]);
         else { hitColor = -1; break; }
         if (hitsPoints) {
           m_HitCollector->Add(hitsPoints);    // Collect to remove  
           TPolyLineShape *hitsShape   = new TPolyLineShape(hitsPoints);
           hitsShape->SetVisibility(1);            hitsShape->SetColorAttribute(hitColor);
           hitsShape->SetStyleAttribute(hitStyle); hitsShape->SetSizeAttribute(hitSize);
           // Create a node to hold it
           TVolume *thisHit = new TVolume("tableHits",points->GetName(),hitsShape);
             thisHit->Mark();
             thisHit->SetVisibility();
           TVolumePosition *pp = m_EventsNode->Add(thisHit); 
           Int_t s = hitsPoints->Size();
           totalHits   += s;
           nextKeyIndx += s; 
           if (!pp && totalHits) printf(" no track position %d\n",totalHits);
         }
       }
       else if (hitColor == -1) break;
       else {
         const void *newID = track2Line->GetKeyAddress(nextKeyIndx);
         nextKeyIndx      += track2Line->CountKey(newID,nextKeyIndx,kFALSE); 
       }
    }
  }
  return totalHits;
}

//________________________________
//
//       -- Filters  --
//________________________________


//_____________________________________________________________________________
Int_t StEventDisplayMaker::SetFlag(Int_t flag, EDisplayEvents filterIndex)
{
 // Set the new filter flag and return the previous one
  StVirtualEventFilter *f = (StVirtualEventFilter *)m_FilterArray->At(filterIndex);
  Int_t res = f ? f->Turn(flag): 0;
  if (Debug()) PrintFilterStatus();
  return res;
}
//_____________________________________________________________________________
StVirtualEventFilter *StEventDisplayMaker::SetFilter(StVirtualEventFilter *filter, EDisplayEvents filterIndex)
{
 // Set the new filter and return the previous one
  StVirtualEventFilter *f = 0;
  if (filter) {
   f = (StVirtualEventFilter *)m_FilterArray->At(filterIndex);
   if (f) 
     filter->Turn(f->GetFlag());
   m_FilterArray->AddAt(filter,filterIndex);
  }
  if (Debug()) PrintFilterStatus();
  return f;
}
 
//_____________________________________________________________________________
void StEventDisplayMaker::PrintFilterStatus()
{
  const Char_t *filterNames[] = {
                                   "Primary Vertex"
                                 , "Tpc Hit"
                                 , "Svt Hit"
                                 , "Ftpc Hit"
                                 , "EmcTower Hit"
                                 , "EmcPreShower Hit"
                                 , "SmdPhi Hit"
                                 , "SmdEta Hit"
                                 , "Vertices"
                                 , "Global Tracks"
                                 , "Track"
                                 , "Track Tpc Hits"
                                 , "Track Svt Hits"
                                 , "Track Ftpc Hits" 
                                 , "TTable generic object" 
                                 , "tpt_track table object" 
                                } ;
   
  Int_t i;
  for (i =0;i<kEndOfEventList;i++) {
      StVirtualEventFilter *filter = (StVirtualEventFilter *)m_FilterArray->At(i);
      printf(" Filter for %16s",filterNames[i]);
      if (filter) {
          if (filter->IsOn()) printf(" is ON\n");
          else printf(" is       OFF\n");
      }
      else printf(" doesn't exist\n");
  }
}

#define DISPLAY_FILTER_DEFINITION(filterName)                                \
Int_t  StEventDisplayMaker::_NAME3_(Set,filterName,Flag)(Int_t flag)         \
{ return SetFlag(flag,_NAME2_(k,filterName)); }                              \
StVirtualEventFilter *StEventDisplayMaker::_NAME2_(Set,filterName)(StVirtualEventFilter *filter) \
{ return SetFilter(filter,_NAME2_(k,filterName)); }

DISPLAY_FILTER_DEFINITION(PrimaryVertex)

// -- Hits collections filters --

DISPLAY_FILTER_DEFINITION(TpcHit)
DISPLAY_FILTER_DEFINITION(SvtHit)
DISPLAY_FILTER_DEFINITION(FtpcHit)
DISPLAY_FILTER_DEFINITION(EmcTowerHit)
DISPLAY_FILTER_DEFINITION(EmcPreShowerHit)
DISPLAY_FILTER_DEFINITION(SmdPhiHit)
DISPLAY_FILTER_DEFINITION(SmdEtaHit)
DISPLAY_FILTER_DEFINITION(Vertices)

// -- StGlobalTrack filters --

DISPLAY_FILTER_DEFINITION(GlobalTracks)
DISPLAY_FILTER_DEFINITION(Track)
DISPLAY_FILTER_DEFINITION(TrackTpcHits)
DISPLAY_FILTER_DEFINITION(TrackSvtHits)
DISPLAY_FILTER_DEFINITION(TrackFtpcHits)

// -- Generic TTable  filters --

DISPLAY_FILTER_DEFINITION(Table)

// -- Tpt track  filter --

DISPLAY_FILTER_DEFINITION(TptTrack)

// --  end of filter list --

//_____________________________________________________________________________
// $Log: StEventDisplayMaker.cxx,v $
// Revision 1.66  2000/08/16 20:50:55  fine
// CTB object has been added to the list of the volumes to draw
//
// Revision 1.65  2000/08/15 23:15:45  fine
// globtrk has been replaced with primtrk for default views
//
// Revision 1.64  2000/08/15 22:17:27  fine
// New defaults to draw globtrk and point
//
// Revision 1.63  2000/08/04 21:03:43  perev
// Leaks + Clear() cleanup
//
// Revision 1.62  2000/07/21 15:50:19  fine
// Bug fix: needs some correction in ROOT/star as well
//
// Revision 1.61  2000/07/03 02:07:47  perev
// StEvent: vector<TObject*>
//
// Revision 1.60  2000/05/03 01:19:42  fine
// emc geometry has been introduced
//
// Revision 1.59  2000/04/26 05:07:35  fine
// buildgeometry method adjusted to new version of TVolumeView ctor
//
// Revision 1.58  2000/04/22 22:53:25  fine
//  new schema to build  the detector geometry based on new ROOT 2.24
//
// Revision 1.57  2000/04/22 20:01:17  fine
// replace St_Table with TTable
//
// Revision 1.56  2000/04/10 20:11:03  fine
// change the default valume fro SVT from SLDI to STSI
//
// Revision 1.55  2000/04/05 03:58:20  fine
// Adjusted for ROOT 2.24
//
// Revision 1.54  2000/03/15 17:22:19  fine
// some extra protection against of dead canvas
//
// Revision 1.53  2000/03/09 22:00:33  fine
// StTrackChair class to draw any track-like table: tpt_track, dst_track has been introduced
//
// Revision 1.52  2000/01/30 02:00:31  fine
// Some adjustment to new ROOT (2.23/11
//
// Revision 1.51  2000/01/24 22:56:46  fine
// new packing schema for ssd introduced
//
// Revision 1.50  1999/12/29 18:56:44  fine
// Change the default filter settings in favour of  StTable
//
// Revision 1.49  1999/12/27 21:45:45  fine
// Protection against of zero-pointer
//
// Revision 1.48  1999/12/22 15:27:34  fine
// Protection against of double names
//
// Revision 1.47  1999/12/21 18:58:15  fine
// new default for SizeAttributes
//
// Revision 1.46  1999/12/20 20:28:52  fine
// adjust some parameteres
//
// Revision 1.45  1999/12/19 00:12:45  fine
// some corrections for the packed tables
//
// Revision 1.44  1999/12/17 23:28:45  fine
// clean up for the sake of docs + new class St_Table3DPackedPoints introduced
//
// Revision 1.43  1999/12/12 01:07:22  fine
// remove the compilation warnings
//
// Revision 1.42  1999/11/24 19:01:15  fine
// all StVirtual::Filter have been renamed to Channel
//
// Revision 1.41  1999/11/24 16:26:27  fine
// Overlap the corrupted v.1.40 with 1.39
//
// Revision 1.39  1999/11/24 03:06:46  fine
// Improved Helix parameters and parser
//
// Revision 1.38  1999/11/22 18:41:45  fine
// Parser for single parameter fixed
//
// Revision 1.37  1999/11/22 16:14:31  fine
// palette() has been removed
//
// Revision 1.36  1999/11/21 00:56:58  fine
// SVT volume name typo fixed
//
// Revision 1.35  1999/11/16 14:40:15  fine
// reference type has been introduced
//
// Revision 1.34  1999/11/12 18:12:37  fine
// SVT view has been introduced
//
// Revision 1.33  1999/11/12 05:27:32  fine
// phase parameter for  StHelix fixed. Thanks  Wensheng and Thomas
//
// Revision 1.32  1999/11/11 17:29:10  fine
// typo Filer replaced with Filter
//
// Revision 1.31  1999/11/10 02:24:36  fine
// StVirtualFilter::Reset method has been introduced
//
// Revision 1.30  1999/11/09 22:47:39  fine
// psi angle was forgotten to be converted from degrees to rad.
//
// Revision 1.29  1999/11/08 17:47:54  fine
// The bug for tpt_track filtering was fixed
//
// Revision 1.28  1999/11/07 05:27:12  fine
// Take in account new data-member of tpt_track table: length
//
// Revision 1.27  1999/11/05 23:18:40  fine
// convertor degree to rad was introduced
//
// Revision 1.26  1999/11/05 02:29:25  fine
// helix drawing for tpt_track table improved
//
// Revision 1.25  1999/11/04 17:59:59  fine
//  several bugs fixed to draw table-objects
//
// Revision 1.24  1999/11/04 01:49:47  fine
// tpt_track drawing is turned On
//
// Revision 1.23  1999/11/02 01:49:26  fine
// A special case for tpt_track table has been introduced
//
// Revision 1.22  1999/10/14 13:42:14  fine
// Some big to draw tables have been fixed
//
// Revision 1.21  1999/10/09 18:17:10  fine
// Some correction to draw tptrack table
//
// Revision 1.20  1999/08/16 16:28:09  fine
// StVirtualEventFilter has been moved to St_base
//
// Revision 1.19  1999/08/09 01:36:47  fine
// MakeTable methods have been activated
//
// Revision 1.18  1999/08/07 20:31:22  fine
// MakeVertex method has been introduced
//
// Revision 1.17  1999/08/05 02:41:12  fine
//  fix problem with the hits attributes
//
// Revision 1.16  1999/08/05 02:14:09  fine
//  style attribute for hits has been fixed
//
// Revision 1.15  1999/08/04 03:52:01  fine
// Helix drawing improvements
//
// Revision 1.14  1999/08/03 19:18:37  fine
// Vertices collections have been introduced
//
// Revision 1.13  1999/08/03 14:50:02  fine
// Clean up
//
// Revision 1.12  1999/08/02 14:43:25  fine
// Vertices collection has been introduced, more simple geometry
//
// Revision 1.11  1999/08/02 02:21:51  fine
// new method ParseName has been introduced,but not activated yet
//_____________________________________________________________________________
