#ifndef RC_CONFIG_HH
#define RC_CONFIG_HH

#include <sys/types.h>
#include <rtsSystems.h>
#include <netinet/in.h>

/***********************************************************************************
 * Configuration files for online
 *
 * The configuration files are organized into tiers
 *
 *     TIER 1 - Internal trigger configuration  (named)
 *     DICT   - Dictionary structure for trigger configurations
 *     SETUP  - Sets of parameters that can be choosen as a group
 *     RUN    - Parameters that are set independently each run
 *
 ************************************************************************************/

#include <stdio.h>
#include <string.h>
#include <rts.h>

#define CONFIG_VERSION 2004

#ifdef RTS_PROJECT_PP
#define RC_PARAM_FILE_BASE_PATH "/PP/conf/handler/"
#define TRG_BASE_PATH "/PP/conf/handler"
#define DAQ_BASE_PATH "/PP/conf/handler"
#else
#define RC_PARAM_FILE_BASE_PATH "/RTS/conf/handler/"
#define TRG_BASE_PATH "/home/startrg/trg/handler/conf"
#define DAQ_BASE_PATH "/RTS/conf/handler"
#endif


#define TRG_CONFIG_FILE_NAME ""
#define DAQ_CONFIG_FILE_NAME ""
#define SC_CONFIG_FILE_NAME ""
#define L3_CONFIG_FILE_NAME ""

#define MAX_REGISTERS 1500
#define CFG_MAX_LABELS 200
#define MAX_TRG_DICT_ENTRIES 1500

#ifdef RTS_PROJECT_PP
#define MAX_ID 32
#else
#define MAX_ID 32
#endif
#define MAX_SUB 4
#define MAX_INST 100
#define MAX_TCD 20

#ifdef RTS_PROJECT_PP 
#define MAX_NODES 20
#else
#define MAX_NODES 200
#endif

#define MAX_THREADS 400
#define MAX_STR_LEN 40

#define TRIGGERS_MAX 32          // Number of Triggers configurable
#define CONDPERTRG_MAX 4         // Conditions per Trigger 
#define PWDEF_BYTES TRIGGERS_MAX / 8
#define TWDEF_BYTES TRIGGERS_MAX / 8   // Assume at most 4 detector groupings

#define MAX_TW (1<<16)    /* in principle max values for pw/tw */
#define MAX_PW (1<<12)

// #define NUM_PHYSICS_BITS 16    // correct values but not used...
// #define NUM_DET_BITS 12

#define RTS_STAT_TOKENS_DONE 8
#define RTS_STAT_FORCE 9
#define RTS_STAT_REQUEST 10

#define RTS_STARTRUN_FLAG    1
#define RTS_STOPRUN_FLAG     2
#define RTS_SENDCONFIG_FLAG  4
#define RTS_STOPRUN_SECOND_FLAG 8

typedef unsigned int UINT32;
typedef unsigned short UINT16;
typedef unsigned char UINT8;

//
// What systems are in the run
//
struct TASK
{
  u_short node;
  u_char task;
  u_char inrun;                // Is the system in the run?
  u_char stoprun;              // Is the system allowed to stop the run?
  u_char start_order;          // start order
  u_char typically_inrun;
  u_char res2;
};

struct SUBSYS_TASKS
{
  TASK nodes[MAX_NODES];
};

struct GLB_SETUP 
{
  char DICT_name[MAX_STR_LEN];
  //char GLB_SETUP_name[MAX_STR_LEN];

  char TRG_SETUP_name[MAX_STR_LEN];
  char TRG_RUN_name[MAX_STR_LEN];
  char TCD_SETUP_name[MAX_STR_LEN];
  char DAQ_SETUP_name[MAX_STR_LEN];
  char DAQ_RUN_name[MAX_STR_LEN];
};

// Automatically filled in by GUI...
struct GLB_RUN
{
  char GLB_SETUP_name[MAX_STR_LEN];
  int run_number;
  float bField;                
  float base_trg_rate;
  int destination;
};

struct TRG_DICT_ENTRY
{
  UINT32 object;
  UINT32 index;
  UINT32 reg;
  char label[MAX_STR_LEN];
};

struct TRG_DICT
{
  //char name[MAX_STR_LEN];
  TRG_DICT_ENTRY entry[MAX_TRG_DICT_ENTRIES];
};

