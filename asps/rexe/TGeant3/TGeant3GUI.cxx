/* *************************************************************************
 * Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/*
$Log: TGeant3GUI.cxx,v $
Revision 1.2  2000/04/23 19:18:13  fisyak
Merge with Alice V3.03

Revision 1.1.1.1  2000/04/23 18:21:15  fisyak
New version from ALICE

Revision 1.5  2000/03/20 15:11:03  fca
Mods to make the code compile on HP

Revision 1.4  2000/01/18 16:12:08  morsch
Bug in calculation of number of volume divisions and number of positionings corrected
Browser for Material and Media properties added

Revision 1.3  1999/11/14 14:31:14  fca
Correct small error and remove compilation warnings on HP

Revision 1.2  1999/11/10 16:53:35  fca
The new geometry viewer from A.Morsch

*/

/* 
 *  Version: 0
 *  Written by Andreas Morsch
 *  
 * 
 *
 * For questions critics and suggestions to this part of the code
 * contact andreas.morsch@cern.ch
 * 
 **************************************************************************/
#include <stdlib.h>
#include <TROOT.h>
#if 0
#include <StarRun.h>
#endif
#include <StarMC.h>
#include <TGeant3.h>
#include <THIGZ.h>
#include <TApplication.h>
#include <TVirtualX.h>
#include <TGListBox.h>
#include <TGListTree.h>
#include <TGClient.h>
#include <TGFrame.h>
#include <TGIcon.h>
#include <TGLabel.h>
#include <TGButton.h>
#include <TGTextEntry.h>
#include <TGMsgBox.h>
#include <TGMenu.h>
#include <TGCanvas.h>
#include <TGComboBox.h>
#include <TGTab.h>
#include <TGSlider.h>
#include <TGDoubleSlider.h>
#include <TGFileDialog.h>
#include <TRootEmbeddedCanvas.h>
#include <TCanvas.h>
#include <TH1.h>
#include <TH2.h>
#include <TRandom.h>
#include <TSystem.h>
#include <TEnv.h>
#include <TGPicture.h>
#include <TGraph.h>

#include "TGeant3GUI.h"

static StarDrawVolume  *gCurrentVolume   = new StarDrawVolume("NULL");
static StarGUIMaterial *gCurrentMaterial = new StarGUIMaterial();
static StarGUIMedium   *gCurrentMedium   = new StarGUIMedium();
static Int_t           gCurrentParticle = 1;
static Int_t           gCurrentProcess  = 1;

ClassImp(StarGeant3GeometryGUI)

    StarGeant3GeometryGUI::StarGeant3GeometryGUI()
{
    fPanel  =   new StarGuiGeomMain(gClient->GetRoot(), 500, 500);
    fNstack = 0;
    fVolumes =   new TClonesArray("StarDrawVolume",1000);
    fMaterials = new TClonesArray("StarGUIMaterial",1000);
    fMedia =     new TClonesArray("StarGUIMedium",1000);
//  Store local copy of zebra bank entries
    TGeant3 *geant3 = (TGeant3*) gMC;
    if (geant3) {
	fZlq=geant3->Lq();
	fZq=geant3->Q();
	fZiq=geant3->Iq();
	fGclink=geant3->Gclink();
	fGcnum=geant3->Gcnum();
//
	ReadGeometryTree();
	ReadMaterials();
    }
}
void StarGeant3GeometryGUI::Streamer(TBuffer &)
{
;
}


void StarGeant3GeometryGUI::ReadGeometryTree()
{
//
// Copy zebra volume tree into ROOT LisTree
//
    char *vname;
    char /* *namec, */ *tmp;
    char namec[30];
//  Icons for 
//  Closed folder (=volume containing children)
    const TGPicture* Folder     = gClient->GetPicture("folder_t.xpm");
//  Open folder   (=volume containing children)
    const TGPicture* OpenFolder = gClient->GetPicture("ofolder_t.xpm");
//  Empty object
    const TGPicture* Document   = gClient->GetPicture("doc_t.xpm");

    StarDrawVolume  *volume;

    Int_t nst=0;
    Int_t nlevel=1;
    Int_t newlevel=nlevel;

    volume = new StarDrawVolume("ALIC");
    volume->SetIdVolume(((TGeant3*)gMC)->VolId("ALIC"));
    volume->SetIdCopy(0);
    volume->SetItem(NULL);
    (*fVolumes)[0]=volume;
//
//  Loop over volumes for which information has been collected
    while(nlevel>nst) {
	for (Int_t i=nst; i<nlevel; i++) 
	{
	    TGListTreeItem *itemi, *item2;
// GEANT3 volume number
	    Int_t ivol=TMath::Abs(Volume(i)->GetIdVolume());
// Copy number
// icopy=1 normal positioning
// icopy>1 positioning with parposp
// icopy<0 division
	    Int_t icopy = Volume(i)->GetIdCopy();
// Medium and material number, handle special case of divisions
	    Int_t imat, imed;
	    
	    if (icopy <0) {
		imed=Medium(-ivol);
		imat=Material(-ivol);
	    } else {
		imed=Medium(ivol);
		imat=Material(ivol);
	    }
//
// Number of children
	    Int_t nch = NChildren(ivol);
	    strcpy(namec,((TGeant3*)gMC)->VolName(ivol));
	    if (nch >= 0) {
		printf("\n %s has %d children  \n ", namec,  nch);
	    } else {
		printf("\n %s has  divisions \n ", namec);
	    }
//
// Name to be used in ListTree
	    vname = new char[5];
	    strncpy(vname,namec, 4);
	    vname[4]='\0';

	    if (icopy >1) {
		sprintf(namec,"%s*%3dPos",namec,icopy);
	    } else if (icopy <0) {
		sprintf(namec,"%s*%3dDiv",namec,-icopy);
	    }
	    if (i>0) {
		itemi=Volume(i)->GetItem();
	    } else {
		itemi=NULL;
	    }
	    
//
// Add volume to list tree
	    
	    if (nch!=0) {
		item2 = fPanel->AddItem(new StarDrawVolume(vname), 
				    itemi, namec, OpenFolder, Folder);
	    } else {
		item2 = fPanel->AddItem(new StarDrawVolume(vname), 
				    itemi, namec, Document, Document);
	    }
//
// Add medium information to list tree item
	    ((StarDrawVolume *) item2->GetUserData())->SetIdVolume(ivol);
	    ((StarDrawVolume *) item2->GetUserData())->SetIdMaterial(imat);
	    ((StarDrawVolume *) item2->GetUserData())->SetIdMedium(imed);
//
// Set current volume to first list tree entry
	    if (!i) gCurrentVolume= ((StarDrawVolume *) item2->GetUserData());
	    
//
// Collect children information
//
// nch < 0: Children by division	    
	    if (nch < 0) {
//
// Geant volume number
		Int_t icvol=Child(ivol,1);
// Name
		strcpy(namec,((TGeant3*)gMC)->VolName(-icvol));
		tmp = new char[4];
		strncpy(tmp,(char *) &namec, 4);
		volume = new StarDrawVolume(namec);
		volume->SetIdVolume(-icvol);
// Number of divisions
		Int_t jvo  = fZlq[fGclink->jvolum-ivol];
		Int_t jdiv = fZlq[jvo-1];
		Int_t ndiv = Int_t (fZq[jdiv+3]);
		volume->SetIdCopy(-ndiv);
// Link to mother
		volume->SetItem(item2);
		(*fVolumes)[newlevel]=volume;
		printf("\n volume %s %d %d %d", namec, icvol, nch, ndiv);
		newlevel++;
//
// Children by positioning
	    } else {
		Int_t nnew=0;
// Loop over children 
		for (Int_t j=0; j<nch; j++) 
		{
//
// Find out if this volume was already positioned and count copies 
		    Int_t icvol=Child(ivol,j+1);
		    icvol = TMath::Abs(icvol);
		    Bool_t inList=kFALSE;
		    for (Int_t k=0; k<nnew; k++) {
			if (icvol==
			    Volume(newlevel-k-1)->GetIdVolume()) 
			{
			    Volume(newlevel-k-1)->AddCopy();
			    inList=kTRUE;
			}
		    }
//
// New child
		    if (!inList) {
//
// Name
			strcpy(namec,((TGeant3*)gMC)->VolName(icvol));
			tmp = new char[4];
			strncpy(tmp,(char *) &namec, 4);
			volume = new StarDrawVolume(namec);
			volume->SetIdVolume(icvol);
			volume->SetIdCopy(1);
// Link to mother
			volume->SetItem(item2);
			(*fVolumes)[newlevel]=volume;
			printf("\n volume %s %d %d", namec, icvol, nch);
			newlevel++;
			nnew++;
		    }
		}
	    }
	}
// Move one level deaper
	nst=nlevel;
	nlevel=newlevel;
    }
}

