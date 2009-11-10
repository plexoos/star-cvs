//////////////////////////////////////////////////////////////////////////
//                                                                      //
// StVertexSeedMaker class                                              //
// Author: G. Van Buren, BNL                                            //
// Description: calculates mean primary vertex positions from           //
//              suitable events to use as seeds in finding better       //
//              primary vertex positions (helpful for low               //
//              multiplicity events like pp collisions)                 //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "Stiostream.h"
#include "StVertexSeedMaker.h"
#include "St_DataSetIter.h"
#include "StDAQMaker/StDAQReader.h"
#include "St_db_Maker/St_db_Maker.h"
#include "StDbLib/StDbManager.hh"
#include "StDbLib/StDbConfigNode.hh"
#include "StDbLib/StDataBaseI.hh"
#include "StDetectorDbMaker/StDetectorDbTriggerID.h"
#include "tables/St_dst_vertex_Table.h"
#include "tables/St_dst_L0_Trigger_Table.h"
#include "StMessMgr.h"
#include "StVertexId.h"
#include "tables/St_vertexSeed_Table.h"
#include "tables/St_vertexSeedTriggers_Table.h"
#include "tables/St_beamInfo_Table.h"
#include "TSystem.h"
#include "TFile.h"
#include "TVirtualFitter.h"
#include "TNtuple.h"
#include "TEventList.h"
#include "TArrayF.h"
#include "StTree.h"
#include "TMath.h"
//_____________________________________________________________________________
// C variables and functions for fit/minimization
//_____________________________________________________________________________
static TArrayF xVert, yVert, zVert, multA, exVert, eyVert;
int nverts,nsize;
double beamWidth;
Double_t funcX(float z,Double_t *par) {
  Double_t x = par[0] + par[1]*z;
  return x;
}
Double_t funcY(float z,Double_t *par) {
  Double_t y = par[2] + par[3]*z;
  return y;
}
void fnch(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag) {
  //calculate chisquare
  double chisq = 0;
  for (int i=0;i<nverts; i++) {
    if (exVert[i]==0) {

      double delta_sq, error_sq;
      // Error1 set such that 5 tracks => ~7mm, proportional to 1/::sqrt(mult)
      // This was determined by trying to get the chisq/dof distribution
      // to peak near 1.0
      //                              =>  Error1   = (0.7cm) / ::sqrt(mult/5)
      //                              =>  Error1^2 = (2.45) cm^2 / mult
      // Beam spot size (sigma) Error2 ~= 0.04 cm (400 microns)
      //                              =>  Error2^2 = 0.0016 cm^2
      // The total error should be ::sqrt(Error1^2 + Error2^2)
      error_sq = 0.0016 + (2.45 / multA[i]); 
      delta_sq = TMath::Power(xVert[i]-funcX(zVert[i],par),2) +
                 TMath::Power(yVert[i]-funcY(zVert[i],par),2);
      chisq += (delta_sq/error_sq);

    } else {

      //chisq += TMath::Power((xVert[i]-funcX(zVert[i],par))/exVert[i],2) +
      //         TMath::Power((yVert[i]-funcY(zVert[i],par))/eyVert[i],2);

      double errx2 = exVert[i]*exVert[i] + beamWidth*beamWidth;
      double erry2 = eyVert[i]*eyVert[i] + beamWidth*beamWidth;
      chisq += TMath::Power(xVert[i]-funcX(zVert[i],par),2)/errx2 +
               TMath::Power(yVert[i]-funcY(zVert[i],par),2)/erry2;

    }
  }
  f = chisq;
}
void setArraySize(int n) {
  xVert.Set(n);
  yVert.Set(n);
  zVert.Set(n);
  multA.Set(n);
  exVert.Set(n);
  eyVert.Set(n);
  nsize = n;
}
void addVert(float x, float y, float z, float m, float ex, float ey) {
  if (nverts >= nsize) setArraySize(nsize*2);
  xVert[nverts] = x;
  yVert[nverts] = y;
  zVert[nverts] = z;
  multA[nverts] = m;
  exVert[nverts] = ex;
  eyVert[nverts] = ey;
  nverts++;
}
//_____________________________________________________________________________


