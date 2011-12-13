#include <stdio.h>
#include <stdlib.h>
#include <algorithm>

#include "Jevp/StJevpPlot/JevpPlotSet.h"
#include "DAQ_READER/daqReader.h"
#include "DAQ_READER/daq_dta.h"
#include "DAQ_READER/daq_det.h"
#include "DAQ_TOF/daq_tof.h"
#include <TLatex.h>
#include <TPad.h>
#include <TH1I.h>
#include <TH2F.h>

#include <math.h>
#include "tofBuilder.h"
#include <RTS/include/rtsLog.h>
#include <assert.h>
// This is the one PlotSet that is guarenteed to always exist
// It's main purpose is to provide the run information 
// To the server...
//
// It has no plots (currently)
//


ClassImp(tofBuilder);
  
tofBuilder::tofBuilder() {
  plotsetname = (char *)"tof";

  np = sizeof(contents) / sizeof(TH1 *);
  int npp = sizeof(plots) / sizeof(JevpPlot *);

  assert(np <= npp);

  // start with histograms undefined...
  memset(&contents, 0, sizeof(contents));
  memset(&extra, 0, sizeof(extra));

  memset(TOF_L0_trg_labels, 0, sizeof(contents));
  TOF_Error1_label = NULL;
  TOF_Error2_labela = NULL;
  TOF_Error2_labelb = NULL;
}

tofBuilder::~tofBuilder() {

  // Delete any existing histograms...
  int n = sizeof(contents) / sizeof(TH1 *);
  for(int i=0;i<n;i++) {
    if(contents.array[i]) delete contents.array[i];
  }

  n = sizeof(extra) / sizeof(TH1 *);
  for(int i=0;i<n;i++) {
    if(extra.array[i]) delete extra.array[i];
  }
}

