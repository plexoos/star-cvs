#include "TVirtualPad.h"
#include "TContextMenu.h"
#include "TImage.h"
#include "TError.h"
#include "TGQt.h"
#include "THLimitsFinder.h"

#include "TQVirtualGL.h"
#include "TQPadOpenGLView.h"

#define QGLVIEWER

#include "TQtCoinWidget.h"
//#include "TQtGLViewerWidget.h"
#include "TObject3DView.h"
#include "TSimageMovie.h"

#include "TSystem.h"
#include "TROOT.h"
#include "TEnv.h"
#include "TColor.h"

#include <qprinter.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qregexp.h> 
#include <qimage.h>
#include <qdir.h>

#if QT_VERSION < 0x40000
#  include <qfiledialog.h>
#  include <qpopupmenu.h>
#  include <qwhatsthis.h> 
#  include <qaction.h>
#  include <qpushbutton.h> 
#  include <qtooltip.h> 
#  include <qcheckbox.h> 
#else 
#  include <q3filedialog.h>
#  include <q3popupmenu.h>
#  include <q3whatsthis.h> 
//Added by qt3to4:
#  include <QLabel>
#  include <Q3Action>
#endif 

#include <qfile.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qmenubar.h>
#include <qimage.h>
#include <qgl.h> 
#include <qpngio.h> 

#include <qpainter.h>
#include <qtextstream.h>
#include <qstatusbar.h>
#include <qsplitter.h>

#include <qevent.h>

#include "TObject3DView.h"

//=============

//#include "TQtCoinWidget.h"
//#include "TVirtualPad.h"
#include "TView.h"
//#include <qglobal.h>
//#include <qmainwindow.h>

#include <qlayout.h> 
#include <qframe.h> 

#include <Inventor/Qt/SoQt.h>
#include <Inventor/Qt/SoQtRenderArea.h>

#include <Inventor/Qt/viewers/SoQtExaminerViewer.h>

#include <Inventor/SoPickedPoint.h>

#include <Inventor/nodes/SoSeparator.h>
#include <Inventor/nodes/SoSelection.h>
#include <Inventor/nodes/SoBaseColor.h> 
#include <Inventor/nodes/SoCallback.h>
#include <Inventor/nodes/SoEventCallback.h>
#include <Inventor/actions/SoCallbackAction.h>
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoOrthographicCamera.h> 
#include <Inventor/nodes/SoPickStyle.h>
#include <Inventor/SoSceneManager.h>
#include <Inventor/nodes/SoShapeHints.h>
#include <Inventor/nodes/SoFaceSet.h>
#include <Inventor/nodes/SoPointSet.h>
#include <Inventor/nodes/SoLineSet.h>
#include <Inventor/nodes/SoIndexedLineSet.h>
//#include <Inventor/VRMLnodes/SoVRMLIndexedLineSet.h>
//#include <Inventor/VRMLnodes/SoVRMLCoordinate.h>
#include <Inventor/VRMLnodes/SoVRMLShape.h>

#include <Inventor/elements/SoCacheElement.h>
#include <Inventor/manips/SoClipPlaneManip.h>

#include <Inventor/SoOffscreenRenderer.h>

#include <Inventor/actions/SoSearchAction.h> 
#include <Inventor/actions/SoToVRML2Action.h>
#include <Inventor/actions/SoGetBoundingBoxAction.h>
#include <Inventor/actions/SoLineHighlightRenderAction.h>
#include <Inventor/actions/SoBoxHighlightRenderAction.h>
#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/nodes/SoDirectionalLight.h>

#include <Inventor/VRMLnodes/SoVRMLGroup.h>
#include <Inventor/manips/SoTransformBoxManip.h>
#include <Inventor/manips/SoTabBoxManip.h>
#include <Inventor/annex/HardCopy/SoHardCopy.h>
#include <Inventor/nodes/SoAnnotation.h>
#include <Inventor/nodes/SoText2.h> 
#include <Inventor/nodes/SoTransparencyType.h> 

#include <Inventor/annex/HardCopy/SoVectorizePSAction.h>
#include <Inventor/annex/HardCopy/SoVectorOutput.h>

#include <Inventor/actions/SoWriteAction.h>
#include <Inventor/actions/SoToVRML2Action.h>
#include <Inventor/VRMLnodes/SoVRMLGroup.h>

#include <Inventor/manips/SoTransformerManip.h>

#include <Inventor/SoPath.h>

#include <Inventor/nodes/SoCone.h>
#include <Inventor/nodes/SoShape.h>
#ifdef NOEXAMINERVIEWER
#  include <SmallChange/nodekits/SmAxisDisplayKit.h>
#endif 
#include <SmallChange/nodekits/SmAxisKit.h>
#include <SmallChange/misc/Init.h>
#include <Inventor/sensors/SoFieldSensor.h> 

#include "x.xpm.h"
#include "y.xpm.h"
#include "z.xpm.h"

#include "xc.xpm.h"
#include "yc.xpm.h"
#include "zc.xpm.h"


#include <vector>
#include "TRootGLU.h"

//#include "OverlayHighlightRenderAction.h"
Bool_t TQtCoinWidget::fgCoinInitialized = kFALSE;
Int_t TQtCoinWidget::gfDefaultMaxSnapFileCounter = 40 ;
//______________________________________________________________________________
static double Round(double range, int prec=2) {
   double r = log10(range);
   double factor = pow(10.0,int(r) - prec - (r < 0 ? 1:0) );
   int fraction  = int(range/factor);
   int remainder = fraction  %10;
   fraction  -= remainder;
   if (2 < remainder && remainder < 8 ) {
      fraction += 5;
   } else if (7 < remainder && remainder < 10 ){
      fraction += 10;
   }
    // restore the number
   return fraction*factor;
}

//______________________________________________________________________________
class TCoinAxisSeparator : public SoSeparator 
{
private:
   bool fOn;
   SmAxisKit *fAxis;
   SoTranslation *fOffset;
   int fNumberTextLabels;
public:
   TCoinAxisSeparator(SmAxisKit *ax=0): SoSeparator(), fOn(false), fAxis(ax)
   , fOffset(0), fNumberTextLabels(-1) {
      setName("MainAxices");
      if (!fAxis) {
         fAxis = new SmAxisKit();
         fAxis->axisName = "X";
      }
      fOffset = new SoTranslation();
      addChild(fOffset);
      addChild(fAxis);
   }
   bool IsOn() const { return fOn; }
   SmAxisKit &Axis() const { return *fAxis; }
   void SetOn(bool on=true) { fOn = on; }
   void Connect(SoGroup *node, bool connect=true){
      if (connect) node->insertChild(this,0);
      else         node->removeChild(this);
   }
   void Disconnect(SoGroup *node) { Connect(node,false); }
   void SetRange(float amin, float amax, int nLabels = 12) {
      double range = amax - amin;
      double textInterval  = Round(range/nLabels);
      amin = textInterval*int(amin/textInterval - (amin < 0 ? 1:0));
      fAxis->textInterval  = textInterval;
      fAxis->axisRange.setValue(amin,amax);
      fAxis->markerHeight  = textInterval/15;
      fAxis->markerInterval= textInterval/10;
      // set the new offset
      double offset  = (amin + amax)/2;
      fOffset->translation.setValue(offset, 0.0f, 0.0f);
   }
   void SetTextLabelNumber(int nLabels=12) {
      if (fAxis && (nLabels > 0)) 
         fAxis->textInterval = Round(fAxis->axisRange.getValue().length()/nLabels);
   }
};

//______________________________________________________________________________
SoGLRenderAction &TQtCoinWidget::BoxHighlightAction()
{
   if (!fBoxHighlightAction) 
      fBoxHighlightAction = new SoBoxHighlightRenderAction;
   return *fBoxHighlightAction;
}
//______________________________________________________________________________
SoGLRenderAction &TQtCoinWidget::LineHighlightAction()
{
   if (!fLineHighlightAction) 
      fLineHighlightAction = new SoLineHighlightRenderAction;
   return *fLineHighlightAction;
}

//______________________________________________________________________________
static void ViewerKeyPressCB(void *userData, SoEventCallback *eventCB)
{
   fprintf(stderr,"ViewerKeyPressCB --> \n");
   if (userData) {
      TQtCoinWidget *currentViewer = (TQtCoinWidget *)userData;
      {
         const SoKeyboardEvent  *event = (SoKeyboardEvent *)eventCB->getEvent();
         int axis = 0;
         fprintf(stderr,"ViewerKeyPressCB %d\n", event->getKey());
         switch (event->getKey()) {
            case SoKeyboardEvent::X: 
               axis = 0; break;
            case SoKeyboardEvent::Y: 
               axis = 1; break;
            case SoKeyboardEvent::Z: 
               axis = 2; break;
            default: 
                         break;
         };
         currentViewer->RotateCamera(axis,(bool)event->wasShiftDown() );
      }
   }
}
#if 0
//______________________________________________________________________________
// Inventor call back function
static void InventorCallback1(void *d, SoAction *action)
{
   printf("static void InventorCallback1\n");
   if (!d) return;
   TQtCoinWidget *currentViewer = (TQtCoinWidget *)d;
   if ( currentViewer ) {
      if (action->isOfType(SoGLRenderAction::getClassTypeId()) )
      {
        // glEnable(GL_LIGHTING);
        SoCacheElement::invalidate(action->getState());
	int max = currentViewer->GetMyGLList1().size();
	printf("static void InventorCallback1 lists count=%i\n", max);
        // testCube();
	for (int i = 0; i < max; i++) glCallList(currentViewer->GetMyGLList1()[i]);
	//gVirtualGL->RunGLList(myGLIst + TPadOpenGLView::kModel);
      }
      //*
      else if (action->isOfType(SoGetBoundingBoxAction::getClassTypeId()) )
      {

         Float_t minBound[3]={-1500,-1500,-1500};
         Float_t maxBound[3]={ 1500, 1500, 1500};

         //currentViewer->GetGLView()->GetPad()->GetView()->GetRange(minBound,maxBound);		// old
	 currentViewer->GetPad()->GetView()->GetRange(minBound,maxBound);				// new
	
	  //printf(" %f %f %f :  %f %f %f \n", minBound[0],minBound[1],minBound[2]
	//		  , maxBound[0],maxBound[1],maxBound[2]);
	  
         if (minBound[0] == maxBound[0]) {
            SoCacheElement::invalidate(action->getState());
         }
         SoGetBoundingBoxAction *boundAction =  (SoGetBoundingBoxAction *)action;
         Float_t min = TMath::Min(TMath::Min(minBound[0],minBound[1]),minBound[2]);
         Float_t max = TMath::Max(TMath::Max(maxBound[0],maxBound[1]),maxBound[2]);
         boundAction->
            extendBy(SbBox3f(min,min,min,max,max,max));

         //boundAction->
         //   extendBy(SbBox3f(minBound[0],minBound[1],minBound[2]
         //   ,maxBound[0],maxBound[1],maxBound[2])
         //      );
             // I had to add the line below into 
             //  SoNode::GLRenderS:780 to supress the annoying warning
             // This is very dirty but I have to do that (Valeri Fine 24.05.2003 fine@bnl.gov)
             //    err =  GL_NO_ERROR;

         const SbVec3f &centerPoint =  boundAction->getBoundingBox().getCenter();
         boundAction->setCenter(centerPoint, false);
         float x,y,z;
         centerPoint.getValue(x,y,z);
         // printf(" new center = %f %f %f \n", x,y,z);
	 
      }
      //*/
   }
}

//______________________________________________________________________________
// Inventor call back function
static void InventorCallback2(void *d, SoAction *action)
{
   if (!d) return;
   TQtCoinWidget *currentViewer = (TQtCoinWidget *)d;
   if ( currentViewer ) {
      if (action->isOfType(SoGLRenderAction::getClassTypeId()) )
      {
        SoCacheElement::invalidate(action->getState());
        //glEnable(GL_LIGHT0); 
        glEnable(GL_LIGHTING);
        //glEnable(GL_COLOR_MATERIAL);

	int max = currentViewer->GetMyGLList2().size();
	printf("static void InventorCallback2 lists count=%i\n", max);
	for (int i = 0; i < max; i++) glCallList(currentViewer->GetMyGLList2()[i]);
      }
   }
}

