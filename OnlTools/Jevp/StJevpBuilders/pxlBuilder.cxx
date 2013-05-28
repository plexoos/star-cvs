#include <stdio.h>
#include <stdlib.h>

#include "Jevp/StJevpPlot/JevpPlotSet.h"
#include "DAQ_READER/daqReader.h"
#include "DAQ_READER/daq_dta.h"
#include "DAQ_READER/daq_det.h"

#include "Jevp/StJevpPlot/RunStatus.h"
#include "StEvent/StTriggerData.h"
#include <TH1I.h>
#include <TH2F.h>

#include <math.h>
#include "pxlBuilder.h"
#include <RTS/include/rtsLog.h>


const int pxlBuilder::PXLERR_HEADER = 0x1;
const int pxlBuilder::PXLERR_HITLEN = 0x2;
const int pxlBuilder::PXLERR_SENSORNUM = 0x4;
const int pxlBuilder::PXLERR_DUPLICATE = 0x8;
const int pxlBuilder::PXLERR_ENDER = 0x10;
const int pxlBuilder::PXLERR_RANGE = 0x20;
const int pxlBuilder::PXLERR_OUTOFORDER = 0x40;
const int pxlBuilder::PXLERR_UNKNOWNROW = 0x80;


//+++++++++++++++++++++++BEGIN Helper Functions+++++++++++++++++++++++
//+++++++++++++++++++++++BEGIN Helper Functions+++++++++++++++++++++++
//+++++++++++++++++++++++BEGIN Helper Functions+++++++++++++++++++++++

int pxlBuilder::decode16bit(unsigned short val, bool MS16,int sensor,int *row,int *prev_row,int *prev_col,int *error_cnt,int *OVF,bitset<NCOL> bs[][NROW],int runlength[][4]){
  int ret;
  bool duplicate;
  int column, coding;

  ret = 0;
  if ( (val & 0x1000) == 0x1000 ) { // State0
    int tmpRow = (val>>2) & 0x3ff;
    if (tmpRow >= NROW) {
      //DEBUGP("**row %4d", tmpRow);
      ret |= PXLERR_RANGE; (*error_cnt)++;
    }
    else {
      //DEBUGP("row %4d", tmpRow);
    }
    if(sensor < NSENSOR) {
      row[sensor] = tmpRow;
      if(tmpRow < (prev_row[sensor]&0xFFF)) {
	if((prev_row[sensor]&0x1000) == 0x1000) {
	  prev_row[sensor] = tmpRow | 0x1000;
	  ret |=  PXLERR_OUTOFORDER; (*error_cnt)++;
	  //DEBUGP(" **out-of-order");
	}
	else {
	  prev_row[sensor] = tmpRow | 0x1000;
	}
      }

      else {
	prev_row[sensor] = tmpRow;
      }
    }
    if( (val & 0x2) == 0x2) {
      // overflow:
      //DEBUGP(" OVF");
      if(sensor < NSENSOR) OVF[sensor]++;
    }
  }
  else { // StateN
    column = (val>>2) & 0x3ff;
    coding = val & 0x3;
    if (column+coding >= NCOL) {
      //DEBUGP("**col %4d ", column);
      ret |= PXLERR_RANGE; (*error_cnt)++;
    }
    else {
      //DEBUGP("col %4d ", column);
    }
    if(sensor < NSENSOR) {
      if (MS16) {
	if (row[sensor] == -1) {
	  // error: row should already be defined here
	  //DEBUGP("(**row %4d) ", row[sensor]);
	  ret |= PXLERR_UNKNOWNROW; (*error_cnt)++;
	}
	else{
	  //DEBUGP("(row %4d) ", row[sensor]);
	}
      }
      else
	//DEBUGP("(row %4d) ", row[sensor]);
      // valid hits, fill bitset if valid row
      if((row[sensor]>=0) && (row[sensor]<NROW)) {
	duplicate = false;
	runlength[sensor][coding]++;
	for(int c=0; c<coding+1; c++) {
	  if((column+c)<NCOL)  {
	    if (bs[sensor][row[sensor]].test(column+c))
	      duplicate = true;
	    bs[sensor][row[sensor]].set(column+c);
	  }
	}
	if(duplicate) {
	  //DEBUGP("**(duplicate) ");
	  ret |= PXLERR_DUPLICATE; (*error_cnt)++;
	}
      }
    }
    //DEBUGP("coding %d", coding);
  }
  return ret;
}


