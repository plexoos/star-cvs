// @(#)root/base:$Name:  $:$Id: TNumDeriv.cxx,v 1.1.1.1 2007/02/21 16:54:03 fisyak Exp $
// Author: Victor Perev   05/08/03


#include "TNumDeriv.h"

#ifndef __CINT__
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

Double_t TNumDeriv::fgTiny=0;
Double_t TNumDeriv::fgEpsilon=0;



ClassImp(TNumDeriv)
ClassImp(TNumDeriv1Test)
//______________________________________________________________________________
Double_t TNumDeriv::DFcn(Double_t add )
{
  double dfdx,delta=fStep,rerr=0;
  dfdx = numericalDerivative(  add ,delta,rerr);
  fStep=delta;
  return dfdx;
}
  

//______________________________________________________________________________
Double_t TNumDeriv::Tiny()
{
  if (fgTiny) return fgTiny;
  for (double d = 1; d ; d/=2) { fgTiny=d;}
  return fgTiny;
}

//______________________________________________________________________________
Double_t  TNumDeriv::Epsilon()
{
  if (fgEpsilon) return fgEpsilon;
  for (double d = 1; (d+1.)>1. ; d/=2) { fgEpsilon=d;}
  return fgEpsilon;
}
//______________________________________________________________________________
double TNumDeriv::numericalDerivative(  double x, double &delta, double &error )
{
//  This code was taken from CLHEP and was slightly improved for the bad cases

  fFail = 1;
  double h0 = delta;
  if(!h0) h0 = 5 * pow(2.0, -17);

  const double maxErrorA = .0012;    // These are the largest errors in steps 
  const double maxErrorB = .0000026; // A, B consistent with 8-digit accuracy.

  const double maxErrorC = .0003; // Largest acceptable validation discrepancy.

  // This value of gives 8-digit accuracy for 1250 > curvature scale < 1/1250.

  const int nItersMax = 6;
  int nIters;
  double bestError = 1.0E30;
  double bestAns = 0;
  double bestDelta = 0;

  const double valFactor  = pow(2.0, -4); //-16);

  const double w   = 5.0/8;
  const double wi2 = 64.0/25.0;
  const double wi4 = wi2*wi2;

  double F0,Fl,Fr;
  F0    = Fcn(x);
  if (!finite(F0)) return 0;
  double size0    = fabs(F0);
  if (size0==0) size0 = pow(2.0, -53);

  const double adjustmentFactor[nItersMax] = {
    1.0,
#if 0
    pow(2.0, -17),
    pow(2.0, +17),
    pow(2.0, -34),
    pow(2.0, +34),
#else
    pow(2.0, -4),
    pow(2.0, +4),
    pow(2.0, -8),
    pow(2.0, +8),
#endif
    pow(2.0, -51)  };

  for ( nIters = 0; nIters < nItersMax; ++nIters ) {
    double size = size0;

    double h = h0 * adjustmentFactor[nIters];
    if (delta > 0 && h > 2*delta) h = 2*delta;
    // Step A: Three estimates based on h and two smaller values:

    Fr = Fcn(x+h);
    if (!finite(Fr)) continue;
    Fl = Fcn(x-h);
    if (!finite(Fl)) continue;
    double A1 = (Fr - Fl)/(2.0*h);
    if (!finite(A1)) continue;
//    size = max(fabs(A1), size);
    if (fabs(A1) > size) size = fabs(A1);

    double hh = w*h;
    Fr = Fcn(x+hh);
    if (!finite(Fr)) continue;
    Fl = Fcn(x-hh);
    if (!finite(Fl)) continue;
    double A2 = (Fr-Fl)/(2.0*hh);
    if (!finite(A2)) continue;
//    size = max(fabs(A2), size);
    if (fabs(A2) > size) size = fabs(A2);

    hh *= w; 
    if (delta > 0 && hh > 2*delta) hh = 2*delta;
    Fr = Fcn(x+hh);
    if (!finite(Fr)) continue;
    Fl = Fcn(x-hh);
    if (!finite(Fl)) continue;
    double A3 = (Fr-Fl)/(2.0*hh);
//    size = max(fabs(A3), size);
    if (!finite(A3)) continue;
    if (fabs(A3) > size) size = fabs(A3);

    if ( (fabs(A1-A2)/size > maxErrorA) || (fabs(A1-A3)/size > maxErrorA) ) { 
      continue;
    }

    // Step B: Two second-order estimates based on h h*w, from A estimates

    double B1 = ( A2 * wi2 - A1 ) / ( wi2 - 1 );
    if (!finite(B1)) continue;
    double B2 = ( A3 * wi2 - A2 ) / ( wi2 - 1 );
    if (!finite(B2)) continue;
    if ( fabs(B1-B2)/size > maxErrorB ) { 
      continue;
    }

    // Step C: Third-order estimate, from B estimates:

    double ans = ( B2 * wi4 - B1 ) / ( wi4 - 1 );
    if (!finite(ans)) continue;
    double err = fabs ( ans - B1 );
    if ( err < bestError ) {
      bestError = err;
      bestAns   = ans;
      bestDelta = h;
    }

    // Validation estimate based on much smaller h value:

    hh = h * valFactor;
    if (delta > 0 && hh > 2*delta) hh = 2*delta;
    double val = (Fcn(x+hh) - Fcn(x-hh))/(2.0*hh);
    if (!finite(val)) continue;
    if ( fabs(val-ans)/size > maxErrorC ) {
      continue;
    }

    // Having passed both apparent accuracy and validation, we are finished:
    fFail = 0;
    break;
  }
  delta = bestDelta;
  error = bestError;
  return bestAns;

}

#endif //!__CINT__