void tofBuilder::initialize(int argc, char *argv[]) {

  // Initialization of histograms.
  for(int i=0;i<NTRAYS;i++) {
    static char tmpchr[255];
    static char tmpchr1[255];

    sprintf(tmpchr,"TOF_Tray%03d_trg_L0",i+1);
    sprintf(tmpchr1,"Tray %03d L0 (trg)",i+1);
    contents.TOF_L0_trg[i]=new TH1F(tmpchr,tmpchr1,32,-0.5,31.5);

    sprintf(tmpchr,"TOF_Tray%03d_LE_hitmap",i+1);
    sprintf(tmpchr1,"Tray %03d Leading hitmap",i+1);
    contents.TOF_Tray_LEhitmap[i] = new TH1F(tmpchr,tmpchr1,192,-0.5,191.5);
    contents.TOF_Tray_LEhitmap[i]->SetLineColor(2);

    sprintf(tmpchr,"TOF_Tray%03d_TE_hitmap",i+1);
    sprintf(tmpchr1,"Tray %03d Trailing hitmap",i+1);
    extra.TOF_Tray_TEhitmap[i] = new TH1F(tmpchr,tmpchr1,192,-0.5,191.5);
    extra.TOF_Tray_TEhitmap[i]->SetLineColor(4);
  }
  
  contents.TOF_L1mult_vs_ZDCadcsum=new TH2F("TOF_L1_vs_ZDCadcsum","TOF_L1_vs_ZDCadcsum",144,0.5,2880.5,150,0,3000);
  contents.TOF_L1mult_vs_ZDCadcsum->SetXTitle("TOF L1 Mult");
  contents.TOF_L1mult_vs_ZDCadcsum->SetYTitle("ZDC hardware adc Sum");

  contents.TOF_L1mult_vs_sumL0=new TH2F("TOF_L1_vs_sumL0","TOF_L1_vs_sumL0",144,0.5,2880.5,144,0.5,2880.5);
  contents.TOF_L1mult_vs_sumL0->SetXTitle("TOF L1 Mult");
  contents.TOF_L1mult_vs_sumL0->SetYTitle("Sum L0 of hits");


  contents.TOF_Error1 = new TH1F("TOF_Error1","TOF electronics error ",244,0.5,125.5);
  contents.TOF_Error1->SetXTitle("Tray #");
  contents.TOF_Error2=new TH1F("TOF_Error2","TOF bunchid shift error",122,0.5,122.5);
  contents.TOF_Error2->SetXTitle("THUB upvpd MTD");
  contents.TOF_Error3=new TH1F("TOF_Error3","TOF bunchid shift error",122,0.5,122.5);
  contents.TOF_EventCount=new TH1F("TOF_EventCount","TOF_EventCount",2,0,2);
  contents.TOF_Tray_hits1=new TH1F("TOF_Tray_hits1","TOF Hits by TrayHalf",244,0.5,122.5);
  contents.TOF_Tray_hits2=new TH1F("TOF_Tray_hits2","TOF Hits by TrayHalf",244,0.5,122.5);

  contents.upvpd_hitmap[0] = new TH1F("upvpd_LE_hitmap","upvpd Leading edge hitmap",54,-0.5,53.5);
  contents.upvpd_hitmap[0]->SetXTitle("PMT #");
  contents.upvpd_hitmap[1] = new TH1F("upvpd_TE_hitmap","upvpd Trailing edge hitmap",54,-0.5,53.5);
  contents.upvpd_hitmap[1]->SetXTitle("PMT #");

  contents.upvpd_ToT = new TH2F("upvpd_ToT","upvpd ToT vs PMT#",54,-0.5,53.5,50,0,50);
  contents.upvpd_ToT->SetXTitle("PMT #");

  contents.upvpd_eastT_vs_westT=new TH2F("upvpd_eastT_vs_westT","upvpd eastT vs westT",400,0,51200,400,0,51200);
  contents.upvpd_eastT_vs_westT->SetXTitle("east time (ns)");



  int n=0;
  for(n=0;n<NTRAYS;n++) {
    plots[n] = new JevpPlot(contents.TOF_L0_trg[n]);
    plots[n]->optstat = 0;
    plots[n]->logy = 1;
  }
  n--;

  plots[++n] = new JevpPlot(contents.TOF_L1mult_vs_ZDCadcsum);
  plots[n]->setDrawOpts("colz");
  // plots[n]->optstat = 0;
  plots[++n] = new JevpPlot(contents.TOF_L1mult_vs_sumL0);
  plots[n]->setDrawOpts("colz");
  // plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.TOF_Error1);
  TOF_Error1_label = new TLatex();
  TOF_Error1_label->SetNDC();
  plots[n]->addElement(TOF_Error1_label);

  plots[++n] = new JevpPlot(contents.TOF_Error2);
  TOF_Error2_labela = new TLatex();
  TOF_Error2_labela->SetNDC();
  TOF_Error2_labelb = new TLatex();
  TOF_Error2_labelb->SetNDC(); 
  plots[n]->gridy = 0;
  plots[n]->addElement(TOF_Error2_labela);
  plots[n]->addElement(TOF_Error2_labelb);
  
  JLatex *l;
  l = new JLatex(.25, .12, "THUB1-NW0");
  l->SetTextSize(.035);
  l->SetTextAlign(13);
  l->SetTextAngle(90);
  plots[n]->addElement(l);

  l = new JLatex(*l);
  l->SetText("THB1-NE0");
  l->SetX(1.25);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("THB1-SW0");
  l->SetX(2.25);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("THB1-SE0");
  l->SetX(3.25);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("upvpd-west0");
  l->SetX(4.25);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("upvpd-east0");
  l->SetX(5.25);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("MTD0");
  l->SetX(6.25);
  plots[n]->addElement(l);

  l = new JLatex(*l);
  l->SetY(.2);
  l->SetText("THUB1-NW1");
  l->SetX(.75);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("THB1-NE1");
  l->SetX(1.75);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("THB1-SW1");
  l->SetX(2.75);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("THB1-SE1");
  l->SetX(3.75);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("upvpd-west1");
  l->SetX(4.75);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("upvpd-east1");
  l->SetX(5.75);
  plots[n]->addElement(l);
  l = new JLatex(*l);
  l->SetText("MTD1");
  l->SetX(6.75);
  plots[n]->addElement(l);

  
  
  plots[++n] = new JevpPlot(contents.TOF_Error3);
  plots[++n] = new JevpPlot(contents.TOF_EventCount);

  for(int i=0;i<NTRAYS;i++) {
    n++;
    plots[n] = new JevpPlot(contents.TOF_Tray_LEhitmap[i]);
    plots[n]->addHisto(extra.TOF_Tray_TEhitmap[i]);
    plots[n]->optstat = 0;
    //plots[n]->logy = 1;
  }
  
  plots[++n] = new JevpPlot(contents.upvpd_hitmap[0]);

  JLine *ln;
  //  JLatex *l;

  contents.upvpd_hitmap[0]->SetFillColor(19);
  ln = new JLine(18.5,.1,18.5,.9);
  ln->SetLineColor(4);
  plots[n]->addElement(ln);
  ln = new JLine(37.5,.1,37.5,.9);
  ln->SetLineColor(4);
  plots[n]->addElement(ln);
  l = new JLatex(7.5, .8, "west side");
  plots[n]->addElement(l);
  l = new JLatex(26.5, .8, "east side");
  plots[n]->addElement(l);
  l = new JLatex(45.5, .8, "PP2PP");
  plots[n]->addElement(l);
  plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.upvpd_hitmap[1]);

  contents.upvpd_hitmap[1]->SetFillColor(19);
  ln = new JLine(18.5,.1,18.5,.9);
  ln->SetLineColor(4);
  plots[n]->addElement(ln);
  ln = new JLine(37.5,.1,37.5,.9);
  ln->SetLineColor(4);
  plots[n]->addElement(ln);
  l = new JLatex(7.5, .8, "west side");
  plots[n]->addElement(l);
  l = new JLatex(26.5, .8, "east side");
  plots[n]->addElement(l);
  l = new JLatex(45.5, .8, "PP2PP");
  plots[n]->addElement(l);
  plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.upvpd_ToT);
  plots[n]->setDrawOpts("colz");

  ln = new JLine(18.5,.1,18.5,.9);
  ln->SetLineColor(44);
  plots[n]->addElement(ln);
  ln = new JLine(37.5,.1,37.5,.9);
  ln->SetLineColor(44);
  plots[n]->addElement(ln);
  l = new JLatex(7.5, .8, "west side");
  plots[n]->addElement(l);
  l = new JLatex(26.5, .8, "east side");
  plots[n]->addElement(l);
  l = new JLatex(45.5, .8, "PP2PP");
  plots[n]->addElement(l);
  plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.upvpd_eastT_vs_westT);
  plots[n]->setDrawOpts("col");
  plots[n]->optstat = 0;
 
  // Add Plots to plot set...
  for(int i=0;i<=n;i++) {
    LOG(DBG, "Adding plot %d",i);
    addPlot(plots[i]);
  }
}
  
