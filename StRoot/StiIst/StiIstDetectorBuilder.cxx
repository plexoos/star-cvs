#include <assert.h>
#include <sstream>
#include <string>

#include "TGeoVolume.h"
#include "TGeoMatrix.h"
#include "TVector3.h"

#include "StMessMgr.h"
#include "StThreeVectorD.hh"

#include "Sti/Base/Factory.h"
#include "Sti/StiPlanarShape.h"
#include "Sti/StiCylindricalShape.h"
#include "Sti/StiMaterial.h"
#include "Sti/StiPlacement.h"
#include "Sti/StiDetector.h"
#include "Sti/StiToolkit.h"
#include "Sti/StiIsActiveFunctor.h"
#include "Sti/StiNeverActiveFunctor.h"
#include "StiIst/StiIstIsActiveFunctor.h"
#include "StiIst/StiIstDetectorBuilder.h"
#include "tables/St_HitError_Table.h"
#include "StEvent/StEvent.h"
#include "StEvent/StEnumerations.h"
#include "StEvent/StEventTypes.h"
#include "StDetectorDbMaker/StiIst1HitErrorCalculator.h"
#include "StIstDbMaker/StIstDb.h"
#include "StIstUtil/StIstConsts.h"
#include "StBFChain/StBFChain.h"


using namespace StIstConsts;


StiIstDetectorBuilder::StiIstDetectorBuilder(bool active, bool buildIdealGeom) :
   StiDetectorBuilder("Ist", active), mBuildIdealGeom(buildIdealGeom), mIstDb(0)
{
   setGroupId(kIstId);
}


void StiIstDetectorBuilder::buildDetectors(StMaker &source)
{
   LOG_INFO << "StiIstDetectorBuilder::buildDetectors() -I- Started " << endm;

   SetCurrentDetectorBuilder(this);

   if (!mBuildIdealGeom) {
      TObjectSet *istDbDataSet = (TObjectSet *) source.GetDataSet("ist_db");

      if (!istDbDataSet) {
         LOG_ERROR << "StiIstDetectorBuilder::buildDetectors: IST geometry was requested from "
                   "DB but no StIstDb object found. Check for istDb option in BFC chain" << endm;
         exit(EXIT_FAILURE);
      }

      mIstDb = (StIstDb *) istDbDataSet->GetObject();
      assert(mIstDb);
      LOG_INFO << "StiIstDetectorBuilder::buildDetectors: Will build IST geometry from DB tables" << endm;
   }

   // Gas material must be defined. Here we use air properties
   const TGeoMaterial* geoMat = gGeoManager->GetMaterial("AIR");

   _gasMat = geoMat ? add(new StiMaterial(geoMat->GetName(), geoMat->GetZ(), geoMat->GetA(), geoMat->GetDensity(), geoMat->GetRadLen()))
                    : add(new StiMaterial("AIR", 7.3, 14.61, 0.001205, 30420.));

   if (StiVMCToolKit::GetVMC()) {
      useVMCGeometry();
      buildInactiveVolumes();
   }
}


