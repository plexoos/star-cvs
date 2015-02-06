#include <assert.h>
#include <sstream>
#include <string>

#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TVector3.h"

#include "StMessMgr.h"
#include "tables/St_ssdDimensions_Table.h"
#include "tables/St_ssdConfiguration_Table.h"
#include "tables/St_ssdWafersPosition_Table.h"

#include "StEvent/StEnumerations.h"
#include "Sti/Base/Factory.h"
#include "Sti/StiPlanarShape.h"
#include "Sti/StiCylindricalShape.h"
#include "Sti/StiMaterial.h"
#include "Sti/StiPlacement.h"
#include "Sti/StiDetector.h"
#include "Sti/StiToolkit.h"
#include "StiSsd/StiSstDetectorBuilder.h"
#include "Sti/StiNeverActiveFunctor.h"
#include "StiSsd/StiSsdIsActiveFunctor.h"
#include "StSsdUtil/StSstConsts.h"
#include "StDetectorDbMaker/StiSsdHitErrorCalculator.h"


/**
 * Builds an object to direct the construction of Sti detectors/volumes.
 *
 * \param active   Set to true when accounting for hits in active volumes or
 * false otherwise
 *
 * \param buildIdealGeom  Set to true (default) to ignore volume position
 * transformation stored in the survey DB tables
 */
StiSstDetectorBuilder::StiSstDetectorBuilder(bool active, bool buildIdealGeom)
   : StiDetectorBuilder("Ssd", active), mBuildIdealGeom(buildIdealGeom), mSstDb(0)
{
   setGroupId(kSsdId);
}


StiSstDetectorBuilder::~StiSstDetectorBuilder()
{}


/** Build the SST detector components. */
void StiSstDetectorBuilder::buildDetectors(StMaker &source)
{
   //StSsdBarrel *mySsd = StSsdBarrel::Instance();

   if (!gGeoManager)
      throw runtime_error("StiSstDetectorBuilder::buildDetectors() "
         "- Cannot build Sti geometry due to missing global object of TGeoManager class. "
         "Make sure STAR geometry is properly loaded with BFC AgML option");

   if (!mBuildIdealGeom) {

      if (!gStSstDbMaker) {
         LOG_ERROR << "StiSstDetectorBuilder::buildDetectors() - SST geometry was requested from "
            "DB but no StSstDb object found. Check for sstDb option in BFC chain" << endm;
         exit(EXIT_FAILURE);
      }

      mSstDb = (StSstDbMaker*) gStSstDbMaker;
      assert(mSstDb);

      LOG_INFO << "StiSstDetectorBuilder::buildDetectors() - Will build SST geometry from DB tables" << endm;
   }

   SetCurrentDetectorBuilder(this);

   // Gas material must be defined. Here we use air properties
   const TGeoMaterial* geoMat = gGeoManager->GetMaterial("AIR");

   _gasMat = geoMat ? add(new StiMaterial(geoMat->GetName(), geoMat->GetZ(), geoMat->GetA(), geoMat->GetDensity(), geoMat->GetRadLen()))
                    : add(new StiMaterial("AIR", 7.3, 14.61, 0.001205, 30420.));

   if (StiVMCToolKit::GetVMC()) {
      useVMCGeometry();
      buildInactiveVolumes();
   }
}


/**
 * Builds the sensors of the SST detector.
 */