void StarGeant3GeometryGUI::ReadMaterials()
{
//
// Puts media and material names into ComboBox and 
// collects material information
// 
    Float_t a, z, dens, radl, absl;
    a=z=dens=radl=absl=-1;
    Int_t npar=0;
    Int_t imat, isvol, ifield;
    Float_t fieldm, tmaxfd, stemax, deemax, epsil, stmin;
    Float_t par[50];
    Float_t ubuf[50];
    Int_t nwbuf;
    
    char natmed[21], namate[21];
//
// Loop over media
    Int_t NEntries=0;
    
    for(Int_t itm=1;itm<=fGcnum->ntmed;itm++) {
	Int_t jtm  = fZlq[fGclink->jtmed-itm];
	if (jtm > 0) {
	    NEntries++;
// 
// Get medium parameters
	    ((TGeant3*)(gMC))->Gftmed(itm, natmed, imat, isvol, ifield, fieldm, 
				      tmaxfd, stemax, deemax, epsil, stmin, ubuf, &nwbuf);
	    strncpy(natmed,(char*)&fZiq[jtm+1],20);
	    natmed[20]='\0';
//
// Create new medium object 
	    StarGUIMedium * medium = 
		new StarGUIMedium(itm, imat, natmed, isvol, ifield, fieldm, 
				      tmaxfd, stemax, deemax, epsil, stmin);
	    (*fMedia)[NEntries-1]=medium;
        { //Begin local scope for j
          for (Int_t j=1; j<=22; j++) {
            medium->SetPar(j,Cut(itm,j));
          }
        } //End local scope for j
        { //Begin local scope for j
          for (Int_t j=23; j<=26; j++) {
            medium->SetPar(j,Cut(itm,j+3));
          }
        } //End local scope for j
        { //Begin local scope for j
          for (Int_t j=27; j<=29; j++) {
            medium->SetPar(j,Cut(itm,j+4));
          }
        } //End local scope for j
//
// Add to ComboBox
	    fPanel->AddMedium(medium, NEntries);
//
// Associated material
	    imat =  Int_t (fZq[jtm+6]);
	    Int_t jma  =  Int_t (fZlq[fGclink->jmate-imat]);
//
// Get material parameters
	    ((TGeant3*)(gMC))->Gfmate (imat,namate,a,z,dens,radl,absl,par,npar);
	    strncpy(namate,(char *)&fZiq[jma+1],20);
	    namate[20]='\0';
//
// Create new material object
	    StarGUIMaterial * material = 
		new StarGUIMaterial(imat,namate,a,z,dens,radl,absl);
	    (*fMaterials)[NEntries-1]=material;
	    material->Dump();
//
// Add to combo box
	    fPanel->AddMaterial(material, NEntries);
	    gCurrentMaterial=material;
	}
    }
    fPanel->SetComboEntries(fMaterials);
    fPanel->SetMediaComboEntries(fMedia);
    fPanel->Update();
}

Int_t StarGeant3GeometryGUI::NChildren(Int_t idvol)
{
//
// Return number of children for volume idvol
    Int_t jvo = fZlq[fGclink->jvolum-idvol];
    Int_t nin = Int_t(fZq[jvo+3]);
    return nin;
}

Int_t StarGeant3GeometryGUI::Child(Int_t idvol, Int_t idc)
{
//
// Return GEANT id of child number idc of volume idvol
    Int_t jvo = fZlq[fGclink->jvolum-idvol];
    Int_t nin=idc;
    Int_t jin = fZlq[jvo-nin];
    Int_t numb =  Int_t (fZq[jin +3]);
    if (numb > 1) {
	return -Int_t(fZq[jin+2]);
    } else {
	return Int_t(fZq[jin+2]);
    }
}

Int_t StarGeant3GeometryGUI::Medium(Int_t idvol)
{
//
// Return medium number for volume idvol.
// If idvol is negative the volume results from a division.
    Int_t imed;
    if (idvol > 0) {
	Int_t jvo = fZlq[fGclink->jvolum-idvol];
	imed = Int_t(fZq[jvo+4]);
    } else {
	idvol=-idvol;
	Int_t jdiv = fZlq[fGclink->jvolum-idvol];
	Int_t ivin = Int_t ( fZq[jdiv+2]);
	Int_t jvin = fZlq[fGclink->jvolum-ivin];
	imed = Int_t (fZq[jvin+4]);
    }
    return imed;
}

Int_t StarGeant3GeometryGUI::Material(Int_t idvol)
{
// Return material number for volume idvol.
// If idvol is negative the volume results from a division.

    Int_t imed=Medium(idvol);
    Int_t jtm  = fZlq[fGclink->jtmed-imed];
    return Int_t (fZq[jtm+6]);
}


Float_t StarGeant3GeometryGUI::Cut(Int_t imed, Int_t icut)
{
// Return cuts icut for medium idmed 
// 
    Int_t jtm  = fZlq[fGclink->jtmed-imed];
//
//  Have the defaults been modified ??
    Int_t jtmn = fZlq[jtm];
    if (jtmn >0) {
	jtm=jtmn;
	
    } else {
	jtm=fGclink->jtmed;
    }
    
    return Float_t (fZq[jtm+icut]);
}

ClassImp(StarDrawVolume)
//
// Drawing parameter tags
enum StarDrawParamId {
   P_Theta,
   P_Phi,
   P_Psi,
   P_U,
   P_V,
   P_Uscale,
   P_Vscale,
   P_Shadow,
   P_Hide,
   P_Fill,
   P_Seen,
   P_Clip,
   P_ClipXmin,
   P_ClipXmax,
   P_ClipYmin,
   P_ClipYmax,
   P_ClipZmin,
   P_ClipZmax
};


StarDrawVolume::StarDrawVolume(char* name)
{
    fName   = name;
    fTheta  = 30;
    fPhi    = 30;
    fPsi    = 0;
    fU      = 10;
    fV      = 10;
    fUscale = 0.01;
    fVscale = 0.01;
    fHide=0;
    fShadow=0;
    fFill=1;
    fSeen=1;
    fClip=0;
    fClipXmin=0.;
    fClipXmax=2000.;
    fClipYmin=0.;
    fClipYmax=2000.;
    fClipZmin=0.;
    fClipZmax=2000.;
}

char* StarDrawVolume::Name()
{
//
// Return volume name
    return fName;
}

    
void StarDrawVolume::Streamer(TBuffer &)
{
;
}



void StarDrawVolume::Draw(Option_t *)
{
    gMC->Gsatt(fName,"seen", fSeen);
    
    if (fHide) {
	gMC->Gdopt("hide", "on");
    } else {
	gMC->Gdopt("hide", "off");
    }

    if (fShadow) {
	gMC->Gdopt("shad", "on");
	gMC->Gsatt("*", "fill", fFill);
    } else {
	gMC->Gdopt("shad", "off");
    }

    	gMC->SetClipBox(".");
    if (fClip) {
	gMC->SetClipBox("*", fClipXmin, fClipXmax, 
			fClipYmin, fClipYmax, fClipZmin, fClipZmax);
    } else {
	gMC->SetClipBox(".");
    }
    

    gMC->Gdraw(fName, fTheta, fPhi, fPsi, fU, fV, fUscale, fVscale);
    THIGZ *higz = (THIGZ*)gROOT->GetListOfCanvases()->FindObject("higz");
    if (higz) higz->Update();
}

void StarDrawVolume::DrawSpec()
{
    gMC->Gsatt(fName,"seen", fSeen);
    
    if (fHide) {
	gMC->Gdopt("hide", "on");
    } else {
	gMC->Gdopt("hide", "off");
    }

    if (fShadow) {
	gMC->Gdopt("shad", "on");
	gMC->Gsatt("*", "fill", fFill);
    } else {
	gMC->Gdopt("shad", "off");
    }

    gMC->SetClipBox(".");
    if (fClip) {
	gMC->SetClipBox("*", fClipXmin, fClipXmax, fClipYmin, fClipYmax, fClipZmin, fClipZmax);
    } else {
	gMC->SetClipBox(".");
    }
    

    ((TGeant3*) gMC)->DrawOneSpec(fName);
    THIGZ *higz = (THIGZ*)gROOT->GetListOfCanvases()->FindObject("higz");
    if (higz) higz->Update();
}

