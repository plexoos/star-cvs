//*-- Author : Jan Balewski
//  
// $Id: histoLPfind.cxx,v 1.2 2001/02/28 19:06:12 balewski Exp $
// $Log: histoLPfind.cxx,v $
// Revision 1.2  2001/02/28 19:06:12  balewski
// some reorganizations
//
// Revision 1.1.1.1  2001/01/31 14:00:07  balewski
// First release
//
//
//////////////////////////////////////////////////////////////////////////
//                                                                    
//   histograms initialization for this class
//                                                                    
//////////////////////////////////////////////////////////////////////////

#include <math.h>
#include <stdio.h>
#include "StppLPfindMaker.h"

#include "TH2.h"

//_______________________________________________________________
//_______________________________________________________________
//_______________________________________________________________
void StppLPfindMaker::init_histo()
{
  float pt0=0., pt1=15.;
  int npt=(int)pt1;

  printf("init histo\n");
  hv[0]=new TH1F("R 00v","Zvertex/cm Geant",50, -30., 30.);
  hv[1]=new TH1F("R 01v","#Delta Zvertex/cm Geant-LMV",100, -5., 5.);
  hv[2]=new TH1F("R 02v","#Delta Zvertex/cm Geant-LMV",200, -200., 200.);

  hv[3]=new TH1F("drh","Dr for hit2hit matching, (cm)",100,0.,2.);
  hv[4]=new TH1F("gpt0","all Pythia events vs. gPt (GeV/c)",npt,pt0,pt1);
  hv[5]=new TH1F("gpt1","all recon events with any rLP vs. gPt (GeV/c)",npt,pt0,pt1);
  hv[6]=new TH1F("gpt2","accepted recon events with any rLP vs. gPt (GeV/c)",npt,pt0,pt1);

  char *tt0[]={"1st", "2nd","no","all"};

  printf("init histo 2\n");
  int ilp;
  
  for(ilp=0;ilp<MX_LPA;ilp++)
   {
     struct RecStep *s=&(lpa[ilp].Inp);
     for(int i=0;i<sizeof(tt1)/sizeof(char*);i++,s++)
       {
	 s->n=0;
	 TString Tt1=tt1[i];
	 TString Tt0=tt0[ilp];
	 s->h=new TH1D((tt0[ilp]+Tt1).Data(),(Tt0+"LeadPart, "+Tt1+" accepted vs. gener. pT (GeV/c)").Data(),npt, pt0, pt1); 
	 s->h->SetXTitle("Generated Leading Particle pT (GeV/c)") ;
	 //printf(" %d %d histo=%s set\n",ilp,i,(tt0[ilp]+Tt1).Data());
       }
   }


  printf("init histo .. done\n");

}

//_______________________________________________________________
//_______________________________________________________________
//_______________________________________________________________
void StppLPfindMaker::printStat()
{
  printf("\nSummary of %s after %d input events\n",GetName(),nEVtot);
  
  printf("                        1st Lead Part                  2nd  Lead Part \n");
  printf("  STEP                 Neve   eff  +- err             Neve   eff  +- err\n");
  struct RecStep *s1=&(lpa[0].Inp);  
  struct RecStep *s2=&(lpa[1].Inp);  
  float in=s1->n;
  float last1=0,last2=0;  
  for(int i=0;i<sizeof(tt1)/sizeof(char*);i++,s1++,s2++) {
    float ef1=-1,er1=-1;
    float ef2=-1,er2=-1;
    if(in>0.) {
      ef1=s1->n/in;
      er1=sqrt(s1->n*(in-s1->n)/in)/in;
      ef2=s2->n/in;
      er2=sqrt(s2->n*(in-s2->n)/in)/in;
    }
    if(i==0) {last1=s1->n; last2=s2->n;}
    int lost1=(int)((1.-s1->n/last1)*100.+.5);
    int lost2=(int)((1.-s2->n/last2)*100.+.5);
    //printf("%1d %4s (F=%2d%c) T= %5d  %6.3f  %6.3f    (F=%2d%c) T= %5d  %6.4f  %6.4f\n",i+1,tt1[i],lost1,'%',s1->n,ef1,er1,lost2,'%',s2->n,ef2,er2);
    printf("%1d %4s (F=%2d%c) T= %5d  %6.3f  %6.3f \n",i+1,tt1[i],lost1,'%',s1->n,ef1,er1
);
    last1=s1->n;
    last2=s2->n;
  }
  printf("\n");
}

