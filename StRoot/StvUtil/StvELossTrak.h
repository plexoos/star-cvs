// $Id: StvELossTrak.h,v 1.11.4.1 2017/12/02 00:37:10 perev Exp $
//
//
// Class StvELossTrak
// ------------------


#ifndef STIELOSSTRAK_H
#define STIELOSSTRAK_H
#include "TObject.h"
class TGeoMaterial;
static const double PiMASS=0.13956995;
 class StvELossTrak : public TObject
{
public:
         StvELossTrak(){Reset();}
        ~StvELossTrak(){;}
    void unset() {Clear();}
    void reset() {Clear();}
    void Reset(double mass = PiMASS, double charge=1);
    void Clear(const char *opt="");
     int Same(const TGeoMaterial *mate) const;
    void Set(double A, double Z, double Dens, double x0,double p);
    void Set(const TGeoMaterial *mate,double p);
    void Add(double len);
//		Last step data
  double Theta2() 	const 		{return fTheta2;	}
  double Ort2()   	const		{return fOrth2;		}
  double dEdX ()  	const		{return fdEdX;		}
  double ELoss()  	const 		{return fELoss;		}
  double PinvLoss()  	const 		{return fPinvLoss;	}
  double ELossErr2() 	const		{return fELossErr2;	}
  double PinvErr2() 	const 		{return fPinvErr2;	}
  double Len() 		const		{return fLen;		}	
//		Total data
  double TotTheta2() 	const 		{return fTotTheta2;	}
  double TotOrt2()   	const		{return fTotOrth2;	}
  double TotdEdX () 	const		{return fTotELoss/(fTotLen+1e-9);}
  double TotELoss() 	const 		{return fTotELoss;	}
  double TotPinvLoss() 	const 		{return fTotPinvLoss;	}
  double TotELossErr2() const		{return fTotELossErr2;	}
  double TotPinvErr2() 	const		{return fTotPinvErr2;	}
  double TotLen() 	const		{return fTotLen;	}	

  double P() 		        const	{return fP;	}	
  double M() 		        const	{return fM;	}	
     int GetNMats() 		const	{return fNMats;	}	
const TGeoMaterial *GetMate()   const	{return fMate ;	}
private:
char   fBeg[1];
double fdEdX,fdEdXErr2;
double fP;		//momentum 
double fM;		//mass 
double fE;		//energy 
double fCharge2;	//particle charge **2
double fFak;
double fA,fZ,fDens,fX0;
int    fNMats;		///Number of different material used
const TGeoMaterial *fMate;
double fELoss; 		///last energy loss
double fPinvLoss; 	///last 1/p loss
double fELossErr2;	///last error of energy loss
double fPinvErr2;	///last error of 1/p loss
double fLen;		///last track length
double fTheta2;
double fOrth2;
//
char   fMed[1];
double fTotELoss; 	///accumulated energy loss
double fTotPinvLoss;	///accumulated 1/p loss
double fTotELossErr2;	///accumulated error of energy loss
double fTotPinvErr2;	///accumulated error of 1/p loss
double fTotLen;		///accumulated track length
double fTotTheta2;
double fTotOrth2;
char   fEnd[1];
ClassDef(StvELossTrak,0) 
};
#endif //STVELOSSTRAK_H   
