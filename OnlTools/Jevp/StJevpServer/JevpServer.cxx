#include <ctype.h>
#include <TROOT.h>
#include <TServerSocket.h>
#include <TSocket.h>
#include <TMessage.h>
#include <TMonitor.h>
#include <TClass.h>
#include <TCanvas.h>
#include <PDFUtil/PdfIndex.hh>
#include <dirent.h>
#include <TFile.h>
#include <TLine.h>
#include <TStyle.h>
#include <TFrame.h>
#include <TText.h>
#include <TSystem.h>
#include <signal.h>

#include "EvpConstants.h"
#include "JevpServer.h"
#include <StRoot/RTS/include/rtsLog.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include "Jevp/StJevpPlot/JLatex.h"
#include "JTMonitor.h"

#include "Jevp/StJevpBuilders/baseBuilder.h"
#include "Jevp/StJevpBuilders/bbcBuilder.h"
#include "Jevp/StJevpBuilders/daqBuilder.h"
#include "Jevp/StJevpBuilders/eemcBuilder.h"
#include "Jevp/StJevpBuilders/bemcBuilder.h"
#include "Jevp/StJevpBuilders/fpdBuilder.h"
#include "Jevp/StJevpBuilders/hltBuilder.h"
#include "Jevp/StJevpBuilders/l3Builder.h"
#include "Jevp/StJevpBuilders/tofBuilder.h"
#include "Jevp/StJevpBuilders/tpxBuilder.h"
#include "Jevp/StJevpBuilders/trgBuilder.h"
#include "Jevp/StJevpBuilders/upcBuilder.h"

static int line_number=0;
#define CP line_number=__LINE__

static void sigHandler(int arg, siginfo_t *sig, void *v)
{
  static char str[255];
  
  if(arg == SIGCHLD) {
    int status;
    waitpid(-1, &status, WNOHANG);
    LOG(DBG, "Got signal SIGCHLD (reading pdf?) ");
    return;
  }

  sprintf(str,"Signal %d: shutting down! (line=%d)", arg, line_number);
  LOG(ERR, "%s", str);

  exit(-1);
}

static void catchSignals(void)
{
  int i ;
  struct sigaction act ;
  
  LOG(DBG, "catching signals");

  // hook signals to my default...
  act.sa_sigaction = sigHandler ;
  act.sa_flags = SA_SIGINFO ;
  
  for(i=0;i<37;i++) {	// hook'em all!
    sigaction(i,&act,NULL) ;
  }
  
  return;
}

void _JevpServerMain(int argc, char *argv[])
{

  gSystem->ResetSignal(kSigChild);
  gSystem->ResetSignal(kSigBus);
  gSystem->ResetSignal(kSigSegmentationViolation);
  gSystem->ResetSignal(kSigIllegalInstruction);
  gSystem->ResetSignal(kSigSystem);
  gSystem->ResetSignal(kSigPipe);
  gSystem->ResetSignal(kSigAlarm);
  gSystem->ResetSignal(kSigUrgent);
  gSystem->ResetSignal(kSigFloatingException);
  gSystem->ResetSignal(kSigWindowChanged);
  

  catchSignals();

  JevpServer::main(argc, argv);
}

void JevpServer::main(int argc, char *argv[])
{
  // gErrorIgnoreLevel = kBreak;   // suppress root messages...
  JevpServer serv;
   
  serv.parseArgs(argc, argv);
  
  rtsLogOutput(serv.log_output);
  rtsLogAddDest(serv.log_dest, serv.log_port);
  rtsLogLevel(serv.log_level);

  LOG("JEFF", "Starting JevpServer: port=%d pid=%d", serv.myport, (int)getpid());

  // Each time we start, archive the existing display file...
  serv.init(serv.myport, argc, argv);

  for(;;) {

    CP;
    int delay = serv.handleEvent();   // get an event and call the builders...

    // delay depends on the event status
    // 0 if successfully read an event
    // longer if no events available...
    CP;
    serv.handleClient(delay);         
  }
}

