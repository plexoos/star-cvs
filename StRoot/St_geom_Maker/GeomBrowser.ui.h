/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/

// Author: Valeri Fine   21/01/2002
/****************************************************************************
** $Id: GeomBrowser.ui.h,v 1.14 2007/03/04 18:50:54 fine Exp $
**
** Copyright (C) 2004 by Valeri Fine.  All rights reserved.
**
** This file may be distributed under the terms of the Q Public License
** as defined by Trolltech AS of Norway and appearing in the file
** LICENSE.QPL included in the packaging of this file.
*****************************************************************************/

#if  ROOT_VERSION_CODE >= ROOT_VERSION(4,03,3)
#include "TVirtualViewer3D.h"
#endif


//_____________________________________________________________________________
class TUpdateList {

private:

   QScrollView *fView;
   bool         fHasBeenLocked;

public: 
   TUpdateList(QScrollView *view) : fView (view),fHasBeenLocked(FALSE) {
      if (fView) {
         fHasBeenLocked = !fView->isUpdatesEnabled();
         if (!fHasBeenLocked) {
            // We should lock the view
            fView->setUpdatesEnabled( FALSE );
            fView->viewport()->setUpdatesEnabled( FALSE );
        }
      }
   }
   ~TUpdateList() {
       if (fView) {
         if (!fHasBeenLocked) {
           // we should unlock the view
            fView->viewport()->setUpdatesEnabled( TRUE );
            fView->setUpdatesEnabled( TRUE );
            fView->repaintContents();
         }
      }
   }
};

//_____________________________________________________________________________
void GeomBrowser::fileNew()
{

}

//_____________________________________________________________________________
void GeomBrowser::fileOpen()
{
   static QString thisCintCommand;
   static QString filetypes = "STAR Geometry macro (*.C);"
                              ";ROOT files (*.root);"
#ifndef  NO_GEANT_MAKER
                              ";GEANT3 Zebra file (*.fz)"
#endif
                              ";";
   QString selectedFilter;
   QString dir = fSaveFileName;
   if (dir.isEmpty()) dir = gSystem->WorkingDirectory();
   else   {
      TString exp((const char*)fSaveFileName);
      gSystem->ExpandPathName(exp);
      dir = exp.Data();
      dir = QFileInfo(dir).dirPath();
   }

   QString fileName = QFileDialog::getOpenFileName (dir
      , filetypes, this, "Open"
      , "Open ROOT file "
      , &selectedFilter);
   if (!fileName.isEmpty()){
      QFileInfo openFile(fileName);
      if (openFile.extension(FALSE).contains("C")) {
         fileOpenMacro(fileName);
      } else if (openFile.extension(FALSE).contains("fz")) {
         fileOpenZebra(fileName);
      } else {
        fileOpenRoot(fileName);
      }
   }
}

//_____________________________________________________________________________
void GeomBrowser::fileSave()
{
  if (!(fSaveFileName.isEmpty()) ) 
       tQtWidget1->Save(fSaveFileName);
   else 
      fileSaveAs();
}
//_____________________________________________________________________________
void GeomBrowser::fileSaveAs()
{
   QString filter = 
      "C++ macro (*.cpp,*.cxx,*.C);"
      ";Postscript (*.ps);"
      ";Encapsulated Postscript (*.eps);"
      ";Scalable Vector Graphics (*.svg);"
      ";ROOT file (*.root);"
      ";Image (";

   UInt_t i;
   for (i = 0; i < QImageIO::outputFormats().count(); i++ ) 
   {
      if (i) filter +=',';
      filter += "*.";
      QString str = QString( QImageIO::outputFormats().at( i ) );
      filter += str.lower();
   }
   filter +=");";
   filter +=";all files (*.*);;";

   QString selectedFilter;
   QString dir = fSaveFileName;
   if (dir.isEmpty()) dir = gSystem->WorkingDirectory(); 
   else               dir = QFileInfo(dir).dirPath();

   QString thatFile = QFileDialog::getSaveFileName(dir
      , filter, this, "SaveAs"
      , "Save the selected Canvas/Pad as"
      , &selectedFilter);

   if (thatFile.isEmpty()) return;
   fSaveFileName = thatFile;

   QString fileNameExtension = QFileInfo(thatFile).extension(FALSE);
   QString  saveType = fileNameExtension.upper();

   tQtWidget1->Save(fSaveFileName,saveType,-1);
}
//_____________________________________________________________________________
void GeomBrowser::filePrint()
{
   QPrinter p;
   if (p.setup()) {
      QPainter pnt(&p);
      pnt.drawPixmap(0,0,tQtWidget1->GetBuffer());
   }
}
//_____________________________________________________________________________
void GeomBrowser::fileExit()
{
  gROOT->ProcessLine(".q");
}
//_____________________________________________________________________________
void GeomBrowser::editUndo()
{

}
//_____________________________________________________________________________
void GeomBrowser::editRedo()
{

}
//_____________________________________________________________________________
void GeomBrowser::editCut()
{

}
//_____________________________________________________________________________
void GeomBrowser::editCopy()
{

}
//_____________________________________________________________________________
void GeomBrowser::editPaste()
{

}
//_____________________________________________________________________________
void GeomBrowser::editFind()
{
#if  ROOT_VERSION_CODE >= ROOT_VERSION(4,01,01)   
   TVirtualViewer3D *viewer = TVirtualViewer3D::Viewer3D(gPad,"ogl");
   if (viewer) {
      // Create Open GL viewer
      TGQt::SetCoinFlag(0);
      viewer->BeginScene();
      viewer->EndScene();
      TQtRootViewer3D *v  = (TQtRootViewer3D*)(viewer);
      if (v) {
        TGLViewerImp *viewerImp = v->GetViewerImp();
        if (viewerImp) 
        {
           connect(&viewerImp->Signals(),SIGNAL( ObjectSelected(TObject *, const QPoint&))
                 , this, SLOT(ObjectSelected(TObject *, const QPoint &)));
       }
      }
    }
#else   
   if (! glViewerLoadFlag) glViewerLoadFlag = !gQt->LoadQt("libRQTGL");
   if (glViewerLoadFlag) {
      // Create Open GL viewer
      TGQt::SetCoinFlag(0);
      tQtWidget1->cd();
      gPad->x3d("OPENGL");
      // tQtWidget1->GetCanvas()->x3d("OPENGL");
   } 
#endif   
   else {
      editFindAction->setEnabled(FALSE);
   }
}
//_____________________________________________________________________________
void GeomBrowser::helpIndex()
{

}
//_____________________________________________________________________________
void GeomBrowser::helpContents()
{

}

