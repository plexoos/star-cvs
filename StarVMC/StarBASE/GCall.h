// $Id: GCall.h,v 1.1 2010/02/11 19:50:48 jwebb Exp $
//
//
// Class GCall
// ------------------


#ifndef STMC_USER_H
#define STMC_USER_H

#include <TNamed.h>
class TVirtualMC;// ......................... Interface to Monte Carlo 
class TVirtualMCApplication;// .............. Interface to MonteCarlo application 
class TVirtualMCStack;

class GCall : public TNamed
{
  public:
    GCall(const char *name="Empty",const char *tit="");
    virtual ~GCall(){}    
    // methods
            void SetInterfaces(TVirtualMCApplication *mca
			      ,TVirtualMC            *mc
			      ,TVirtualMCStack       *stk);
    virtual void Init() {};
    virtual int  Fun ();
    virtual int  FunDD(const Double_t* d1, Double_t* d2);
    virtual void Field(const Double_t* d1, Double_t* d2);
    virtual void Print(const Option_t* opt=0) const;
    virtual void Finish(const Option_t* opt=0) {if(opt){}};

    int operator()(){return Fun();}
    int operator()(const double* d1, double* d2) {return FunDD(d1,d2);}
    void SetDebug(int db=1)	{fDebug = db  ;}
    int  GetDebug() const 	{return fDebug;}

  protected:
    // data members
  int fDebug;
  TVirtualMC            *fMC; // .............. Interface to Monte Carlo 
  TVirtualMCApplication *fMCA;// .............. Interface to MonteCarlo application 
  TVirtualMCStack       *fStack;
    
    ClassDef(GCall,0) // Extended TParticle
};

#endif //STMC_USER_H   
   