void JevpServer::parseArgs(int argc, char *argv[])
{
  throttleAlgos = 1;

  log_output = RTS_LOG_STDERR;
  log_dest = (char *)"172.16.0.1";
  log_port = 8404;
  log_level = (char *)WARN;

  for(int i=1;i<argc;i++) {
    if(strcmp(argv[i], "-dd")==0) {
      i++;
      displays_fn = argv[i];
    }
    else if (strcmp(argv[i], "-basedir") == 0) {
      i++;
      basedir = argv[i];
    }
    else if (strcmp(argv[i], "-nothrottle") == 0) {
      throttleAlgos = 0;
    }
    else if (strcmp(argv[i], "-nodb")==0) {
      nodb = 1;
    }
    else if (strcmp(argv[i], "-db") == 0) {
      nodb = 0;
    }
    else if (strcmp(argv[i], "-port")==0) {
      i++;
      myport = atoi(argv[i]);
    }
    else if (strcmp(argv[i], "-file")==0) {
      i++;
      daqfilename = argv[i];
    }
    else if (strcmp(argv[i], "-die")==0) {
      die = 1;
    }
    else if (strcmp(argv[i], "-production") == 0) {
      nodb = 0;
      myport = JEVP_PORT;
    }
    else if (strcmp(argv[i], "-test")==0) {
      nodb = 1;
      basedir = "/RTScache/conf/jevp_test";
      pdfdir = "/a/jevp_test/pdf";
      refplotdir = "/a/jevp_test/refplots";
      myport = JEVP_PORT + 10;
    }
    else if (strcmp(argv[i], "-diska")==0) {   // used only to pass to builders on launch...
      i++;
      diska = argv[i];
    }
    else {
      printf("\n\nUsage for %s:  (bad arg %s)\n",argv[0],argv[i]);
      printf("\t[-dd filename]       for each display definition:\n");
      printf("\t[-basedir basedir]   config file directory\n");
      printf("\t[-nodb]\n");
      printf("\t[-db]    not usually needed, but db usually disabled in reanalysis\n");
      printf("\t[-port] port]\n");
      printf("\t[-die]    (exit after end of run..)\n");
      printf("\t[-file daqfilename]\n");
      printf("\t[-test]   (set port to %d)\n",myport+10);
      printf("\t[-production]\n");
      printf("\t[-diska [/net/a]]  (used to pass to builders on launch)\n");
      printf("\t[-nothrottle]\n");
      printf("\n\n");
      printf("Defaults:  \n");
      printf("\tbasedir      = '/RTScache/conf'\n");
      printf("\tdisplay file = 'HistoDefs.txt'\n");
      printf("\tport         = %d\n", JEVP_PORT);
      printf("\tuse database!\n");
      printf("\n\n");
      exit(0);
    }
  }


  if(!displays_fn) {
    displays_fn = (char *)"HistoDefs.txt";
  }    
}

int JevpServer::updateDisplayDefs()
{
  char tmp[100];
  sprintf(tmp, "%s/%s", basedir, displays_fn);
  if(displays) delete displays;
  displays = new DisplayFile();
  displays->Read(tmp);

  displays->dump();
  return 0;
}


int JevpServer::init(int port, int argc, char *argv[]) {

  ssocket = new TServerSocket(port,kTRUE,100);
  mon = new JTMonitor();
  mon->Add(ssocket);

  updateDisplayDefs();

  // Create daq reader...
  LOG(DBG, "Reader filename is %s",daqfilename ? daqfilename : "none");
  rdr = new daqReader(daqfilename);
  if(diska) rdr->setEvpDisk(diska);

  // daqreader resets it?
  rtsLogOutput(log_output);
  rtsLogAddDest(log_dest, log_port);
  rtsLogLevel(log_level);


  // Create builders...
  builders.Add(new baseBuilder);
  builders.Add(new bbcBuilder());
  builders.Add(new daqBuilder());
  builders.Add(new bemcBuilder());
  builders.Add(new eemcBuilder());
  builders.Add(new fpdBuilder());
  builders.Add(new hltBuilder());
  builders.Add(new l3Builder());
  builders.Add(new tofBuilder());
  builders.Add(new tpxBuilder());
  builders.Add(new trgBuilder());
  builders.Add(new upcBuilder());
  

  TListIter next(&builders);
  JevpPlotSet *curr;
  while((curr = (baseBuilder *)next())) {
    curr->_initialize(argc, argv);
  }
  CP;

  return 0;
}  


// returns delay in milliseconds
int JevpServer::handleEvent()
{


  JevpPlotSet *curr;
  TListIter next(&builders);

  CP;

  // Get an event...
  char *ret = rdr->get(0, EVP_TYPE_ANY);
  if(ret == NULL) {
    switch(rdr->status) {
    case EVP_STAT_OK:
      LOG(DBG, "EVP reader burped a bit...");
      CP;
      return 100;
      
    case EVP_STAT_EOR:
      LOG(DBG, "End of the run!");
      CP;
      if(runStatus.running()) {
	performStopRun();
      }
      CP;
      return 1000;

    case EVP_STAT_EVT:
      LOG(ERR, "Problem reading event... skipping");
      return 1000;

    case EVP_STAT_CRIT:
      LOG(CRIT, "Criticle problem reading event... exiting");
      exit(0);

    default:
      LOG(ERR, "Not ok, eor,evt or crit???");
      return 1000;      
    }
  }

  CP;
  if(rdr->status) {
    LOG(ERR, "Bad status on read?  rdr->status=%d",rdr->status);
    return 1000;
  }
  
  if(rdr->run != (unsigned int)runStatus.run) {
    CP;
    LOG("JEFF", "Starting new run #%d  (%d)",rdr->run, runStatus.run);
    performStartRun();
    eventsThisRun = 0;
  }

  eventsThisRun++;
  
  // Now we have an event!
  //
  // fill histograms!
  CP;
  while((curr = (JevpPlotSet *)next())) {
   
    double throttle_time = .025;

    if(throttleAlgos) {
      if((curr->processingTime / (double)eventsThisRun) > throttle_time) {
	LOG("JEFF", "Skipping builder for event %d: %s due to %d ms/event throttle (%lf secs/event : %d of %d so far)",
	    rdr->seq, curr->getPlotSetName(), (int)(throttle_time * 1000), curr->getAverageProcessingTime(), curr->numberOfEventsRun, eventsThisRun);
	
	continue;
      }
    }
    
    CP;
    LOG(DBG, "Sending event #%d to builder: %s  (avg processing time=%lf secs/evt)",rdr->seq, curr->getPlotSetName(), curr->getAverageProcessingTime());

    curr->_event(rdr);

    CP;
  }
  
  CP;
  return 0;
}