void StiIstDetectorBuilder::useVMCGeometry()
{
   LOG_INFO << "StiIstDetectorBuilder::buildDetectors() -I- Use VMC geometry" << endm;

   // Define silicon material used in manual construction of sensitive layers in this builder
   const TGeoMaterial* geoMat = gGeoManager->GetMaterial("SILICON");

   StiMaterial* silicon = geoMat ? add(new StiMaterial(geoMat->GetName(), geoMat->GetZ(), geoMat->GetA(), geoMat->GetDensity(), geoMat->GetRadLen()))
                                 : add(new StiMaterial("SILICON", 14, 28.0855, 2.33, 9.36) );

   // Build active sti volumes for SST sensors
   int stiRow = getNRows(); // Put all sensitive volumes in the same (and next available) Sti row
   // Use the "middle" sensor on the ladder to extract alignment corrections from DB
   int iSensor = floor(kIstNumSensorsPerLadder/2);

   for (int iLadder = 1; iLadder <= kIstNumLadders; ++iLadder)
   {
      std::ostringstream geoPath;
      geoPath << "/HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1/IBAM_" << iLadder << "/IBLM_" << iSensor << "/IBSS_1";

      bool isAvail = gGeoManager->cd(geoPath.str().c_str());

      if (!isAvail) {
         Warning("useVMCGeometry()", "Cannot find path to IBSS (IST sensitive) node. Skipping to next ladder...");
         continue;
      }

      TGeoVolume* sensorVol = gGeoManager->GetCurrentNode()->GetVolume();
      TGeoMatrix* sensorMatrix = 0;

      if (mBuildIdealGeom) {
         sensorMatrix = gGeoManager->MakePhysicalNode(geoPath.str().c_str())->GetMatrix();
      } else {
         sensorMatrix = (TGeoMatrix*) mIstDb->getHMatrixSensorOnGlobal(iLadder, iSensor);
      }

      if (!sensorMatrix) {
         Warning("useVMCGeometry()", "Could not get IST sensor position matrix. Skipping to next ladder...");
         continue;
      }

      TGeoBBox *sensorBBox = (TGeoBBox*) sensorVol->GetShape();

      // Split the ladder in two halves
      for (int iLadderHalf = 1; iLadderHalf <= 2; iLadderHalf++)
      {
         // Create new Sti shape based on the sensor geometry
         std::string halfLadderName(geoPath.str() + (iLadderHalf == 1 ? "_HALF1" : "_HALF2") );

         // IBSS shape : DX =1.9008cm ; DY = .015cm ; DZ = 3.765 cm
         double sensorLength = kIstNumSensorsPerLadder * (sensorBBox->GetDZ() + 0.10); // halfDepth + deadedge 0.16/2 + sensor gap 0.04/2
         StiShape *stiShape = new StiPlanarShape(halfLadderName.c_str(), sensorLength, 2*sensorBBox->GetDY(), sensorBBox->GetDX()/2);

         TVector3 offset((iLadderHalf == 1 ? -sensorBBox->GetDX()/2 : sensorBBox->GetDX()/2), 0, 0);
         StiPlacement *pPlacement= new StiPlacement(*sensorMatrix, offset);

         // Build final detector object
         StiDetector *stiDetector = getDetectorFactory()->getInstance();
         StiIsActiveFunctor* isActive = _active ?  new StiIstIsActiveFunctor :
            static_cast<StiIsActiveFunctor*>(new StiNeverActiveFunctor);

         stiDetector->setProperties(halfLadderName, isActive, stiShape, pPlacement, getGasMat(), silicon);
         stiDetector->setHitErrorCalculator(StiIst1HitErrorCalculator::instance());

         // Add created sensitive IST layer to Sti
         add(iLadderHalf-1, iLadder-1, stiDetector);
      }
   }
}


/**
 * Returns the active StiDetector corresponding to a sensitive layer in IST. An
 * active volume can have hits associated with it. The ladder id is expected to
 * follow the human friendly numbering scheme, i.e.
 *
 * 1 <= ladder <= kIstNumLadders
 *
 * In this builder the active IST layers are added in stiRow = 0.
 */
const StiDetector* StiIstDetectorBuilder::getActiveDetector(int ladder) const
{
   return (ladder < 1 || ladder > kIstNumLadders) ? 0 : getDetector(0, ladder-1);
}


/**
 * Creates a crude approximation of the IST detector. The geometry is modeled
 * with a single tube using the dimensions and other physical properties of the
 * IST mother volume defined in the ROOT TGeo geometry.
 */