struct TcdSetup
{
  UINT32 tcdId;
  UINT32 millisec;
};

/* struct Expansion */
/* { */
/*   UINT32 x[1000]; */
/* }; */

#define L1_DSM_OBJECT    1
#define BC1_DSM_OBJECT   2
#define MIX_DSM_OBJECT   3
#define CTB_DSM_OBJECT   4
#define BCW_DSM_OBJECT   5
#define BCE_DSM_OBJECT   6
#define EEC_DSM_OBJECT   7
#define BBC_DSM_OBJECT   8
#define FPE_DSM_OBJECT   9
#define FMS_DSM_OBJECT  10
#define QT1_FMS_OBJECT 11
#define QT2_FMS_OBJECT 12
#define QT3_FMS_OBJECT 13
#define QT4_FMS_OBJECT 14
#define TRG_OBJECT 29
#define DAQ_OBJECT 30           // Reserved for DAQ use..
#define PHYSICS_BIT_OBJECT 32
#define DETECTOR_BIT_OBJECT 33
#define TCD_OBJECT 34
#define L1_OBJECT 35
#define L2_OBJECT 36
#define L3_OBJECT 37            // Reserved for L3 use...

#define BIT_OBJECT 100          // Reserved for TCU bit labels

struct RegValue
{
  UINT32 object;            // zero is invalid
  UINT32 index;
  UINT32 reg;
  UINT32 value;
};

struct TrgLabels
{
  char label[MAX_STR_LEN];
  int object;
  int value;
  int dfault;
  char comment[MAX_STR_LEN];
};

struct LxAlgorithm
{
  UINT32 id;                // zero is invalid
  int    userInt[5];
  float  userFloat[5];
  int    specialProcessing;
};

struct TwCondition
{
  UINT32 Pw;           // set to zero for invalid record
  UINT32 detectorLiveOnBits;
  UINT32 detectorLiveOffBits;
};

struct PwCondition
{
  UINT32 used;       // set to zero for invalid record  
  UINT32 onbits;
  UINT32 offbits;
};

struct AwCondition
{
  UINT32 TW;
  UINT32 PS;
  UINT32 AW;
  UINT32 pre;
  UINT32 post;
};

struct TriggerData
{
  int offlineBit;

  int desiredTrgCmd;             // these are subject to a later consistency check.
  int desiredDaqCmd;
  int desiredPre;            
  int desiredPost;

  float expected_L0_fraction; // fraction of ZDC's
  float desired_L0_rate;      
  float expected_L1_fraction;
  float desired_L1_rate;
  float expected_L2_fraction;
  float desired_L2_rate;
  float expected_L3_fraction;
  float desired_L3_rate;

  int res;          // Suggestion for the PW... not guarenteed to be followed.
  int dataStream;             // 0 is default physics stream
  int tokenZero;
  int calibration;       // never copy to other streams...
};

// User interface to the triggers
struct Trigger
{
  int used;

  char name[MAX_STR_LEN];
  
  TriggerData userdata;
  PwCondition L0conditions[CONDPERTRG_MAX];

  int detectorLiveOnBits;    // required to be alive
  int detectorLiveOffBits;   // required to be off
  int detectorRequest;       // these fire!
  
  LxAlgorithm l1;
  LxAlgorithm l2;
  LxAlgorithm l3;

  TcdSetup tcd;
};

struct EvpGroup
{
  char name[MAX_STR_LEN];    // name & id from /RTS/conf/handler/evpGroup.txt
  int id;                    // 
  int definition;    // bitmask of triggers...
  float rate;
};

struct TRG_SETUP 
{
  //char name[MAX_STR_LEN];                               // Name of this setup file

  char TIER1_name[MAX_STR_LEN];                         // Tier 1 filename
  
  Trigger triggers[TRIGGERS_MAX];
  
  PwCondition contaminationDef;        //-------------------------------------

  char dataStreamNames[16][MAX_STR_LEN];

  TrgLabels labels[CFG_MAX_LABELS];
  RegValue registers[MAX_REGISTERS];

  //  L3_SETUP l3_setup;
  EvpGroup evpGroup[TRIGGERS_MAX];
};

struct TRG_RUN 
{
  // L1CTL_User_Params
  int configOpt;         // Force configuration depth
    