void StarDrawVolume::SetParam(Int_t ip, Float_t param)
{
    switch (ip) {
    case P_Theta:
	fTheta=param;
	break;
    case P_Phi:
	fPhi=param;
	break;
    case P_Psi:
	fPsi=param;
	break;
    case P_U:
	fU=param;
	break;
    case P_V:
	fV=param;
	break;
    case P_Uscale:
	fUscale=param;
	break;
    case P_Vscale:
	fVscale=param;
	break;
    case P_Hide:
	fHide=Int_t(param);
	break;
    case P_Shadow:
	fShadow=Int_t(param);
	break;
    case P_Fill:
	fFill=Int_t(param);
	break;
    case P_Seen:
	fSeen=Int_t(param);
	break;
    case P_Clip:
	fClip=Int_t(param);
	break;
    case P_ClipXmin:
	fClipXmin=param;
	break;
    case P_ClipXmax:
	fClipXmax=param;
	break;
    case P_ClipYmin:
	fClipYmin=param;
	break;
    case P_ClipYmax:
	fClipYmax=param;
	break;
    case P_ClipZmin:
	fClipZmin=param;
	break;
    case P_ClipZmax:
	fClipZmax=param;
	break;
    }
}

Float_t  StarDrawVolume::GetParam(Int_t ip)
{
    switch (ip) {
    case P_Theta:
	return fTheta;
    case P_Phi:
	return fPhi;
    case P_Psi:
	return fPsi;
    case P_U:
	return fU;
    case P_V:
	return fV;
    case P_Uscale:
	return fUscale;
    case P_Vscale:
	return fVscale;
    case P_Hide:
	return Float_t(fHide);
    case P_Shadow:
	return Float_t(fShadow);
    case P_Fill:
	return Float_t(fFill);
    case P_Seen:
	return Float_t(fSeen);
    case P_Clip:
	return Float_t(fClip);
    case P_ClipXmin:
	return fClipXmin;
    case P_ClipXmax:
	return fClipXmax;
    case P_ClipYmin:
	return fClipYmin;
    case P_ClipYmax:
	return fClipYmax;
    case P_ClipZmin:
	return fClipZmin;
    case P_ClipZmax:
	return fClipZmax;
    default:
	return 0.;
    }
    return 0.;
}


ClassImp(StarGuiGeomMain)

 const Text_t* LabelTextP[19]  = 
{"PAIR  ", "COMP  ", "PHOT  ", "PFIS  ", "DRAY  ", "ANNI  ", "BREM  ", 
 "HADR  ", "MUNU  ", "DCAY  ", "LOSS  ", "MULS  ", "GHCOR1", "BIRK1 ", 
 "BIRK2 ", "BIRK3 ", "LABS  ", "SYNC  ", "STRA  "};


 const Text_t* LabelTextC[10]  = 
 {"CUTGAM", "CUTELE", "CUTNEU", "CUTHAD", "CUTMUO", "BCUTE", "BCUTM",
  "DCUTE ", "DCUTM ", "PPCUTM"};

const Text_t* LabelTextPart[24]  = 
{"Photon", "Positron", "Electron", "Neutrino", "Muon+", "Muon-", 
 "Pi0", "Pi+", "Pi-", "Kaon_L", "Kaon+", "Kaon-", "Neutron", "Proton", 
 "Anti Proton", "Kaon_S", "Eta", "Lambda", "Sigma+", "Sigma0", "Sigma-",
 "Xi0", "Xi-", "Omega-"};

const Text_t* LabelTextMechanism[24]  = 
{"HADF", "INEF", "ELAF", "FISF", "CAPF",
 "HADG", "INEG", "ELAG", "FISG", "CAPG",
 "LOSS", "PHOT", "ANNI", "COMP", "BREM",
 "PAIR", "DRAY", "PFIS", "RAYL", "HADG",
 "MUNU", "RANG", "STEP", "MUON"};




enum ETestCommandIdentifiers {
   M_FILE_OPEN,
   M_FILE_SAVE,
   M_FILE_SAVEAS,
   M_FILE_EXIT,

   M_TEST_DLG,

   M_HELP_CONTENTS,
   M_HELP_SEARCH,
   M_HELP_ABOUT,


   VId1,
   HId1,
   VId2,
   HId2,

   VSId1,
   HSId1,
   VSId2,
   HSId2
};


Int_t mb_button_id[9] = { kMBYes, kMBNo, kMBOk, kMBApply,
                          kMBRetry, kMBIgnore, kMBCancel,
                          kMBClose, kMBDismiss };

EMsgBoxIcon mb_icon[4] = { kMBIconStop, kMBIconQuestion,
                           kMBIconExclamation, kMBIconAsterisk };

const char *filetypes[] = { "All files",     "*",
                            "ROOT files",    "*.root",
                            "ROOT macros",   "*.C",
                            0,               0 };




TGListTreeItem*  StarGuiGeomMain::
AddItem(TObject * obj, TGListTreeItem *parent, const char* name, const TGPicture *open, const TGPicture *closed)
{
    return fLt->AddItem(parent, name, obj, open, closed);
}

StarGuiGeomMain::StarGuiGeomMain(const TGWindow *p, UInt_t w, UInt_t h)
      : TGMainFrame(p, w, h)
{
    fDialog=0;
   // Create test main frame. A TGMainFrame is a top level window.
   // Create menubar and popup menus. The hint objects are used to place
   // and group the different menu widgets with respect to eachother.
   fMenuBarLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                                      0, 0, 1, 1);
   fMenuBarItemLayout = new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0);
   fMenuBarHelpLayout = new TGLayoutHints(kLHintsTop | kLHintsRight);

   fMenuFile = new TGPopupMenu(gClient->GetRoot());
   fMenuFile->AddEntry("&Open...", M_FILE_OPEN);
   fMenuFile->AddEntry("&Save", M_FILE_SAVE);
   fMenuFile->AddEntry("S&ave as...", M_FILE_SAVEAS);
   fMenuFile->AddEntry("&Close", -1);
   fMenuFile->AddSeparator();
   fMenuFile->AddEntry("E&xit", M_FILE_EXIT);

   fMenuFile->DisableEntry(M_FILE_SAVEAS);
   fMenuFile->DisableEntry(M_FILE_OPEN);
   fMenuFile->DisableEntry(M_FILE_SAVE);



   fMenuTest = new TGPopupMenu(this);
   fMenuTest->AddLabel("Draw");
   fMenuTest->AddSeparator();
   fMenuTest->AddEntry("&Volume Draw Control", M_TEST_DLG);
   fMenuTest->AddSeparator();
   fMenuTest->Associate(this);
   
   
   fMenuHelp = new TGPopupMenu(gClient->GetRoot());
   fMenuHelp->AddEntry("&Contents", M_HELP_CONTENTS);
   fMenuHelp->AddEntry("&Search...", M_HELP_SEARCH);
   fMenuHelp->AddSeparator();
   fMenuHelp->AddEntry("&About", M_HELP_ABOUT);

   fMenuFile->DisableEntry(M_HELP_CONTENTS);
   fMenuFile->DisableEntry(M_HELP_SEARCH);
   fMenuFile->DisableEntry(M_HELP_ABOUT);
   // Menu button messages are handled by the main frame (i.e. "this")
   // ProcessMessage() method.
   fMenuFile->Associate(this);
   fMenuTest->Associate(this);
   fMenuHelp->Associate(this);

   fMenuBar = new TGMenuBar(this, 1, 1, kHorizontalFrame);
   fMenuBar->AddPopup("&File", fMenuFile, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Draw Control", fMenuTest, fMenuBarItemLayout);
   fMenuBar->AddPopup("&Help", fMenuHelp, fMenuBarHelpLayout);

   AddFrame(fMenuBar, fMenuBarLayout);

// 
// Volumes
//
   fTab = new TGTab(this, 400, 400);
   TGCompositeFrame *tf = fTab->AddTab("Volumes");
   TGLayoutHints *fLTab = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                          kLHintsExpandY, 2, 2, 5, 1);
   AddFrame(fTab, fLTab);

// Create TGCanvas and a canvas container which uses a tile layout manager
   fCanvasWindow = new TGCanvas(tf, 400, 240);
// Create TreeList
   fLt = new TGListTree(fCanvasWindow->GetViewPort(), 10, 10, kHorizontalFrame,
                        fgWhitePixel);
   fLt->Associate(this);
   fCanvasWindow->SetContainer(fLt);

    
   TGLayoutHints *lo = new TGLayoutHints(kLHintsExpandX | kLHintsExpandY);
   tf->AddFrame(fCanvasWindow, lo);
