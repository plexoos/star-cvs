#ifndef __StGammaStrip_h__
#define __StGammaStrip_h__

#include <TObject.h>
#include <vector>
#include <TRefArray.h>

enum { kEEmcSmdu=0, kEEmcSmdv=1, kBEmcSmdEta=10, kBEmcSmdPhi=11 };

class StGammaStrip : public TObject
{
 public:
  StGammaStrip();
  ~StGammaStrip(){ /* nada */ }
  Int_t index;   // index of strip in plane
  Int_t sector;  // or bemc module
  Int_t plane;   // 0=esmd-u 1=esmdv 10=bsmd-eta 11=bsmd-phi
  Float_t energy;  // energy deposited
  Float_t position; // Reference position for the calculation of moments
                    // bsmd-eta -> theta
                    // bsmd-phi -> phi
                    // esmd-u -> u index
                    // esmd-v -> v index
  Int_t stat;  // status bits (non fatal HW problems)
  Int_t fail;    // fail bits (fatal HW problems)
  TRefArray candidates; // referencing candidates
  void print();

  ClassDef(StGammaStrip,2);
};
typedef std::vector<StGammaStrip> StGammaStripVec_t;

#endif
