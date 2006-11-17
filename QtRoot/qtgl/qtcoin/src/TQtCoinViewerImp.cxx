#include "TVirtualPad.h"
#include "TContextMenu.h"

#include "TQVirtualGL.h"
#include "TQPadOpenGLView.h"
#include "TQtCoinWidget.h"

#define QGLVIEWER

#include "TQtCoinViewerImp.h"
//#include "TQtGLViewerWidget.h"
#include "TObject3DView.h"

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

#if QT_VERSION < 0x40000
#  include <qfiledialog.h>
#  include <qpopupmenu.h>
#  include <qwhatsthis.h> 
#  include <qaction.h>
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

#ifdef WIN32
#  ifdef CopyFile
#     undef CopyFile
#  endif
#endif


#include "TObject3DView.h"

//=============

//#include "TQtCoinViewerImp.h"
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
#include <Inventor/nodes/SoMaterial.h>
#include <Inventor/nodes/SoDrawStyle.h>
#include <Inventor/nodes/SoTranslation.h>
#include <Inventor/nodes/SoCamera.h>
#include <Inventor/nodes/SoPerspectiveCamera.h>
#include <Inventor/nodes/SoPickStyle.h>
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
#include <SmallChange/nodekits/SmAxisDisplayKit.h>
#include <SmallChange/nodekits/SmAxisKit.h>
#include <SmallChange/misc/Init.h>
#include <Inventor/sensors/SoFieldSensor.h> 

#include <vector>
#include "TRootGLU.h"


/*
//______________________________________________________________________________
TQtCoinViewerImp::TQtCoinViewerImp(TPadOpenGLView *pad, const char *title,
                       UInt_t width, UInt_t height)
	: TGLViewerImp(pad,title,width,height)
{	
if QT_VERSION < 0x40000
   : QMainWindow(0,"glviewer", Qt::WDestructiveClose)
#else 
   : Q3MainWindow(0,"glviewer", Qt::WDestructiveClose)
#endif 
   , TGLViewerImp(pad,title,width,height)
   , fSaveType("JPEG"),fMaxSnapFileCounter(2),fGLWidget(0),fPad(0),fContextMenu(0),fSelectedView(0),fSelectedViewActive(kFALSE)
   , fSelectionViewer(kFALSE),fSelectionHighlight(kFALSE),fShowSelectionGlobal(kFALSE),fWantRootContextMenu(kFALSE)
   , fSnapShotAction(0)
 {
   if (pad) {
      TVirtualPad *thisPad = pad->GetPad();
      if (thisPad) {

         // Create the default SnapShot file name and type if any
         const char *fileDir = gSystem->Getenv("SnapShotDirectory");
         if (!(fileDir  && fileDir[0]) && ( gEnv ) ) {
             fileDir  = gEnv->GetValue("Gui.SnapShotDirectory",(const char *)0);
         }
         if (fileDir  && fileDir[0]) {  fSaveFile = fileDir; fSaveFile += "/"; }

         fSaveFile += thisPad->GetName();
         fSaveFile += ".";
         fSaveFile += "jpg";
         QString caption = thisPad->GetTitle();
         caption += ": OpenGL viewer";
         setCaption(caption);
         resize(width, height);
         fGLView = pad;
         CreateViewer(title);
         MakeMenu();
         int parts[] = {43,7,10,39};
         CreateStatusBar(parts,4);
         SetDrawList(0);
         ShowStatusBar();
         show();
      }
   }
   fMaxSnapFileCounter = CreateSnapShotCounter();

}
*/
/*
//______________________________________________________________________________
TQtGLViewerImp::TQtGLViewerImp(TPadOpenGLView *pad, const char *title,
                       Int_t x, Int_t y, UInt_t width, UInt_t height)

}
*/
//______________________________________________________________________________
TQtCoinViewerImp::TQtCoinViewerImp(TVirtualPad *pad, const char *title,
                       UInt_t width, UInt_t height)
#if QT_VERSION < 0x40000
   : QMainWindow(0,"coinviewer", Qt::WDestructiveClose)
#else 
   : Q3MainWindow(0,"coinviewer", Qt::WDestructiveClose)