//
// Materials
//
   tf = fTab->AddTab("Materials");
   fF2 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fL2 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
// ComboBox for materials
   fMaterialCombo = new TGComboBox(fF2, 1);
   fF2->AddFrame(fMaterialCombo, fL2);
// 
// text labels with material properties 
//
   Text_t* LabelText[6]  = 
   {"Material Number  ", 
    "Atomic Weight    ",
    "Atomic Number    ", 
    "Density          ",
    "Radiation Length ", 
    "Absorption Length"};

   TGLayoutHints* Bly   = 
       new TGLayoutHints(kLHintsTop | kLHintsExpandY, 5, 5, 5, 5);
   TGLayoutHints* Bfly1 = 
       new TGLayoutHints(kLHintsLeft | kLHintsExpandX );
   fF21 = new TGCompositeFrame(fF2, 60, 20, kVerticalFrame);
   fF2->AddFrame(fF21,fL2);
   for (Int_t i=0; i<6; i++) {
       Int_t idT=i+1;       
       fHframe[i] = new TGHorizontalFrame(fF21, 400, 100, kFixedWidth);
       fF21->AddFrame(fHframe[i], Bly);
       fTbh[i] = new TGTextBuffer(10);
       fTeh[i] = new TGTextEntry(fHframe[i], fTbh[i],idT);
       fTeh[i]->Associate(this);
       fLabel[i] = new TGLabel(fHframe[i], LabelText[i]);
       fHframe[i]->AddFrame(fLabel[i], Bfly1);
       fHframe[i]->AddFrame(fTeh[i], Bfly1);
   }
  tf->AddFrame(fF2, fL2);
  fMaterialCombo->Resize(200, 20);
  fMaterialCombo->Associate(this);


// Media Combo
//   
   tf = fTab->AddTab("Media");
   fF3 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
// ComboBox for tracking media
   fMediaCombo = new TGComboBox(fF3, 2);
   fF3->AddFrame(fMediaCombo, fL2);
// 
// text labels with material properties 
//
   Text_t* LabelTextM[8]  = 
   {"Sensitivity Flag      ", 
    "Magnetic Field Flag   ",
    "Maximum Field         ", 
    "Max. Ang. Deviation   ",
    "Maximum Step          ", 
    "Max. Frac. Energy Loss",
    "Crossing Precission   ",
    "Minimum Step Size     "};

   fF31 = new TGCompositeFrame(fF3, 60, 20, kVerticalFrame);
   fF3->AddFrame(fF31,fL2);
   { //Begin local scope for i
     for (Int_t i=0; i<8; i++) {
       Int_t idT=i+1;       
       fHframeM[i] = new TGHorizontalFrame(fF31, 400, 100, kFixedWidth);
       fF31->AddFrame(fHframeM[i], Bly);
       fTbhM[i] = new TGTextBuffer(10);
       fTehM[i] = new TGTextEntry(fHframeM[i], fTbhM[i],idT);
       fTehM[i]->Associate(this);
       fLabelM[i] = new TGLabel(fHframeM[i], LabelTextM[i]);
       fHframeM[i]->AddFrame(fLabelM[i], Bfly1);
       fHframeM[i]->AddFrame(fTehM[i], Bfly1);
     }
   } //End local scope for i
  tf->AddFrame(fF3, fL2);
  fMediaCombo->Resize(200, 20);
  fMediaCombo->Associate(this);
//
// Processes
   tf = fTab->AddTab("Processes");
   fF4 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fProcessLB = new TGListBox(fF4, 1);
   fF4->AddFrame(fProcessLB, fL2);
   tf->AddFrame(fF4, fL2);
   fProcessLB->Resize(150, 350);
   fProcessLB->Associate(this);
   
//
// Cuts
   tf = fTab->AddTab("Cuts");
   fF5 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);
   fCutsLB = new TGListBox(fF5, 1);
   fF5->AddFrame(fCutsLB, fL2);
   tf->AddFrame(fF5, fL2);
   fCutsLB->Resize(150, 350);

//
// de/dx and cross-sections
   tf = fTab->AddTab("DE/DX and X-Sections");
   fF6 = new TGCompositeFrame(tf, 60, 20, kHorizontalFrame);

// ComboBox for particles
   fF61 = new TGCompositeFrame(fF6, 60, 20, kVerticalFrame);
   fF6->AddFrame(fF61, fL2);
   fParticleCombo = new TGComboBox(fF61, 3);
   fF61->AddFrame(fParticleCombo, fL2);
   { //Begin local scope for i
     for (Int_t i = 0; i < 24; i++) {
       char tmp[20];
       sprintf(tmp, "%s", LabelTextPart[i]);
       fParticleCombo->AddEntry(tmp, i+1);
     }
   } //End local scope for i
   fParticleCombo->Select(1);
   fParticleCombo->Resize(100, 20);
   fParticleCombo->Associate(this);

// ComboBox for mechanisms
   fF63 = new TGCompositeFrame(fF6, 60, 20, kVerticalFrame);
   fF6->AddFrame(fF63, fL2);
   fMechanismCombo = new TGComboBox(fF63, 4);
   fF63->AddFrame(fMechanismCombo, fL2);
   { //Begin local scope for i
     for (Int_t i = 0; i < 24; i++) {
       char tmp[20];
       sprintf(tmp, "%s", LabelTextMechanism[i]);
       fMechanismCombo->AddEntry(tmp, i+1);
     }
   } //End local scope for i
   fMechanismCombo->Select(1);
   fMechanismCombo->Resize(100, 20);
   fMechanismCombo->Associate(this);

//
// Energy Range
//
   fTbh61 = new TGTextBuffer(10);
   fTeh61 = new TGTextEntry(fF61, fTbh61,10);
   fTbh61->AddText(0, "  100");
   fTeh61->Associate(this);

   fTbh62 = new TGTextBuffer(10);
   fTeh62 = new TGTextEntry(fF61, fTbh62,11);
   fTbh62->AddText(0, "0.001");
   fTeh62->Associate(this);
    
   fTbh63 = new TGTextBuffer(10);
   fTeh63 = new TGTextEntry(fF61, fTbh63,12);
   fTbh63->AddText(0, "10.");
   fTeh63->Associate(this);

   fEmin=0.001;
   fEmax=10.;
   fNbins=100;
   
   fSLabel61 = new TGLabel(fF61, "Nbins-Emin-Emax");
   Bfly1 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX );
   fF61->AddFrame(fSLabel61, Bfly1);
   fF61->AddFrame(fTeh61, Bfly1);
   fF61->AddFrame(fTeh62, Bfly1);
   fF61->AddFrame(fTeh63, Bfly1);
//
// Plot Button
   fF62 = new TGCompositeFrame(fF6, 60, 20, kHorizontalFrame);
   fF6->AddFrame(fF62, fL2);
   fPlotButton = new TGTextButton(fF62, "Plot", 1);
   fPlotButton -> Associate(this);
   fF62->AddFrame(fPlotButton);

   tf->AddFrame(fF6, fL2);
// Window name and final mapping
//
   SetWindowName("StarRoot Geometry Browser");
   MapSubwindows();
   // We need to use GetDefault...() to initialize the layout algorithm...
   Resize(GetDefaultSize());
   MapWindow();
}

StarGuiGeomMain::~StarGuiGeomMain()
{
   // Delete all created widgets.

   delete fContainer;
   delete fCanvasWindow;

   delete fMenuBarLayout;
   delete fMenuBarItemLayout;
   delete fMenuBarHelpLayout;

   delete fMenuFile;
   delete fMenuTest;
   delete fMenuHelp;
}

void StarGuiGeomMain::Streamer(TBuffer &)
{
;
}

