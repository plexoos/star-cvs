// $Id: StFtpcSlowSimField.hh,v 1.3 2001/03/06 23:36:05 jcs Exp $
// $Log: StFtpcSlowSimField.hh,v $
// Revision 1.3  2001/03/06 23:36:05  jcs
// use database instead of params
//
// Revision 1.2  2000/11/27 14:08:04  hummler
// inplement tzero and lorentz angle correction factor
//
// Revision 1.1  2000/11/23 10:16:43  hummler
// New FTPC slow simulator in pure maker form
//
//
///////////////////////////////////////////////////////////////////////////
//  Author: W.G.Gong
//  Email: gong@mppmu.mpg.de
//  Date:  Oct 25, 1996
///////////////////////////////////////////////////////////////////////////
#ifndef STAR_StFtpcSlowSimField
#define STAR_StFtpcSlowSimField

class Inparam;
class StFtpcParamReader;
class StFtpcDbReader;
extern  int Locate(const int npt, const float* x, const float xx);

//
//  class field  will divide radius into grid points and 
//  define efield and gas parameters at grid points.
//
class StFtpcSlowSimField
{
public:
  StFtpcSlowSimField(StFtpcParamReader *paramReader,
                     StFtpcDbReader    *dbReader);
  ~StFtpcSlowSimField();
  float Interpolate(const int npt, const float* x, 
		    const float* y,const int ich, 
		    const float xx);
  float InterpValue(const int npt, const float* x, 
		    const float* y, const float xx);
  void Output() const;     // write out the field value for checking

  int GetGridIndex(const float r) const
    { return ((int) ((r - innerRadius)*inverseDeltaRadius) ) ; }

  // GetVelocityZ is inline although too long because it is called in the
  // main drift loop
  inline void GetVelocityZ(const float inverseRadius, const int padrow, float *inverseVelocity, float *angle)  
    {
      float e_now=radTimesField * inverseRadius;
      int iLower= (int)((e_now-EFieldMin)*EFieldStepInverted);
      int iUpper= iLower + 1;
      int padrowIndex= nMagboltzBins*padrow;
      float diffUp=preciseEField[iUpper]-e_now;
      float diffDown=e_now-preciseEField[iLower];
      iLower+=padrowIndex;
      iUpper+=padrowIndex;
      *inverseVelocity = EFieldStepInverted*((inverseDriftVelocity[iUpper])*diffDown + (inverseDriftVelocity[iLower])*diffUp);
      *angle = EFieldStepInvConverted*((preciseLorentzAngle[iUpper])*diffDown + (preciseLorentzAngle[iLower])*diffUp)*angleFactor;
    }    

  float GetVelAtReadout() const { return  finalVelocity; }
  
  float GetDeltaRadius() const { return del_r; }

  float GetInverseDeltaRadius() const { return inverseDeltaRadius; }

  float GetTwoDeltaRadius() const { return twoDeltaRadius; }

  float GetDiffusionXSqr(const int i) const {return grid_point[i].diff_x;}

  float GetDiffusionZSqr(const int i) const {return grid_point[i].diff_z;}

  float GetDlnvDr(const int i) const {return grid_point[i].dlnv_dr;}
  
private:
  StFtpcParamReader *mParam;
  StFtpcDbReader *mDb;
  // variable declarations
  float del_r;             // radial grid size
  float inverseDeltaRadius; // 1/del_r
  float twoDeltaRadius; // 2*del_r
  float finalVelocity;
  // variables to handle fcl_padtrans information:
  int nMagboltzBins; 
  int nPadrowPositions;
  float EFieldMin;
  float EFieldStep;
  float EFieldStepInverted;
  float EFieldStepInvConverted; // include pi/180
  float radTimesField;
  float innerRadius;
  float outerRadius;
  float *preciseEField;
  float *inverseDriftVelocity;
  float *preciseLorentzAngle;
  float angleFactor;
  
  struct grid_data {
    float rhit;    // hit radius in cm
    float ef;    // Efield in V/cm
    float vel_z;    // long.  drift speed in cm/usec
    float diff_z;    // long. diffusion coeff. in um/sqrt(cm)
    float diff_x;    // trans. diffusion coef. in um/sqrt(cm)
    float diff_y;    // trans. diffusion coef. in um/sqrt(cm)
    float lorentz;    // Lorentz angle in rad
    float dlnv_dr;    // drift velocity gradient
  };
  grid_data *grid_point;
};

#endif