void tofBuilder::startrun(daqReader *rdr) {
  //printf("start\n");
  LOG("JEFF", "tofBuilder starting run #%d",rdr->run);
  resetAllPlots();

  //printf("reset\n");

  nevts=0;
  nerr1=0;
  nerr2=0;
  nerr3=0;

  //printf("tray list\n");

  ReadTrayList();
  ReadValidBunchidPhase();
  ReadTraymaskoutList();

  long TOF_L0_trg_idx = ((long)&contents.TOF_L0_trg[0] - (long)&contents) / 4;

  //printf("labels\n");

  for(int i=0;i<NTRAYS;i++) {
    LOG(DBG, "tray %d   (idx=%d)",i, TOF_L0_trg_idx);

    if(TOF_L0_trg_labels[i]) {
      // remove element deletes it!
      plots[i+TOF_L0_trg_idx]->removeElement(TOF_L0_trg_labels[i]);
      //delete TOF_L0_trg_labels[i];
      TOF_L0_trg_labels[i] = NULL;
    }

   
    LOG(DBG, "checking... tray %d",i);

    if(NotActiveTray[i+1]) {
      LOG(DBG, "Tray %d not active",i+1);
      TOF_L0_trg_labels[i] = new TLatex(.2,.8,"Not Active");
      TOF_L0_trg_labels[i]->SetNDC();
      plots[i+TOF_L0_trg_idx]->addElement(TOF_L0_trg_labels[i]);
    }
    else if (MaskoutTray[i+1]) {
      LOG(DBG,"Tray %d masked out\n",i+1);
      TOF_L0_trg_labels[i] = new TLatex(.2,.8,"Masked Out");
      TOF_L0_trg_labels[i]->SetNDC();
      plots[i+TOF_L0_trg_idx]->addElement(TOF_L0_trg_labels[i]);
    } 
    
    LOG(DBG, "checkinged tray %d",i);
    
  }

  // printf("done\n");
}

int tofBuilder::Get_TOFTHUB(int trayid){

  int trayinTHUB1[30]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,51,52,53,54,55,56,57,58,59,60};
  int trayinTHUB2[30]={21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50};
  int trayinTHUB3[30]={61,62,63,64,65,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};
  int trayinTHUB4[30]={66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};

  if(trayid<1 || trayid>120) return -1;

  int nthub=-1;

  for(int itray=0;itray<30;itray++){
    if(trayid == trayinTHUB1[itray]) nthub=0;
    if(trayid == trayinTHUB2[itray]) nthub=1;
    if(trayid == trayinTHUB3[itray]) nthub=2;
    if(trayid == trayinTHUB4[itray]) nthub=3;
  }

  return nthub;  

}

int tofBuilder::TDIGChan2TINOChan(int tdc,int chan)
{
  int tdcid[24]  ={0,1,0,1,0,1,2,0,2,0,1,0,1,2,0,2,0,2,2,1,2,1,2,1};
  int tdcchan[24]={7,7,0,2,5,6,7,4,4,2,3,6,0,2,3,3,1,6,0,5,1,4,5,1};
  int tinoid[24] ={2,2,2,2,2,2,1,1,1,1,2,2,0,0,1,1,1,1,0,0,0,0,0,0};
  int index=-1;
  for(int i=0;i<24;i++){
    if(tdcid[i]==tdc && tdcchan[i]==chan) {index=i;break;}
  }
  if(index<0) cout<<" tdc="<<tdc<<" chan="<<chan<<" not valid!!!"<<endl;
  return tinoid[index];
}

