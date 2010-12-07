#ifndef _JEVPGUI_H_
#define _JEVPGUI_H_

#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#include <TString.h>
#include <TObjString.h>
#include <TObjArray.h>
#include <TRegexp.h>
#include <string>
#include <fstream>
#include <sstream>

#include "TQtRootAction.h"

#include <qobject.h> 
#include <TQtWidget.h>
#include <qtabwidget.h>
#include "Jevp/StJevpPlot/JevpPlot.h"



#  include <q3intdict.h> 
#  include <q3mainwindow.h>
#  include <Q3Frame>

class JevpLogic;



#if QT_VERSION < 0x40000
  class QToolBar;
  class QHBox;
  class QProgressBar;
#else /* QT4 */
  class Q3ToolBar;
  class Q3HBox;
  class Q3ProgressBar;
#endif /* QT4 */

class QTabWidget ;
class QPushButton;
class QButton;
class TCanvas;
class QMenuBar;
class QWidget;
class QLineEdit;
class EventInfo;
class ServerInfo;
class ServerStatus;
class TriggerDetectorBitsInfo;
class TCanvas;
class TObject;
class TQtWidget;
class TQtZoomPadWidget;


using namespace std;

enum { kFileChangeToRun, kFileLive, kFilePrint, kFilePrintAll, kHelpAbout, kUpdate, kAutoUpdate, kToolBar, kOnlPrinter2 }; 

class JevpScreenWidget : public  TQtWidget {
public:
  QTabWidget *parentMenu;
  std::string *plot;
  u_int combo_index;
  TList *plots;
  TList *jevpPlots;

  void addJevpPlot(JevpPlot *plot) {  jevpPlots->Add(plot); }
  JevpPlot *getJevpPlot(char *name);
  void addPlot(TObject *plot) { plot->SetBit(kCanDelete); plots->Add(plot); };
  void Clear() { 
    printf("clear\n");
    plots->Clear(); 
    jevpPlots->Clear();
    printf("more clear\n");
    GetCanvas()->Clear(); 
    printf("clearest\n");
  };
  JevpScreenWidget(char *tabname, char *plotname, u_int combo_index, QTabWidget *menu);
  virtual ~JevpScreenWidget();

  //TObject *getPlot
  
  void mousePressEvent(QMouseEvent *e);
};


// ************************************************************************
class JevpGui : public Q3MainWindow {
     
  Q_OBJECT

 public:
    EventInfo* mEventInfo;
    ServerInfo* mServerInfo;
    TriggerDetectorBitsInfo* mTriggerDetectorBitsInfo;
    QPushButton* fStarLogo;
		  
private:
    int mWidth;
    int mHight;
    char mPsName[1024];

    JevpLogic *logic;

#if QT_VERSION < 0x40000
    QFrame* mBitsFrame;
#else /* QT4 */
    Q3Frame* mBitsFrame;
#endif /* QT4 */
    QTabWidget* fTab;
    QTabWidget* fStaticTab;
    QTabWidget* fDynamicTab;
#if QT_VERSION < 0x40000
    QToolBar* fToolBar;
#else /* QT4 */
    Q3ToolBar* fToolBar;
#endif /* QT4 */
    QLineEdit* mRun;
    QLineEdit* mCount;
    QLineEdit* mEvent;
    QLineEdit* mToken;
#if QT_VERSION < 0x40000
    QHBox* mCentralWidget;
    QProgressBar* fProgressBar;
#else /* QT4 */
    Q3HBox* mCentralWidget;
    Q3ProgressBar* fProgressBar;
#endif /* QT4 */
    //Array of tabs
    // Fist index upper_level tabs
    // Second index subtabs
    //
    // Number of Subtabs for a given Tab
    //int nSubTabs[MAX_TABS];
    //sep

    // char *TabNames[MAX_TABS][MAX_SUBTABS];

    // int  nx[MAX_TABS][MAX_SUBTABS];      // number of horizontal pads on a canvas
    // int  ny[MAX_TABS][MAX_SUBTABS];      // number of vertical pads on a canvas
    //int  nHist[MAX_TABS][MAX_SUBTABS];   // number of histograms for a canvas
    //const char* hNames[MAX_TABS][MAX_SUBTABS][MAX_PADS];  // histogram names
    //TString myNames[MAX_TABS][MAX_SUBTABS][MAX_PADS]; // new array for histogram Names

