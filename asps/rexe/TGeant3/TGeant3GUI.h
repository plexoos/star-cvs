#ifndef ROOT_guitest
#define ROOT_guitest
/* Copyright(c) 1998-1999, ALICE Experiment at CERN, All rights reserved. *
 * See cxx source for full Copyright notice                               */

/* $Id: TGeant3GUI.h,v 1.2 2000/04/23 19:18:14 fisyak Exp $ */

#include <TROOT.h>
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
#include <TClonesArray.h>
#include <TGeant3.h>

class StarGuiGeomDialog;
class StarGUISliders;
class StarGuiGeomMain;
class StarDrawVolume;
class StarGUIMaterial;
class StarGUIMedium;


class StarGeant3GeometryGUI : public TObject {
 private:
    StarGuiGeomMain *fPanel;     // the main gui panel
    Int_t          fNstack;      // number of volumes
    TClonesArray   *fVolumes;    // array of volumes  
    Int_t          fNMaterials;  // number of materials and media
    TClonesArray   *fMaterials;  // array of materials
    TClonesArray   *fMedia;  // array of materials    
// Zebra bank related information	
    Int_t*    fZlq;              
    Float_t*  fZq;
    Int_t*    fZiq;
    Gclink_t* fGclink;
    Gcnum_t*  fGcnum;
 public:
    StarGeant3GeometryGUI();
    // Reads the zebra geometry tree and put it into the ListTree
    void  ReadGeometryTree();
    // Read material and media information and put it into ComboBox 
    void  ReadMaterials();
    Float_t Cut(Int_t idmed, Int_t icut);
 private:
    virtual StarDrawVolume* Volume(Int_t id)
	{return (StarDrawVolume *) (fVolumes->UncheckedAt(id));}
    // Return number of children for volume idvol
    Int_t NChildren(Int_t idvol);
    // Return child number idc of volume idvol
    Int_t Child(Int_t idvol, Int_t idc);
    // Return medium number for given volume idvol
    Int_t Medium(Int_t idvol);
    // Return material number for given volume idvol
    Int_t Material(Int_t idvol);
    //

    
    ClassDef(StarGeant3GeometryGUI,1)  // GUI for Geant3 geometry visualisation
};


class StarGuiGeomMain : public TGMainFrame {

private:
    TGTab              *fTab;     
    TGCanvas           *fCanvasWindow;
    TGCompositeFrame   *fContainer, *fF2, *fF21, *fF3, *fF31, *fF4, *fF5;
    TGCompositeFrame   *fF6, *fF61, *fF62, *fF63;
    TGListTree         *fLt;
    TGMenuBar          *fMenuBar;
    TGPopupMenu        *fMenuFile, *fMenuTest, *fMenuHelp;
    TGLayoutHints      *fMenuBarItemLayout, *fMenuBarHelpLayout,
	               *fMenuBarLayout, fLTab;
    TGLayoutHints      *fL2;
    StarGuiGeomDialog   *fDialog;                   //! no output please
    TGComboBox         *fMaterialCombo;
    TGComboBox         *fMechanismCombo;
    TGComboBox         *fMediaCombo, *fParticleCombo;
    TGListBox          *fProcessLB, *fCutsLB;
    TClonesArray       *fComboEntries;
    TClonesArray       *fComboMediaEntries;    
    TGHorizontalFrame  *fHframe[6],*fHframeM[8];
    TGTextBuffer       *fTbh[6], *fTbhM[8], *fTbh61, *fTbh62, *fTbh63;
    TGTextEntry        *fTeh[6], *fTehM[8], *fTeh61, *fTeh62, *fTeh63;
    TGLabel            *fLabel[6], *fLabelM[8], *fSLabel61;
    TGTextButton       *fPlotButton;
    Float_t            fEmin;
    Float_t            fEmax;
    Int_t              fNbins;
    
 public:
    StarGuiGeomMain(const TGWindow *p, UInt_t w, UInt_t h);
    virtual ~StarGuiGeomMain();
    // Destroy the main window
    virtual void CloseWindow();
    // Add item to ListTree
    virtual TGListTreeItem *
	AddItem(TObject *, TGListTreeItem* ,
		const char*, const TGPicture*, const TGPicture*);
    // Add Material to ComboBox
    virtual void AddMaterial(StarGUIMaterial *Material, Int_t i);
    // Add Medium to ComboBox
    virtual void AddMedium(StarGUIMedium *Medium, Int_t i);
    // Process messages from this window
    virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t);
    // Update widgets
    virtual void Update();
    // Update ComboBoxes
    virtual void UpdateCombo();
    virtual void UpdateListBox();
    // Relate objects to ComboEntries
    // Currently ComboBox Entries are strings only, hence we need this construction
    virtual void SetComboEntries(TClonesArray *entries) {fComboEntries=entries;}
    virtual void SetMediaComboEntries(TClonesArray *entries)
	{fComboMediaEntries=entries;}
    virtual void Plot();
    ClassDef(StarGuiGeomMain,1)  // MainFrame for Geometry Browser
};