void JevpServer::handleClient(int delay) {
  TMessage *mess;
  TSocket *s;

  CP;

  // printf("Got a message\n");
  //  LOG("JEFF", "calling sleep");
  //   sleep(1);
  LOG(DBG, "calling select");

  s = mon->Select(delay);
  LOG(DBG, "back from select");
  CP;
  if((long)s <= 0) {
    if(delay > 0) {
      LOG(DBG, "Got a timeout or an error: %d (delay was %d)",s,delay);
    }
    CP;
    return;
  }

  CP;
  LOG(NOTE, "Got a message:  s=%d",s);

  if(s == ssocket) {
    CP;
    TSocket *nsock = ssocket->Accept();
    TInetAddress adr = nsock->GetInetAddress();
    mon->Add(nsock);
  }
  else {
    CP;
    // read...
  
    int ret = s->Recv(mess);

    CP;
    if(ret == 0) {    // Got a disconnection...
      CP;
      LOG(DBG, "Disconnecting a client!");

      mon->Remove(s);
      delete s;
      delete mess;
      CP;
      return;
    }
    
    CP;

    // Handle control messages...
    if(strcmp(mess->GetClass()->GetName(),"EvpMessage")==0) {
      CP;

      EvpMessage *msg = (EvpMessage *)mess->ReadObject(mess->GetClass());
	
      handleEvpMessage(s, msg);
	
      delete msg;
    }
    else if (strcmp(mess->GetClass()->GetName(), "JevpPlot")==0) {
      CP;
      JevpPlot *plot = (JevpPlot *)mess->ReadObject(mess->GetClass());
      
      if(plot->refid != 0) {
	saveReferencePlot(plot);
      }
      else {
	LOG(ERR, "Got a JevpPlot from client, but doesn't seem to be a reference plot...");
      }
      
      delete plot;
    }
    else {
      CP;
      LOG(ERR, "Got invalid message type: %s\n",mess->GetClass()->GetName());
    }

    CP;
    delete(mess);
    CP;
  }    
  CP;
}

void JevpServer::handleEvpMessage(TSocket *s, EvpMessage *msg)
{
  CP;
  if(strcmp(msg->cmd, "dump") == 0) {
    CP;
    dump();
  }
  else if(strcmp(msg->cmd, "display_desc") == 0) {  // Display Descriptor

    LOG(DBG, "Got request for display %s", msg->args);
    int ret = displays->setDisplay(msg->args);
    LOG(DBG, "setdisplay returend %d", ret);

    EvpMessage m;
    m.setSource("serv");
    m.setCmd("xml");
    if(!displays) {
      LOG("JEFF", "No displays available\n");
      return;
    }
    
    m.setArgs(displays->textBuff);
    CP;

    TMessage mess(kMESS_OBJECT);
    mess.WriteObject(&m);
    s->Send(mess);
  }
  else if(strcmp(msg->cmd, "GetStatus") == 0) {
    TMessage mess(kMESS_OBJECT);
    mess.WriteObject(&runStatus);
    s->Send(mess);
  }
  else if(strcmp(msg->cmd, "ping") == 0) {    
    EvpMessage m;
    m.setSource((char *)"serv");
    m.setCmd((char *)"ping");
    TMessage mess(kMESS_OBJECT);
    mess.WriteObject(&m);
    CP;
    s->Send(mess);
  }
  else if(strcmp(msg->cmd, "print") == 0) {
    char printer[100];
    //int tab;
    //int display;

    //sscanf(msg->args, "%s %d %d", printer, &display, &tab);
    //LOG("JEFF", "Request to printing tab %d to printer %s", tab, printer);

    
    writePdf((char *)"/tmp/jevp.pdf", 1);

    gSystem->Exec("/usr/bin/convert /tmp/jevp.pdf /tmp/jevp.ps");
    
  }
  else if(strcmp(msg->cmd, "getplot") == 0) {
    CP;
    handleGetPlot(s,msg->args);
  }
  else if(strcmp(msg->cmd, "swaprefs") == 0) {
    CP;
    handleSwapRefs(msg->args);
  }
  else if(strcmp(msg->cmd, "deleteplot") == 0) {
    CP;
    char str[256];
    int idx;
    sscanf(msg->args, "%s %d", str, &idx);
    deleteReferencePlot(str,idx);
  }
  else if(strcmp(msg->cmd, "addServerTag") == 0) {
    CP;
    //LOG("JEFF", "Adding serverTags: %s", msg->args);
    addServerTags(msg->args);
    CP;
  }
  else if(strcmp(msg->cmd, "getServerTags") == 0) {
    CP;
    EvpMessage m;
    m.setSource((char *)"serv");
    m.setCmd((char *)"getServerTags");
    if(serverTags) {
      m.setArgs(serverTags);
    }
    else {
      m.setArgs("");
    }

    TMessage mess(kMESS_OBJECT);
    mess.WriteObject(&m);
    s->Send(mess);
  }
  else if(strcmp(msg->cmd, "monitor") == 0) {
    CP;
    EvpMessage m;
    m.setSource((char *)"serv");
    m.setCmd((char *)"monitor");

    getMonitorString(msg->args, &m);
    
    TMessage mess(kMESS_OBJECT);
    mess.WriteObject(&m);
    CP;
    s->Send(mess);
  }
  else {
    CP;
    LOG(WARN,"Unknown command: %s\n",msg->cmd);
  }
  CP;
}

