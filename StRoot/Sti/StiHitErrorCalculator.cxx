//StiHitErrorCalculator.cxx
//Andrew Rose, Wayne State University
//01.21.2002

/*! \class StiHitErrorCalculator

  Real errors are generated by fitting and parameterizing a histogram of
  hit residuals in 3D (radius, dip angle, and drift time). The original 
  parameterization method used in Tpt is determined by StRedsidualMaker 
  and implemented in tpt_hit_uncert, both by Mike Lisa. The method used 
  here closely follows his code.

  For details of Mike's analysis of the hit errors, see his web page:
  http://www.star.bnl.gov/~lisa/HitErrors (Mar 14, 2001)

  The default error function returns a zero error over all space. This 
  should allow easier error checking for pathalogic and error behavior.


  \author A. A. Rose (Wayne State)
  \date 2.20.2002
*/

#include <utility>
using std::pair;
#include <cmath>
#include <float.h>
#include <iostream>

//StiIncludes
#include "StiIOBroker.h"
#include "StiDetector.h"
#include "StiHit.h"
#include "StiHitErrorCalculator.h"

class StiIOBroker;
class StiHit;
class StiDetector;

#define DEG2RAD 57.2958

StiHitErrorCalculator::StiHitErrorCalculator()
  :mIOBroker(StiIOBroker::instance())
{}

StiHitErrorCalculator::~StiHitErrorCalculator()
{ }

StiHitErrorDefault::StiHitErrorDefault()
     :fTpcSource(0), fSvtSource(0), fFtpcSource(0)
{
  Init();
}

StiHitErrorDefault::~StiHitErrorDefault()
{}

void StiHitErrorDefault::Init()
{
  //set parameters from IOBroker->Database or defaults
  //should probably get a list of detectors from Detector Factory?
  
  //get TPC parameters from database
  //no brokered io for now, set function to defaults
  SetTpcInnerParam(3.12735e-03,1.51055e-04,2.43806e-02,1.68243e-03,5.23272e-05,5.75341e-02);
  SetTpcOuterParam(8.158e-03,5.69582e-05,4.48440e-02,2.02775e-04,3.55219e-05,6.45610e-02);
  //get Ftpc parameters from database
  //SetFtpcParameterization(defaultFunc);

  //get SVT parameters from database
  //SetSvtParameterization(defaultFunc);

}

//get methods
pair<double, double> StiHitErrorDefault::getHitError(StiHit* mHit, double cross, double dip)
{
  
  //retrieve detector info and set function pointer
  if(strstr(mHit->detector()->getName().c_str(), "Tpc")!=NULL)
    {
      double rad=sqrt(mHit->x()*mHit->x()+mHit->y()+mHit->y());
      //temp cludge until padrow dep. istituted
       if(rad<100.) return (TpcInnerHitError(mHit, cross, dip));
       return (TpcOuterHitError(mHit,cross,dip));
    }
  else if (strstr(mHit->detector()->getName().c_str(), "Ftpc")!=NULL) 
    {
      return FtpcHitError(mHit,cross,dip);
    }
  else if (strstr(mHit->detector()->getName().c_str(), "Svt")!=NULL) 
    {
      return SvtHitError(mHit,cross,dip);
    }
  else       
    {
      cout <<"StiHitErrorDefault::getHitError: \n"
	   <<"\t Request for errors of unsupported detector:"
	   <<mHit->detector()->getName()
	   <<endl;
      return pair<double,double>(DBL_MAX,DBL_MAX);
    }
}

int StiHitErrorDefault::querySource(StiHit* mHit)
{
 
  if(strstr(mHit->detector()->getName().c_str(), "Tpc")!=NULL)
    {
      return (fTpcSource);
    }
  else if (strstr(mHit->detector()->getName().c_str(), "Ftpc")!=NULL)
    {
      return (fFtpcSource);
    }
  else if (strstr(mHit->detector()->getName().c_str(), "Svt")!=NULL)
    {
      return (fSvtSource);
    }
  else
    {
      cout << "StiHitErrorDefault::querySource: Error\n"
	   << "\t Query hit error status for undefined detector: "
	   << mHit->detector()->getName() <<endl;
      return -1;
    }
}

