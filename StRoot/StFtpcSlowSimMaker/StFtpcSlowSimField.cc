// $Id: StFtpcSlowSimField.cc,v 1.7 2001/04/24 12:54:59 oldi Exp $
// $Log: StFtpcSlowSimField.cc,v $
// Revision 1.7  2001/04/24 12:54:59  oldi
// mParam->slowSimPressure() used instead of mParam->normalizedNowPressure().
//
// Revision 1.6  2001/04/02 12:04:33  jcs
// get FTPC calibrations,geometry from MySQL database and code parameters from StarDb/ftpc
//
// Revision 1.5  2001/03/19 15:53:10  jcs
// use ftpcDimensions from database
//
// Revision 1.4  2001/03/06 23:36:01  jcs
// use database instead of params
//
// Revision 1.3  2001/01/11 18:28:44  jcs
// use PhysicalConstants.h instead of math.h, remove print statement
//
// Revision 1.2  2000/11/27 14:08:00  hummler
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
//
//  Modifications:
//          02/27/98    Janet Seyboth   correct definition of new arrays
///////////////////////////////////////////////////////////////////////////

#include "PhysicalConstants.h"

#include "StFtpcSlowSimField.hh"
#include "StFtpcClusterMaker/StFtpcParamReader.hh"
#include "StFtpcClusterMaker/StFtpcDbReader.hh"

StFtpcSlowSimField::StFtpcSlowSimField(StFtpcParamReader *paramReader,
                                       StFtpcDbReader *dbReader)
{
  mParam=paramReader;
  mDb = dbReader;
  // inplementation of drift information from fcl_padtrahns, which is
  // more precise than that in ftpcSlowSimGas (includes calculated magfield)
  // there's a job to do: fuse both tables into one!
  innerRadius = mDb->sensitiveVolumeInnerRadius();
  outerRadius = mDb->sensitiveVolumeOuterRadius();
  angleFactor = mParam->lorentzAngleFactor();
  grid_point = new grid_data[mParam->numSlowSimGridPoints()];
  
  int i;
  float cathodeVoltage = mParam->chamberCathodeVoltage();
  
  // grid size
  del_r = (outerRadius - innerRadius)
    / (float) (mParam->numSlowSimGridPoints()-1);
  inverseDeltaRadius = 1/del_r; // '*' faster than '/' in later calculations
  twoDeltaRadius = 2*del_r;
  
  int num_data = mParam->numberOfFssGasValues();    // number of data point
  float * in_efield  = new float[num_data];
  float * in_velocity_z = new float[num_data];
  float * in_diffusion_z = new float[num_data];
  float * in_diffusion_x = new float[num_data];
  float * in_diffusion_y = new float[num_data];
  float * in_angle_lorentz = new float[num_data];
  for ( i=0; i<num_data; i++) {
    in_efield[i]        = mParam->fssGasEField(i);
    in_velocity_z[i]    = mParam->fssGasVDrift(i);
    in_diffusion_z[i]   = mParam->fssGasDiffusionZ(i);
    in_diffusion_x[i]   = mParam->fssGasDiffusionX(i);
    in_diffusion_y[i]   = mParam->fssGasDiffusionY(i);
    in_angle_lorentz[i] = mParam->fssGasLorentzAngle(i);
  }
  
  float eff;
  // big loop to define the field
  for ( i=0; i<mParam->numSlowSimGridPoints(); i++) {
    
    grid_point[i].rhit = innerRadius + i * del_r;

    eff = fabs(cathodeVoltage) / log(outerRadius / innerRadius) 
      / grid_point[i].rhit;
    grid_point[i].ef   = eff;
    
    int index = Locate(num_data, in_efield, eff);
    
    grid_point[i].vel_z   
      = Interpolate(num_data,in_efield,in_velocity_z,index,eff);
    float diffusionX=Interpolate(num_data,in_efield,in_diffusion_x,
				 index,eff);
    float diffusionY=Interpolate(num_data,in_efield,in_diffusion_y,
				 index,eff);
    float diffusionZ=Interpolate(num_data,in_efield,in_diffusion_z,
				 index,eff);
    grid_point[i].diff_z  
      = diffusionZ*diffusionZ;
    grid_point[i].diff_x  
      = diffusionX*diffusionY;
  }
  
  // calculate drift velocity gradients
  
  for (i=mParam->numSlowSimGridPoints()-1; i>0; i--) {
    grid_point[i].dlnv_dr 
      = ( 1. - grid_point[i-1].vel_z/grid_point[i].vel_z)
      *inverseDeltaRadius;
  }
  grid_point[0].dlnv_dr = grid_point[1].dlnv_dr; 
  delete [] in_efield;
  delete [] in_velocity_z;
  delete [] in_diffusion_z;
  delete [] in_diffusion_x;
  delete [] in_diffusion_y;
  delete [] in_angle_lorentz;
  
  radTimesField = mDb->radiusTimesField();
  nPadrowPositions = mDb->numberOfPadrowsPerSide();
  nMagboltzBins = mDb->numberOfMagboltzBins();
  preciseEField = new float[nMagboltzBins];
  inverseDriftVelocity = new float[nMagboltzBins*nPadrowPositions];
  preciseLorentzAngle = new float[nMagboltzBins*nPadrowPositions];
  
  float deltaP = mParam->slowSimPressure()-mParam->standardPressure();
  for(int i=0; i<nMagboltzBins; i++)
    {
      preciseEField[i] = mDb->magboltzEField(i);
      for(int j=0; j<nPadrowPositions; j++)
	{
	  inverseDriftVelocity[i+nMagboltzBins*j] = 
	    1/ (mDb->magboltzVDrift(i,j)
		+ deltaP * mDb->magboltzdVDriftdP(i,j));
	  preciseLorentzAngle[i+nMagboltzBins*j] = 
	    mDb->magboltzDeflection(i,j) 
	    + deltaP * mDb->magboltzdDeflectiondP(i,j);
	}
    }
  EFieldMin=preciseEField[0];
  EFieldStep=preciseEField[1]-EFieldMin;
  EFieldStepInverted= 1/EFieldStep;
  EFieldStepInvConverted= EFieldStepInverted * degree;
  finalVelocity = grid_point[mParam->numSlowSimGridPoints()-1].vel_z*10.;

}