int tofBuilder::parseData(daqReader *rdr)
{
  //printf("parsedata\n");

  int halftrayid=-1;
  int trayid=-1;
  int bunchid=0;
  memset(tinohit, 0, sizeof(tinohit));
  for(int i=0;i<2;i++) for(int j=0;j<122;j++) allbunchid[i][j] = -9999;

  // Get TOF data...
  daq_dta *dd = rdr->det("tof")->get("legacy");
  tof_t *tof;
  if(dd) {
    while(dd->iterate()) {
      tof = (tof_t *)dd->Void;

      // TOF trigger window per THUB, subject to change anytime.
      //int trigwindowLow[4]={2830,2840,2910,2910};
      //int trigwindowHigh[4]={2910,2920,2990,2990};
      int trigwindowLowpertray[120]={
	2809,2809,2806,2808,2809, 2809,2809,2809,2809,2810, 2809,2809,2809,2809,2809,
	2809,2808,2809,2809,2808, 2809,2809,2809,2809,2806, 2809,2818,2818,2818,2818,
	2818,2824,2826,2822,2828, 2826,2827,2825,2827,2819, 2821,2821,2820,2818,2809,
	2809,2809,2809,2809,2809, 2784,2784,2784,2795,2796, 2797,2797,2797,2809,2809,
	2884,2884,2871,2873,2872, 2883,2877,2884,2878,2883, 2884,2884,2884,2884,2885,
	2884,2892,2894,2895,2893, 2894,2893,2892,2894,2884, 2884,2883,2883,2884,2875,
	2876,2877,2875,2876,2878, 2884,2884,2890,2884,2897, 2897,2897,2897,2896,2897,
	2896,2896,2894,2896,2897, 2896,2884,2891,2884,2884, 2884,2885,2883,2884,2884
      };
      int trigwindowHighpertray[120];
      for(int iii=0;iii<120;iii++)
	trigwindowHighpertray[iii]=trigwindowLowpertray[iii]+80;

      for(int ifib=0;ifib<4;ifib++){
	int ndataword = tof->ddl_words[ifib];    // 
	if(ndataword<=0) continue;
	for(int iword=0;iword<ndataword;iword++){
	  int dataword=tof->ddl[ifib][iword];

	  // error stuff...
	  int packetid = (dataword&0xF0000000)>>28;
	  if(!ValidDataword(packetid)) {
	    
	    // ignore tray95-0 error
	    if(trayid != 95) {
	      contents.TOF_Error1->Fill(trayid+0.5*halftrayid); 
	      nerr1++;
	    }
	  }

	  if(ndataword<=0) continue;
	  if( (dataword&0xF0000000)>>28 == 0xD) continue;  
	  if( (dataword&0xF0000000)>>28 == 0xE) continue;  
	  if( (dataword&0xF0000000)>>28 == 0xA) {  // header trigger data flag
	    // do nothing at this moment.
	    continue;
	  }
	  // geographical data words for tray number.
	  if( (dataword&0xF0000000)>>28 == 0xC) {
	    halftrayid = dataword&0x01;    
	    trayid     = (dataword&0x0FE)>>1;
	    continue;
	  }

	  if(trayid < 1 || trayid > 124) continue;  
	  if( (dataword&0xF0000000)>>28 == 0x6) {continue;}
	  if( (dataword&0xF0000000)>>28 == 0x2) {
	    bunchid=dataword&0xFFF;
	    allbunchid[halftrayid][trayid-1] = bunchid;
	    continue;  
	  }
	  //
	  int edgeid =int( (dataword & 0xf0000000)>>28 );

	  int tdcid=(dataword & 0x0F000000)>>24;  // 0-15
	  int tdigboardid=tdcid/4;   // 0-3 for half tray.
	  int tdcchan=(dataword&0x00E00000)>>21;          // tdcchan is 0-7 here.
	  int globaltdcchan=tdcchan + (tdcid%4)*8+tdigboardid*24+96*halftrayid; // 0-191 for tray

	  int timeinbin=((dataword&0x7ffff)<<2)+((dataword>>19)&0x03);  // time in tdc bin
	  int time = timeinbin * 25./1024;   // time in ns 


	  if((trayid == 121) || (trayid == 122)) {    // handle upvpd...
	    int moduleid=-1;
	    int modulechan=-1;
	    int globalmodulechan=-1;
	    
	    moduleid=trayid;
	    modulechan=tdcchan2upvpdPMTchan(globaltdcchan,edgeid,trayid);
	    globalmodulechan=modulechan;
	  
	    contents.upvpd_hitmap[edgeid-4]->Fill(modulechan);  
	    numberforsort= time+globalmodulechan*1.e5+trayid*1.e8;
	    if(edgeid==4) leadinghits.push_back(numberforsort);
	    if(edgeid==5) trailinghits.push_back(numberforsort);
	  }

	  if(edgeid==4) {
	    if((trayid >= 1) && (trayid<=120))
	      contents.TOF_Tray_LEhitmap[trayid-1]->Fill(globaltdcchan);
	  }

	  if(edgeid==5) {
	    if((trayid >= 1) && (trayid<=120))
	      extra.TOF_Tray_TEhitmap[trayid-1]->Fill(globaltdcchan);
	  }

	  if(edgeid !=4) continue;    // leading edge data is enough


	  float trgTime = 25.*bunchid;
	  float timeDiff = time- trgTime;
	  while(timeDiff<0) timeDiff += 51200;

	  //int thub=Get_TOFTHUB(trayid);
	  //if(timeDiff<trigwindowLow[thub] || timeDiff>trigwindowHigh[thub]) continue;   // trigger window cut

	  if(timeDiff<trigwindowLowpertray[trayid-1] || timeDiff>trigwindowHighpertray[trayid-1]) continue;

	  //
	  int atdig = globaltdcchan/24;   // 0,1,....7
	  int atdcid  = globaltdcchan/8;    // 0,1,....24    
	  int ahptdcid = atdcid%3;
	  int atdcchan = globaltdcchan%8;
	  int tinoid=TDIGChan2TINOChan(ahptdcid,atdcchan);
	  int tinoidx = atdig*3 + tinoid;
	  //
	  tinohit[trayid-1][tinoidx]++; 
	  
	  if((edgeid !=4) && (edgeid != 5)) continue;
	  
	  if(halftrayid==0) contents.TOF_Tray_hits1->Fill(trayid-0.5);
	  if(halftrayid==1) contents.TOF_Tray_hits2->Fill(trayid);
	}  // end loop nword
      }  // end loop fiber
    }
    return 1;
  }
  else {
    return 0;
  }
}