    int mTab;
    int mSubTab;

    TQtZoomPadWidget *mZoomer;
    //-----------------------------------

    //TString mDefaultPSFilePath; // default path for PS files
    //TString mHistoPSFile;       // file name for PostScript file
    //TString mHistoGIFFile;       // file name for GIF file
    //TString mHistoPDFFile;       // file name for PDF file
    //TString mHistoFileNameShort;  // file name without extension, but with path

    int mDebugLevel;


    int IntFromTString(const TString &ts);
    float FloatFromTString(const TString &ts);
    int ParseString (const TString &tChain, TObjArray &Opt);

    int file_uploaded; // flag if pdf file was uploaded into database; yes=1, no=0, error=-1

    TString mStrLive;
    TString mStrFile;
    TString mStrRun;
    TString mStrStop;

    Q3IntDict<TQtRootAction> fActions;

    QMenuBar *fMenuBar;
    
    Int_t   fGuiRefreshRate; // msec.

    void buildTabs(QWidget *container);
    void fillTab(QTabWidget *tab, u_int idx);

    JevpScreenWidget *currentScreen;
  

 protected:
    void StopEventLoop();
 public:

    JevpGui(JevpLogic *logic, bool isRefWindow = false);
    virtual ~JevpGui();

    // getters for static tabs
    int GetTabId();
    int GetSubTabId();
    int GetWidth() { return mWidth;}
    int GetHight() { return mHight; }
    //TCanvas* GetCanvas();
    // getters for dynamic tabs
    //TQtWidget* GetGroupWidget();
    //TCanvas* GetGroupCanvas();

    void  MakeActions();

public slots:
     void  ProcessMessage();
// void  TurnLive(bool on=true);
     void SaveCB();
     void SaveAsCB();
     void PrintCB();
     void PrintAllCB();
     void QuitCB();
     void AboutCB();
     void ToolBarCB();
     void ChangeToRun();
     void ChangeToLive();

     void tabChanged(QWidget* w=0);
     void SetWindowName(const char* displayText);
     void ShowToolBar(bool show);
     void setEventInfo(int run, int event, int count, int token,  unsigned int triggerBits,  unsigned int detectorBits, unsigned int triggerBitsRun , unsigned int detectorBitsRun );
     void setServerInfo(ServerStatus*);
     void onlPrinter2();
     void OpenReference();
     void updateRequest();
    void CloseWindow(void);
    void DefineLayouts(void);
    void DoLiveButton(void);
    void DoFileButton(void);
    void DoBitsButton(void);
    void DoUpdateButton(void);
    void DoAutoUpdateButton(void);
    void DoPrintButton(void);
    int  GetDebugLevel(void){return mDebugLevel;}
    //    TString GetHistoPSFilePath(void){return mDefaultPSFilePath;}
    // TString GetHistoPSFile(void){return mHistoPSFile;}
    void MakeConnectionFrame(void);
    void MakePrintFrame(void);
    void MakeMenuBar(void);
    void SetDebugLevel(int lDebugLevel){mDebugLevel = lDebugLevel;}
    void SetDefaults(void);

    //    void SetHistoPSFile(TString lHistoPSFile){mHistoPSFile =lHistoPSFile;}
    //    void removeGroupTabs(); // removed recursively all group tabs
    //void addGroupTab(const char*); // adds a new QTabWidget to the dynamic tabs;
    //void addGroup(const char*);    // adds a new QWidget to that last QTabWidget in the dynamic tabs
    void UpdatePlots();

    signals:
      void save();
      void saveAs();
      void live();
      void file();
      void update();
      void update(TCanvas* cc, const char* );
      void update(TCanvas* cc, int tab, int subTab);
      void autoUpdate();
      void print();
      void printAll(const char*);
      void tab(int);
      void subTab(int);
      void canvas(TCanvas*);
      //   void updatePlots();
      void openReference();
};
//************************************************************************




#endif