//______________________________________________________________________________
// Inventor call back function
static void InventorCallback3(void *d, SoAction *action)
{
   printf("static void InventorCallback2\n");
   if (!d) return;
   TQtCoinWidget *currentViewer = (TQtCoinWidget *)d;
   if ( currentViewer ) {
      if (action->isOfType(SoGLRenderAction::getClassTypeId()) )
      {
        SoCacheElement::invalidate(action->getState());
	int max = currentViewer->GetMyGLList3().size();
	for (int i = 0; i < max; i++) glCallList(currentViewer->GetMyGLList3()[i]);
      }
   }
}
static QPNGImagePacker *fAnimator = 0;
static QIODevice *fAnimDevice;
static int iframe = 0;

#endif
//______________________________________________________________________________
// Inventor call back function
static void MovieCallback(void *d, SoAction *action)
{
   if (!d) return;
   TQtCoinWidget *currentViewer = (TQtCoinWidget *)d;
   if ( currentViewer && currentViewer->Recording() ) {
      if (action->isOfType(SoGetBoundingBoxAction::getClassTypeId()) )
//      if (action->isOfType(SoGLRenderAction::getClassTypeId()) )
      {
        // SoCacheElement::invalidate(action->getState());
        currentViewer->SaveSnapShot();
      }
   }
}

//static bool g_isManip = false;

//______________________________________________________________________________
static void DeselectCB(void * viewer, SoPath *) 
{
   // printf("DeselectCB %p %d\n",p,p->getRefCount() );	
	/*
   SoPath * path = new SoPath(*p);
   path->ref();
   path->truncate(path->getLength()-3);
   if (path->getLengt  printf("TQtCoinWidget::TQtCoinWidget %d end\n",fMaxSnapFileCounter);
h() == 0) {
	//printf("!!!Er:path.len = 0\n");
	return;
   }
   SoSeparator * group = (SoSeparator *) path->getTail();
   if (g_isManip) {
	   SoTransformerManip * manip = (SoTransformerManip *) group->getChild(0);
	   if ( manip->isOfType(SoTransformerManip::getClassTypeId()) ) {
	      path->append(manip);
	      manip->replaceManip(path, new SoTransform);
	   }
   }
   path->unref();
	*/
  ((TQtCoinWidget*)(viewer))->Update();
}


//______________________________________________________________________________
static SoPath * PickFilterCB(void * viewer, const SoPickedPoint * pick)
{
   SoPath *selPath = 0;
   SoPath *p = pick->getPath();
	/*
   if (p->getTail()->getTypeId() == SoLineSet::getClassTypeId()          ||
       p->getTail()->getTypeId() == SoIndexedLineSet::getClassTypeId()) {
	// for lines
	((TQtCoinWidget*)(viewer))->SetLineSelection();
   } else {
	// for figures
	((TQtCoinWidget*)(viewer))->SetBoxSelection();
   }
	*/
   int i=0;
   int l = p->getLength();
   if (l > 1) {
      for (i = l - 1; i > 0; i--) {
         SoNode *n = p->getNode(i);
         if (n->getName() == "CoinShapeNode")
            break;
     }
   }
//  SoGroup *thisGroup = (SoGroup *)p->getNode(i);
  TObject *o = (TObject*)p->getNode(i)->getUserData();
  TObject3DView *v = dynamic_cast<TObject3DView *>(o);
 // printf("static SoPath *PickFilterCB l=%d, i=%d %s : root name = %s \n",p->getLength(),i
 //        ,(const char *)p->getNode(i)->getName(),(const char *)v->GetName());
  TQtCoinWidget *thisViewer = (TQtCoinWidget*)viewer;
  if (v) {
     if (v->IsSolid()) thisViewer->SetLineSelection();
     else              thisViewer->SetBoxSelection();
     // Emit signal at once
     if (!thisViewer->WasPicked(v) ) {
        TObject3DView  *parent = (TObject3DView  *)v->GetParent();
        TObject *obj = parent ? parent->GetObject() :0 ;
        if (obj)  thisViewer->EmitSelectSignal(obj);
     }
     selPath = p->copy(0, i);
  } else {
     // No TObject attached to the coin path was discovered
     o = (TObject *)p->getTail()->getUserData();
     if (o) {
        thisViewer->EmitSelectSignal(o);
     } else {
        thisViewer->EmitNodeSelectSignal(p->getTail());
     }
     selPath = p->copy(0);
  }
  // fprintf(stderr,"static SoPath *PickFilterCB path %p\n", selPath);
  return selPath;
}

//______________________________________________________________________________
static void SelectCB(void * viewer, SoPath *p)
{
   // fprintf(stderr, "SelectCB %p %d\n",p,p->getRefCount() );	
	/*
   SoPath * path = new SoPath(*p);
   path->ref();
   path->truncate(path->getLength()-3);
   //printf("SelectCB; %S\n", p->getTail()->getTypeId().getName().getString());
   if (path->getLength() != 0) {
	   TObject3DView * v = (TObject3DView*)(path->getTail()->getUserData());
	   //printf("\tnode->userdata=%p\n", v);
	   ((TQtCoinWidget*)(viewer))->EmitSelectSignal(v);
	   if (g_isManip) {
		   SoSeparator * group = (SoSeparator *) path->getTail();
		   if ( !group->getChild(0)->isOfType(SoTransform::getClassTypeId())) {
		      group->insertChild(new SoTransform, 0);
		   }	
		   SoTransform * transform = (SoTransform *) group->getChild(0);
		   path->append(transform);
		   static SoTransformerManip * manip = 0;
		   // if (!manip) {
		      manip = new SoTransformerManip;
		      manip->ref();
		   // }
		   manip->replaceNode(path);
	   }
   } else {
	//printf("!!!Er:path.len = 0\n");
   }
   path->unref();
   */
   TQtCoinWidget *thisViewer = (TQtCoinWidget*)viewer;
   thisViewer->Update();

    // Popup the object tip

   TObject *selectedObject = thisViewer->GetSelected();
   if ((!thisViewer->WantRootContextMenu()) && selectedObject ) {
      QPoint globalPosition = QCursor::pos();
      QWidget *viewerWidget = thisViewer->GetCoinViewer()->getWidget();
      QPoint cursorPosition = viewerWidget->mapFromGlobal(globalPosition);
      QString tipText =  selectedObject->GetObjectInfo(cursorPosition.x(),cursorPosition.y());
#if QT_VERSION < 0x40000
      QWhatsThis::display(tipText, globalPosition,viewerWidget);
#else
      Q3WhatsThis::display(tipText,globalPosition,viewerWidget);
#endif
   } else if (!selectedObject) {
      QPoint globalPosition = QCursor::pos();
      QWidget *viewerWidget = thisViewer->GetCoinViewer()->getWidget();
      SoNode *node = p->getTail();
      if (node) {
         SbName name = node->getName();
         if (name.getLength()) {            
            QString tipText =  name.getString();
#if QT_VERSION < 0x40000
            QWhatsThis::display(tipText, globalPosition,viewerWidget);
#else
            Q3WhatsThis::display(tipText,globalPosition,viewerWidget);
#endif
         }
      }
   }
}

//______________________________________________________________________________
TQtCoinWidget::TQtCoinWidget(QWidget *parent, COINWIDGETFLAGSTYPE f)
#if QT_VERSION < 0x40000
      :QFrame(parent,0,f)
#else
      :QFrame(parent,f)
#endif      
   , TGLViewerImp(0,"",0,0)
   , fInventorViewer(0),fRootNode(0)
   , fShapeNode(0),fWiredShapeNode(0),fClippingShapeNode(0),fSolidShapeNode(0),fRawShapeNode(0)
   , fFileNode(0),fSelNode(0),fAnnotation(0),fFooterText(0),fCamera(0),fAxes(0)
   , fXAxis(0), fYAxis(0), fZAxis(0),fCameraSensor(0),fPickedObject(0)
   , fSaveType("JPEG"),fMaxSnapFileCounter(2),fSnapshotCounter(0),fPad(0),fContextMenu(0),fSelectedObject(0)
   , fWantRootContextMenu(kFALSE)
   //,fGLWidget(0),fSelectedView(0),fSelectedViewActive(kFALSE)
   //, fSelectionViewer(kFALSE),fSelectionHighlight(kFALSE),fShowSelectionGlobal(kFALSE)
   , fSnapShotAction(0),fBoxHighlightAction(0),fLineHighlightAction(0)
   , fWantClipPlane(kFALSE), fClipPlaneMan(0), fClipPlane(0), fSlicePlane(0),fHelpWidget(0),fRecord(kFALSE)
   , fMovie(0),fMPegMovie(0),fClipPlaneState(0),fClipPlanePath(0)
   , fKeyboardHandler(0)
{
      memset(fPivotClipPoint,0,sizeof(fPivotClipPoint));
      fMaxSnapFileCounter = CreateSnapShotCounter();
      printf("TQtCoinWidget::TQtCoinWidget %d end\n",fMaxSnapFileCounter);
}
//______________________________________________________________________________
void TQtCoinWidget::SetPad(TVirtualPad *pad)
{
   fPad = pad;
   if ( fPad ) {
	   printf("TQtCoinWidget::TQtCoinWidget begin Pad=%p\n", pad);
       //Create the default SnapShot file name and type if any
      const char *fileDir = gSystem->Getenv("SnapShotDirectory");
      if (!(fileDir  && fileDir[0]) && ( gEnv ) ) {
           fileDir  = gEnv->GetValue("Gui.SnapShotDirectory",(const char *)0);
      }
      QString saveFile;
      if (fileDir  && fileDir[0]) {  saveFile = fileDir; saveFile += "/"; }
      saveFile += fPad->GetName();
      saveFile += ".";
      saveFile += "jpg";
      SetFileType("JPG");
      SetFileName(saveFile);
            
      QString caption = fPad->GetTitle();
      caption += ": Coin viewer";
      fGLView = 0;
      CreateViewer(caption);
      SetDrawList(0);
   }
}
      
//______________________________________________________________________________
TQtCoinWidget::TQtCoinWidget(TVirtualPad *pad, const char *title,
                       UInt_t width, UInt_t height)
#if QT_VERSION < 0x40000
   : QFrame(0,"coinviewer", Qt::WDestructiveClose)
#else 
   : QFrame(0, Qt::WDestructiveClose)