void JevpServer::performStartRun()
{
  runStatus.run = rdr->run;
  eventsThisRun = 0;

  LOG("JEFF", "Start run #%d",runStatus.run);
  clearForNewRun();

  runStatus.setStatus("running");
}

void JevpServer::performStopRun()
{
  LOG("JEFF", "Got run stop for run #%d (%d displays to write out)",runStatus.run, displays->nDisplays());


  JevpPlotSet *curr;
  TListIter next(&builders);
  
  while((curr = (JevpPlotSet *)next())) {
    LOG("JEFF", "End of run report for %s: (%lf secs/event : %d of %d analyzed)",
	curr->getPlotSetName(), curr->getAverageProcessingTime(), curr->numberOfEventsRun, eventsThisRun);
    
    continue;
  }

  
  eventsThisRun = 0;


  // Write out the pdfs for all displays...
  for(int i=0;i<displays->nDisplays();i++) {
    LOG("JEFF","Writing pdf for display %d, run %d",i,runStatus.run);
    CP;
    writeRunPdf(i, runStatus.run);
    CP;
  }

  // Update the palletes and write out xml again
  char fn[256];
  sprintf(fn, "%s/%s", basedir, displays_fn);

  LOG("JEFF", "fn=%s",fn);
  CP;

  // Add any new plots to the pallet...

  freePallete();

  next.Reset();
  while((curr = (JevpPlotSet *)next())) {

    LOG(DBG, "Adding plot to pallete: builder=%s",curr->getPlotSetName());

    JevpPlot *currplot;
    TListIter nextplot(&curr->plots);
    while((currplot = (JevpPlot *)nextplot())) { 
      LOG("JEFF", "                    : plot = %s",currplot->GetPlotName());
      addToPallete(currplot);
    }
  }
  CP;

  LOG(DBG, "Writing display file...%s",fn);
  if(displays->Write(fn) < 0) {
    LOG(ERR, "Error writing xml file %s",fn);
  }

  char *args[4];
  args[0] = "OnlTools/Jevp/archiveHistoDefs.pl";
  args[1] = basedir;
  args[2] = displays_fn;
  args[3] = NULL;

  execScript("OnlTools/Jevp/archiveHistoDefs.pl", args);
 
  if(die) {
    LOG("JEFF", "die is set, so now exit");
    exit(0);
  }

  runStatus.setStatus("stopped");
}

void JevpServer::clearForNewRun()
{
  // Delete all from histogram list
  // First free the actual histo, then remove the link...
  LOG("JEFF", "Clear for new run  #%d",runStatus.run);

  TListIter next(&builders);

  JevpPlotSet *curr;
  while((curr = (JevpPlotSet *)next())) {

    LOG(DBG, "Send startrun for: %s", curr->getPlotSetName());
    curr->_startrun(rdr);
  }

  if(serverTags) {
    free(serverTags);
    serverTags = NULL;
  }
}


JevpPlot *JevpServer::getPlot(char *name) {
  JevpPlotSet *curr;
  TListIter next(&builders);
  
  while((curr = (JevpPlotSet *)next())) {

    JevpPlot *currplot;
    TListIter nextplot(&curr->plots);
  
    while((currplot = (JevpPlot *)nextplot())) {

      LOG(DBG, "getPlot():  checking %s vs %s",name,currplot->GetPlotName());

      if(strcmp(currplot->GetPlotName(), name) == 0) {
	return currplot;
      }
    }
  }

  return NULL;
}


