#include "StOnlineTriggerMonitoring.h"
#include <iostream>
#include <fstream>
using namespace std;

#include <TROOT.h>
#include <TSystem.h>
#include <TFile.h>
#include <TString.h>

#include "StDbLib/StDbManager.hh"
#include "StDbLib/StDbConfigNode.hh"
#include "StDbLib/StDbTable.h"
#include "StDbLib/StDataBaseI.hh"

#include "tables/St_emcTriggerStatus_Table.h"
#include "tables/St_emcTriggerPed_Table.h"
#include "tables/St_emcTriggerLUT_Table.h"

#include "StDaqLib/EMC/StEmcDecoder.h"

char BITMASK[15][130];
int PATCH[300], DSM[300], BIT[300], PA[300], HT[300], TOWER[30][160], PED[30][160], BITCONV[30][10];
int FORMULATAG[30][10],FORMULAPARAMETER[30][10][6];
int PEDSHIFT;

ClassImp(StOnlineTriggerMonitoring)

int unpackbits(char *data, int nbits, int index)
{
  TString a = data;
	TString v= a(index,nbits);
	int b = strtol(v.Data(),NULL,2);
	return b;
}
void swapline(char*pointer)
{
	char T[16];
	for(int i=0;i<16;i++) T[i] = pointer[i];
	pointer[8] = T[4];
	pointer[9] = T[5];
	pointer[10] = T[6];
	pointer[11] = T[7];
	pointer[12] = T[0];
	pointer[13] = T[1];
	pointer[14] = T[2];
	pointer[15] = T[3];
	
	pointer[0] = T[12];
	pointer[1] = T[13];
	pointer[2] = T[14];
	pointer[3] = T[15];
	pointer[4] = T[8];
	pointer[5] = T[9];
	pointer[6] = T[10];
	pointer[7] = T[11];
}
void makeString(unsigned char* data,int nb, TString &result)
{
	result = "";
	for(int i=0;i<nb;i++) for (Int_t j = 7;j >= 0;j--) result += (((short)data[i] & (1 << j)) ? "1" : "0");
}
void loadTriggerMasks(char* file,int start)
{
	FILE *W = fopen(file,"rb");
	unsigned char mask[16];
	for(int dsm=1;dsm<=15;dsm++)
	{
	  char * pointer = (char*)mask;
		fread(pointer,(int)sizeof(unsigned char)*16,1,W);
		swapline(pointer);
		TString result;
		makeString((unsigned char*) pointer,16, result);
		sprintf(BITMASK[dsm-1],"%s",result.Data());
		//for(int i=0;i<16;i++) cout <<hex<<(unsigned int)mask[i]<<" "; cout <<dec<<endl;
		//cout << "<" << result << ">" << endl;
	}
	cout <<"Decoding the data "<<endl;
	for(int patch = 1; patch<=150;patch++)
	{
		int dsm = DSM[patch-1+start];
		int bit = BIT[patch-1+start];
		int ht = unpackbits((char*)BITMASK[dsm-1],6,128-bit-6);
		int pa = unpackbits((char*)BITMASK[dsm-1],6,128-bit-12);
		if(ht==63) HT[patch-1+start] = 1; else HT[patch-1+start] = 0;
		if(pa==63) PA[patch-1+start] = 1; else PA[patch-1+start] = 0;
		
		//cout <<endl; 
		//cout <<"Patch = "<<patch-1+start<<" DSM = "<<dsm<<"  BIT = "<<bit
		//     <<"  htv = "<<ht<<"  pav = "<<pa 
		//     <<"  HT = "<<HT[patch-1+start]<<"  PA = "<<PA[patch-1+start]<<endl;
		//cout <<"00000000011111111112222222222333333333344444444445555555555666666666677777777778888888888999999999900000000001111111111222222222\n";
		//cout <<BITMASK[dsm-1]<<endl;
	}
	fclose(W);
}