#endif 
   , TGLViewerImp(0,title,width,height)
   , fInventorViewer(0),fRootNode(0)
   , fShapeNode(0),fWiredShapeNode(0),fClippingShapeNode(0),fSolidShapeNode(0),fRawShapeNode(0)
   , fFileNode(0),fSelNode(0),fAnnotation(0),fFooterText(0),fCamera(0),fAxes(0)
   , fXAxis(0), fYAxis(0), fZAxis(0),fCameraSensor(0),fPickedObject(0)
   , fSaveType("JPEG"),fMaxSnapFileCounter(2),fSnapshotCounter(0),fPad(pad),fContextMenu(0),fSelectedObject(0)
   , fWantRootContextMenu(kFALSE)
   //,fGLWidget(0),fSelectedView(0),fSelectedViewActive(kFALSE)
   //, fSelectionViewer(kFALSE),fSelectionHighlight(kFALSE),fShowSelectionGlobal(kFALSE)
   , fSnapShotAction(0),fBoxHighlightAction(0),fLineHighlightAction(0)
   , fWantClipPlane(kFALSE), fClipPlaneMan(0), fClipPlane(0), fSlicePlane(0),fHelpWidget(0),fRecord(kFALSE)
   , fMovie(0),fMPegMovie(0),fClipPlaneState(0),fClipPlanePath(0)
   , fKeyboardHandler(0)
{
   printf("TQtCoinWidget::TQtCoinWidget begin Pad=%p\n", pad);
   //Create the default SnapShot file name and type if any
   const char *fileDir = gSystem->Getenv("SnapShotDirectory");
   if (!(fileDir  && fileDir[0]) && ( gEnv ) ) {
        fileDir  = gEnv->GetValue("Gui.SnapShotDirectory",(const char *)0);
   }
   QString saveFile;
   memset(fPivotClipPoint,0,sizeof(fPivotClipPoint));
   if (fileDir  && fileDir[0]) {  saveFile = fileDir; saveFile += "/"; }
   if ( fPad ) {
      saveFile += fPad->GetName();
      saveFile += ".";
      saveFile += "jpg";
      
      SetFileType("JPG");
      SetFileName(saveFile);
     
      QString caption = fPad->GetTitle();
      caption += ": Coin viewer";
      setCaption(caption);
      resize(width, height);
      fGLView = 0;
      CreateViewer(title);
      SetDrawList(0);
   }
   fMaxSnapFileCounter = CreateSnapShotCounter();
   printf("TQtCoinWidget::TQtCoinWidget %d end\n",fMaxSnapFileCounter);
}
/*
//______________________________________________________________________________
TQtCoinWidget::TQtCoinWidget(TVirtualPad *pad, const char *title,
                       Int_t x, Int_t y, UInt_t width, UInt_t height)
	:TGLViewerImp(0,title,x,y,width,height)
	{
*/
/*
#if QT_VERSION < 0x40000
   : QMainWindow(0,"glviewer",Qt::WDestructiveClose | Qt::WRepaintNoErase | Qt:: WResizeNoErase )
#else 
   : Q3MainWindow(0,"glviewer",Qt::WDestructiveClose | Qt::WNoAutoErase | Qt:: WResizeNoErase )
#endif 
   , TGLViewerImp(0,title,x,y,width,height)
   , fSaveType("JPEG"),fMaxSnapFileCounter(2),fGLWidget(0),fPad(pad),fContextMenu(0),fSelectedView(0),fSelectedViewActive(kFALSE)
   ,fSelectionViewer(kFALSE),fSelectionHighlight(kFALSE),fShowSelectionGlobal(kFALSE),fWantRootContextMenu(kFALSE)
   , fSnapShotAction(0)
{
   if (fPad) {
      // Create the default SnapShot file name and type if any
      const char *fileDir = gSystem->Getenv("SnapShotDirectory");
      if (!(fileDir  && fileDir[0]) && ( gEnv ) ) {
         fileDir  = gEnv->GetValue("Gui.SnapShotDirectory",(const char *)0);
      }
      if (fileDir  && fileDir[0]) {  fSaveFile = fileDir; fSaveFile += "/"; }
      fSaveFile += fPad->GetName();
      fSaveFile += ".";
      fSaveFile += "jpg";
      QString caption = fPad->GetTitle();
      caption += ": OpenGL viewer";
      setCaption(caption);

      setGeometry(x, y, width, height);
      fGLView = 0;
      CreateViewer(title);
      MakeMenu();
      int parts[] = {43,7,10,39};
      CreateStatusBar(parts,4);
      SetDrawList(0);
      show();
   }
   fMaxSnapFileCounter = CreateSnapShotCounter();
   
}  

//______________________________________________________________________________
TQtGLViewerImp::TQtGLViewerImp(TQtGLViewerImp &parent) :
#if QT_VERSION < 0x40000
     QMainWindow(&parent,"glviewerutil", Qt::WDestructiveClose | Qt::WType_TopLevel)
#else 
     Q3MainWindow(&parent,"glviewerutil",Qt::WDestructiveClose | Qt::WType_TopLevel)
#endif 
   , TGLViewerImp(0,"selection",parent.width(),parent.height())
   , fSaveType(parent.fSaveType),fMaxSnapFileCounter(2),fGLWidget(0),fPad(parent.fPad),fContextMenu(0),fSelectedView(0),fSelectedViewActive(kFALSE)
   , fSelectionViewer(kTRUE),fSelectionHighlight(kFALSE),fShowSelectionGlobal(kFALSE),fWantRootContextMenu(kFALSE)
   , fSnapShotAction(0)
{
   // create a satelite widget
   connect(this,SIGNAL(destroyed() ),&parent, SLOT(DisconnectSelectorWidgetCB()));
   QString caption;
   if (fPad) {
      // Create the default SnapShot file name and type if any
      const char *fileDir = gSystem->Getenv("SnapShotDirectory");
      if (!(fileDir  && fileDir[0]) && ( gEnv ) ) {
         fileDir  = gEnv->GetValue("Gui.SnapShotDirectory",(const char *)0);
      }
      if (fileDir  && fileDir[0]) {  fSaveFile = fileDir; fSaveFile += "/"; }
      fSaveFile += fPad->GetName();
      fSaveFile += ".";
      fSaveFile += "jpg";
      caption = fPad->GetTitle();
   }
   fGLView = 0;
   caption += ": selection viewer";
   setCaption(caption);
   resize(2*parent.width()/3,2*parent.height()/3);
   CreateViewer(parent.GLWidget(),"selection");
   MakeMenu();
   int parts[] = {43,7,10,39};
   CreateStatusBar(parts,4);
   SetDrawList(0);
   show();
   fMaxSnapFileCounter = CreateSnapShotCounter();
}
*/
//______________________________________________________________________________
TQtCoinWidget::~TQtCoinWidget()
{ 
   if (fInventorViewer) { 
      SoQtExaminerViewer *viewer = (SoQtExaminerViewer*)fInventorViewer; 
      fInventorViewer = 0; delete viewer; 
   }
   if (fMPegMovie)    { delete fMPegMovie;      fMPegMovie     = 0;}
   if (fMovie)        { fMovie       ->unref(); fMovie         = 0;}
   if (fSlicePlane)   { fSlicePlane->unref();   fSlicePlane    = 0;}
   if (fClipPlaneMan) { fClipPlaneMan->unref(); fClipPlaneMan  = 0;}
   if (fClipPlanePath){ fClipPlanePath->unref();fClipPlanePath = 0;}
#ifdef NOEXAMINERVIEWER
   if (fAxes)         { fAxes        ->unref(); fAxes          = 0;}
#endif
   if (fXAxis)        { fXAxis       ->unref(); fXAxis         = 0;}
   if (fYAxis)        { fYAxis       ->unref(); fYAxis         = 0;}
   if (fZAxis)        { fZAxis       ->unref(); fZAxis         = 0;}
   delete fBoxHighlightAction; fBoxHighlightAction  = 0;
   delete fLineHighlightAction;fLineHighlightAction = 0;
   delete fCameraSensor;       fCameraSensor        = 0;
   if (fRootNode)     { fRootNode    ->unref(); fRootNode      = 0;}
}

//______________________________________________________________________________
void TQtCoinWidget::AddRootChild(ULong_t id, EObject3DType type)
{
   assert(id);
   //fRootNode->addChild(new SoCone);
   switch (type) {
       case TGLViewerImp::kSolid:
          //fShapeNode->addChild((SoNode*)id);
          fSolidShapeNode->addChild((SoNode*)id);
          // printf("TQtCoinWidget::AddRootChild------------SOLID----------  <===\n");
          break;
       case TGLViewerImp::kWired:
          fWiredShapeNode->addChild((SoNode*)id);
          // printf("TQtCoinWidget::AddRootChild------------WIRED----------  <===\n");
          break;
       case TGLViewerImp::kRaw:
          fRawShapeNode->addChild((SoNode*)id);          
          break;
       default:
          fSolidShapeNode->addChild((SoNode*)id);
          // printf("TQtCoinWidget::AddRootChild------------DEFAULT----------  <===\n");
          break;              
    };
   
   // Make myCamera see everything.
  // vf --  ViewAll();
}
//______________________________________________________________________________
void TQtCoinWidget::ViewAll()
{
   // Make myCamera see everything.
   if (GetCamera()) {
      GetCamera()->viewAll(fRootNode, fInventorViewer->getViewportRegion());
   }
}

//______________________________________________________________________________
void TQtCoinWidget::ClearCB()
{
   if (fFileNode)    fFileNode->removeAllChildren();
   if (fRawShapeNode)fRawShapeNode->removeAllChildren(); ;	
   Clear();
}

//______________________________________________________________________________
void TQtCoinWidget::Clear(const char *opt)
{
   if (opt) {}
	//fShapeNode->removeAllChildren();
	fSolidShapeNode->removeAllChildren();
   fWiredShapeNode->removeAllChildren();
   /*
	fInventorViewer->setSceneGraph(NULL);
	fInventorViewer->setSceneGraph(NULL);
	fRootNode->unref();
	fRootNode = NULL;
	*/
	/*
     // Clear (remove) all objects from the view
   QWidget *c = centralWidget();
   if (!c) return;
   TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
   glView->clearGLList();
   Update();
	*/
}
/*
//______________________________________________________________________________
void TQtGLViewerImp::CreateStatusBar(Int_t nparts)
{
 //  Create the status bar with "nparts" separate portions
  QStatusBar *thisStatusBar = statusBar();
  Int_t i=0;
  fStatusBar.resize(nparts);
  for (i=0;i<nparts;i++) {
    QLabel *l = new QLabel(thisStatusBar);
    thisStatusBar->addWidget(l,1,TRUE);
    // remember to delete later
    fStatusBar.insert(i,l);  
  }
}
*/

//______________________________________________________________________________
void TQtCoinWidget::CreateStatusBar(Int_t *parts, Int_t nparts)
{ 
    // Dummy for this class 
   if (parts && nparts) {}
}

//______________________________________________________________________________
int TQtCoinWidget::CreateSnapShotCounter()
{
   // Define the number of the snapshot frames based on either
   // "SnapShotFileCounter" environment variable
   //    use: 
   //          setenv SnapShotFileCounter 100
   // to produce 100  different frames in raw
   // or the ROOT parameters (via ".rootrc" file) 
   //
   //   Gui.SnapShotFileCounter 100
   //
   const char *dcounter = gSystem->Getenv("SnapShotFileCounter");
   if (!(dcounter && dcounter[0]) && ( gEnv ) ) {
        dcounter  = gEnv->GetValue("Gui.SnapShotFileCounter","10");
   }
   if (dcounter && dcounter[0]) {
      int count = QString(dcounter).toInt();
      gfDefaultMaxSnapFileCounter = count;      
   }
   return gfDefaultMaxSnapFileCounter;
}

//______________________________________________________________________________
TContextMenu &TQtCoinWidget::ContextMenu() 
{
   // Create the TConextMenu if needed and return it
   if (!fContextMenu) {
        fContextMenu = new TContextMenu("3DViewContextMenu");
   }
   return *fContextMenu;
}

/*
//______________________________________________________________________________
void TQtGLViewerImp::DisconnectSelectorWidgetCB()
{
   // [slot] to disconnect the destroyed  "selection" widget
  fSelectedView = 0; 
}

//______________________________________________________________________________
void TQtGLViewerImp::SetStatusText(const char *text, Int_t partidx, Int_t stype)
{  
  // Set Text into the 'npart'-th part of the status bar
  if (Int_t(fStatusBar.size()) > partidx) {
    if (stype >=  0) {
       TColor *rootColor = gROOT->GetColor(stype);
       float rgb[3];
       rootColor->GetRGB(rgb[0],rgb[1],rgb[2]);
       fStatusBar[partidx]->setPaletteForegroundColor(QColor(int(255*rgb[0]),int(255*rgb[1]),int(255*rgb[2])));
    }
    fStatusBar[partidx]->setText(text);
  }
}
//______________________________________________________________________________
void TQtGLViewerImp::ShowStatusBar(Bool_t show)
{
   // Show / Hide the status bar
   
  if (show) statusBar()->show();
  else      statusBar()->hide();
}
*/
//______________________________________________________________________________
 void  TQtCoinWidget::DisconnectPad()
 {
    fPad = 0;
 }
//______________________________________________________________________________
void TQtCoinWidget::SetUpdatesEnabled(bool enable)
{   
   setUpdatesEnabled(enable);  
   if (fInventorViewer) fInventorViewer->setAutoRedraw(enable);
}

