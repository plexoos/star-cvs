#include "StvDiver.h"
#include "TGeoVolume.h"
#include "TVirtualMCStack.h"
#include "TGeant3TGeo.h"
#include "StarVMC/GeoTestMaker/StVMCApplication.h"
#include "StarVMC/GeoTestMaker/StMCStack.h"
#include "StarVMC/GeoTestMaker/StTGeoHelper.h"
#include "StvUtil/StvNodePars.h"
#include "StarMagField.h"
#include "StvELossTrak.h"
#include "THelixTrack.h"
#include "TRandom.h"

class MyRandom : public TRandom 
{
public:
virtual Double_t Rndm(Int_t i = 0) 				{return 0.5;}
virtual Double_t Gaus(Double_t mean = 0, Double_t sigma = 1)	{return 0.0;}
};
static MyRandom* gMyRandom=0;
static const double gMyPiMass=0.1396;
static const    int gMyPiPdg =9999;





static void mybreak(int key)
{ static int kto=-2010;
  if (kto != key) return;
  printf ("BOT OHO %d\n",key);
}

ClassImp(StvDiver)
//_____________________________________________________________________________
//_____________________________________________________________________________
const StvELossData &StvDiver::GetELossData() const { return mSteps->GetELossData();}
//_____________________________________________________________________________
StvDiver::StvDiver(const char *name):TNamed(name,"")
{
  memset(mBeg,0,mEnd-mBeg+1);
}
//_____________________________________________________________________________
int StvDiver::Init() 
{
  gMyRandom = new MyRandom;
  mHelix = new THelixTrack;
  mELoss = new StvELossTrak;
  StVMCApplication *app = (StVMCApplication*)TVirtualMCApplication::Instance();
  assert(app);
  mSteps =  ( StvMCStepping*)app->GetStepping();
  assert(mSteps);
  mSteps->Set(mHelix);
  mSteps->Set(mELoss);
  mGen = (StvMCPrimaryGenerator*)app->GetPrimaryGenerator();
  mFld = (StvMCField*           )app->GetField();
  mSteps->Set(mFld);
  
  TVirtualMC::GetMC()->DefineParticle( gMyPiPdg,"MyPi+",kPTHadron,gMyPiMass, 1,1e+10
	                             ,"pType", 0., 0, 1, 0, 1, 1, 1, 0, 1, 1);
  TVirtualMC::GetMC()->DefineParticle(-gMyPiPdg,"MyPi-",kPTHadron,gMyPiMass,-1,1e+10
	                             ,"pType", 0., 0, 1, 0, 1, 1, 1, 0, 1, 1);
  return 0;
}
//_____________________________________________________________________________
void StvDiver::SetRZmax(double rMax,double zMax) 
{   
  StVMCApplication *app = (StVMCApplication*)TVirtualMCApplication::Instance();
  app->SetRZmax(rMax,zMax);
}