//set methods
void StiHitErrorDefault::SetTpcSource(int mSource)  {fTpcSource=mSource;}
void StiHitErrorDefault::SetFtpcSource(int mSource) {fFtpcSource=mSource;}
void StiHitErrorDefault::SetSvtSource(int mSource)  {fSvtSource=mSource;}

void StiHitErrorDefault::SetTpcInnerParam(double integralcross, double difcross, double croscross,
					  double integraldip, double difdip, double crosdip)
{
  TpcInnerErrorParam[0]=integralcross; TpcInnerErrorParam[1]=difcross; TpcInnerErrorParam[2]=croscross;
  TpcInnerErrorParam[3]=integraldip; TpcInnerErrorParam[4]=difdip; TpcInnerErrorParam[5]=crosdip;
}

void StiHitErrorDefault::SetTpcOuterParam(double integralcross, double difcross, double croscross,
					  double integraldip, double difdip, double crosdip)
{
  TpcOuterErrorParam[0]=integralcross; TpcOuterErrorParam[1]=difcross; TpcOuterErrorParam[2]=croscross;
  TpcOuterErrorParam[3]=integraldip; TpcOuterErrorParam[4]=difdip; TpcOuterErrorParam[5]=crosdip;
}

void StiHitErrorDefault::SetSvtParam(double integralcross, double difcross, double croscross,
					  double integraldip, double difdip, double crosdip)
{
  SvtErrorParam[0]=integralcross; SvtErrorParam[1]=difcross; SvtErrorParam[2]=croscross;
  SvtErrorParam[3]=integraldip; SvtErrorParam[4]=difdip; SvtErrorParam[5]=crosdip;
}

void StiHitErrorDefault::SetFtpcParam(double integralcross, double difcross, double croscross,
					  double integraldip, double difdip, double crosdip)
{
  FtpcErrorParam[0]=integralcross; 
  FtpcErrorParam[1]=difcross; FtpcErrorParam[2]=croscross;
  FtpcErrorParam[3]=integraldip; 
  FtpcErrorParam[4]=difdip; FtpcErrorParam[5]=crosdip;
}
						  
pair<double, double> StiHitErrorDefault::TpcInnerHitError(StiHit* mHit, double cross, double dip)
{//return Tpc hit errors based on parameterization provided my Mike Lisa

  //get hit z
  
  double ecross=TpcInnerErrorParam[0]
    -TpcInnerErrorParam[1]*(200.-mHit->z())/cos(cross/DEG2RAD)
    +TpcInnerErrorParam[2]*(tan(cross/DEG2RAD)*tan(cross/DEG2RAD));
  double edip=TpcInnerErrorParam[3]
    -TpcInnerErrorParam[4]*(200.-mHit->z())/cos(dip/DEG2RAD)
    +TpcInnerErrorParam[5]*(tan(dip/DEG2RAD)*tan(dip/DEG2RAD));

  return pair<double,double>(ecross,edip);
}

pair<double, double> StiHitErrorDefault::TpcOuterHitError(StiHit* mHit, double cross, double dip)
{//return Tpc hit errors based on parameterization provided my Mike Lisa

  //get hit z
  
  double ecross=TpcOuterErrorParam[0]
    -TpcOuterErrorParam[1]*(200.- mHit->z())/cos(cross/DEG2RAD)
    +TpcOuterErrorParam[2]*(tan(cross/DEG2RAD)*tan(cross/DEG2RAD));
  double edip=TpcOuterErrorParam[3]
    -TpcOuterErrorParam[4]*(200.- mHit->z())/cos(dip/DEG2RAD)
    +TpcOuterErrorParam[5]*(tan(dip/DEG2RAD)*tan(dip/DEG2RAD));

  return pair<double,double>(ecross,edip);
}

pair<double,double> StiHitErrorDefault::SvtHitError(StiHit* mHit, double cross, double dip)
{
  //not yet implemented
  return pair<double,double>(DBL_MAX,DBL_MAX);
}

pair<double,double> StiHitErrorDefault::FtpcHitError(StiHit* mHit, double cross, double dip)
{
  //not yet implemented
  return pair<double,double>(DBL_MAX,DBL_MAX);
}




