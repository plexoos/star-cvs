//////////////////////////////////////////////////////////
//   This class has been automatically generated 
//     (Thu Nov 29 16:25:03 2001 by ROOT version3.02/01)
//   from TTree Bichsel/H.Bichel Calculation Summary
//   found on file: dEdx2T.root
//////////////////////////////////////////////////////////


#ifndef Bichsel_h
#define Bichsel_h
//#define P03ia
#include "TString.h"
#include "dEdxParameterization.h"
class tpcCorrection_st;
class Bichsel {
 public: 
  enum EParTypes {kP10, kBichsel, kPAI, kTotal};
 private: 
  static TString        m_Tags[kTotal];
  Int_t                 m_Type;
  TString               m_Tag;
  static dEdxParameterization *m_dEdxParameterizations[kTotal]; //!
  dEdxParameterization *m_dEdxParameterization; //!
 public:
  Bichsel(const Char_t *tag="P10", Int_t keep3D=0);
  virtual ~Bichsel() {};
  static Double_t GetdEdxResolution(Int_t k=1, Double_t TrackLengthInTPC=60);
  static Double_t CalcCorrection(const tpcCorrection_st *cor,const Double_t x);
  static Double_t SumSeries(const Double_t &X,const Int_t &N,const Double_t *params);
  static void Clean();
  Double_t    GetMostProbableZ(Double_t log10bg, Double_t log2dx = 1., Int_t kase=0) {
    return m_dEdxParameterization->GetMostProbableZ(log10bg,log2dx,kase);
  }
  Double_t    GetAverageZ(Double_t log10bg, Double_t log2dx = 1., Int_t kase=0) {
    return m_dEdxParameterization->GetAverageZ(log10bg,log2dx,kase);
  }
  Double_t    GetRmsZ(Double_t log10bg, Double_t log2dx = 1., Int_t kase=0) {
    return m_dEdxParameterization->GetRmsZ(log10bg,log2dx,kase);
  }
  Double_t    GetI70(Double_t log10bg, Double_t log2dx = 1., Int_t kase=0);
  Double_t    GetI60(Double_t log10bg, Double_t log2dx = 1., Int_t kase=0)  {
    return m_dEdxParameterization->GetI60(log10bg,log2dx,kase);
  }
  Double_t    GetMostProbabledEdx(Double_t log10bg, Double_t log2dx = 1., Int_t kase = 0) {
    return m_dEdxParameterization->GetMostProbabledEdx(log10bg,log2dx,kase);
  }
  Double_t    GetdEdxWidth(Double_t log10bg, Double_t log2dx = 1., Int_t kase=0) {
    return m_dEdxParameterization->GetdEdxWidth(log10bg,log2dx,kase);
  }
  Double_t    GetProbability(Double_t log10bg, Double_t log2dx, Double_t z, Int_t kase=0) {
    return m_dEdxParameterization->GetProbability(log10bg,log2dx,z,kase);}
  Double_t    TofCorrection(Double_t log10bg);
  const dEdxParameterization *Parameterization() const {return m_dEdxParameterization;}
  virtual void Print();
  const Char_t      *Tag() const {return    m_dEdxParameterization->Tag();}   
  const TProfile2D  *P()   const {return     m_dEdxParameterization->P();}     
  const TProfile2D  *A()   const {return     m_dEdxParameterization->A();}     
  const TProfile2D  *I70() const {return   m_dEdxParameterization->I70();}   
  const TProfile2D  *I60() const {return   m_dEdxParameterization->I60();}   
  const TProfile2D  *D()   const {return     m_dEdxParameterization->D();}     
  const TProfile2D  *Rms() const {return   m_dEdxParameterization->Rms();}   
  const TProfile2D  *W()   const {return     m_dEdxParameterization->W();}     
  const TH3D        *Phi() const {return   m_dEdxParameterization->Phi();}     
  ClassDef(Bichsel,0)
};
#endif