//_____________________________________________________________________________
void StvDiver::Reset() 
{
 mSteps->Reset();
}
//_____________________________________________________________________________
void StvDiver::SetSkip(int skip) 
{
 mSteps->SetSkip(skip);
}
//_____________________________________________________________________________
int  StvDiver::Dive()
{
  TRandom *myRandom = gRandom;
  gRandom = gMyRandom;

  mFld->SetHz(mInpPars->_hz);	//Set initial value of mag field
  assert(fabs(mInpPars->_hz)<0.01);

  while(1) {
    mInpPars->get(mHelix);
    mInpErrs->Get(mHelix);
    mHlxDeri[0][0]=0;
    if (!mDir) mHelix->Backward();

    TVirtualMC::GetMC()->ProcessEvent();
    if (mSteps->GetExit()==kDiveBreak) return kDiveBreak;

    TVector3 pos = mSteps->CurrentPosition().Vect();
    TVector3 mom = mSteps->CurrentMomentum().Vect();
    mOutPars->_x = pos.X();
    mOutPars->_y = pos.Y();
    mOutPars->_z = pos.Z();
    mOutPars->_psi = mom.Phi();
    mOutPars->_ptin = -mSteps->Charge()/mom.Pt();
    mOutPars->_tanl = mom[2]*fabs(mOutPars->_ptin);
    mOutPars->_hz   = mFld->GetHz();
    mOutPars->ready(); 

    mOutErrs->Set(mHelix,mOutPars->_hz);
    assert(mOutErrs->mPP>0);
    mOutPars->convert(*mOutDeri,mHlxDeri);


    if (!mDir) {
      mOutPars->reverse();
      mOutPars->reverse(*mOutDeri,*mOutDeri);
      mOutErrs->Backward();
      assert(mOutErrs->mPP>0);
    }
    if (mSteps->GetExit() !=kDiveMany) break;
    *mInpPars = *mOutPars;
    double push = (mDir)?  -1:1;
    mInpPars->move(push);
  }
  assert (mInpPars->_ptin * mOutPars->_ptin >=0);
  gRandom = myRandom;

  return mSteps->GetExit();
}
//_____________________________________________________________________________
void StvDiver::Set(StvNodePars *inpar,const StvFitErrs *inerr,int idir)
{
  mInpPars= inpar;
  mInpErrs= inerr;
  mDir    = idir;
  mSteps->SetDir(mDir);
  mGen->Set(inpar,idir);
}
//_____________________________________________________________________________
void StvDiver::Set(StvNodePars *otpar,StvFitErrs *oterr,Mtx55D_t *deriv)
{
  mOutPars = otpar;
  mOutErrs = oterr;
  mOutDeri = deriv;
  mSteps->Set(&mHlxDeri);

}
//_____________________________________________________________________________
double StvDiver::GetLength() const
{
  return TVirtualMC::GetMC()->TrackLength();
}

//_____________________________________________________________________________
//_____________________________________________________________________________
ClassImp(StvMCInitApp)

//_____________________________________________________________________________
StvMCInitApp::StvMCInitApp()  
{}   
//_____________________________________________________________________________
int  StvMCInitApp::Fun()
{
  StVMCApplication  *app = (StVMCApplication*)TVirtualMCApplication::Instance();
  TVirtualMC *myMC  = new TGeant3TGeo("C++ Interface to Geant3"); 
  Info("Init","TGeant3TGeo has been created.");
  StvMCConstructGeometry *geo = new StvMCConstructGeometry(app->GetName());
  app->SetConstructGeometry(geo);
  app->SetPrimaryGenerator(new StvMCPrimaryGenerator());
  app->SetField(new StvMCField);
  StvMCStepping *steps =   new StvMCStepping("");
  app->SetStepping(steps);

  app->SetDebug(0);
  myMC->SetStack(new StMCStack(1));
  myMC->Init();
  myMC->BuildPhysics(); 
  ((TGeant3*)myMC)->SetDEBU(0,0,0); 
  ((TGeant3*)myMC)->SetMaxNStep(-1000);

  Gcphys_t* gcphys = ((TGeant3*)myMC)->Gcphys(); if (gcphys){}

  Info("Init","switch off physics");
  myMC->SetProcess("DCAY", 0);
  myMC->SetProcess("ANNI", 0);
  myMC->SetProcess("BREM", 0);
  myMC->SetProcess("COMP", 0);
  myMC->SetProcess("HADR", 0);
  myMC->SetProcess("MUNU", 0);
  myMC->SetProcess("PAIR", 0);
  myMC->SetProcess("PFIS", 0);
  myMC->SetProcess("PHOT", 0);
  myMC->SetProcess("RAYL", 0);
  myMC->SetProcess("LOSS", 4); // no fluctuations 
  //  myMC->SetProcess("LOSS 1"); // with delta electron above dcute
  myMC->SetProcess("DRAY", 0);
  myMC->SetProcess("MULS", 0);
  myMC->SetProcess("STRA", 0);
  myMC->SetCut("CUTGAM",	1e-3  );
  myMC->SetCut("CUTELE", 	1e-3  );
  myMC->SetCut("CUTHAD", 	.001  );
  myMC->SetCut("CUTNEU", 	.001  );
  myMC->SetCut("CUTMUO", 	.001  );
  myMC->SetCut("BCUTE", 	.001  );
  myMC->SetCut("BCUTM", 	.001  );
  myMC->SetCut("DCUTE", 	1e-3  );
  myMC->SetCut("DCUTM", 	.001  );
  myMC->SetCut("PPCUTM", 	.001  );
  myMC->SetCut("TOFMAX", 	50.e-6);

  return 0;
}
//_____________________________________________________________________________
//_____________________________________________________________________________
ClassImp(StvMCStepping)