//_____________________________________________________________________________
void GeomBrowser::helpAbout()
{

}

//_____________________________________________________________________________
void GeomBrowser::listView1_selectionChanged( QListViewItem *item )
{
    if (item) {
       // fprintf(stderr,"listView1_selectionChanged %s\n", (const char *) item->text(0));
       drawItem(item, 1, tQtWidget2);
#if o      
      TQtObjectListItem* itemRoot =(TQtObjectListItem* )item;
      TObject *obj = itemRoot->Object();
      TObject *objInfo = obj;
      TVolume *volume = dynamic_cast<TVolume *>(obj);
      objInfo = volume->GetShape();
      if (fInspector) delete fInspector;
      fInspector  = (TQtInspectImp *)(gGuiFactory->CreateInspectorImp(objInfo,frame6->width(),frame6->height()));
      // fInspector->Hide();
      fInspector->move(frame6->pos());
      //  fInspector->reparent(frame6,0,QPoint(0,0)); 
      //fInspector->Show();
#endif      
   }
}


//_____________________________________________________________________________
void GeomBrowser::listView1_contextMenuRequested( QListViewItem *item, const QPoint &pos, int col )
{
   if (item && col >= 0) {
      if (!fContextMenu) fContextMenu = new TContextMenu("BrowserContextMenu");
      TQtObjectListItem* that =(TQtObjectListItem* )item;
      if (that->Object())
         fContextMenu->Popup(pos.x(),pos.y(), that->Object(),(TBrowser *)0);
   }
}

//_____________________________________________________________________________
void GeomBrowser::slider1_valueChanged( int val )
{
   drawItem(fCurrentDrawn, val, tQtWidget1 ); 
}
//_____________________________________________________________________________
TQtObjectListItem* GeomBrowser::AddItemToListView( TObject *obj, bool /*expand*/ )
{
   TVolume *volume = 0;
   TQtObjectListItem* item = 0;
   if (       obj->IsA() == TGeometry::Class()) {
      volume = new TVolume(*((TGeometry*)obj)->GetCurrentNode());
   } else if (obj->IsA() == TGeoManager::Class()) {
      volume = TGeoDrawHelper::MakeVolume(((TGeoManager*)obj)->GetTopVolume());
      fGeoManager2Delete = (TGeoManager*)obj;
   } else if (obj->IsA() == TGeoVolume::Class()) {
      volume = TGeoDrawHelper::MakeVolume((TGeoVolume*)obj);
   } else if (obj->IsA() == TVolume::Class()) {
      volume = (TVolume *)obj;
   }
   if(volume) {
      item = new TQtObjectListItem(volume,listView1, volume->GetName(), QCheckListItem::CheckBox);
      item->setText(1,volume->GetTitle());
      item->setText(3,volume->ClassName());
      item->setExpandable(volume->GetListSize());
      //     item->setPixmap(0 , QPixmap::fromMimeSource( "h2_t.png"));
      SetVisibility(item, volume->GetVisibility());
      listView1->setOpen(item,true);
   }
   return item;
}


//_____________________________________________________________________________
void GeomBrowser::listView1_expanded(QListViewItem *item)
{
   if (item) {
      // fprintf(stderr,"listView1_expanded %s\n", (const char *) item->text(0));
      TQtObjectListItem* itemRoot =(TQtObjectListItem* )item;
      TObject *obj = itemRoot->Object();
      TVolume *volume = dynamic_cast<TVolume *>(obj);
      
      TUpdateList listLock(item->listView());

      TDataSetIter next(volume);
      TVolume *child = 0;
      while ( (child = (TVolume *)next()) ) {
         TQtObjectListItem* itemChild = new TQtObjectListItem(child,itemRoot,child->GetName(),QCheckListItem::CheckBox);     
         itemChild->setText(1,child->GetTitle());
         itemChild->setText(3,child->ClassName());
         itemChild->setExpandable(child->GetListSize());
         SetVisibility(itemChild, child->GetVisibility());
      }
   }
}