void StiSstDetectorBuilder::useVMCGeometry()
{
   // Define silicon material used in manual construction of sensitive layers in this builder
   const TGeoMaterial* geoMat = gGeoManager->GetMaterial("SILICON");

   StiMaterial* silicon = geoMat ? add(new StiMaterial(geoMat->GetName(), geoMat->GetZ(), geoMat->GetA(), geoMat->GetDensity(), geoMat->GetRadLen()))
                                 : add(new StiMaterial("SILICON", 14, 28.0855, 2.33, 9.36) );

   // Build active sti volumes for SST sensors
   int stiRow = getNRows(); // Put all sensitive volumes in the same (and next available) Sti row
   // Use the "middle" sensor on the ladder to extract alignment corrections from DB
   int iSensor = floor(kSstNumSensorsPerLadder/2);

   for (int iLadder = 1; iLadder <= kSstNumLadders; ++iLadder)
   {
      std::ostringstream geoPath;
      geoPath << "/HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/SFMO_1/SFLM_" << iLadder << "/SFSW_" << iSensor << "/SFSL_1/SFSD_1";

      bool isAvail = gGeoManager->cd(geoPath.str().c_str());

      if (!isAvail) {
         LOG_WARN << "StiSstDetectorBuilder::useVMCGeometry() - Cannot find path to SFSD (SST sensitive) node. Skipping to next ladder..." << endm;
         continue;
      }

      TGeoVolume* sensorVol = gGeoManager->GetCurrentNode()->GetVolume();
      TGeoMatrix* sensorMatrix = 0;

      if (mBuildIdealGeom) {
         sensorMatrix = gGeoManager->MakePhysicalNode(geoPath.str().c_str())->GetMatrix();
      } else {
         sensorMatrix = (TGeoMatrix*) mSstDb->getHMatrixSensorOnGlobal(iLadder, iSensor);
      }

      if (!sensorMatrix) {
         LOG_WARN << "StiSstDetectorBuilder::useVMCGeometry() - Cannot get SST sensor position matrix. Skipping to next ladder..." << endm;
         continue;
      }

      TGeoBBox *sensorBBox = (TGeoBBox*) sensorVol->GetShape();

      // XXX:ds: Need to verify the constant for sensor spacing
      double sensorLength = kSstNumSensorsPerLadder * (sensorBBox->GetDZ() + 0.02); // halfDepth + 0.02 ~= (dead edge + sensor gap)/2
      StiShape *stiShape = new StiPlanarShape(geoPath.str().c_str(), sensorLength, 2*sensorBBox->GetDY(), sensorBBox->GetDX());

      StiPlacement *pPlacement= new StiPlacement(*sensorMatrix);

      // Build final detector object
      StiDetector *stiDetector = getDetectorFactory()->getInstance();
      StiIsActiveFunctor* isActive = _active ? new StiSsdIsActiveFunctor :
         static_cast<StiIsActiveFunctor*>(new StiNeverActiveFunctor);

      stiDetector->setProperties(geoPath.str(), isActive, stiShape, pPlacement, getGasMat(), silicon);
      stiDetector->setHitErrorCalculator(StiSsdHitErrorCalculator::instance());

      add(stiRow, iLadder-1, stiDetector);
   }
}


/**
 * Creates a crude model of the SST detector. The geometry is modeled with tubes
 * segmented in z and r. The dimensions and other physical properties of the
 * tube volumes are determined manually by looking at distribution of the
 * material in the ROOT TGeo geometry.
 *
 * We use the following dimensions and material properties:
 *
 * SFMO_CENTER_IN:   22.2 < r < 23.5 cm  -34.25 < z <  34.25 cm  Z = 7.38471  A = 14.7875  Dens = 0.1777280  X0 = 28128.1
 * SFMO_CENTER_MID:  23.5 < r < 25.8 cm  -34.25 < z <  34.25 cm  Z = 7.29364  A = 14.5971  Dens = 0.0147153  X0 = 30146.9
 * SFMO_CENTER_OUT:  25.8 < r < 27.0 cm  -34.25 < z <  34.25 cm  Z = 7.27831  A = 14.5655  Dens = 0.0372666  X0 = 29681.4
 *
 * SFMO_LEFT_IN:     22.2 < r < 23.5 cm  -51.50 < z < -34.25 cm  Z = 7.90551  A = 15.8598  Dens = 0.5131150  X0 = 21711.5
 * SFMO_LEFT_MID:    23.5 < r < 25.0 cm  -49.50 < z < -34.25 cm  Z = 7.67447  A = 15.3544  Dens = 0.3013760  X0 = 23510.2
 * SFMO_LEFT_OUT:    25.0 < r < 26.5 cm  -49.50 < z < -34.25 cm  Z = 7.52669  A = 15.0602  Dens = 0.2235500  X0 = 25904.3
 *
 * SFMO_RIGHT_IN:    22.2 < r < 23.5 cm   34.25 < z <  51.50 cm  Z = 7.92641  A = 15.9019  Dens = 0.5298270  X0 = 21402.6
 * SFMO_RIGHT_MID:   23.5 < r < 25.0 cm   34.25 < z <  49.50 cm  Z = 7.66531  A = 15.3361  Dens = 0.2953080  X0 = 23582.3
 * SFMO_RIGHT_OUT:   25.0 < r < 26.5 cm   34.25 < z <  49.50 cm  Z = 7.53069  A = 15.0682  Dens = 0.2273420  X0 = 25831.5
 *
 * The inner radius of the central tube is increased by 0.85 cm to avoid overlap
 * with sensitive layers, and the density of that volume is scaled accordingly.
 *
 * \author Dmitri Smirnov
 */