//_____________________________________________________________________________
StvMCStepping::StvMCStepping(const char *name,const char *tit)
  : StMCStepping(name,tit)
{
   memset(fFist,0,fLast-fFist);
}   
//_____________________________________________________________________________
StvMCStepping::~StvMCStepping()
{
}
//_____________________________________________________________________________
void StvMCStepping::Reset() 
{
  memset(fFist,0,fMidl-fFist);
  fELossTrak->Reset();
}		
//_____________________________________________________________________________
void StvMCStepping::Print(const Option_t*) const
{
}		
//_____________________________________________________________________________
int StvMCStepping::Fun()
{
static int nCall = 0;
nCall++;
mybreak(nCall);
  TString ts,modName;
  fKount++;
  Case();

  assert(fCurrentLength< 10000);
  assert(fEnterLength  < 10000);
  
//   StTGeoHelper::Instance()->Print(KazeAsString(fKaze));
//   printf("fEnterLength=%g fCurrentLength=%g Rxy=%g Z=%g\n\n"
//         , fEnterLength, fCurrentLength,fCurrentPosition.Perp(),fCurrentPosition.Z());
SWITCH: int myKaze = fKaze;
if (GetDebug()) {printf("%d - ",nCall); Print();}

  switch (fKaze) {
    case kNEWtrack:;

    case kENTERtrack:;
         if (strcmp(fVolume->GetName(),"HALL")==0) fKaze=kENDEDtrack;
         if (!StTGeoHelper::Inst()->GetHitShape()->Inside(fCurrentPosition.Z(),fCurrentPosition.Perp()))
	    fKaze=kENDEDtrack;
         if (fKaze==kENDEDtrack) break;
         if ((fExit = BegVolume())) fKaze=kENDEDtrack;
    break;
    
    case kCONTINUEtrack:
    case kIgnore:
    break;
    
    case kOUTtrack:
    case kENDEDtrack:
      fExit=kDiveBreak;
      TVirtualMC::GetMC()->StopTrack();
      break;

    case kEXITtrack:
    {
      fExit = EndVolume();
      if (!fExit) {
        if (fSkip) 			break;
        StTGeoHelper *tgh = StTGeoHelper::Instance();
        if (!tgh->IsSensitive(fVolume)) break;
        if (!tgh->IsActive(0)) 		break;
      }
      TVirtualMC::GetMC()->StopTrack();
    }
    break;

    default:
     Error("Case","Unexpected case %x == %s",fCase,fCasName.Data());
     assert(0);
  }
  if (fKaze!=myKaze) 			goto SWITCH;
  if (fExit) 				return 0;
  if (fKaze!=fLastKaze) {fLastNumb=0; fLastKaze=fKaze; return 0;}
  fLastNumb++; if (fLastNumb<100)	return 0;
  fExit=kDiveMany; fLastNumb=0;
  return 0;
}		
//_____________________________________________________________________________
int StvMCStepping::BegVolume()
{
static int nCall=0; nCall++;

  fELossTrak->Reset(); 
  fPrevMat = fMaterial;
  fELossTrak->Set(fMaterial->GetA(),fMaterial->GetZ(),fMaterial->GetDensity(), fX0);
  fELossTrak->Set(fEnterMomentum.Vect().Mag());
  return (IsDca00(0));
}
//_____________________________________________________________________________
int StvMCStepping::EndVolume()
{
static int nCall=0; nCall++;
  double pos[4],mom[4];

  int isDca = (IsDca00(1));
  if (isDca>=kDiveBreak) return isDca;
  double dL = fCurrentLength-fEnterLength;
  if (dL<1e-6) return isDca;
  fCurrentPosition.GetXYZT(pos);
  fCurrentMomentum.GetXYZT(mom);
  double pt = fCurrentMomentum.Pt();
  double curva = -fField->GetHz()/pt*fCharge;
  double rho   = fHelix->GetRho();
  if (fabs(pos[2])<200) {
    assert (curva*rho>=-1e-6);

    assert (fabs(curva)+fabs(rho)< 1e-4 
          ||fabs(curva-rho)<=0.5*(fabs(curva)+fabs(rho))*(dL+1));
  } 

  fELossTrak->Add(dL);
  fHelix->Set((2*rho+curva)/3);

  if (!(*fDeriv)[0][0]) {	//first time
    fHelix->Move(dL,*fDeriv);
  } else {
    double T[5][5],R[5][5];
    fHelix->Move(dL,T);
    Multiply(R,T,*fDeriv);
    memcpy((*fDeriv)[0],R[0],sizeof(R));
  }

  assert(fabs(fHelix->Pos()[0]-pos[0])<0.3*(dL+1));
  assert(fabs(fHelix->Pos()[1]-pos[1])<0.3*(dL+1));
  assert(fabs(fHelix->Pos()[2]-pos[2])<0.3*(dL+1));


  THEmx_t *emx = fHelix->Emx();
  assert(emx->mCC>0);
  fELossData.mTheta2 = fELossTrak->GetTheta2();
  fELossData.mOrt2   = fELossTrak->GetOrt2();
  double cosL = fHelix->GetCos();
  double tanL = fHelix->GetTan();
  emx->mAA+=fELossData.mTheta2/(cosL*cosL);
  emx->mLL+=fELossData.mTheta2;
  emx->mCC+=fELossData.mTheta2*pow(curva*tanL,2);
  emx->mCL-=fELossData.mTheta2*curva*tanL;

  emx->mHH+= fELossData.mOrt2;
  emx->mZZ+= fELossData.mOrt2/(cosL*cosL);
//  double eerr2= fELossTrak->ELossErr2();
  fHelix->Set(pos,mom,curva);
  return isDca;
}
//_____________________________________________________________________________
int StvMCStepping::IsDca00(int begEnd)
{
  fCurrentSign = fCurrentMomentum[0]*fCurrentPosition[0]
               + fCurrentMomentum[1]*fCurrentPosition[1];
  switch (begEnd) {

    case 0: { // begin volume
      fStartSign = fCurrentSign; 
      if ((fCurrentSign<0)==(fDir==0)) return 0;
      return kDiveBreak;
    }

    case 1: { // end volume
      double dL = fCurrentLength-fEnterLength;
      if (dL<1e-6) return 0;
      if ((fCurrentSign<0)==(fDir==0)) return 0;
      THelixTrack th(*fHelix);
      double dcaL = th.Path(0.,0.);
      if (dcaL< 0) return kDiveBreak;
      double curva = -fField->GetHz()/fCurrentMomentum.Pt()*fCharge;
      double rho = fHelix->GetRho();
      curva = (rho*(dL-dcaL)+dcaL*curva)/dL;
      th.Set((2*rho+curva)/3);
      dcaL = th.Path(0.,0.);
  //		Update end position
      th.Move(dcaL);
      fCurrentLength=fEnterLength+dcaL;
      fCurrentPosition.SetVect(TVector3(th.Pos()));
      double pt = fabs(fField->GetHz()*fCharge/curva);
      double p  = pt/th.GetCos();
      fCurrentMomentum.SetVectM(TVector3(th.Dir())*p,fMass);
      return kDiveDca;
    }
  }
  return kDiveBreak;
}

