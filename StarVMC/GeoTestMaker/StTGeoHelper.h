// $Id: StTGeoHelper.h,v 1.7 2010/01/27 23:02:57 perev Exp $
//
//
// Class StTGeoHelper
// ------------------
// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __


#ifndef ST_TGEOHELPER_H
#define ST_TGEOHELPER_H
#include <map>
#include <vector>
#include "StEnumerations.h"
#include "TString.h"
#include "TObject.h"
class TObjArray;
class TGeoNavigator;
class TGeoNode;
class TGeoVolume;
class TGeoMedium;
class TGeoMaterial;
class TGeoHMatrix;
class StVoluInfo;
class StTGeoHitShape;
class StTGeoIter;
class StHitPlaneInfo;
class StMultiKeyMap;

// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __
struct InvertLtStr
{
bool operator()(const TString& s1, const TString& s2) const
  {
    int n1 = s1.Length(),n2 = s2.Length();
    if (n1 != n2) return n1<n2;
    const char *c1 = s1.Data(),*c2 = s2.Data();
    for (int j=n1-1; j>=0; --j) {if (c1[j]!=c2[j]) return (c1[j]<c2[j]);}
    return 0;
  }
};

// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __
class StiHit; 
class StHitPlane; 
typedef std::map< unsigned int, StHitPlane*>  	StHitPlaneHardMap;
typedef std::pair<unsigned int, StHitPlane*>    StHitPlaneHardPair;
typedef StHitPlaneHardMap::const_iterator       StHitPlaneHardMapIter;

typedef std::vector<void*>  		    	StVoidArr;


typedef std::map< const TString, StHitPlane*, InvertLtStr>  StHitPlanePathMap;
typedef std::pair<const TString, StHitPlane*>               StHitPlanePathPair;
typedef StHitPlanePathMap::const_iterator                   StHitPlanePathMapIter;
typedef float Mtx33_t[3][3];

// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __
class StVoluInfo : public TObject
{
public:
enum E_VoluInfo {
      kModule      = BIT(15),   // The volume is a module, like TPCE,SVTT
      kMODULE      = BIT(16),   // The volume is a Module with Hits 
      kActive      = BIT(17),  	// The volume is active 
      kHitPlane    = BIT(21)};	// The volume is a Hit plane
public:
enum E_Kind { kVoluInfo=1,kHitPlaneInfo=2};

   	 StVoluInfo(int voluNumber)	{SetUniqueID(voluNumber);}
virtual ~StVoluInfo(){;}
	int IsModule  ()   	const 	{return TestBit(kModule);}
	int IsMODULE() 		const 	{return TestBit(kMODULE);}
	int IsHitPlane() 	const 	{return TestBit(kHitPlane);}
	int IsActive() 	        const 	{return TestBit(kActive);}
       void SetModule  (int s=1)        {SetBit(kModule,s)      ;}
       void SetMODULE  (int s=1)     	{SetBit(kMODULE,s)      ;}
       void SetHitPlane(int s=1)     	{SetBit(kHitPlane,s)    ;}
        int GetNumber() 	const 	{return GetUniqueID()   ;}
const  TGeoVolume* GetVolu() 	const; 
virtual int Kind() 		const	{return kVoluInfo;}
const char *GetName() const;
public:
ClassDef(StVoluInfo,0) //
};



// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __
class StHitPlaneInfo : public StVoluInfo
{
friend class StTGeoHelper;
public:
      StHitPlaneInfo(int volId);	
virtual ~StHitPlaneInfo(){;}
void  operator=(const StVoluInfo& ext){*((StVoluInfo*)this)=ext;}
      int Kind() const		{return kHitPlaneInfo;}
      int Axis() const		{return fAxi;}
const Mtx33_t &GetDir() const 	{return fDir;}
const float   *GetOrg() const	{return fOrg;}
      StHitPlane *MakeHitPlane(const StTGeoIter &it);
      StHitPlane *GetHitPlane (const TString &path) const;
      void SetAxis(int axi)	{fAxi=axi;}
void  Print(const char* opt="") const;
protected:
int    fAxi;
float fOrg[3];
float fDir[3][3];
StHitPlanePathMap fHitPlanePathMap;
ClassDef(StHitPlaneInfo,0) //
};

// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __
class StHitPlane : public TNamed
{
friend class StTGeoHelper;
friend class StHitPlaneInfo;
public:
      StHitPlane(const char *path, int volId);	
virtual ~StHitPlane();
void  Clear(const char *opt="");
void  InitHits();

virtual const Mtx33_t &GetDir(const float *) const {return fDir;}
virtual const float   *GetOrg(const float *) const {return fOrg;}
virtual const float   *GetPnt()        const {return fOrg;}
virtual       void  AddHit(void *hit,const float xyz[3]);
virtual       int   Kind() const {return 0;}
              int   GetNumber()  { return GetUniqueID(); }
int   GetNHits() const;
protected:
float fOrg[3];
float fDir[3][3];
StMultiKeyMap *fHitMap;
ClassDef(StHitPlane,0) //
};
// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __
class StHitTube : public StHitPlane
{
friend class StTGeoHelper;
friend class StHitPlaneInfo;
public:
      StHitTube(const char *path, int volId);	
virtual ~StHitTube(){;}

virtual const Mtx33_t &GetDir(const float *)	const ;
virtual const float   *GetPnt()	const {return fPnt;}
virtual       int      Kind() 	const {return 1;}
protected:
float fPnt[3];
ClassDef(StHitTube,0) //
};
// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __
class StTGeoHelper : public TObject
{
         StTGeoHelper();
virtual ~StTGeoHelper();

public:
       void Init(int mode=0);