//_____________________________________________________________________________
void GeomBrowser::listView1_onItem( QListViewItem *item )
{
   if (item) {  
      TQtObjectListItem* itemRoot =(TQtObjectListItem* )item;
      TObject *obj = itemRoot->Object();
      if (obj && gPad) {
         QString m = "The ";
         TVolume::ENodeSEEN s = ((TVolume *)obj)->GetVisibility();
         if ( s & TVolume::kThisUnvisible) m += "in";
         m += "visible volume: ";
         const char *info = obj->GetObjectInfo(gPad->XtoPixel(0),gPad->YtoPixel(0));
         if (info) m += info;

         statusBar()->message(m);            
      }
#if 0      
      TVolume *volume = dynamic_cast<TVolume *>(obj);
      if (fInspector) delete fInspector;
      fInspector  = dynamic_cast<TQtInspectImp *>(gGuiFactory->CreateInspectorImp(obj));
      fInspector->reparent(this);
      fInspector->Show();
#endif      
#ifndef WIN32         
      gSystem->ProcessEvents();
#endif         
   }
}

//_____________________________________________________________________________
void GeomBrowser::init()
{
#ifndef WIN32
   if (gSystem->Load("libTable")) 
   {
      QString errorMsg = "Can not load the ROOT shared library: ";
      errorMsg += "libTable.so";
      QMessageBox::critical(this, "ROOT shared library load error", errorMsg
         , QMessageBox::Ok,QMessageBox::NoButton );

   }
   if (gSystem->Load("libGeom")) 
   {
      QString errorMsg = "Can not load the ROOT shared library: ";
      errorMsg += "libGeom.so";
      QMessageBox::critical(this, "ROOT shared library load error", errorMsg
         , QMessageBox::Ok,QMessageBox::NoButton );

   }
#endif
#ifdef  NO_GEANT_MAKER
   comboBox2->setEnabled(FALSE);
   comboBox2->hide();
#else
   // Create a text editor
   fTextEdit = new TextEdit(this);
   fTextEdit->setCaption( "GEANT3 geometry editor" );
   fTextEdit->resize( 640, 800 );
   // populate the standard geometry type   
   // create the STAR search path
   QString file("pams/geometry/geometry/geometry.g");
   QString path(".:$STAR");
   connect(fTextEdit,SIGNAL(textSaved(const QString &)),this,SLOT(RemakeGeom(const QString &)));
   const char *found = gSystem->Which((const char*)path, (const char*)file);
   if (found) {
     if (found[0]) {
       QFile geomFile(found);
       if (geomFile.open(IO_ReadOnly)) {
       QString line;
       QRegExp exp("^\\s+\\bon\\b.+");
       while (geomFile.readLine(line, 40) >=0 ) 
       {
          if (line.contains( exp ) ) {
             line = line.simplifyWhiteSpace ();
             QString tag = line.section(' ',1,1);
             // printf(" new tag found --<%s>-- \n --<%s>--\n",(const char*) line, (const char*)tag);
             // remove "on"
             tag.remove("on");
             tag.stripWhiteSpace();
             // exceptions:
             //    " HELP NO_BREM LOW_EM TPC_ ONLY"
             if (!(tag.isEmpty()
                 || tag.contains("COMPLETE") 
                 || tag.contains("HELP") 
                 || tag.contains("_ON") 
                 || tag.contains("_OFF") 
                 || tag.contains("NO_BREM") 
                 || tag.contains("LOW") 
                 || tag.contains("TPC_")
                 || tag.contains("DUMM")
                 ||  tag.contains("ONLY") ) ) {
              // add to the combo box
                  comboBox2->insertItem(tag);
                 // printf(" new tag found --<%s>--\n", (const char*)tag);
             } 
          }         
       }
     } else {
        assert(0);
     } 
   }
 }
 delete [] found;

#endif
   // remove tmp Coin file
   QFileInfo tmpInfo(QDir::currentDirPath() ,"GeomBrowser_tmp.iv");
   QFile tmp(tmpInfo.absFilePath ());
   tmp.remove();
   fGeometryLoaded = false;
   fChain          = 0;
   fGeant          = 0;
   fInspector       = 0;
   fContextMenu     = 0;
   fCurrentDrawn    = 0;
   fGeoManager2Delete = 0;
   fCurrentViewer     = 0;
   glViewerLoadFlag = false;
   fFile            = 0;
   fIconSet = Action->iconSet();
//   fIconSet.setPixmap(QPixmap::fromMimeSource("wirebox.xpm"),QIconSet::Automatic);

   // Connect the QLineEditor with the ROOT command interpreter
   connect(comboBox1->lineEdit(),SIGNAL(returnPressed ()), SLOT( rootCommandExecute()) );
   connect(comboBox1->lineEdit(),SIGNAL(returnPressed ()), comboBox1->lineEdit(), SLOT( clear()) );

   // Attach the validator to facilitate the ROOT <tab> completion
   // fTabCompValidator = new TQtTabValidator(comboBox1);
   // comboBox1->setValidator(fTabCompValidator);

   // Predefine some ROOT commands
   comboBox1->insertItem("gGeometry->SetBomb(1.7);");
   comboBox1->insertItem("gPad->SetFillColor(kBlack);");
   comboBox1->insertItem("gPad->SetFillColor(kWhite);");
   comboBox1->insertItem(".qqqqqq");
   comboBox1->insertItem(".q");
   spinBox1->setValue(3);
   tQtWidget1->GetCanvas()->SetFillColor(kBlack); 
   // do we have the QGLViewer?
   char *libRQTGL = 0;
   libRQTGL = gSystem->DynamicPathName("libRQTGL",kTRUE);
      editFindAction->setEnabled(libRQTGL);
    //  strange why it crashes like the heel 
    //  if (libRQTGL) delete [] libRQTGL;
      libRQTGL = 0;

   // do we have the Coin3D viewer?
   libRQTGL = gSystem->DynamicPathName("libRQIVTGL",kTRUE);
      editView_Coin3DAction->setEnabled(libRQTGL);
      // if (libRQTGL) delete [] libRQTGL;

   fCurrentWidget = tQtWidget1; fCurrentWidget->cd();

   // Open pre-defined ROOT file
   TKey *key = 0;
   char **argv = qApp->argv();
   if (qApp->argc() > 1 && QFileInfo(argv[1]).isReadable() ) {

      fSaveFileName = argv[1];
      printf(" file %s\n", (const char *)fSaveFileName);
      fFile=new TFile(fSaveFileName);
      fSaveFileName = "";
      
      TUpdateList listLock(listView1);

      TIter next(fFile->GetListOfKeys());
      // Fill the "tree view" with the object info from the file
      int countObject = 0;
      while((key = (TKey*) next())) {
         if (AddItemToListView(key->ReadObj(),true)) countObject++;
      }
      if (!countObject)   
           QMessageBox::warning(this,"Open ROOT geometry file","No 3D object was found here");
 
      // Create a separate Wwigdet with ROOT Object browser, Just in case
      //  new TBrowser();
   } else {
      //   fileOpen();
     fSaveFileName = "$STAR/StarDb/VmcGeometry/";
     if (gGeoManager) {
        TUpdateList listLock(listView1);
        AddItemToListView(gGeoManager->GetTopVolume(),true);
      }
   }
}