#endif 
   , TGLViewerImp(0,title,width,height),fCoinWidget(0)
   , fSnapShotAction(0)
   //,fGLWidget(0),fSelectedView(0),fSelectedViewActive(kFALSE)
   //, fSelectionViewer(kFALSE),fSelectionHighlight(kFALSE),fShowSelectionGlobal(kFALSE)
{
   if (pad ) {
	   printf("TQtCoinViewerImp::TQtCoinViewerImp begin Pad=%p\n", pad);
       //Create the default SnapShot file name and type if any
      
      QString caption = pad->GetTitle();
      caption += ": Coin viewer";
      setCaption(caption);
      resize(width, height);
      fGLView = 0;
      CreateViewer(title);      
      fCoinWidget ->SetPad(pad);
      int parts[] = {43,7,10,39};
      CreateStatusBar(parts,4);
      MakeMenu();
      SetDrawList(0);
      show();
   }
   // fMaxSnapFileCounter = 2;//CreateSnapShotCounter();
   printf("TQtCoinViewerImp::TQtCoinViewerImp end\n");
}
/*
//______________________________________________________________________________
TQtCoinViewerImp::TQtCoinViewerImp(TVirtualPad *pad, const char *title,
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
TQtCoinViewerImp::~TQtCoinViewerImp()
{ }

//______________________________________________________________________________
void TQtCoinViewerImp::AddRootChild(ULong_t id)
{ 
   if (fCoinWidget) fCoinWidget->AddRootChild(id);
}

//______________________________________________________________________________
void TQtCoinViewerImp::ClearCB()
{
	Clear();
}

//______________________________________________________________________________
void TQtCoinViewerImp::Clear()
{
   if (fCoinWidget) fCoinWidget->Clear();
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
void TQtCoinViewerImp::CreateStatusBar(Int_t * /*parts*/, Int_t /*nparts*/ )
{
  //  Create the status bar with "nparts" separate portions
  // The initial relative size of each part is defined by "parts" array
/*
  QStatusBar *thisStatusBar = statusBar();
#ifdef WIN32
  thisStatusBar->setSizeGripEnabled(FALSE);
#endif
  // Any number of widgets may be controlled by just
  // one splitter
  QSplitter *split = new QSplitter(thisStatusBar);
  thisStatusBar->addWidget(split,1,FALSE);

  fStatusBar.resize(nparts);
  Int_t iField=0;
  for (iField=0; iField<nparts; iField++) {
    QLabel *infoBox = new QLabel(split);
    infoBox->setIndent(3);
    QSize s = infoBox->size();
    s.setWidth(parts[iField]);
    infoBox->resize(s);

    // remember to delete later
    fStatusBar.insert(iField,infoBox);
  }
  */
}
/*
//______________________________________________________________________________
int TQtCoinViewerImp::CreateSnapShotCounter()
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
        dcounter  = gEnv->GetValue("Gui.SnapShotFileCounter","2");
   }
   if (dcounter && dcounter[0]) {
      int count = QString(dcounter).toInt();
      if (count > 2 )  gfDefaultMaxSnapFileCounter = count;
   }
   return gfDefaultMaxSnapFileCounter;
}
*/
//______________________________________________________________________________
TContextMenu &TQtCoinViewerImp::ContextMenu() 
{
   // Create the TContextMeny if needed and return it
   return  fCoinWidget ? fCoinWidget->ContextMenu() : *(TContextMenu *)0;
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
 void  TQtCoinViewerImp::DisconnectPad()
 {
    if (fCoinWidget) fCoinWidget->DisconnectPad();
 }
//______________________________________________________________________________
void TQtCoinViewerImp::SetUpdatesEnabled(const bool&enable)
{  
    setUpdatesEnabled(enable);                            
    if (fCoinWidget) fCoinWidget->SetUpdatesEnabled(enable);
}
//______________________________________________________________________________
TVirtualPad *TQtCoinViewerImp::GetPad() 
{
   if (GetGLView()) return GetGLView()->GetPad();
   return  fCoinWidget ? fCoinWidget->GetPad() : 0;
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
void TQtCoinViewerImp::AddGLList(unsigned int list, int type)
{
   printf("TQtCoinViewerImp::AddGLList\n");
   if (fCoinWidget) fCoinWidget->AddGLList(list, type);
}

//______________________________________________________________________________
void TQtCoinViewerImp::RemoveGLList(unsigned int list)
{
	printf("TQtCoinViewerImp::RemoveGLList\n");
   if (fCoinWidget) fCoinWidget->RemoveGLList(list);
}
/*
//______________________________________________________________________________
void TQtGLViewerImp::NewViewer(){
  new TQtGLViewerImp(GetGLView());

*/
//______________________________________________________________________________
void TQtCoinViewerImp::PrintCB()
{
   if (fCoinWidget) fCoinWidget->PrintCB();
}

//______________________________________________________________________________
void TQtCoinViewerImp::CopyCB()
{
   //  Copy the current 3d view to the system clipboard 
   if (fCoinWidget)  fCoinWidget->CopyCB();
}
//______________________________________________________________________________
void TQtCoinViewerImp::CopyFrameCB()
{
   // Copy the entire window including the menu and the status bar
   QClipboard *cb = QApplication::clipboard();
   cb->setPixmap(QPixmap::grabWidget(topLevelWidget()));
}

//______________________________________________________________________________
void TQtCoinViewerImp::OpenCB()
{ 
   printf("TQtCoinViewerImp::OpenCB\n");
   if (fCoinWidget) {
      QString filter = "OpenInventor IV files (*.iv);;WRL File (*.wrl)";
      QString selectedFilter;
#if QT_VERSION < 0x40000
      QString thatFile = QFileDialog::getOpenFileName(gSystem->WorkingDirectory()
#else 
      QString thatFile = Q3FileDialog::getOpenFileName(gSystem->WorkingDirectory()
#endif 
        , filter, centralWidget(), "Open"
        , "Add graph to coin scene");
      
      fCoinWidget->ReadInputFile(thatFile);
   }
}

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
       (void)fprintf(stdout, "%s: %s (extension%s: ",
                      fullname.getString(), description.getString(),
                      extlist.getLength() > 1 ? "s" : "");
         a+= " ( " ;
         for (int j=0; j < extlist.getLength(); j++) {
            if (j>0) a+= ", "; a+= "*."; a+=(const char*) extlist[j];
            (void)fprintf(stdout, "%s%s", j>0 ? ", " : "", (const char*) extlist[j]);
         }
         a += " );";
         (void)fprintf(stdout, ")\n");
    }
  }
  return a;
}
//______________________________________________________________________________
static QStringList ExtensionList(const QString &filter)
{
   // Return the list of the extsntion from the file dialog filter
   QRegExp rx("(\\*\\.\\w+\\b)");
   QStringList extension;
   int pos = 0;
   while ( pos >= 0 ) {
      pos = rx.search(filter,pos);
      if ( pos > -1 ) {
         extension += rx.cap(1).replace("*.","");
         pos  += rx.matchedLength();
      }
   }   
   return extension ;
}
//______________________________________________________________________________
void TQtCoinViewerImp::SaveCB()
{ 
   if (fCoinWidget) {
      printf("TQtCoinViewerImp::SaveCB\n");
      //if (!c) return;
      QString filter = ListOfFilters();
      filter +=";Transparent Img File (*.rgbt);;WRL File (*.wrl);;IV files (*.iv);";

      QString selectedFilter;
#if QT_VERSION < 0x40000
      QString thatFile = QFileDialog::getSaveFileName(gSystem->WorkingDirectory()
#else 
      QString thatFile = Q3FileDialog::getSaveFileName(gSystem->WorkingDirectory()
#endif 
    , filter, centralWidget(), "SaveAs"
    , "Save the current view as"
    , &selectedFilter);
   
      QStringList selectedExtensions = ExtensionList(selectedFilter);
   
      if (thatFile.isEmpty()) return;
      QString e;
      QFileInfo outFile( thatFile );
      e = outFile.extension(FALSE).lower();
      int extIndex = selectedExtensions.findIndex(e);
     
      if (extIndex == -1) {
         // use the first extension
         e = selectedExtensions[0];
         // add this extenstion tho the existent file
         thatFile += ".";  thatFile += e;
      }
      printf("selectedFilter = %s\n", selectedFilter.data());
      printf("thatFile = %s\n", thatFile.data());
      printf("e = <%s>\n", e.data());
   
      fCoinWidget->Save(thatFile,e);
   }
}
//______________________________________________________________________________
void TQtCoinViewerImp::SaveAsCB()
{ 
  if (fCoinWidget) fCoinWidget->SaveAsCB();
}

//______________________________________________________________________________
void TQtCoinViewerImp::SaveSnapShot(bool yes)
{
  if (fCoinWidget) fCoinWidget->SaveSnapShot(yes);
}
/*
//______________________________________________________________________________
void TQtGLViewerImp::CopyFile(const QString &fileName2Copy,Int_t counter)
{
  QFileInfo infoOldFile(fileName2Copy);
  QString newName = infoOldFile.dirPath() + "/" + infoOldFile.baseName(TRUE) +
  "_S."+infoOldFile.extension(FALSE); 
  QFileInfo infoNewFile(newName);

QString shellCommand;
#ifndef WIN32  
   shellCommand = "cd " + infoOldFile.dirPath(true)  + " && " +
                  "cp " + infoOldFile.baseName(TRUE) + "-000" + QString::number(counter) + "." + infoOldFile.extension(FALSE)
                       + "   " + infoNewFile.baseName(TRUE) + "-000" + QString::number(counter) + "."
                         + infoNewFile.extension(FALSE);
#else
   shellCommand = 
                 "copy " + infoOldFile.baseName(TRUE) + "-000" + QString::number(counter) + "." + infoOldFile.extension(FALSE)
                         + "   " + infoNewFile.baseName(TRUE) + "-000" + QString::number(counter) + "." + infoNewFile.extension(FALSE);
#endif
   gSystem->Exec((const char*) shellCommand);
//   fprintf(stderr," ==Copy==  %s \n", (const char*) shellCommand);

   SaveHtml(newName,counter);
}
//______________________________________________________________________________
void TQtGLViewerImp::SaveHtml(Int_t counter)
{
   if (fSaveFile.IsNull())  SaveAsCB();
   if (!fSaveFile.IsNull()) {
     QString f(fSaveFile.Data());
     SaveHtml(f,counter);
   }
}
//______________________________________________________________________________
void TQtGLViewerImp::SaveHtml(QString &saveFile, Int_t counter)
{
#if 0
   if (fSaveFile.IsNull()) {
       SaveAsCB();
   }
   if (fSaveFile.IsNull()) return;
   
   QFileInfo info(fSaveFile.Data());
#else
   if (saveFile.isEmpty()) return;
   
   QFileInfo info(saveFile);
#endif   
   QString htmlFile(  info.dirPath(true) + "/" + info.baseName(TRUE) + "-000" +
                      QString::number(counter) +".html");
#ifdef GENERATE_HTML
   TVirtualPad *thisPad= GetPad();
   QString caption = thisPad->GetTitle();
   FILE *f = fopen((const char*)htmlFile, "w+");
   if (f) {
   QTextOStream(f) << "<html>"   << endl; 
   QTextOStream(f) << "<title>"  << endl;
   QTextOStream(f) <<  caption << endl;
//   QTextOStream(f) <<  caption() << endl;
   QTextOStream(f) << "</title>" << endl;
   QTextOStream(f) << "<head>"   << endl;
   QTextOStream(f) << "<meta http-equiv=\"REFRESH\" content=\"15\">" << endl;
   QTextOStream(f) << "</head>"  << endl;
   QTextOStream(f) << "<body>"   << endl;
   QTextOStream(f) << "<center>" << endl;
   QTextOStream(f) << "<H2> <a href=\"http://www.star.bnl.gov\">STAR@rhic</a></H2><br>" << endl;
//   QTextOStream(f) << "<H3>" << caption() << "</H3><br><hr><br>"   << endl;
   QTextOStream(f) << "<H3>" << caption << "</H3><br><hr><br>"   << endl;
   QTextOStream(f) << "<img src=\"" << info.baseName(TRUE) << "-000" + 
     QString::number(counter) + "." <<info.extension(FALSE)  << "\">" << endl;
//     QString::number(counter) + "." <<info.extension(FALSE)  << "\" width=100%>" << endl;
   QTextOStream(f) << "</center>"<< endl;
   QTextOStream(f) << "</body>"  << endl;
   QTextOStream(f) << "</html>"  << endl;
   fflush(f);
   fclose(f);
#ifndef WIN32  
   QString shellCommand =  "cd " + info.dirPath(true)  + " && " 
                         + "mv   " + info.baseName(TRUE) + "-000" + QString::number(counter) +".html "
                         + info.baseName(TRUE) + ".html";
#else
   QString shellCommand =  "cd  " + info.dirPath(true)  + " && " 
                         + "ren " + info.baseName(TRUE) + "-000" + QString::number(counter) +".html "
                                  + info.baseName(TRUE) + ".html";
#endif                         
#else
   QString shellCommand;
   QFileInfo html(htmlFile);
   if (html.exists() && html.isReadable() ) {
#ifndef WIN32
           shellCommand =  "cd " + info.dirPath(true)  + " && " 
                         + "cp " + htmlFile   + " "
                                 + "." + info.baseName(TRUE) + ".html && "
                         + "mv " + "." + info.baseName(TRUE) + ".html " + info.baseName(TRUE) + ".html";
#else
          shellCommand =   "cd  " + info.dirPath(true)  + " && "
                         + "copy "+ htmlFile            + "   "
                                  + "." + info.baseName(TRUE) + ".html && "
                         + "ren  "+ "." + info.baseName(TRUE) + ".html " + info.baseName(TRUE) + ".html";
#endif
   }
#endif
    if (!shellCommand.isEmpty())  gSystem->Exec((const char*) shellCommand);
    // fprintf(stderr," ***  %s \n", (const char*) shellCommand);
#ifdef GENERATE_HTML
   } else {
      char buffer[100];
      sprintf(buffer,"TQtGLViewerImp::SaveHtml file %s", (const char*)htmlFile);
      perror(buffer);
      assert(0);
   }
#endif
}
*/

//______________________________________________________________________________
void TQtCoinViewerImp::SnapShotSaveCB(bool on)
{  
	 if (fCoinWidget) fCoinWidget->SnapShotSaveCB(on);
}

//______________________________________________________________________________
void TQtCoinViewerImp::SynchTPadCB(bool on)
{  
   if (fCoinWidget) fCoinWidget->SynchTPadCB (on);
}

//______________________________________________________________________________
void TQtCoinViewerImp::ShowFrameAxisCB(bool on)
{  
   if (fCoinWidget) fCoinWidget->ShowFrameAxisCB(on);
}

//______________________________________________________________________________
void TQtCoinViewerImp::ShowLightsCB(bool on)
{  
  if (fCoinWidget) fCoinWidget->ShowLightsCB(on);
}

//______________________________________________________________________________
void TQtCoinViewerImp::WantRootContextMenuCB(bool on)
{
  // Create "ROOT Context menu" otherwise use the QGLViewer "right mouse click"
  if (fCoinWidget) fCoinWidget->WantRootContextMenuCB(on);
}

//______________________________________________________________________________
void TQtCoinViewerImp::AboutCB()
{ 
   QMessageBox::aboutQt(0);
   QString rootVersion = "ROOT ";
   rootVersion += gROOT->GetVersion();
   rootVersion += "with Qt interface";
   QMessageBox::about(0,rootVersion,"ROOT Qt interface Coin Viewer 2006 Zubanov Alexei AZubanov@gmail.com");
}

//______________________________________________________________________________
void TQtCoinViewerImp::HelpCB()
{
  if (fCoinWidget) fCoinWidget->HelpCB();
}
//______________________________________________________________________________
void TQtCoinViewerImp::CreateViewer(const char *name)
{
   printf("TQtCoinViewerImp::CreateViewer\n");
   if (!fCoinWidget) {
      fCoinWidget = new TQtCoinWidget(this);
      fCoinWidget->setName(name);
      setCentralWidget (fCoinWidget);
      connect(fCoinWidget,SIGNAL(ObjectSelected(TObject*,const QPoint &)), &this->Signals(), SIGNAL(ObjectSelected(TObject *, const QPoint&)));
   }
}

//______________________________________________________________________________
void TQtCoinViewerImp::SetBoxSelection()
{
   if (fCoinWidget) fCoinWidget->SetBoxSelection();
}

//______________________________________________________________________________
void TQtCoinViewerImp::SetLineSelection()
{
   if (fCoinWidget) fCoinWidget->SetLineSelection();
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
//______________________________________________________________________________
void TQtCoinViewerImp::MakeMenu()
{
   // Create a "save" action
#if QT_VERSION < 0x40000
   QAction *saveAction =  new QAction("Save", "&Save", CTRL+Key_S, this, "save" );
#else 
   Q3Action *saveAction =  new Q3Action("Save", "&Save", Qt::CTRL+Qt::Key_S, this, "save" );
#endif 
   connect ( saveAction, SIGNAL( activated() ) , this, SLOT( SaveCB() ) );

   const char * saveText = "<p><img source=\"save\"> "
                "Click this button to save a <em>3D image</em>to the current image file. <br>"
                "You can also select the <b>Save</b> command "
                "from the <b>File</b> menu.</p>";
   saveAction->setWhatsThis( saveText );

   // Create a "save as" action
#if QT_VERSION < 0x40000
   QAction *saveAsAction =  new QAction("SaveAs", "Save As", CTRL+Key_A, this, "saveas" );
#else 
   Q3Action *saveAsAction =  new Q3Action("SaveAs", "Save As", Qt::CTRL+Qt::Key_A, this, "saveas" );
#endif 
   connect ( saveAsAction, SIGNAL( activated() ) , this, SLOT( SaveAsCB() ) );

   const char * saveAsText = "<p><img source=\"save\"> "
                "Click this button to select file and save a <em>3D image</em>there. <br>"
                "You can also select the <b>Save As</b> command "
                "from the <b>File</b> menu.</p>";
   saveAsAction->setWhatsThis( saveAsText );
 
    // Create a "open" action
#if QT_VERSION < 0x40000
   QAction *openAction =  new QAction("Open", "Open", CTRL+Key_O, this, "open" );
#else 
   Q3Action *saveAsAction =  new Q3Action("Open", "Open", Qt::CTRL+Qt::Key_O, this, "open" );
#endif 
   connect ( openAction, SIGNAL( activated() ) , this, SLOT( OpenCB() ) );

   const char * openText = "<p><img source=\"save\"> "
                "Click this button to select file and open a <em>3D image</em>there. <br>"
                "You can also select the <b>Open</b> command "
                "from the <b>File</b> menu.</p>";
   openAction->setWhatsThis( openText );
 
   // Create a "save as" action
#if QT_VERSION < 0x40000
   fSnapShotAction  =  new QAction("snapShot", "SnapShotSave", CTRL+Key_O, this, "snapshot" );
   connect ( fSnapShotAction, SIGNAL( toggled(bool) ) , this, SLOT( SnapShotSaveCB(bool) ) );
#else 
   fSnapShotAction  =  new Q3Action("snapShot", "SnapShotSave", Qt::CTRL+Qt::Key_O, this, "snapshot" );
#endif 
   connect ( fSnapShotAction, SIGNAL( toggled(bool) ) , this, SLOT( SnapShotSaveCB(bool) ) );

   const char * snapShotText = "<p><img source=\"snapshot\"> "
                "Click this button to save the image each time the frame is updated";
   fSnapShotAction->setWhatsThis( snapShotText );
   fSnapShotAction->setToggleAction(true);

   // Create a "print" action
#if QT_VERSION < 0x40000
   QAction *printAction =  new QAction("Print", "&Print graph in ctout", CTRL+Key_P, this, "print" );
#else 
   Q3Action *printAction =  new Q3Action("Print", "&Print", Qt::CTRL+Qt::Key_P, this, "print" );
#endif 
   connect ( printAction, SIGNAL( activated() ) , this, SLOT( PrintCB() ) );

   const char * printText = "<p><img source=\"print\"> "
                "Click this button to print a <em>3D image</em>. <br>"
                "You can also select the <b>Print</b> command "
                "from the <b>File</b> menu.</p>";
   printAction->setWhatsThis( printText );

   // Create a "clear scene" action
#if QT_VERSION < 0x40000
   QAction *clearAction =  new QAction("Print", "&Clear Scene", CTRL+Key_C, this, "clear" );
#else 
   Q3Action *clearAction =  new Q3Action("Print", "&Clear", Qt::CTRL+Qt::Key_C, this, "clear" );
#endif 
   connect ( clearAction, SIGNAL( activated() ) , this, SLOT( ClearCB() ) );

   const char * clearText = "<p><img source=\"print\"> "
                "Click this button to clear a <em>3D image</em>. <br>"
                "You can also select the <b>Clear</b> command "
                "from the <b>File</b> menu.</p>";
   clearAction->setWhatsThis( clearText );

   // Create a "copy" action
#if QT_VERSION < 0x40000
   QAction *copyAction =  new QAction("Copy", "&Copy", CTRL+Key_C, this, "copy" );
#else 
   Q3Action *copyAction =  new Q3Action("Copy", "&Copy", Qt::CTRL+Qt::Key_C, this, "copy" );
#endif 
   connect ( copyAction, SIGNAL( activated() ) , this, SLOT( CopyCB() ) );

   const char * copyText = "<p><img source=\"copy\"> "
                "Click this button to copy a <em>3D image</em>to the system clipborad. <br>"
                "You can also select the <b>Copy</b> command "
                "from the <b>Edit</b> menu.</p>";
   copyAction->setWhatsThis( copyText );
 
    // Create a "copy frame" action
#if QT_VERSION < 0x40000
   QAction *copyFrameAction =  new QAction("Frame", "Copy &Frame", CTRL+Key_F, this, "frame" );
#else 
   Q3Action *copyFrameAction =  new Q3Action("Frame", "Copy &Frame", Qt::CTRL+Qt::Key_F, this, "frame" );
#endif 
   connect ( copyFrameAction, SIGNAL( activated() ) , this, SLOT( CopyFrameCB() ) );
   const char * copyFrameText = "<p><img source=\"frame\"> "
                "Click this button to copy <em>the frame of the 3D image</em>to the system clipborad. <br>"
                "You can also select the <b>Copy Frame</b> command "
                "from the <b>Edit</b> menu.</p>";
   copyFrameAction->setWhatsThis( copyFrameText );

   // Create "close" action
#if QT_VERSION < 0x40000
   QAction *fileCloseAction = new QAction( "Close", "&Close", Qt::ALT+Qt::Key_F4, this,
      "close" );
#else
   Q3Action *fileCloseAction = new Q3Action( "Close", "&Close", Qt::ALT+Qt::Key_F4, this,
      "close" );
#endif
   connect ( fileCloseAction, SIGNAL( activated() ) , this,  SLOT( close() ) );

   // Synchronize TPad rotation 
   
#if QT_VERSION < 0x40000
   QAction *synchAction =  new QAction("TPadSynch", "Synchromize with TPad", CTRL+Key_I, this, "synch" );
#else 
   Q3Action *synchAction =  new Q3Action("TPadSynch", "Synchromize with TPad", Qt::CTRL+Qt::Key_I, this, "synch" );
#endif 
   connect ( synchAction, SIGNAL( toggled(bool) ) , this, SLOT( SynchTPadCB(bool)  ) );
   const char * synchText = "<p><img source=\"frame\"> "
                "Select this option if your want the OpenGL view follows the <b>TPad</B> rotation";
   synchAction->setToggleAction(true);
   synchAction->setWhatsThis( synchText );

   // Show frame axis
   
#if QT_VERSION < 0x40000
   QAction *showFrameAxisAction =  new QAction("Frame3DAxis", "Show Frame axis", CTRL+Key_1, this, "frameaxis" );
#else 
   Q3Action *showFrameAxisAction =  new Q3Action("Frame3DAxis", "Show Frame axis", Qt::CTRL+Qt::Key_1, this, "frameaxis" );
#endif 
   connect ( showFrameAxisAction, SIGNAL( toggled(bool) ) , this, SLOT( ShowFrameAxisCB(bool)  ) );
   const char *showFrameAxisText = "Show the ROOT 3D object axis";
   showFrameAxisAction->setToggleAction(true);
   showFrameAxisAction->setWhatsThis( showFrameAxisText);

   // Show frame axis
   
#if QT_VERSION < 0x40000
   QAction *showSmallAxesAction =  new QAction("Small3DAxes", "Show \"small\" axes", CTRL+Key_2, this, "smallaxes" );
#else 
   Q3Action *showSmallAxesAction =  new Q3Action("Small3DAxes", "Show \"small\" axes", Qt::CTRL+Qt::Key_2, this, "smallaxes" );
#endif 
   connect ( showSmallAxesAction, SIGNAL( toggled(bool) ) , this, SLOT( SmallAxesActionCB(bool)  ) );
   const char *showSmallAxesText = "Show the  small 3D axes at the bottom right corner of the screen. Can slow down the rendering !!!";
   showSmallAxesAction->setToggleAction(true);
   showSmallAxesAction->setWhatsThis( showSmallAxesText);

      // Edit ClipPlane 
   
#if QT_VERSION < 0x40000
   QAction *editClipPlaneAction  =  new QAction("clipPlane", "Edit the Clip Plane", CTRL+Key_9, this, "clipplane" );
#else 
   Q3Action *editClipPlaneAction =  new Q3Action("clipPlane", "Edit the Clip Plane", Qt::CTRL+Qt::Key_9, this, "clipplane " );
#endif 
   connect ( editClipPlaneAction, SIGNAL( toggled(bool) ) , this, SLOT( FrameAxisActionCB(bool)  ) );
   const char *editClipPlaneText = "Activate Clip Plane manipulator";
   editClipPlaneAction->setToggleAction(true);
   showFrameAxisAction->setWhatsThis( editClipPlaneText);

#if QT_VERSION < 0x40000
   QAction *showLightsAction =  new QAction("GLLights", "Show &light", CTRL+Key_L, this, "gllight" );
#else 
   Q3Action *showLightsAction =  new Q3Action("GLLights", "Show &light", Qt::CTRL+Qt::Key_L, this, "gllight" );
#endif 
   connect ( showLightsAction, SIGNAL( toggled(bool) ) , this, SLOT( ShowLightsCB(bool)  ) );
   const char * showLightsText = "<p><img source=\"frame\"> "
                "Show the light source to debug the code";
   showLightsAction->setToggleAction(true);
   showLightsAction->setWhatsThis( showLightsText );
   /*
   // Create a "Event selectable" action
#if QT_VERSION < 0x40000
   QAction *selectEventAction =  new QAction("Event", "Select Event", CTRL+Key_E, this, "event" );
#else 
   Q3Action *selectEventAction =  new Q3Action("Event", "Select Event", Qt::CTRL+Qt::Key_E, this, "event" );
#endif 
   connect ( selectEventAction, SIGNAL( toggled(bool) ) , this, SLOT( SelectEventCB(bool) ) );
   const char * selectEventText = "Turn this option on to be able to select the <b>event</b> (tracks and hits)  and similar \"wired\" object";
   selectEventAction->setToggleAction(true);
   selectEventAction->setWhatsThis( selectEventText);
   
   // Create a "Detector selectable" action
#if QT_VERSION < 0x40000
   QAction *selectDetectorAction =  new QAction("Detector", "Select Detector", CTRL+Key_D, this, "detector" );
#else 
   Q3Action *selectDetectorAction =  new Q3Action("Detector", "Select Detector", Qt::CTRL+Qt::Key_D, this, "detector" );
#endif 
   connect ( selectDetectorAction, SIGNAL( toggled(bool)  ) , this, SLOT( SelectDetectorCB(bool) ) );
   const char * selectDetectorText = "Turn this option on to be able to select the <b>detector</b> (solid) objects";
   selectDetectorAction->setWhatsThis(  selectDetectorText);
   selectDetectorAction->setToggleAction(true);

   // Create a "Detector selectable" action
#if QT_VERSION < 0x40000
   QAction *viewSelectionAction =  new QAction("Selection", "Show the selected object", CTRL+Key_T, this, "selection" );
#else 
   Q3Action *viewSelectionAction =  new Q3Action("Selection", "Show the selected object", Qt::CTRL+Qt::Key_T, this, "selection" );
#endif 
   connect ( viewSelectionAction, SIGNAL( toggled(bool)  ) , this, SLOT( ActivateSelectorWidgetCB(bool) ) );
   const char * viewSelectionActionText = "Turn this option on to see the selected object with the dedicted view";
   viewSelectionAction->setWhatsThis( viewSelectionActionText );
   viewSelectionAction->setToggleAction(true);

   // Show  the "selected object" with the separate widget using the global coordinate
#if QT_VERSION < 0x40000
   QAction *viewSelectionGlobalAction =  new QAction("SelectionGlobal", "Show the selected object in global system", CTRL+Key_I, this, "selectionglobal" );
#else 
   Q3Action *viewSelectionGlobalAction =  new Q3Action("SelectionGlobal", "Show the selected object in global system", Qt::CTRL+Qt::Key_I, this, "selectionglobal" );
#endif 
   connect ( viewSelectionGlobalAction, SIGNAL( toggled(bool)  ) , this, SLOT( ActivateSelectionGlobalCB(bool) ) );
   connect ( viewSelectionAction, SIGNAL( toggled(bool)  ), viewSelectionGlobalAction, SLOT( setEnabled(bool) ) );
   const char * viewSelectionGlobalActionText = "Turn this option on to show the selected object in the global coordinate system";
   viewSelectionGlobalAction->setWhatsThis( viewSelectionGlobalActionText );
   viewSelectionGlobalAction->setToggleAction(true);

   // Create a "Highlight the selected object" action
#if QT_VERSION < 0x40000
   QAction *viewSelectionHighlightAction =  new QAction("SelectionHighLight", "Highlight the selected object", CTRL+Key_I, this, "selectionhighlight" );
#else 
   Q3Action *viewSelectionHighlightAction =  new Q3Action("SelectionHighLight", "Highlight the selected object", Qt::CTRL+Qt::Key_I, this, "selectionhighlight" );
#endif 
   connect ( viewSelectionHighlightAction, SIGNAL( toggled(bool)  ) , this, SLOT( ActivateSelectionHighlighCB(bool) ) );
   const char * viewSelectionHighlightActionText = "Turn this option on to highlight the selected object";
   viewSelectionHighlightAction->setWhatsThis( viewSelectionHighlightActionText );
   viewSelectionHighlightAction->setToggleAction(true);
 */
   // Create a "Context Menu for  the selected object" action
#if QT_VERSION < 0x40000
   QAction *viewContextMenuAction =  new QAction("ContexteMenu", "Context menu", CTRL+Key_I, this, "contextmenu" );
#else 
   Q3Action *viewContextMenuAction =  new Q3Action("ContexteMenu", "Context menu", Qt::CTRL+Qt::Key_I, this, "contextmenu" );
#endif 
   connect ( viewContextMenuAction, SIGNAL( toggled(bool)  ) , this, SLOT( WantRootContextMenuCB(bool) ) );
   const char * viewContextMenuActionText  = "Show the ROOT context menu for the selected ROOT object";
   viewContextMenuAction->setWhatsThis( viewContextMenuActionText );
   viewContextMenuAction->setToggleAction(true);

   QMenuBar   *mainMenu = menuBar();

   // -- populate the menu bar
#if QT_VERSION < 0x40000
      QPopupMenu *fileMenu      = new QPopupMenu();
#else 
      Q3PopupMenu *fileMenu      = new Q3PopupMenu();
#endif 
      mainMenu->insertItem("&File",fileMenu);
 /*
#if QT_VERSION < 0x40000
      QPopupMenu *editMenu      = new QPopupMenu();
#else 
      Q3PopupMenu *editMenu      = new Q3PopupMenu();
#endif 
      mainMenu->insertItem("&Edit",editMenu);
*/

#if QT_VERSION < 0x40000
      QPopupMenu *optionMenu    = new QPopupMenu();
#else 
      Q3PopupMenu *optionMenu    = new Q3PopupMenu();
#endif 
      mainMenu->insertItem("&Options",optionMenu);

#if QT_VERSION < 0x40000
      QPopupMenu *helpMenu   = new QPopupMenu();
#else 
      Q3PopupMenu *helpMenu   = new Q3PopupMenu();
#endif 
      mainMenu->insertItem("&Help",helpMenu);
  // -- The menu bar has been completed

 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 //  fileMenu
   
   saveAction     ->addTo(fileMenu);
   //saveAsAction   ->addTo(fileMenu);
   //fSnapShotAction->addTo(fileMenu);
   openAction     ->addTo(fileMenu);
   clearAction     ->addTo(fileMenu);
                    fileMenu->insertSeparator();
   printAction    ->addTo(fileMenu);
                    fileMenu->insertSeparator();
   fileCloseAction->addTo(fileMenu);


 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 //  editMenu
    //copyAction     ->addTo(editMenu);
    //copyFrameAction->addTo(editMenu);
   
 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 //  optionMenu
    //synchAction->addTo(optionMenu);
    //synchAction->setOn(true);
    //synchAction->setEnabled (true);
#ifdef QGLVIEWER

    //QWidget *c = centralWidget();
    //TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;

    showFrameAxisAction->addTo(optionMenu);
    //showFrameAxisAction->setOn(glView? glView->FrameAxisScale()> 0 : false);
    showFrameAxisAction->setOn(false);
    showFrameAxisAction->setEnabled(true);
    
    showSmallAxesAction->addTo(optionMenu);
    showSmallAxesAction->setOn(false);
    showSmallAxesAction->setEnabled(true);
    
/*
    showLightsAction->addTo(optionMenu);
    showLightsAction->setOn(false);
    showLightsAction->setEnabled (true);

    
    optionMenu->insertSeparator();

    selectEventAction->addTo(optionMenu);
    selectEventAction->setOn(glView ? glView->IsWiredSelectable() : false );
    selectEventAction->setEnabled (true);

    selectDetectorAction->addTo(optionMenu);
    selectDetectorAction->setOn( glView ? glView->IsSolidSelectable() : false );
    selectDetectorAction->setEnabled (true);
    
    optionMenu->insertSeparator();
    
    viewSelectionAction->addTo(optionMenu);
    viewSelectionAction->setOn( false );
    viewSelectionAction->setEnabled (true);
    
    viewSelectionGlobalAction->addTo(optionMenu);
    viewSelectionGlobalAction->setOn( false );
    viewSelectionGlobalAction->setEnabled (false);

    viewSelectionHighlightAction->addTo(optionMenu);
    viewSelectionHighlightAction->setOn( false );
    viewSelectionHighlightAction->setEnabled (true);
*/
    viewContextMenuAction->addTo(optionMenu);
    viewContextMenuAction->setOn( false );
    viewContextMenuAction->setEnabled (true);
    
    editClipPlaneAction->addTo(optionMenu);
    editClipPlaneAction->setOn( false );
    editClipPlaneAction->setEnabled (true);

#endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 //  helpMenu
    helpMenu->insertItem("&Help",this,SLOT(HelpCB()));
    helpMenu->insertSeparator();
    helpMenu->insertItem("&About",this,SLOT(AboutCB()));
}
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
void TQtCoinViewerImp::Update()
{  
   if (fCoinWidget) fCoinWidget->Update();
}
/*
//______________________________________________________________________________
void TQtGLViewerImp::SetFooter(QString &text)
{
   // Set the footer text
#ifdef QGLVIEWER
  if (fGLWidget) ((TQtGLViewerWidget*)fGLWidget)->setFooter(text);
#endif
}
*/
/*
//______________________________________________________________________________
void TQtCoinViewerImp::SetPadSynchronize(Bool_t on)
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
void TQtCoinViewerImp::SetRotationAxisAngle(const float x, const float y, const float z, const float a)
{
   // Set the current rotation of the frame. 
   // Parameters are the rotation axis vector and its angle (in radians). 
   if (fCoinWidget) fCoinWidget->SetRotationAxisAngle(x,y,z,a);
}

//______________________________________________________________________________
void TQtCoinViewerImp::SetBackgroundColor(Color_t color)
{
   if (fCoinWidget) fCoinWidget->SetBackgroundColor(color);
}

//______________________________________________________________________________
void TQtCoinViewerImp::ShowObjectInfo(TObject *obj, const QPoint &cursorPosition)
{
   if (obj && fCoinWidget) fCoinWidget->ShowObjectInfo(obj,cursorPosition);
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
ULong_t TQtCoinViewerImp::GetViewerID() const
{
   return ULong_t((QMainWindow *) this);
}

//______________________________________________________________________________
void TQtCoinViewerImp::SetSnapFileCounter(int counter)
{ 
	if (fCoinWidget) fCoinWidget->SetSnapFileCounter(counter);
}
//______________________________________________________________________________
void TQtCoinViewerImp::SetCliPlaneMan(Bool_t on)
{
    if (fCoinWidget) fCoinWidget->SetCliPlaneMan(on);
}

//______________________________________________________________________________
void TQtCoinViewerImp::FrameAxisActionCB(bool on)
{
  SetCliPlaneMan(on); 
}
//______________________________________________________________________________
void TQtCoinViewerImp::SmallAxesActionCB(bool on)
{
     if (fCoinWidget) fCoinWidget->SmallAxesActionCB(on);
}
//______________________________________________________________________________
SoCamera *TQtCoinViewerImp::GetCamera() const 
{ 
   return fCoinWidget ? fCoinWidget->GetCamera(): 0;
}
//______________________________________________________________________________
SmAxisDisplayKit *TQtCoinViewerImp::GetAxis() const 
{ 
   return fCoinWidget ? fCoinWidget->GetAxis() : 0;
}
//______________________________________________________________________________
std::vector<int> TQtCoinViewerImp::GetMyGLList1() const
{ 
   static std::vector<int>  d;
   return fCoinWidget ? fCoinWidget->GetMyGLList1() : d; 
}
//______________________________________________________________________________
std::vector<int> TQtCoinViewerImp::GetMyGLList2() const
{ 
   static std::vector<int>  d;
   return fCoinWidget ? fCoinWidget->GetMyGLList2() : d; 
}
//______________________________________________________________________________
std::vector<int> TQtCoinViewerImp::GetMyGLList3() const
{
   static std::vector<int>  d;
   return fCoinWidget ? fCoinWidget->GetMyGLList3() : d; 
}
//______________________________________________________________________________
TObject  *TQtCoinViewerImp::GetSelected() const 
{ 
   return fCoinWidget ? fCoinWidget->GetSelected() : 0;
}
//______________________________________________________________________________
SoQtViewer  *TQtCoinViewerImp::GetCoinViewer()  const 
{
   return fCoinWidget ? fCoinWidget->GetCoinViewer():0; 
}
//______________________________________________________________________________
Bool_t  TQtCoinViewerImp::WantRootContextMenu() const 
{
   return fCoinWidget ? fCoinWidget->WantRootContextMenu():kFALSE;
}
//______________________________________________________________________________
Bool_t   TQtCoinViewerImp::WasPicked(void *p) const { 
   return fCoinWidget ? fCoinWidget->WasPicked(p) : kFALSE;
}
