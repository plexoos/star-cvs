// $Id: StFtpcSlowSimulator.cc,v 1.7 2001/04/20 12:50:29 jcs Exp $
// $Log: StFtpcSlowSimulator.cc,v $
// Revision 1.7  2001/04/20 12:50:29  jcs
// cleanup comments
//
// Revision 1.6  2001/04/02 12:04:39  jcs
// get FTPC calibrations,geometry from MySQL database and code parameters from StarDb/ftpc
//
// Revision 1.5  2001/03/19 15:53:10  jcs
// use ftpcDimensions from database
//
// Revision 1.4  2001/03/06 23:36:24  jcs
// use database instead of params
//
// Revision 1.3  2001/01/11 17:37:25  jcs
// replace math.h with PhysicalConstants.h
//
// Revision 1.2  2000/11/27 14:08:08  hummler
// inplement tzero and lorentz angle correction factor
//
// Revision 1.1  2000/11/23 10:16:44  hummler
// New FTPC slow simulator in pure maker form
//
//
///////////////////////////////////////////////////////////////////////////
//
// This is the main routine  for the FTPC Simulator
//

#include "PhysicalConstants.h"

#include "StFtpcSlowSimulator.hh"
#include "StFtpcSlowSimField.hh"
#include "StFtpcSlowSimCluster.hh"
#include "StFtpcSlowSimReadout.hh"
#include "StFtpcRawWriter.hh"
#include "StFtpcClusterMaker/StFtpcGeantReader.hh"
#include "StFtpcClusterMaker/StFtpcParamReader.hh"
#include "StFtpcClusterMaker/StFtpcDbReader.hh"

#ifndef DEBUG
#define DEBUG 0
#endif

StFtpcSlowSimulator::StFtpcSlowSimulator(StFtpcGeantReader *geantReader,
					 StFtpcParamReader *paramReader,
                                         StFtpcDbReader *dbReader,
					 StFtpcRawWriter *rawWriter)
{
  mGeant = geantReader;
  mParam = paramReader;
  mDb    = dbReader;
  mWriter = rawWriter;
}