//**************************************************
int pxlBuilder::pxl_decoder(const u_int *d, const int wordLength,bitset<NCOL> bs[][NROW],int *OVF, struct _pxlHeaderInfo *pxlHeaderInfo,int *error_cnt,float *ave_runlength){
  int i,j;
  int ret; 

  // Sensor variables:
  int sensor;
  int row[NSENSOR]; // 40 sensors per RDO
  unsigned short valLS, valMS;
  int prev_row[NSENSOR], prev_col[NSENSOR];
  int runlength[NSENSOR][4];
  ret = 0; // no error

  // clear the row array
  for (i=0; i<NSENSOR; i++) {
    row[i] = -1;
    prev_row[i] = 0;
    prev_col[i] = 0;
  }

  // dump header
  //DEBUGP("\t%4d: 0x%08X\n",0,d[0]);
  if (d[0] != 0xAAAAAAAA) {
    //DEBUGP(" *** wrong header token!\n");
    ret |= PXLERR_HEADER; error_cnt++;
  }

  pxlHeaderInfo->tcdWord = (unsigned short)(d[1]&0xFFFFF);
  pxlHeaderInfo->rhicStrobeCtr = d[7];
  pxlHeaderInfo->temperature[0][0] = (unsigned short)(d[2]&0x3FF);
  pxlHeaderInfo->temperature[0][1] = (unsigned short)((d[2]&0xFFC00)>>10);
  pxlHeaderInfo->temperature[1][0] = (unsigned short)((d[2]&0x3FF00000)>>20);
  pxlHeaderInfo->temperature[1][1] = (unsigned short)(d[3]&0x3FF);
  pxlHeaderInfo->temperature[2][0] = (unsigned short)((d[3]&0xFFC00)>>10);
  pxlHeaderInfo->temperature[2][1] = (unsigned short)((d[3]&0x3FF00000)>>20);
  pxlHeaderInfo->temperature[3][0] = (unsigned short)(d[4]&0x3FF);
  pxlHeaderInfo->temperature[3][1] = (unsigned short)((d[4]&0xFFC00)>>10);

  // hit block length
  int hitLength = d[16];
  //DEBUGP("\t%4d: 0x%08X - hit block length: %d",16,d[16],d[16]);

  // Now dump hits:
  int endOfHits;
  if ( (hitLength+16) < (wordLength-2) ) {
    endOfHits = hitLength+17;
  }
  else {
    ret |= PXLERR_HITLEN; error_cnt++;
    //DEBUGP(" *** too big!!!");
    endOfHits = wordLength - 2;
  }
  //DEBUGP("\n");

  for(i=17; i<endOfHits; i++) {
    //DEBUGP("\t%4d: 0x%08X",i,d[i]);
    // decode hit data:
    sensor = ((d[i]>>26) & 0x38) | ((d[i]>>13) & 0x7);
    //DEBUGP(" - Sensor %2d", sensor);
    // sensor goes  from 1-40, internally use 0-39:
    sensor -= 1;
    if(sensor > (NSENSOR-1)) {
      ret |= PXLERR_SENSORNUM; error_cnt++;
      //DEBUGP("**invalid");
    }

    valLS = d[i] & 0xffff;
    valMS = (d[i]>>16) & 0xffff;

    // first decode least significant 16 bits

    ret |= decode16bit(valLS, false, sensor, row, prev_row, prev_col, 
		       error_cnt, OVF, bs, runlength);

    ret |= decode16bit(valMS, true, sensor, row, prev_row, prev_col,
		       error_cnt, OVF, bs, runlength);
    //DEBUGP("\n");
  }
	
  if (d[wordLength-1] != 0xBBBBBBBB) {
    ret |= PXLERR_ENDER; (*error_cnt)++;
    //DEBUGP(" *** wrong ender token!");
  }
  //DEBUGP(" - Ender\n");
	


  // now calculate the average runlength (coding+1):
  for(i=0; i<NSENSOR; i++) {
    int total=0, N=0;
    for(j=0; j<4; j++) {
      total += (j+1)*runlength[i][j];
      N += runlength[i][j];
    }
    if (N>0)
      ave_runlength[i] = (float)total/((float)N);
    else
      ave_runlength[i] = 0.0;
  }
  return ret;
}