  int res1;          // Where log messages are written
  int res2;          // Severity of saved log messages

  int useFastDMA;          // Where log messages are written
  int res4;          // Severity of saved log messages
    
  int res5;         // Where log messages are written
  int res6;         // Severity of saved log messages

  int res7;        // Where log messages are written
  int res8;        // Severity of saved log messages

  int res9;          // Where log messages are written
  int res10;          // Severity of saved log messages

  int res11;
  int res12;

  int res13;
  int res14;

  int scaler_log_level;  

  int res15;    // Max token value allowed in system
  int nToken;    // Min token value allowed in system
  int tokenBitsOn;          // Skip every n tokens
  int tokenBitsOff;
  int useSTPnetwork;      // Number of tokens to time 
  int res17;        // turns of/on timing n tokens
  
  int clockSource;       // Clock source
  int res18;     // number of clock tick errors before setting flag
  int stopRunOnError;    // set to stop run/restart clock on error
    
  int l2DataWrite;       // L2 data writing switch
  int dataWriteTimer;    // L2 clock ticks between data writing
  int everyNEvents;      // how many events to skip when writing
  int nAbort;            // Frequency of test aborts

  int res19;        // New 2003
  int res20;
  int EvpPolicy;
 
};

struct TCD_ENTRY
{
  int phase;
  int gg_width;
  int daq_busy;
  int delay4;
  int delay8;
  int res1;
  int res2;
  int res3;
  int res4;
};

struct TCD_SETUP
{
  int tcd_log_level;
  int res1;
  int res2;
  int res3;
  int res4;
  int res5;
  TCD_ENTRY tcd[MAX_TCD];
};

//
// DAQ configuration
//

struct DAQ_DET_SETUP
{
  int ped_mode;
  int gain_mode;
  int analysis;
  int default_format;

  u_char asic_seq_hi;
  u_char asic_seq_lo;
  u_char asic_thr_hi;
  u_char asic_thr_lo;
  int time_bin_lo;
  int time_bin_hi;
  int clust_charge_lo;

  int raw_write;   // write raw data? (n means write every n)
  int cl_write;    // write clusters  (n means write every n)
  int cl_done;     // do cluster finding
};

struct DAQ_SUBDET
{
  u_int rb_mask;
};

struct DAQ_DET
{
  int log_level;
  int use_l2;
  int use_sl3;

  int res1;
  int res2;
  int res3;
  int res4;
  int res5;
  int res6;
  int res7;
  int res8;
  int res9;
  int res10;

  DAQ_SUBDET sub[24];
};

struct DAQ_SETUP
{
  //char DAQ_SETUP_name[MAX_STR_LEN];
  int run_type;

  DAQ_DET_SETUP detectors[MAX_ID];
};

struct DAQ_DAQ
{
  int force_reset;
  int evp_suppression;
  int event_suppression;
  int evb_log_level;
  int myri_log_level;
  int bb_log_level;
  int gb_log_level;
  int evp_log_level;
  int mon_log_level;
  int write_pedestals;     // write pedestal events to evb?
  int res1;           // 0 followSetup, 1 all events, 2 1hz, 3 none
  int res2;           // emulate TPX?
  int res3;
  int res4;
  int res5;
};

struct DAQ_RUN
{ 
  DAQ_DAQ        daq;
  DAQ_DET        detectors[MAX_ID];
};


/************************************************************************************
 * STAR_CFG fully defines the STAR run
 ************************************************************************************/

#ifndef RTS_PROJECT_PP

struct STAR_CFG
{
  SUBSYS_TASKS subsys_tasks;  // HANDLER --> GUI at gui start
  GLB_RUN   glb_run;          // HANDLER --> GUI at gui start
  TRG_DICT  dict;             // GUI loads at editor panel load

  GLB_SETUP glb_setup;        // Modified the order so that referenced 

  TRG_SETUP trg_setup;
  TRG_RUN   trg_run;

  TCD_SETUP tcd_setup;

  DAQ_SETUP daq_setup;
  DAQ_RUN   daq_run;
  
  //L3_RUN    l3_run;
  
  //Expansion expansion;
};

#endif // !PP

