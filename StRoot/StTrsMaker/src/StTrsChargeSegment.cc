/***************************************************************************
 *
 * $Id: StTrsChargeSegment.cc,v 1.11 1999/03/16 02:00:40 lasiuk Exp $
 *
 * Author: brian May 18, 1998
 *
 ***************************************************************************
 *
 * Description: Input charge segment...much like a g2t_tpc_hit but
 *              with added functionality
 *
 ***************************************************************************
 *
 * $Log: StTrsChargeSegment.cc,v $
 * Revision 1.11  1999/03/16 02:00:40  lasiuk
 * use STL in ionization generation;
 * do not always use betaGamma as a scale factor;
 * still a problem with the neutrals depositing energy
 *
 * do not always use betaGamma as a scale factor;
 * still a problem with the neutrals depositing energy
 *
 * Revision 1.10  1999/03/15 13:49:30  lasiuk
 * field is initialized with Tesla in the data base!
 *
 * Revision 1.9  1999/03/02 17:51:45  lasiuk
 * geant PID
 *
 * Revision 1.8  1999/02/28 20:15:17  lasiuk
 * splitting test/add muon to pid
 *
 * Revision 1.7  1999/02/18 21:18:33  lasiuk
 * rotate() mods to StTpcCoordinateTranform
 *
 * Revision 1.6  1999/02/16 18:15:41  fisyak
 * Check in the latest updates to fix them
 *
 * Revision 1.5  1999/02/12 01:26:37  lasiuk
 * Limit debug output
 *
 * Revision 1.4  1999/02/10 18:02:24  lasiuk
 * verbose output and ostream
 *
 * Revision 1.3  1999/01/28 02:50:28  lasiuk
 * beta gamma for particle mass
 *
 * Revision 1.2  1999/01/15 10:59:11  lasiuk
 * remove g2t pointer
 * add pid member; add systemofunits; mv access fcts to .hh
 * add ostream operator
#ifdef __sun
 **************************************************************************/
#include <algorithm>
#if  defined(__sun) && ! defined(__GNUG__)
#include "ospace/stl/src/randgen.cpp"
#endif

#include "StPhysicalHelix.hh"

#include "StTrsChargeSegment.hh"
#include "StTpcCoordinateTransform.hh"
#include "StTrsDeDx.hh"


StTrsChargeSegment::StTrsChargeSegment()
    : mPosition(0,0,0), mMomentum(0,0,0)
{
    mNumberOfElectrons = -1;
    mDE = 0;
    mDs = 0;
    mPid = 0;
    mSectorOfOrigin = 0;
    mSector12Position = mPosition;
    //mG2tTpcHit = new g2t_tpc_hit;
}

StTrsChargeSegment::StTrsChargeSegment(StThreeVector<double>& pos,
				       StThreeVector<double>& mom,
				       double de,
				       double ds,
				       int    pid,
				       float  ne)
    : mPosition(pos), mMomentum(mom)
{
    mNumberOfElectrons = ne;   // default is -1
    mDE = de;
    mDs = ds;
    mPid = pid;                // default is -1
    mSectorOfOrigin = 0;
    mSector12Position = mPosition;
}

StTrsChargeSegment::~StTrsChargeSegment() {/* nopt */ }


void StTrsChargeSegment::rotate(StTpcGeometry* geodb, StTpcSlowControl* SCdb, StTpcElectronics* elecDb)
{ // rotate to sector 12 ---use a coordinate transform:
    StTpcCoordinateTransform transformer(geodb, SCdb, elecDb);

    cout << "rotate() position= " << mPosition << endl;

    mSector12Position =
	transformer.sector12Coordinate(mPosition, &mSectorOfOrigin);

    PR(mSectorOfOrigin);
    PR(mSector12Position);
}

#ifndef ST_NO_TEMPLATE_DEF_ARGS
void StTrsChargeSegment::split(StTrsDeDx*       gasDb,
			       StMagneticField* magDb,
			       int subSegments, 
			       list<StTrsMiniChargeSegment>* listOfMiniSegments)
#else
void StTrsChargeSegment::split(StTrsDeDx*       gasDb,
			       StMagneticField* magDb,
			       int subSegments,
			       list<StTrsMiniChargeSegment,allocator<StTrsMiniChargeSegment> >* listOfMiniSegments)
