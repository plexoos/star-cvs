#include "TVirtualPad.h"
#include "TContextMenu.h"

#include "TQVirtualGL.h"
#include "TQPadOpenGLView.h"

#define QGLVIEWER

#include "TQtGLViewerImp.h"
#include "TQtGLViewerWidget.h"
#include "TObject3DView.h"

#include "TSystem.h"
#include "TROOT.h"
#include "TEnv.h"
#include "TColor.h"

#include <qprinter.h>
#include <qpixmap.h>
#include <qapplication.h>
#include <qclipboard.h>
#include <qimage.h>

#if QT_VERSION < 0x40000
#  include <qfiledialog.h>
#  include <qpopupmenu.h>
#  include <qwhatsthis.h> 
#  include <qaction.h>
#else /* QT_VERSION */
#  include <q3filedialog.h>
#  include <q3popupmenu.h>
#  include <q3whatsthis.h> 
//Added by qt3to4:
#  include <QLabel>
#  include <Q3Action>
#endif /* QT_VERSION */

#include <qfile.h>
#include <qfileinfo.h>
#include <qmessagebox.h>
#include <qmenubar.h>

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

// ClassImp(TQtGLViewerImp)
Int_t TQtGLViewerImp::gfDefaultMaxSnapFileCounter = 2;
//______________________________________________________________________________
#if QT_VERSION < 0x40000
TQtGLViewerImp::TQtGLViewerImp():QMainWindow(){}
#else /* QT_VERSION */
TQtGLViewerImp::TQtGLViewerImp():Q3MainWindow(){}
#endif /* QT_VERSION */
//______________________________________________________________________________
TQtGLViewerImp::TQtGLViewerImp(TPadOpenGLView *pad, const char *title,
                       UInt_t width, UInt_t height)
#if QT_VERSION < 0x40000
   : QMainWindow(0,"glviewer", Qt::WDestructiveClose)
#else /* QT_VERSION */
   : Q3MainWindow(0,"glviewer", Qt::WDestructiveClose)
#endif /* QT_VERSION */
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
//______________________________________________________________________________
TQtGLViewerImp::TQtGLViewerImp(TPadOpenGLView *pad, const char *title,
                       Int_t x, Int_t y, UInt_t width, UInt_t height)
#if QT_VERSION < 0x40000
   : QMainWindow(0,"glviewer", Qt::WDestructiveClose | Qt::WRepaintNoErase | Qt:: WResizeNoErase )
#else /* QT_VERSION */
   : Q3MainWindow(0,"glviewer",Qt::WDestructiveClose | Qt::WNoAutoErase | Qt:: WResizeNoErase )
#endif /* QT_VERSION */
   , TGLViewerImp(pad,title,x,y,width,height)
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

         setGeometry(x, y, width, height);
         fGLView = pad;
         CreateViewer(title);
         MakeMenu();
         int parts[] = {43,7,10,39};
         CreateStatusBar(parts,4);
         SetDrawList(0);
         show();
      }
   }
   fMaxSnapFileCounter = CreateSnapShotCounter();
}
//______________________________________________________________________________
TQtGLViewerImp::TQtGLViewerImp(TVirtualPad *pad, const char *title,
                       UInt_t width, UInt_t height)
#if QT_VERSION < 0x40000
   : QMainWindow(0,"glviewer", Qt::WDestructiveClose)
#else /* QT_VERSION */
   : Q3MainWindow(0,"glviewer", Qt::WDestructiveClose)
#endif /* QT_VERSION */
   , TGLViewerImp(0,title,width,height)
   , fSaveType("JPEG"),fMaxSnapFileCounter(2),fGLWidget(0),fPad(pad),fContextMenu(0),fSelectedView(0),fSelectedViewActive(kFALSE)
   , fSelectionViewer(kFALSE),fSelectionHighlight(kFALSE),fShowSelectionGlobal(kFALSE),fWantRootContextMenu(kFALSE)
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
      resize(width, height);
      fGLView = 0;
      CreateViewer(title);
      int parts[] = {43,7,10,39};
      CreateStatusBar(parts,4);
      MakeMenu();
      SetDrawList(0);
      show();
   }
   fMaxSnapFileCounter = CreateSnapShotCounter();
}
//______________________________________________________________________________
TQtGLViewerImp::TQtGLViewerImp(TVirtualPad *pad, const char *title,
                       Int_t x, Int_t y, UInt_t width, UInt_t height)