bool tofBuilder::ValidDataword(int packetid)
{
  if(packetid == 0x2) return true;
  if(packetid == 0xD) return true;
  if(packetid == 0xE) return true;
  if(packetid == 0xA) return true;
  if(packetid == 0xC) return true;
  if(packetid == 0x4) return true;
  if(packetid == 0x5) return true;

  return false;

}

// int tofBuilder::ValidBunchid(int trayid,int halftrayid,int bunchid,int refbunchid)
// {
//   if(trayid<1 || trayid>122) return -1;
//   if(trayid == 123) return -1;
//   // THUB NW
//   int trayinTHUB1[30]={21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50};
//   // THUB NE 
//   int trayinTHUB2[30]={66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};
//   // THUB SW
//   int trayinTHUB3[30]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,51,52,53,54,55,56,57,58,59,60};
//   // THUB SE
//   int trayinTHUB4[30]={61,62,63,64,65,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};
 
//   // the following numbers are moved into TOF_ValidBunchidPhase.txt 
//   /*
//   int trayvalidshift[2][4]={{4,-5,0,-7},{5,-4,0,-6}};
//   //tray 121 half0: 4 5    half1: 5 6
//   //tray 122 half0: -5 -4  half1: -4 -3
//   int tray121shift[2][2]={{ 4, 5},{ 5, 6}};
//   int tray122shift[2][2]={{-5,-4},{-4,-3}};
//   int tray124shift[2]   = {-5,-6};
//   */

//   int nthub=-1;
//   int ret=-1;
//   for(int itray=0;itray<30;itray++){
//     if(trayid == trayinTHUB1[itray]) nthub=0;
//     if(trayid == trayinTHUB2[itray]) nthub=1;
//     if(trayid == trayinTHUB3[itray]) nthub=2;
//     if(trayid == trayinTHUB4[itray]) nthub=3;
//   }
//   if(trayid == 121) nthub = 4;
//   if(trayid == 122) nthub = 5;
 
//   int diff = bunchid - refbunchid;
//   if(diff>2048)   {diff =diff-4096;} 
//   else if(diff<-2048) {diff =diff+4096;}

//   //  cout<<"tray="<<trayid<<" halftrayid="<<halftrayid<<" bunchid="<<bunchid<<" refbunchid="<<refbunchid<<" diff="<<diff<<" nthub="<<nthub<<endl;

//   if(trayid>1 && trayid<121){
//     if( (diff != mValidShiftTray[0][nthub])  && (diff != mValidShiftTray[1][nthub]) ) ret=nthub;
//   } else if(trayid==121){
//     if(diff !=mValidShift121[halftrayid][0] && diff != mValidShift121[halftrayid][1]) ret=nthub;
//   } else if(trayid==122){
//     if(diff !=mValidShift122[halftrayid][0] && diff != mValidShift122[halftrayid][1]) ret=nthub;
//   }

//   if(ret>=0) {
//     cout<<"ERROR!! tray="<<trayid<<" halftrayid="<<halftrayid<<" bunchid="<<bunchid<<" refbunchid="<<refbunchid<<" diff="<<diff<<" nthub="<<ret<<endl;
//   }

//   return ret;
// }

