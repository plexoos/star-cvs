// @(#)root/base:$Name:  $: TQtRootViewer3D.cxx
// Author: Valeri Fine 05/05/2005

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// TQtRootViewer3D                                                      //
//                                                                      //
// Generic 3D shapes viewer.                                            //
//                                                                      //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include "TQtRootViewer3D.h"
#include "TObject3DView.h"
#include "TDataSetIter.h"
#include "TQtGLViewerImp.h"
#include "TVirtualPad.h"
#include "TGQt.h"

#include "TList.h"
#include "TError.h"

#ifndef __APPLE__
#  include "TRootGLU.h"
#else
#  include <glu.h>
#endif

#include "TObjectOpenGLViewFactory.h"
#if QT_VERSION >= 0x40000
//Added by qt3to4:
#include <QPixmap>
#endif /* QT_VERSION */



ClassImp(TQtRootViewer3D)

//______________________________________________________________________________
void SlotDisconnect::CleanPrimitives()
{
   if (fMaster) fMaster->ClearPrimitives();
}

//______________________________________________________________________________
void SlotDisconnect::Disconnect()
{
   if (fMaster){  fMaster->fViewer = 0; fMaster->Disconnect(); } 
}
//______________________________________________________________________________
void SlotDisconnect::DestroyMaster()
{ 
   Disconnect();
   TQtRootViewer3D *master = fMaster; 
   fMaster = 0; 
   emit Destroyed(master);
   delete master;
}
//______________________________________________________________________________
void  SlotDisconnect::UpdateView(QPixmap *)
{
  if ( fMaster && (fMaster->fPad == (TVirtualPad*)gPad) ) fMaster->UpdateView();
}

//______________________________________________________________________________
TQtRootViewer3D::TQtRootViewer3D(TVirtualPad*pad)
: TVirtualViewer3D(), fPad(pad),fViewer(0),fDepth(3),fBuildingScena(kFALSE)
{  }
//______________________________________________________________________________
TQtGLViewerImp   *TQtRootViewer3D::Viewer()
{
   if (!fViewer && fPad) {
      fViewer = new TQtGLViewerImp(fPad,fPad->GetName(),fPad->UtoPixel(1.),fPad->VtoPixel(0.) );
      // fprintf(stderr,"QtRootViewer3D %p for TPad %p : %s \n", this, fPad, fPad->GetName());
      fDisconnectSlot = new SlotDisconnect(this);
     //  QObject::connect(fViewer,SIGNAL(destroyed()), fDisconnectSlot, SLOT(Disconnect()));
      QObject::connect(fViewer,SIGNAL(destroyed()), fDisconnectSlot, SLOT(DestroyMaster()));
 //     QObject::connect(fViewer->GLWidget(),SIGNAL(viewerAbout2Close())
 //                     , fDisconnectSlot, SLOT(CleanPrimitives()));
      // fPad->Connect("Modified()","TQtRootViewer3D", this, "UpdateView()");
      fPad->Connect("Closed()","TQtRootViewer3D", this, "DisconnectPad()");
      QObject::connect(gQt->Emitter(),SIGNAL(padPainted(QPixmap*)),fDisconnectSlot, SLOT(UpdateView(QPixmap*)));
   }
   return fViewer;
}
//______________________________________________________________________________
TQtRootViewer3D::~TQtRootViewer3D()
{
    TQtGLViewerImp *sav = fViewer;
    if (sav && fDisconnectSlot) {
       QObject::disconnect(sav,SIGNAL(destroyed()), fDisconnectSlot, SLOT(DestroyMaster()));
//       QObject::disconnect(sav,SIGNAL(viewerAbout2Close()), fDisconnectSlot, SLOT(CleanPrimitives()));
       ClearPrimitives();
    }
    if (fDisconnectSlot) {  delete fDisconnectSlot; fDisconnectSlot = 0; }
    Disconnect();
    if (sav) delete sav;
}
//______________________________________________________________________________
Int_t  TQtRootViewer3D::AddObject(TObject *obj, Option_t *drawOption, Bool_t *addChildren )
{
  // Add one TObject to the viewer at the end of the list
  Int_t result = 0;
  if (addChildren) {}
  if (obj && obj->InheritsFrom("TAtt3D") ) {
    Option_t *opt = drawOption;
    if (!opt || !opt[0]) opt = obj->GetDrawOption();
    TObject3DView *view = new TObject3DView(obj,opt);
    fListOfPrimitives.TDataSet::AddLast(view);
    result = 1;
  }
  return result;
}

//______________________________________________________________________________
Int_t  TQtRootViewer3D::AddObjectFirst(TObject *obj, Option_t *drawOption, Bool_t *addChildren )
{
  // Add one front TObject to the viewer
  Int_t result = 0;
  if (addChildren) {}
  if (obj && obj->InheritsFrom("TAtt3D") ) {
    Option_t *opt = drawOption;
    if (!opt || !opt[0]) opt = obj->GetDrawOption();
    TObject3DView *view = new TObject3DView(obj,opt);
    fListOfPrimitives.TDataSet::AddFirst(view);
    result = 1;
  }
  return result;
}

