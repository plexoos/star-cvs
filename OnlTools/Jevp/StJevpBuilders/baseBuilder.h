#include <stdio.h>
#include <stdlib.h>

#include "Jevp/StJevpPlot/JevpPlotSet.h"
#include "DAQ_READER/daqReader.h"
//#include "RunStatus.h"

#include <TH1I.h>
#include <TH2F.h>

#include <math.h>

// This is the one PlotSet that is guarenteed to always exist
// It's main purpose is to provide the run information 
// To the server...
//
// It has no plots (currently)
//


class baseBuilder : public JevpPlotSet {
public:
  //RunStatus status;
  int run;

  baseBuilder() {
    plotsetname = (char *)"base";
    hello_cmds = (char *)"steal";
  }

  void initialize(int argc, char *argv[]);
  void startrun(daqReader *rdr);
  void stoprun(daqReader *rdr);
  void event(daqReader *rdr);

  static void main(int argc, char *argv[]);

  ClassDef(baseBuilder, 1);
};