int tofBuilder::ValidBunchid(int trayid,int halftrayid,int bunchid,int refbunchid)
{
  if(trayid<1 || trayid>122) return -1;

  if(trayid == 123) return -1;

  // THUB NW
  int trayinTHUB1[30]={21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50};
  // THUB NE 
  int trayinTHUB2[30]={66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95};
  // THUB SW
  int trayinTHUB3[30]={1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,51,52,53,54,55,56,57,58,59,60};
  // THUB SE
  int trayinTHUB4[30]={61,62,63,64,65,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};
 
  // the following numbers are moved into TOF_ValidBunchidPhase.txt 
  /*
  int trayvalidshift[2][4]={{4,-5,0,-7},{5,-4,0,-6}};
  //tray 121 half0: 4 5    half1: 5 6
  //tray 122 half0: -5 -4  half1: -4 -3
  int tray121shift[2][2]={{ 4, 5},{ 5, 6}};
  int tray122shift[2][2]={{-5,-4},{-4,-3}};
  int tray124shift[2]   = {-5,-6};
  */

  int nthub=-1;
  int ret=-1;
  for(int itray=0;itray<30;itray++){
    if(trayid == trayinTHUB1[itray]) nthub=0;
    if(trayid == trayinTHUB2[itray]) nthub=1;
    if(trayid == trayinTHUB3[itray]) nthub=2;
    if(trayid == trayinTHUB4[itray]) nthub=3;
  }
  if(trayid == 121) nthub =4;
  if(trayid == 122) nthub =5;
  int diff=bunchid-refbunchid;
  if(diff>2048)   {diff =diff-4096;} 
  else if(diff<-2048) {diff =diff+4096;}

//  cout<<"tray="<<trayid<<" halftrayid="<<halftrayid<<" bunchid="<<bunchid<<" refbunchid="<<refbunchid<<" diff="<<diff<<" nthub="<<nthub<<endl;
  if(trayid>1 && trayid<121){
    if( (diff != mValidShiftTray[0][nthub])  && (diff != mValidShiftTray[1][nthub]) ) ret=nthub;
  } else if(trayid==121){
    if(diff !=mValidShift121[halftrayid][0] && diff != mValidShift121[halftrayid][1]) ret=nthub;
  } else if(trayid==122){
    if(diff !=mValidShift122[halftrayid][0] && diff != mValidShift122[halftrayid][1]) ret=nthub;
  }

  //if(ret>=0)cout<<"ERROR!! tray="<<trayid<<" halftrayid="<<halftrayid<<" bunchid="<<bunchid<<" refbunchid="<<refbunchid<<" diff="<<diff<<" nthub="<<ret<<endl;

  return ret;
}