void JevpServer::handleGetPlot(TSocket *s, char *argstring) 
{
  JevpPlot *plot=NULL;
  char refidstr[20];
  char runidstr[20];
  char plotname[80];

  LOG(DBG,"argstring is %s\n",argstring);
  if(!getParamFromString(plotname, argstring)) {
    LOG(ERR,"No plot indicated in getplot?\n");
    return;
  }
 
  LOG(DBG,"Plotname is %s\n",plotname);

  if(getParamFromString(refidstr, argstring, (char *)"refid")) {
    char fn[256];
    sprintf(fn, "%s/REF.%s.%d.root", DEFAULT_REF_PLOT_DIR, plotname, atoi(refidstr));
      
    LOG(DBG,"Reading [%s] from file %s\n",plotname, fn);

    TFile *f1 = new TFile(fn);
    if(!f1) {
      LOG(ERR,"Error opening file: %s",fn);
      plot = NULL;
    }
    else {
      //f1->GetObject(plotname, plot);
      // If JevpSummary, build a new one first...
      f1->GetObject("JevpPlot",plot);
      f1->Close();

      if(plot) {
	LOG(DBG,"Got plot.....xxx\n");
	plot->refid = atoi(refidstr);
      }
      else {
	LOG(WARN,"Didn't get plot %s\n",plotname);
      }
    }
  }
  else if (getParamFromString(runidstr, argstring, (char *)"run")) {    
    char fn[256];
    sprintf(fn, "%s/%d.root",EVP_SAVEPLOT_DIR, atoi(runidstr));

    TFile *f1 = new TFile(fn);
    if(!f1) {
      LOG(ERR,"Error opening file: %s",fn);
      plot = NULL;
    } 
    else {
      f1->GetObject(plotname, plot);
      f1->Close();
    }
  }
  else {
    LOG(DBG,"getplot..\n");

    if(strcmp(plotname, "serv_JevpSummary") == 0) {
      plot = getJevpSummaryPlot();
    }
    else {
      plot = getPlot(plotname);
    }
  }

    
  if(!plot) {
    char tmp[100];
    sprintf(tmp, "No plot %s",plotname);
    EvpMessage m;
    m.setSource((char *)"serv");
    m.setCmd((char *)"noplot");
    m.setArgs(tmp);
    TMessage mess(kMESS_OBJECT);
    mess.WriteObject(&m);
    
    int ret = s->Send(mess);
    LOG(DBG, "sent (errmess) %d bytes",ret);
  } else {
    TMessage mess(kMESS_OBJECT);
    mess.WriteObject(plot);
    int ret = s->Send(mess);
    LOG(DBG, "Sent (plot) %d bytes",ret);
  }
}

JevpPlot *JevpServer::getJevpSummaryPlot()
{
  if(jevpSummaryPlot) {
    delete jevpSummaryPlot;
    jevpSummaryPlot = NULL;
  }

  CP;
  jevpSummaryPlot = new JevpPlot();
  jevpSummaryPlot->setParent((char *)"serv");
  TH1I *h = new TH1I("JevpSummary", "JevpSummary", 64,0,63);
  //h->GetXaxis()->SetAxisColor(kWhite);
  h->GetXaxis()->SetTickLength(0);
  h->GetXaxis()->SetLabelColor(kWhite);
  //h->GetYaxis()->SetAxisColor(kWhite);
  h->GetYaxis()->SetTickLength(0);
  h->GetYaxis()->SetLabelColor(kWhite);
  //h->SetLineColor(kWhite);
  //h->SetAxisColor(kWhite);
  //h->SetLabelColor(kWhite);

  jevpSummaryPlot->addHisto(h);

  jevpSummaryPlot->setOptStat(0);
  jevpSummaryPlot->gridx = 0;
  jevpSummaryPlot->gridy = 0;
  
  
  CP;
  JLatex *l;
  
  
  int i = 0;
  char tmp[256];

  sprintf(tmp,"Run #%d: (%s for %ld seconds)",runStatus.run, runStatus.status, time(NULL) - runStatus.timeOfLastChange);
  l = new JLatex(2, liney(i++), tmp);
  i++;
  l->SetTextSize(.05);
  jevpSummaryPlot->addElement(l);

  sprintf(tmp, "Tags:   %s", serverTags);
  l = new JLatex(2, liney(i++), tmp);
  i++;
  l->SetTextSize(.035);
  jevpSummaryPlot->addElement(l);


  // Now show builders...
  TListIter next(&builders);
  BuilderStatus *curr;
  int n=0;
  while((curr = (BuilderStatus *)next())) {
    n++;
    sprintf(tmp, "builder %10s%c: \t(run #%d, status %s, events %d, evttime %ld, contacttime %ld)",
	    curr->name, curr->official ? '*' : '-', curr->run, curr->status, curr->events, time(NULL) - curr->lastEventTime, time(NULL) - curr->lastTransaction);
    l = new JLatex(2, liney(i++), tmp);
    l->SetTextSize(.035);
    jevpSummaryPlot->addElement(l); 
  }
  
  CP;
  if(n == 0) {
    sprintf(tmp,"There are no builders");
    l = new JLatex(2, liney(i++), tmp);
    l->SetTextSize(.035);
    jevpSummaryPlot->addElement(l);
  }
  CP;

  return jevpSummaryPlot;
}