StFtpcSlowSimField::~StFtpcSlowSimField() {
  delete[] preciseEField;
  delete[] inverseDriftVelocity;
  delete[] preciseLorentzAngle;
  delete[] grid_point;
}

float StFtpcSlowSimField::Interpolate(const int npt, const float* x, 
				      const float* y,const int ich, 
				      const float xx)
{
// 
// this subroutine will Interpolate the value (xx,yy) from
// the arrays {x(1:npt), y(1:npt)} at a given channel ich.
// 
    float x1[5];
    float y1[5];
    register int i;

    if (ich < 2) {
        for ( i=0; i<2; i++) {
            x1[i] = *(x+i) ;
            y1[i] = *(y+i) ;
        }
        return  InterpValue(2, x1, y1, xx);
    }
    else if (ich > npt-3) {
        for ( i=0; i<2; i++) {
            x1[i] = *(x+npt-1-i) ;
            y1[i] = *(y+npt-1-i) ;
        }
        return  InterpValue(2, x1, y1, xx);
    }
    else {
        for ( i=0; i<5; i++) {
            x1[i] = *(x+ich-2+i) ;
            y1[i] = *(y+ich-2+i) ;
        }
        return  InterpValue(5, x1, y1, xx);
    }
}

float StFtpcSlowSimField::InterpValue(const int npt, const float* x, 
				      const float* y, const float xx)
{
//
// use Newton's method to Interpolate value at x
//
    float term;
    float sum = 0;

    for(register int i=0; i < npt; i++) {
        term = y[i];
        for(register int j=0; j < npt; j++) {
            if (j != i) term *= (xx-x[j])/(x[i]-x[j]);
        }
        sum += term;
    }
    return sum;
}

void StFtpcSlowSimField::Output() const
{
    ofstream fout("field.dat");
    char space = ' ';
    // print out the field value to check
    for (int  i=0; i<mParam->numSlowSimGridPoints(); i++) {
       fout << grid_point[i].rhit << space 
            << grid_point[i].ef << space 
            << grid_point[i].vel_z << space
            << grid_point[i].diff_z<< space 
            << grid_point[i].diff_x<< space 
            << grid_point[i].lorentz/degree << space 
            << grid_point[i].dlnv_dr 
		 *mParam->lorentzAngleFactor()
            << endl;
    }
}