void StiIstDetectorBuilder::buildInactiveVolumes()
{
   // Prepare shapes for each ladder
   StiShape *digiBoardShape = new StiPlanarShape("IBAM_DIGI_BOARD",  5.830, 2*0.0823, 1.9355);
   StiShape *connectorShape = new StiPlanarShape("IBAM_CONNECTOR",   1.125, 2*0.3050, 3.0500);
   StiShape *cfBackingShape = new StiPlanarShape("IBAM_CF_BACKING", 27.800, 2*0.3050, 3.0500); // Carbon fiber backing

   // StiMaterial(const string &name, double z, double a, double density, double X0)
   StiMaterial* digiBoardMaterial     = new StiMaterial("IBAM_DIGI_BOARD",      9.01, 18.01,      1.70,   18.2086);
   StiMaterial* alumConnectorMaterial = new StiMaterial("IBAM_ALUM_CONNECTOR", 13.00, 26.98, 1.05*2.70,    8.8751);
   StiMaterial* gtenConnectorMaterial = new StiMaterial("IBAM_GTEN_CONNECTOR",  9.01, 18.02, 1.10*1.70,   18.2086);
   StiMaterial* cfBackingMaterial     = new StiMaterial("IBAM_CF_BACKING",      6.34, 12.71,      0.19, 1096.0000);

   // Volumes offsets
   TVector3 digiBoardOffset    (2.11591, -2.087862, -25.4911);
   TVector3 alumConnectorOffset(0.47779, -0.352065, -29.3221);
   TVector3 gtenConnectorOffset(0.47779, -0.352065,  28.8080);
   TVector3 cfBackingOffset    (0.47779, -0.352065,  -0.5141);

   // We use position of sensitive IST volumes to place new inactive volumes
   int stiRow = getNRows(); // Put volumes in the same (and next available) Sti row
   // Use the "middle" sensor on the ladder to extract alignment corrections from DB
   int iSensor = floor(kIstNumSensorsPerLadder/2);

   for (int iLadder = 1; iLadder <= kIstNumLadders; ++iLadder)
   {
      std::ostringstream geoPath, ssPfx;
      geoPath << "/HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1/IBAM_" << iLadder << "/IBLM_" << iSensor << "/IBSS_1";
      ssPfx   << "/HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1/IBAM_" << iLadder << "/";

      // Save first part of geoPath to reuse in new detector names
      std::string pfx(ssPfx.str());

      bool isAvail = gGeoManager->cd(geoPath.str().c_str());

      if (!isAvail) {
         Warning("useVMCGeometry()", "Cannot find path to IBSS (IST sensitive) node. Skipping to next ladder...");
         continue;
      }

      TGeoMatrix* transMatrix = 0;

      if (mBuildIdealGeom) {
         transMatrix = gGeoManager->MakePhysicalNode(geoPath.str().c_str())->GetMatrix();
      } else {
         transMatrix = (TGeoMatrix*) mIstDb->getHMatrixSensorOnGlobal(iLadder, iSensor);
      }

      if (!transMatrix) {
         Warning("useVMCGeometry()", "Could not get IST sensor position matrix. Skipping to next ladder...");
         continue;
      }

      StiPlacement *cfBackingPlacement = new StiPlacement(*transMatrix, cfBackingOffset);
      StiPlacement *alumConnectorPlacement = new StiPlacement(*transMatrix, alumConnectorOffset);
      StiPlacement *gtenConnectorPlacement = new StiPlacement(*transMatrix, gtenConnectorOffset);
      StiPlacement *digiBoardPlacement = new StiPlacement(*transMatrix, digiBoardOffset);

      StiDetector *stiDetector = getDetectorFactory()->getInstance();
      stiDetector->setProperties(pfx+"IBAM_CF_BACKING", new StiNeverActiveFunctor, cfBackingShape, cfBackingPlacement, getGasMat(), cfBackingMaterial);
      add(stiRow, iLadder-1, stiDetector);

      stiDetector = getDetectorFactory()->getInstance();
      stiDetector->setProperties(pfx+"IBAM_ALUM_CONNECTOR", new StiNeverActiveFunctor, connectorShape, alumConnectorPlacement, getGasMat(), alumConnectorMaterial);
      add(stiRow+1, iLadder-1, stiDetector);

      stiDetector = getDetectorFactory()->getInstance();
      stiDetector->setProperties(pfx+"IBAM_GTEN_CONNECTOR", new StiNeverActiveFunctor, connectorShape, gtenConnectorPlacement, getGasMat(), gtenConnectorMaterial);
      add(stiRow+2, iLadder-1, stiDetector);

      stiDetector = getDetectorFactory()->getInstance();
      stiDetector->setProperties(pfx+"IBAM_DIGI_BOARD", new StiNeverActiveFunctor, digiBoardShape, digiBoardPlacement, getGasMat(), digiBoardMaterial);
      add(stiRow+3, iLadder-1, stiDetector);
   }

   std::string pfx("/HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1/");

   // Implement plastic brackets as a thin cylinder on east and west sides
   StiCylindricalShape* ibamBracketShape = new StiCylindricalShape("IBAM_BRACKET", 0.635, 0.2, 12, 2*M_PI);
   StiPlacement* ibamBracketEastPlacement = new StiPlacement(0, 11.9, 0,  29.34);
   StiPlacement* ibamBracketWestPlacement = new StiPlacement(0, 11.9, 0, -29.6);
   StiMaterial* ibamBracketMaterial = new StiMaterial("IBAM_BRACKET", 6.089, 12.149, 24*0.2601, 160);

   StiDetector* stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"IBAM_BRACKET_EAST", new StiNeverActiveFunctor, ibamBracketShape, ibamBracketEastPlacement, getGasMat(), ibamBracketMaterial);
   add(getNRows(), 0, stiDetector);

   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"IBAM_BRACKET_WEST", new StiNeverActiveFunctor, ibamBracketShape, ibamBracketWestPlacement, getGasMat(), ibamBracketMaterial);
   add(getNRows(), 0, stiDetector);

   // Implement cooling line and cablings in transition area
   StiCylindricalShape* icctShape = new StiCylindricalShape("ICCT", 4.35720/2., 22.384-19.115, 22.384, 2*M_PI);
   StiPlacement* icctPlacement = new StiPlacement(0, (22.384+19.115)/2., 0, -57.4);
   StiMaterial* icctMaterial = new StiMaterial("ICCT", 26.8, 56.9, 0.673, 17.9);

   stiDetector = getDetectorFactory()->getInstance();
   stiDetector->setProperties(pfx+"ICCT", new StiNeverActiveFunctor, icctShape, icctPlacement, getGasMat(), icctMaterial);
   add(getNRows(), 0, stiDetector);
}