void JevpServer::handleSwapRefs(char *name)
{
  char name1[256];
  char name2[256];
  char tmp[256];
  char base[256];
  int idx1, idx2;
  sscanf(name, "%s %d %d", base, &idx1, &idx2);
  
  LOG(DBG,"Swapping %s (%d <--> %d)\n",base,idx1,idx2);
  sprintf(name1, "%s/REF.%s.%d.root",refplotdir, base, idx1);
  sprintf(name2, "%s/REF.%s.%d.root",refplotdir, base, idx2);

  sprintf(tmp, "%s/REF.%s.root.tmp",refplotdir, base);
  rename(name1, tmp);
  rename(name2, name1);
  rename(tmp, name2);
}

void JevpServer::writeRunPdf(int display, int run)
{
  int ret = displays->setDisplay(display);
  if(ret < 0) {
    LOG(ERR, "Can't set display to %d",display);
    return;
  }
  LOG(DBG, "Set displays to %d",ret);
  
  char filename[256];
  sprintf(filename, "%s/%s_%d.pdf",pdfdir, displays->displayRoot->name, run);
  
  writePdf(filename, 1);

  // Save it in the database...
  if(nodb != 1) {
    LOG(DBG, "Writing PDF file: %s",filename);

    char *args[5];

    args[0] = (char *)"WritePDFToDB";
    char tmp[10];
    sprintf(tmp, "%d", run);
    args[1] = tmp;
    args[2] = filename;
    args[3] = displays->displayRoot->name;
    args[4] = NULL;

    //int ret = char((execScript *)"WritePDFToDB",args);
    int ret = execScript("WritePDFToDB", args);
    LOG("JEFF", "Wrote PDF file: %s (ret=%d)", filename, ret);
  }
}

void JevpServer::writePdf(char *filename, int combo_index)
{
  LOG("JEFF", "Writing pdf: %s index=%d",filename,combo_index);
  DisplayNode *root = displays->getTab(combo_index);

  if(combo_index == 0) {
    LOG("JEFF", "disproot = 0x%x root = 0x%x", displays->displayRoot, root);
    root = displays->displayRoot;
  }


  //   char filename[256];
  //   sprintf(filename, "%s/%s_%d.pdf", pdfdir, displays->displayRoot->name, run);

  PdfIndex index;
  writeNodePdf(root, &index, NULL, filename, 1, 0);
  
  // Now a summary....
  char endfilename[256];
  strcpy(endfilename, filename);
  strcat(endfilename, ")");
  TCanvas summary("c2");
  summary.Print(endfilename, "pdf,Portrait");
  
  // Index the file...
  char indexedfilename[256];
  strcpy(indexedfilename, filename);
  // strcat(indexedfilename, ".idx");
  index.CreateIndexedFile(filename, indexedfilename);
}

int JevpServer::writeNodePdf(DisplayNode *node, PdfIndex *index, index_entry *prevIndexEntry, char *filename, int page, int nosibs)
{
  
  LOG(DBG, "writeNodePdf:  %s page=%d",node->name,page);

  int npages = 0;
  if(node->leaf) {   // We are writing histograms...
    writeHistogramLeavesPdf(node, index, prevIndexEntry, filename, page);
    return 1;
  }
  else {   // We are just writing index entries
    // are we the child?
    index_entry *currIndexEntry;
    if(node->prev == NULL) {
      currIndexEntry = index->add_child(prevIndexEntry, node->name, page, 0);
    }
    else {
      currIndexEntry = index->add_sibling(prevIndexEntry, node->name, page, 0);
    }
    
    if(node->child) {
      npages += writeNodePdf(node->child, index, currIndexEntry, filename, page, 0);
    }
    
    if(node->next && !nosibs) {
      npages += writeNodePdf(node->next, index, currIndexEntry, filename, page + npages, 0);
    }

    return npages;
  }
}    