//_____________________________________________________________________________
//_____________________________________________________________________________
void StvMCStepping::Finish(const char *opt)
{
}
ClassImp(StvMCConstructGeometry)

//_____________________________________________________________________________
StvMCConstructGeometry::StvMCConstructGeometry(const char *gy)
  : GCall(gy,"StvMCConstructGeometry")
{
}   
//_____________________________________________________________________________
int  StvMCConstructGeometry::Fun()
{
  TVirtualMC *myMC = TVirtualMC::GetMC();
  myMC->SetRootGeometry();
  Info("Init","switch off physics");
  myMC->SetProcess("DCAY", 0);
  myMC->SetProcess("ANNI", 0);
  myMC->SetProcess("BREM", 0);
  myMC->SetProcess("COMP", 0);
  myMC->SetProcess("HADR", 0);
  myMC->SetProcess("MUNU", 0);
  myMC->SetProcess("PAIR", 0);
  myMC->SetProcess("PFIS", 0);
  myMC->SetProcess("PHOT", 0);
  myMC->SetProcess("RAYL", 0);
  myMC->SetProcess("LOSS", 4); // no fluctuations 
  //  myMC->SetProcess("LOSS 1"); // with delta electron above dcute
  myMC->SetProcess("DRAY", 0);
  myMC->SetProcess("MULS", 0);
  myMC->SetProcess("STRA", 0);
  myMC->SetCut("CUTGAM",	1e-3  );
  myMC->SetCut("CUTELE", 	1e-3  );
  myMC->SetCut("CUTHAD", 	.001  );
  myMC->SetCut("CUTNEU", 	.001  );
  myMC->SetCut("CUTMUO", 	.001  );
  myMC->SetCut("BCUTE", 	.001  );
  myMC->SetCut("BCUTM", 	.001  );
  myMC->SetCut("DCUTE", 	1e-3  );
  myMC->SetCut("DCUTM", 	.001  );
  myMC->SetCut("PPCUTM", 	.001  );
  myMC->SetCut("TOFMAX", 	50.e-6);
  


  return 0;
}
//_____________________________________________________________________________
//_____________________________________________________________________________
ClassImp(StvMCPrimaryGenerator);
//_____________________________________________________________________________
StvMCPrimaryGenerator::StvMCPrimaryGenerator()
{
 mPars=0;
 mDir=0; // direction of moving 1=along track; 0=opposite
}  