StiPlacement* StiIstDetectorBuilder::createPlacement(const TGeoMatrix& transMatrix, const TVector3& localCenterOffset, const TVector3& normal)
{
   // Convert center of the geobox to coordinates in the global coordinate system
   double sensorXyzLocal[3]  = {localCenterOffset.X(), localCenterOffset.Y(), localCenterOffset.Z()};
   double sensorXyzGlobal[3] = {};

   double normalXyzLocal[3]  = {normal.X() + localCenterOffset.X(), normal.Y() + localCenterOffset.Y(), normal.Z() + localCenterOffset.Z()};
   double normalXyzGlobal[3] = {};

   transMatrix.LocalToMaster(sensorXyzLocal, sensorXyzGlobal);
   transMatrix.LocalToMaster(normalXyzLocal, normalXyzGlobal);

   TVector3 centralVec(sensorXyzGlobal);
   TVector3 normalVec(normalXyzGlobal);

   normalVec -= centralVec;

   if (normalVec.Dot(centralVec) < 0) normalVec *= -1;

   double deltaPhi = centralVec.DeltaPhi(normalVec);
   double normalVecMag = fabs(centralVec.Perp() * cos(deltaPhi));

   return new StiPlacement(normalVec.Phi(), normalVecMag, centralVec.Perp()*sin(deltaPhi), localCenterOffset.Z());
}