// If page = 1 prints out start tag --> "filename("
// But assumes a summary follows, so there is no end tag --> "filename)"
//
int JevpServer::writeHistogramLeavesPdf(DisplayNode *node, PdfIndex *index, index_entry *prevIndexEntry, char *filename, int page)
{
  LOG(DBG, "Write histogram leaves: %s",node->name);

  CP;
  if((node->prev != NULL) || (!node->leaf)) {
    LOG(ERR, "Shouldn't happen: prev=0x%x leaf=%d", node->prev, node->leaf);
  }

  CP;
  // create index first
  index_entry *cindex = index->add_child(prevIndexEntry, node->name, page, 0);
  DisplayNode *cnode = node->next;
  while(cnode) {
    cindex = index->add_sibling(cindex, cnode->name, page, 0);
    cnode = cnode->next;
  }
  CP;
  // Now draw histograms...
  gStyle->SetCanvasColor(19);
  TCanvas *c1 = new TCanvas("c1","c1",1000,800);

  char fname[256];
  strcpy(fname, filename);
  if(page == 1) {
    strcat(fname, "(");
  }
  CP;
  int wide = node->getIntParentProperty("wide");
  if(wide < 0) wide = 1;
  int deep = node->getIntParentProperty("deep");
  if(deep < 0) deep = 1;
  int scaley = node->getIntParentProperty("scaley");
  if(scaley <= 0) scaley = 0;
  CP;
  c1->Clear();
  c1->Divide(wide, deep);
  int pad = 1;
  CP;
  if(scaley) {
    double ymax = -999999;
    cnode = node;
    while(cnode) {

      LOG(DBG, "cnode->name = %s", cnode->name);
      JevpPlot *plot = getPlot(cnode->name);
      if(plot) {
	LOG(DBG, "got plot 0x%x",plot);
	double my = plot->getMaxY();
	if(my > ymax) ymax = my;
      }
      cnode = cnode->next;
    }
    CP;
    
    printf("Got scaley...  Setting max value to ymax=%lf\n",ymax*1.1);
    cnode = node;
    while(cnode) {
      JevpPlot *plot = getPlot(cnode->name);
      if(plot) {
	if(plot->logy) {
	  plot->setMaxY(ymax * 2);
	}
	else {
	  plot->setMaxY(ymax * 1.1);
	}
      }
      cnode = cnode->next;
    }
  }
  CP;

  cnode = node;
  while(cnode) {
    c1->cd(pad);
    CP;

    LOG(DBG, "Plotting %s on page %d / pad %d",cnode->name, page, pad);

    JevpPlot *plot = getPlot(cnode->name);
    if(plot) {
      LOG(DBG, "Found plot %s",cnode->name);
      plot->draw();
    }
    else {
      LOG(DBG, "Can't find plot %s",cnode->name);
      DrawCrossOfDeath(cnode->name);
    }

    cnode = cnode->next;
    pad++;
  }
  CP;
  while(pad <= wide*deep) {
    c1->cd(pad);
    TLatex *x = new TLatex(.5,.5," ");
    x->Draw();
    //gPad->Draw();
    // printf("Drawing jeff %d\n",pad);
    pad++;
  }
  
  CP;
  c1->Print(fname, "pdf,Portrait");

  delete c1;
  return 1;
}


int JevpServer::getMaxRef(char *name)
{
  int maxid = 0;
  struct dirent *dirent;

  DIR *dir = opendir(refplotdir);
  if(dir == NULL) {
    LOG(ERR,"Error opening dir (%s)\n", refplotdir);
    return -1;
  }

  char basename[256];
  sprintf(basename, "REF.%s.", name);
  
  while((dirent = readdir(dir)) != NULL) {
    if(memcmp(basename, dirent->d_name, strlen(basename)) == 0) {
      char *tmp = dirent->d_name;
      tmp += strlen(basename);

      int id = atoi(tmp);     
      if(id > maxid) maxid = id;
    }
  }
  
  closedir(dir);
  return maxid;
}

void JevpServer::shiftRefPlotsUp(char *name, int first)
{
  int max = getMaxRef(name);
  for(int i=max;i>=first;i--) {
    char dst[256];
    char src[256];
    sprintf(dst, "%s/REF.%s.%d.root",refplotdir,name,i+1);
    sprintf(src, "%s/REF.%s.%d.root",refplotdir,name,i);   
    LOG(DBG,"Renaming file: %s --> %s\n", src, dst);
    rename(src, dst);
  }
}

void JevpServer::shiftRefPlotsDown(char *name, int first)
{
  int max = getMaxRef(name);
  for(int i=first;i<=max;i++) {
    char dst[256];
    char src[256];
    sprintf(dst, "%s/REF.%s.%d.root",refplotdir,name,i-1);
    sprintf(src, "%s/REF.%s.%d.root",refplotdir,name,i);

    LOG(DBG,"Renaming file: %s --> %s\n", src, dst);
    rename(src, dst);
  }
}

void JevpServer::deleteReferencePlot(char *name, int refid) {
  char filename[256];

  sprintf(filename, "%s/REF.%s.%d.root",refplotdir,name,refid);
  LOG(DBG,"Deleting file: %s\n", filename);
  unlink(filename);
  shiftRefPlotsDown(name,refid+1);
}

void JevpServer::saveReferencePlot(JevpPlot *plot) {

  LOG("JEFF","save refplot");

  char plotname[256];

  if(plot->refid > 0) {
    shiftRefPlotsUp(plot->GetPlotName(), plot->refid);
  }


  LOG("JEFF", "refplot %s %d",plot->GetPlotName(), plot->refid);
  
  
  sprintf(plotname, "%s/REF.%s.%d.root",refplotdir,plot->GetPlotName(), plot->refid);

  LOG("JEFF", "plotname = %s", plotname);

  // Now actually save plot to the file plotname...
  TFile f(plotname, "new");
  plot->Write();
  f.Close();
}