void StarGuiGeomMain::Plot()
{
    const Float_t avo=0.60221367;
    Float_t *tkin  = new Float_t[fNbins];
    Float_t *value = new Float_t[fNbins];
    Float_t *pcut  = new Float_t[fNbins];
    Int_t ixst;
    Int_t imate=gCurrentMaterial->Id();
    Float_t z=gCurrentMaterial->Z();
    Float_t a=gCurrentMaterial->A();
    Float_t density=gCurrentMaterial->Density();
    
    Int_t ipart=gCurrentParticle;
    const char *chmeca= LabelTextMechanism[gCurrentProcess-1];
    char* tmp;
    tmp = new char[5];
    strncpy(tmp, chmeca, 4);
    tmp[4]='\0';
    Int_t kdim=fNbins;
    Float_t de=(fEmax-fEmin)/fNbins;
    for (Int_t i=0; i<kdim; i++) {
	tkin[i]=fEmin+Float_t(i)*de;
	value[i]=0.;
    }
    if (chmeca!="MUON") {
	((TGeant3*) gMC)->Gftmat(imate, ipart, tmp, kdim, tkin, value, pcut, ixst);
    } else {
	for (Int_t i=0; i<kdim; i++) {
	    Float_t ekin=tkin[i];
	    value[i]+=((TGeant3*) gMC)->Gbrelm(z,ekin,1.e10);
	    value[i]+=((TGeant3*) gMC)->Gprelm(z,ekin,1.e10);
	    value[i]*=1000.*avo*density/a;
	}
    }
    
    
    printf("\n %d %d %s %d \n",imate, ipart, chmeca, kdim);
    if (ixst) {
	TGraph *graph = new TGraph(kdim,tkin,value);
	TCanvas *c1 = new TCanvas("c1"," ",400,10,600,700);
	c1->Divide(1,1);
	c1->cd(1);
	
	graph->SetFillColor(42);
	graph->SetMarkerColor(4);
	graph->SetMarkerStyle(21);
	graph->Draw("AC");
	graph->GetHistogram()->SetXTitle("Energy (GeV)");
	if (chmeca == "RANG" || chmeca == "STEP") {
	    graph->GetHistogram()->SetYTitle
		("Distance (cm)");   
	} else if (chmeca == "LOSS" || chmeca == "MUON") {
	    graph->GetHistogram()->SetYTitle("dE/dx (MeV/cm)");   
	} else {
	    graph->GetHistogram()->SetYTitle
		("Macroscopic X-Section (1/cm)"); 
	}
    }
    
    delete tkin;
    delete value;
    delete pcut;
    
    
}

void StarGuiGeomMain::Update()
{
    if (fDialog) {
	fDialog->Update();
    }

    Int_t imat=gCurrentVolume->Material();
    Int_t nmat=fComboEntries->GetEntriesFast();
    Int_t i=0;
    for (i=0; i<nmat; i++) {
	gCurrentMaterial = (StarGUIMaterial*) 
	    (fComboEntries->UncheckedAt(i));
	if (gCurrentMaterial->Id()==imat) break;
    }
    gCurrentMedium   = (StarGUIMedium*) 
	(fComboMediaEntries->UncheckedAt(i));
    UpdateCombo();
    UpdateListBox();
    
}

void StarGuiGeomMain::UpdateCombo()
{

    Int_t   imat = gCurrentMaterial->Id();
    Float_t    a = gCurrentMaterial->A();
    Float_t    z = gCurrentMaterial->Z();    
    Float_t dens = gCurrentMaterial->Density();
    Float_t radl = gCurrentMaterial->RadiationLength();
    Float_t absl = gCurrentMaterial->AbsorptionLength();
    Int_t entry=gCurrentMaterial->ItemId();
//    
//
    fMaterialCombo->Select(entry);
    fMediaCombo->Select(entry);    
    char buf[10];
     
    sprintf(buf, "%10i", imat);
    fTbh[0]->Clear();
    fTbh[0]->AddText(0, buf);
    gClient->NeedRedraw(fTeh[0]);
    sprintf(buf, "%10.2e", a);
    fTbh[1]->Clear();
    fTbh[1]->AddText(0, buf);
    gClient->NeedRedraw(fTeh[1]);

    sprintf(buf, "%10.2e", z);
    fTbh[2]->Clear();
    fTbh[2]->AddText(0, buf);
    gClient->NeedRedraw(fTeh[2]);

    sprintf(buf, "%10.2e", dens);
    fTbh[3]->Clear();
    fTbh[3]->AddText(0, buf);
    gClient->NeedRedraw(fTeh[3]);

    sprintf(buf, "%10.2e", radl);
    fTbh[4]->Clear();
    fTbh[4]->AddText(0, buf);
    gClient->NeedRedraw(fTeh[4]);

    sprintf(buf, "%10.2e", absl);
    fTbh[5]->Clear();
    fTbh[5]->AddText(0, buf);
    gClient->NeedRedraw(fTeh[5]);

//  Media Combo
    sprintf(buf, "%10i", gCurrentMedium->Isvol());
    fTbhM[0]->Clear();
    fTbhM[0]->AddText(0, buf);
    gClient->NeedRedraw(fTehM[0]);


    sprintf(buf, "%10i", gCurrentMedium->Ifield());
    fTbhM[1]->Clear();
    fTbhM[1]->AddText(0, buf);
    gClient->NeedRedraw(fTehM[1]);

    sprintf(buf, "%10.2e", gCurrentMedium->Fieldm());
    fTbhM[2]->Clear();
    fTbhM[2]->AddText(0, buf);
    gClient->NeedRedraw(fTehM[2]);

    sprintf(buf, "%10.2e", gCurrentMedium->Tmaxfd());
    fTbhM[3]->Clear();
    fTbhM[3]->AddText(0, buf);
    gClient->NeedRedraw(fTehM[3]);

    sprintf(buf, "%10.2e", gCurrentMedium->Stemax());
    fTbhM[4]->Clear();
    fTbhM[4]->AddText(0, buf);
    gClient->NeedRedraw(fTehM[4]);

    sprintf(buf, "%10.2e", gCurrentMedium->Deemax());
    fTbhM[5]->Clear();
    fTbhM[5]->AddText(0, buf);
    gClient->NeedRedraw(fTehM[5]);

    sprintf(buf, "%10.2e", gCurrentMedium->Epsil());
    fTbhM[6]->Clear();
    fTbhM[6]->AddText(0, buf);
    gClient->NeedRedraw(fTehM[6]);

    sprintf(buf, "%10.2e", gCurrentMedium->Stmin());
    fTbhM[7]->Clear();
    fTbhM[7]->AddText(0, buf);
    gClient->NeedRedraw(fTehM[7]);
}

void StarGuiGeomMain::UpdateListBox()
{
    Int_t i;
    fProcessLB->RemoveEntries(1,19);
    for (i=11; i < 30; i++) {
	Float_t p=gCurrentMedium->GetPar(i);
	char tmp[20];
	sprintf(tmp, "%6s%5d", LabelTextP[i-11], Int_t(p));
	fProcessLB->AddEntry(tmp, i-10);
    }
    fProcessLB->MapSubwindows();
    fProcessLB->Layout();

    fCutsLB->RemoveEntries(1,10);
    for (i=1; i < 11; i++) {
	Float_t p=gCurrentMedium->GetPar(i);
	char tmp[20];
	sprintf(tmp, "%6s%10.3e", LabelTextC[i-1], p);
	fCutsLB->AddEntry(tmp,i);
    }
    fCutsLB->MapSubwindows();
    fCutsLB->Layout();
}


void StarGuiGeomMain::CloseWindow()
{
   // Got close message for this MainFrame. Calls parent CloseWindow()
   // (which destroys the window) and terminate the application.
   // The close message is generated by the window manager when its close
   // window menu item is selected.

   TGMainFrame::CloseWindow();
   gApplication->Terminate(0);
}