void StiSstDetectorBuilder::buildInactiveVolumes()
{
   // StiCylindricalShape(const string &name, float halfDepth_, float thickness_, float outerRadius_, float openingAngle_)
   StiCylindricalShape* sfmoCntrInnShape = new StiCylindricalShape("SFMO_CNTR_INN", fabs(34.25+34.25)/2., 23.5-23.2, 23.5, 2*M_PI);
   StiCylindricalShape* sfmoCntrMidShape = new StiCylindricalShape("SFMO_CNTR_MID", fabs(34.25+34.25)/2., 25.8-23.5, 25.8, 2*M_PI);
   StiCylindricalShape* sfmoCntrOutShape = new StiCylindricalShape("SFMO_CNTR_OUT", fabs(34.25+34.25)/2., 27.0-25.8, 27.0, 2*M_PI);

   StiCylindricalShape* sfmoLeftInnShape = new StiCylindricalShape("SFMO_LEFT_INN", fabs(51.50-34.25)/2., 23.5-22.2, 23.5, 2*M_PI);
   StiCylindricalShape* sfmoLeftMidShape = new StiCylindricalShape("SFMO_LEFT_MID", fabs(49.50-34.25)/2., 25.0-23.5, 25.0, 2*M_PI);
   StiCylindricalShape* sfmoLeftOutShape = new StiCylindricalShape("SFMO_LEFT_OUT", fabs(49.50-34.25)/2., 26.5-25.0, 26.5, 2*M_PI);

   StiCylindricalShape* sfmoRghtInnShape = new StiCylindricalShape("SFMO_RGHT_INN", fabs(51.50-34.25)/2., 23.5-22.2, 23.5, 2*M_PI);
   StiCylindricalShape* sfmoRghtMidShape = new StiCylindricalShape("SFMO_RGHT_MID", fabs(49.50-34.25)/2., 25.0-23.5, 25.0, 2*M_PI);
   StiCylindricalShape* sfmoRghtOutShape = new StiCylindricalShape("SFMO_RGHT_OUT", fabs(49.50-34.25)/2., 26.5-25.0, 26.5, 2*M_PI);

   // StiPlacement(float normRefAngle, float normRadius, float normYOffset, float centralZ, StiRegion region)
   StiPlacement* sfmoCntrInnPlacement = new StiPlacement(0, (23.5+23.2)/2., 0, (-34.25+34.25)/2.);
   StiPlacement* sfmoCntrMidPlacement = new StiPlacement(0, (25.8+23.5)/2., 0, (-34.25+34.25)/2.);
   StiPlacement* sfmoCntrOutPlacement = new StiPlacement(0, (27.0+25.8)/2., 0, (-34.25+34.25)/2.);

   StiPlacement* sfmoLeftInnPlacement = new StiPlacement(0, (23.5+22.2)/2., 0, (-51.50-34.25)/2.);
   StiPlacement* sfmoLeftMidPlacement = new StiPlacement(0, (25.0+23.5)/2., 0, (-49.50-34.25)/2.);
   StiPlacement* sfmoLeftOutPlacement = new StiPlacement(0, (26.5+25.0)/2., 0, (-49.50-34.25)/2.);

   StiPlacement* sfmoRghtInnPlacement = new StiPlacement(0, (23.5+22.2)/2., 0, (+51.50+34.25)/2.);
   StiPlacement* sfmoRghtMidPlacement = new StiPlacement(0, (25.0+23.5)/2., 0, (+49.50+34.25)/2.);
   StiPlacement* sfmoRghtOutPlacement = new StiPlacement(0, (26.5+25.0)/2., 0, (+49.50+34.25)/2.);

   // StiMaterial(const string &name, double z, double a, double density, double X0)
   StiMaterial* sfmoCntrInnMaterial = new StiMaterial("SFMO_CNTR_INN", 7.38471, 14.7875, 0.7536000, 28128.1);
   StiMaterial* sfmoCntrMidMaterial = new StiMaterial("SFMO_CNTR_MID", 7.29364, 14.5971, 0.0147153, 30146.9);
   StiMaterial* sfmoCntrOutMaterial = new StiMaterial("SFMO_CNTR_OUT", 7.27831, 14.5655, 0.0372666, 29681.4);

   StiMaterial* sfmoLeftInnMaterial = new StiMaterial("SFMO_LEFT_INN", 7.90551, 15.8598, 0.5131150, 21711.5);
   StiMaterial* sfmoLeftMidMaterial = new StiMaterial("SFMO_LEFT_MID", 7.67447, 15.3544, 0.3013760, 23510.2);
   StiMaterial* sfmoLeftOutMaterial = new StiMaterial("SFMO_LEFT_OUT", 7.52669, 15.0602, 0.2235500, 25904.3);

   StiMaterial* sfmoRghtInnMaterial = new StiMaterial("SFMO_RGHT_INN", 7.92641, 15.9019, 0.5298270, 21402.6);
   StiMaterial* sfmoRghtMidMaterial = new StiMaterial("SFMO_RGHT_MID", 7.66531, 15.3361, 0.2953080, 23582.3);
   StiMaterial* sfmoRghtOutMaterial = new StiMaterial("SFMO_RGHT_OUT", 7.53069, 15.0682, 0.2273420, 25831.5);

   // Define a prefix for detector name for consistency with automatic TGeo to Sti conversion
   std::string pfx("/HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/");

   StiDetector* stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"SFMO_CNTR_INN", new StiNeverActiveFunctor, sfmoCntrInnShape, sfmoCntrInnPlacement, getGasMat(), sfmoCntrInnMaterial);
   add(getNRows(), 0, stiDetector);

   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"SFMO_CNTR_MID", new StiNeverActiveFunctor, sfmoCntrMidShape, sfmoCntrMidPlacement, getGasMat(), sfmoCntrMidMaterial);
   add(getNRows(), 0, stiDetector);

   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"SFMO_CNTR_OUT", new StiNeverActiveFunctor, sfmoCntrOutShape, sfmoCntrOutPlacement, getGasMat(), sfmoCntrOutMaterial);
   add(getNRows(), 0, stiDetector);


   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"SFMO_LEFT_INN", new StiNeverActiveFunctor, sfmoLeftInnShape, sfmoLeftInnPlacement, getGasMat(), sfmoLeftInnMaterial);
   add(getNRows(), 0, stiDetector);

   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"SFMO_LEFT_MID", new StiNeverActiveFunctor, sfmoLeftMidShape, sfmoLeftMidPlacement, getGasMat(), sfmoLeftMidMaterial);
   add(getNRows(), 0, stiDetector);

   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"SFMO_LEFT_OUT", new StiNeverActiveFunctor, sfmoLeftOutShape, sfmoLeftOutPlacement, getGasMat(), sfmoLeftOutMaterial);
   add(getNRows(), 0, stiDetector);


   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"SFMO_RGHT_INN", new StiNeverActiveFunctor, sfmoRghtInnShape, sfmoRghtInnPlacement, getGasMat(), sfmoRghtInnMaterial);
   add(getNRows(), 0, stiDetector);

   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"SFMO_RGHT_MID", new StiNeverActiveFunctor, sfmoRghtMidShape, sfmoRghtMidPlacement, getGasMat(), sfmoRghtMidMaterial);
   add(getNRows(), 0, stiDetector);

   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"SFMO_RGHT_OUT", new StiNeverActiveFunctor, sfmoRghtOutShape, sfmoRghtOutPlacement, getGasMat(), sfmoRghtOutMaterial);
   add(getNRows(), 0, stiDetector);
}