//_____________________________________________________________________________
void GeomBrowser::dtor()
{
    delete fFile; fFile = 0;
    delete fContextMenu; fContextMenu = 0;
}
//_____________________________________________________________________________
void GeomBrowser::rootCommandExecute()
{
   // TQtTabValidator::Clear();
   gROOT->ProcessLine(comboBox1->lineEdit()->text());
   tQtWidget1->Refresh();
   gROOT->ProcessLine("gPad->Update();");
}
//_____________________________________________________________________________
void GeomBrowser::listView1_clicked( QListViewItem *item )
{
   if (item) {
      TQtObjectListItem* itemRoot =(TQtObjectListItem* )item;
      TObject *obj = itemRoot->Object();
      TVolume *volume = dynamic_cast<TVolume *>(obj);

      // check visibility
      if (volume) {
         TVolume::ENodeSEEN s;
#if (QT_VERSION > 0x030100)
         switch ( itemRoot->state() ){
            case QCheckListItem::On      : s = TVolume::kBothVisible;   break;
            case QCheckListItem::NoChange: s = TVolume::kThisUnvisible; break;
            case QCheckListItem::Off     : s = TVolume::kNoneVisible;   break;
            default:                       s = TVolume::kBothVisible;   break;
               //      case TVolume::kSonUnvisible: s = QCheckListItem::Off; break;
         };
#else
         s = ( itemRoot->isOn() ) ? TVolume::kBothVisible : TVolume::kThisUnvisible ;
#endif         
         // set visibility
         if (volume->GetVisibility() != s) {
            volume->SetVisibility(s);

            QString m = "The ";
            if ( s & TVolume::kThisUnvisible) m += "in";
            m += "visible volume: ";
            m += obj->GetObjectInfo(gPad->XtoPixel(0),gPad->YtoPixel(0));

            statusBar()->message(m);           

            // adjust 1 level view
            if (item == item->listView()->selectedItem() ) 
               drawItem(item, 1, tQtWidget2);

            // adjust multi-level  view
            if (fCurrentDrawn) {
               if ( item == fCurrentDrawn) {
                  drawItem(fCurrentDrawn, spinBox1->value(), tQtWidget1 ); 

               } else if ( item->depth() > fCurrentDrawn->depth() ) {
                  TObject *rootObject = ((TQtObjectListItem* )fCurrentDrawn)->Object() ;
                  QListViewItemIterator it(fCurrentDrawn);
                  while ( it.current() ) {
                     if (rootObject == (((TQtObjectListItem*)it.current())->Object()) ){
                        tQtWidget1->Refresh();

                        //                     drawItem(fCurrentDrawn, spinBox1->value(), tQtWidget1 );
                        break;
                     }
                  }
               }
            }
         }
      }
   }
}
//_____________________________________________________________________________
void GeomBrowser::listView1_collapsed( QListViewItem *item )
{
   if (item) {
      // check fCurrentDrawn
      TUpdateList listLock(item->listView());

      QListViewItemIterator it(item);
      while ( it.current() ) {
         if ( (it.current() != item) && (it.current() == fCurrentDrawn)  ) 
         {   fCurrentDrawn =0;  break;   }
         ++it;
      }
      // clean the collapsed branch
      QListViewItem* child = 0;;
      while ( (child  =  item->firstChild()) ){
            delete child;
      }
      if (!item->depth()) item->setOpen(true);
   }
}