class StarGuiGeomDialog : public TGTransientFrame {

private:
    StarGUISliders       *fF1;
    TGCompositeFrame    *fFrame1, *fF2, *fF3, *fF4;
    TGButton            *fOkButton, *fCancelButton;
    TGButton            *fChk1, *fChk2, *fChk3;
    TGComboBox          *fCombo, *fCombo2;
    TGLabel             *fLabel1, *fLabel2;
    TGTab               *fTab;
    TGLayoutHints       *fL1, *fL2, *fL3, *fL4, *fBly, *fBfly1;
    TGHorizontalFrame   *fHSframe1, *fHSframe2, *fHSframe3;
    TGTextBuffer        *fTbh11, *fTbh12, *fTbh21, *fTbh22, *fTbh31, *fTbh32;
    TGTextEntry         *fTeh11, *fTeh12, *fTeh21, *fTeh22, *fTeh31, *fTeh32;
    TGDoubleHSlider     *fDslider1, *fDslider2, *fDslider3;
    TGLabel             *fSLabel1,  *fSLabel2,  *fSLabel3;
    
public:
   StarGuiGeomDialog(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h,
               UInt_t options = kMainFrame | kVerticalFrame);
   virtual ~StarGuiGeomDialog();
// Destroy this window
   virtual void CloseWindow();
// Process messages from this window    
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
// Update widgets   
   virtual void Update();
};

class StarGUISliders : public  TGCompositeFrame {

private:
//
    TGHorizontalFrame *fHframe[8];
    TGLayoutHints     *fBly, *fBfly1;
    TGHSlider         *fHslider[8];
    TGTextEntry       *fTeh[8];
    TGTextBuffer      *fTbh[8];
    TGLabel           *fLabel[8];
    Text_t            fLabelText[8];
public:
   StarGUISliders(const TGWindow *p, const TGWindow *main, UInt_t w, UInt_t h);
   virtual ~StarGUISliders();
   virtual void CloseWindow();
   virtual Bool_t ProcessMessage(Long_t msg, Long_t parm1, Long_t parm2);
   virtual void Update();
      
   //   ClassDef(StarGUISliders,1)  // Window containing sliders 
};

class StarDrawVolume : public TObject 
{
public:
    StarDrawVolume(char* name);
    virtual ~StarDrawVolume(){;}
    // Draw the volume
    virtual void    Draw(Option_t * =0);
    // Draw volume specs
    virtual void    DrawSpec();
    // Return volume name
    virtual char*   Name();
    // Set volume parameter i
    virtual void    SetParam(Int_t i, Float_t);
    // Get volume parameters i
    virtual Float_t GetParam(Int_t i);
    // Set volume id
    virtual void  SetIdVolume(Int_t id) {fIdVolume = id;}
    // Set volume copy number
    virtual void  SetIdCopy(Int_t id)   {fIdCopy = id;}
    // Set volume medium number
    virtual void  SetIdMedium(Int_t id)   {fIdMedium = id;}
    // Set volume material number
    virtual void  SetIdMaterial(Int_t id) {fIdMaterial = id;}
    // Get volume id
    virtual Int_t GetIdVolume()         {return fIdVolume;}
    // Get copy number
    virtual Int_t GetIdCopy()           {return fIdCopy;}
    // Get medium number
    virtual Int_t Medium()   {return fIdMedium;}
    // Get material number
    virtual Int_t Material() {return fIdMaterial;}
    // Increase copy number by one
    virtual void  AddCopy()             {fIdCopy ++;}
    // Set link to ListTree Item 
    virtual void  SetItem(TGListTreeItem *item) {fItem = item;}
    // Get link to ListTree Item
    virtual TGListTreeItem* GetItem() {return fItem;}
	    
private:
    char*   fName;        // name of the volume 
    Float_t fTheta;       // theta-angle for drawing
    Float_t fPhi;         // phi-angle   for drawing
    Float_t fPsi;         // psi-angle   for drawing 
    Float_t fU;           // u-position
    Float_t fV;           // v-position
    Float_t fUscale;      // u-scaling factor
    Float_t fVscale;      // v-scaling factor
    Bool_t  fHide;        // hide flag
    Bool_t  fShadow;      // shadow flag
    Int_t   fFill;        // fill option 1-6
    Int_t   fSeen;        // seen option -2 - 1
    Bool_t  fClip;        // clipping flag
    Float_t fClipXmin;    // clip box range xmin
    Float_t fClipXmax;    // clip box range xmax
    Float_t fClipYmin;    // clip box range ymin
    Float_t fClipYmax;    // clip box range ymax
    Float_t fClipZmin;    // clip box range zmin
    Float_t fClipZmax;    // clip box range zmax
    Int_t   fIdVolume;    // geant volume id
    Int_t   fIdMedium;    // geant medium id
    Int_t   fIdMaterial;  // geant material id    
    Int_t   fIdCopy;      // copy flag
    TGListTreeItem        *fItem;
    ClassDef(StarDrawVolume,1) // Volume Object for Drawing 
};


