/***********************************************************************
 *
 * $Id: StStrangeCuts.cc,v 3.1 2001/01/30 04:06:45 genevb Exp $
 *
 * Author: Gene Van Buren, UCLA, 26-May-2000
 *
 ***********************************************************************
 *
 * Description: handling of cuts for strangeness micro DST
 *
 ***********************************************************************
 *
 * $Log: StStrangeCuts.cc,v $
 * Revision 3.1  2001/01/30 04:06:45  genevb
 * Better handling of file switches
 *
 * Revision 3.0  2000/07/14 12:56:49  genevb
 * Revision 3 has event multiplicities and dedx information for vertex tracks
 *
 * Revision 2.0  2000/06/05 05:19:42  genevb
 * New version of Strangeness micro DST package
 *
 *
 ***********************************************************************/
#include "TDataSet.h"
#include "TTable.h"
#include "TClass.h"
#include "TDataMember.h"
#include "TDataType.h"
#include "TString.h"
#include "StStrangeCuts.hh"
#include "StMessMgr.h"

ClassImp(StStrangeCuts)
//_____________________________________________________________________________
StStrangeCuts::StStrangeCuts() : cuts(0) {
}
//_____________________________________________________________________________
StStrangeCuts::~StStrangeCuts() {
}
//_____________________________________________________________________________
void StStrangeCuts::Fill(const char* prefix, TDataSet* cutSet) {
  // Fill cuts from tables
  TTable* cutTable = dynamic_cast<TTable*> (cutSet);
  if (!cutTable) return;
  TString colName;
  Char_t buf[16];
  Int_t nrows = cutTable->GetNRows();
  TClass* rowClass = cutTable->GetRowClass();
  for (Int_t row = 0; row<nrows; row++) {
    for (UInt_t col = 0; col<cutTable->GetNumberOfColumns(); col++) {
      const Char_t* colBaseName = cutTable->GetColumnName(col);
      ((colName = prefix) += ".") += colBaseName;
      if (nrows>1) {
        sprintf(buf,":%d",row);
        colName += buf;
      }
      UInt_t colOffset = cutTable->GetOffset(col);
      void* colValue = (void*) (((UInt_t) ((*cutTable)[0])) + colOffset);
      TDataType* colType = rowClass->GetDataMember(colBaseName)->GetDataType();
      Add(colName.Data(),colType->AsString(colValue));
    }
  }
}
//_____________________________________________________________________________
void StStrangeCuts::Append(TOrdCollection* oldCuts) {
  // Add any new cuts to any old cuts
  if (oldCuts) {
    if (cuts) {
      for (Int_t i=(oldCuts->GetSize() - 1); i>=0; i--) {
        TObject* oldCut = oldCuts->At(i);
        if (NewCut(oldCut)) cuts->AddFirst(oldCut);
      }
    } else cuts = oldCuts;
  } else {
    gMessMgr->Warning() << "StStrangeCuts: no StrangeCuts to read in.\n "
                     << "   Creating new one." << endm;    
    Assure();
  }
}
//_____________________________________________________________________________
Bool_t StStrangeCuts::NewCut(TObject* aCut) {
  for (Int_t i=0; i<(cuts->GetSize()); i++) {
    TObject* bCut = cuts->At(i);
    if (!(bCut->Compare(aCut))) {
      if (!(strcmp(bCut->GetTitle(),aCut->GetTitle())))
        return kFALSE;
    }
  }
  return kTRUE;
}