#if QT_VERSION < 0x40000
   : QMainWindow(0,"glviewer",Qt::WDestructiveClose | Qt::WRepaintNoErase | Qt:: WResizeNoErase )
#else /* QT_VERSION */
   : Q3MainWindow(0,"glviewer",Qt::WDestructiveClose | Qt::WNoAutoErase | Qt:: WResizeNoErase )
#endif /* QT_VERSION */
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
#else /* QT_VERSION */
     Q3MainWindow(&parent,"glviewerutil",Qt::WDestructiveClose | Qt::WType_TopLevel)
#endif /* QT_VERSION */
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
//______________________________________________________________________________
TQtGLViewerImp::~TQtGLViewerImp()
{  }
//______________________________________________________________________________
void TQtGLViewerImp::Clear()
{
     // Clear (remove) all objects from the view
   QWidget *c = centralWidget();
   if (!c) return;
   TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
   glView->clearGLList();
   Update();
}
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
//______________________________________________________________________________
void TQtGLViewerImp::CreateStatusBar(Int_t *parts, Int_t nparts)
{
  //  Create the status bar with "nparts" separate portions
  // The initial relative size of each part is defined by "parts" array

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
}
//______________________________________________________________________________
int TQtGLViewerImp::CreateSnapShotCounter()
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
void TQtGLViewerImp::SetUpdatesEnabled(const bool&enable)
{  setUpdatesEnabled(enable); }
//______________________________________________________________________________
void TQtGLViewerImp::ShowStatusBar(Bool_t show)
{
   // Show / Hide the status bar
   
  if (show) statusBar()->show();
  else      statusBar()->hide();
}
//______________________________________________________________________________
 void  TQtGLViewerImp::DisconnectPad()
 {
    fPad = 0;
 }
//______________________________________________________________________________
TVirtualPad *TQtGLViewerImp::GetPad() 
{
   if (GetGLView()) return GetGLView()->GetPad();
   return fPad;
}
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