void tofBuilder::event(daqReader *rdr)
{

  //printf("event\n");

  // Get Trigger Data...
  StTriggerData *trgd = getStTriggerData(rdr);
  if(!trgd) return;

  //printf("trgd\n");

  // trigger only histos...
  int npre1=trgd->numberOfPreXing();
  int npost1=trgd->numberOfPostXing();

  for( int ipost=-npre1; ipost<npost1+1; ipost++) {
    int prepost=ipost;
    if(prepost != 0) continue;    // only look at prepost =0 data.

    for(int tray=1;tray<=120;tray++){

      //cout<<" TOF TRAY :::: "<<atrayid<<endl;
      int trigger_mult=trgd->tofTrayMultiplicity(tray,prepost);
      if(trigger_mult > 31) trigger_mult=31;
      contents.TOF_L0_trg[tray-1]->Fill(trigger_mult);
    }
  }
  //printf("to parsedata\n");

  leadinghits.clear();
  trailinghits.clear();
  parseData(rdr);

  int sum_L0_hit=0;
  for(int itray=0;itray<120;itray++){
    //if(Tray_NotInRun(itray+1)) continue;
    // mult. from hits
    int hit_mult=0;
    for(int i=0;i<24;i++){if(tinohit[itray][i]>0) hit_mult++;}
    sum_L0_hit += hit_mult;
  }

  float TOF_L1mult = (float)trgd->tofMultiplicity(0);

  contents.TOF_L1mult_vs_ZDCadcsum->Fill(TOF_L1mult, trgd->zdcHardwareSum());
  contents.TOF_L1mult_vs_sumL0->Fill(TOF_L1mult, sum_L0_hit);

  // Check errors...
  contents.TOF_EventCount->Fill(1);

  // check bunch id shift
  int bunchidref1 =   allbunchid[0][mReferenceTray-1];   // bunchid from tray 1 as reference.
  int bunchidref2 =   allbunchid[1][mReferenceTray-1];   // bunchid from tray 1 as reference.
  if(bunchidref1 != bunchidref2) {
    printf("ids %d %d\n",bunchidref1,bunchidref2);
    contents.TOF_Error2->Fill(3);
    nerr2++;
  }
  
  for(int itray=0;itray<122;itray++){
    int traynum=itray+1;
    if(NotActiveTray[traynum]) continue;
    for(int ihalf=0;ihalf<2;ihalf++){
      int bunchid=allbunchid[ihalf][itray];
      //if(bunchid == -9999) continue;
      
      int ret=ValidBunchid(traynum,ihalf,bunchid,bunchidref1);

      if(ret>=0) {
	contents.TOF_Error2->Fill(ret+0.5*ihalf);
	nerr2++;

	if(bunchid == -9999) { 
	  contents.TOF_Error3->Fill(ret+0.5*ihalf);
	  nerr3++;
	}
      }
    }
  }

  nevts++;
  char t[256];
  if(nerr1 == 0) {
    sprintf(t, "No electronics errors in %d events",nevts);
  }
  else {
    sprintf(t, "%d electronics errors in %d events",nerr1, nevts);
  }
  TOF_Error1_label->SetText(.2,.8,t);

  if(nerr2 == 0) {
    sprintf(t, "No bunchid errors in %d events",nevts);
  }
  else {
    sprintf(t, "%d bunchid errors in %d events",nerr2, nevts);
  }
  TOF_Error2_labela->SetText(.2,.8,t);

  if((nevts > 9) && (nerr3 > nevts/2)) {
    sprintf(t, "Possible Tray Lost.  CALL EXPERT");
  }
  else {
    t[0] = '\0';
  }
  TOF_Error2_labelb->SetText(.2,.7,t);
 
  std::sort(leadinghits.begin(), leadinghits.end());
  std::sort(trailinghits.begin(), trailinghits.end());

  float leadingtime[54],trailingtime[54];  // will only get one hit of each channel
  for(int i=0;i<54;i++){leadingtime[i]=0.;trailingtime[i]=0;}

  for(int ich=0;ich<54;ich++){
    for(unsigned int ile=0;ile<leadinghits.size();ile++){
      double thisnumber = leadinghits[ile];
      int thistrayid= int(thisnumber/1.e8);
      int  thismodule=int((thisnumber-thistrayid*1.e8)/1.e5);
      float thistime=  thisnumber-thistrayid*1.e8-thismodule*1.e5;
      if(thismodule == ich) {leadingtime[ich]= thistime;break;} 
    }
  }
  for(int ich=0;ich<54;ich++){
    for(unsigned int ite=0;ite<trailinghits.size();ite++){
      double thisnumber = trailinghits[ite];
      int thistrayid= int(thisnumber/1.e8);
      int  thismodule=int((thisnumber-thistrayid*1.e8)/1.e5);
      float thistime=  thisnumber-thistrayid*1.e8-thismodule*1.e5;
      if(thismodule == ich) {trailingtime[ich]= thistime;break;} 
    }
  }
 
  for(int ich=0;ich<54;ich++){
    if(leadingtime[ich]*trailingtime[ich]<1) continue;
    float ToT = trailingtime[ich]-leadingtime[ich];
    if(ToT<0) ToT = ToT + 51200;
    if(ToT>0) contents.upvpd_ToT->Fill(ich,ToT);
  }
  for(int iwest=0;iwest<19;iwest++){
    int ieast=iwest+19;
    if(leadingtime[ieast]*leadingtime[iwest]<1) continue;
    contents.upvpd_eastT_vs_westT->Fill(leadingtime[ieast],leadingtime[iwest]);
  }


  if(trgd) delete trgd;
}

void tofBuilder::stoprun(daqReader *rdr) {
}

void tofBuilder::main(int argc, char *argv[])
{
  tofBuilder me;
  
  me.Main(argc, argv);
}


void tofBuilder::ReadValidBunchidPhase(){
  //cout<<"TOFcheckHistogramGroup::Bunchid phase config file:"<<mBunchShiftList<<endl;
  char mBunchShiftList[256];
  sprintf(mBunchShiftList, "%s/tof/%s",confdatadir,"TOF_ValidBunchidPhase.txt");

  mReferenceTray=1;
  
  TString buffer;
  ifstream filein(mBunchShiftList);
  int count=0;

  if(filein){
    while(!filein.eof()){
      buffer.ReadLine(filein);
      if(buffer.BeginsWith("/")) continue;
      if(buffer.BeginsWith("#")) continue;
      int number=atoi(buffer.Data());
      if(count==0) mReferenceTray=number;
      if(count>=1 && count<=8)mValidShiftTray[(count-1)%2][(count-1)/2]=number;

      if(count== 9 || count==10) mValidShift121[0][(count-1)%2]=number;
      if(count==11 || count==12) mValidShift121[1][(count-1)%2]=number;

      if(count==13 || count==14) mValidShift122[0][(count-1)%2]=number;
      if(count==15 || count==16) mValidShift122[1][(count-1)%2]=number;

      count++;
    }
  } else {cout<<"TOFcheckHistogramGroup::Can not open file:"<<mBunchShiftList<<endl;}

}


