//*-- Author :    Valery Fine(fine@bnl.gov)   11/07/99  
//   
//  
//
#ifndef STAR_StEventDisplayMaker
#define STAR_StEventDisplayMaker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StEventDisplayMaker base class                                         //
//                                                                      //
//                                                                      //
//  Submit any problem with this code via begin_html <A HREF="http://www.star.bnl.gov/STARAFS/comp/sofi/bugs/send-pr.html"><B><I>"STAR Problem Report Form"</I></B></A> end_html
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif


class    St_Node;
class    St_NodeView;

class TH2F;
class TCanvas;
class StEvent;
class StObjArray;
class StVecPtrTpcHit;
class StGlobalTrack;
class StVirtualEventFilter;

class StEventDisplayMaker : public StMaker {
 private:
// static Char_t  m_VersionCVS = "$Id: StEventDisplayMaker.h,v 1.5 1999/07/23 00:25:30 fine Exp $";
 private: 
    TList         *m_HitCollector;     //!
    TList         *m_TrackCollector;   //!

 protected:
    St_Node      *m_Hall;         //!
    St_NodeView  *m_FullView;     //!
    St_NodeView  *m_ShortView;    //!
    St_NodeView  *m_Sensible;     //!
    St_Node      *m_EventsNode;   //!
    St_NodeView  *m_EventsView;   //!
    StEvent      *m_Event;        //!
    TObjArray    *m_FilterArray;     // Array of the "event" user supplied filters

    TCanvas      *m_PadBrowserCanvas; //!

 public: 
                  StEventDisplayMaker(const char *name="EventDisplay");
   virtual       ~StEventDisplayMaker();
   virtual Int_t  BuildGeometry();
   virtual Int_t  Init();
   virtual Int_t  Make();
   virtual Int_t  MakeGlobalTracks();
   virtual Int_t  MakeTracks( StGlobalTrack *globTrack,StVirtualEventFilter *filter);
   virtual Int_t  MakeHits(const StObjArray *eventCollection,StVirtualEventFilter *filter);
   virtual void   Clear(Option_t *option="");
   virtual void   ClearCanvas(); // *MENU*
   virtual void   ClearEvents();
   virtual Int_t  CreateCanvas();
   virtual Int_t  CreateTrackNodes();
   virtual St_Node *GetHall()          { return m_Hall; }
   virtual St_NodeView *GetFullView()  { return m_FullView;  }
   virtual St_NodeView *GetShortView() { return m_ShortView; }
   virtual St_NodeView *GetSensible()  { return m_Sensible;  }
   virtual St_Node     *GetEventsNode(){ return m_EventsNode;}
   virtual Color_t      GetColorAttribute(Int_t adc);
   virtual void         SetMode       (Int_t   m = 0){StMaker::SetMode(m);} // *MENU*
   virtual Int_t        ReDraw(){ClearCanvas(); return Make();} // *MENU*
   virtual void         TurnOn() { SetMode(); }  // *MENU*
   virtual void         TurnOff(){ SetMode(1); } // *MENU*
     // --   Filters  --

     enum EDisplayEvents 
          {
            kPrimaryVertex  ,kTpcHit      ,kSvtHit      ,kFtpcHit      ,kEmcTowerHit,
            kEmcPreShowerHit,kSmdPhiHit   ,kSmdEtaHit   ,kGlobalTracks ,
            kTrack          ,kTrackTpcHits,kTrackSvtHits,kTrackFtpcHits, 
            kEndOfEventList
          } ;


 
     // --   Filters  --
 
     Int_t SetFlag(Int_t flag, EDisplayEvents filterIndex);
     StVirtualEventFilter *SetFilter(StVirtualEventFilter *filter, EDisplayEvents filterIndex);

     // -- Vertex filters --

     Int_t SetPrimaryVertexFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetPrimaryVertex(StVirtualEventFilter *filter);

     // -- Hits collections filters --

     Int_t SetTpcHitFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetTpcHit(StVirtualEventFilter *filter);

     Int_t SetSvtHitFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetSvtHit(StVirtualEventFilter *filter);

     Int_t SetFtpcHitFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetFtpcHit(StVirtualEventFilter *filter);

     Int_t SetEmcTowerHitFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetEmcTowerHit(StVirtualEventFilter *filter);

     Int_t SetEmcPreShowerHitFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetEmcPreShowerHit(StVirtualEventFilter *filter);

     Int_t SetSmdPhiHitFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetSmdPhiHit(StVirtualEventFilter *filter);

     Int_t SetSmdEtaHitFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetSmdEtaHit(StVirtualEventFilter *filter);

     // -- StGlobalTrack filters --

     Int_t SetGlobalTracksFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetGlobalTracks(StVirtualEventFilter *filter);

     Int_t SetTrackFlag(Int_t flag=1);    // *MENU*
     StVirtualEventFilter *SetTrack(StVirtualEventFilter *filter);

     Int_t SetTrackTpcHitsFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetTrackTpcHits(StVirtualEventFilter *filter);

     Int_t SetTrackSvtHitsFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetTrackSvtHits(StVirtualEventFilter *filter);

     Int_t SetTrackFtpcHitsFlag(Int_t flag=1); // *MENU*
     StVirtualEventFilter *SetTrackFtpcHits(StVirtualEventFilter *filter);

  // --  end of filter list --

   virtual const char *GetCVS() const
  {static const char cvs[]="Tag $Name:  $ $Id: StEventDisplayMaker.h,v 1.5 1999/07/23 00:25:30 fine Exp $ built "__DATE__" "__TIME__ ; return cvs;}

   ClassDef(StEventDisplayMaker, 0)   //
};

#endif
