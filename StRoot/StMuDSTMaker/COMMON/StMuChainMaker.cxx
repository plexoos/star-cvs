/***************************************************************************
 *
 * $Id: StMuChainMaker.cxx,v 1.1 2002/04/01 22:42:29 laue Exp $
 * Author: Frank Laue, BNL, laue@bnl.gov
 *
 **************************************************************************/
#include <fstream>

#include "StMuException.hh"
#include "StMuDebug.h"
#include "StMuChainMaker.h"

#include "StMaker.h"
#include "StChain.h"

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include "TSystem.h"

extern TSystem* gSystem;

ClassImp(StMuChainMaker)
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StMuChainMaker::StMuChainMaker(const char* name) : mTreeName(name) {
  DEBUGMESSAGE2("");
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
StMuChainMaker::~StMuChainMaker() {
  DEBUGMESSAGE2("");
  int n=0;
  while (mSubFilters[n]) {
    delete mSubFilters[n++];
  }
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
string StMuChainMaker::buildFileName(string dir, string fileName, string extention){
  DEBUGMESSAGE2("");
  fileName = dir + fileName + extention;
  return fileName;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
string StMuChainMaker::basename(string s){
  string name(s);
  size_t pos;
  pos = name.find_last_of("/");
  if (pos!=string::npos ) name.erase(0, pos+1 );
  pos = name.find_first_of(".");
  if (pos!=string::npos ) name.erase(pos,name.length() );
  DEBUGVALUE2(name);
  return name;
} 
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
string StMuChainMaker::dirname(string s){
  string name(s);
  string base(basename(s));
  name.erase(name.find(base),base.length());
  size_t pos;
  pos = name.find_last_of("/");
  if (pos!=string::npos ) name.erase(pos, name.length());
  if (name=="/") name = "";
  DEBUGVALUE2(name);
  return name;
} 
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
TChain*  StMuChainMaker::make(string dir, string file, string filter, int maxFiles) {
  DEBUGMESSAGE1("");
  TChain* chain;
  if (file!="") {
    if (file.find(".lis")!=string::npos) chain =  fromList(dir, file, maxFiles);
    if (file.find(".MuDst.root")!=string::npos) chain = fromFile(dir,file, maxFiles);
  }
  else {
    chain = fromDir(dir,filter, maxFiles);
  }
  DEBUGMESSAGE2("return");
  return chain;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
string**  StMuChainMaker::subFilter(string filter) {
  DEBUGMESSAGE2("");
  string** subFilter = new string*[100];
  string tmp(filter);
  int n=0;
  size_t pos=0;
  while (tmp.find_first_of(":")!=string::npos ) {
    pos = tmp.find_first_of(":");
    subFilter[n] = new string( tmp.substr(0,pos) );
    tmp.erase(0,pos+1);
    n++;
  }				
  subFilter[n++] = new string( tmp );
  DEBUGMESSAGE2("return");
  return subFilter;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
TChain* StMuChainMaker::fromDir(string dir, string filter, int maxFiles) {
  DEBUGMESSAGE2("");
  TChain* chain = new TChain(mTreeName.c_str());
  mSubFilters = subFilter(filter);
  DEBUGVALUE(gSystem);

  void *pDir = gSystem->OpenDirectory(dir.c_str());
  // now find the files that end in the specified extention
  const char* fileName(0);
  int fileCount(0);
  while((fileName = gSystem->GetDirEntry(pDir))){
    bool good = true;
    string name(fileName);
    if( strcmp(fileName,".")==0 || strcmp(fileName,"..")==0) good=false;
    if( strcmp(fileName,".root")==0 || strcmp(fileName,"..")==0) good=false;
    if ( name.find(".MuDst.root")==string::npos ) good=false;
    if ( pass(name,mSubFilters) ) {
      char* fullFile = gSystem->ConcatFileName(dir.c_str(),fileName);
      // add it to the chain
      DEBUGVALUE(fileCount);
      chain->Add(fullFile);
      fileCount++;
      delete fullFile;
    }
    if(fileCount >= maxFiles) break;
  }   
  DEBUGVALUE2(fileCount);
  return chain;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
TChain* StMuChainMaker::fromList(string dir, string list, int maxFiles) {
  DEBUGMESSAGE2("");
  TChain* chain = new TChain(mTreeName.c_str());
  ifstream* inputStream = new ifstream;
  inputStream->open(list.c_str());
  if (!(inputStream)) {
    DEBUGMESSAGE("can not open list file");
    return chain;
  }
  char* temp;
  int fileCount=0;
  DEBUGVALUE(inputStream->good());
  for (;inputStream->good();) {
    temp = new char[200];
    inputStream->getline(temp,200);
    DEBUGVALUE2(temp);
    chain->Add(temp);
    delete temp;
    ++fileCount;
    if (fileCount>maxFiles) break;
  }   
  delete inputStream;
  DEBUGVALUE2(fileCount);
  return chain;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
TChain* StMuChainMaker::fromFile(string dir, string list, int maxFiles) {
  DEBUGMESSAGE2("");
  return 0;
}
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
bool StMuChainMaker::pass(string file, string**  filters) {
  bool good = true;
  int n=0;
  while (filters[n] && good) {
    if ( file.find(*filters[n])==string::npos ) good=false;
    n++;
  }
  return good;
}
/***************************************************************************
 *
 * $Log: StMuChainMaker.cxx,v $
 * Revision 1.1  2002/04/01 22:42:29  laue
 * improved chain filter options
 *
 *
 **************************************************************************/


