void pxlBuilder::IncrementMultiplicity(int sensor_number,int row_count){
  (sensor_number < 11) ? multiplicity_inner += row_count : multiplicity_outer += row_count;
}

int pxlBuilder::WhichLadder(int sector_number,int sensor_number){
  //sector_number 1 - 10, sensor 1 - 40, returns ladder number 1 - 40
  int ladder_number = 1+(sensor_number/11)+(4*(sector_number-1));
  return ladder_number;
}

void pxlBuilder::UpdateLadderCount(int sector_number,int sensor_number,int sensor_count){
  int ladder_number = WhichLadder(sector_number,sensor_number);

  if(LadderCount->count(ladder_number) == 0) LadderCount->insert(make_pair(ladder_number,sensor_count));
  else{
    LadderCount->find(ladder_number)->second += sensor_count;
  } 
}

void pxlBuilder::SetRunLength(int sensor_number,double average_run_length){
  if(AverageRunLength->count(sensor_number) == 0) AverageRunLength->insert(make_pair(sensor_number,average_run_length));
  else{
    //This shouldn't happen
    //AverageRunLength->find(sensor_number)->second += average_run_length;
  }
}
bool pxlBuilder::ScaleTH1Bin(TH1 *hist,int bin,int scale_factor){
  int bin_content = 0;
  bin_content += hist->GetBinContent(bin);
  
  int new_content = bin_content/scale_factor;
  hist->SetBinContent(bin,new_content);

 return (hist->GetBinContent(bin) == new_content) ? true : false;
}

bool pxlBuilder::UpdateTH1(TH1 *hist,int bin,double value,bool scale,int mod_val){
  int bin_content = 0;
  bin_content += hist->GetBinContent(bin);
  
  int new_content = bin_content+value;
  hist->SetBinContent(bin,new_content);

  bool did_it_work = (hist->GetBinContent(bin) == new_content) ? true : false;
  if(!scale) return did_it_work;
  else{
    if(!did_it_work) return false;
    else{
      if(mod_val<2) return false;
      else{
	if((number_of_events+1 % mod_val) == 0){
	  return ScaleTH1Bin(hist,bin,mod_val);
	}
	else return true;
      }
    }
  }
}

bool pxlBuilder::UpdateTH1(TH1 *hist,int bin,double value){
  double bin_content = 0;
  bin_content += hist->GetBinContent(bin);
  double new_content = bin_content+value;

  if(new_content == 0.0) return true;
  else{
    hist->SetBinContent(bin,new_content);
    return (hist->GetBinContent(bin) == new_content) ? true : false;
  }
}

bool pxlBuilder::UpdateTH2(TH1 *hist,int x_bin,int y_bin,double value){
  double bin_content = 0;
  bin_content += hist->GetBinContent(x_bin,y_bin);
  double new_content = bin_content+value;

  if(new_content == 0.0) return true;
  else{
    hist->SetBinContent(x_bin,y_bin,new_content);
    return (hist->GetBinContent(x_bin,y_bin) == new_content) ? true : false;
  }
}

void pxlBuilder::SetLadderMap(){
  for(int i=1; i<41; i++){
    int value = ((i-1)/4)+1;
    LadderMap->insert(make_pair(i,value));
  }
}
//+++++++++++++++++++++++END Helper Functions+++++++++++++++++++++++
//+++++++++++++++++++++++END Helper Functions+++++++++++++++++++++++
//+++++++++++++++++++++++END Helper Functions+++++++++++++++++++++++


ClassImp(pxlBuilder);
  