//______________________________________________________________________________
bool TQtCoinWidget::GetUpdatesEnabled() const
{   
   // This method sets whether redrawing should be handled automatically 
   // or not when data in the scenegraph changes
   return 
      fInventorViewer ? fInventorViewer->isAutoRedraw()  
                      : isUpdatesEnabled();                        
}

//______________________________________________________________________________
TVirtualPad *TQtCoinWidget::GetPad() 
{
   // This method returns whether redrawing is handled automatically not
   if (GetGLView()) return GetGLView()->GetPad();
   return fPad;
}
/*
//______________________________________________________________________________
//
// Slots
//______________________________________________________________________________
//______________________________________________________________________________
void TQtGLViewerImp::ActivateSelectorWidgetCB(bool on)
{
     // Active the separate window to show the selected objects
    fSelectedViewActive =  on;
    CreateSelectionViewer();
}
//______________________________________________________________________________
void TQtGLViewerImp::ActivateSelectionHighlighCB(bool on)
{
     // Active the separate window to show the selected objects
    fSelectionHighlight  =  on;
}
//______________________________________________________________________________
void TQtGLViewerImp::ActivateSelectionGlobalCB(bool on)
{
     // Show the selected object on the global coordinate system
    fShowSelectionGlobal =  on;
}
*/
//______________________________________________________________________________
void TQtCoinWidget::AddGLList(unsigned int list, EObject3DType type)
{
   printf("TQtCoinWidget::AddGLList\n");
   
   flist[type].push_back(list);
	/*
   QWidget *c = centralWidget();
   if (!c) return;   
#ifdef QGLVIEWER
     TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
     switch (type) {
     case 0:
        glView->addGLList(list, TQtGLViewerWidget::kWired);
        break;
     case 1: default:
        glView->addGLList(list, TQtGLViewerWidget::kSolid);
        break;
     case 2:
        glView->addGLList(list, TQtGLViewerWidget::kSelecting);
        break;
     };
#endif
	*/
}

//______________________________________________________________________________
void TQtCoinWidget::RemoveGLList(unsigned int list)
{
	printf("TQtCoinWidget::RemoveGLList %d\n",list);
	/*
   QWidget *c = centralWidget();
   if (!c) return;   
#ifdef QGLVIEWER
     TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
     glView->removeGLList(list);
#endif
	*/
}
/*
//______________________________________________________________________________
void TQtGLViewerImp::NewViewer(){
  new TQtGLViewerImp(GetGLView());

*/
//______________________________________________________________________________
void TQtCoinWidget::PrintCB(){
	/*
   QPrinter p;
   QWidget *c = centralWidget();
   if (c && p.setup()) {
      QPainter pnt(&p);
        QGLWidget *glView = (QGLWidget *)c;
        if (glView) pnt.drawImage(0,0,glView->grabFrameBuffer());
      pnt.end();
   }
	*/	
	SoWriteAction 	writeAction;
	writeAction.apply(fInventorViewer->getSceneManager()->getSceneGraph()); // fRootNode //writes the entire scene graph to stdout
}

//______________________________________________________________________________
void TQtCoinWidget::CopyCB()
{
   //  Copy the current 3d view to the system clipboard 
   QGLWidget *w = (QGLWidget *)GetCoinViewer()->getGLWidget();
   if (w) {
      QClipboard *cb = QApplication::clipboard();
      cb->setImage(w->grabFrameBuffer(TRUE));
   }
}
//______________________________________________________________________________
void TQtCoinWidget::CopyFrameCB()
{	
   // Copy the entire window including the menu and the status bar
   QClipboard *cb = QApplication::clipboard();
   cb->setPixmap(QPixmap::grabWidget(topLevelWidget()));	
}
//______________________________________________________________________________
void TQtCoinWidget::ReadInputFile(const char *fileName)
{
   // Read in the external scene in the "OpenInventor" format
   ReadInputFile(QString( fileName));
}

//______________________________________________________________________________
void TQtCoinWidget::ReadInputFile(QString fileName)
{ 	
   // Read in the external scene in the "OpenInventor" format
    QFileInfo info(fileName);
    SoInput viewDecor;
    if (info.isReadable() ) {
       QString saveWorkingDir = QDir::currentDirPath();
       TString ivDir = (const char*)info.dirPath();
       gSystem->ExpandPathName(ivDir);
       QDir::setCurrent((const char*)ivDir);
       if ( viewDecor.openFile(info.fileName() ) ) {
          SoSeparator *extraObjects = SoDB::readAll(&viewDecor);
          if (extraObjects) {
	           printf("readings ... %s from %s\n", (const char *)info.fileName(), (const char*)info.dirPath());
              if (!fFileNode) {
                 fFileNode = new SoSeparator();
                 fShapeNode->addChild(fFileNode);
              }
              fFileNode->addChild(extraObjects);
          }
       }
       QDir::setCurrent(saveWorkingDir);
    }
}
#if 0
//______________________________________________________________________________
static QString ListOfFilters() 
{
  QString a = "";
  SoOffscreenRenderer r(*(new SbViewportRegion));
  int num = r.getNumWriteFiletypes();

 if (num == 0) {
    (void)fprintf(stdout,
                   "No image formats supported by the "
                  "SoOffscreenRenderer except SGI RGB and Postscript.\n");
 } else {
    for (int i=0; i < num; i++) {
       SbPList extlist;
       SbString fullname, description;
       r.getWriteFiletypeInfo(i, extlist, fullname, description);
       if (i > 0) a+= ";";
       a += fullname.getString(); 
//       (void)fprintf(stdout, "%s: %s (extension%s: ",
//                      fullname.getString(), description.getString(),
//                      extlist.getLength() > 1 ? "s" : "");
         a+= " ( " ;
         for (int j=0; j < extlist.getLength(); j++) {
            if (j>0) a+= ", "; a+= "*."; a+=(const char*) extlist[j];
//            (void)fprintf(stdout, "%s%s", j>0 ? ", " : "", (const char*) extlist[j]);
         }
         a += " );";
//         (void)fprintf(stdout, ")\n");
    }
  }
  return a;
}
//______________________________________________________________________________
static QStringList ExtensionList(const QString &filter)
{
   // Return the list of the extension from the file dialog filter
   QRegExp rx("(\\*\\.\\w+\\b)");
   QStringList extension;
   int pos = 0;
   while ( pos >= 0 ) {
      pos = rx.search(filter,pos);
      if ( pos > -1 ) {
         extension += rx.cap(1);
         pos  += rx.matchedLength();
      }
   }   
   return extension ;
}
#endif
//______________________________________________________________________________
void TQtCoinWidget::Save(QString fileName,QString type)
{ 
   if (fileName.isEmpty()) return;
   
   QString &thatFile  = fileName;
   QString &e = type;
   printf("thatFile = %s  type=%s\n", thatFile.data(), e.data());
   
   if (!Recording()) {
      SetFileName(thatFile);
      SetFileType(e);
   }
   
   if (e == "rgb") {
      SoOffscreenRenderer osr(fInventorViewer->getViewportRegion());
      osr.setComponents(SoOffscreenRenderer::RGB);
      SbBool ok = osr.render(fShapeNode);
      if (ok) ok = osr.writeToRGB(thatFile);
   } else if (e == "rgbt") {
      SoOffscreenRenderer osr(fInventorViewer->getViewportRegion());
      osr.setComponents(SoOffscreenRenderer::RGB_TRANSPARENCY);      
      SbBool ok = osr.render(fShapeNode);
      if (ok) ok = osr.writeToRGB(thatFile);
   } else if (e == "wrl") {
      printf("Converting...\n");
      SoToVRML2Action tovrml2;
      tovrml2.apply(fShapeNode);
      SoVRMLGroup *newroot = tovrml2.getVRML2SceneGraph();
      newroot->ref();
      printf("Writing...\n");
      SoOutput out;
      out.openFile(thatFile);
      out.setHeaderString("#VRML V2.0 utf8");
      SoWriteAction wra(&out);
      wra.apply(newroot);
      out.closeFile();	
      newroot->unref();
   } else if (e == "ps" || e=="eps") {
         //*
#if 0      
      int printerDPI = 400;
      const SbViewportRegion &vp  = fInventorViewer->getViewportRegion();
      const SbVec2s &imagePixSize = vp.getViewportSizePixels();
      SbVec2f imageInches;
      float pixPerInch;

      pixPerInch = SoOffscreenRenderer::getScreenPixelsPerInch();
      imageInches.setValue((float)imagePixSize[0] / pixPerInch,
                           (float)imagePixSize[1] / pixPerInch);

      // The resolution to render the scene for the printer
      // is equal to the size of the image in inches times
      // the printer DPI;
      SbVec2s postScriptRes;
      postScriptRes.setValue((short)(imageInches[0])*printerDPI,
                             (short)(imageInches[1])*printerDPI);

            // Create a viewport to render the scene into.
      SbViewportRegion myViewport;
      myViewport.setWindowSize(postScriptRes);
      myViewport.setPixelsPerInch((float)printerDPI);

      // Render the scene
      SoOffscreenRenderer myRenderer(myViewport);
      if (myRenderer.render(fShapeNode)) 
          myRenderer.writeToPostScript(thatFile);
      //*/	
#else
     SoVectorizePSAction *va = new SoVectorizePSAction;
     if (true) {
        va->setGouraudThreshold(0.1f);
     } else {
        va->setGouraudThreshold(0.0f);
     }
     va->setBackgroundColor(TRUE, SbColor(1.0f, 1.0f, 1.0f));
     SoVectorOutput * out = va->getOutput();
     if (!out->openFile((const char*)thatFile)) {
       fprintf(stderr,"Unable to open %s for writing\n",
              (const char*)thatFile);
     }
     SbVec2s vpsize = fInventorViewer->getViewportRegion().getViewportSizePixels();
     float vpratio = ((float)vpsize[0]) / ((float)vpsize[1]);

     //
     if (vpratio > 1.0f) {
        va->setOrientation(SoVectorizeAction::LANDSCAPE);
        vpratio = 1.0f / vpratio;
     } else {
        va->setOrientation(SoVectorizeAction::PORTRAIT);
     }
     
     const float BORDER = 10.0f;
     
//     va->beginStandardPage(SoVectorizeAction::A4, BORDER);
     va->beginPage (SbVec2f(0,0), SbVec2f(8,10), SoVectorizeAction::INCH);
      
     // try to fill as much "paper" as possible

     // FIXME: consider making getPageSize() public
     //SbVec2f size = va->getPageSize();
     
//     SbVec2f size = SbVec2f(210.0f - BORDER*2.0f,
//                            297.0f - BORDER*2.0f);

     SbVec2f size = SbVec2f(203.0f - BORDER*2.0f,
                            279.0f - BORDER*2.0f);
     
     float pageratio = size[0] / size[1];
     float xsize, ysize;

     if (pageratio < vpratio) {
        xsize = size[0];
        ysize = xsize / vpratio;
     } else {
       ysize = size[1];
       xsize = ysize * vpratio;
     }

     float offx = BORDER + (size[0]-xsize) * 0.5f;
     float offy = BORDER + (size[1]-ysize) * 0.5f;

     va->beginViewport(SbVec2f(offx, offy), SbVec2f(xsize, ysize));
     va->calibrate(fInventorViewer->getViewportRegion() );

     fprintf(stdout,"Vectorizing...");
     fflush(stdout);

     va->apply(fInventorViewer->getSceneManager()->getSceneGraph());
     fprintf(stdout,"Creating postscript file (%s)...", (const char*)thatFile);
     fflush(stdout);
     va->endViewport();
     va->endPage();
     out->closeFile();

     fprintf(stdout,"done\n");
     fflush(stdout);
#endif      
   } else if (e == "iv") {
      printf("Saving in iv format ...\n");
      SoWriteAction myAction;
      myAction.getOutput()->openFile(thatFile);
      myAction.getOutput()->setBinary(FALSE);
      myAction.apply(fShapeNode);
      myAction.getOutput()->closeFile();
   } else if (e == "mpg") {
      SnapShotSaveCB(TRUE);
   } else {
        QGLWidget *w = (QGLWidget *)GetCoinViewer()->getGLWidget();
        if (w) {
           QImage im =  w->grabFrameBuffer(TRUE); 
           if ( e.lower() == "gif") {
              // Save animated GIF via TImage (to avoid the "Sofwatre patent" issue
              Int_t saver = gErrorIgnoreLevel;
              gErrorIgnoreLevel = kFatal;
              TImage *img = TImage::Create();
              if (img) {
                 QPixmap finalPixmap(im);
                 img->SetImage(Pixmap_t(TGQt::rootwid(&finalPixmap)),0);
                 QString gifFile = thatFile;
//                 if ( Recording() ) 
                 gifFile += '+';
                 img->WriteImage((const char*)gifFile, 
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,0)            
                    TImage::kAnimGif);
//                    Recording() ? TImage::kAnimGif:TImage::kGif);
#else            
                    TImage::kGif);
#endif
                 static QString GIF("GIF");
                 EmitImageSaved(thatFile, GIF,fSnapshotCounter);
                 delete img;
              }
              gErrorIgnoreLevel = saver;
           } else {
              im.save(thatFile,e.upper().replace("JPG","JPEG"));
              EmitImageSaved(thatFile
                 , e.upper().replace("JPG","JPEG")
                 , fSnapshotCounter);
           }
        }
   }
}
//______________________________________________________________________________
void TQtCoinWidget::SaveAsCB()
{ 
/*
   QWidget *c = centralWidget();
   if (!c) return;
#ifndef QGLVIEWER
   QString filter = 
     "Image (";

   QString defExtension[] = {"bmp","C"};

   UInt_t i;
   for (i = 0; i < QImageIO::outputFormats().count(); i++ ) 
   {
      if (i) filter +=',';
      filter += "*.";
      QString str = QString( QImageIO::outputFormats().at( i ) );
      filter += str.lower();
   }
   filter +=");;";

   QString selectedFilter;

#if QT_VERSION < 0x40000
   QString thatFile = QFileDialog::getSaveFileName(gSystem->WorkingDirectory()
#else 
   QString thatFile = Q3FileDialog::getSaveFileName(gSystem->WorkingDirectory()
#endif 
    , filter, centralWidget(), "SaveAs"
    , "Save the current x3d view as"
    , &selectedFilter);

   if (thatFile.isEmpty()) return;
 
   UInt_t nExt = sizeof(defExtension)/sizeof(const char *);
   QString e;
   for (i = 0; i < nExt-1; i++) {
     e = '.'; e += defExtension[i];
     if (selectedFilter.contains(e)) break;
   }

   if (! thatFile.contains('.'))  thatFile += '.';
   if (thatFile.at(thatFile.length()-1) == '.')  thatFile += defExtension[i];

   fSaveFile = (const char *)thatFile;
   QGLWidget *glView = (QGLWidget *)c;
   glView-> grabFrameBuffer().save(fSaveFile.Data(),fSaveType.Data());
#else
     TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
     glView->saveSnapshot(false);
     fSaveFile = (const char *)glView->snapshotFileName( );
#endif
*/
};
//______________________________________________________________________________
void TQtCoinWidget::SaveMpegShot(bool)      
{
     // Save the frame in mpg format
   assert(fMPegMovie);
   QGLWidget *w = (QGLWidget *)GetCoinViewer()->getGLWidget();
   if (w) {
      QImage im = w->grabFrameBuffer(TRUE); 
      if (im.size()  != QSize( fMPegMovie->Width(), fMPegMovie->Height())) {
        // All frames of the mpeg file must be one and the same size
        fMPegMovie->Close();
        fMPegMovie->Open(im.size().width(),im.size().height()); 
        fMPegMovie->SetMovie(); // Re-use the previous file name
        printf(" new size %d %d \n", im.size().width(),im.size().height());
      }
      fMPegMovie->AddFrame(im.bits());       
   }
}
//______________________________________________________________________________
void TQtCoinWidget::SetSnapshotCounter(int counter)
{
   // reset the current counter
   fSnapshotCounter = counter;
}
//______________________________________________________________________________
void TQtCoinWidget::EmitImageSaved(QString &fileName,QString &fileType, int frameCounter)
{
   emit ImageSaved(fileName,fileType,frameCounter);
}

