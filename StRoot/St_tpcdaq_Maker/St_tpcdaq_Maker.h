// $Id: St_tpcdaq_Maker.h,v 1.3 1999/04/07 19:48:41 ward Exp $
// $Log: St_tpcdaq_Maker.h,v $
// Revision 1.3  1999/04/07 19:48:41  ward
// Fixed adc mis-cast and also mis-count of pixel offset.
//
// Revision 1.2  1999/03/15 03:24:14  perev
// New maker schema
//
// Revision 1.1  1999/02/19 16:32:21  fisyak
// rename h-file and access name to Trs
//
// Revision 1.1  1999/02/18 16:56:35  ward
// There may be bugs. = Moshno oshibki.
//
// #define TRS_SIMPLE
#ifndef STAR_St_tpcdaq_Maker
#define STAR_St_tpcdaq_Maker

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_tpcdaq_Maker virtual base class for Maker                         //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
#ifndef StMaker_H
#include "StMaker.h"
#endif

class St_DataSet;
class St_raw_row;
class St_raw_pad;
class St_raw_seq;
class St_type_shortdata;
class StTpcRawDataEvent;
class StTrsSimpleMaker;
class StTpcUnpacker;
class StSequence;
class St_tpcdaq_Maker : public StMaker {
 private:
   StTpcRawDataEvent *mEvent; //!
#ifdef TRS_SIMPLE
   StTrsSimpleMaker *mUnpacker; //!
#else
   StTpcUnpacker *mUnpacker; //!
#endif
   Int_t GetEventAndDecoder();
 protected:
 public: 
                  St_tpcdaq_Maker(const char *name="tpc_raw");
   int mErr;
   
   void OrderTheSequences(int nseq,StSequence *los);
   void FatalError(int);
   void SeqWrite(St_raw_seq *raw_seq_gen,int rownum,
                  int startTimeBin,int numberOfBinsInSequence);
   void PixelWrite(St_type_shortdata *pixel_data_gen,
                    int rownum,unsigned char datum);
   void PadWrite(St_raw_pad *raw_pad_gen,int padR,int padOffset,
      int seqOffset,int nseq,int nSeqB4Offset,int pad);
   void RowWrite(St_raw_row *raw_row_gen,int rownumber,
          int pixSave, int iseqSave,int nPixelThisPadRow,
          int nSeqThisPadRow,int offsetIntoPadTable,
          int nPadsWithSignal,int pixTblWhere,int ipadrow);
   void MkTables(
      int isect,
      St_DataSet *sector,
      St_raw_row **raw_row_in,
      St_raw_row **raw_row_out,
      St_raw_pad **raw_pad_in,
      St_raw_pad **raw_pad_out,
      St_raw_seq **raw_seq_in,
      St_raw_seq **raw_seq_out,
      St_type_shortdata **pixel_data_in,
      St_type_shortdata **pixel_data_out);
   char *NameOfSector(int isect);
   void PrintErr(int,char);
   int Output();
   int tpcSectorZgetSector(int j1,int *j2); // junk for early debugging
   int tpcSectorZgetPadList(int ipad,char **padlist); // junk 4 early debugging
   int myDecoderZgetSequences(int ipadrow,int pad,int *nseq,
                 StSequence **listOfSequences); // junk for early debugging
   virtual       ~St_tpcdaq_Maker();
   virtual Int_t  Init();
   virtual Int_t  Make();
   virtual void   PrintInfo();
// virtual void Set_mode       (Int_t   m =      2){m_mode       = m;} // *MENU*
   ClassDef(St_tpcdaq_Maker, 1)   //StAF chain virtual base class for Makers
};

#endif
