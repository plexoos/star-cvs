#include <iomanip>
#include <assert.h>
#include "TRVector.h"
#include "TError.h"
#include "TString.h"
ClassImp(TRVector);
//________________________________________________________________________________
TRVector::TRVector(Int_t nrows):    TRMatrix(nrows,1) {;}
//________________________________________________________________________________
TRVector::TRVector(Int_t nrows,const Double_t *fArray): TRMatrix(nrows,1,fArray){;}
//________________________________________________________________________________
TRVector::TRVector(Int_t nrows,const Float_t *fArray): TRMatrix(nrows,1,fArray){;}
//________________________________________________________________________________
TRVector::TRVector(const TRMatrix& A, ETRMatrixCreatorsOp kop,const TRVector& B) :
    TRMatrix(A, kop, B) {assert (kop = kAxB);}
//________________________________________________________________________________
TRVector::TRVector(const TRMatrix& S, Int_t I) {// get I'th row
  if (I == 0) {::Error("TRVector::TRVector(const TRMatrix&)", "index i %d out of bounds (size: %d, this: 0x%08x)", 
		       I, S.NI(), this); I = 1;}
  if (I > S.NI()) {::Error("TRVector::TRVector(const TRMatrix&)", "index i %d out of bounds (size: %d, this: 0x%08x)", 
			   I, S.NI(), this); I = 1;}
  fNrows = S.NJ();
  fNcols = 1;
  Set(fNrows,S.GetArray()+S.NJ()*(I-1));
}
//________________________________________________________________________________
TRVector::TRVector(const TRVector& A, ETRMatrixCreatorsOp kop,const TRMatrix& B) {
  Int_t NI, NJ, NK;
  switch (kop) {
  case kAxB:
    NI = A.GetNrows(); fNcols = NI;
    NJ = A.GetNcols();
    assert (NJ == B.GetNrows());
    NK = B.GetNcols(); fNrows = NK;
    Set(NI*NK);
    TCL::mxmpy(A.GetArray(),B.GetArray(),fArray,NI,NJ,NK);
    break;
  default:
    Error("TRVector(ETRMatrixCreatorsOp)", "operation %d not yet implemented", kop);
  }
}
//________________________________________________________________________________
TRVector::TRVector(Int_t nrows,const Char_t *s): TRMatrix(nrows,1,s) {;}
//________________________________________________________________________________
TRVector::TRVector(const TRSymMatrix &S, ETRMatrixCreatorsOp kop,const TRVector& A) :
    TRMatrix(S,kop,A) {;}
//________________________________________________________________________________
TRVector::TRVector(const TRVector& A, ETRMatrixCreatorsOp kop,const TRSymMatrix &S) :
    TRMatrix(A,kop,S) {;}
//________________________________________________________________________________
TRVector::TRVector(Int_t nrows,Double_t a0, ...) : TRMatrix(nrows,1) {
  __VA_LIST__(a0);
} 
//________________________________________________________________________________
ostream& operator<<(ostream& s,const TRVector &target) {
  Int_t Nrows = target.GetNrows();
  assert(target.GetNcols() == 1);
  const Double_t *Array = target.GetArray();
  s << "Vector[" << Nrows << "] = ";// << endl;
  if (Array) for (int i = 0; i< Nrows; i++) s << Form("\t%10.3f",Array[i]);
  else s << " Empty";
  s << endl;
  return s;
}
//________________________________________________________________________________
void TRVector::Print(Option_t *opt) const {if (opt) {}; cout << *this;}