//______________________________________________________________________________
void TQtCoinWidget::SaveSnapShot(bool on)
{
   const QString &saveType = SaveType();
   printf("\n TQtCoinWidget::SaveSnapShot %s\n", (const char*)saveType);
   if ( (saveType.lower() == "mpg") || (saveType.lower() == "mpeg")) {
      SaveMpegShot(on);  
   } else {
#if ROOT_VERSION_CODE >= ROOT_VERSION(5,13,0)    
      if (saveType.upper() == "GIF") {
         Save(SaveFile(),saveType.upper());
      } else 
#endif
      {
         if (fMaxSnapFileCounter != 0) fSnapshotCounter=fSnapshotCounter%fMaxSnapFileCounter;
         Save(QString().sprintf((const char*)SaveFilePattern(),fSnapshotCounter++),saveType.upper());
      }
   }
   
	/*
#ifdef QGLVIEWER
//  QString filename(fSaveFile.Data());
//  glView->saveSnapshot("STAR",true);
    if (fSaveFile.IsNull()) {
       SaveAsCB();
    } else {
       // raise the widget
       QWidget *c = centralWidget();
       TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
       TVirtualPad *thisPad= GetPad();
       QString footer(thisPad->GetTitle());
       glView->setFooter(footer);
       
       // raise();
       
       glView->saveSnapshot(true,true);
       if (fMaxSnapFileCounter <= 2) {
          SaveHtml(glView->snapshotCounter()-1);
          float x,y,z;
          if (glView->RotationAxisAngle(x,y,z)< 0.02) {
             CopyFile(QString(fSaveFile.Data()),glView->snapshotCounter()-1);
          }
       }
       if (glView->snapshotCounter() ==  fMaxSnapFileCounter) glView->setSnapshotCounter(0);
   }
#endif
*/
}

/*
//______________________________________________________________________________
void TQtGLViewerImp::SelectEventCB(bool on)
{
    // Toggle the widget the "wried object selecction mode
   QWidget *c = centralWidget();
   TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
   if (glView) glView->SetWiredSelectable(on);
}
//______________________________________________________________________________
void TQtGLViewerImp::SelectDetectorCB(bool on)
{
   // Toggle the widget the "wried object selecction mode
   QWidget *c = centralWidget();
   TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
   if (glView) glView->SetSolidSelectable(on);
}
*/

//______________________________________________________________________________
void TQtCoinWidget::SnapShotSaveCB(bool on)
{  
	fRecord = on;
   if (on) {
       fRootNode->addChild(fMovie);
      if ( (SaveType().lower() == "mpg") || (SaveType().lower() == "mpeg")) {
         if (!fMPegMovie) fMPegMovie = new TSimageMovie();
         else fMPegMovie->Open();
         fMPegMovie->SetMovie(SaveFile());
      } 
   } else {
      if (fMPegMovie) fMPegMovie->Close();
      fRootNode->removeChild(fMovie);
   }
         /*
   QWidget *c = centralWidget();
   TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
   // Adjust the menu indicator if any
   if (fSnapShotAction) {
      bool snapStatus = fSnapShotAction->isOn();
      if (snapStatus != on ) {
         blockSignals (true);
         fSnapShotAction->setOn(on);
         blockSignals (false);
      }
   }
   if (on) {
         // fGLWidget->resize(720, 576); // PAL DV format (use 720x480 for NTSC DV)
       raise();
       connect(glView, SIGNAL(drawFinished(bool)), this, SLOT(SaveSnapShot(bool)));
    } else {
       disconnect(glView, SIGNAL(drawFinished(bool)), this, SLOT(SaveSnapShot(bool)));
    }
	*/
}

//______________________________________________________________________________
void TQtCoinWidget::SynchTPadCB(bool on)
{  
    //SetPadSynchronize(on);
   if (on) {}
}

//______________________________________________________________________________
void TQtCoinWidget::ShowFrameAxisCB(bool on)
{  
   if (on) {
      if (!fXAxis) { fXAxis = new TCoinAxisSeparator; fXAxis->ref(); }
      
      SoGetBoundingBoxAction ba(fInventorViewer->getViewportRegion());
      ba.apply(fShapeNode);
   
      SbBox3f box = ba.getBoundingBox();
      double amin = -1;   double amax = +1;
      // check range to avoid the crash before any geometry added to the viewer
      if ( (TMath::Abs(box.getMin()[0])< 1.0E+37) &&  (TMath::Abs(box.getMin()[0]) < 1.0E+37))  {
         amin = box.getMin()[0]; amax = box.getMax()[0];
      }
      double aminopt;   double amaxopt; double width ;int ndivopt;
      THLimitsFinder::Optimize(amin,amax,15,aminopt,amaxopt,ndivopt,width);
      fXAxis->SetRange(aminopt, amaxopt ,ndivopt);
      fXAxis->Connect(fShapeNode);
   } else {
      if (fXAxis) fXAxis->Disconnect(fShapeNode);
   }
/*
#ifdef QGLVIEWER
   QWidget *c = centralWidget();
   TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
   if (on) 
      glView->SetFrameAxisFactor(5.0);
   else 
      glView->SetFrameAxisFactor(-1);
   glView->update();
#endif
   */
}

//______________________________________________________________________________
void TQtCoinWidget::ShowLightsCB(bool on)
{  
   if (on) {}
/*
#ifdef QGLVIEWER
   QWidget *c = centralWidget();
   TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
   glView->setDrawLight(on);glView->update();
#endif
	*/
}

//______________________________________________________________________________
void TQtCoinWidget::WantRootContextMenuCB(bool on)
{
  // Create "ROOT Context menu" otherwise use the QGLViewer "right mouse click"
  fWantRootContextMenu = on; 
}

//______________________________________________________________________________
void TQtCoinWidget::AboutCB()
{ 
   QMessageBox::aboutQt(0);
   QString rootVersion = "ROOT ";
   rootVersion += gROOT->GetVersion();
   rootVersion += "with Qt interface";
   QMessageBox::about(0,rootVersion,"ROOT Qt interface Coin Viewer 2006 Zubanov Alexei AZubanov@gmail.com");
}

