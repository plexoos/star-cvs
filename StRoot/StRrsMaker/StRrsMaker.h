/**********************************************************
 * $Id: StRrsMaker.h,v 2.2 2003/09/10 19:47:31 perev Exp $
 *
 * Description:
 *  StRrsMaker is the main module
 *  StRichRawData. It has the standard Maker functions:
 *
 *  St_RD_Maker is derived from StMaker, the standard
 *  Root4Star Maker.  
 *  Init() is once the first time of execution
 *  Init(int) is a specialized Init to allow
 *  histogramming. Pass 1 for on, 0 for off.
 *  Make() is called for every event
 *  Finish() is called once at the end to clean
 *  memory.
 ***********************************************************
 *  $Log: StRrsMaker.h,v $
 *  Revision 2.2  2003/09/10 19:47:31  perev
 *  ansi corrs
 *
 *  Revision 2.1  2000/08/09 17:59:56  gans
 *  Removed Int_t Clear() which was stomped in ver from 1.12->2.0
 *
 *  Revision 2.0  2000/08/09 16:17:06  gans
 *  Readded Files That were not added in last CVS. Cosmetic Changes, naming convention
 *  for StRichDrawableT(foo)
 *
 *  Revision 1.11  2000/04/14 22:37:22  lasiuk
 *  Recommit code
 *
 *  Revision 1.10  2000/03/17 14:55:15  lasiuk
 *  Large scale revisions after ROOT dependent memory leak
 *
 *  Revision 1.9  2000/03/13 21:58:02  lasiuk
 *  singleton classes
 *
 *  Revision 1.8  2000/02/14 01:08:02  lasiuk
 *  write the data set
 *  add two member functions for pedestal and noise switches
 *  add coordinate conditional and StCoordinateTransform
 *  incorporate track_p into GHit
 *
 *  Revision 1.7  2000/02/12 21:54:25  lasiuk
 *  Introduce provisions to read in local coordinates
 *
 *  Revision 1.6  2000/02/08 23:46:46  lasiuk
 *  comment to prevent streamer for ionize and inducesignal. Remove filter
 *
 *  Revision 1.5  2000/02/08 16:36:51  lasiuk
 *  Bring into line with HP
 *
 *  Revision 1.4  2000/01/27 17:10:04  lasiuk
 *  modify to work stand-alone from ROOT
 *
 *  Revision 1.3  2000/01/26 23:39:19  lasiuk
 *  Forward declaration of classes to bypass CINT evaluation
 *  comment the list data member in StRichID
 *
 *  Revision 1.2  2000/01/25 22:02:23  lasiuk
 *  Second Revision
 *
 *  Revision 1.1  2000/01/18 21:32:05  lasiuk
 *  Initial Revision
 *
 *  7/27/1999 class created. First prototypes of make,
 *    init, finish, induceSignal. Pointers to DBs. 
 *                                    Alexandre Nevski
 *  8/2/1999 Input module plugged in. Alexandre Nevski
 *
 *  8/5/1999 First run in Root framework.
 *           Classdef and Classimp macros added.
 *                                         Valery Fine
 *
 *  8/5/1999 Data read, minor changes.Alexandre Nevski
 *
 *  8/20/1999 Moved includes to DB and Viewer to .cxx
 *                                         Valery Fine
 *
 *  8/25/1999 drawing functions included
 *                                    Caroline Peter
 *  8/30/1999 removed bool for SUN compatibility
 *                                    Alexandre Nevski
 *
 **********************************************************/
#ifdef __ROOT__
#ifndef ST_RRS_MAKER_H
#define ST_RRS_MAKER_H

#ifndef StMaker_H
#include "StMaker.h"                  // Root4Star Maker
#endif

#ifndef HEP_SYSTEM_OF_UNITS_H
#include "SystemOfUnits.h"
#endif
class StParticleTable;

#ifndef ST_NO_NAMESPACES
using namespace units;
#endif

#include "StRichRrsMacros.h"

// DataBases
class StRichGeometryDb;
class StRichPhysicsDb;
class StRichCoordinateTransform;
class StRichMomentumTransform;
class StRichPadPlane;
class StRichWriter;
class StRichAnalogSignalGenerator;

#include "StRichIonization.h"
#include "StRichSelectWire.h"
#include "StRichGasGain.h"
#include "StRichAnalogToDigitalConverter.h"
#include "StRichNoiseSimulator.h"

class StRrsMaker : public StMaker 
{
public:
    StRrsMaker(const char *name="Rrs");
    ~StRrsMaker();
    
    Int_t Init();
    Int_t Init(int histograms);
    Int_t Make();
    Int_t Finish();
    
public:
    void useLocalCoordinate(int);
    void addPedestal(int);
    void addElectricNoise(int);
    
    int readFile(char*);//!
    int writeFile(char*, int);//!
    
    //St_DataSet * getPadPlaneTable();
    //St_DataSet * getIDTable();      
    //int getADC(int row, int col);   
    
    void drawWhichQuadrant();       
    void drawParticleId();          
    void drawPadPlane();            
    void drawClusterElectrons();    
    void drawErrorDetection();      
    void drawWhichWire();           
    void drawFeedback();            
    void drawPolia();               
    void drawAnalogSignals();       
    void drawTotalCharge();         
    void drawADCSignal();           
    void drawNoise();               

private:
    // Decode the volume ID into quadrant and RICH element
    int whichVolume(int val, string* vName);

private:
    StRichGeometryDb               *mGeometryDb;//!
    StRichPhysicsDb                *mPhysicsDb;//!
    StRichCoordinateTransform      *mCoordinateTransform;//!
    StRichMomentumTransform        *mMomentumTransform;//!
    
    // Processing
    StRichIonization                mIonize;//!
    StRichSelectWire                mWireSelector;//!
    StRichGasGain                   mAmplification;//!
    StRichAnalogSignalGenerator*    mAnalogSignalGenerator;//!
    StRichAnalogToDigitalConverter  mADC;//!
    StRichNoiseSimulator            mNoiseSimulator;//!

    // Output Data
    StRichPadPlane                 *mPadPlane;//!
    StRichWriter                   *mWriter;//!

    // GEANT helper
    StParticleTable                *mTable;//!
    
    // I/O streams
    char*                        mInputFileName;//!
    char*                        mOutputFileName;//!
    int                          mNumberOfEvents;//!
    /* TO BE ADDED */
    /*StRrsIstream*                mInputStream;  */
    /*StRrsOstream*                mOutputStream; */
    
    //   Flags settable at macro level!
    int       mWriteToFile;
    int       mReadFromFile;

    int       mUseLocalCoordinate;
    int       mAddPedestal;
    int       mAddElectricNoise;
    
    virtual const char *GetCVS() const
    {static const char cvs[]= "Tag $Name:  $ $Id: StRrsMaker.h,v 2.2 2003/09/10 19:47:31 perev Exp $ built "__DATE__" "__TIME__ ; return cvs;}

    ClassDef(StRrsMaker,0)            // StAF chain
};
#endif
#endif // __ROOT__