Bool_t StarGuiGeomMain::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
    switch (GET_MSG(msg)) {
//
//  Text entries for binning of cross-section plots
    case kC_TEXTENTRY:
	switch (GET_SUBMSG(msg)) {
	case kTE_TEXTCHANGED:
	    switch (parm1) {
	    case 10:
		fNbins=(Int_t) atof(fTbh61->GetString());
		break;
	    case 11:
		fEmin= atof(fTbh62->GetString());
		break;
	    case 12:
		fEmax= atof(fTbh63->GetString());
		break;
	    }
	    printf("\n %d %f %f binning", fNbins, fEmin, fEmax);
	}
	break;
//
// ListTree for volumes
    case kC_LISTTREE:
	switch (GET_SUBMSG(msg)) {
//
// Handle mouse click 
	case kCT_ITEMCLICK:
	    TGListTreeItem *item;
//
// Button 1: Select volume
	    if (parm1 == kButton1) {
		if ((item = fLt->GetSelected())) 
		{
		    gCurrentVolume=((StarDrawVolume *) item->GetUserData());
		    Update();
		}
	    }
//
// Button 2: Draw volume specifications

	    if (parm1 == kButton2) {
		TGListTreeItem *item;
		if ((item = fLt->GetSelected())) 
		{

		    ((StarDrawVolume *) item->GetUserData())->DrawSpec();

		    gCurrentVolume=((StarDrawVolume *) item->GetUserData());
		    Update();
		}
	    }
//
// Button 3: Draw Volume
	    if (parm1 == kButton3) {
		TGListTreeItem *item;
		if ((item = fLt->GetSelected())) 
		{
		    ((StarDrawVolume *) item->GetUserData())->Draw();
		    gCurrentVolume=((StarDrawVolume *) item->GetUserData());
		    Update();
		}
	    }
	    
	    
	    break;
	case kCT_ITEMDBLCLICK:
	    if (parm1 == kButton1) {
		if (fLt->GetSelected() != 0) {
		    gClient->NeedRedraw(fLt);
		}
	    }
	    break;
	default:
	    break;
	}
	break;
    case kC_COMMAND:
	switch (GET_SUBMSG(msg)) {
	case kCM_BUTTON:
	    switch(parm1) {
	    case 1:
		Plot();
		break;
	    }
	    break;
	case kCM_COMBOBOX:
//
// Combo box bindings
	    switch(parm1) {
//
// Material Combo
	    case 1:
		gCurrentMaterial=(StarGUIMaterial*) 
		    (fComboEntries->UncheckedAt(Int_t(parm2-1)));
		gCurrentMedium=(StarGUIMedium*) 
		    (fComboMediaEntries->UncheckedAt(Int_t(parm2-1)));
		UpdateCombo();
		UpdateListBox();
		break;
//
// Media Combo
	    case 2:
		gCurrentMedium=(StarGUIMedium*) 
		    (fComboMediaEntries->UncheckedAt(Int_t(parm2-1)));
		gCurrentMaterial=(StarGUIMaterial*) 
		    (fComboEntries->UncheckedAt(Int_t(parm2-1)));
		UpdateCombo();
		UpdateListBox();
		break;
//
// Particle Combo
	    case 3:
		gCurrentParticle=Int_t(parm2);
		break;
//
// Mechanism Combo
	    case 4:
		gCurrentProcess=Int_t(parm2);
		printf("\n Process %d", gCurrentProcess);
		break;
	    }
	    break;
	case kCM_MENUSELECT:
	    break;
	    
	case kCM_MENU:
	    switch (parm1) {
		
	    case M_FILE_OPEN:
	    {
		TGFileInfo fi;
		fi.fFileTypes = (char **)filetypes;
		new TGFileDialog(gClient->GetRoot(), this, kFDOpen,&fi);
	    }
	    break;
	    
	    case M_TEST_DLG:
		fDialog = new StarGuiGeomDialog
		    (gClient->GetRoot(), this, 400, 200);
		break;
		
	    case M_FILE_SAVE:
		printf("M_FILE_SAVE\n");
		break;
		
	    case M_FILE_EXIT:
		CloseWindow();   // this also terminates theApp
		break;
	    default:
		break;
	    }
	default:
	    break;
	}
    default:
	break;
    }
    return kTRUE;
}

void StarGuiGeomMain::AddMaterial(StarGUIMaterial *Material, Int_t i)
{
    char* tmp;
    tmp=Material->Name();
    Material->SetItemId(i);
    fMaterialCombo->AddEntry(tmp, i);
    fMaterialCombo->Select(i);
    fMaterialCombo->Resize(200, 20);
}

void StarGuiGeomMain::AddMedium(StarGUIMedium *Medium, Int_t i)
{
    char* tmp;
    tmp=Medium->Name();
    Medium->SetItemId(i);
    
    fMediaCombo->AddEntry(tmp, i);
    fMediaCombo->Select(i);
    fMediaCombo->Resize(200, 20);
}


StarGuiGeomDialog::StarGuiGeomDialog(const TGWindow *p, const TGWindow *main, UInt_t w,
                       UInt_t h, UInt_t options)
    : TGTransientFrame(p, main, w, h, options)
{
   // Create a dialog window. A dialog window pops up with respect to its
   // "main" window.

   fFrame1 = new TGHorizontalFrame(this, 60, 20, kFixedWidth);

   fOkButton = new TGTextButton(fFrame1, "&Ok", 1);
   fOkButton->Associate(this);
   fCancelButton = new TGTextButton(fFrame1, "&Cancel", 2);
   fCancelButton->Associate(this);

   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                           2, 2, 2, 2);
   fL2 = new TGLayoutHints(kLHintsBottom | kLHintsRight, 2, 2, 5, 1);

   fFrame1->AddFrame(fOkButton, fL1);
   fFrame1->AddFrame(fCancelButton, fL1); 

   fFrame1->Resize(150, fOkButton->GetDefaultHeight());
   AddFrame(fFrame1, fL2);

   //--------- create Tab widget and some composite frames for Tab testing

   fTab = new TGTab(this, 300, 300);
   fL3 = new TGLayoutHints(kLHintsTop | kLHintsLeft, 5, 5, 5, 5);
//
// Tab1: Sliders
//
   TGCompositeFrame *tf = fTab->AddTab("Draw");
   fF1 = new StarGUISliders(tf, this, 60, 20);
   tf->AddFrame(fF1,fL3);
   
// 
// Tab2: Drawing Options
//
   tf = fTab->AddTab("Options");
   fL1 = new TGLayoutHints(kLHintsTop | kLHintsLeft | kLHintsExpandX,
                           200, 2, 2, 2);
   fF2 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);

   fF2->AddFrame(fChk1 = new TGCheckButton(fF2, "Shadow", 86), fL3);
   fF2->AddFrame(fChk2 = new TGCheckButton(fF2, "Hide  ", 87), fL3);
   fF2->AddFrame(fChk3 = new TGCheckButton(fF2, "Clip  ", 88), fL3);

   fF2->AddFrame(fLabel1 = new TGLabel(fF2, "Fill"),fL3);
   
   fCombo = new TGComboBox(fF2, 89);
   fF2->AddFrame(fCombo, fL3);

   tf->AddFrame(fF2, fL3);

   int i;
   for (i = 0; i < 8; i++) {
      char tmp[20];

      sprintf(tmp, "%i", i+1);
      fCombo->AddEntry(tmp, i+1);
   }
   fCombo->Select(1);
   fCombo->Resize(50, 20);
   fCombo->Associate(this);

   fChk1->Associate(this);
   fChk2->Associate(this);
   fChk3->Associate(this);
// 
// Tab3: Seen Option
//
   tf = fTab->AddTab("Seen");
   fF3 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
   fF3->AddFrame(fLabel2 = new TGLabel(fF3, "Seen"),fL3);
   fCombo2 = new TGComboBox(fF3, 90);
   fF3->AddFrame(fCombo2, fL3);
   tf->AddFrame(fF3, fL3);

   for (i = 0; i < 4; i++) {
      char tmp[20];

      sprintf(tmp, "%i", i-2);
      fCombo2->AddEntry(tmp, i+1);
   }
   fCombo2->Select(4);
   fCombo2->Resize(50, 20);
   fCombo2->Associate(this);
// 
// Tab4: Clip Box
//
   tf = fTab->AddTab("ClipBox");
   //--- layout for buttons: top align, equally expand horizontally
   fBly = new TGLayoutHints(kLHintsTop | kLHintsExpandY, 5, 5, 5, 5);

   //--- layout for the frame: place at bottom, right aligned
   fBfly1 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX );
//
//  Frames
//
//  Slider1
   fF4 = new TGCompositeFrame(tf, 60, 20, kVerticalFrame);
       
   fHSframe1 = new TGHorizontalFrame(fF4, 400, 100, kFixedWidth);

   fTbh11 = new TGTextBuffer(10);
   fTeh11 = new TGTextEntry(fHSframe1, fTbh11,1);
   fTbh11->AddText(0, "   0");
   fTeh11->Associate(this);

   fTbh12 = new TGTextBuffer(10);
   fTeh12 = new TGTextEntry(fHSframe1, fTbh12,1);
   fTbh12->AddText(0, "2000");
   fTeh12->Associate(this);
    
   fDslider1 = new TGDoubleHSlider(fHSframe1, 400, kSlider1 | kScaleBoth, 1);
   fDslider1->Associate(this);
   fDslider1->SetRange(-2000, 2000);
   fDslider1->SetPosition(0, 2000);
   
   fSLabel1 = new TGLabel(fHSframe1, "xmin-xmax");

   fHSframe1->AddFrame(fSLabel1, fBfly1);
   fHSframe1->AddFrame(fTeh11, fBfly1);
   fHSframe1->AddFrame(fTeh12, fBfly1);
   fHSframe1->AddFrame(fDslider1, fBfly1);
   
   fF4->AddFrame(fHSframe1, fBly);