#ifdef RTS_PROJECT_PP
struct PP_SETUP {
  //char name[MAX_STR_LEN];
  char daq_setup[MAX_STR_LEN];
  char filename[MAX_STR_LEN];
  u_int enable_mask;
  u_int resync;
  u_int p3;
  u_int p4;
  u_int p5;
  u_int p6;
};

struct PP_RUN {
  char SETUP_name[MAX_STR_LEN];
  int run_number;
  u_int p1;    // skip n events
  u_int p2;
  u_int p3;
  u_int p4;
  u_int p5;
  u_int p6;
};

struct PP_CFG
{
  SUBSYS_TASKS subsys_tasks;

  PP_SETUP setup;  
  PP_RUN   run;
  DAQ_SETUP daq_setup;
  DAQ_RUN   daq_run;
};

typedef PP_CFG STAR_CFG;
#endif

typedef STAR_CFG DAQ_CFG;
typedef STAR_CFG TRG_CFG;
typedef STAR_CFG SC_CFG;
typedef STAR_CFG L3_CFG;

typedef STAR_CFG RC_Config;

// SWAPS...
#ifdef RTS_LITTLE_ENDIAN

void swapTASK(TASK *);
void swapSUBSYS_TASKS(SUBSYS_TASKS *);
void swapGLB_SETUP(GLB_SETUP *);
void swapGLB_RUN(GLB_RUN *);
void swapTRG_DICT_ENTRY(TRG_DICT_ENTRY *);
void swapTRG_DICT(TRG_DICT *);
void swapTcdSetup(TcdSetup *);
void swapRegValue(RegValue *);
void swapTrgLabels(TrgLabels *);
void swapLxAlgorithm(LxAlgorithm *);
void swapTwCondition(TwCondition *);
void swapPwCondition(PwCondition *);
void swapAwCondition(AwCondition *);
void swapTriggerData(TriggerData *);
void swapTrigger(Trigger *);
void swapTRG_SETUP(TRG_SETUP *);
void swapTRG_RUN(TRG_RUN *);
void swapTCD_SETUP(TCD_SETUP *);
void swapTCD_ENTRY(TCD_ENTRY *);
void swapDAQ_DET_SETUP(DAQ_DET_SETUP *);
void swapDAQ_DET(DAQ_DET *);
void swapDAQ_SETUP(DAQ_SETUP *);
void swapDAQ_DAQ(DAQ_DAQ *);
void swapDAQ_RUN(DAQ_RUN *);
void swapSTAR_CFG(STAR_CFG *);

#ifdef RTS_PROJECT_PP
void swapPP_SETUP(PP_SETUP *);
void swapPP_RUN(PP_RUN *);
void swapPP_CFG(PP_CFG *);
#endif

#else
#define swapTASK(yada)
#define swapSUBSYS_TASKS(yada)
#define swapGLB_SETUP(yada)
#define swapGLB_RUN(yada)
#define swapTRG_DICT_ENTRY(yada)
#define swapTRG_DICT(yada)
#define swapTcdSetup(yada)
#define swapRegValue(yada)
#define swapTrgLabels(yada)
#define swapL1Algorithm(yada)
#define swapLxAlgorithm(yada)
#define swapTwCondition(yada)
#define swapPwCondition(yada)
#define swapAwCondition(yada)
#define swapTriggerData(yada)
#define swapTrigger(yada)
#define swapTRG_SETUP(yada)
#define swapTRG_RUN(yada)
#define swapTCD_ENTRY(yada)
#define swapTCD_SETUP(yada)
#define swapDAQ_DET_SETUP(yada)
#define swapDAQ_DET(yada)
#define swapDAQ_SETUP(yada)
#define swapDAQ_DAQ(yada)
#define swapDAQ_RUN(yada)
#define swapSTAR_CFG(yada)
#define swapPP_SETUP(yada)
#define swapPP_RUN(yada)
#define swapPP_CFG(yada)
#endif


// Configuration file functions...The functions get properly swapped
// versions of the configuration file.
//
// The file is stored big endian, even (stupidly?) on little endian 
// machines...
//

// fills filename
struct ic_msg;

char *cmd2name(int cmd);
char *getConfigBase(char *filename, int port, int trg);
char *getConfigBase(char *filename, ic_msg *m);