//_____________________________________________________________________________
void GeomBrowser::listView1_doubleClicked( QListViewItem *item )
{
   if (item && fCurrentDrawn != item) {
      // remove the old mark
      { 
        TUpdateList listLock(item->listView());
        
        TObject  *obj = 0;
        const QIconSet  *set = 0;
        if ( fCurrentDrawn ) {
           obj = ((TQtObjectListItem *)fCurrentDrawn)->Object();
           if (obj) {
              if (obj->InheritsFrom(TVolume::Class()) )
                 set =   TQtIconBrowserImp::Shape2GeoShapeIcon(((TVolume *)obj)->GetShape()->ClassName());
              else if (obj->InheritsFrom(TVolumeView::Class()) )
                 set =  TQtIconBrowserImp::Shape2GeoShapeIcon(((TVolumeView *)obj)->GetShape()->ClassName());
              else if (obj->InheritsFrom(TShape::Class()) ) 
                set =  TQtIconBrowserImp::Shape2GeoShapeIcon(((TShape *)obj)->ClassName());
              else if (obj->InheritsFrom(TGeoVolume::Class()) ) 
                 set = TQtIconBrowserImp::IconList()->GetIcon(((TGeoVolume *)(obj))->GetShape()->GetName());
           }
           fCurrentDrawn->setPixmap(0,set ? set->pixmap(QIconSet::Small,true,QIconSet::On) : QPixmap());
           fCurrentDrawn->setPixmap(1,QPixmap());
        }

         fCurrentDrawn = item;
         obj = ((TQtObjectListItem *)fCurrentDrawn)->Object();
         if (obj) {
            if (obj->InheritsFrom(TVolume::Class()) )
              set =   TQtIconBrowserImp::Shape2GeoShapeIcon(((TVolume *)obj)->GetShape()->ClassName());
            else if (obj->InheritsFrom(TVolumeView::Class()) )
               set =  TQtIconBrowserImp::Shape2GeoShapeIcon(((TVolumeView *)obj)->GetShape()->ClassName());
            else if (obj->InheritsFrom(TShape::Class()) )
               set =  TQtIconBrowserImp::Shape2GeoShapeIcon(((TShape *)obj)->ClassName());
            else if (obj->InheritsFrom(TGeoVolume::Class()) )
               set = TQtIconBrowserImp::IconList()->GetIcon(((TGeoVolume *)(obj))->GetShape()->GetName());
         }
        // highlight the new item
         fCurrentDrawn->setPixmap(0,fIconSet.pixmap(QIconSet::Small,true,QIconSet::On)); 
//       fCurrentDrawn->setPixmap(0,QPixmap::fromMimeSource("arrow_right.xpm")); 
         fCurrentDrawn->setPixmap(1,set ? set->pixmap(QIconSet::Small,true,QIconSet::On) 
                                        : QPixmap::fromMimeSource("arrow_left.xpm")); 
      }
      drawItem(fCurrentDrawn, spinBox1->value(), tQtWidget1 ); 
   }
}

//_____________________________________________________________________________
void GeomBrowser::drawItem( QListViewItem *item, int depth, TQtWidget *view )
{
  if (item) {
      TQtObjectListItem* itemRoot =(TQtObjectListItem* )item;
      TObject *obj = itemRoot->Object();
      TVolume *volume = dynamic_cast<TVolume *>(obj);
      if(volume && view) {
          QCursor currentCursor = cursor();
          if (!depth) depth = 3;
          // Check the item deep visibility
          TVolume::ENodeSEEN s = volume->GetVisibility();
          TDataSet  *set = volume;
          if ( s & TVolume::kThisUnvisible ) {
             TDataSetIter nextVolume(volume, depth);
             set = 0;
             while ( (set = nextVolume())  ) {
                s = ((TVolume *)set)->GetVisibility();
                if ( !(s & (TVolume::kThisUnvisible) )) break;
             }
          }
          // setCursor(Qt::WaitCursor);
          frame3->setEnabled(false);
          view->cd();
          if (set) {
             volume->Draw((const char *)QString().setNum(depth));
          } else
             view->GetCanvas()->Clear();
          view->Refresh();
          
          frame3->setEnabled(true);
          
         // setCursor(currentCursor);
#ifndef WIN32
          // gSystem->ProcessEvents();
#endif
       } 
   }
}