//
   fHSframe2 = new TGHorizontalFrame(fF4, 400, 100, kFixedWidth);

   fTbh21 = new TGTextBuffer(10);
   fTeh21 = new TGTextEntry(fHSframe2, fTbh21,1);
   fTbh21->AddText(0, "   0");
   fTeh21->Associate(this);

   fTbh22 = new TGTextBuffer(10);
   fTeh22 = new TGTextEntry(fHSframe2, fTbh22,1);
   fTbh22->AddText(0, "2000");
   fTeh22->Associate(this);

   fDslider2 = new TGDoubleHSlider(fHSframe2, 400, kSlider1 | kScaleBoth, 2);
   fDslider2->Associate(this);
   fDslider2->SetRange(-2000, 2000);
   fDslider2->SetPosition(0, 2000);
   
   fSLabel2 = new TGLabel(fHSframe2, "ymin-ymax");

   fHSframe2->AddFrame(fSLabel2, fBfly1);
   fHSframe2->AddFrame(fTeh21, fBfly1);
   fHSframe2->AddFrame(fTeh22, fBfly1);
   fHSframe2->AddFrame(fDslider2, fBfly1);
   
   fF4->AddFrame(fHSframe2, fBly);

//
   fHSframe3 = new TGHorizontalFrame(fF4, 400, 100, kFixedWidth);

   fTbh31 = new TGTextBuffer(10);
   fTeh31 = new TGTextEntry(fHSframe3, fTbh31,1);
   fTbh31->AddText(0, "   0");
   fTeh31->Associate(this);

   fTbh32 = new TGTextBuffer(10);
   fTeh32 = new TGTextEntry(fHSframe3, fTbh32,1);
   fTbh32->AddText(0, "2000");
   fTeh32->Associate(this);

   fDslider3 = new TGDoubleHSlider(fHSframe3, 400, kSlider1 | kScaleBoth, 3);
   fDslider3->Associate(this);
   fDslider3->SetRange(-2000, 2000);
   fDslider3->SetPosition(0, 2000);
   
   fSLabel3 = new TGLabel(fHSframe3, "zmin-zmax");

   fHSframe3->AddFrame(fSLabel3, fBfly1);
   fHSframe3->AddFrame(fTeh31, fBfly1);
   fHSframe3->AddFrame(fTeh32, fBfly1);
   fHSframe3->AddFrame(fDslider3, fBfly1);
   
   fF4->AddFrame(fHSframe3, fBly);
   tf->AddFrame(fF4, fL3);
//
//
   TGLayoutHints *fL5 = new TGLayoutHints(kLHintsBottom | kLHintsExpandX |
                                          kLHintsExpandY, 2, 2, 5, 1);
   AddFrame(fTab, fL5);

   MapSubwindows();
   Resize(GetDefaultSize());

   // position relative to the parent's window
   Window_t wdum;
   int ax, ay;
   gVirtualX->TranslateCoordinates(main->GetId(), GetParent()->GetId(),
                          (((TGFrame *) main)->GetWidth() - fWidth) >> 1,
                          (((TGFrame *) main)->GetHeight() - fHeight) >> 1,
                          ax, ay, wdum);
   Move(ax, ay);

   SetWindowName("Dialog");

   MapWindow();
   //gClient->WaitFor(this);    // otherwise canvas contextmenu does not work
}

StarGuiGeomDialog::~StarGuiGeomDialog()
{
   // Delete test dialog widgets.

   delete fOkButton;
   delete fCancelButton;
   delete fFrame1;
   delete fChk1; delete fChk2;
   delete fF1; delete fF2; delete fF3; delete fF4;
   delete fCombo;
   delete fTab;
   delete fL3; delete fL4;
   delete fL1; delete fL2;
   delete fBly; delete fBfly1;
   delete fTbh11; delete fTbh12; delete fTbh21; delete fTbh22; 
   delete fTbh31; delete fTbh32; delete fTeh11; delete fTeh12; 
   delete fTeh21; delete fTeh22; delete fTeh31; delete fTeh32;
   delete fDslider1; delete fDslider2; delete fDslider3;
   delete fSLabel1;  delete fSLabel2;  delete fSLabel3;
}

void StarGuiGeomDialog::Update()
{
    
    Float_t param;
//  Update Sliders
    if (fF1) {
	fF1->Update();
    }
//  Seen
    if (fCombo2) {
	param=gCurrentVolume->GetParam(P_Seen);
	fCombo2->Select(Int_t(param)+3);
    }
//  Hide, Shadow, Clip
    if (fChk1) {
	if (Int_t(gCurrentVolume->GetParam(P_Shadow))) {
	    fChk1->SetState(kButtonDown);
	} else {
	    fChk1->SetState(kButtonUp);
	}
    }

    if (fChk2) {
	if (Int_t(gCurrentVolume->GetParam(P_Hide))) {
	    fChk2->SetState(kButtonDown);
	} else {
	    fChk2->SetState(kButtonUp);
	}
    }

    if (fChk3) {
	if (Int_t(gCurrentVolume->GetParam(P_Clip))) {
	    fChk3->SetState(kButtonDown);
	} else {
	    fChk3->SetState(kButtonUp);
	}
    }
    
}

void StarGuiGeomDialog::CloseWindow()
{
   // Called when window is closed via the window manager.
   delete this;
}

Bool_t StarGuiGeomDialog::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Process messages coming from widgets associated with the dialog.
    char buf[10];
    Float_t min,max;
    switch (GET_MSG(msg)) {
    case kC_HSLIDER:
	switch (GET_SUBMSG(msg)) {
	case kSL_POS:
	    switch (Int_t(parm1)) {
	    case 1:
		min=fDslider1->GetMinPosition();
		max=fDslider1->GetMaxPosition();
		sprintf(buf, "%6.2f", min);
		fTbh11->Clear();
		fTbh11->AddText(0, buf);
		sprintf(buf, "%6.2f", max);
		fTbh12->Clear();
		fTbh12->AddText(0, buf);
		gClient->NeedRedraw(fTeh11);
		gClient->NeedRedraw(fTeh12);
		gCurrentVolume->SetParam(P_ClipXmin,min);
		gCurrentVolume->SetParam(P_ClipXmax,max);
		break;
	    case 2:
		min=fDslider2->GetMinPosition();
		max=fDslider2->GetMaxPosition();
		sprintf(buf, "%6.2f", min);
		fTbh21->Clear();
		fTbh21->AddText(0, buf);
		sprintf(buf, "%6.2f", max);
		fTbh22->Clear();
		fTbh22->AddText(0, buf);
		gClient->NeedRedraw(fTeh21);
		gClient->NeedRedraw(fTeh22);
		gCurrentVolume->SetParam(P_ClipYmin,min);
		gCurrentVolume->SetParam(P_ClipYmax,max);
		break;
	    case 3:
		min=fDslider3->GetMinPosition();
		max=fDslider3->GetMaxPosition();
		sprintf(buf, "%6.2f", min);
		fTbh31->Clear();
		fTbh31->AddText(0, buf);
		sprintf(buf, "%6.2f", max);
		fTbh32->Clear();
		fTbh32->AddText(0, buf);
		gClient->NeedRedraw(fTeh31);
		gClient->NeedRedraw(fTeh32);
		gCurrentVolume->SetParam(P_ClipZmin,min);
		gCurrentVolume->SetParam(P_ClipZmax,max);
		break;
	    default:
		break;
	    }
	}
	break;
    case kC_COMMAND:
	switch (GET_SUBMSG(msg)) {
	case kCM_BUTTON:
	    switch(parm1) {
	    case 1:
	    case 2:
		printf("\nTerminating dialog: %s pressed\n",
		       (parm1 == 1) ? "OK" : "Cancel");
		CloseWindow();
		break;
	    }
	    break;
	case kCM_COMBOBOX:
	    switch(parm1) {
	    case 89:
		gCurrentVolume->SetParam(P_Fill, Float_t(parm2));
		gCurrentVolume->Draw();
		break;
	    case 90:
		gCurrentVolume->SetParam(P_Seen, Float_t(parm2-3));
		gCurrentVolume->Draw();
		break;
	    }
	    break;
	case kCM_CHECKBUTTON:
	    switch (parm1) {
	    case 86:
		if (Int_t(gCurrentVolume->GetParam(P_Shadow))) {
		    gCurrentVolume->SetParam(P_Shadow, 0.);
		} else {
		    gCurrentVolume->SetParam(P_Shadow, 1.);
		}
		gCurrentVolume->Draw();
		break;
	    case 87:
		if (Int_t(gCurrentVolume->GetParam(P_Hide))) {
		    gCurrentVolume->SetParam(P_Hide, 0.);
		} else {
		    gCurrentVolume->SetParam(P_Hide, 1.);
		}
		gCurrentVolume->Draw();
		break;
	    case 88:
		if (Int_t(gCurrentVolume->GetParam(P_Clip))) {
		    gCurrentVolume->SetParam(P_Clip, 0.);
		} else {
		    gCurrentVolume->SetParam(P_Clip, 1.);
		}
		gCurrentVolume->Draw();
		break;

	    default:
		break;
	    }
	    break;
	case kCM_TAB:
	    break;
	default:
	    break;
	}
	break;
    default:
	break;
    }
    return kTRUE;
}