//_____________________________________________________________________________
int StvMCPrimaryGenerator::Fun() 
{     

 // Add one primary particle to the user stack (derived from TVirtualMCStack).
 // Track ID (filled by stack)
 // Option: to be tracked

 int toBeDone = 1; 
 
 // Particle type
 int pdg  = (mPars->getCharge()>0)? gMyPiPdg:-gMyPiPdg;
 // Particle momentum
 double p[3];
 mPars->getMom(p);

 if (!mDir) { pdg = -pdg; p[0]=-p[0]; p[1]=-p[1];p[2]=-p[2];}
 
 // Polarization
static const double polx = 0.,poly = 0.,polz = 0.; 

 // Position
static  const double tof = 0.;
 
 double e  = sqrt(gMyPiMass*gMyPiMass + mPars->getP2());
 // Add particle to stack 
 assert(e>1e-6);
 int ntr=1;
 TVirtualMC::GetMC()->GetStack()->Clear();
 TVirtualMC::GetMC()->GetStack()->PushTrack(
  	 toBeDone,-1,pdg,p[0],  p[1],  p[2],e 
	,mPars->_x,  mPars->_y, mPars->_z,tof
        ,polx,poly,polz, kPPrimary, ntr, 1.,0);
 return 0;
}
//_____________________________________________________________________________
//_____________________________________________________________________________
ClassImp(StvMCField)
//_____________________________________________________________________________
StvMCField::StvMCField() 
{
  mHz = 3e33;
  mFild =  StarMagField::Instance();
}
//_____________________________________________________________________________
int StvMCField::FunDD(const double *x,double *b) 
{     
  if (!mFild) { mFild =  StarMagField::Instance();assert(mFild);}
static const Double_t EC = 2.99792458e-4;
  mFild->BField(x,b); 
  mHz = b[2]*EC;
  if (fabs(mHz) < 1e-5) mHz=1e-5;
  return 0;
}
  
  
