#ifndef StiTrackNode_H
#define StiTrackNode_H 1

#include <Stiostream.h>
#include <stdlib.h>

#include "StiTreeNode.h"
#include "StiHit.h"
class StiDetector;
enum eTkPars {kNPars=6,kNErrs=21};
class StiNodePars {
public:	
void reset(){memset(this,0,sizeof(StiNodePars));_cosCA=1;}
void ready(){_cosCA=cos(_eta);_sinCA=sin(_eta);_curv = _hz*_ptin;}
StiNodePars &merge(double wt,StiNodePars &other);
double  operator[](int idx) const {return P[idx];}
double &operator[](int idx)       {return P[idx];}
int     check(const char *pri=0) const;
void    print() const;

  enum {kX=0,kY,kZ,kEta,kCurv,kTanL};
  /// sine and cosine of cross angle
  double _cosCA;
  double _sinCA;
  union{double P[1];double _x;};
  /// local Y-coordinate of this track (reference plane)           
  double _y; 
  /// local Z-coordinate of this track (reference plane)
  double _z;
  /// (signed curvature)*(local Xc of helix axis - X current point on track)
  double _eta;
  /// signed invert pt [sign = sign(-qB)]
  double _ptin;  
  /// tangent of the track momentum dip angle
  double _tanl;
  /// signed curvature [sign = sign(-qB)]
  double _curv;  
  /// Z component magnetic field in units Pt(Gev) = Hz * RCurv(cm)
  double _hz;  
};
class StiNodeMtx {
public:	
void reset(){memset(this,0,sizeof(StiNodeMtx));}
  double A[kNPars][kNPars];
};



class StiNodeErrs {
public:	
void reset()				{memset(this,0,sizeof(StiNodeErrs));}
double getDelta()  const 		{return sqrt(_cXX+_cYY+_cZZ);}
double getDelta2() const 		{return     (_cXX+_cYY+_cZZ);}
StiNodeErrs &operator*=(double f) 	{for (int i=0;i<kNErrs;i++){A[i]*=f;}; return *this;}
StiNodeErrs &merge(double wt,StiNodeErrs &other);

void get00(      double *a) const;
void set00(const double *a)      ;
void get10(      double *a) const;
void set10(const double *a)      ;
void get11(      double *a) const;
void set11(const double *a)      ;

void recov(); 
int  check(const char *pri=0) const; 
double sign() const; 
double operator()(int i,int j) const;
void zeroX();
void print() const;

public:	
union{double A[1];double _cXX;};
  double _cYX,_cYY;                       
  double _cZX,_cZY, _cZZ;                 
  double _cEX,_cEY, _cEZ, _cEE;           
  double _cPX,_cPY, _cPZ, _cPE, _cPP;     
  double _cTX,_cTY, _cTZ, _cTE, _cTP, _cTT;
};  
class StiHitErrs{
public:
void reset()			 {memset(this,0,sizeof(*this));}
StiHitErrs &operator*=(double f) {for (int i=0;i<6;i++){A[i]*=f;};return *this;}
void rotate(double angle);
union{
  double hXX;		double A[1];};
  double hYX,hYY;                       
  double hZX,hZY, hZZ;                 
};

/*! \class StiNode2pars
  Axiliary class for StiKalmanTrackNode only. 
  Contains only  Node_Y and Node_Z in local frame
  with error matrix. Currently used to keep NOT FITTED to this hit values.
  So difference with hit is not a residuals. Could be used for allignment
  or hit errors fiting 
  \author Victor Perev
*/
class StiNode2Pars
{
public:
float mPar[2];  // Node_Y , Node_Z 
float mErr[3];  // yy,yz,zz
};


/*! \class StiHitContino
  Axiliary class for StiKalmanTrackNode only. Small container
  of about 3 best hits for this node. Used for refit.
  \author Victor Perev
*/

class StiHitContino {
public:
  StiHitContino()			{reset();}
  void   reset(); 		
  StiHit *getHit (int idx) const	{return mHits[idx];}
  int    getNHits () const;		
  double getChi2 (int idx=0) const	{return mChi2[idx];}
  double getDetr (int idx=0) const	{return mDetr[idx];}
  void   add (StiHit *hit,double chi2,double detr=1.);
  void   print (const char* opt="") const;
private:
  enum {kMaxSize=10};
  StiHit *mHits[kMaxSize+1];
  double  mChi2[kMaxSize+1];
  double  mDetr[kMaxSize+1];
};



class StiTrackNode : public StiTreeNode
{ 
public:
enum eTrackNodeStatus {
  kTNReset 	= 0, // TrackNode after reset
  kTNRotBeg 	= 1, // TrackNode start rotation
  kTNRotEnd 	= 2, // TrackNode rotated, errmatrix 3d
  kTNProBeg     = 3, // TrackNode start propagation, errmatrix wrong
  kTNProEnd     = 4, // TrackNode end propagation, errmatrix still wrong
  kTNNudBeg     = 5, // TrackNode start nudge
  kTNFitBeg 	= 6, // TrackNode start fit

  kTNReady	=10, // TrackNode propagateError called,errmatrix OK
  kTNNudEnd     =11, // TrackNode end nudge
  kTNFitEnd 	=12, // TrackNode end fit
  kTNInit 	=13}; // TrackNode initialized
enum eTrackNodeFlags {
  kTNRefit 	= 1 // TrackNode in refit stage
};

  virtual ~StiTrackNode(){};    
  virtual double getPt() const=0;
  const StiTrackNode& operator=(const StiTrackNode& node);  
  void reset();
  void unset(){;}
  StiHit * getHit() const 		{return _hit;}
  void setHit(StiHit* hit)		{_hit   =hit;}
  const StiDetector *getDetector() const; 
  void  setDetector(const StiDetector *detector);
  double getChi2 ()  const		{return _chi2;} 		
  double getDeterm() const		{return _det ;} 		
  void setChi2(double chi2)		{_chi2  =chi2;}
  int getState() const 			{return _state;}
 void setInvalid()  			{ _state=0;}
 void setReady()  			{ _state=kTNReady;}
  int isValid()  const 			{return _state>=kTNReady;}
  int isFitted() const 			{return (_hit && _chi2<1e3);}
  double getRefPosition() const;
  double getLayerAngle()  const;
protected:   
static void mult6(double Rot[kNPars][kNPars],const double Pro[kNPars][kNPars]); 
static void errPropag6(double G[21],const double F[6][6],int nF);
static int  cylCross(double r, const double dx[4],double Rho,double out[4]);
static double  sinX(double x);  // (sin(x)-x)/x**3

  StiTrackNode()			{reset();}    
  int _state;
  const StiDetector * _detector; 
  StiHit* _hit;
  double _det;
  double _chi2;
public:
static int mgFlag;
};


inline void StiTrackNode::reset()
{ 
   StiTreeNode::reset();
  _state = kTNReset;
  _detector = 0;
  _hit=0;
  _chi2 = 1e60;
}


inline const StiDetector * StiTrackNode::getDetector() const
{
  return _detector;
}

inline void StiTrackNode::setDetector(const StiDetector *detector)
{
  _detector = detector; 
}
inline  const StiTrackNode& StiTrackNode::operator=(const StiTrackNode& node)  
{
   //not called StiTreeNode::operator=();    //Relationship is not changed
  _state    = node._state;	
  _detector = node._detector;	
  _hit      = node._hit;
  _chi2     = node._chi2;
  return *this;
}	
	
#endif