static Text_t* LabelText[7]  = 
{"Theta  ", "Phi    ", "Psi    ", "U      ", "V      ", "UScale", "VScale"};
static Int_t   IRangeMin[7]  = {    0,     0,     0,    0,    0,   0,   0};
static Int_t   IRangeMax[7]  = {36000, 36000, 36000, 2000, 2000, 10, 10};
static Int_t   DefaultPos[7] = { 3000,  4000,     0, 1000, 1000,   1,   1};

StarGUISliders::StarGUISliders(const TGWindow *p, const TGWindow *,
                         UInt_t w, UInt_t h) :
    TGCompositeFrame(p, w, h,kVerticalFrame)
{
    ChangeOptions((GetOptions() & ~kHorizontalFrame) | kVerticalFrame);
   //--- layout for buttons: top align, equally expand horizontally
    fBly = new TGLayoutHints(kLHintsTop | kLHintsExpandY, 5, 5, 5, 5);

   //--- layout for the frame: place at bottom, right aligned
    fBfly1 = new TGLayoutHints(kLHintsLeft | kLHintsExpandX );
//
// Frames

   for (Int_t i=0; i<7; i++) {
       Int_t idT=i+1;
       Int_t idS=i+8;       
       fHframe[i] = new TGHorizontalFrame(this, 400, 100, kFixedWidth);
       fTbh[i] = new TGTextBuffer(10);
       fTeh[i] = new TGTextEntry(fHframe[i], fTbh[i],idT);
       char buf[10];
       sprintf(buf, "%6.2f", Float_t(DefaultPos[i])/100);
       fTbh[i]->AddText(0, buf);
       fTeh[i]->Associate(this);
       
       fHslider[i] = new TGHSlider(fHframe[i], 400, kSlider1 | kScaleBoth, idS);
       fHslider[i]->Associate(this);
       fHslider[i]->SetRange(IRangeMin[i], IRangeMax[i]);
       fHslider[i]->SetPosition(DefaultPos[i]);

       fLabel[i] = new TGLabel(fHframe[i], LabelText[i]);
       
       
       fHframe[i]->AddFrame(fLabel[i], fBfly1);
       fHframe[i]->AddFrame(fTeh[i], fBfly1);
       fHframe[i]->AddFrame(fHslider[i], fBfly1);
       AddFrame(fHframe[i], fBly);
   }
}

StarGUISliders::~StarGUISliders()
{
    delete fBfly1; delete fBly;
   // Delete dialog.
    for (Int_t i=1; i<7; i++) {
	delete fHframe[i];
	delete fHslider[i];
	delete fTeh[i];
	delete fTbh[i]; 
    }
}

void StarGUISliders::Update()
{
    char buf[10];
    
    for (Int_t i=0; i<7; i++) {
	Float_t param = gCurrentVolume->GetParam(i);
//
	fHslider[i]->SetPosition(Int_t(param*100.));
	gClient->NeedRedraw(fHslider[i]);
//
	sprintf(buf, "%6.2f", param);
	fTbh[i]->Clear();
	fTbh[i]->AddText(0, buf);
	gClient->NeedRedraw(fTeh[i]);
//
    }

    
}

void StarGUISliders::CloseWindow()
{
   // Called when window is closed via the window manager.

   delete this;
}

Bool_t StarGUISliders::ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2)
{
   // Process slider messages.

   char buf[10];

   switch (GET_MSG(msg)) {
   case kC_TEXTENTRY:
       switch (GET_SUBMSG(msg)) {
       case kTE_TEXTCHANGED:
	   Int_t idT=Int_t(parm1)-1;
	   fHslider[idT]->SetPosition((Int_t)atof(fTbh[idT]->GetString())*100);
	   gClient->NeedRedraw(fHslider[idT]);
	   gCurrentVolume->SetParam(idT,atof(fTbh[idT]->GetString()));
	   gCurrentVolume->Draw();
       }
       break;
   case kC_HSLIDER:
       switch (GET_SUBMSG(msg)) {
       case kSL_POS:
	   sprintf(buf, "%6.2f", Float_t(parm2)/100);
	   Int_t idS=Int_t(parm1)-8;
	   fTbh[idS]->Clear();
	   fTbh[idS]->AddText(0, buf);
	   gClient->NeedRedraw(fTeh[idS]);
	   gCurrentVolume->SetParam(idS, Float_t(parm2)/100.);
	   gCurrentVolume->Draw();
       }
       break;
   }
   return kTRUE;
}

ClassImp(StarGUIMaterial)

StarGUIMaterial::StarGUIMaterial()
{ 
    fId=-1;
    fName = 0; 
    fA=-1; 
    fZ=-1; 
    fDensity=-1;
    fRadl=-1;   
    fAbsl=-1;    
}

StarGUIMaterial::StarGUIMaterial(Int_t imat, char* name, Float_t a, Float_t z,
		   Float_t dens, Float_t radl, Float_t absl)
{ 
    fId=imat;
    fName=name;
    fA=a; 
    fZ=z; 
    fDensity=dens;
    fRadl=radl;   
    fAbsl=absl;    
}
void StarGUIMaterial::Dump()
{
    printf("\n *****************************************");
    printf("\n Material Number:   %10d", fId);
    printf("\n %s", fName);
    printf("\n Mass   Number:     %10.2f", fA);    
    printf("\n Charge Number:     %10.2f", fZ);
    printf("\n Density:           %10.2f", fDensity);
    printf("\n Radiation  Length: %10.2f", fRadl);
    printf("\n Absorption Length: %10.2f", fAbsl);        	
}

Int_t StarGUIMaterial::Id()
{
    return fId;
}

char*  StarGUIMaterial::Name()
{
    return fName;
}

Float_t  StarGUIMaterial::A()
{
    return fA;
}

Float_t  StarGUIMaterial::Z()
{
    return fZ;
}

Float_t  StarGUIMaterial::Density()
{
    return fDensity;
}

Float_t  StarGUIMaterial::RadiationLength()
{
    return fRadl;
}

Float_t  StarGUIMaterial::AbsorptionLength()
{
    return fAbsl;
}


void StarGUIMaterial::Plot()
{
    ;
}

void StarGUIMaterial::Streamer(TBuffer &)
{
;
}

ClassImp(StarGUIMedium)

StarGUIMedium::StarGUIMedium()
{ 
    fId=-1;
    fName = 0; 
}

StarGUIMedium::StarGUIMedium(Int_t imed, Int_t imat, char* name, Int_t isvol, 
			   Int_t ifield,
			   Float_t fieldm, Float_t tmaxfd, Float_t stemax, Float_t deemax,
			   Float_t epsil, Float_t stmin)
{
    fId=imed;
    fIdMat=imat;
    fName=name;
    fIsvol=isvol;
    fIfield=ifield;
    fFieldm=fieldm;
    fTmaxfd=tmaxfd;
    fStemax=stemax;
    fDeemax=deemax;
    fEpsil=epsil;
    fStmin=stmin;
}

void StarGUIMedium::Dump()
{
    ;
}

Int_t StarGUIMedium::Id()
{
    return fId;
}

char*  StarGUIMedium::Name()
{
    return fName;
}

Float_t StarGUIMedium::GetPar(Int_t ipar)
{ 
    Float_t p;
    if (ipar < 23) {
	p= fPars[ipar-1];   
    } else if(ipar >=23 && ipar <27) {
	p= fPars[ipar-1+3];   
    } else {
	p= fPars[ipar-1+4];   
    }
    
    return p;
}
 
void StarGUIMedium::Streamer(TBuffer &)
{
;
}















