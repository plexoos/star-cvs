//*-- Author :    Valery Fine(fine@bnl.gov)   11/07/99  
// $Id: StTrackFilter.h,v 1.1 2004/02/06 02:30:36 munhoz Exp $ 
// $Log: StTrackFilter.h,v $
// Revision 1.1  2004/02/06 02:30:36  munhoz
// inserting SVT online monitor
//
// Revision 1.6  2000/08/10 17:09:31  fine
//  Wensheng example has been fixed
//
// Revision 1.5  2000/03/10 19:14:59  fine
// Separate SubChannel for dst_track table has been introduced
//
// Revision 1.4  1999/12/21 19:17:31  fine
// Width_t type has been replaced by Size_t
//
// Revision 1.3  1999/12/15 23:04:14  fine
// header file fixed by Wensheng
// 
//
#ifndef STAR_StTrackFilter
#define STAR_StTrackFilter

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StTrackFilter base class                              //
//                                                                      //
//                                                                      //
//  Submit any problem with this code via begin_html <A HREF="http://www.star.bnl.gov/STARAFS/comp/sofi/bugs/send-pr.html"><B><I>"STAR Problem Report Form"</I></B></A> end_html
//
//////////////////////////////////////////////////////////////////////////

#include "StVirtualEventFilter.h"

class TTableIter;
class St_tcl_tphit;
class St_dst_track;
class St_g2t_tpc_hit;
class St_g2t_svt_hit;
class St_dst_point;

class StTrackFilter : public StVirtualEventFilter  {
 private:
    Int_t     m_Track_P[100]; // track id to draw g2t_tpc_hit
    Int_t     m_LTrackP;      // the number of hits in m_Track_P

    Int_t     m_id_globtrk[100000];
    Int_t     m_Lid_globtrk;

    Int_t     m_tpt_track_id[100];
    Int_t     m_Ltpt_track;

    Int_t  waferID;
    Int_t  hybridID;

    Bool_t mPrimary;
    Bool_t mGlobal;

    Bool_t mAllTracks;
    Bool_t mTracksHitSvt;
    Bool_t mDrawHits;
    Bool_t mDrawTracks;

    const TTableSorter *m_G2t_track; // pointer to g2t_track sorted table
    TTableIter *m_NextG2tTrack;

    Int_t     m_Ge_pid;          // The "ge_pid" value we are looking for
    const TTableSorter *m_G2t_vertex;

    double GetLength(St_dst_track   &track, Int_t rowNumber); 

 protected:
    Int_t SubChannel(St_tcl_tphit   &hit,   Int_t rowNumber,Size_t &size,Style_t &style); 
    Int_t SubChannel(St_dst_track   &track, Int_t rowNumber,Size_t &size,Style_t &style); 
    Int_t SubChannel(St_g2t_tpc_hit &hit,   Int_t rowNumber,Size_t &size,Style_t &style); 
    Int_t SubChannel(St_g2t_svt_hit &hit,   Int_t rowNumber,Size_t &size,Style_t &style); 
    Int_t SubChannel(St_dst_point &hit, Int_t rowNumber,Size_t &size, Style_t &style);

 public:
    StTrackFilter() : m_LTrackP(0),m_Lid_globtrk(0),m_Ltpt_track(0), m_G2t_track(0), m_NextG2tTrack(0) {}
    virtual ~StTrackFilter() {Reset();}
    virtual Int_t Channel(const TTableSorter *tableObject,Int_t index,Size_t &size,Style_t &style);
    virtual Int_t Channel(const TTable *tableObject,Int_t rowNumber,Size_t &size,Style_t &style);
    virtual Int_t Reset(Int_t reset=0);

    Int_t SetGe_pid(Int_t pid);
    Int_t SetTrack_P(Int_t *track_p,Int_t n=1); 
    Int_t SetId_globtrk(Int_t *id_globtrk,Int_t n=1); 
    Int_t SetTptID(Int_t *track_id,Int_t n=1); 

    void SetHybridID(int wafer, int hybrid){waferID = wafer; hybridID = hybrid;}

    void SetPrimary(Bool_t flag){mPrimary = flag;}
    void SetGlobal(Bool_t flag){mGlobal = flag;}

    void SetAllTracks(Bool_t flag){mAllTracks = flag;}
    void SetTracksHitSvt(Bool_t flag){mTracksHitSvt = flag;}
    void SetDrawHits(Bool_t flag){mDrawHits = flag;}
    void SetDrawTracks(Bool_t flag){mDrawTracks = flag;}

    ClassDef(StTrackFilter,0)
};

#endif