void tofBuilder::ReadTraymaskoutList(){
  
  TString buffer;
  char mTraymaskoutList[256];
  sprintf(mTraymaskoutList, "%s/tof/%s",confdatadir,"TOF_TrayMaskout.txt");

  ifstream filein(mTraymaskoutList);
  for(int i=0;i<128;i++){MaskoutTray[i]=false;}
  if(filein){ 
    while(!filein.eof()) {
      buffer.ReadLine(filein);
      if(buffer.BeginsWith("/")) continue;
      if(buffer.BeginsWith("#")) continue;
      int trayid = atoi(buffer.Data());
      if(trayid<1 || trayid>127) continue;
      MaskoutTray[trayid]=true;
    }   
  } else {cout<<"tofBuilder::Can not open file:"<<mTraymaskoutList<<endl;}
  filein.close();

}

void tofBuilder::ReadTrayList(){
  
  //cout<<"tofBuilder::TrayList config file:"<<mTrayList<<endl;
  char mTrayList[256];
  sprintf(mTrayList, "%s/tof/%s",confdatadir,"TOF_TrayNotInRun.txt");
  
  TString buffer;

  ifstream filein(mTrayList);
  for(int i=0;i<128;i++){NotActiveTray[i]=false;}
  if(filein){ 
    while(!filein.eof()) {
      buffer.ReadLine(filein);
      if(buffer.BeginsWith("/")) continue;
      if(buffer.BeginsWith("#")) continue;
      int trayid = atoi(buffer.Data());
      if(trayid<1 || trayid>127) continue;
      NotActiveTray[trayid]=true;
    }   
  } else {cout<<"tofBuilder::Can not open file:"<<mTrayList<<endl;}
  filein.close();

}


int tofBuilder::tdcchan2upvpdPMTchan(int globaltdcchan, int edgeid,int trayid)
{

  if(trayid<121 || trayid >122) return -1;
  if(globaltdcchan<0 || globaltdcchan>191) {cout<<"Wrong global tdc chan: "<<globaltdcchan<<endl; return -1;}

  /* run 9 ---->
//                      1   2   3  4  5  6  7  8  9  10  11 12  13  14  15 16 17 18 19
  int upvpdLEchan[54]={142,122,118,98,46,26,22,2,112,101,24,136,123,120,99,40,27,16,3,  //west
                       142,122,118,98,46,26,22,2,112,101,24,136,123,120,99,40,27,16,3,  //east
		       48,64,50,70,0,29,5,96,   48,64,50,70,0,29,5,96};                 //pp2pp 
  int upvpdTEchan[54]={129,131,105,107,33,35,9,11,109,110,39,133,132,135,108,37,36,13,12,  //west
                       129,131,105,107,33,35,9,11,109,110,39,133,132,135,108,37,36,13,12,  //east
		       63,61,59,57,15,38,14,111,    63,61,59,57,15,38,14,111};             //pp2pp 
  */
  // run10--->
//                      1   2   3  4  5  6  7  8  9  10  11 12  13  14  15 16  17  18 19
  int upvpdLEchan[54]={  5, 22, 12,29,46,36,53,70,60,101, 0,24,118,108,125,142,132,48,96,  //west
			 5, 22, 12,29,46,36,53,70,60,101, 0,24,118,108,125,142,132,48,96,  //east
		        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1};               //pp2pp 
  int upvpdTEchan[54]={  5, 22, 12,29,46,36,53,70,60,101, 0,24,118,108,125,142,132,48,96,  //west
			 5, 22, 12,29,46,36,53,70,60,101, 0,24,118,108,125,142,132,48,96,  //east
		        -1,-1,-1,-1,-1,-1,-1,-1, -1,-1,-1,-1,-1,-1,-1,-1};               //pp2pp 

  int inputglobalchan=globaltdcchan;
  int pmtchan=-1;
  int pmtLEchan =-1;
  int startpoint=-1;
  if(trayid==121) startpoint=0;   // west
  if(trayid==122) startpoint=19;  // east

  for(int i=startpoint;i<startpoint+19;i++){
    if(upvpdLEchan[i]==inputglobalchan) {pmtLEchan=i;break;}
  }
  for(int i=38;i<46;i++){
    if(upvpdLEchan[i]==inputglobalchan) {pmtLEchan=i;if(trayid==122)pmtLEchan=pmtLEchan+8;break;}
  }
  //
  int pmtTEchan=-1;
  for(int i=startpoint;i<startpoint+19;i++){
    if(upvpdTEchan[i]==inputglobalchan) {pmtTEchan=i;break;}
  }
  for(int i=38;i<46;i++){
    if(upvpdTEchan[i]==inputglobalchan) {pmtTEchan=i;if(trayid==122)pmtTEchan=pmtTEchan+8;break;}
  }

  if(edgeid==4) pmtchan = pmtLEchan;
  if(edgeid==5) pmtchan = pmtTEchan;

  //cout<<" inside map:: trayid="<<trayid<<" globaltdcchan="<<globaltdcchan<<" edgeid="<<edgeid<<" return="<<pmtchan<<endl;

  return pmtchan;
}