       void SetModule (const char *voluName,int akt=1);
       void SetModule (int   moduLevl=2) {fModuLev = moduLevl;}
       void InitInfo();
       void InitModuLev();

       void InitHitShape();

       void SetHitPlane(const char *moduName,const char *voluName,int axis=1);
       void InitHitPlane();
       void InitHits();
       void ClearHits();

static	StTGeoHelper *Instance(); 
static	StTGeoHelper *Inst(){return Instance();}; 

public:
	void  Print(const char *tit=0) const;
        void  ls(const char* opt="Mmps")                const;
	void  SetInfo   (StVoluInfo *ext);
	void  SetActive (const char *voluName,int act=1);
	void  SetActive (StDetectorId did,int act=1);
  StVoluInfo *SetFlag   (const TGeoVolume *volu,StVoluInfo::E_VoluInfo flg,int act=1);
	 int  IsFlag    (const TGeoVolume *volu,StVoluInfo::E_VoluInfo flg) const;
	 int  IsModule  (const TGeoVolume *volu) 	const;
	 int  IsModule  (const TGeoNode   *node) 	const;
	 int  IsMODULE  (const TGeoVolume *volu) 	const;
	 int  IsActive  () 				const;
	 int  IsActive  (StDetectorId did) 		const;
static	 int  IsSensitive(const TGeoVolume *volu);
static	 int  DetId(const char *detName);
static	const char *DetName(int detId);
static	const char *ModName(int detId);



   StVoluInfo *GetInfo  (int idx) 	const;
   StVoluInfo *GetINFO  (int idx);
const   char  *GetPath() const;     
const TGeoVolume *GetModu() const;
const TGeoVolume *GetVolu() const;
StVoidArr     *GetSeedHits()        const {return fSeedHits       ;}
StHitPlaneHardMap *GetPlaneHardMap() const {return fHitPlaneHardMap;}

StHitPlaneInfo* IsHitPlane(const TGeoVolume *volu) const;
StHitPlaneInfo* IsHitPlane(const TGeoNode   *node) const;
StHitPlane   *GetCurrentHitPlane ();

	     int  MayHitPlane     (const TGeoVolume *volu) const;
	     int  MakeHitPlaneInfo(const StTGeoIter &iter);
	void  AddHitPlane(StHitPlane *pla);

const StHitPlane *AddHit(void *hit,const float xyz[3],unsigned int hardw,int seed);

        void  ShootZR(double z,double rxy);

const StTGeoHitShape* GetHitShape() const {return fHitShape;}

static void Test();
static void Break(int kase);
private:


private:
char fBeg[1];
int fMode; 	//0=fill infos + hitShape, 1= hit planes
int fModuLev;
Long64_t fActiveModu;
TObjArray      *fVoluInfoArr;		// array of all StVoluIinfo
TObjArray      *fHitPlaneArr;           // array of StHitPlane's
StHitPlaneHardMap *fHitPlaneHardMap;	// StHitPlane[hardwarePosition]
StVoidArr  *fSeedHits;			// Vector for hits used in seed finder
StTGeoHitShape *fHitShape;
char fEnd[1];
ClassDef(StTGeoHelper,0) //
};


// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __
class StTGeoIter
{
public:
                  StTGeoIter();
                 ~StTGeoIter();
             void Reset();
StTGeoIter   &operator++();
const TGeoVolume *operator*() const {return fVolu;}
StTGeoIter   &Next();
StTGeoIter   &Down();
StTGeoIter   &Upp();

const TGeoNode    *GetNode(int idx=0) const;
const TGeoVolume  *GetVolu(int idx=0) const;
const TGeoHMatrix *GetMatr(int idx=0) const;
             int  GetLev() const {return fLev;}
             int  GetIdx() const {return (fLev)? fStk[fLev-1]:0;}
const        int *GetStk() const {return fStk;}
             int  IsFirst()const {return fNow ==1;}
             int  IsLast ()const {return fNow ==2;}
             int  IsEnd()  const {return fVolu==0;}
             int  State()  const {return fNow;}
            void  LocalToMaster    (const double* local, double* master) const;
            void  LocalToMasterVect(const double* local, double* master) const;
            void  LocalToMaster    (const float*  local, float*  master) const;
            void  LocalToMasterVect(const float*  local, float*  master) const;
            void  Print(const char *tit="") const;
const       char *GetPath() const;     
private:

private:
	int  fNow; // 1=first visit,2=last visit,3=first and last visit        	
	int  fKase;
TGeoVolume    *fVolu;
TGeoNavigator *fNavi;
	int  fLev;
	int  fStk[100];
    
};
// _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ __
class StTGeoHitShape
{
public:
     StTGeoHitShape(double zMin,double zMax);
void Update(double z1, double z2, double rxy);
int  Inside(double z,double rxy) const;
private:
enum {kNZ=100};
double fZMin;
double fZMax;
double fRxy[kNZ];
};
#endif //ST_TGEOHELPER_H   
   