pxlBuilder::pxlBuilder(JevpServer *parent) : JevpPlotSet(parent) {
  plotsetname = (char *)"pxl";

  // start with histograms undefined...
  memset(&contents, 0, sizeof(contents));

  event_multiplicity = 0;
  multiplicity_inner = 0;
  multiplicity_outer = 0;
  sensor_count = 0;
  number_of_events = 0;

  max_count = 0;
  max_count_sector2 = 0;
  max_count_sector4 = 0;
  max_count_sector7 = 0;
  max_count_inner = 0;
  max_count_outer = 0;

  min_count = 1000000;
  min_count_sector2 = 1000000;
  min_count_sector4 = 1000000;
  min_count_sector7 = 1000000;
  min_count_inner = 1000000;
  min_count_outer = 1000000;

  LadderMap = new map<int,int>();
  SetLadderMap();
}

pxlBuilder::~pxlBuilder() {

  // Delete any existing histograms...
  int n = sizeof(contents) / sizeof(TH1 *);
  for(int i=0;i<n;i++) {
    if(contents.array[i]) delete contents.array[i];
  }
}

void pxlBuilder::initialize(int argc, char *argv[]) {
  // Initialization of histograms.
  //contents.myhisto = new TH1F("myhisto","My Histogram",50,0,10);
  //contents.GlobalHitMultiplicity =  new TH1F("GlobalHitMultiplicity","Distribution of the Total Number of Hits per Event",400,1,8001);

  //%%%%%%%%%%%%%%Creating Histograms%%%%%%%%%%%%%%
  //%%%%%%%%%%%%%%Creating Histograms%%%%%%%%%%%%%%
  //Tab 1: PXL Multiplicity Plots
  contents.GlobalHitMultiplicity = new TH1I("GlobalHitMultiplicity","",2500,1,75001);
  

  //contents.GlobalHitMultiplicitySector1 = new TH1I("GlobalHitMultiplicitySector1","",500,1,15001);
  contents.GlobalHitMultiplicitySector2 = new TH1I("GlobalHitMultiplicitySector2","",2500,1,50001);
  //contents.GlobalHitMultiplicitySector3 = new TH1I("GlobalHitMultiplicitySector3","",500,1,15001);
  contents.GlobalHitMultiplicitySector4 = new TH1I("GlobalHitMultiplicitySector4","",500,1,10001);
  //contents.GlobalHitMultiplicitySector5 = new TH1I("GlobalHitMultiplicitySector5","",500,1,15001);
  //contents.GlobalHitMultiplicitySector6 = new TH1I("GlobalHitMultiplicitySector6","",500,1,15001);
  contents.GlobalHitMultiplicitySector7 = new TH1I("GlobalHitMultiplicitySector7","",750,1,20001);
  //contents.GlobalHitMultiplicitySector8 = new TH1I("GlobalHitMultiplicitySector8","",500,1,15001);
  //contents.GlobalHitMultiplicitySector9 = new TH1I("GlobalHitMultiplicitySector9","",500,1,15001);
  //contents.GlobalHitMultiplicitySector10 = new TH1I("GlobalHitMultiplicitySector10","",500,1,15001);

  contents.HitMultiplicityPerEvent = new TH1I("HitMultiplicityPerEvent","",500,1,5001);
  //contents.HitMultiplicityPerEvent = new TH1I("HitMultiplicityPerEvent","",5000,1,5001);
  
  contents.HitsPerLadder = new TH1I("HitsPerLadder","",40,1,41);
  
  contents.HitCorrelation = new TH2I("HitCorrelation","",750,1,75001,75,1,7501);
  
  
  //Tab 2: PXL Hit Maps
  contents.SensorHitsInnerLayer = new TH2I("SensorHitsInnerLayer","",10,1,11,10,1,11);
  contents.SensorHitsInnerLayerPerEvent = new TH2I("SensorHitsInnerLayerPerEvent","",10,1,11,10,1,11);
  
  contents.SensorHitsOuterLayer = new TH2I("SensorHitsOuterLayer","",30,11,41,10,1,11);
  contents.SensorHitsOuterLayerPerEvent = new TH2I("SensorHitsOuterLayerPerEvent","",30,11,41,10,1,11);
  
  contents.AverageRunLengthInnerLayer = new TH2D("AverageRunLengthInnerLayer","",10,1,11,10,1,11);
  contents.AverageRunLengthOuterLayer = new TH2D("AverageRunLengthOuterLayer","",30,11,41,10,1,11);
  

  //%%%%%%%%%%%%%%Histogram Attributes%%%%%%%%%%%%%%
  //%%%%%%%%%%%%%%Histogram Attributes%%%%%%%%%%%%%%
  contents.GlobalHitMultiplicity->SetTitle("Distribution of the Total Number of Hits per Event;Global Hit Multiplicity per Event;Counts");
  contents.GlobalHitMultiplicity->SetFillColor(kMagenta+3);

  contents.GlobalHitMultiplicitySector2->SetTitle("Distribution of the Total Number of Hits per Event (Sector 2);Global Hit Multiplicity per Event;Counts");
  contents.GlobalHitMultiplicitySector2->SetFillColor(kMagenta+3);
  contents.GlobalHitMultiplicitySector2->SetFillStyle(3001);

  contents.GlobalHitMultiplicitySector4->SetTitle("Distribution of the Total Number of Hits per Event (Sector 4);Global Hit Multiplicity per Event;Counts");
  contents.GlobalHitMultiplicitySector4->SetFillColor(kMagenta+3);
  contents.GlobalHitMultiplicitySector4->SetFillStyle(3001);

  contents.GlobalHitMultiplicitySector7->SetTitle("Distribution of the Total Number of Hits per Event (Sector 7);Global Hit Multiplicity per Event;Counts");
  contents.GlobalHitMultiplicitySector7->SetFillColor(kMagenta+3);
  contents.GlobalHitMultiplicitySector7->SetFillStyle(3001);


  contents.HitMultiplicityPerEvent->SetTitle("Global Hit Multiplicity per Event;Event Counter;Global Hit Multiplicity");
  contents.HitMultiplicityPerEvent->SetFillColor(kRed+2);
  
  contents.HitsPerLadder->SetTitle("Number of Hits per Ladder;Ladder Number;Hit Multiplicity");
  contents.HitsPerLadder->SetFillColor(kGreen+2);
  
  contents.HitCorrelation->SetTitle("Distribution Hit Correlation Inner-Outer Layer;Outer Layer Hit Multiplicity per Event;Inner Layer Hit Multiplicity per Event");
  
  contents.SensorHitsInnerLayer->SetTitle("Number of Hits per Sensor (Inner Layer);Ladder Number;Sensor Number");

  contents.SensorHitsInnerLayerPerEvent->SetTitle("Number of Hits per Sensor (Inner Layer) per Event;Ladder Number;Sensor Number");
  
  contents.SensorHitsOuterLayer->SetTitle("Number of Hits per Sensor (Outer Layer);Ladder Number;Sensor Number");

  contents.SensorHitsOuterLayerPerEvent->SetTitle("Number of Hits per Sensor (Outer Layer) per Event;Ladder Number;Sensor Number");
  
  contents.AverageRunLengthInnerLayer->SetTitle("Intensity Plot of Average Run Length (Inner Layer);Ladder Number;Sensor Number");

  contents.AverageRunLengthOuterLayer->SetTitle("Intensity Plot of Average Run Length (Outer Layer);Ladder Number;Sensor Number");



  //%%%%%%%%%%%%%%Add root histograms to Plots%%%%%%%%%%%%%%
  //%%%%%%%%%%%%%%Add root histograms to Plots%%%%%%%%%%%%%%
  int np = sizeof(contents) / sizeof(TH1 *);
  JevpPlot *plots[np];

  int n=0;
  //plots[n] = new JevpPlot(contents.myhisto);
  //plots[n]->setDrawOpts("col");
  //plots[n]->optstat = 0;

  plots[n] = new JevpPlot(contents.GlobalHitMultiplicity);
  plots[n]->optstat = 0;

  //plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector1);
  //plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector2);
  plots[n]->optstat = 0;

  //plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector3);
  //plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector4);
  plots[n]->optstat = 0;

  //plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector5);
  //plots[n]->optstat = 0;

  //plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector6);
  //plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector7);
  plots[n]->optstat = 0;

  //plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector8);
  //plots[n]->optstat = 0;

  //plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector9);
  //plots[n]->optstat = 0;

  //plots[++n] = new JevpPlot(contents.GlobalHitMultiplicitySector10);
  //plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.HitMultiplicityPerEvent);
  plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.HitsPerLadder);
  plots[n]->optstat = 0;

  plots[++n] = new JevpPlot(contents.HitCorrelation);
  plots[n]->optstat = 0;
  plots[n]->setDrawOpts("colz");

  plots[++n] = new JevpPlot(contents.SensorHitsInnerLayer);
  plots[n]->optstat = 0;
  plots[n]->setDrawOpts("colz");

  plots[++n] = new JevpPlot(contents.SensorHitsInnerLayerPerEvent);
  plots[n]->optstat = 0;
  plots[n]->setDrawOpts("colz");

  plots[++n] = new JevpPlot(contents.SensorHitsOuterLayer);
  plots[n]->optstat = 0;
  plots[n]->setDrawOpts("colz");

  plots[++n] = new JevpPlot(contents.SensorHitsOuterLayerPerEvent);
  plots[n]->optstat = 0;
  plots[n]->setDrawOpts("colz");

  plots[++n] = new JevpPlot(contents.AverageRunLengthInnerLayer);
  plots[n]->optstat = 0;
  plots[n]->setDrawOpts("colz");

  plots[++n] = new JevpPlot(contents.AverageRunLengthOuterLayer);
  plots[n]->optstat = 0;
  plots[n]->setDrawOpts("colz");

  //plots[++n] = new JevpPlot(contents.h0_evt_size);
  //plots[n]->optstat = 0;

  // Add Plots to plot set...
  for(int i=0;i<=n;i++) {
    LOG(DBG, "Adding plot %d",i);
    addPlot(plots[i]);
  }


}
  

