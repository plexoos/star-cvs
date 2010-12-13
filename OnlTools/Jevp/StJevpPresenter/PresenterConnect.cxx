#include "PresenterConnect.h"

#include "JevpLogic.h"
#include "JevpGui.h"

#if QT_VERSION < 0x40000
#  include <qfiledialog.h>
#else /* QT4 */
#  include <q3filedialog.h>
#endif /* QT4 */

#include "TH1.h"
#include "TCanvas.h"

PresenterConnect::PresenterConnect(JevpGui* gui, JevpLogic* pre) :
#if QT_VERSION < 0x40000
  mGui(gui), mPresenter(pre) {
#else /* QT4 */
  QObject(0),mGui(gui), mPresenter(pre) {
#endif /* QT4 */

  connect(mGui,SIGNAL(live()), this, SLOT(live()) ); 
  connect(mGui,SIGNAL(file()), this, SLOT(file()) ); 
  connect(mGui,SIGNAL(update(TCanvas*, int, int )), this, SLOT(update(TCanvas*, int, int )) ); 
  connect(mGui,SIGNAL(update(TCanvas*, const char* )), this, SLOT(update(TCanvas*, const char*)) ); 
  connect(mGui,SIGNAL(update()), this, SLOT(update()) ); 
  connect(mGui,SIGNAL(save()), this, SLOT(save()) ); 
  connect(mGui,SIGNAL(saveAs()), this, SLOT(saveAs()) ); 
  connect(mGui,SIGNAL(print()), this, SLOT(print()) ); 
  connect(mGui,SIGNAL(openReference()), this, SLOT(openReference()) ); 

  connect(mGui,SIGNAL( tab(int) ),         this, SLOT( setTab(int)) ); 
  connect(mGui,SIGNAL( subTab(int) ),      this, SLOT( setSubTab(int)) ); 
  connect(mGui,SIGNAL( canvas(TCanvas*) ), this, SLOT( setCanvas(TCanvas*)) ); 

  connect(this, SIGNAL( signalEventInfo(int,int,int,int, unsigned int, unsigned int,unsigned int, unsigned int) ), mGui, SLOT( setEventInfo(int,int,int,int, unsigned int, unsigned int,unsigned int, unsigned int) ) ); 
  connect(this, SIGNAL( signalServerInfo(ServerStatus*) ), mGui, SLOT( setServerInfo(ServerStatus*) ) ); 
  connect(this, SIGNAL( updateRequest() ), mGui, SLOT( updateRequest() ) );


  //connect(mPresenter, SIGNAL( addGroupTab(const char*) ), mGui, SLOT( addGroupTab(const char*) ) );
  //connect(mPresenter, SIGNAL( addGroup(const char*) ), mGui, SLOT( addGroup(const char*) ) );
  //connect(mPresenter, SIGNAL( removeGroupTabs()), mGui, SLOT( removeGroupTabs()) );
  connect(mPresenter, SIGNAL( setEnabled(bool)) , mGui, SLOT( setEnabled(bool) ) );

  // Gui --> Presenter
  connect(mGui,SIGNAL(printAll(const char*)), mPresenter, SLOT(printAll(const char*)) );
  //connect(mGui,SIGNAL(nextEvent()), mPresenter, SLOT(NextEvent()) );
//  connect(qApp,SIGNAL(lastWindowClosed()),mPresenter,ClosePresenter()));
  mCanvas = 0;
}


void PresenterConnect::save() {
  mPresenter->Save("");
}

void PresenterConnect::saveAs() {
  // QString dir(EvpUtil::GetOutputPath());
  QString dir("");

  QString filter("*.root");
#if QT_VERSION < 0x40000
  QFileDialog dialog( dir, filter, mGui, "", true );
#else /* QT4 */
  Q3FileDialog dialog( dir, filter, mGui, "", true );
#endif /* QT4 */
  dialog.exec();
  if (!dialog.selectedFile().isEmpty()) {
    mPresenter->Save( dialog.selectedFile().ascii() );
  }
}


void PresenterConnect::live() {
  //cout << "liveButton" << endl;
  mPresenter->Stop();   // will restart automatically
  //mPresenter->SetSource();
}

void PresenterConnect::file() {
  //cout << "fileButton" << endl;
  //QString dir(EvpUtil::GetOutputPath());
  QString dir("/home/jml");

  QString caption("File dialog");
#if QT_VERSION < 0x40000
  QFileDialog dialog(dir, QString(), mGui,caption);
#else /* QT4 */
  Q3FileDialog dialog(dir, QString(), mGui,caption);
#endif /* QT4 */
  dialog.setCaption(caption);
  dialog.addFilter("*.root");
  //  dialog.addFilter("*.map");
  dialog.exec();

  QString file = dialog.selectedFile();
  QString mapFile = file;
  int iret = 0;
//  if ( file.find(".map") < 0 ) {   // must be root file, only *.root and *.map are allowed
//    mapFile.replace(".root",".map");
//    iret = EvpUtil::Root2Map(file,mapFile);
//  } 

  if (iret) {
#if QT_VERSION < 0x40000
    cerr << "### error ### Can not open file : " << mapFile << endl;
#else /* QT4 */
    cerr << "### error ### Can not open file : " << mapFile.toStdString() << endl;
#endif /* QT4 */
    return;
  }

  //  mPresenter->SetSource( mapFile.ascii() );
  emit updateRequest();
}

void PresenterConnect::openReference() {
  cout << "Opening reference" << endl;
  //JevpGui* gui2 = new JevpGui(true);
  //gui2->resize(500,500);
  //gui2->show();
//  EvpUtil::ReadCanvasDefinitions();
  //EvpPresenter* presenter2 = new EvpPresenter(EvpUtil::mReference);
  //JevpLogic* presenter2 = new JevpLogic();
  //PresenterConnect* con2 = new PresenterConnect(gui2,presenter2);

//  QString file = "/home/dkettler/test/run10029077.root";
//  QString mapFile = file;
//  mapFile.replace(".root",".map");
//  int iret = EvpUtil::Root2Map(file,mapFile);
//  
//  if (iret) {
//    cerr << "### error ### Can not open file : " << mapFile << endl;
//    return;
//  }

//  QString mapFile = "/a/pplot/histos/run10031084.map";
//  presenter2->SetSource( EvpUtil::mReference );
}
   
void PresenterConnect::update() {
//   if ( mPresenter->serverStatus()->diffTimeInSec() >120.) {
//     if ( mPresenter->Status() ) {
//       live();
//       std::cout << "live again "<< endl;
//     }
//   }
  emit signalEventInfo(mPresenter->run(),0,0,0,0,0,0,0);
  //  emit signalServerInfo(mPresenter->serverStatus());
}


void PresenterConnect::update(TCanvas* canvas, int tab, int subTab) {
  update();
  printf("In update? a \n");
  if (canvas) mPresenter->Draw(canvas,mTab,mSubTab);
}

void PresenterConnect::update(TCanvas* canvas, const char* name) {
  update();
  printf("In update? b\n");
  if (canvas) mPresenter->Draw(canvas, name );
}

void PresenterConnect::print() {
//   //cout << "print" << endl;
//   int tab = mGui->GetTabId();
//   int subTab = mGui->GetSubTabId();
//   TCanvas* cc = mGui->GetCanvas();
//   mPresenter->Print(cc,tab,subTab);
}
   
   
void PresenterConnect::setTab(int t) { mTab = t;} 
void PresenterConnect::setSubTab(int t) { mSubTab = t;} 
void PresenterConnect::setCanvas(TCanvas* t) { mCanvas = t;} 