//______________________________________________________________________________
void TQtCoinWidget::HelpCB()
{
	    // Gives infos on current event
    QString msg;
    
    
    msg += "<b>hold down left mousebutton</b> and move mouse pointer to <b>rotate</b> ";
    msg += "the camera around it's current focal point"; 
    msg += "(the focal point can be changed by doing a seek operation)";
    msg += "<P>";

    msg += "<b>hold middle mousebutton</b> to <b>pan</b>";
    msg += " (or a CTRL-key plus left mousebutton, or a SHIFT-key plus left mousebutton)";
    msg += "<P>";

    msg += "<b>hold down left + middle mousebutton</b> to <b>zoom / dolly</b>, ";
    msg += "or CTRL + middle mousebutton, or CTRL + SHIFT + the left mousebutton";
    msg += "<P>";

    msg += "<b>click \'s\'</b>, then <b>pick</b> with the left mousebutton to seek";
    msg += "<P>";

    msg += "<b>right mousebutton</b> opens the <b>popup menu</b>";
    msg += "<P>";

    msg += "<b>click \'ESC\' key</b> to switch to and from \'camera interaction\' mode";
    msg += " and \'scenegraph interaction\' mode (see setViewing() documentation)";
    
//    msg += "<P>";
//    msg += "<b>\'q\'</b> quits the application"; 
//    msg += "<br>";

    msg += "<hr>";
    msg += "The SoQtExaminerViewer provides a user decoration\'s button";
    msg += " for toggling between <b>orthographic</b> or <b>perspective</b> camera view volumes and projection methods.";
    msg += "This is the <b>bottom-most</b> click button on the right decoration border.";
    msg += "<hr>";
    msg += "It also inherits the decoration buttons from the SoQtFullViewer:";
    msg += "<br>";
    msg += "<b>the arrow</b> for switching to \"scenegraph interaction\" mode,";
    msg += "<br>";
    msg += "<b>the hand</b> for setting back to \"camera interaction\" mode,";
    msg += "<br>";
    msg += "<b>the house</b> for \"reset camera to home position\",";
    msg += "<br>";
    msg += "<b>the blueprint house</b> for \"set new camera home position\",";
    msg += "<br>";
    msg += "<b>the eye</b> for \"zoom camera out to view full scene\","; 
    msg += "<br>";
    msg += "<b>the flashlight</b> for setting \"click to seek\" mode.";
    msg += "<hr>";
    msg += "See: <a href=\"http://doc.coin3d.org/SoQt\">http://doc.coin3d.org/SoQt </a> and";
    msg += "<P>";
    msg += "<a href=\"http://doc.coin3d.org/SoQt/classSoQtExaminerViewer.html\">";
    msg += "http://doc.coin3d.org/SoQt/classSoQtExaminerViewer.html</a> also";

    QMessageBox::information(0,"Infos on mouse and decoration buttons"
                              ,msg,QMessageBox::Ok);
}
#ifdef NOEXAMINERVIEWER
//______________________________________________________________________________
static void cameraChangeCB(void *data, SoSensor *)
{
   // Callback  that reports whenever the viewer's orientation change
  TQtCoinWidget *viewer = (TQtCoinWidget *) data;
  const SoCamera *camera = viewer->GetCamera();
  if (camera) {
     SmAxisDisplayKit *axis = viewer->GetAxis();
     if (axis) {
        axis->orientation = camera->orientation;
     }
  }
}
#endif 
//______________________________________________________________________________
void TQtCoinWidget::CreateViewer(const char * /*name*/)
{
   connect(this, SIGNAL(ObjectSelected(TObject*, const QPoint &)), &this->Signals(), SIGNAL(ObjectSelected(TObject *, const QPoint&)));
   if ( !fgCoinInitialized) { 
      SoQt::init(this); 
      SoHardCopy::init();
 #ifdef __SMALLCHANGE__ 
      smallchange_init(); 
#endif
     fgCoinInitialized = kTRUE;
   }
    // 
    // fInventorViewer
    //      | 
    //  fRootNode ---+
    //               | fSelNode
    //               |----------+
    //               |          |  fShapeNode
    //               |          |-------------+
    //               | SelectCB               | ShapeHints
    //               |---------               |------------
    //               |                        |
    //               |DeselectCB              |  fileNode
    //               |----------              |------------
    //               |
    //               |PickFilterCB    
    //               |---------- 
    //               |
    //               | MovieCB    
    //               |---------- 
    //               |
    //               | fAxes    
    //               |---------- 
    //               |
    //               | fAnnotation    
    //               |------------- 
    //                           |
    //                           |  camera
    //                           |----------
    //                           |
    //                           |  fFooterText
    //                           |-------------
    // 
   //OverlayHighlightRenderAction::initClass();	
	//*
   fRootNode = new SoSeparator;fRootNode->ref();
   fRootNode->setName("RootNode");
   if (!fCamera) {
    	fCamera = new SoPerspectiveCamera;
      fRootNode->addChild(fCamera);	
    }

   fSelNode = new SoSelection;
   fSelNode->setName("SelectionNode");
   fRootNode->addChild(fSelNode);

   fShapeNode = new SoSeparator;
   fShapeNode->setName("ShapesNode");
   fSelNode->addChild(fShapeNode);
   
 
   fWiredShapeNode = new SoSeparator;
   fWiredShapeNode->setName("WiredShapes");
   fShapeNode->addChild(fWiredShapeNode);
   
   fClippingShapeNode = new SoSeparator;
   fClippingShapeNode->setName("ClippingShapes");
   fShapeNode->addChild(fClippingShapeNode);
   
   fRawShapeNode = new SoSeparator;
   fRawShapeNode->setName("RawShapes");
   fClippingShapeNode->addChild(fRawShapeNode);

   fSolidShapeNode = new SoSeparator;
   fSolidShapeNode->setName("SolidShapes");
   fClippingShapeNode->addChild(fSolidShapeNode);
   
 // ---------------------------------------------------------------------
 // void SoSeparator::setNumRenderCaches ( const int  howmany ) [static] 
 // ---------------------------------------------------------------------
 // Set the maximum number of caches that SoSeparator nodes may allocate
 // for render caching. This is a global value which will be used for all
 // SoSeparator nodes, but the value indicate the maximum number per
 // SoSeparator node. More caches might give better performance, but will
 //  use more memory. The built-in default value is 2.
 //
 // The value can also be changed globally by setting the host system's 
 // environment variable IV_SEPARATOR_MAX_CACHES to the wanted number.
 // This is primarily meant as an aid during debugging, to make it easy to 
 // turn off rendercaching completely (by setting "IV_SEPARATOR_MAX_CACHES=0")
 // without having to change any application code.
 //

	SoShapeHints * sh = new SoShapeHints;
	sh->vertexOrdering = SoShapeHints::CLOCKWISE;
	sh->shapeType      = SoShapeHints::SOLID;
	sh->faceType       = SoShapeHints::CONVEX;
   //sh->shapeType = SoShapeHints::UNKNOWN_SHAPE_TYPE;
	//sh->faceType = SoShapeHints::UNKNOWN_FACE_TYPE;
	fSolidShapeNode->addChild(sh);	
    
	//*/
   //fRootNode = new SoSelection;

   //SoMaterial *myMaterial = new SoMaterial;
   //fRootNode->addChild(myCamera);
   //fRootNode->addChild(new SoDirectionalLight);
   //myMaterial->diffuseColor.setValue(1.0, 0.0, 0.0);   // Red
   //fRootNode->addChild(myMaterial);
   //fRootNode->addChild(new SoCone);
   //SoSeparator * fWNode = new SoSeparator;

	// old GLFactory
	/*
   SoCallback * cb1 = new SoCallback;
   SoCallback * cb2 = new SoCallback;
   SoCallback * cb3 = new SoCallback;
   cb1->setCallback(InventorCallback1, this);
   cb2->setCallback(InventorCallback2, this);
   cb3->setCallback(InventorCallback3, this);

   SoSeparator * s1 = new SoSeparator;
   SoSeparator * s2 = new SoSeparator;
   SoSeparator * s3 = new SoSeparator;
   s1->addChild(cb1);
   //s2->addChild(new SoShape);
   s2->addChild(cb2);
   s3->addChild(cb3);

   fRootNode->addChild(s1);		// wired
   fRootNode->addChild(s2);		// solid
   fRootNode->addChild(s3);		// selecting
   
   SoDrawStyle *wireStyle;
   wireStyle = new SoDrawStyle;
   wireStyle->style = SoDrawStyle::LINES;
   s1->insertChild(wireStyle, 0);
	*/
   /*
      QMainWindow       *fMainWidget;
      fMainWidget = new QMainWindow(0,"Coin3dViewer",Qt::WDestructiveClose|Qt::WType_TopLevel);
      //connect(fMainWidget,SIGNAL(destroyed()),this,SLOT(Disconnect()));
      QFrame *glFrame = new QFrame(fMainWidget);
      fMainWidget->setCentralWidget (glFrame);
      qt_viewer = new SoQtExaminerViewer((QWidget*)glFrame);   
      qt_viewer->setSceneGraph(fRootNode);
      setCentralWidget(fInventorViewer);
      */
   
   
   fSelNode->addSelectionCallback(SelectCB, this); 		//!
   fSelNode->addDeselectionCallback(DeselectCB, this); 		//!
   fSelNode->setPickFilterCallback(PickFilterCB, this); 		//!
   
   QVBoxLayout *l  = new QVBoxLayout(this);
   fInventorViewer = new SoQtExaminerViewer(this);
   // Decorate it with the extra buttons
   // add X, Y, Z viewpoint buttons
   //  X-button
   SoQtFullViewer *fullViewer = (SoQtFullViewer *)fInventorViewer;
   QWidget *buttonParent = fullViewer->getAppPushButtonParent();

   QPushButton *button = new QPushButton(buttonParent);
   button->setFocusPolicy(QWidget::NoFocus);
   button->setPixmap(QPixmap((const char **) x_xpm));
   QObject::connect(button, SIGNAL(clicked()),
                    this, SLOT(ViewPlaneX()));
   QToolTip::add(button,"View the YZ plane");
   fullViewer->addAppPushButton(button);
   //  Y-button
   button = new QPushButton(buttonParent);
   button->setFocusPolicy(QWidget::NoFocus);
   button->setPixmap(QPixmap((const char **) y_xpm));
   QObject::connect(button, SIGNAL(clicked()),
                    this, SLOT(ViewPlaneY()));
   QToolTip::add(button,"View the XZ plane");
   fullViewer->addAppPushButton(button);

   //  Z-button
   button = new QPushButton(buttonParent);
   button->setFocusPolicy(QWidget::NoFocus);
   button->setPixmap(QPixmap((const char **) z_xpm));
   QObject::connect(button, SIGNAL(clicked()),
                    this, SLOT(ViewPlaneZ()));
   QToolTip::add(button,"View the XY plane");
   fullViewer->addAppPushButton(button);

    // Clip Plane buttons;

   //  X-button
   button = new QPushButton(buttonParent);
   button->setName("z");
   button->setFocusPolicy(QWidget::NoFocus);
   button->setPixmap(QPixmap((const char **) xc_xpm));
   QObject::connect(button, SIGNAL(clicked()),
                    this, SLOT(SetClipPlaneXCB()));
   QToolTip::add(button,"Clip the YZ plane");
   fullViewer->addAppPushButton(button);
   //  Y-button
   button = new QPushButton(buttonParent);
   button->setName("y");
   button->setFocusPolicy(QWidget::NoFocus);
   button->setPixmap(QPixmap((const char **) yc_xpm));
   QObject::connect(button, SIGNAL(clicked()),
                    this, SLOT(SetClipPlaneYCB()));
   QToolTip::add(button,"Clip the XZ plane");
   fullViewer->addAppPushButton(button);

   //  Z-button
   button = new QPushButton(buttonParent);
   button->setName("z");
   button->setFocusPolicy(QWidget::NoFocus);
   button->setPixmap(QPixmap((const char **) zc_xpm));
   QObject::connect(button, SIGNAL(clicked()),
                    this, SLOT(SetClipPlaneZCB()));
   QToolTip::add(button,"Clip the XY plane");
   fullViewer->addAppPushButton(button);

   //  Plane-switch-button
   fClipPlaneState = new QCheckBox(buttonParent);
   fClipPlaneState->setFocusPolicy(QWidget::NoFocus);
   fClipPlaneState->setTristate(true);
   fClipPlaneState->setNoChange();
   // button->setPixmap(QPixmap((const char **) zc_xpm));
   QObject::connect(fClipPlaneState, SIGNAL(stateChanged(int)),
                    this, SLOT(ClipPlaneModeCB(int)));
   QToolTip::add(fClipPlaneState,"Switch between edit/clip/no clip views");
   fullViewer->addAppPushButton(fClipPlaneState);


   //  Slice-switch-button
#ifdef SLICEPLANEBUTTON
   // suspend this feature for the time being
   button = new QPushButton(buttonParent);
   button->setName("slice");
   button->setFocusPolicy(QWidget::NoFocus);
   button->setPixmap(QPixmap((const char **) zc_xpm));
   QObject::connect(button, SIGNAL(clicked()),
                    this, SLOT(SetSlicePlaneCB()));
   QToolTip::add(button,"Slice plane");
   fullViewer->addAppPushButton(button);
#endif
   l->addWidget(fInventorViewer->getWidget());
   fInventorViewer->setSceneGraph(fRootNode);
   fInventorViewer->setTransparencyType(SoGLRenderAction::NONE);

   fCamera = fInventorViewer->getCamera(); 
   fMovie  = new SoCallback;
//    SoSeparator *mv    = new SoSeparator;
   fMovie->setCallback(MovieCallback, this);
   fMovie->ref();
   
   //  Pick the background color from pad
   SetBackgroundColor(fPad->GetFillColor());
//   SetFooter("Test Two lines\nSecond lines");
#if 1
   // Create the keyborad handler
   fKeyboardHandler = new SoEventCallback();

   fKeyboardHandler->addEventCallback(
         SoKeyboardEvent::getClassTypeId()
       , ViewerKeyPressCB,this);
  // fRootNode->insertChild(fKeyboardHandler,0);
  fRootNode->addChild(fKeyboardHandler);
#endif
   connect(this,SIGNAL(ObjectSelected(TObject*,const QPoint &)),this,SLOT( ShowObjectInfo(TObject *, const QPoint&)));
}