void pxlBuilder::startrun(daqReader *rdr) {
  LOG(NOTE, "pxlBuilder starting run #%d",rdr->run);
  resetAllPlots();
}

#define safelog(x) ((x > 0) ? log10(x) : 0)

void pxlBuilder::event(daqReader *rdr){
  //int pxl_size = rdr->getDetectorSize("pxl");
  //contents.myhisto->Fill(safelog(pxl_size));

  //Reset Variables
  event_multiplicity = 0;
  multiplicity_inner = 0;
  multiplicity_outer = 0;
  sensor_count = 0;
  
  
  AverageRunLength = new map<int,double>();
  LadderCount = new map<int,int>();
  
  //Reset Per-Event Histograms
  //contents.HitMultiplicityPerEvent->Reset();
  contents.SensorHitsInnerLayerPerEvent->Reset();
  contents.SensorHitsOuterLayerPerEvent->Reset();

  //+++++++++++++++++++++++Per Event Decoding+++++++++++++++++++++++
  //+++++++++++++++++++++++Per Event Decoding+++++++++++++++++++++++
  //+++++++++++++++++++++++Per Event Decoding+++++++++++++++++++++++
  
  /*************************** PXL **************************/
  daq_dta *dd ;
  dd = rdr->det("pxl")->get("raw") ;
  if(dd){ // "pxl" was found
    // iterate over RDO's (each RDO has 4 ladders)
    int event_count = 0;

    while(dd->iterate()){
      int OVF[NSENSOR];
      int error_cnt;
      error_cnt = 0;
      
      
      int pxl_sector = (dd->sec-1)*5 + dd->rdo;
      //printf("DAQ Sector %d, RDO %d (PXL Sector %d): %d words\n", dd->sec, dd->rdo,pxl_sector, dd->ncontent/4) ;
      
      for(int i=0; i<NSENSOR; i++){
	OVF[i] = 0;
	for(int j=0; j<NROW; j++){
	  bs[i][j].reset();
	}
      }
      
      u_int *d = (u_int *)dd->Void ; // point to the start of the DDL raw data	
      int wordLength = dd->ncontent/4; // number of 32bit words

      int ret = pxl_decoder(d, wordLength, bs, OVF, &pxlHeaderInfo, &error_cnt, ave_runlength);
      if (ret != 0){
	//printf("pxl_decoder returned 0x%x, error_cnt = %d\n", ret, error_cnt);
      }
      else{
	//printf("pxl_decoder returned no errors\n");
      }
      
      
      int sensor_count = 0;
      int row_count = 0;
      int sector_count = 0;
      int sensor_id = 0;

      for(int i=0; i<NSENSOR; i++){
	sensor_count = 0;
	sensor_id = i+1;

	for(int j=0; j<NROW; j++) {
	  row_count = bs[i][j].count();

	  IncrementMultiplicity(i+1, row_count);
	  sensor_count += row_count;
	  event_count +=  row_count;
	  sector_count +=  row_count;
	  
	  //if (bs[i][j].any()) printf("found %4d hits in sensor %2d row %3d\n", bs[i][j].count(), i+1, j);
	}

	//Filling Sensor histograms
	int ladder_number = WhichLadder(pxl_sector,sensor_id);
	int ladder_bin = LadderMap->find(ladder_number)->second;

	if(!UpdateTH1(contents.HitsPerLadder,ladder_number,(double)sector_count)) cout<<"Something is very wrong with HitsPerLadder!"<<endl;

	if(sensor_id < 11){
	  if(!UpdateTH2(contents.SensorHitsInnerLayer,ladder_bin,sensor_id,(double)sector_count)) cout<<"Something is very wrong with SensorHitsInnerLayer!"<<endl;
	  if(!UpdateTH2(contents.SensorHitsInnerLayerPerEvent,ladder_bin,sensor_id,(double)sector_count)) cout<<"Something is very wrong with SensorHitsInnerLayer!"<<endl;

	  if(!UpdateTH2(contents.AverageRunLengthInnerLayer,ladder_bin,sensor_id,(double)ave_runlength[i])) cout<<"Something is very wrong with AverageRunLengthInnerLayer!"<<endl;
	}
	else{
	  if(sensor_id < 21){
	    if(!UpdateTH2(contents.SensorHitsOuterLayer,ladder_bin,sensor_id-10,(double)sector_count)) cout<<"Something is very wrong with SensorHitsOuterLayer!"<<endl;
	    if(!UpdateTH2(contents.SensorHitsOuterLayerPerEvent,ladder_bin,sensor_id-10,(double)sector_count)) cout<<"Something is very wrong with SensorHitsOuterLayer!"<<endl;

	    if(!UpdateTH2(contents.AverageRunLengthOuterLayer,ladder_bin,sensor_id-10,(double)ave_runlength[i])) cout<<"Something is very wrong with AverageRunLengthOuterLayer!"<<endl;
	  }
	  else if(sensor_id < 31){
	    if(!UpdateTH2(contents.SensorHitsOuterLayer,ladder_bin+10,sensor_id-20,(double)sector_count)) cout<<"Something is very wrong with SensorHitsOuterLayer!"<<endl;
	    if(!UpdateTH2(contents.SensorHitsOuterLayerPerEvent,ladder_bin+10,sensor_id-20,(double)sector_count)) cout<<"Something is very wrong with SensorHitsOuterLayer!"<<endl;

	    if(!UpdateTH2(contents.AverageRunLengthOuterLayer,ladder_bin+10,sensor_id-20,(double)ave_runlength[i])) cout<<"Something is very wrong with AverageRunLengthOuterLayer!"<<endl;
	  }
	  else if(sensor_id < 41){
	    if(!UpdateTH2(contents.SensorHitsOuterLayer,ladder_bin+20,sensor_id-30,(double)sector_count)) cout<<"Something is very wrong with SensorHitsOuterLayer!"<<endl;
	    if(!UpdateTH2(contents.SensorHitsOuterLayerPerEvent,ladder_bin+20,sensor_id-30,(double)sector_count)) cout<<"Something is very wrong with SensorHitsOuterLayer!"<<endl;

	    if(!UpdateTH2(contents.AverageRunLengthOuterLayer,ladder_bin+20,sensor_id-30,(double)ave_runlength[i])) cout<<"Something is very wrong with AverageRunLengthOuterLayer!"<<endl;
	  }
	  else cout<<"Something is extremely wrong!"<<endl;
	}

      }
      
      //Finding the max_count/min_count for the different histograms

      if(pxl_sector == 2) {
	((sector_count > max_count_sector2) && (max_count_sector2 = sector_count));
	((sector_count < min_count_sector2) && (min_count_sector2 = sector_count));
      }
      if(pxl_sector == 4){
	((sector_count > max_count_sector4) && (max_count_sector4 = sector_count));
	((sector_count < min_count_sector4) && (min_count_sector4 = sector_count));
      }
      if(pxl_sector == 7){
	((sector_count > max_count_sector7) && (max_count_sector7 = sector_count));
	((sector_count < min_count_sector7) && (min_count_sector7 = sector_count));
      }
      
      //Finding the min_count for the different histograms
     

      //cout<<"number_of_events = "<<number_of_events;

      if(pxl_sector == 2){
	contents.GlobalHitMultiplicitySector2->Fill(sector_count);
	contents.GlobalHitMultiplicitySector2->SetAxisRange(min_count_sector2-10,max_count_sector2+10);
      }
      if(pxl_sector == 4){
	contents.GlobalHitMultiplicitySector4->Fill(sector_count);
	contents.GlobalHitMultiplicitySector4->SetAxisRange(min_count_sector4-10,max_count_sector4+10);
      }
      if(pxl_sector == 7){
	contents.GlobalHitMultiplicitySector7->Fill(sector_count);
	contents.GlobalHitMultiplicitySector7->SetAxisRange(min_count_sector7-10,max_count_sector7+10);
      }

     
      
    }
    //After dd loop
    //^^^HIST^^^::GlobalHitMultiplicity
    (event_count > max_count) && (max_count = event_count);
    (event_count < min_count) && (min_count = event_count);
    contents.GlobalHitMultiplicity->Fill(event_count);
    contents.GlobalHitMultiplicity->SetAxisRange(min_count+10,max_count+10);

    //^^^HIST^^^::HitCorrelation
    (multiplicity_inner > max_count_inner) && (max_count_inner = multiplicity_inner);
    (multiplicity_inner < min_count_inner) && (min_count_inner = multiplicity_inner);

    (multiplicity_outer > max_count_outer) && (max_count_outer = multiplicity_outer);
    (multiplicity_outer < min_count_outer) && (min_count_outer = multiplicity_outer);

    contents.HitCorrelation->Fill(multiplicity_outer,multiplicity_inner);
    contents.HitCorrelation->SetAxisRange(min_count_outer+10,max_count_outer+10);
    contents.HitCorrelation->SetAxisRange(min_count_inner+10,max_count_inner+10,"Y");

    //^^^HIST^^^::HitMultiplicityPerEvent
    if(number_of_events < 5000){
      int event_bin = (number_of_events/10)+1;
      contents.HitMultiplicityPerEvent->SetAxisRange(0,number_of_events+10);
      if(!UpdateTH1(contents.HitMultiplicityPerEvent,event_bin,(double)event_count)) cout<<"Something is very wrong with HitMultiplicityPerEvent!"<<endl;
    }
    else{
      cout<<"FYI: Number of events in evp > 5000"<<endl;
    }

    double scale_factor = (double)((double)number_of_events/(double)(number_of_events+1));
    (scale_factor == 0.0) && (scale_factor = 1.0);

    contents.SensorHitsInnerLayer->Scale(scale_factor);
    contents.SensorHitsOuterLayer->Scale(scale_factor);
    contents.AverageRunLengthInnerLayer->Scale(scale_factor);
    contents.AverageRunLengthOuterLayer->Scale(scale_factor);
    
  }
  
  else {
    //printf("no PXL found\n");
  }
  
  //$$$$$$$$$$$$$$$$$$$ Do I need to delete this?
  //delete rdr ;	// cleanup i.e. if running through a set of files.
  
  
  // End Fill Histograms...
  number_of_events++;
  delete AverageRunLength;
  delete LadderCount;
}

void pxlBuilder::stoprun(daqReader *rdr) {
}

void pxlBuilder::main(int argc, char *argv[]){
  pxlBuilder me;
  
  me.Main(argc, argv);
}