//_____________________________________________________________________________
void GeomBrowser::SetVisibility( TQtObjectListItem * item, TVolume::ENodeSEEN vis )
{
   // Convert the the visbility status to the checkmark  
   if (item) {
#if (QT_VERSION > 0x030100)
      item->setTristate(item->isExpandable());
      QCheckListItem::ToggleState s = QCheckListItem::Off;
      switch (vis) {
            case TVolume::kBothVisible:   s = QCheckListItem::On;       break;
            case TVolume::kSonUnvisible:  s = QCheckListItem::On;       break;
            case TVolume::kThisUnvisible: s = item->isExpandable() ? 
                                             QCheckListItem::NoChange : QCheckListItem::Off;
                                                                        break;
            case TVolume::kNoneVisible:   s = QCheckListItem::Off;      break;
      };
      item->setState(s); 
#else
      bool s = true;
      switch (vis) {
            case TVolume::kBothVisible:   s = true; break;
            case TVolume::kSonUnvisible:  s = true; break;
            case TVolume::kThisUnvisible: s = false; break;
            case TVolume::kNoneVisible:   s = false; break;
      };
      item->setOn(s); 
#endif   
   }
}


//_____________________________________________________________________________
TVirtualViewer3D *GeomBrowser::viewCoin3D()
{
// 
#if  ROOT_VERSION_CODE >= ROOT_VERSION(4,03,3) 
   if (fCurrentViewer) 
      ((TQtRootViewer3D*)fCurrentViewer)->DisconnectPad();
   fCurrentViewer = TVirtualViewer3D::Viewer3D(tQtWidget1->GetCanvas(),"oiv");
   if (fCurrentViewer) {
       // Create Open GL viewer
       TGQt::SetCoinFlag(1);
       fCurrentViewer->BeginScene();
       fCurrentViewer->EndScene();
       TQtRootViewer3D *v  = (TQtRootViewer3D*)(fCurrentViewer);
       if (v) {
           TGLViewerImp *viewerImp = v->GetViewerImp();
           if (viewerImp) 
           {
               connect(&viewerImp->Signals(),SIGNAL( ObjectSelected(TObject *, const QPoint&))
                 , this, SLOT(ObjectSelected(TObject *, const QPoint &)));
           }
       }
    } else {
         editView_Coin3DAction->setEnabled(false);
    }
    return fCurrentViewer;
#else  
   if (! glViewerLoadFlag) glViewerLoadFlag = !gQt->LoadQt("libRQTGL");
   if (glViewerLoadFlag) {
      TGQt::SetCoinFlag(1);
      if (fCurrentDrawn){
         TQtObjectListItem* itemRoot =(TQtObjectListItem* )fCurrentDrawn;
         OpenInventorConvertor(dynamic_cast<TVolume *>(itemRoot->Object()));
      }
      gPad->x3d("OPENGL");
   } 
#endif   
}


//_____________________________________________________________________________
void GeomBrowser::OpenInventorConvertor( TVolume *volume )
{
   if (volume) {
      static bool load = false;
      if (!load && (gROOT->LoadMacro("r2iv_1.C") == 0) ) load = true;
      if (load) {
         static bool setcut = false;
         static int currentCut = -1;
         char buff[100];
         if (!setcut) {
          // gROOT->ProcessLineSync("TIVShape::SetCut(3);");
           setcut=true;
         }
         // get the current cut
         //  sprintf(buff,"*(int *)0x%p = TIVShape::GetCut();",&currentCut);
         gROOT->ProcessLineSync(buff);
         if (currentCut > 0) {
            TVolumeView view(*volume,spinBox1->value(),0, TDataSet::kAll);
            view.MarkAll();
//            sprintf(buff,""
//               "TR2iv riv((TVolumeView *)0x%p, 1, %d);"
//               "riv.Draw(\"GeomBrowser_tmp.iv\");", &view, spinBox1->value());	
//              "riv.Draw(\"GeomBrowser_tmp.iv\");", &view, spinBox1->value(),3);	
            gROOT->ProcessLineSync(buff);
         } else {
            // remove tmp Coin file
            QFileInfo tmpInfo(QDir::currentDirPath() ,"GeomBrowser_tmp.iv");
            QFile tmp(tmpInfo.absFilePath ());
            tmp.remove();
         }
      }
   }
}

//_____________________________________________________________________________
void GeomBrowser::OpenInventorConvertor( TGeoVolume *volume )
{
   if (volume) {
   }
}

//_____________________________________________________________________________
void GeomBrowser::fileOpenMacro( const QString &fileName )
{
  // Read ROOT macro with the ROOT TGeo geometry model
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
  if (!gROOT->LoadMacro((const char *)fileName)) {
      TDataSet *topSet = (TDataSet *)gROOT->ProcessLineFast("CreateTable()");
      if (topSet) {
         // Look for the "Geometry" set
         TUpdateList listLock(listView1);
         TQtObjectListItem* item = AddItemToListView(gGeoManager,true);
         if (item) {
            // rename it
            QString name = QFileInfo(fileName).baseName(TRUE);
            item->setText(0,name); 
         }
      }
      fOpenFileName = fileName;
   } else {
       QMessageBox::warning(this,"Open ROOT macro geometry file","Can not open the macro file");
   }
   if (fGeoManager2Delete) {
     delete fGeoManager2Delete;fGeoManager2Delete = 0;
     gGeoManager = 0;
   }
   QApplication::restoreOverrideCursor();
}