// fills cfg, returns sizeof(cfg)
int getConfigFile(STAR_CFG *cfg, ic_msg *m);
int getConfigFile(STAR_CFG *cfg, char *filename);

// puts cfg into filename, returns sizeof(cfg)
int putConfigFile(STAR_CFG *cfg, char *filename);

///////////////////////////////////////////////////////////////
// Trigger Definition stuff
///////////////////////////////////////////////////////////////


struct TrgCfg;
struct TrgPS;

void writeRCCNF(char *fn, STAR_CFG *cfg);   // write rc def file...

bool node_inrun(int node, STAR_CFG *cfg);
bool system_inrun(int sys, STAR_CFG *cfg);
void maskDetectorsInRun(STAR_CFG *cfg);

// cfg is a pointer to an existing TrgCfg structure
// the function allocates memory for its internal structures
// it assumes that these structures are free in this call
//
int cfgGetPWForInput(TrgCfg *cfg, STAR_CFG *rccfg, UINT32 input);
int cfgGetTWForInput(TrgCfg *cfg, STAR_CFG *rccfg, UINT32 pw, UINT32 detmask);

bool cfgBuildTrgTables(TrgCfg *cfg, RC_Config *rccfg);
bool cfgBuildPS(TrgPS *ps, RC_Config *rccfg);

// Frees the internal memory in the cfg structure
void cfgFreeTrgTables(TrgCfg *cfg);

// How much memory is allocated?
int getTrgTableSizeAlloc(TrgCfg *cfg);
int getTrgTableSizeReal(TrgCfg *cfg);

// Figure out whether a given pw contributes to a given trigger id
bool cfgPwSatisfiesDaqTrgId(TrgCfg *cfg, int id, int pw);

// Does a given pw satisfy the lastDSM input
bool cfgPwSatisfiesInput(TrgCfg *cfg, UINT16 pw, UINT16 in);

// Does a given tw satisfy the twlut input
bool cfgTwSatisfiesInput(TrgCfg *cfg, RC_Config *rccfg, UINT16 tw, UINT16 pw, UINT16 detmask);

///////////////////////////////////////////////////
// Used to build trigger configuration tables;
///////////////////////////////////////////////////

struct _tmask {
  unsigned char mask[TRIGGERS_MAX/8];
};

struct _tdef {
  int n;
  int nalloc;
  _tmask *defs;
};

struct _tuint16a {
  int n;
  int nalloc;
  UINT16 *v;
};

struct _tfloata {
  int n;
  int nalloc;
  float *v;
};

struct _tawca {
  int n;
  int nalloc;
  AwCondition *awc;
};

struct _trginfo {
  char name[MAX_STR_LEN];
  int used;

  UINT32 eff_l0ps;   // effective l0 ps
  float eff_ps;     // effective total ps

  float l1ps;
  float l2ps;
  float l3ps;

  _tuint16a pws;
  _tuint16a tws;
  _tfloata rs;
};


struct TrgCfg {
  int ndetbits;        // 6?
  int npwbits;         // 10?
  int nphysbits;
  
  _tdef pwdef;
  _tdef twdef;
  _trginfo info[TRIGGERS_MAX];
  _tawca awc;
};

struct TrgPSEntry {
  int l0ps;
  float l1ps;
  float l2ps;
  float l3ps;
};

struct TrgPS {
  TrgPSEntry ps[TRIGGERS_MAX];
};

struct EthServer
{
  UINT16 node;
  UINT16 task;
  UINT32 ip;
  UINT32 port;
};

// Reads from all.conf
// returns -1 if no server, 0 if server exists.
int getEthServer(int node, int task, EthServer *eth);   
char *ReadAllDotConf(int node, int task, char *param, char *result=NULL, char *paramfilename="/RTS/conf/handler/all.conf");

///////////////////////////////////////////////////

//
//  token based EVB determination for LXSB & GB
//

class EvbChooser
{
 public:
  EvbChooser() {};
  void configure(STAR_CFG *cfg, int legacy);

  EthServer *choose(int token); // returns ptr (NULL on error)
  int chooseIdx(int token);     // returns idx (-1 on error)
  
  EthServer servers[50];        // linearized servers
  int nservers;

 private:
  int evbNodes[10][5];          // idx to server by evb/server
  int nevbserv[10];             // nservers by evb
  int nevbs;
};

#endif