class StarGUIMaterial : public TObject 
{
public:
    StarGUIMaterial();
    StarGUIMaterial(Int_t imat, char* name, Float_t a, Float_t z,
		   Float_t dens, Float_t radl, Float_t absl);
    virtual ~StarGUIMaterial(){;}
    // Dump material parameters
    virtual void  Dump();
    // Get material id
    virtual Int_t Id();
    // Get material name
    virtual char* Name();
    // Get mass number 
    virtual Float_t A();
    // Get charge number 
    virtual Float_t Z();
    // Get density
    virtual Float_t Density();
    // Get radiation length
    virtual Float_t RadiationLength();
    // Get absorption lenth
    virtual Float_t AbsorptionLength();
    // Plot
    virtual void  Plot();
    // Set and get link to widget entry
    virtual Int_t ItemId() {return fItem;}
    virtual void  SetItemId(Int_t id) {fItem=id;}
private:
    Int_t   fId;          // Id number of the material
    char*   fName;        // name of the material 
    Float_t fA;           // mass number of the material
    Float_t fZ;           // charge number of the material
    Float_t fDensity;     // density of the material
    Float_t fRadl;        // radiation length of the material
    Float_t fAbsl;        // absorption length
    //
    Int_t   fItem;            // Link to Widget Entry
    ClassDef(StarGUIMaterial,1) // Material Object for GUI 
};


const static Int_t kNPars=33;

class StarGUIMedium : public TObject 
{
public:
    StarGUIMedium();
    StarGUIMedium(Int_t imed, Int_t imat, char* name, Int_t isvol, Int_t ifield,
		 Float_t fieldm, Float_t tmaxfd, Float_t stemax, Float_t deemax,
		 Float_t epsil, Float_t stmin);
    
    virtual ~StarGUIMedium(){;}
    // Dump medium parameters
    virtual void    Dump();
    // Get id
    virtual Int_t   Id();
    // Get name
    virtual char*   Name();
    // Get parameters
    virtual Int_t   IdMat()   {return fIdMat;}
    virtual Int_t   Isvol()   {return fIsvol;}
    virtual Int_t   Ifield()  {return fIfield;}
    virtual Float_t Fieldm()  {return fFieldm;}    
    virtual Float_t Tmaxfd()  {return fTmaxfd;}        
    virtual Float_t Stemax()  {return fStemax;}    
    virtual Float_t Deemax()  {return fDeemax;}        
    virtual Float_t Epsil()   {return fEpsil;}
    virtual Float_t Stmin()   {return fStmin;}
    virtual void    SetPar(Int_t ipar, Float_t par) {fPars[ipar-1]=par;}
    virtual Float_t GetPar(Int_t ipar);
    // Set and get link to widget entry
    virtual Int_t ItemId() {return fItem;}
    virtual void  SetItemId(Int_t id) {fItem=id;}
    
 private:
    Float_t fPars[kNPars];   // special medium parameters
    Int_t   fId;             // Id number of the Medium
    Int_t   fIdMat;          // Associated material
    char*   fName;           // Name of the Medium
    Int_t   fIsvol;          // Sensitivity flag 
    Int_t   fIfield;         // Magnetic Field Flag
    Float_t fFieldm;         // Maximum Field Strength
    Float_t fTmaxfd;         // Max. Ang. Deviation
    Float_t fStemax;         // Maximum Step   
    Float_t fDeemax;         // Max. Frac. Energy Loss",
    Float_t fEpsil;          // Crossing Precission 
    Float_t fStmin;          // Minimum Step Size
    //
    Int_t   fItem;           // Link to Widget Entry
    ClassDef(StarGUIMedium,1) // Tracking Medium Object for GUI 
};

#endif