// tags delimeted by "|"
void JevpServer::addServerTags(char *tags)
{
  char *tmp = (char *)malloc(sizeof(tags)+1);
  strcpy(tmp, tags);
  
  if(tmp[0] != '|') {
    LOG(ERR, "Bad tag string: %s",tags);
    free(tmp);
    return;
  }

  char *t = strtok(tmp, "|");
  while(t) {
    addServerTag(t);
    t = strtok(NULL, "|");
  }
  
  free(tmp);
}


int JevpServer::execScript(const char *name, char *args[], int waitforreturn)
{
  CP;
  pid_t pid = fork();

  if(pid == -1) {
    LOG(CRIT, "Error spawning script: %s (%s)",name, strerror(errno),0,0,0);
    return 1;
  }

  if(pid == 0) {
    for(int i=0;;i++) {
      if(args[i] == NULL) break;
      LOG(NOTE, "args[%d] = %s",i,args[i]);
    }
    
    int ret = execvp(name,args);
    if(ret < 0) {
      char buff[100];
      LOG(CRIT, "Error spawning script: %s (%s)  (%s)",name, strerror(errno),getcwd(buff,100));
      return 1;
    }
  }

  CP;
  if(!waitforreturn) return 0;
  CP;

  // Wait for child to return....
  int stat=0;
  do {
    waitpid(pid,&stat,0);
  } while(WIFEXITED(stat) == 0);

  return WEXITSTATUS(stat);
}

DisplayNode *JevpServer::getPalleteNode()
{
  DisplayNode *palleteNode = displays->root->child;
  
  while(palleteNode) {
    if(strcmp(palleteNode->name, "pallete") == 0) {
      return palleteNode;
    }
    palleteNode = palleteNode->next;
  }
  return NULL;
}

void JevpServer::freePallete()
{
  DisplayNode *palleteNode = getPalleteNode();
  if(palleteNode) {
    palleteNode->freeChildren();
  }
}

// This function actually checks if already in pallete
// if not, adds....
void JevpServer::addToPallete(JevpPlot *plot)
{
  char *builder = plot->getParent();
  char *name = plot->GetPlotName();

  DisplayNode *palleteNode = getPalleteNode();

  CP;
  if(!palleteNode) {
    LOG(ERR, "No pallete found!");
    return;
  }
  CP;

  // Look for builder...
  DisplayNode *builderNode = palleteNode->findChild(builder);

  CP;
  if(!builderNode) {
    CP;
    builderNode = new DisplayNode();
    builderNode->setName(builder);
    palleteNode->insertChildAlpha(builderNode);
  }
  CP;
  // Look for plot...

  DisplayNode *plotNode = builderNode->findChild(name);

  CP;
  if(plotNode) {
    CP;
    LOG(ERR, "We already have a pallete entry for %s:%s",builder,name);
  }
  else {
    CP;
    // The plot was not found... insert it
    plotNode = new DisplayNode();
    plotNode->setName(name);
    plotNode->leaf = 1;
    builderNode->insertChildAlpha(plotNode);
  }
  CP;
}


// Parse a string of the form
// "defaultparam param1=x param2=y"
char *JevpServer::getParamFromString(char *dest, char *source, char *param)
{
  char *tmp = dest;
  char *str = source;

  // Find the "param=" and position directly after it...
  if(param != NULL) {
    str = strstr(source, param);
    if(!str) return NULL;

    str += strlen(param);
    if(*str != '=') {
      return NULL;
    }
    str++;
  }
  
  // copy till whitespace or end...
  while((*str != '\0') && !isspace(*str)) {
    *tmp = *str;
    tmp++;
    str++;
  }
  *tmp = '\0';
  return dest;  
}


void JevpServer::DrawCrossOfDeath(char *str)
{
  TLine* a = new TLine(0.,0.,1.,1.);
  TLine* b = new TLine(0.,1.,1.,0.);
  TText* t = new TText(0.5,0.5,str);

//   // This is how we free the memory...
  a->SetBit(kCanDelete);
  b->SetBit(kCanDelete);
  t->SetBit(kCanDelete);
//   screen->addPlot(a);
//   screen->addPlot(b);
//   screen->addPlot(t);

  a->SetLineColor(2);
  b->SetLineColor(2);
  t->SetTextColor(3);
  t->SetTextAlign(22);

  // Already cd()'d to proper pad...
  a->Draw();
  b->Draw();
  t->Draw();

  //delete a;
  //delete b;
  //delete t;
  // gCanvas->Update();
  //cout << __PRETTY_FUNCTION__ << endl;
  return;

}


double JevpServer::liney(double x)
{
  return 1.0 - (x+5.0)/25.0;
}