//______________________________________________________________________________
SoCamera *TQtCoinWidget::GetCamera() const 
{ 
   return fInventorViewer->getCamera();
} 
//______________________________________________________________________________
SoCamera &TQtCoinWidget::Camera() const
{
   return *GetCamera();
}
//______________________________________________________________________________
void TQtCoinWidget::EmitNodeSelectSignal(SoNode *node)
{
   static QPoint mousePosition;
   
   fSelectedObject = 0;
   
   mousePosition = fInventorViewer->getWidget()->mapFromGlobal ( QCursor::pos());
   emit NodeSelected(ULong_t(node), mousePosition);
}

//______________________________________________________________________________
void TQtCoinWidget::EmitSelectSignal(TObject *obj)
{
   static QPoint mousePosition;

   mousePosition = fInventorViewer->getWidget()->mapFromGlobal ( QCursor::pos());
   if (obj) {
      fSelectedObject = obj;
      if (obj) {
         // fprintf(stderr,"\tTQtCoinWidget::EmitSelectSignal view = %p, obj = %p; obj name %s \n", view, obj, (const char*)obj->GetName());
         emit ObjectSelected(obj,  mousePosition);
      }      
   }
}

//______________________________________________________________________________
void TQtCoinWidget::SetBoxSelection()
{
 // BoxHighlightAction().setTransparencyType(fInventorViewer->getTransparencyType());
 // fInventorViewer->setGLRenderAction( &BoxHighlightAction());
}

//______________________________________________________________________________
void TQtCoinWidget::SetLineSelection()
{
   LineHighlightAction().setTransparencyType(fInventorViewer->getTransparencyType());
	fInventorViewer->setGLRenderAction( &LineHighlightAction());
}

/*
//______________________________________________________________________________
void TQtGLViewerImp::CreateViewer(QGLWidget *share, const char *name)
{
   // Create satellite viewe to share the same GL lists
   if (!fGLWidget) {
      fGLWidget = new TQtGLViewerWidget( this,name,share );
      TQtGLViewerWidget *glw = (TQtGLViewerWidget *)fGLWidget;
#if 0      
      glw->setSceneCenter(((TQtGLViewerWidget *)share)->sceneCenter());
      glw->setSceneRadius(((TQtGLViewerWidget *)share)->sceneRadius());
      glw->setBackgroundColor(((TQtGLViewerWidget *)share)->backgroundColor());
#endif      
      glw->makeCurrent();
      glw->setSceneCenter(((TQtGLViewerWidget *)share)->sceneCenter());
      glw->setSceneRadius(((TQtGLViewerWidget *)share)->sceneRadius());
      glw->setBackgroundColor(((TQtGLViewerWidget *)share)->backgroundColor());

      setCentralWidget (fGLWidget);
      glw->setSnapshotFileName  (fSaveFile.Data());
      connect(fGLWidget,SIGNAL(objectSelected(TObject*,const QPoint &)),this,SLOT( ShowObjectInfo(TObject *, const QPoint&)));
   }
}
//______________________________________________________________________________
void TQtGLViewerImp::MakeCurrent() 
{
#ifdef QGLVIEWER
   fGLWidget->makeCurrent();
#endif
}
*/
/*
//______________________________________________________________________________
void TQtGLViewerImp::Paint(Option_t *opt)
{
   CreateViewer();
#ifndef QGLVIEWER
   TGLViewerImp::Paint(opt);
#else
   if (opt);
   Update();
#endif

  //// Paint G3D objects  
  // Int_t myGLIst = GetGLView()->GetGLList()-1;
  // assert(myGLIst );
  // if (myGLIst) {

  //   // Pick the current TPad rotation
  //   gVirtualGL->RunGLList(myGLIst + TPadOpenGLView::kView);

  //   // Draw G3D objects
  //   gVirtualGL->RunGLList(myGLIst + TPadOpenGLView::kModel);
  // }
}
*/
//______________________________________________________________________________
void TQtCoinWidget::Update()
{  
	fRootNode->touch();
   fInventorViewer->render();
	/*
   CreateViewer();
#ifdef QGLVIEWER
   if (fGLWidget && !fSelectionViewer) 
      ((TQtGLViewerWidget*)fGLWidget)->SetSceneLayout();
#endif
   TGLViewerImp::Update();
   centralWidget()->update();
   */
}

/*
//______________________________________________________________________________
void TQtCoinWidget::SetPadSynchronize(Bool_t on)
{
  // Define whether the GL Widget view should follow TPad rotation
#ifndef QGLVIEWER
   if(on) {} // QtGLWidget provide no synch means
#else
   if (fGLWidget) ((TQtGLViewerWidget*)fGLWidget)->setPadSynchronize(on);
#endif
}
*/
//______________________________________________________________________________
void TQtCoinWidget::SetFileName(const QString &fileName)
{
   fSaveFile = fileName;
   // Set the file pattern
   QFileInfo  fi(fSaveFile);
   fSaveFileMoviePattern = 
            fi.dirPath()+"/" + fi.baseName(TRUE)+ "-%04d" + "." + fi.extension(FALSE);
   printf(" TQtCoinWidget::SetFileName %s\n", (const char *)fSaveFileMoviePattern);
}  
//______________________________________________________________________________
void TQtCoinWidget::SetFileType(const QString &fileType)
{
   fSaveType =  fileType;
}
     
//______________________________________________________________________________
void TQtCoinWidget::SetFooter(const char *text)
{ 
   QString f(text);
   SetFooter(f);
}

//______________________________________________________________________________
void TQtCoinWidget::SetFooter(QString &text)
{
   // Set the footer text
  if (fInventorViewer) 
  {
     if (!fAnnotation) {
        SoSeparator *s = new SoSeparator();
        // Add annotation in front of fMovie if any
        int indx = -1;
        if (fMovie) indx = fRootNode->findChild(fMovie);
        if (indx >0)
           fRootNode->insertChild(s,indx-1);
        else 
           fRootNode->addChild(s);
        fAnnotation = new SoAnnotation();
        s->addChild(fAnnotation);
        SoCamera *acamera =  new SoOrthographicCamera();
        fAnnotation->addChild(acamera);
        SoTranslation *translate = new SoTranslation();
        fAnnotation->addChild(translate);
        translate->translation = SbVec3f(0,-0.95,0);
     }
     if (!fFooterText) {
        fFooterText = new SoText2();
        ((SoText2*)fFooterText)->justification =SoText2::CENTER; 
        fAnnotation->addChild(fFooterText);        
     }
     QStringList list = QStringList::split(QChar('\n'),text);
     int i =0;
     for ( QStringList::Iterator it = list.begin(); it != list.end(); ++it ) 
     {
       ((SoText2*)fFooterText)->string.set1Value(i++,(*it).ascii());
     }

  }
}
//______________________________________________________________________________
void TQtCoinWidget::SetFullScreenView(bool on)
{
   //   Set the full screen view
   if (fInventorViewer) 
      fInventorViewer->setFullScreen(on);
}
      
//______________________________________________________________________________
bool TQtCoinWidget::IsFullScreen( )       const
{
   // return the current view mode
   return fInventorViewer?fInventorViewer->isFullScreen(): false;
   
}
//______________________________________________________________________________
void TQtCoinWidget::SetRotationAxisAngle(const float x, const float y, const float z, const float a)
{
	if (x + y + z + a >0) { RotateCamera(2,a);}
   /*
   // Set the current rotation of the frame. 
   // Parameters are the rotation axis vector and its angle (in radians). 
#ifdef QGLVIEWER
  if (fGLWidget) ((TQtGLViewerWidget*)fGLWidget)->setRotationAxisAngle(x,y,z,a);
#endif
	*/
}
//______________________________________________________________________________
void TQtCoinWidget::StartRecordingCB(bool on)
{
  fRecord = on;
}

//______________________________________________________________________________
void TQtCoinWidget::StopRecordingCB(bool on)
{
  if (on) {} 
  StartRecordingCB (kFALSE);
}

//______________________________________________________________________________
void TQtCoinWidget::SetBackgroundColor(Color_t color)
{
   TColor *background = gROOT->GetColor(color);
   if (background) {
     float rgb[3];
     background->GetRGB(rgb[0],rgb[1],rgb[2]);
     fInventorViewer->setBackgroundColor(SbColor(rgb));
   }  
}

//______________________________________________________________________________
void TQtCoinWidget::ShowObjectInfo(TObject *obj, const QPoint &cursorPosition)
{
  if (obj) {
	/*
   //  Qt Slot to accept the signal emiited by the GL Widget selecting the ROOT object
   static QRect previousTipArea;
   QString tipText = obj->GetObjectInfo(cursorPosition.x(),cursorPosition.y());
   
   SetStatusText(obj->GetName(), 0);
   Color_t objectColor = -1;
   if (obj->InheritsFrom("TAttLine")) {
      TAttLine *lAttr = dynamic_cast<TAttLine *>(obj);
      if (lAttr)  objectColor =  lAttr->GetLineColor();
   }
   SetStatusText(QString("%1,%2").arg(cursorPosition.x()).arg(cursorPosition.y()), 1,objectColor);
   SetStatusText(obj->ClassName(), 2);
   SetStatusText(tipText, 3);
   // Create a tooltip

   TQtGLViewerWidget *tipped = (TQtGLViewerWidget *)sender();
   QPoint globalPosition = tipped->mapToGlobal(cursorPosition);
#if QT_VERSION < 0x40000
   QWhatsThis::display(tipText, globalPosition,tipped);
#else 
   Q3WhatsThis::display(tipText, globalPosition,tipped);
#endif 

   TObject3DView *view = (TObject3DView *)tipped->selectedName();

   // open a separate window to show the selection   
   if (fSelectedViewActive) {      
      if (!fSelectedView) 
         CreateSelectionViewer();
      else 
         fSelectedView->Clear();
      fSelectedView->setCaption(tipText);
      ULong_t id = 0;
      if (fShowSelectionGlobal) {
          if (!view->GetViewId(TObject3DViewFactoryABC::kSelected)) view->CompileSelection();
          id = view->GetViewId(TObject3DViewFactoryABC::kSelected);
      } else { 
          id = view->GetViewId();
      }
      fSelectedView->AddGLList(id, view->IsSolid() );
//      fSelectedView->Update();
      fSelectedView->update();
   }
   if (fSelectionHighlight) {
      // remove the previouse selection
      tipped->clearGLList(TQtGLViewerWidget::kSelected);
      // Hightlight the new one
      if (!view->GetViewId(TObject3DViewFactoryABC::kSelected)) view->CompileSelection();
      tipped->addGLList(view->GetViewId(TObject3DViewFactoryABC::kSelected), TQtGLViewerWidget::kSelected);
   }
   */
   // Display the context menu if any
      if (fWantRootContextMenu) {
      // Popup the context menu
         QPoint pos = fInventorViewer->getWidget()->mapToGlobal(cursorPosition);
         ContextMenu().Popup(pos.x(),pos.y(), obj,(TBrowser *)0);
      }
   }
}
/*
//______________________________________________________________________________
void TQtGLViewerImp::CreateSelectionViewer( ) 
{
   // Create another OpenGL viewer to show the selection 
   if (fSelectedViewActive && !fSelectedView)  {
      fSelectedView  = new TQtGLViewerImp(*this);
      fSelectedView->SelectEventCB(false);
      fSelectedView->SelectDetectorCB(false);
   }
}
*/
//______________________________________________________________________________
ULong_t TQtCoinWidget::GetViewerID() const
{
   return ULong_t((QFrame *) this);
}