//_____________________________________________________________________________
void GeomBrowser::fileOpenRoot( const QString &fileName )
{
  QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
  if (fFile) delete fFile;
     QString base = QFileInfo(fileName).baseName(TRUE); 
     fFile = TFile::Open((const char *)fileName);
     if (!fFile->IsZombie()) {
        TKey *key = 0;
        TIter next(fFile->GetListOfKeys());
        // Fill the "tree view" with the object info from the file
 
        TUpdateList listLock(listView1);

        int countObject = 0;
        while((key = (TKey*) next())) {
           TObject *o = key->ReadObj();
           TQtObjectListItem* item = AddItemToListView(o,true);
           if (item) {
              // rename it
              QString name = base;
              name += ".";
              name += o->GetName();
              item->setText(0,name);
              //count it
              countObject++;              
           }
        }
        if (!countObject)   
           QMessageBox::warning(this,"Open ROOT geometry file"
                                    ,"No 3D object was found here");
        fOpenFileName = fileName;
     } else {
        delete fFile; fFile=0;
        QString errorMsg = "Zombie ROOT file: ";
        errorMsg += fileName;
        QMessageBox::critical(this, "ROOT file access error", errorMsg
            , QMessageBox::Ok,QMessageBox::NoButton );
     }
     if (fGeoManager2Delete) {
        delete fGeoManager2Delete;fGeoManager2Delete = 0;
        gGeoManager = 0;
     }
     QApplication::restoreOverrideCursor();
}


//_____________________________________________________________________________
void GeomBrowser::ReloadFileAction_activated()
{
  // Block the widgets update 
   QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
   
   TUpdateList listLock(listView1);

   tQtWidget1->setUpdatesEnabled( FALSE );
   tQtWidget2->setUpdatesEnabled( FALSE );
   fCurrentDrawn = 0;   
   tQtWidget1->GetCanvas()->Clear();
   tQtWidget2->GetCanvas()->Clear();
   tQtWidget1->GetCanvas()->Update();
   tQtWidget2->GetCanvas()->Update();
   // clean 
   QListViewItem* child = listView1->firstChild();
   while ( child ) {
      TQtObjectListItem *geoRootItem = dynamic_cast<TQtObjectListItem*>(child);
      if (geoRootItem){
         delete geoRootItem->Object();
         geoRootItem->SetObject(0);
      }
       child = child->nextSibling();
   }
   listView1->clear();
   if (!fOpenFileName.isEmpty()){
      QFileInfo openFile(fOpenFileName);
      if (openFile.extension(FALSE).contains("C"))
         fileOpenMacro(fOpenFileName);   
      else {
         fileOpenRoot(fOpenFileName);
      }
   } else {
     fileOpen();
   }
   tQtWidget1->setUpdatesEnabled( TRUE );
   tQtWidget2->setUpdatesEnabled( TRUE );
   if (fGeoManager2Delete) {
       delete fGeoManager2Delete;fGeoManager2Delete = 0;
       gGeoManager = 0;
   }
   QApplication::restoreOverrideCursor();
}

//_____________________________________________________________________________
void GeomBrowser::ObjectSelected( TObject *obj, const QPoint &)
{
   // [slot] to accept the selected object to expand the ListTreeView
   
   if ( obj->InheritsFrom(TDataSet::Class()) ) {
      // suspend the list view update
      
      TUpdateList listLock(fCurrentDrawn->listView());

      TObject  *topObject = ((TQtObjectListItem *)fCurrentDrawn)->Object();
      QValueStack<TObject *> items;
      items.push(obj);
      TDataSet *set = (TDataSet *)obj;
      while ( (set = set->GetParent()) ) {
         items.push(set);
         // fprintf(stderr," 1. GeomBrowser::ObjectSelected %s %p %p \n", set->GetName(),set,topObject );
         if (((TObject *)set) == topObject) break;
      }
      if (set) {
         // Found the whole path, now roll it back
         const TQtObjectListItem *selectedItem = (TQtObjectListItem *)fCurrentDrawn;
         while ( !items.isEmpty() )  {
            topObject = items.pop();
            const TQtObjectListItem *nextSelectedItem =  (const TQtObjectListItem *)selectedItem->Find(topObject);
             // fprintf(stderr," 2. GeomBrowser::ObjectSelected %s %p %p\n", topObject->GetName(),topObject, nextSelectedItem);
            if (nextSelectedItem) {
               if ( !nextSelectedItem->isOpen() ) ((TQtObjectListItem *)nextSelectedItem)->setOpen(true);
               selectedItem = nextSelectedItem;
            }
         }
         listView1->ensureItemVisible ((TQtObjectListItem *)selectedItem );
         listView1->setSelected((TQtObjectListItem *)selectedItem,TRUE );
     }
#ifndef NO_GEANT_MAKER
#if 1    
     QString srcFile=(const char*)Geant().GetVolumeSrcFile(obj->GetName());
     if (!srcFile.isEmpty()) {
        fTextEdit->show();
        fTextEdit->load(srcFile);
//        QRect itemRec = listView1->itemRect(listView1->selectedItem());
//        QPoint pos(itemRec.x(),itemRec.y());
//        pos = listView1->mapToGlobal(pos);
//        QWhatsThis::display(srcFile,pos);
      }
#endif      
#endif     
   }
}