int StFtpcSlowSimulator::simulate()
{
    int i;

    if(DEBUG) cout << "Begin Initialization..." << endl;

    // setup fields
    StFtpcSlowSimField *field = new StFtpcSlowSimField(mParam, mDb); // define the field
    if(DEBUG) field->Output();                  // write out field 

    // setup readout
    float *ADC = new float[mDb->numberOfPadrows()
			  *mDb->numberOfSectors()
			  *mDb->numberOfPads()
			  *mDb->numberOfTimebins()];      
    
    StFtpcSlowSimReadout *rdout = new StFtpcSlowSimReadout(mParam, mDb, ADC, field);
    // create readout
    if(DEBUG) rdout->Print();

    /////////////////////////////////////
    // big loop over all the hit points
    /////////////////////////////////////
    if (DEBUG) cout << "Looping over points..." << endl;

    // tmp variables
    float electron;      
    float rad_off;
    float pad_off;
    float rad;
    float phi=0.;
    float drift_time;

    float aip = mDb->gasIonizationPotential()*1.0e-9;           // in GeV
    float px, py, pz, pp;
    float xx, yy, zz;
    float de;
    float r_min    = mDb->sensitiveVolumeInnerRadius();
    float r_max    = mDb->sensitiveVolumeOuterRadius();
    float dip_ang, cross_ang;

    int number_hits = mGeant->numberOfHits();
    int rad_rej = 0;
    int de_zero = 0;
    int n_cross_ang_max = 0;
    int counter=0;
    for ( i=0; i<number_hits; ++i ) 
      {
      

	//          px = mGeant->pLocalX(i);
	//          py = mGeant->pLocalY(i);
	//          pz = mGeant->pLocalZ(i);

	// as local momenta from gstar are garbage:
	// use gstar vertex momenta instead.
	// errors should be small, except for shower tracks,
	// where track_p points to the shower generating track...
	px = mGeant->pVertexX(i);
	py = mGeant->pVertexY(i);
	pz = mGeant->pVertexZ(i);
	pp = sqrt ( px*px + py*py );
	
	xx = mGeant->x(i);
	yy = mGeant->y(i);
	zz = mGeant->z(i);
                
//   Test that current point is within chamber          
         rad = sqrt ( xx*xx + yy*yy );
        if(rad < r_min || rad > r_max) {
	  ++rad_rej;
	  continue ;
        }

	de = mGeant->energyLoss(i);
	if ( de == 0 ) {
	  ++de_zero;
	}


//	// TEMPORARY FIX: pad length = 1 in gstar  JCS
//	de = de*20.;
//	// TEMPORARY FIX: pad length = 1 in gstar JCS
	if(DEBUG)
	  cout << "Now processing hit " << i << endl;
	int irow= mGeant->geantVolume(i);
	if ( irow > 200){
	  irow = irow - 201 + 10;
	}
	else  {
	  irow = irow - 101;
	}

	// angle between r and p vectors in xy plane:
        float fpp=0; if (pp>0) fpp=atan2(py,px);
	float alpha = fpp - atan2(yy,xx);
	
	
	// momentum components with respect to r in xy plane:
	float p_perp = pp * sin(alpha);
	float p_rad = pp * cos(alpha);
	if(DEBUG)
	  cout << "alpha=" << alpha 
               << " pperp=" << p_perp 
               << " prad=" << p_rad 
               << " xx = "<<xx
               << " yy = "<<yy
               << " zz = "<<zz
               << " px = "<<px
               << " py = "<<py
               << " pz=" << pz << endl;

	//  dip angle with respect to plane defined by z- and phi- axes.	
	dip_ang   = atan(p_perp / pz);

        //  cross angle with respect to plane defined by z- and r- axes
	cross_ang = atan(p_rad / pz);
	if(cross_ang>halfpi) cross_ang = cross_ang - pi;

	//   Limit cross_ang and dip_angle to 1.5 to avoid nonsensical results
        //   Holm has inactivated these tests
 	float ang_max = 0;
 	if ( fabs( cross_ang) > ang_max ) {
 	  cross_ang = ang_max*cross_ang/fabs(cross_ang) ;
 	  ++n_cross_ang_max;
 	}
 	if ( fabs( dip_ang) > ang_max ) {
 	  dip_ang = ang_max*dip_ang/fabs(dip_ang) ;
 	  ++n_cross_ang_max;
 	}

        // calculate the polar coordinates
        //  so that phi-phi_min >= 0.0  (phi_min=halfpi)

        if (xx > 0.0) {
                phi = twopi + atan(yy/xx); 
        }
        else if (xx < 0.0) {
                phi = pi + atan(yy/xx);
        }
        else {
            if (yy >= 0.0)
                phi = halfpi;
            else if (yy < 0.0) 
                phi = 1.5*pi ;
        }


         
        // define cluster for each accepted hit point
        ++counter;
        drift_time  = -mDb->tZero();                          
        electron    = de / aip;
	
        rad_off     = rdout->GetPadLength() * tan(dip_ang);
        pad_off     = rdout->GetPadLength() * tan(cross_ang); // in cm


        if (DEBUG) {
            cout << " ##### Point i= " << i
                 << " counter=" << counter
                 << " nel=" << electron
                 << " rad=" << rad
                 << " phi=" << phi
                 << endl;
            cout << " #####            "
                 << " rad_off=" << rad_off
                 << " pad_off=" << pad_off
                 << endl;
           cout << " x = " << xx
                << " y = " << yy
                << " z = " << zz << endl;
           cout << " px = " << px
                << " py = " << py
                << " pz = " << pz
                << " pp = " << pp
                << " de = " << de << endl;
           cout << " dip_angle = " << dip_ang
                << " cross_angle = " << cross_ang
                << " row_id = " << mGeant->geantVolume(i)
                << " track_id = " << mGeant->track(i)+1
                << " ge_pid = " << mGeant->trackPid(i)
                << endl;
        }
	StFtpcSlowSimCluster *clus = 
	  new StFtpcSlowSimCluster(mParam, mDb, field, electron, rad_off, pad_off, 
				   rad, phi, drift_time, irow);

	clus->DriftDiffuse(field); // transport the cluster

        rdout->Avalanche(clus); // multiply electrons

        rdout->PadResponse(clus);            // response on pad 

        rdout->ShaperResponse(clus);  // response on shaper 

        rdout->Digitize(clus, irow);         // digitize signal

       	delete clus;

    } // end of loop over hit points
    
   if (DEBUG) {
       cout << "Total number of hit points tested = " << number_hits << endl;
       cout << "Number of hit points accepted = " << counter << endl;
       cout << "Number of hit points rejected (radius test) = " << rad_rej << endl;
       cout << "Number of hit points rejected (de=0 test) = " << de_zero << endl;
     //cout << "Number of hit points with cross_ang > cross_ang_max  = " << n_cross_ang_max << endl;
       cout << "Writing out ADC array in raw data structure." << endl;
    }

   rdout->OutputADC();

   mWriter->writeArray(ADC,
		       mDb->numberOfPadrows(),
		       mDb->numberOfSectors(),
		       mDb->numberOfPads(),
		       mDb->numberOfTimebins(),
		       mParam->zeroSuppressThreshold());

    delete rdout;
    delete field;
    delete[] ADC;

    return 1;
}

StFtpcSlowSimulator::~StFtpcSlowSimulator()
{

}