#endif
{
#ifndef ST_NO_NAMESPACES
    using namespace units;
#endif

#ifndef ST_NO_TEMPLATE_DEF_ARGS
	vector<int>   ionization(StTrsDeDx::numberOfElectrons);
	vector<float> theIonization;

#else
	vector<int, allocator<int> >     ionization(StTrsDeDx::numberOfElectrons);
	vector<float, allocator<float> > theIonization;
#endif


    //
    // Calculate the number of electrons in complete segment
    //
    if (mNumberOfElectrons<0)
	mNumberOfElectrons = (mDE)/(gasDb->W());

//     PR(mDE/eV);
//     PR(gasDb->W());
//     PR(mNumberOfElectrons);
    //
    // Only do costly initialization if you
    // break into more than one subsegment
    //
     double particleMass;
     int    charge;

     if(subSegments>1) {
	 //
	 // PID are GEANT3 Conventions
	 // Must get from pid structures
	 switch (mPid) {
	 case 2:    // e+
	     particleMass = .00051099906*GeV;
	     charge = 1;
	     break;
	 case 3:    // e-
	     particleMass = .00051099906*GeV;
	     charge = -1;	      
	     break;
	 case 5:    // muon+
	     particleMass = .105658389*GeV;
	     charge = 1;
	     break;
	 case 6:    // muon-
	     particleMass = .105658389*GeV;
	     charge = -1;
	     break;
	 case 8:    // pion+
	     particleMass = .1395700*GeV;
	     charge = 1;
	     break;
	 case 9:    // pion-
	     particleMass = .1395700*GeV;
	     charge = -1;
	     break;
	 case 11:    // kaon+
	     particleMass = .493677*GeV;
	     charge = 1;
	     break;
	 case 12:    // kaon-
	     particleMass = .493677*GeV;
	     charge = -1;
	     break;
	 case 14:    // proton
	     particleMass = .93827231*GeV;
	     charge = 1;
	     break;
	 case 15:    // anti-proton
	     particleMass = .93827231*GeV;
	     charge = -1;
	     break;
	 default: // Probably uncharged, but DO NOT BREAK IT!
	     //cout << "Mass is Undefined (" << mPid << ")" << endl;
	     subSegments = 1;
	     break;
	 }
     }

     //
     // Double Check
     if(subSegments > 1) {  // if an uncharged particle deposits energy, do not split it

	 // what is the subsegment length?
	 
	 double deltaS = mDs/static_cast<double>(subSegments);

    // set the segment length in the gasDb:
	 gasDb->setPadLength(deltaS*centimeter);
// 	 cout << "StTrsDeDx::padLength() -->        "  << (gasDb->padLength()/millimeter) << " mm" << endl;
// 	 cout << "StTrsChargeSegment::split() de--> " << (this->dE()/eV)                 << " eV" << endl;
// 	 cout << "StTrsChargeSegment::split() ds--> " << (this->ds()/millimeter)         << " mm" << endl;
// 	 cout << "StTrsDeDx::W() -->                "   << (gasDb->W()/eV)                 << " eV"  << endl;
// 	 cout << "StMagneticField:at() -->          " << (magDb->at(mSector12Position)/tesla)    << " T"  <<endl;
//     PR(mSector12Position);

     //
     // theIonization
     //
	 theIonization.clear();

     // number of segments to split given by command line argument (default 1):
     //  should be related to mNumberOfElectrons
	 
	 double ionizationLeft = mNumberOfElectrons;

	 //PR(subSegments);
	 for(int ii=0; ii<(subSegments-1); ii++) {
	     //
	     // generate electrons
	     double betaGamma = abs(mMomentum)/particleMass;
// 	     PR(betaGamma);
	     if(betaGamma>3)
		 gasDb->electrons(ionization, betaGamma);
	     else
		 gasDb->electrons(ionization);  // betaGamma = 3
	     
// 	     PR(ionization[StTrsDeDx::primaries]);
// 	     PR(ionization[StTrsDeDx::secondaries]);
// 	     PR(ionization[StTrsDeDx::total]);
// 	     PR(ionizationLeft);
	    
	    // Don't generate too much ionization
	     if(ionization[StTrsDeDx::total] > ionizationLeft) {
		 theIonization.push_back(ionizationLeft);
	     }
	     else {
		 theIonization.push_back(ionization[StTrsDeDx::total]);
	     }
	
	     ionizationLeft -= theIonization.back();
// 	     PR(ionizationLeft);
	     
	 } // loop over subsegments
	 theIonization.push_back(ionizationLeft);

	 //copy(theIonization.begin(),theIonization.end(), ostream_iterator<float>(cout,","));
	 random_shuffle(theIonization.begin(), theIonization.end());
	 //cout << endl;
	 //copy(theIonization.begin(), theIonization.end(), ostream_iterator<float>(cout,","));
     
     //To decompose track use the helix parameterization.
     //StPhysicalHelix(p,x,B,+/-)
     // Need some track info from pid:

	 StPhysicalHelix
	     track(mMomentum,
		   mSector12Position,
		   (magDb->at(mSector12Position)).z(), //*tesla NOT now!
		   charge);

//  	PR(particleMass/GeV);
// 	PR(mMomentum.mag());

	//
	// calculate the offset to the start position
	//
	 double newPosition = -mDs/2. + deltaS/2.;
	//PR(newPosition);
	
	//
	// loop over all subSegments and distribute charge
	//
    
	//cout << "ionization.size() " << (ionization.size()) << endl;
// 	PR(subSegments);
	for(ii=0; ii<subSegments; ii++) {
	    // Take the electrons from the vector

	    // If there is no Ionization, take the next
//  	    cout << "theIonization[" << ii << "]:" << (theIonization[ii]) << endl;
	    if(!theIonization[ii]) {
		newPosition += deltaS;
		continue;
	    }
	
 	    //PR(newPosition);
 	    //PR(track.at(newPosition));
	
	    StTrsMiniChargeSegment aMiniSegment(track.at(newPosition),
						theIonization[ii],
						deltaS);
	    listOfMiniSegments->push_back(aMiniSegment);

	    newPosition += deltaS;
// 	PR(newPosition);
	} // loop over subsegments

     } // if (subsegments > 1)  ---> allows us to skip the helix construction
     else if(subSegments == 1) {
	 StTrsMiniChargeSegment aSingleMiniSegment(mPosition,
						   mNumberOfElectrons,
						   mDs);
	 listOfMiniSegments->push_back(aSingleMiniSegment);
// 	PR(mPosition);
// 	PR(mNumberOfElectrons);
     }

}

// Non-member
ostream& operator<<(ostream& os, const StTrsChargeSegment& seg)
{
    return os << '(' << seg.position() << ", " << seg.momentum() << ", " << seg.dE() << ", " << seg.ds() << ')';
}