//______________________________________________________________________________
void   TQtRootViewer3D::BeginScene() {
   BeginScene(fPad);
}
//______________________________________________________________________________
void  TQtRootViewer3D::BeginScene(TVirtualPad *pad)
{
   // called by TPad::Paint | PaintModified
   // Create the 3D view of the object from the list of the primitives
   fBuildingScena = kTRUE;
   Viewer(); 
   // Bool_t began3DScene = kFALSE;
   ClearPrimitives(); 
   if (pad) {
      TObjOptLink *lnk = (TObjOptLink*)pad->GetListOfPrimitives()->FirstLink();
      TObject *obj = 0;
      while (lnk) {
         obj = lnk->GetObject();

        // Create a 3D view for the encountered a 3D shape
        Int_t objCount = 0;
        if (obj->InheritsFrom(TCollection::Class())){
           TIter next((TCollection*)obj);
           TObject *addMe  = 0;
           while ( (addMe = next())  ) objCount += AddObject(addMe,lnk->GetOption());
        } else {
            objCount += AddObject(obj,lnk->GetOption());
        }
        // printf(" TQtRootViewer3D::BeginScene obj %s 3d=%d\n", obj->GetName(), obj->InheritsFrom("TAtt3D"));
        //Int_t depth = fDepth;
        // if (obj->InheritsFrom("TGeoVolume")) depth *= 2;
        // obj->Paint(lnk->GetOption());
        lnk = (TObjOptLink*)lnk->Next();
      }
   }
}
//______________________________________________________________________________
Bool_t TQtRootViewer3D::BuildingScene() const{
   // called by TPad::Paint | PaintModified
   return  fBuildingScena;
}
//______________________________________________________________________________
void TQtRootViewer3D::ClearPrimitives()
{
   if (fViewer)  {
      fViewer->MakeCurrent();
      fViewer->setUpdatesEnabled(FALSE);
      fViewer->Clear();
   }
   fListOfPrimitives.Delete();
   fListOfPrimitives.Clear();
}
//______________________________________________________________________________
void  TQtRootViewer3D::EndScene(){
   // called by TPad::Paint | PaintModified
   // This is a signal the scene has been closed and we should refresh the display
   if (fViewer) {
     fViewer->MakeCurrent();
#if 1
      fViewer->setUpdatesEnabled(FALSE);
      fViewer->Clear();
#endif
    fViewer->SetBackgroundColor(gPad->GetFillColor());
#if 0     
     fListOfPrimitives.CompileViewLevel();
#endif     
     TDataSetIter nextList(&fListOfPrimitives);
     TObject3DView *glo = 0;
     while( (glo = (TObject3DView *)nextList()  )) {
        fViewer->AddGLList(glo->GetViewId(), glo->IsSolid() );
#ifdef EXTRASELECTION
        if ( glo->GetViewId(TObject3DViewFactoryABC::kSelectable) ) 
           fViewer->AddGLList(glo->GetViewId(TObject3DViewFactoryABC::kSelectable), 2 );
#endif
     }
     fViewer->setUpdatesEnabled(TRUE);
     fViewer->Update();
   }
   fBuildingScena = kFALSE;
}
//______________________________________________________________________________
void   TQtRootViewer3D::DisconnectPad()
{
   if (fPad) {
      // fprintf(stderr," TQtRootViewer3D::DisconnectPad() fPad= %p %s\n", fPad, fPad->GetName());
      QObject::disconnect(gQt->Emitter(),SIGNAL(padPainted(QPixmap*)),fDisconnectSlot, SLOT(UpdateView(QPixmap*)));
      if (fViewer) {
          fViewer->DisconnectPad();
      }
      fPad->TQObject::Disconnect("Closed()",this, 0);
      fPad->TQObject::Disconnect("Modified()",this, 0);
      fPad = 0;
   }
}
//______________________________________________________________________________
void   TQtRootViewer3D::Disconnect() 
{
   if (fPad)  {
      // fprintf(stderr," TQtRootViewer3D::Disconnect() fViewer=%p\n", fViewer);
      if (fViewer) {
         QObject::disconnect(fViewer,SIGNAL(destroyed()),        fDisconnectSlot, SLOT(DestroyMaster()));
         // QObject::disconnect(fViewer,SIGNAL(viewerAbout2Close()),fDisconnectSlot, SLOT(CleanPrimitives()));
         fViewer->DisconnectPad();
      }
      fPad->TQObject::Disconnect("Closed()",this, 0);
      fPad->TQObject::Disconnect("Modified()",this, 0);
#if ROOT_VERSION_CODE < ROOT_VERSION(5,07,00)
      TVirtualViewer3D *currentPadViewer = fPad->GetViewer3D(0);
#else
      TVirtualViewer3D *currentPadViewer = fPad->GetViewer3D("");
#endif
      if (currentPadViewer == (TVirtualViewer3D *)this ) 
         fPad->ReleaseViewer3D();
   }
   fViewer = 0;
}


//   Dummy methdos  --

//______________________________________________________________________________
// Simple object addition - buffer represents a unique single positioned object
Int_t  TQtRootViewer3D::AddObject(const TBuffer3D &, Bool_t *) 
{ return 0;}

//______________________________________________________________________________
   // Complex object addition - for adding placed objects which have common template 
   // shapes. In this case buffer describes template shape (aside from kCore). 
Int_t  TQtRootViewer3D::AddObject(UInt_t , const TBuffer3D &, Bool_t *) 
{return 0;}


//______________________________________________________________________________
#if  ROOT_VERSION_CODE <  ROOT_VERSION(5,00,00) 
void  TQtRootViewer3D::OpenComposite(const TBuffer3D &, Bool_t *)
{}
#else
Bool_t TQtRootViewer3D::OpenComposite(const TBuffer3D &, Bool_t *)
{ return kFALSE; }
#endif

//______________________________________________________________________________
void  TQtRootViewer3D::UpdateView()
{
   BeginScene();
   EndScene();
}
