//*CMZ :          05/08/98  18.27.27  by  Valery Fine(fine@bnl.gov)
//*-- Author :    Valery Fine(fine@mail.cern.ch)   05/08/98 
// $Id: St_FileSet.h,v 1.5 1999/03/11 00:34:44 perev Exp $
// $Log: St_FileSet.h,v $
// Revision 1.5  1999/03/11 00:34:44  perev
// St_base in new maker schema
//
// Revision 1.4  1998/12/26 21:40:38  fisyak
// Add Id and Log
// 

#ifndef ROOT_St_FileSet
#define ROOT_St_FileSet
  
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// St_FileSet                                                           //
//                                                                      //
// St_FileSet class is a class to convert the                           // 
//      "native file system structure"                                  //
// into an instance of the St_DataSet class                             //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
 
#include "St_DataSet.h"
#include "TString.h"
  
class St_FileSet : public St_DataSet
{
 public: 
    St_FileSet();
    St_FileSet(const TString &dirname, const Char_t *filename=0,Bool_t expand=kTRUE);
    virtual ~St_FileSet();
    virtual Long_t HasData() const;
    virtual Bool_t IsEmpty() const;
    virtual Bool_t IsFolder();
    ClassDef(St_FileSet,1)
};

#endif