ClassImp(StVertexSeedMaker)
//_____________________________________________________________________________
StVertexSeedMaker::StVertexSeedMaker(const char *name,
    const char* defaultDir):StMaker(name){
  xdist = new TH1F("xdist","xdist",1000,HIST_MIN,HIST_MAX);
  ydist = new TH1F("ydist","ydist",1000,HIST_MIN,HIST_MAX);
  xerr  = new TH1F("xerr","x measured - x guess",1000,HIST_MIN,HIST_MAX);
  yerr  = new TH1F("yerr","y measured - y guess",1000,HIST_MIN,HIST_MAX);
  defDir = defaultDir;
  resNtuple = 0;
  nsize = 0;
  setArraySize(512);
  UseEventDateTime(); // By default, use the data & time from the first event
  dbTriggersTable = 0;
  Reset();
}
//_____________________________________________________________________________
void StVertexSeedMaker::Reset() {
  minEntries = 100;   //require 100 valid verts for a seed determination
  maxX0Err     = 0.05; //vertex x should be good to 500 microns
  maxY0Err     = 0.05; //vertex y should be good to 500 microns
  mHistOut=kTRUE;
  zVertexMax = 100.0;
  zVertexMin = -100.0;
  r2VertexMax = 15.0;
  nverts = 0;
  Clear("");
  xguess = 0;
  yguess = 0;
  HIST_MIN = -3.0;
  HIST_MAX =  3.0;
  xdist->Reset();
  ydist->Reset();
  xerr->Reset();
  yerr->Reset();
  if (resNtuple) delete resNtuple;
  resNtuple = new TNtuple("resNtuple","resNtuple","event:x:y:z:mult:trig:run:fill:zdc:rank:itpc:otpc:detmap:ex:ey");
  date = 0;
  time = 0;
  fill = -1;
  run = -1;
  zdc = -1;
  rank = 0;
  itpc = 0;
  otpc = 0;
  detmap = 0;
  a[0]   = -888.0;
  //a[0]   = 0.0;
  a[1] = 0.0;
  a[2]   = 0.0;
  a[3] = 0.0;
  chi = 0.0;
}
//_____________________________________________________________________________
StVertexSeedMaker::~StVertexSeedMaker(){
}
//_____________________________________________________________________________
Int_t StVertexSeedMaker::Init(){
  AddHist(xdist);
  AddHist(ydist);
  AddHist(xerr);
  AddHist(yerr);
  return StMaker::Init();
}
//_____________________________________________________________________________
void StVertexSeedMaker::Clear(Option_t *option){
  xguess = 0;
  yguess = 0;
  zvertex = -999.0;
  yvertex = -999.0; 
  xvertex = -999.0; 
  exvertex = 0;
  eyvertex = 0;
}
//_____________________________________________________________________________
Int_t StVertexSeedMaker::Make(){
  if (date==0) FillDateTime();

  // Currently only finds database values for first event
  if (a[0] == -888) {
    LOG_INFO << "Reading db values at the start..." << endm;
    Int_t status = FillAssumed();
    if (status != kStOk) return status;
    status = GetVertexSeedTriggers();
    if (status != kStOk) return status;
  }

  if (CheckTriggers()) {   // returns false for good events
    LOG_INFO << "event does not satisfy triggers" << endm;
    return kStOk;
  }

  Int_t eventResult = GetEventData();
  if (eventResult != kStOk) return eventResult;

  if (zvertex<-998) {
    LOG_INFO << "No primary vertex" << endm;
    return kStOk;
  }

  // Calculate guessed vertex x & y from assumed params and measured z
  xguess = a[0] + (a[1] * zvertex);
  yguess = a[2] + (a[3] * zvertex);
  LOG_INFO << "x guess = " << xguess << endm; 
  LOG_INFO << "y guess = " << yguess << endm; 

  // Check to see if vertex is good for use in the fit
  float r2vertex = xvertex*xvertex + yvertex*yvertex;
  if ((zvertex > zVertexMin) && (zvertex < zVertexMax) &&
      (mult >= 5) && (r2vertex < r2VertexMax)){
    xdist->Fill(xvertex);
    xerr ->Fill(xvertex-xguess);
    ydist->Fill(yvertex);
    yerr ->Fill(yvertex-yguess);
    eventNumber = (float)GetEventNumber();
    resNtuple->Fill(eventNumber,xvertex,yvertex,zvertex,mult,trig,
                    (float) run,(float) fill,zdc,rank,
                    (float) itpc,(float) otpc,(float) detmap,exvertex,eyvertex);
    addVert(xvertex,yvertex,zvertex,mult,exvertex,eyvertex);
  }

  return kStOk;
}
//_____________________________________________________________________________
Int_t StVertexSeedMaker::Finish() {
  FindResult();
  return StMaker::Finish();
}
//_____________________________________________________________________________
Bool_t StVertexSeedMaker::CheckTriggers() {
  // Check trigger word
  StDetectorDbTriggerID* dbTriggerId = StDetectorDbTriggerID::instance();
  St_DataSet *daqReaderSet = GetDataSet("StDAQReader");
  if (!daqReaderSet) {
    LOG_ERROR << "No StDAQReader!" << endm;
    return kStErr;
  }
  StTrigSummary* trigSummary =
    ((StDAQReader*) (daqReaderSet->GetObject()))->getTrigSummary();
  UInt_t summary = 0;
  switch (dbTriggerId->getDefaultTriggerLevel()) {
    case (1) : { summary=trigSummary->L1summary[0]; break; }
    case (2) : { summary=trigSummary->L2summary[0]; break; }
    case (3) : { summary=trigSummary->L3summary[0]; break; }
    default  : {}
  }
  Bool_t notTrig = kTRUE;
  for (unsigned int iTrg = 0;
       (notTrig) && (iTrg < dbTriggerId->getIDNumRows()) ; iTrg++) {
    if (summary & (1 << (dbTriggerId->getDaqTrgId(iTrg)))) {
      if (ValidTrigger(dbTriggerId->getOfflineTrgId(iTrg))) notTrig = kFALSE;
    }
  }
  return notTrig;
}
//_____________________________________________________________________________
Bool_t StVertexSeedMaker::ValidTrigger(unsigned int tid) {
  // Determine if trigger id is among valid set
  if (!dbTriggersTable) return kTRUE; // running without DB access
  vertexSeedTriggers_st* trigsTable = dbTriggersTable->GetTable();
  Int_t nTrigs = (Int_t) dbTriggersTable->GetNRows();
  for (Int_t i = 0; i < nTrigs; i++, trigsTable++) {
    unsigned int dbTid = trigsTable->trigid;
    if (dbTid > 0 && tid == dbTid) {
      trig = (float) tid;
      return kTRUE;
    }
  }
  return kFALSE;
}
//_____________________________________________________________________________
Int_t StVertexSeedMaker::GetEventData() {
  // Get primary vertex from evr
  TDataSet *ds=GetDataSet("dst/vertex");
  if (!ds) {
    LOG_ERROR << "Cannot locate vertex dataset!" << endm;
    return kStErr;
  }
  TDataSetIter dsiter(ds);
  St_dst_vertex *vert = (St_dst_vertex *) dsiter.Find("vertex");
  if (!vert) {
    LOG_ERROR << "Cannot locate vertex table!" << endm;
    return kStErr;
  }
  dst_vertex_st *sth = vert->GetTable();
  for (int ij=0;ij<vert->GetNRows();ij++,sth++){
    if ((sth->iflag==1) && (sth->vtx_id==kEventVtxId)){
      zvertex = sth->z;
      yvertex = sth->y;
      xvertex = sth->x;
      mult = (float)(sth->n_daughters);
      break;    // found primary vertex
    }
  }
  return kStOk;
}
//_____________________________________________________________________________
void StVertexSeedMaker::FindResult(Bool_t checkDb) {
  Bool_t writeIt = kFALSE;
  if (nverts >= minEntries){
    FitData();
    if (ep[0] > maxX0Err){
      LOG_ERROR << "x unstable. x0 error = " << ep[0] << " cm." << endm;
    }
    if (ep[2] > maxY0Err){
      LOG_ERROR << "y unstable. y0 error = " << ep[2] << " cm." << endm;
    }
    if ((ep[0] <= maxX0Err) && (ep[2] <= maxY0Err)) {
      if (checkDb) {
        // Do comparison of this data with data from database to see if
        // values have changed or improved.
        LOG_INFO << "Reading db values at the end..." << endm;
        Int_t status = FillAssumed();
        if (status == kStOk) {
          if (ChangedValues() || BetterErrors()) writeIt = kTRUE;
          else { LOG_INFO << "Values have not changed/improved." << endm; }
        } else {
          LOG_WARN << "Could not get db values." << endm;
          writeIt = kTRUE;
        }
      } else {
        writeIt = kTRUE;
      }
    }
  } else {
    LOG_ERROR << "Insufficient stats for " <<
     "mean vertex determination.\n  Only " << nverts << " entries." << endm;
  }

  if (writeIt) WriteTableToFile();
  else { LOG_WARN << "Not writing table!!!!!" << endm; }

  if (mHistOut) WriteHistFile();
}
//_____________________________________________________________________________
void StVertexSeedMaker::PrintInfo() {
  LOG_INFO << "\n**************************************************************"
           << "\n* $Id: StVertexSeedMaker.cxx,v 1.41 2009/11/10 20:54:13 fisyak Exp $"
           << "\n**************************************************************" << endm;

  if (Debug()) StMaker::PrintInfo();
}
//_____________________________________________________________________________
void StVertexSeedMaker::WriteTableToFile(){
  char filename[80]; 
  if (defDir.Length()>0 && !defDir.EndsWith("/")) defDir.Append("/");
  sprintf(filename,"%svertexSeed.%08d.%06d.C",defDir.Data(),date,time);
  LOG_INFO << "Writing new table to:\n  "
    << filename << endm;
  TString dirname = gSystem->DirName(filename);
  if (gSystem->OpenDirectory(dirname.Data())==0) { 
    if (gSystem->mkdir(dirname.Data())) {
      LOG_WARN << "Directory creation failed for:\n  " << dirname
      << "\n  Putting table file in current directory" << endm;
      for (int i=0;i<80;i++){filename[i]=0;}
      sprintf(filename,"vertexSeed.%08d.%06d.C",date,time);
    }
  }
  ofstream *out = new ofstream(filename);
  VertexSeedTable()->SavePrimitive(*out,"");
  return;
}
//_____________________________________________________________________________
St_vertexSeed* StVertexSeedMaker::VertexSeedTable(){
  St_vertexSeed* table = new St_vertexSeed("vertexSeed",1);
  vertexSeed_st* row = table->GetTable();
  row->x0 = p[0];
  row->dxdz = p[1];
  row->y0 = p[2];
  row->dydz = p[3];
  row->err_x0 = ep[0];
  row->err_dxdz = ep[1];
  row->err_y0 = ep[2];
  row->err_dydz = ep[3];
  row->chisq_dof = chi;
  row->weight = 100.0; // Fixed for now!!!
  row->stats = (float) nverts;
  table->SetNRows(1);
  return table;
}
//_____________________________________________________________________________
void StVertexSeedMaker::WriteHistFile(){
  if (resNtuple->GetEntries() == 0) {
    LOG_INFO << "Not writing histograms - no entries!!!" << endm;
    return;
  }
  char filename[80]; 
  // .ROOT is NOT a typo !!!
  if (defDir.Length()>0 && !defDir.EndsWith("/")) defDir.Append("/");
  sprintf(filename,"%svertexseedhist.%08d.%06d.ROOT",defDir.Data(),date,time);
  LOG_INFO << "Writing new histograms to:\n  "
    << filename << endm;
  TString dirname = gSystem->DirName(filename);
  if (gSystem->OpenDirectory(dirname.Data())==0) { 
    if (gSystem->mkdir(dirname.Data())) {
      LOG_WARN << "Directory creation failed for:\n  " << dirname
      << "\n  Putting histogram file in current directory" << endm;
      for (int i=0;i<80;i++){filename[i]=0;}
      sprintf(filename,"vertexseedhist.%08d.%06d.ROOT",date,time);
    }
  }
  TFile out(filename,"RECREATE");
  GetHistList()->Write();
  resNtuple->Write();
  out.Close();
}
//_____________________________________________________________________________
Int_t StVertexSeedMaker::FillAssumed(){
  TDataSet* dbDataSet = GetDataBase("Calibrations/rhic");
  if (!dbDataSet) {
    LOG_ERROR << "Could not find Calibrations/rhic database" << endm;
    return kStErr;
  }
  St_vertexSeed* dbTableC =
    (St_vertexSeed*) (dbDataSet->FindObject("vertexSeed"));
  if (!dbTableC) {
    LOG_ERROR << "Could not find vertexSeed in database" << endm;
    return kStErr;
  }
  vertexSeed_st* dbTable = dbTableC->GetTable();
  a[0] = dbTable->x0;
  a[1] = dbTable->dxdz;
  a[2] = dbTable->y0;
  a[3] = dbTable->dydz;
  ea[0] = dbTable->err_x0;
  ea[1] = dbTable->err_dxdz;
  ea[2] = dbTable->err_y0;
  ea[3] = dbTable->err_dydz;
  LOG_INFO << "Assumed values:"
    << "\n     x0 assumed = " << a[0] << " +/- " << ea[0]
    << "\n   dxdz assumed = " << a[1] << " +/- " << ea[1]
    << "\n     y0 assumed = " << a[2] << " +/- " << ea[2]
    << "\n   dydz assumed = " << a[3] << " +/- " << ea[3]
    << endm;
  return kStOk;
}
//_____________________________________________________________________________
Int_t StVertexSeedMaker::GetVertexSeedTriggers(){
  TDataSet* dbDataSet = GetDataBase("Calibrations/rhic");
  if (!dbDataSet) {
    LOG_ERROR << "Could not find Calibrations/rhic database" << endm;
    return kStErr;
  }
  dbTriggersTable =
    (St_vertexSeedTriggers*) (dbDataSet->FindObject("vertexSeedTriggers"));
  if (!dbTriggersTable) {
    LOG_ERROR << "Could not find vertexSeedTriggers in database" << endm;
    return kStErr;
  }
  return kStOk;
}
//_____________________________________________________________________________
Bool_t StVertexSeedMaker::BetterErrors(){
  Bool_t better = kFALSE;
  if ((ep[0] < ea[0]) || (ep[1] < ea[1]) ||
      (ep[2] < ea[2]) || (ep[3] < ea[3])) better = kTRUE;
  if (better) { LOG_INFO << "Values have improved" << endm; }
  return better;
}
//_____________________________________________________________________________
Bool_t StVertexSeedMaker::ChangedValues(){
  Bool_t changed = kFALSE;
  for (int i = 0; i<4; i++) {
    double diff = TMath::Abs(p[i] - a[i]);
    if ((diff >= ep[i]) || (diff >= ea[i])) changed = kTRUE;
  }
  if (changed) { LOG_INFO << "Values have changed" << endm; }
  return changed;
}
//_____________________________________________________________________________
void StVertexSeedMaker::FillDateTime() {
  date = GetDate();
  time = GetTime();
  LOG_INFO << "event date = " << date << endm;
  LOG_INFO << "event time = " << time << endm;
  if (!useEventDateTime) GetFillDateTime();
}
//_____________________________________________________________________________
void StVertexSeedMaker::GetFillDateTime() {

  StDbManager* mgr=StDbManager::Instance();
  StDbConfigNode* node=mgr->initConfig("RunLog_onl");
  StDbTable* tab=node->addDbTable("beamInfo");
  StDataBaseI* db=mgr->findDb("RunLog_onl");
  unsigned int ts;
  char queryStr[128];

  // Find beamInfo entry for this date time
  sprintf(queryStr,"%08d %06d",date,time);
  TString tdStr = queryStr;
  tdStr.Insert(4,'-').Insert(7,'-').Insert(13,':').Insert(16,':');
  const char* tdstr = tdStr.Data();
  sprintf(queryStr,
    " where beginTime<='%s' and deactive=0 order by beginTime desc limit 1",
    tdstr);
  ts = db->QueryDb(tab,queryStr);

  if (ts) {
    // Find earliest entry for this fill
    run = *(int*) (tab->getDataValue("runNumber",0));
    LOG_INFO << tdstr << " is from run " << run << endm;
    float thisFill = *(float*) (tab->getDataValue("blueFillNumber",0));
    sprintf(queryStr,
      " where blueFillNumber=%f and deactive=0 order by beginTime asc limit 1",
      thisFill);
    ts = db->QueryDb(tab,queryStr);

    // Extract date and time at start of fill
    char* start = tab->getBeginDateTime();
    fill = (int) thisFill;
    time = atoi(&(start[8]));
    start[8] = 0;
    date = atoi(start);

    LOG_INFO << "Using fill no.  = " << fill << endm;
    LOG_INFO << "Using fill date = " << date << endm;
    LOG_INFO << "Using fill time = " << time << endm;
  } else {
    LOG_WARN << "Could not find beamInfo in database\n" <<
      "  Using event date/time." << endm;
    UseEventDateTime();
  }
}
//_____________________________________________________________________________
void StVertexSeedMaker::FitData() {
   LOG_INFO << "Now fitting the data..." <<
     "\n  *****************************************************" << endm;
   TVirtualFitter *minuit = TVirtualFitter::Fitter(0,4);
   minuit->SetFCN(fnch);

// Set starting values and step sizes for parameters
   static Double_t pstart[4] = {0., 0., 0., 0.};
   static Double_t pstep[4]  = {0.0001, 0.0000001, 0.0001, 0.0000001};
   static Double_t plow[4]   = {-3., -0.05, -3., -0.05};
   static Double_t phigh[4]  = { 3.,  0.05,  3.,  0.05};
   minuit->SetParameter(0, "x0"  , pstart[0], pstep[0], plow[0], phigh[0]);
   minuit->SetParameter(1, "dxdz", pstart[1], pstep[1], plow[1], phigh[1]);
   minuit->SetParameter(2, "y0"  , pstart[2], pstep[2], plow[2], phigh[2]);
   minuit->SetParameter(3, "dydz", pstart[3], pstep[3], plow[3], phigh[3]);

// Now ready for minimization step
   double arglist[10];
   arglist[0] = 500;

   beamWidth = 0;
   do {

   int status = minuit->ExecuteCommand("MIGRAD", arglist ,1);
   if (status) {
     LOG_ERROR << "StVertexMaker: error on migrad call, err = "
       << status << endm;
     return;
   }
   
   double amin,edm,errdef;
   int nvpar,nparx;
   minuit->GetStats(amin,edm,errdef,nvpar,nparx);
   chi = amin/((double) (nverts-4));
   LOG_INFO << "beamWidth = " << beamWidth << ", chisq = " << amin << ", chisq/dof = " << chi <<
     "\n  *****************************************************" << endm;

   beamWidth += 0.005; // 50 micron steps
   } while (chi>1.1 && beamWidth<=0.15);
   
   char pname[10];
   for (int i=0; i<4; i++)
     minuit->GetParameter(i, pname, p[i], ep[i], plow[i], phigh[i]);
}
//_____________________________________________________________________________
Int_t StVertexSeedMaker::Aggregate(Char_t* dir, const Char_t* cuts) {
  // Format of filenames for parsing must be:
  // vertexseedhist.DDDDDDDD.TTTTTT.root
  // where D and T are 8 and 6 digit representations of date and time

  const char* defaultDir = "./";
  TString dirStr = dir;
  if (!dir) dirStr = defaultDir;
  if (dirStr.EndsWith("/")) dirStr.Append("vertexseedhist.*.root");
  StFile allFiles;
  allFiles.AddFile(dirStr.Data());
  // allFiles.ls(); allFiles.Rewind();
  TList fileList;
  const char* fileName;
  for (int fileb = 0; fileb < allFiles.GetNBundles(); fileb++) {
    allFiles.GetNextBundle();
    int filen=0;
    while ((fileName = allFiles.GetFileName(filen++))) {
      fileList.Add(new TNamed(fileName,fileName));
    }
  }
  fileList.Sort();

  TFile* currentFile=0;
  float* vals=0;
  int nfiles = fileList.GetSize();
  for (int filen = 0; filen < nfiles; filen++) {
    int fillf = fill;
    int datef = date;
    int timef = time;
    fileName = fileList.At(filen)->GetName();
    TString dateTime = fileName;
    dateTime.Remove(0,dateTime.Last('/') + 1);
    dateTime.Remove(0,dateTime.First('.') + 1).Remove(15);
    TString dateStr = dateTime;
    date = atoi(dateStr.Remove(8).Data());
    time = atoi(dateTime.Remove(0,9).Remove(6).Data());
    GetFillDateTime();
    if ((currentFile) && (fill != fillf)) {
      LOG_INFO << "Fill number has changed\n"
        << "  Processing data from previous fill before continuing" << endm;
      int fillp = fill;
      int datep = date;
      int timep = time;
      fill = fillf;
      date = datef;
      time = timef;
      currentFile->Close();
      currentFile = 0;
      FindResult(kFALSE);
      Reset();
      fill = fillp;
      date = datep;
      time = timep;
    }

    LOG_INFO << "Now opening file:\n  " << fileName << endm;
    if (currentFile) currentFile->Close();
    currentFile = new TFile(fileName);
    TNtuple* curNtuple = (TNtuple*) currentFile->Get("resNtuple");
    if (!curNtuple) {
      LOG_ERROR << "No resNtuple found in " << fileName << endm;
      continue;
    }
    curNtuple->Draw(">>elist",cuts);
    TEventList* elist = (TEventList*) gDirectory->Get("elist");
    Int_t nentries = (Int_t) elist->GetN();
    for (Int_t entryn = 0; entryn < nentries; entryn++) {
      curNtuple->GetEntry(elist->GetEntry(entryn));
      vals = curNtuple->GetArgs();
      unsigned int tid = (unsigned int) vals[5];
      if (ValidTrigger(tid)) {
        resNtuple->Fill(vals);
        if (curNtuple->GetNvar()>12) 
          addVert(vals[1],vals[2],vals[3],vals[4],vals[12],vals[13]);
        else
          addVert(vals[1],vals[2],vals[3],vals[4],0.,0.);
      } else {
        LOG_INFO << "Invalid trigger: " << tid << endm;
      }
    }
    LOG_INFO << "Current statistics: " << nverts << endm;
  }
  if (currentFile) {
    currentFile->Close();
    currentFile = 0;
    FindResult(kFALSE);
  }
  LOG_INFO << "Examined " << nfiles << " files" << endm;
  return nfiles;
}
//_____________________________________________________________________________
// $Id: StVertexSeedMaker.cxx,v 1.41 2009/11/10 20:54:13 fisyak Exp $
// $Log: StVertexSeedMaker.cxx,v $
// Revision 1.41  2009/11/10 20:54:13  fisyak
// pams Cleanup
//
// Revision 1.40  2009/05/22 23:50:50  genevb
// Code mods for BEMC matches, BeamWidth
//
// Revision 1.39  2008/05/21 17:48:39  genevb
// Use vertex errors for weighting
//
// Revision 1.38  2008/04/29 23:30:33  genevb
// Added cuts capability to Aggregate
//
// Revision 1.37  2008/04/29 19:06:06  genevb
// handle no DB access
//
// Revision 1.36  2007/11/27 23:42:47  genevb
// Move valid triggers from code to DB
//
// Revision 1.35  2007/07/12 19:46:56  fisyak
// Add includes for ROOT 5.16
//
// Revision 1.34  2007/04/28 17:56:31  perev
// Redundant StChain.h removed
//
// Revision 1.33  2007/04/22 04:25:59  genevb
// printf, gMessMgr ==> STAR Logger
//
// Revision 1.32  2006/09/01 22:27:16  genevb
// More detailed info in ntuple
//
// Revision 1.31  2006/08/16 21:58:01  genevb
// Added 2006 pp62 triggers
//
// Revision 1.30  2006/05/27 19:54:03  genevb
// Yet more 2006 triggers
//
// Revision 1.29  2006/05/11 18:09:44  genevb
// More pp2006 triggers
//
// Revision 1.28  2006/05/10 03:57:08  genevb
// ppProductionTrans trigger for 2006
//
// Revision 1.27  2006/05/08 02:38:21  genevb
// Added 2006 triggers
//
// Revision 1.26  2005/07/16 21:24:03  genevb
// Fixed bug with pp400 data from 2005
//
// Revision 1.25  2005/07/15 18:38:47  genevb
// ppTrans triggers, and fix for too many open files
//
// Revision 1.24  2005/07/14 21:02:40  genevb
// Modified use of test triggers
//
// Revision 1.23  2005/07/01 21:46:01  genevb
// Specify output directory
//
// Revision 1.22  2005/06/14 18:51:31  genevb
// Updates to allow for pp2005 triggers, and inheritance
//
// Revision 1.21  2004/08/02 01:19:33  genevb
// minor fixes for getting directories correct
//
// Revision 1.20  2004/07/23 16:56:01  genevb
// 2004 pp triggers
//
// Revision 1.19  2003/09/02 17:58:45  perev
// gcc 3.2 updates + WarnOff
//
// Revision 1.18  2003/05/30 19:02:51  genevb
// Add ppFPDw-slow
//
// Revision 1.17  2003/05/19 14:34:54  genevb
// Add ppFPD slow triggers
//
// Revision 1.16  2003/04/22 19:20:52  genevb
// Add ppMinBias triggers
//
// Revision 1.15  2003/03/21 15:12:24  genevb
// Allow use of TOF triggers
//
// Revision 1.14  2003/02/25 03:49:16  genevb
// Choose only dAu minbias triggers
//
// Revision 1.13  2003/02/12 04:19:39  genevb
// Small improvements
//
// Revision 1.12  2003/02/11 22:24:20  genevb
// Update to use beamInfo table from database
//
// Revision 1.11  2003/01/22 23:32:23  genevb
// Type cast fix
//
// Revision 1.10  2002/04/19 22:24:16  perev
// fixes for ROOT/3.02.07
//
// Revision 1.9  2002/03/23 01:05:15  jeromel
// Create files with extension .ROOT instead of .root (db_Maker will read the .root
// and crash otherwise). Will fix this with a more elegant scheme later.
//
// Revision 1.8  2002/03/23 00:23:54  genevb
// switch from float arrays to TArrayF
//
// Revision 1.7  2002/03/22 20:29:09  jeromel
// Not .C but .root
//
// Revision 1.6  2002/03/20 21:09:10  genevb
// Oops...introduced a typo last time
//
// Revision 1.5  2002/03/20 19:22:59  genevb
// changed output directory of histogram files
//
// Revision 1.4  2002/03/20 00:40:42  genevb
// Addition of Aggregate feature, minor updates
//
// Revision 1.3  2002/01/28 22:16:33  genevb
// Some revisions to output date/time stamps
//
// Revision 1.2  2002/01/27 01:56:14  genevb
// Write db files with date/time for fill
//
// Revision 1.1  2002/01/26 18:55:33  jeromel
// StTpcT0Maker moved from directory of the same name. First version
// of StVertexSeedMaker.
//
//