#ifndef NO_GEANT_MAKER
  static int Geant3Init = 0;
#endif
//_____________________________________________________________________________
void GeomBrowser::STAR_geometry_activated( const QString &geoVersion )
{
#ifndef NO_GEANT_MAKER
   QString kuipCmd  = "detp geometry ";
   kuipCmd         +=  geoVersion;
   QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
   // fprintf(stderr,"STAR_geometry_activated <%s>\n", (const char*)kuipCmd );
   if (!fGeometryLoaded)  fGeometryLoaded = !gSystem->Load("geometry");
   if (fGeometryLoaded) {
      // pre-define the kuipCommand to used within St_geant::Geometry method
      Geant().LoadGeometry((const char*)kuipCmd );
      if (Geant3Init) {
          Geant().Do("gdrop all"); // To allow calling Init more then one time;
          Geant().Do((const char *)kuipCmd); 
          if (false) 
             Geant().Do("make geometry"); 
          else 
             Geant().Geometry();
         // Geant().Do("gclose all");
      } else {          
         fChain->Init(); Geant3Init = 1;     
      }
      // comboBox2->setEnabled(FALSE); // we can communicate GEANT one time ony :(
      TVolume *v = dynamic_cast<TVolume *>(Geant().GetDataSet("HALL"));
      if (v) {
         // Make CAVE invisible
         // v->SetName((const char*)geoVersion);
         TVolume *cave = (TVolume *)v->FindByName("CAVE");
         if (cave) {
            cave->SetVisibility(TVolume::ENodeSEEN(2));
         }
         TVolume *hall = (TVolume *)v->FindByName("HALL");
         if (hall) {
              hall->SetVisibility(TVolume::ENodeSEEN(2));
              v = hall;
         }
         gPad->SetFillColor(kBlack);
         TUpdateList listLock(listView1);
         TQtObjectListItem* item = AddItemToListView(v,true);
         if (item) item->setText(0,geoVersion);
      }
   }
   QApplication::restoreOverrideCursor();
#else
  if   (geoVersion.isEmpty() ){}    
#endif
}

//_____________________________________________________________________________
St_geant_Maker & GeomBrowser::Geant() 
{
#ifndef NO_GEANT_MAKER
    if (!fGeant) {
       gSystem->Load("St_base");
       gSystem->Load("StChain");
       gSystem->Load("St_Tables");
       gSystem->Load("St_g2t.so");
       gSystem->Load("StarMagField");
       gSystem->Load("St_geant_Maker");  
       gSystem->Load("StUtilities");
       
       fChain = new StChain(); 
       fGeant = new St_geant_Maker();
       // fChain->Init();
    }
#endif 
    return *fGeant;
}


//_____________________________________________________________________________
void GeomBrowser::fileOpenZebra( const QString &fileName )
{
   // fprintf(stderr,"fileOpenZebra <%s>\n", (const char*)kuipCmd );
#ifndef NO_GEANT_MAKER
   QString kuipCmd  = "gfile p  ";
   kuipCmd         +=  fileName;
   QApplication::setOverrideCursor( QCursor(Qt::WaitCursor) );
   Geant().SetInputFile(fileName);
   if (Geant3Init) {
       Geant().Do("gdrop all"); // To allow calling Init more then one time;
       Geant().Do((const char *)kuipCmd); 
       Geant().Do("gclose all");
   } else {
      fChain->Init(); Geant3Init = 1;
   }
   // comboBox2->setEnabled(FALSE); // we can communicate GEANT one time ony :(
   QApplication::restoreOverrideCursor();
#else
   if (fileName.isEmpty()) {}
   comboBox2->setEnabled(FALSE); // we can communicate GEANT one time ony :(
#endif
}


//_____________________________________________________________________________
void GeomBrowser::fileOpenInventor( const QString &fileName )
{
   // Open the new Coin widget and feed the IV file there
   if (editView_Coin3DAction->isEnabled() ) {
      TVirtualViewer3D *viewer = viewCoin3D(); 
      if (viewer) {
         TQtRootViewer3D *v  = (TQtRootViewer3D*)(viewer);
         if (v) {
            TGLViewerImp *viewerImp = v->GetViewerImp();
            if (viewerImp) viewerImp->ReadInputFile((const char*)fileName);
         }
      }
   }
}

//_____________________________________________________________________________
void GeomBrowser::RemakeGeom( const QString &) 
{
   Geant().SetRemake(kTRUE); 
}