//______________________________________________________________________________
void TQtCoinWidget::SetSnapFileCounter(int counter)
{ 
	fMaxSnapFileCounter = counter;  
}
//_______________________________________________________________________________
void TQtCoinWidget::SetClipPlane(SoClipPlane *plane, int planeDirection)
{
   if (plane) {
      SbVec3f normal;
      switch (planeDirection) {
         case 0: normal.setValue(1,0,0); break;
         case 1: normal.setValue(0,1,0); break;
         case 2: normal.setValue(0,0,1); break;
      };
      plane->plane.setValue(SbPlane(normal,SbVec3f( fPivotClipPoint[0]
                                                   ,fPivotClipPoint[1]
                                                   ,fPivotClipPoint[2]
                                                   )));
   }
}
//_______________________________________________________________________________
void TQtCoinWidget::SetClipPlaneXCB() 
{
  // Set the clip plane orthogonal to oX
  SetActiveClipPlane(0);
}
//_______________________________________________________________________________
void TQtCoinWidget::SetClipPlaneYCB() 
{
  // Set the clip plane orthogonal to oY
  SetActiveClipPlane(1);
}

//_______________________________________________________________________________
void TQtCoinWidget::SetClipPlaneZCB() 
{
  // Set the clip plane orthogonal to oZ
  SetActiveClipPlane(2);
}
//_______________________________________________________________________________
void TQtCoinWidget::SetSlicePlaneCB()
{
   // Switch to Off state
   if (fClipPlaneState->state() != QButton::Off) {
      fClipPlaneState->setChecked(false);
      ClipPlaneModeCB(QButton::Off);
   }
    SbVec3d normal =   fClipPlane->plane.getValue().getNormal();
   // Invert normal
   normal *= -1;
   float distance =   fClipPlane->plane.getValue().getDistanceFromOrigin();
   // increase the distance
   distance = TMath::Abs(distance - 10);
   fSlicePlane->plane = SbPlane(normal,distance);
   fSlicePlane->on = TRUE;
}

//_______________________________________________________________________________
void TQtCoinWidget::SetActiveClipPlane(int planeDirection)
{
   if (fClipPlaneState->state() != QButton::Off) 
   {
      fClipPlaneState->blockSignals(true);
      if (fClipPlaneState->state() == QButton::On) {
         // Recreate the path
         if (fClipPlanePath) {
            fClipPlanePath->unref(); 
            fClipPlanePath = 0;
         }
         if (fClipPlaneMan) {
            fClippingShapeNode->removeChild(fClipPlaneMan);
            // fShapeNode->removeChild(fClipPlaneMan);
            fClipPlaneMan->unref(); fClipPlaneMan = 0;
         }
      } else {
         fClipPlaneState->setChecked(true);
      }
      switch (planeDirection) {
         case 0: SetClipPlaneMan(kTRUE,1,0,0); break;
         case 1: SetClipPlaneMan(kTRUE,0,1,0); break;
         case 2: SetClipPlaneMan(kTRUE,0,0,1); break;
      };      
      fClipPlaneState->blockSignals(false);
   } else {
      if (fClipPlane)  {
         SetClipPlane( fClipPlane, planeDirection);
         if (fSlicePlane && fSlicePlane->on.getValue() ) SetSlicePlaneCB();
      }
   }
}

//______________________________________________________________________________
void TQtCoinWidget::SetClipPlaneMan(bool on, float x, float y, float z)
{
   if (on) {
     if (!fClipPlanePath) {
        // fprintf(stderr," TQtCoinWidget::SetClipPlaneMan \n",on);
        // int wiredIndx = fSolidShapeNode ? fShapeNode->findChild(fSolidShapeNode):0;
        fClipPlaneMan = new SoClipPlaneManip();       
        fClipPlaneMan->ref();
        SoGetBoundingBoxAction ba(fInventorViewer->getViewportRegion());
        ba.apply(fShapeNode);
   
        SbBox3f box = ba.getBoundingBox();
        box.getCenter().getValue( fPivotClipPoint[0]
                                 ,fPivotClipPoint[1]
                                 ,fPivotClipPoint[2]
                                 );

        fClipPlaneMan->setValue(box, SbVec3f(x, y, z), 1.02f);
        // construct the clip plane path
//        fClipPlanePath = new SoPath(fShapeNode);
        fClipPlanePath = new SoPath(fClippingShapeNode);
        fClipPlanePath->ref();
#ifdef SLICEPLANEBUTTON
        if ((!fSlicePlane) && false)  // suspend this feature for the time being
        {
           fSlicePlane = new SoClipPlane(); 
           fSlicePlane->on = FALSE; 
           fSlicePlane->ref();
           // fShapeNode->insertChild(fSlicePlane, wiredIndx==-1 ? 0 : wiredIndx );
           fClippingShapeNode->insertChild(fSlicePlane, wiredIndx==-1 ? 0 : wiredIndx );
        }
#endif
//        fShapeNode->insertChild(fClipPlaneMan, fShapeNode->findChild(fSlicePlane)+1);
        fClippingShapeNode->insertChild(fClipPlaneMan, fClippingShapeNode->findChild(fSlicePlane)+1);
        fClipPlanePath->append(fClipPlaneMan);
     } else if ((fClipPlaneMan->getRefCount() == 1) || true ) { // FIX ME LATER !!!
         fClipPlaneMan->replaceNode(fClipPlanePath);
     }
     if (fSlicePlane && fSlicePlane->on.getValue()) 
                                        fSlicePlane  ->on = FALSE;
     if (!fClipPlaneMan->on.getValue()) fClipPlaneMan->on = TRUE;

   } else if (fClipPlanePath && ((fClipPlaneMan->getRefCount() > 1)|| true) ) { // FIX ME LATER !!!
          fClipPlaneMan->replaceManip(fClipPlanePath, fClipPlane = new SoClipPlane());
         if (fSlicePlane && fSlicePlane && fSlicePlane->on.getValue())
              fSlicePlane->on = FALSE;
   }
}

//______________________________________________________________________________
void TQtCoinWidget::FrameAxisActionCB(bool on)
{
  SetClipPlaneMan(on); 
}
//______________________________________________________________________________
void TQtCoinWidget::ViewPlaneX() const
{
  SoCamera * const camera = GetCamera();
  if (! camera) return; // probably a scene-less viewer

  SbVec3f dir;
  camera->orientation.getValue().multVec(SbVec3f(0, 0, -1), dir);
  SbVec3f focalpoint  = camera->position.getValue()
                      + camera->focalDistance.getValue() * dir;
  camera->position    = focalpoint
                      + camera->focalDistance.getValue() * SbVec3f(1, 0, 0);
  camera->orientation = SbRotation(SbVec3f(0, 1, 0), float(M_PI) / 2.0f);
}

//______________________________________________________________________________
void TQtCoinWidget::ViewPlaneY() const
{
  SoCamera * const camera = GetCamera();
  if (! camera) return; // probably a scene-less viewer

  SbVec3f dir;
  camera->orientation.getValue().multVec(SbVec3f(0, 0, -1), dir);
  SbVec3f focalpoint  = camera->position.getValue()
                      + camera->focalDistance.getValue() * dir;
  camera->position    = focalpoint
                      + camera->focalDistance.getValue() * SbVec3f(0, 1, 0);
  camera->orientation = SbRotation(SbVec3f(1, 0, 0), -float(M_PI) / 2.0f);
}
//______________________________________________________________________________
void  TQtCoinWidget::RotateCamera(int axis,bool clockWise)
{
   RotateCamera(axis, float(clockWise ? 0.5l : -0.5l ));
}

//______________________________________________________________________________
void TQtCoinWidget::RotateCamera(int axis,float angle)
{
  SoCamera * const camera = GetCamera();
  if (! camera) return; // probably a scene-less viewer

  SbVec3f dir;
  SbRotation cameraRotation = camera->orientation.getValue();
  cameraRotation.multVec(SbVec3f(0, 0, -1), dir);
  SbVec3f focalpoint  = camera->position.getValue()
                      + camera->focalDistance.getValue() * dir;
  SbVec3f rotAxis(0,0,0);
  switch (axis) {
   case 0:
      rotAxis.setValue(1,0,0);
      break;
   case 1:
      rotAxis.setValue(0,1,0);
      break;
   case 2:
      rotAxis.setValue(0,0,1);
      break;
   default: assert(0);
  };

  SbRotation neworient(rotAxis, angle );
  SbVec3f position = camera->position.getValue() - focalpoint;
  neworient.multVec(position,position);
  camera->position    = position;

  neworient.multVec(dir,dir);
  float oldAngle;
  cameraRotation.getValue(dir,oldAngle);
  cameraRotation.setValue(dir,oldAngle+angle);
  camera->orientation.setValue(cameraRotation);

//  camera->position    = focalpoint 
//                      + camera->focalDistance.getValue() * SbVec3f(0, 1, 0);
//  camera->orientation = SbRotation(SbVec3f(1, 0, 0), angle );
}

//______________________________________________________________________________
void TQtCoinWidget::ViewPlaneZ() const
{
  SoCamera * const camera = GetCamera();
  if (! camera) return; // probably a scene-less viewer

  SbVec3f dir;
  camera->orientation.getValue().multVec(SbVec3f(0, 0, -1), dir);
  SbVec3f focalpoint  = camera->position.getValue()
                      + camera->focalDistance.getValue() * dir;
  camera->position    = focalpoint
                      + camera->focalDistance.getValue() * SbVec3f(0, 0, 1);
  camera->orientation = SbRotation(SbVec3f(0, 1, 0), 0);
}

//______________________________________________________________________________
void TQtCoinWidget::ClipPlaneModeCB(int mode)
{
   switch (mode) {
       case QButton::Off:
          // Remove the clip plane manipulator
          SetClipPlaneMan(false);
          break;
       case QButton::On:
          // Set the clip plane manipulator
          SetClipPlaneMan(true);
          break;
       case QButton::NoChange:
       default:
          // Remove the clip plane manipulator if present
          if (fClipPlaneMan->getRefCount() > 1 ) SetClipPlaneMan(false);
          // and disable the cliplane
          if (fSlicePlane && fSlicePlane->on.getValue()) fSlicePlane->on = FALSE;
          if (fClipPlane-> on.getValue()) fClipPlane->on  = FALSE;
          break;
   };
}

//______________________________________________________________________________
void TQtCoinWidget::SmallAxesActionCB(bool on)
{
#ifndef NOEXAMINERVIEWER
    ((SoQtExaminerViewer*)fInventorViewer)->setFeedbackVisibility(on);
#else
    if (on) {
#ifdef __SMALLCHANGE__ 
       if (!fAxes) { 
          fAxes =  new SmAxisDisplayKit();
          fAxes->ref();
          
          SmAxisDisplayKit *ax = fAxes;
          // axis sizes
          ax->axes.set1Value(0, 20.,  0.,  0.);
          ax->axes.set1Value(1,  0., 20.,  0.);
          ax->axes.set1Value(2,  1.,  0., 20.);
   
          // axis colors
          ax->colors.set1Value( 0, 1., 0., 0.);
          ax->colors.set1Value( 1, 0., 1., 0.);
          ax->colors.set1Value( 2, 1., 0., 1.);
   
          // axis labels
          ax-> annotations.set1Value(0,"x");
          ax-> annotations.set1Value(1,"y");
          ax-> annotations.set1Value(2,"z");
   
          if (!fCameraSensor) 
             fCameraSensor = new SoFieldSensor(cameraChangeCB,this);
       }
       //  Adjust the initial orientation if any
       fAxes->orientation = Camera().orientation;
       fCameraSensor->attach(&Camera().orientation);
       fRootNode->addChild(fAxes);
   } else if (fAxes) {
      fRootNode->removeChild(fAxes);
      fCameraSensor->detach();
   }
#endif
#endif
}