//______________________________________________________________________________
void TQtGLViewerImp::AddGLList(unsigned int list, int type)
{
	// printf("TQtGLViewerImp::AddGLList");
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
}
//______________________________________________________________________________
void TQtGLViewerImp::RemoveGLList(unsigned int list)
{
   QWidget *c = centralWidget();
   if (!c) return;   
#ifdef QGLVIEWER
     TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
     glView->removeGLList(list);
#endif
}
//______________________________________________________________________________
void TQtGLViewerImp::NewViewer(){
  new TQtGLViewerImp(GetGLView());
}
//______________________________________________________________________________
void TQtGLViewerImp::PrintCB(){
   QPrinter p;
   QWidget *c = centralWidget();
   if (c && p.setup()) {
      QPainter pnt(&p);
        QGLWidget *glView = (QGLWidget *)c;
        if (glView) pnt.drawImage(0,0,glView-> grabFrameBuffer());
      pnt.end();
   }
}
//______________________________________________________________________________
void TQtGLViewerImp::CopyCB()
{
 //  Copy the current x3d view to the system clipboard 
   QWidget *c = centralWidget();
   if (c) { 
      QClipboard *cb = QApplication::clipboard();
      cb->setPixmap(QPixmap::grabWindow(c->winId () )) ;
   }
}
//______________________________________________________________________________
void TQtGLViewerImp::CopyFrameCB()
{
   // Copy the entire window including the menu and the status bar
   QClipboard *cb = QApplication::clipboard();
   cb->setPixmap(QPixmap::grabWidget(topLevelWidget()));
}
//______________________________________________________________________________
void TQtGLViewerImp::SaveCB()
{ 
   QWidget *c = centralWidget();
   if (!c) return;
   if (fSaveFile.IsNull()) 
      SaveAsCB();
   else {
#ifndef QGLVIEWER
     QGLWidget *glView = (QGLWidget *)c;
     glView-> grabFrameBuffer().save(fSaveFile.Data(),fSaveType.Data());
#else
     TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
     glView->setSnapshotFileName  (fSaveFile.Data());
     glView->saveSnapshot(true);
     fSaveFile = (const char*) glView->snapshotFileName();
#endif
  }
}
//______________________________________________________________________________
void TQtGLViewerImp::SaveAsCB()
{ 
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
#else /* QT_VERSION */
   QString thatFile = Q3FileDialog::getSaveFileName(gSystem->WorkingDirectory()
#endif /* QT_VERSION */
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
}
//______________________________________________________________________________
void TQtGLViewerImp::SaveSnapShot(bool)
{
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
}
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

//______________________________________________________________________________
void TQtGLViewerImp::SnapShotSaveCB(bool on)
{  
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
}

//______________________________________________________________________________
void TQtGLViewerImp::SynchTPadCB(bool on)
{  
    SetPadSynchronize(on);
}

//______________________________________________________________________________
void TQtGLViewerImp::ShowFrameAxisCB(bool on)
{  
#ifdef QGLVIEWER
   QWidget *c = centralWidget();
   TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
   if (on) 
      glView->SetFrameAxisFactor(5.0);
   else 
      glView->SetFrameAxisFactor(-1);
   glView->update();
#endif
}
//______________________________________________________________________________
void TQtGLViewerImp::ShowLightsCB(bool on)
{  
#ifdef QGLVIEWER
   QWidget *c = centralWidget();
   TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;
   glView->setDrawLight(on);glView->update();
#endif
}
//______________________________________________________________________________
void TQtGLViewerImp::WantRootContextMenuCB(bool on)
{
  // Create "ROOT Context menu" otherwise use the QGLViewer "right mouse click"
  fWantRootContextMenu = on; 
}
//______________________________________________________________________________
void TQtGLViewerImp::AboutCB()
{ 
   QMessageBox::aboutQt(0);
   QString rootVersion = "ROOT ";
   rootVersion += gROOT->GetVersion();
   rootVersion += "with Qt interface";
   QMessageBox::about(0,rootVersion
     ,"ROOT Qt interface Copyright (C) 2001-2005, Valeri Fine. Brookhaven National Laboratory. All right reserved.");
}
//______________________________________________________________________________
void TQtGLViewerImp::HelpCB()
{
   // Show TGLWidget command key values
#ifndef QGLVIEWER
   TQtGLWidget *c = dynamic_cast<TQtGLWidget *>(centralWidget());
   if (c) c->ShowHelp();
#else
   TQtGLViewerWidget *c = dynamic_cast<TQtGLViewerWidget *>(centralWidget());
   if (c) c->help();
#endif
}
//______________________________________________________________________________
void TQtGLViewerImp::CreateViewer(const char *name)
{
   if (!fGLWidget) {
      if (fPad) {
      fGLWidget = 
#ifndef QGLVIEWER
                 new TQtGLWidget      ( fPad,name,this);
#else
                 new TQtGLViewerWidget( fPad, name, this );
#endif
      } else {
      fGLWidget = 
#ifndef QGLVIEWER
                 new TQtGLWidget      (GetGLView(),name,this);
#else
                 new TQtGLViewerWidget( GetGLView(), name, this );
#endif
      }
      setCentralWidget (fGLWidget);
#ifdef QGLVIEWER
      ((TQtGLViewerWidget *)fGLWidget)->setSnapshotFileName  (fSaveFile.Data());
      connect(fGLWidget,SIGNAL(objectSelected(TObject*,const QPoint &)),this,SLOT( ShowObjectInfo(TObject *, const QPoint&)));
      connect(fGLWidget,SIGNAL(objectSelected(TObject*,const QPoint &)),&this->Signals(),SIGNAL( ObjectSelected(TObject *, const QPoint&)));
#endif
   };
}
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
//______________________________________________________________________________
void TQtGLViewerImp::MakeMenu()
{
   // Create a "save" action
#if QT_VERSION < 0x40000
   QAction *saveAction =  new QAction("Save", "&Save", CTRL+Key_S, this, "save" );
#else /* QT_VERSION */
   Q3Action *saveAction =  new Q3Action("Save", "&Save", Qt::CTRL+Qt::Key_S, this, "save" );
#endif /* QT_VERSION */
   connect ( saveAction, SIGNAL( activated() ) , this, SLOT( SaveCB() ) );

   const char * saveText = "<p><img source=\"save\"> "
                "Click this button to save a <em>3D image</em>to the current image file. <br>"
                "You can also select the <b>Save</b> command "
                "from the <b>File</b> menu.</p>";
   saveAction->setWhatsThis( saveText );

   // Create a "save as" action
#if QT_VERSION < 0x40000
   QAction *saveAsAction =  new QAction("SaveAs", "Save As", CTRL+Key_A, this, "saveas" );
#else /* QT_VERSION */
   Q3Action *saveAsAction =  new Q3Action("SaveAs", "Save As", Qt::CTRL+Qt::Key_A, this, "saveas" );
#endif /* QT_VERSION */
   connect ( saveAsAction, SIGNAL( activated() ) , this, SLOT( SaveAsCB() ) );

   const char * saveAsText = "<p><img source=\"save\"> "
                "Click this button to select file and save a <em>3D image</em>there. <br>"
                "You can also select the <b>Save As</b> command "
                "from the <b>File</b> menu.</p>";
   saveAsAction->setWhatsThis( saveAsText );
 
   // Create a "save as" action
#if QT_VERSION < 0x40000
   fSnapShotAction  =  new QAction("snapShot", "SnapShotSave", CTRL+Key_O, this, "snapshot" );
   connect ( fSnapShotAction, SIGNAL( toggled(bool) ) , this, SLOT( SnapShotSaveCB(bool) ) );
#else /* QT_VERSION */
   fSnapShotAction  =  new Q3Action("snapShot", "SnapShotSave", Qt::CTRL+Qt::Key_O, this, "snapshot" );
#endif /* QT_VERSION */
   connect ( fSnapShotAction, SIGNAL( toggled(bool) ) , this, SLOT( SnapShotSaveCB(bool) ) );

   const char * snapShotText = "<p><img source=\"snapshot\"> "
                "Click this button to save the image each time the frame is updated";
   fSnapShotAction->setWhatsThis( snapShotText );
   fSnapShotAction->setToggleAction(true);

   // Create a "print" action
#if QT_VERSION < 0x40000
   QAction *printAction =  new QAction("Print", "&Print", CTRL+Key_P, this, "print" );
#else /* QT_VERSION */
   Q3Action *printAction =  new Q3Action("Print", "&Print", Qt::CTRL+Qt::Key_P, this, "print" );
#endif /* QT_VERSION */
   connect ( printAction, SIGNAL( activated() ) , this, SLOT( PrintCB() ) );

   const char * printText = "<p><img source=\"print\"> "
                "Click this button to print a <em>3D image</em>. <br>"
                "You can also select the <b>Print</b> command "
                "from the <b>File</b> menu.</p>";
   printAction->setWhatsThis( printText );

   // Create a "copy" action
#if QT_VERSION < 0x40000
   QAction *copyAction =  new QAction("Copy", "&Copy", CTRL+Key_C, this, "copy" );
#else /* QT_VERSION */
   Q3Action *copyAction =  new Q3Action("Copy", "&Copy", Qt::CTRL+Qt::Key_C, this, "copy" );
#endif /* QT_VERSION */
   connect ( copyAction, SIGNAL( activated() ) , this, SLOT( CopyCB() ) );

   const char * copyText = "<p><img source=\"copy\"> "
                "Click this button to copy a <em>3D image</em>to the system clipborad. <br>"
                "You can also select the <b>Copy</b> command "
                "from the <b>Edit</b> menu.</p>";
   copyAction->setWhatsThis( copyText );
 
    // Create a "copy frame" action
#if QT_VERSION < 0x40000
   QAction *copyFrameAction =  new QAction("Frame", "Copy &Frame", CTRL+Key_F, this, "frame" );
#else /* QT_VERSION */
   Q3Action *copyFrameAction =  new Q3Action("Frame", "Copy &Frame", Qt::CTRL+Qt::Key_F, this, "frame" );
#endif /* QT_VERSION */
   connect ( copyFrameAction, SIGNAL( activated() ) , this, SLOT( CopyFrameCB() ) );
   const char * copyFrameText = "<p><img source=\"frame\"> "
                "Click this button to copy a <em>tge frame of the 3D image</em>to the system clipborad. <br>"
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
#else /* QT_VERSION */
   Q3Action *synchAction =  new Q3Action("TPadSynch", "Synchromize with TPad", Qt::CTRL+Qt::Key_I, this, "synch" );
#endif /* QT_VERSION */
   connect ( synchAction, SIGNAL( toggled(bool) ) , this, SLOT( SynchTPadCB(bool)  ) );
   const char * synchText = "<p><img source=\"frame\"> "
                "Select this option if your want the OpenGL view follows the <b>TPad</B> rotation";
   synchAction->setToggleAction(true);
   synchAction->setWhatsThis( synchText );

   // Synchronize TPad rotation 
   
#ifdef QGLVIEWER
#if QT_VERSION < 0x40000
   QAction *showFrameAxisAction =  new QAction("Frame3DAxis", "Show Frame axis", CTRL+Key_1, this, "frameaxis" );
#else /* QT_VERSION */
   Q3Action *showFrameAxisAction =  new Q3Action("Frame3DAxis", "Show Frame axis", Qt::CTRL+Qt::Key_1, this, "frameaxis" );
#endif /* QT_VERSION */
   connect ( showFrameAxisAction, SIGNAL( toggled(bool) ) , this, SLOT( ShowFrameAxisCB(bool)  ) );
   const char *showFrameAxisText = "Show the ROOT 3D object axis";
   showFrameAxisAction->setToggleAction(true);
   showFrameAxisAction->setWhatsThis( showFrameAxisText);

#if QT_VERSION < 0x40000
   QAction *showLightsAction =  new QAction("GLLights", "Show &light", CTRL+Key_L, this, "gllight" );
#else /* QT_VERSION */
   Q3Action *showLightsAction =  new Q3Action("GLLights", "Show &light", Qt::CTRL+Qt::Key_L, this, "gllight" );
#endif /* QT_VERSION */
   connect ( showLightsAction, SIGNAL( toggled(bool) ) , this, SLOT( ShowLightsCB(bool)  ) );
   const char * showLightsText = "<p><img source=\"frame\"> "
                "Show the light source to debug the code";
   showLightsAction->setToggleAction(true);
   showLightsAction->setWhatsThis( showLightsText );
   
   // Create a "Event selectable" action
#if QT_VERSION < 0x40000
   QAction *selectEventAction =  new QAction("Event", "Select Event", CTRL+Key_E, this, "event" );
#else /* QT_VERSION */
   Q3Action *selectEventAction =  new Q3Action("Event", "Select Event", Qt::CTRL+Qt::Key_E, this, "event" );
#endif /* QT_VERSION */
   connect ( selectEventAction, SIGNAL( toggled(bool) ) , this, SLOT( SelectEventCB(bool) ) );
   const char * selectEventText = "Turn this option on to be able to select the <b>event</b> (tracks and hits)  and similar \"wired\" object";
   selectEventAction->setToggleAction(true);
   selectEventAction->setWhatsThis( selectEventText);
   
   // Create a "Detector selectable" action
#if QT_VERSION < 0x40000
   QAction *selectDetectorAction =  new QAction("Detector", "Select Detector", CTRL+Key_D, this, "detector" );
#else /* QT_VERSION */
   Q3Action *selectDetectorAction =  new Q3Action("Detector", "Select Detector", Qt::CTRL+Qt::Key_D, this, "detector" );
#endif /* QT_VERSION */
   connect ( selectDetectorAction, SIGNAL( toggled(bool)  ) , this, SLOT( SelectDetectorCB(bool) ) );
   const char * selectDetectorText = "Turn this option on to be able to select the <b>detector</b> (solid) objects";
   selectDetectorAction->setWhatsThis(  selectDetectorText);
   selectDetectorAction->setToggleAction(true);

   // Create a "Detector selectable" action
#if QT_VERSION < 0x40000
   QAction *viewSelectionAction =  new QAction("Selection", "Show the selected object", CTRL+Key_T, this, "selection" );
#else /* QT_VERSION */
   Q3Action *viewSelectionAction =  new Q3Action("Selection", "Show the selected object", Qt::CTRL+Qt::Key_T, this, "selection" );
#endif /* QT_VERSION */
   connect ( viewSelectionAction, SIGNAL( toggled(bool)  ) , this, SLOT( ActivateSelectorWidgetCB(bool) ) );
   const char * viewSelectionActionText = "Turn this option on to see the selected object with the dedicted view";
   viewSelectionAction->setWhatsThis( viewSelectionActionText );
   viewSelectionAction->setToggleAction(true);

   // Show  the "selected object" with the separate widget using the global coordinate
#if QT_VERSION < 0x40000
   QAction *viewSelectionGlobalAction =  new QAction("SelectionGlobal", "Show the selected object in global system", CTRL+Key_I, this, "selectionglobal" );
#else /* QT_VERSION */
   Q3Action *viewSelectionGlobalAction =  new Q3Action("SelectionGlobal", "Show the selected object in global system", Qt::CTRL+Qt::Key_I, this, "selectionglobal" );
#endif /* QT_VERSION */
   connect ( viewSelectionGlobalAction, SIGNAL( toggled(bool)  ) , this, SLOT( ActivateSelectionGlobalCB(bool) ) );
   connect ( viewSelectionAction, SIGNAL( toggled(bool)  ), viewSelectionGlobalAction, SLOT( setEnabled(bool) ) );
   const char * viewSelectionGlobalActionText = "Turn this option on to show the selected object in the global coordinate system";
   viewSelectionGlobalAction->setWhatsThis( viewSelectionGlobalActionText );
   viewSelectionGlobalAction->setToggleAction(true);

   // Create a "Highlight the selected object" action
#if QT_VERSION < 0x40000
   QAction *viewSelectionHighlightAction =  new QAction("SelectionHighLight", "Highlight the selected object", CTRL+Key_I, this, "selectionhighlight" );
#else /* QT_VERSION */
   Q3Action *viewSelectionHighlightAction =  new Q3Action("SelectionHighLight", "Highlight the selected object", Qt::CTRL+Qt::Key_I, this, "selectionhighlight" );
#endif /* QT_VERSION */
   connect ( viewSelectionHighlightAction, SIGNAL( toggled(bool)  ) , this, SLOT( ActivateSelectionHighlighCB(bool) ) );
   const char * viewSelectionHighlightActionText = "Turn this option on to highlight the selected object";
   viewSelectionHighlightAction->setWhatsThis( viewSelectionHighlightActionText );
   viewSelectionHighlightAction->setToggleAction(true);
 
   // Create a "Context Menu for  the selected object" action
#if QT_VERSION < 0x40000
   QAction *viewContextMenuAction =  new QAction("ContexteMenu", "Context menu", CTRL+Key_I, this, "contextmenu" );
#else /* QT_VERSION */
   Q3Action *viewContextMenuAction =  new Q3Action("ContexteMenu", "Context menu", Qt::CTRL+Qt::Key_I, this, "contextmenu" );
#endif /* QT_VERSION */
   connect ( viewContextMenuAction, SIGNAL( toggled(bool)  ) , this, SLOT( WantRootContextMenuCB(bool) ) );
   const char * viewContextMenuActionText  = "Show the ROOT context menu for the selected ROOT object";
   viewContextMenuAction->setWhatsThis( viewContextMenuActionText );
   viewContextMenuAction->setToggleAction(true);
#endif
  
   QMenuBar   *mainMenu = menuBar();

   // -- populate the menu bar
#if QT_VERSION < 0x40000
      QPopupMenu *fileMenu      = new QPopupMenu();
#else /* QT_VERSION */
      Q3PopupMenu *fileMenu      = new Q3PopupMenu();
#endif /* QT_VERSION */
      mainMenu->insertItem("&File",fileMenu);
 
#if QT_VERSION < 0x40000
      QPopupMenu *editMenu      = new QPopupMenu();
#else /* QT_VERSION */
      Q3PopupMenu *editMenu      = new Q3PopupMenu();
#endif /* QT_VERSION */
      mainMenu->insertItem("&Edit",editMenu);

#if QT_VERSION < 0x40000
      QPopupMenu *optionMenu    = new QPopupMenu();
#else /* QT_VERSION */
      Q3PopupMenu *optionMenu    = new Q3PopupMenu();
#endif /* QT_VERSION */
      mainMenu->insertItem("&Options",optionMenu);

#if QT_VERSION < 0x40000
      QPopupMenu *helpMenu   = new QPopupMenu();
#else /* QT_VERSION */
      Q3PopupMenu *helpMenu   = new Q3PopupMenu();
#endif /* QT_VERSION */
      mainMenu->insertItem("&Help",helpMenu);
  // -- The menu bar has been completed

 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 //  fileMenu
   
   saveAction     ->addTo(fileMenu);
   saveAsAction   ->addTo(fileMenu);
   fSnapShotAction->addTo(fileMenu);
                    fileMenu->insertSeparator();
   printAction    ->addTo(fileMenu);
                    fileMenu->insertSeparator();
   fileCloseAction->addTo(fileMenu);


 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 //  editMenu
    copyAction     ->addTo(editMenu);
    copyFrameAction->addTo(editMenu);
   
 // - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 //  optionMenu
    synchAction->addTo(optionMenu);
    synchAction->setOn(true);
    synchAction->setEnabled (true);
#ifdef QGLVIEWER
    QWidget *c = centralWidget();
    TQtGLViewerWidget *glView = (TQtGLViewerWidget *)c;

    showFrameAxisAction->addTo(optionMenu);
    showFrameAxisAction->setOn(glView? glView->FrameAxisScale()> 0 : false);
    showFrameAxisAction->setEnabled(true);

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
    
    viewContextMenuAction->addTo(optionMenu);
    viewContextMenuAction->setOn( false );
    viewContextMenuAction->setEnabled (true);

#endif
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 //  helpMenu
    helpMenu->insertItem("&Help",this,SLOT(HelpCB()));
    helpMenu->insertSeparator();
    helpMenu->insertItem("&About",this,SLOT(AboutCB()));
}
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

//______________________________________________________________________________
void TQtGLViewerImp::Update()
{  
   CreateViewer();
#ifdef QGLVIEWER
   if (fGLWidget && !fSelectionViewer) 
      ((TQtGLViewerWidget*)fGLWidget)->SetSceneLayout();
#endif
   TGLViewerImp::Update();
   centralWidget()->update();
}
//______________________________________________________________________________
void TQtGLViewerImp::SetFooter(QString &text)
{
   // Set the footer text
#ifdef QGLVIEWER
  if (fGLWidget) ((TQtGLViewerWidget*)fGLWidget)->setFooter(text);
#endif
}
//______________________________________________________________________________
void TQtGLViewerImp::SetPadSynchronize(Bool_t on)
{
  // Define whether the GL Widget view should follow TPad rotation
#ifndef QGLVIEWER
   if(on) {} // QtGLWidget provide no synch means
#else
   if (fGLWidget) ((TQtGLViewerWidget*)fGLWidget)->setPadSynchronize(on);
#endif
}
//______________________________________________________________________________
void TQtGLViewerImp::SetRotationAxisAngle(const float x, const float y, const float z, const float a)
{
   // Set the current rotation of the frame. 
   // Parameters are the rotation axis vector and its angle (in radians). 
#ifdef QGLVIEWER
  if (fGLWidget) ((TQtGLViewerWidget*)fGLWidget)->setRotationAxisAngle(x,y,z,a);
#endif
}
//______________________________________________________________________________
void TQtGLViewerImp::SetBackgroundColor(Color_t color)
{
#ifdef QGLVIEWER
  if (fGLWidget) ((TQtGLViewerWidget*)fGLWidget)->SetBackgroundColor(color);
#endif
}
//______________________________________________________________________________
void TQtGLViewerImp::SetBackgroundColor(const TColor *color)
{
#ifdef QGLVIEWER
  if (fGLWidget) ((TQtGLViewerWidget*)fGLWidget)->SetBackgroundColor(color);
#endif
} 
//______________________________________________________________________________
void TQtGLViewerImp::ShowObjectInfo(TObject *obj, const QPoint &cursorPosition)
{
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
#else /* QT_VERSION */
   Q3WhatsThis::display(tipText, globalPosition,tipped);
#endif /* QT_VERSION */

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
   // Display the context menu if any
   if (fWantRootContextMenu) {
      if (!fContextMenu) fContextMenu = new TContextMenu("3DViewContextMenu");
      TObject *obj = view->GetObject();
      if (obj) {
         QPoint pos = tipped->mapToGlobal(cursorPosition);
         fContextMenu->Popup(pos.x(),pos.y(), obj,(TBrowser *)0);
      }
   }
}

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

//______________________________________________________________________________
ULong_t TQtGLViewerImp::GetViewerID() const
{
#if QT_VERSION < 0x40000
   return ULong_t((QMainWindow *) this);
#else /* QT_VERSION */
   return ULong_t((Q3MainWindow *) this);
#endif /* QT_VERSION */
}
//______________________________________________________________________________
void TQtGLViewerImp::SetSnapFileCounter(int counter)
{  fMaxSnapFileCounter = counter;                                              }