void StOnlineTriggerMonitoring::saveTrigger(char* TS, bool status, bool pedestal, bool lut, bool saveDB, char* startrg2, char* sc3, char* bemcStatusCopy, char *bceTable, char *bcwTable) {
  cout <<"startrg2 dir = "<<startrg2<<endl;
  cout <<"sc3 dir      = "<<sc3<<endl;
  cout <<"bemcStatusCopy = "<<bemcStatusCopy<<endl;
  cout <<"bceTable       = "<< bceTable <<endl;
  cout <<"bcwTable       = "<< bcwTable <<endl;
	
	TString FILENAME;
	
	cout <<"Saving trigger tables in offline DB for TS = "<<TS<<endl;
	
	// trigger decoding map
	// load the files bcw_table.txt and bce_table.txt
	// used in John Nelson's conv_lut program
	
	ifstream inw(bcwTable);
	for(int i=0;i<150;i++) {
	    inw >> PATCH[i]>>DSM[i]>>BIT[i];
	    //DSM[i] = ((PATCH[i] - 1) / 10) + 1;
	}
	inw.close();
	ifstream ine(bceTable);
	for(int i=0;i<150;i++) {
	    ine >> PATCH[i+150]>>DSM[i+150]>>BIT[i+150];
	    //DSM[i+150] = ((PATCH[i+150] - 1) / 10) + 1;
	}
	ine.close();
	
	FILENAME = startrg2; FILENAME+="/bcw.lut.bin";
	cout <<"Loading trigger masks from "<<FILENAME.Data()<<endl;
	loadTriggerMasks((char*)FILENAME.Data(),0);
	FILENAME = startrg2; FILENAME+="/bce.lut.bin";
	cout <<"Loading trigger masks from "<<FILENAME.Data()<<endl;
	loadTriggerMasks((char*)FILENAME.Data(),150);
	
	int NHT=0, NPA=0;
	
	for(int i=0;i<300;i++)
	{
	  if(HT[i]==0) cout <<"High Tower "<<i<<" masked out\n";
	  if(PA[i]==0) cout <<"Patch sum  "<<i<<" masked out\n";
	  if(HT[i]==0) NHT++;
	  if(PA[i]==0) NPA++;
	}
	cout <<"Number of high towers masked out     = "<<NHT<<endl;
	cout <<"Number of trigger patches masked out = "<<NPA<<endl;
	
	// reading online pedestals...
	
	for(int crate = 1;crate<=30;crate++)
	{
		FILENAME = sc3;
		FILENAME+="/pedestal_crate0x";
		char str[10];
		sprintf(str,"%02x",crate);
		FILENAME+=str;
		FILENAME+=".dat";
  
	  long id,flag,mod;
    Long64_t size;
    if(gSystem->GetPathInfo(FILENAME.Data(),&id,&size,&flag,&mod)==0)
		{
		  cout <<FILENAME.Data()<<"  is Ok\n";
			ifstream in(FILENAME.Data());
			int towerid;
			float ped, rms;
			for(int i=0;i<160;i++) 
			{
			  in>>towerid>>ped>>rms;
				PED[crate-1][towerid] = (int)(ped*100);
				//cout <<" Crate = "<<crate<<"  index = "<<towerid<<"  ped (x100) = "<<PED[crate-1][towerid]<<endl;
			}
			in.close();
		}
		else for(int i=0;i<160;i++) PED[crate-1][i]=0;		
	}
	
	// reading BemcConfig.dat
	FILENAME = sc3; FILENAME+="/BemcConfig.dat";
	ifstream bemcConf(FILENAME.Data());
	PEDSHIFT = 0;
	char line[4096];
	while(!bemcConf.eof())
	{ 
	  bemcConf.getline(line,4096);
		//cout << line << endl;
		TString L = line;
		if(L.Index("TriggerPedestalShift")>=0)
		{ 
		  TString b = L(L.Index("TriggerPedestalShift")+21,L.Length());
			if(b.BeginsWith("0x")) PEDSHIFT = strtol(b.Data(),NULL,16);
			else PEDSHIFT = atoi(b.Data());
			PEDSHIFT*=100;
		}
	}
	
	// reading single tower masks and trigger bit conversion mode
	for(int crate = 1;crate<=30;crate++)
	{
		FILENAME = sc3; FILENAME+= "/config_crate0x";
		char str[10];
		sprintf(str,"%02x",crate);
		FILENAME+=str;
		FILENAME+=".dat";
  
	  long id,flag,mod;
    Long64_t size;
    if(gSystem->GetPathInfo(FILENAME.Data(),&id,&size,&flag,&mod)==0)
		{
		  cout <<FILENAME.Data()<<"  is Ok\n";
			ifstream in(FILENAME.Data());
			char tmp[50];
			short bit6[2][5],mask[2][5],formula[2][5],par[2][5][6];
			in >> tmp; // nodeId
			in >> tmp; // rhic clock latency
			in >> tmp; // led latency
			in >> tmp; // led amplitude
			in >> tmp; // led mask 1
			in >> tmp; // led mask 2
			in >> tmp; // pulser delay
			in >> tmp; // pulser latency
						
			for(int board = 1; board<=5;board++)
			{
			  in >> tmp; // fifo latency
				in >> tmp; // calibration pulser amplitude
				in >> tmp; // calibration pulser mask 1
				
				for(int patch = 0;patch<2;patch++)
				{
				  in >> tmp; // trigger mask 1;
			    TString A1 = tmp;
			    if(A1.BeginsWith("0x")) mask[patch][board-1] = strtol(A1.Data(),NULL,16);
			    else mask[patch][board-1] = atoi(A1.Data());
				
				  in >> tmp; // formula tag 1
				  TString A11 = tmp;
			    if(A11.BeginsWith("0x")) formula[patch][board-1] = strtol(A11.Data(),NULL,16);
			    else formula[patch][board-1] = atoi(A11.Data());
					
				  for(int f = 0;f<6;f++) // formula parameters
				  {  
				    in >> tmp;
					  TString A12 = tmp;
			      if(A12.BeginsWith("0x")) par[patch][board-1][f] = strtol(A12.Data(),NULL,16);
			      else par[patch][board-1][f] = atoi(A12.Data());
				  }
					
			    in >> tmp; // high tower 6 bits 4
			    TString A = tmp;
			    if(A.BeginsWith("0x")) bit6[patch][board-1] = strtol(A.Data(),NULL,16);
			    else bit6[patch][board-1] = atoi(A.Data());
//cout << "!!!!!! " << bit6[patch][board-1] << endl;
				}
			}
			if (!in.eof()) {
			    for(int board = 0; board<=5;board++) {
				in >> tmp; // inrun
				in >> tmp; // ht
				if (board >= 1) {
				    Bool_t boardHT = atoi(tmp);
				    if (!boardHT) {
					cout << "Board " << board << " HT peds not configured" << endl;
					// zero out pedestals
				    }
				}
				in >> tmp; // jp
				if (board >= 1) {
				    Bool_t boardJP = atoi(tmp);
				    if (!boardJP) {
					cout << "Board " << board << " LUT not configured" << endl;
					formula[0][board-1] = 0; // LUT formula = 0
					formula[1][board-1] = 0; // LUT formula = 0
					par[0][board-1][0] = 1; // LUT scale = 1
					par[1][board-1][0] = 1; // LUT scale = 1
					par[0][board-1][1] = 0; // LUT ped = 0
					par[1][board-1][1] = 0; // LUT ped = 0
					par[0][board-1][2] = 0; // LUT sigma = 0
					par[1][board-1][2] = 0; // LUT sigma = 0
					par[0][board-1][3] = 1; // LUT powerup = 0
					par[1][board-1][3] = 1; // LUT powerup = 0
					par[0][board-1][4] = 0; // LUT par4 = 0
					par[1][board-1][4] = 0; // LUT par4 = 0
					par[0][board-1][5] = 0; // LUT par5 = 0
					par[1][board-1][5] = 0; // LUT par5 = 0
				    }
				}
				in >> tmp; // checkconfig
				in >> tmp; // checkped
				in >> tmp; // checklut
			    }
			}
			for(int i=0;i<160;i++)
			{
				int board = i/32+1;
				int channel = i%32;
				int patch = channel/16;
				int m = mask[patch][board-1];
				int bit = channel-16*patch;
				FORMULATAG[crate-1][i/16] = formula[patch][board-1];
				for(int p = 0;p<6;p++)
				   FORMULAPARAMETER[crate-1][i/16][p] = par[patch][board-1][p];
			  BITCONV[crate-1][i/16] = bit6[patch][board-1];
				if( ((m>>bit) & 0x1)==1 ) TOWER[crate-1][i] = 1; else  TOWER[crate-1][i] = 0;
				if(TOWER[crate-1][i] == 0) cout <<"  Masked out channel "<<i<<" from crate "<<crate<<endl;
			}
			in.close();
		}
		else for(int i=0;i<160;i++) { BITCONV[crate-1][i/16]=0; TOWER[crate-1][i]=0;}
	}
          
	// create tables and save them to the database
	int first=-1;
	TString TSp = TS;
  TString timestamp=TSp(first+1,4);
  timestamp+="-";
  timestamp+=TSp(first+5,2);
  timestamp+="-";
  timestamp+=TSp(first+7,2);
  timestamp+=" ";
  timestamp+=TSp(first+10,2);
  timestamp+=":";
  timestamp+=TSp(first+12,2);
  timestamp+=":";
  timestamp+=TSp(first+14,2);
	cout <<"TS = "<<TS<<"  TimeStamp = "<<timestamp<<endl;

    {
    ofstream bemcStatusStream(bemcStatusCopy);
    bemcStatusStream << "##################################################################################" << endl;
    bemcStatusStream << "# This plain text file contains the complete BEMC trigger configuration" << endl;
    bemcStatusStream << "# Generated by the online BEMC trigger monitoring program on emc01.starp.bnl.gov" << endl;
    bemcStatusStream << "# Timestamp: " << timestamp << endl;

    StEmcDecoder *decoder = new StEmcDecoder();
    
    int towerData[4800][7];
    for (int i = 0;i < 4800;i++) {
	for (int j = 0;j < 7;j++) {
	    towerData[i][j] = 0;
	}
    }
    for(int crate = 1;crate <= 30;crate++) {
	for(int tower = 0;tower < 160;tower++) {
	    int softId;
	    if (decoder && decoder->GetTowerIdFromCrate(crate, tower, softId)) {
		towerData[softId - 1][0] = TOWER[crate - 1][tower];
		towerData[softId - 1][3] = PED[crate - 1][tower];
		towerData[softId - 1][4] = crate;
		towerData[softId - 1][5] = tower;
		int triggerPatch;
		if (decoder->GetTriggerPatchFromCrate(crate, tower, triggerPatch)) {
		    towerData[softId - 1][6] = triggerPatch;
		    towerData[softId - 1][1] = HT[triggerPatch];
		    towerData[softId - 1][2] = PA[triggerPatch];
		}
	    }
	}
    }
    bemcStatusStream << "#" << endl;
    bemcStatusStream << "# SoftId\tCrate\tCrate seq\tTower unmasked?\tPatch unmasked in HT?\tPatch unmasked in sum?\tPedestal\ttriggerPatch" << endl;
    for (int i = 0;i < 4800;i++) {
	bemcStatusStream << "SoftId " << (i + 1) << "\t" << towerData[i][4] << "\t" << towerData[i][5] << "\t" << towerData[i][0] << "\t" << towerData[i][1] << "\t" << towerData[i][2] << "\t" << (Float_t(towerData[i][3]) / 100.0) << "\t" << towerData[i][6] << endl;
    }
    bemcStatusStream << "#" << endl;

    bemcStatusStream << "TriggerPedestalShift " << (float(PEDSHIFT)/100.0) << endl;
    bemcStatusStream << "#" << endl;
    
    bemcStatusStream << "# triggerPatch\tCrate\tCrate patch\tUnmasked in HT?\tUnmasked in sum?\tBit conversion mode\tLUT formula and parameters" << endl;
    for(int crate = 1;crate <= 30;crate++) {
	for(int patch = 0;patch < 10;patch++) {
	    int triggerPatch;
	    if (decoder->GetTriggerPatchFromCrate(crate, patch*16, triggerPatch)) {
		bemcStatusStream << "triggerPatch " << triggerPatch << "\t" << crate << "\t" << patch << "\t";
		bemcStatusStream << HT[triggerPatch] << "\t";
		bemcStatusStream << PA[triggerPatch] << "\t";
		bemcStatusStream << BITCONV[crate-1][patch] << "\t";
		bemcStatusStream << FORMULATAG[crate-1][patch] << "\t";
	        bemcStatusStream << FORMULAPARAMETER[crate-1][patch][0] << "\t";
		bemcStatusStream << FORMULAPARAMETER[crate-1][patch][1] << "\t";
		bemcStatusStream << FORMULAPARAMETER[crate-1][patch][2] << "\t";
		bemcStatusStream << FORMULAPARAMETER[crate-1][patch][3] << "\t";
		bemcStatusStream << FORMULAPARAMETER[crate-1][patch][4] << "\t";
		bemcStatusStream << FORMULAPARAMETER[crate-1][patch][5];
		bemcStatusStream << endl;
	    }
	}
    }

    if (decoder) delete decoder; decoder = 0;
    bemcStatusStream << "# End of file" << endl;
    bemcStatusStream << "##################################################################################" << endl;
    bemcStatusStream.close();
    }
    
	St_emcTriggerPed *pedestals_t = new St_emcTriggerPed("bemcTriggerPed",1);
	St_emcTriggerStatus *status_t = new St_emcTriggerStatus("bemcTriggerStatus",1);
	St_emcTriggerLUT *lut_t = new St_emcTriggerLUT("bemcTriggerLUT",1);
	
	emcTriggerPed_st* pedestals_st = pedestals_t ? pedestals_t->GetTable() : 0;
	emcTriggerStatus_st* status_st = status_t ? status_t->GetTable() : 0;
	emcTriggerLUT_st* lut_st = lut_t ? lut_t->GetTable() : 0;
	
	if (pedestals_st) pedestals_st->PedShift = PEDSHIFT;
	for(int crate = 1;crate<=30;crate++) {
	    for(int patch =0;patch<10;patch++) {
		if (pedestals_st) pedestals_st->BitConversionMode[crate-1][patch] = BITCONV[crate-1][patch];
		if (lut_st) {
		    lut_st->FormulaTag[crate-1][patch] = FORMULATAG[crate-1][patch];
		    lut_st->FormulaParameter0[crate-1][patch] = FORMULAPARAMETER[crate-1][patch][0];
		    lut_st->FormulaParameter1[crate-1][patch] = FORMULAPARAMETER[crate-1][patch][1];
		    lut_st->FormulaParameter2[crate-1][patch] = FORMULAPARAMETER[crate-1][patch][2];
		    lut_st->FormulaParameter3[crate-1][patch] = FORMULAPARAMETER[crate-1][patch][3];
		    lut_st->FormulaParameter4[crate-1][patch] = FORMULAPARAMETER[crate-1][patch][4];
		    lut_st->FormulaParameter5[crate-1][patch] = FORMULAPARAMETER[crate-1][patch][5];
		}
	    }
	    for(int tower =0;tower<160;tower++) {
		if (pedestals_st) pedestals_st->Ped[crate-1][tower]=PED[crate-1][tower];
		if (status_st) status_st->TowerStatus[crate-1][tower]=TOWER[crate-1][tower];			
	    }
	}
	for(int patch=0;patch<300;patch++) {
	    if (status_st) {
		status_st->PatchStatus[patch]=PA[patch];
		status_st->HighTowerStatus[patch]=HT[patch];
	    }
	}
	
	StDbManager* mgr = 0;
	StDbConfigNode* node = 0;
	StDbTable *t = 0;
	if(saveDB) {
	    TString dbName  = "Calibrations_emc";
	    mgr=StDbManager::Instance();
	    node=mgr->initConfig(dbName.Data());
	}

	if (status) {
	    if (saveDB && mgr) {
		t = node ? node->addDbTable("bemcTriggerStatus") : 0;
		if (t) {
		    t->setFlavor("ofl");
		    t->SetTable((char*)status_st,1);
		    mgr->setStoreTime(timestamp.Data());
		    mgr->storeDbTable(t);
		}
	    }
	    FILENAME = "StarDb/Calibrations/emc/trigger/bemcTriggerStatus."; FILENAME+=TS; FILENAME+=".root";
	    TFile *FF = new TFile(FILENAME.Data(),"RECREATE");
	    FF->cd();
	    status_t->AddAt(status_st,0);
	    status_t->Write();
	    FF->Close();
	    delete FF;		
	}
	if (pedestal) {
	    if (saveDB && mgr) {
		t = node ? node->addDbTable("bemcTriggerPed") : 0;
	        if (t) {
		    t->setFlavor("ofl");
		    t->SetTable((char*)pedestals_st,1);
		    mgr->setStoreTime(timestamp.Data());
		    mgr->storeDbTable(t);
		}
	    }
	    FILENAME = "StarDb/Calibrations/emc/trigger/bemcTriggerPed."; FILENAME+=TS; FILENAME+=".root";
	    TFile *FF = new TFile(FILENAME.Data(),"RECREATE");
	    FF->cd();
	    pedestals_t->AddAt(pedestals_st,0);
	    pedestals_t->Write();
	    FF->Close();
	    delete FF;		
	}
	if (lut) {
	    if(saveDB) {
		t = node ? node->addDbTable("bemcTriggerLUT") : 0;
		if (t) {
		    t->setFlavor("ofl");
		    t->SetTable((char*)lut_st,1);
		    mgr->setStoreTime(timestamp.Data());
		    mgr->storeDbTable(t);
		}
	    }
	    FILENAME = "StarDb/Calibrations/emc/trigger/bemcTriggerLUT."; FILENAME+=TS; FILENAME+=".root";
	    TFile *FF = new TFile(FILENAME.Data(),"RECREATE");
	    FF->cd();
	    lut_t->AddAt(lut_st,0);
	    lut_t->Write();
	    FF->Close();
	    delete FF;
	}
}

