#include <stdio.h>
#include <map>
#include <exception>
#include <stdexcept>

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
#include "TDataSetIter.h"
#include "THashList.h"
#include "TString.h"
#include "tables/St_HitError_Table.h"
#include "StEvent/StEvent.h"
#include "StEvent/StEnumerations.h"
#include "StEvent/StEventTypes.h"
#include "StDetectorDbMaker/StiIst1HitErrorCalculator.h"
#include "StIstDbMaker/StIstDb.h"
#include "StIstUtil/StIstConsts.h"
#include "StBFChain/StBFChain.h"


void testPlanerVolume(StiDetector *det, const char* detName="", float halfDepth=0, float thickness=0, float halfWidth=0
                        ,float yShift=0, float rShift=0, float zShift=0, StiPlacement *placement=0)
{
static TString myName;
static double wasVolu = 0,nowVolu=0;

if (det && det!=(StiDetector*)(-1)) { 
  StiShape *stiShape= det->getShape();
  myName = det->getName().c_str();
  double wasDepth = stiShape->getHalfDepth();
  double wasThick = stiShape->getThickness();
  double wasWidth = stiShape->getHalfWidth();
  wasVolu = wasDepth*wasThick*wasWidth*4;
  nowVolu=0;
}
nowVolu += halfDepth*thickness*halfWidth*4;
if (det!=(StiDetector*)(-1)) return; 
  double fakt = nowVolu/wasVolu;
  if (fabs(fakt-1)<0.01) return;
  printf("***** %s => %s fakt =%g %g %g\n",myName.Data(),detName,fakt,wasVolu,nowVolu);

}


using namespace std;
using namespace StIstConsts;


StiIstDetectorBuilder::StiIstDetectorBuilder(bool active, bool buildIdealGeom) :
   StiDetectorBuilder("Ist", active), mSiMaterial(0), mHybridMaterial(0), mBuildIdealGeom(buildIdealGeom), mIstDb(0)
{ }


void StiIstDetectorBuilder::buildDetectors(StMaker &source)
{
   LOG_INFO << "StiIstDetectorBuilder::buildDetectors() -I- Started " << endm;

   setNRows(1);

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
   _gasMat = add(new StiMaterial("PixelAir", 7.3, 14.61, 0.001205, 30420.*0.001205, 7.3 * 12.e-9));

   if (StiVMCToolKit::GetVMC()) {
      useVMCGeometry();
      buildInactiveVolumes();
   }
}


void StiIstDetectorBuilder::useVMCGeometry()
{
   cout << "StiIstDetectorBuilder::buildDetectors() -I- Use VMC geometry" << endl;

   unsigned int ROW = 1;

   THashList *istRot = 0;

   if (!mBuildIdealGeom)
      istRot = mIstDb->getRotations();

   // Build the material map
   struct Material_t {
      const Char_t *name;
      StiMaterial    **p;
   };

   Material_t map[] = {
      {"AIR",     &_gasMat},
      {"SILICON", &mSiMaterial},
      {"SILICON", &mHybridMaterial}
   };

   Int_t M = sizeof(map) / sizeof(Material_t);

   for (Int_t i = 0; i < M; i++) {
      const TGeoMaterial *mat =  gGeoManager->GetMaterial(map[i].name);

      if (! mat) continue;

      Double_t PotI = StiVMCToolKit::GetPotI(mat);
      *map[i].p = add(new StiMaterial(mat->GetName(),
                                      mat->GetZ(),
                                      mat->GetA(),
                                      mat->GetDensity(),
                                      mat->GetDensity()*mat->GetRadLen(),
                                      PotI));
   }

   int iSensor = 3;

   for (int iLadder = 1; iLadder < kIstNumLadders; ++iLadder)
   {
      unsigned int matIst = 1000 + (iLadder) * kIstNumSensorsPerLadder + iSensor;
      LOG_DEBUG << "iLadder/iSensor/matIst : " << iLadder << " " << iSensor << " " << matIst << endm;

      char name[50];
      sprintf(name, "Ist/Ladder_%d/Sensor_%d", iLadder, iSensor);

      TString Path("HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1");
      Path += Form("/IBAM_%d/IBLM_%d/IBSS_1", iLadder, iSensor);
      gGeoManager->cd(Path); // retrieve info of IBSS volume

      TGeoHMatrix *combI = 0;

      if (!mBuildIdealGeom)
         combI = (TGeoHMatrix *) istRot->FindObject(Form("R%04i", matIst));
      else
         combI = gGeoManager->MakePhysicalNode(Path.Data())->GetMatrix();

      if (combI) {
         combI->Print();
      } else {
         Error("useVMCGeometry()", "Could not find TGeoHMatrix for sensor %d in database", matIst);
         continue;
      }

      TGeoNode *nodeT = gGeoManager->GetCurrentNode();
      // Extract volume geometry for this node
      TGeoBBox *box = (TGeoBBox *) nodeT->GetVolume()->GetShape();
      LOG_DEBUG << " DZ/DY/DX : " << box->GetDZ()
                << " " << box->GetDY()
                << " " << box->GetDX()
                << " " << endm;

      //IBSS shape : DX =1.9008cm ; DY = .015cm ; DZ = 3.765 cm
      StiShape *sh  = new StiPlanarShape(name,
                                         kIstNumSensorsPerLadder * (box->GetDZ() + 0.10), // halfDepth + deadedge 0.16/2 + sensor gap 0.04/2
                                         2 * box->GetDY(),              // thickness
                                         box->GetDX());                 // halfWidth
      add(sh);

      Double_t     *xyz    = combI->GetTranslation();
      Double_t     *rot    = combI->GetRotationMatrix();
      StThreeVectorD centerVector(xyz[0], xyz[1], xyz[2]);
      StThreeVectorD normalVector(rot[1], rot[4], rot[7]);

      Double_t prod = centerVector * normalVector;

      if (prod < 0) normalVector *= -1;

      // Normalize normal vector, just in case....
      normalVector /= normalVector.magnitude();

      // Volume positioning
      StiPlacement *pPlacement = new StiPlacement;
      Double_t phi  = centerVector.phi();
      Double_t phiD = normalVector.phi();
      Double_t r    = centerVector.perp();
      pPlacement->setZcenter(0);
      pPlacement->setLayerRadius(r);

      pPlacement->setLayerAngle(phi);
      pPlacement->setRegion(StiPlacement::kMidRapidity);
      pPlacement->setNormalRep(phiD, r * TMath::Cos(phi - phiD), r * TMath::Sin(phi - phiD));
      assert(pPlacement);

      //Build final detector object
      StiDetector *p = getDetectorFactory()->getInstance();

      if ( !p ) {
         LOG_INFO << "StiIstDetectorBuilder::AverageVolume() -E- StiDetector pointer invalid." << endm;
         return;
      }

      p->setName(name);
      p->setIsOn(kTRUE);
      if (_active) {  p->setIsActive(new StiIstIsActiveFunctor);}
      else         {  p->setIsActive(new StiNeverActiveFunctor);}
      p->setIsContinuousMedium(false);
      p->setIsDiscreteScatterer(true);
      p->setShape(sh);
      p->setPlacement(pPlacement);
      p->setGas(GetCurrentDetectorBuilder()->getGasMat());

      if (!p->getGas()) LOG_INFO << "gas not there!" << endm;

      p->setMaterial(mSiMaterial);
      p->setHitErrorCalculator(StiIst1HitErrorCalculator::instance());

      // Adding detector, note that no keys are set in IST!
      add(ROW, iLadder, p);

      // Whole bunch of debugging information
      Float_t rad2deg = 180.0 / 3.1415927;
      LOG_DEBUG << "===>NEW:IST:pDetector:Name               = " << p->getName()                               << endm
                << "===>NEW:IST:pPlacement:NormalRefAngle    = " << pPlacement->getNormalRefAngle()*rad2deg    << endm
                << "===>NEW:IST:pPlacement:NormalRadius      = " << pPlacement->getNormalRadius()              << endm
                << "===>NEW:IST:pPlacement:NormalYoffset     = " << pPlacement->getNormalYoffset()             << endm
                << "===>NEW:IST:pPlacement:CenterRefAngle    = " << pPlacement->getCenterRefAngle()*rad2deg    << endm
                << "===>NEW:IST:pPlacement:CenterRadius      = " << pPlacement->getCenterRadius()              << endm
                << "===>NEW:IST:pPlacement:CenterOrientation = " << pPlacement->getCenterOrientation()*rad2deg << endm
                << "===>NEW:IST:pPlacement:LayerRadius       = " << pPlacement->getLayerRadius()               << endm
                << "===>NEW:IST:pPlacement:LayerAngle        = " << pPlacement->getLayerAngle()*rad2deg        << endm
                << "===>NEW:IST:pPlacement:Zcenter           = " << pPlacement->getZcenter()                   << endm
                << "===>NEW:IST:pDetector:Ladder             = " << iLadder                                    << endm
                << "===>NEW:IST:pDetector:Sensor             = " << iSensor                                    << endm
                << "===>NEW:IST:pDetector:row/ladder (ITTF)  = " << ROW << " / " << iLadder                    << endm
                << "===>NEW:IST:pDetector:Active?            = " << p->isActive()                              << endm;
   }
}


/**
 * Creates a crude approximation of the IST detector. The geometry is modeled
 * with a single tube using the dimensions and other physical properties of the
 * IST mother volume defined in the ROOT TGeo geometry.
 */
void StiIstDetectorBuilder::buildInactiveVolumes()
{
   // Build average inactive volumes
   const VolumeMap_t volumes[] = {
      {"IHTC", "Top Kapton hybrid east box volume",                   "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //kapton hybrid
      {"IHTH", "Top Kapton hybrid west box volume",                   "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"IHBC", "Bottom Kapton hybrid east volume",                    "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"IHBH", "Bottom Kapton hybrid west volume",                    "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      //{"IBHC", "North Kapton hybrid edge tub volume",               "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1","",""},
      {"ICFC", "carbon foam east box volume",                         "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //carbon foam
      {"ICFD", "carbon foam west box volume",                         "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ICCU", "CFRPMix honeycomb east box volume",                   "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //carbon honeycomb
      {"ICCD", "CFRPMix honeycomb west box volume",                   "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ISTC", "top carbon fiber skin east box volume",               "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //top carbon skin
      {"ISTH", "top carbon fiber skin west box volume",               "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ISBC", "bottom carbon fiber skin east box volume",            "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //bottom carbon skin
      {"ISBH", "bottom carbon fiber skin west box volume",            "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"IECE", "east aluminum end-cap box volume",                    "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //east AL end-cap
      {"IECW", "west PEEK CF 30 end-cap box volume",                  "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //west carbon end-cap
      {"ICTC", "middle aluminum cooling tube volume",                 "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //middle cooling tube
      {"ICTE", "middle aluminum cooling tube east cone volume",       "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ICTW", "middle aluminum cooling tube west cone volume",       "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"IBCW", "middle Novec 7200 cooling liquid volume",             "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //liquid coolant
      {"IBRB", "G10 T-board box volume",                              "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //T-Board
      {"IRSA", "Delrin T-board slot A box volume",                    "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //connectors
      {"IRSB", "Delrin T-board slot B box volume",                    "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"IRSC", "Delrin T-board slot C box volume",                    "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"IBTS", "Slicon thermal sensor box volume",                    "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //thermal sensor
      {"IBAP", "inactive silicon volume APV chips",                   "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //APV chips
      {"ISCA", "east short cable A volume",                           "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //east short cables
      {"ISCB", "east short cable B volume",                           "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ISCC", "east short cable C volume",                           "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ICLE", "east short tube liquid volume",                       "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //east liquid coolant
      {"ICTJ", "east short cooling tube joint volume",                "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //east cooling tube joint volume
      {"ICTA", "east short cooling tube connector volume",            "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //east cooling tube connector volume
      {"ICTB", "east short cooling tube right volume",                "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //east cooling tube right volume
      {"ICJS", "west cooling loop left tube volume",                  "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ICJA", "west cooling loop left cone volume",                  "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ICJU", "west cooling loop left connector volume",             "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ICLN", "west cooling loop right liquid volume",               "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ISRR", "PEEK CF 30 support block south side sub-volume",      "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""}, //support block
      {"ISRB", "PEEK CF 30 support block north side sub-volume",      "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ISRL", "PEEK CF 30 support block trapezoid sub-volume volume","HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ISRO", "PEEK CF 30 support block top-right volume",           "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ISRC", "PEEK CF 30 support block trapezoid volume",           "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ISRS", "PEEK CF 30 support block small box volume",           "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1/IBMO_1", "", ""},
      {"ICCT", "Cooling line and cablings in transition area",        "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1"       , "", ""},
      {"ICCA", "Copper cablings on PIT",                              "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1"       , "", ""},
      {"ICCL", "Al cooling lines on PIT",                             "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1"       , "", ""},
      {"ICLP", "Liquid coolant on PIT",                               "HALL_1/CAVE_1/TpcRefSys_1/IDSM_1"       , "", ""}
   };

   // Build the volume map and loop over all found volumes
   Int_t nVolumes = sizeof(volumes) / sizeof(VolumeMap_t);
   gGeoManager->RestoreMasterVolume();
   gGeoManager->CdTop();

   // material for support block inner thin tube volume (ISRA)
   StiMaterial *matISRA = NULL;
   double gWt,sWt,gWtTot=0,sWtTot=0,dWtTot=0,sWtYF=0;

   for (Int_t i = 0; i < nVolumes; i++) {

      if ( !gGeoManager->cd(volumes[i].path) ) {
         Warning("buildInactiveVolumes()", "Cannot find path to %s node. Skipping to next node...", volumes[i].name);
         continue;
      }

      TGeoNode *geoNode = gGeoManager->GetCurrentNode();

      if (!geoNode) continue;

      LOG_DEBUG << "Current node : " << i << "/" << nVolumes << " path is : " << volumes[i].name << endm;
      LOG_DEBUG << "Number of daughters : " << geoNode->GetNdaughters() << " weight : " << geoNode->GetVolume()->Weight() << endm;

      TGeoPhysicalNode *gnode = StiVMCToolKit::LoopOverNodes(geoNode, volumes[i].path, volumes[i].name, MakeAverageVolume);
      if (gnode){};
      TGeoVolume *gvolu = gGeoManager->FindVolumeFast(volumes[i].name);
      assert(gvolu);
//      gWt = gvolu->Weight(0.01,"a"); gWtTot+=gWt; sWt = 0;
      // Access last added volume
      int row = getNRows() - 1;
      int sector = 0;

      // Make Sti detector active, i.e. use it in tracking
      StiDetector *stiDetector = getDetector(row, sector);
//      sWtYF = stiDetector->getWeight();
      
      // Retrieve material, placement, energy loss information of carbon foam stave for re-definitions.
      // The loaded carbon foam volumes (ICFC and ICFD) are disabled in tracking and removed by zero volume.
      // The new carbon foam stave will consist of three simple box volumes to avoid overlap when place
      // cooling tube and coolant. This method have been applied to IST east and west end-caps.
      // Detailed information can be found at:
      // https://drupal.star.bnl.gov/STAR/blog/ypwang/ist-sti-geometry
      TString ts,dir;int idx;
      if (string(volumes[i].name) == string("ICFC")) {
         int startRow = getNRows() - kIstNumLadders;

         for (int iICFC = 0; iICFC < kIstNumLadders; iICFC++) {
            row = startRow + iICFC;
            stiDetector = getDetector(row, sector);
            assert(strstr(stiDetector->getName().c_str(),"/ICFC_"));
//		prepare "directory"
            dir = stiDetector->getName().c_str();idx = dir.Last('/'); assert(idx>=0); dir.Remove(idx+1,999);

            StiMaterial *matICFC                    = stiDetector->getMaterial();
            StiPlacement *stiPlacementICFC          = stiDetector->getPlacement();

            //construct carbon foam stave north side volume
            StiDetector *stiDetectorN = getDetectorFactory()->getInstance();
            ts = dir; ts +="ICFCn";
            buildPlanerVolume(*stiDetectorN, ts.Data(), 8.825 * 0.5, 0.5663, 1.25 * 0.5, 0.625, 0., 0., stiPlacementICFC, matICFC);
             testPlanerVolume(stiDetector,   ts.Data(), 8.825 * 0.5, 0.5663, 1.25 * 0.5, 0.625, 0., 0., stiPlacementICFC);
            del(row, sector);
            add(row, sector, stiDetectorN);
//            sWt+=stiDetectorN->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for ICFC north side " << stiDetectorN->getName() << " at layer " << row << endm;

            //construct carbon foam stave bottom side volume
            StiDetector *stiDetectorB = getDetectorFactory()->getInstance();
            ts = dir; ts +="ICFCb.";
            buildPlanerVolume(*stiDetectorB, ts.Data(), 8.825 * 0.5, 0.042775, 0.47625 * 0.5, -0.238125, -0.2617625, 0., stiPlacementICFC, matICFC);
             testPlanerVolume(0,             ts.Data(), 8.825 * 0.5, 0.042775, 0.47625 * 0.5, -0.238125, -0.2617625, 0., stiPlacementICFC );
            int layer = getNRows();
            add(layer, sector, stiDetectorB);
//            sWt+=stiDetectorB->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for ICFC bottom side " << stiDetectorB->getName() << " at layer " << layer << endm;

            //construct carbon foam stave south side volume
            StiDetector *stiDetectorS = getDetectorFactory()->getInstance();
            ts = dir;; ts += "ICFCs";
            buildPlanerVolume(*stiDetectorS, ts.Data(), 8.825 * 0.5, 0.5663, 0.77375 * 0.5, -0.863125, 0., 0., stiPlacementICFC, matICFC);
            testPlanerVolume((StiDetector*)(-1),    ts.Data(), 8.825 * 0.5, 0.5663, 0.77375 * 0.5, -0.863125, 0., 0., stiPlacementICFC);
            layer = getNRows();
            add(layer, sector, stiDetectorS);
//            sWt+=stiDetectorS->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for ICFC south side " << stiDetectorS->getName() << " at layer " << layer << endm;

            matICFC                 = NULL;
            stiPlacementICFC        = NULL;
            stiDetector             = NULL;
         }

      }

      if (string(volumes[i].name) == string("ICFD")) {
         int startRow = getNRows() - kIstNumLadders;

         for (int iICFD = 0; iICFD < kIstNumLadders; iICFD++) {
            row = startRow + iICFD;
            stiDetector = getDetector(row, sector);
            assert(strstr(stiDetector->getName().c_str(),"/ICFD_"));
//		prepare "directory"
            dir = stiDetector->getName().c_str();idx = dir.Last('/'); assert(idx>=0); dir.Remove(idx+1,999);

            StiMaterial *matICFD                    = stiDetector->getMaterial();
            StiPlacement *stiPlacementICFD          = stiDetector->getPlacement();

            //construct carbon foam north side volume
            StiDetector *stiDetectorN = getDetectorFactory()->getInstance();
            ts = dir; ts+="ICFDn"; 
            buildPlanerVolume(*stiDetectorN, ts.Data(), 47.055 * 0.5, 0.58, 1.25 * 0.5, 0.625, 0., 0., stiPlacementICFD, matICFD);
             testPlanerVolume( stiDetector,  ts.Data(), 47.055 * 0.5, 0.58, 1.25 * 0.5, 0.625, 0., 0., stiPlacementICFD);
            del(row, sector);
            add(row, sector, stiDetectorN);
//            sWt+=stiDetectorN->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for ICFD north side " << stiDetectorN->getName() << " at layer " << row << endm;

            //construct carbon foam bottom side volume
            StiDetector *stiDetectorB = getDetectorFactory()->getInstance();
            ts = dir; ts+="ICFDb";
            buildPlanerVolume(*stiDetectorB, ts.Data(), 47.055 * 0.5, 0.049675, 0.47625 * 0.5, -0.238125, -0.2651625, 0., stiPlacementICFD, matICFD);
             testPlanerVolume( 0          ,  ts.Data(), 47.055 * 0.5, 0.049675, 0.47625 * 0.5, -0.238125, -0.2651625, 0., stiPlacementICFD);
            int layer = getNRows();
            add(layer, sector, stiDetectorB);
//            sWt+=stiDetectorB->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for ICFD bottom side " << stiDetectorB->getName() << " at layer " << layer << endm;

            //construct carbon foam south side volume
            StiDetector *stiDetectorS = getDetectorFactory()->getInstance();
            ts=dir;ts+="ICFDs"; 
            buildPlanerVolume(*stiDetectorS, ts.Data(), 47.055 * 0.5, 0.58, 0.77375 * 0.5, -0.863125, 0., 0., stiPlacementICFD, matICFD);
             testPlanerVolume( (StiDetector*)(-1),  ts.Data(), 47.055 * 0.5, 0.58, 0.77375 * 0.5, -0.863125, 0., 0., stiPlacementICFD);
            layer = getNRows();
            add(layer, sector, stiDetectorS);
//            sWt+=stiDetectorS->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for ICFD south side " << stiDetectorS->getName() << " at layer " << layer << endm;

            matICFD                 = NULL;
            stiPlacementICFD        = NULL;
            stiDetector             = NULL;
         }
      }

      // Retrieve info. of east aluminum end-cap volume
      if (string(volumes[i].name) == string("IECE")) {
         int startRow = getNRows() - kIstNumLadders;

         for (int iIECE = 0; iIECE < kIstNumLadders; iIECE++) {
            row = startRow + iIECE;
            stiDetector = getDetector(row, sector);
            assert(strstr(stiDetector->getName().c_str(),"/IECE_"));
//		prepare "directory"
            dir = stiDetector->getName().c_str();idx = dir.Last('/'); assert(idx>=0); dir.Remove(idx+1,999);


            StiMaterial *matIECE                    = stiDetector->getMaterial();
            StiPlacement *stiPlacementIECE          = stiDetector->getPlacement();

            //construct east end-cap north side volume
            StiDetector *stiDetectorN = getDetectorFactory()->getInstance();
            ts = dir; ts +="IECEn";
            buildPlanerVolume(*stiDetectorN, ts.Data(), 2.25 * 0.5, 0.5413, 1.23485 * 0.5, 2.4326, 0., 0., stiPlacementIECE, matIECE);
             testPlanerVolume( stiDetector,  ts.Data(), 2.25 * 0.5, 0.5413, 1.23485 * 0.5, 2.4326, 0., 0., stiPlacementIECE);
            del(row, sector);
            add(row, sector, stiDetectorN);
//            sWt+=stiDetectorN->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for IECE north side " << stiDetectorN->getName() << " at layer " << row << endm;

            //construct east end-cap bottom side volume
            StiDetector *stiDetectorB = getDetectorFactory()->getInstance();
            ts = dir; ts +="IECEb";
            buildPlanerVolume(*stiDetectorB, ts.Data(), 2.25 * 0.5, 0.0193, 0.5065 * 0.5, 1.5619, -0.261, 0., stiPlacementIECE, matIECE);
             testPlanerVolume( 0          ,  ts.Data(), 2.25 * 0.5, 0.0193, 0.5065 * 0.5, 1.5619, -0.261, 0., stiPlacementIECE);
            int layer = getNRows();
            add(layer, sector, stiDetectorB);
//            sWt+=stiDetectorB->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for IECE bottom side " << stiDetectorB->getName() << " at layer " << layer << endm;

            //construct east end-cap south side volume
            StiDetector *stiDetectorS = getDetectorFactory()->getInstance();
            ts = dir; ts +="IECEs"; //ts+=iIECE;
            buildPlanerVolume(*stiDetectorS, ts.Data(), 2.25 * 0.5, 0.5413, 4.35865 * 0.5, -0.870675, 0., 0., stiPlacementIECE, matIECE);
             testPlanerVolume((StiDetector*)(-1),  ts.Data(), 2.25 * 0.5, 0.5413, 4.35865 * 0.5, -0.870675, 0., 0., stiPlacementIECE);
            layer = getNRows();
            add(layer, sector, stiDetectorS);
//            sWt+=stiDetectorS->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for IECE south side " << stiDetectorS->getName() << " at layer " << layer << endm;

            matIECE                 = NULL;
            stiPlacementIECE        = NULL;
            stiDetector             = NULL;
         }
      }

      // Modify dimensions of west carbon end-cap volume
      if (string(volumes[i].name) == string("IECW")) {
         int startRow = getNRows() - kIstNumLadders;

         for (int iIECW = 0; iIECW < kIstNumLadders; iIECW++) {
            row = startRow + iIECW;
            stiDetector = getDetector(row, sector);
            assert(strstr(stiDetector->getName().c_str(),"/IECW_"));
//		prepare "directory"
            dir = stiDetector->getName().c_str();idx = dir.Last('/'); assert(idx>=0); dir.Remove(idx+1,999);


            StiMaterial *matIECW                    = stiDetector->getMaterial();
            StiPlacement *stiPlacementIECW          = stiDetector->getPlacement();

            //construct west end-cap north side volume
            StiDetector *stiDetectorN = getDetectorFactory()->getInstance();
            ts = dir; ts +="IECWn"; //ts+=iIECW;
            buildPlanerVolume(*stiDetectorN, ts.Data(), 2.25 * 0.5, 0.555, 1.23485 * 0.5, 2.4326, 0., 0., stiPlacementIECW, matIECW);
             testPlanerVolume( stiDetector,  ts.Data(), 2.25 * 0.5, 0.555, 1.23485 * 0.5, 2.4326, 0., 0., stiPlacementIECW);
            del(row, sector);
            add(row, sector, stiDetectorN);
//            sWt+=stiDetectorN->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for IECW north side " << stiDetectorN->getName() << " at layer " << row << endm;

            //construct west end-cap bottom side volume
            StiDetector *stiDetectorB = getDetectorFactory()->getInstance();

            ts = dir; ts +="IECWb"; //ts+=iIECW;
            buildPlanerVolume(*stiDetectorB, ts.Data(), 2.25 * 0.5, 0.01925, 0.5065 * 0.5, 1.5619, -0.267875, 0., stiPlacementIECW, matIECW);
             testPlanerVolume( 0          ,  ts.Data(), 2.25 * 0.5, 0.01925, 0.5065 * 0.5, 1.5619, -0.267875, 0., stiPlacementIECW);
            int layer = getNRows();
            add(layer, sector, stiDetectorB);
//            sWt+=stiDetectorB->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for IECW bottom side " << stiDetectorB->getName() << " at layer " << layer << endm;

            //construct west end-cap south side volume
            StiDetector *stiDetectorS = getDetectorFactory()->getInstance();
            ts = dir; ts +="IECWs"; //ts+=iIECW;
            buildPlanerVolume(*stiDetectorS, ts.Data(), 2.25 * 0.5, 0.555, 4.35865 * 0.5, -0.870675, 0., 0., stiPlacementIECW, matIECW);
             testPlanerVolume((StiDetector*)(-1),  ts.Data(), 2.25 * 0.5, 0.555, 4.35865 * 0.5, -0.870675, 0., 0., stiPlacementIECW);
            layer = getNRows();
//            sWt+=stiDetectorS->getWeight();
            add(layer, sector, stiDetectorS);
            LOG_DEBUG << "StiIstDetectorBuilder::build planar volume for IECW south side " << stiDetectorS->getName() << " at layer " << layer << endm;

            matIECW                 = NULL;
            stiPlacementIECW        = NULL;
            stiDetector             = NULL;
         }
      }

      // Modify/Simplify west cooling loop (ICJR) to a box shape with same volume, and retrieve placement/material information from ICJS volume
      if (string(volumes[i].name) == string("ICJS")) {
         int startRow = getNRows() - kIstNumLadders;

         for (int iICJS = 0; iICJS < kIstNumLadders;iICJS += 2) {
            row = startRow + iICJS;
            stiDetector = getDetector(row, sector);
            assert(strstr(stiDetector->getName().c_str(),"/ICJS_"));
//		prepare "directory"
            dir = stiDetector->getName().c_str();idx = dir.Last('/'); assert(idx>=0); dir.Remove(idx+1,999);

            StiMaterial *matICJS                    = stiDetector->getMaterial();
            StiPlacement *stiPlacementICJS1         = stiDetector->getPlacement();

            stiDetector = getDetector(row + 1, sector);
//            dir = stiDetector->getName().c_str();idx = dir.Last('/'); assert(idx>=0); dir.Remove(idx+1,999);
            StiPlacement *stiPlacementICJS2         = stiDetector->getPlacement();

            StiDetector *stiDetectorICJR = getDetectorFactory()->getInstance();
            ts = dir; ts +="ICJRn."; ts+=iICJS ;
            buildPlanerVolume(*stiDetectorICJR, ts.Data(), 0.524188 * 0.5, 0.47625, 4.41625 * 0.5, stiPlacementICJS2->getNormalYoffset(), 0., 0.524188 * 0.5 + 6.35 * 0.5, stiPlacementICJS1, matICJS);
             testPlanerVolume( stiDetector,     ts.Data(), 0.524188 * 0.5, 0.47625, 4.41625 * 0.5, stiPlacementICJS2->getNormalYoffset(), 0., 0.524188 * 0.5 + 6.35 * 0.5, stiPlacementICJS1);
             testPlanerVolume( (StiDetector*)(-1));
            int layer = getNRows();
            add(layer, sector, stiDetectorICJR);
//            sWt+=stiDetectorICJR->getWeight();
            LOG_DEBUG << "StiIstDetectorBuilder::build west cooling loop volume " << stiDetectorICJR->getName() << " at layer " << layer << endm;

         }
      }

      //resize the dimenssion of cooling/cabling in transition area
      if (string(volumes[i].name) == string("ICCT")) {
         assert(strstr(stiDetector->getName().c_str(),"/ICCT_"));
         StiCylindricalShape *stiShape = (StiCylindricalShape *) stiDetector->getShape();
         stiShape->setOuterRadius(20.28725);
      }

      // retrieve material, energy loss information for support block tube ring re-definition
      if (string(volumes[i].name) == string("ISRR")) {
         assert(strstr(stiDetector->getName().c_str(),"/ISRR_"));
         if (!matISRA)
            matISRA = stiDetector->getMaterial();
      }
//      if (sWt<=0) sWt = sWtYF; sWt*=1e-3;sWtTot+=sWt;dWtTot+=fabs(sWt-gWt);
//       if (fabs(sWt-gWt)> 1e-2*gWt) {
//         printf("StiIstDetectorBuilder::buildInactiveVolumes(%s)geoWeight=%g stiWeight=%g diff=%g(%d%%)\n"
//               ,volumes[i].name ,gWt,sWt,sWt-gWt,int(fabs(sWt-gWt)/gWt*100));
//       }
      sWt = 0;
   }

   if (matISRA) {

      //StiShape for the support block inner thin tube volume (as a whole for all 24 support blocks)
      float halfDepth = 0.5 * 1.27;
      float thickness = 0.15;
      float outerRadius = 12.0753;
      float openingAngle = 2 * M_PI;

      //east support block inner thin tube volume
      StiDetector *stiDetectorISRAeast = getDetectorFactory()->getInstance();
      buildTubeVolume(*stiDetectorISRAeast, "ISRAeast", halfDepth, thickness, outerRadius, openingAngle, -34.19005 + 0.15875, matISRA);
      int layer = getNRows();
      add(layer, 0, stiDetectorISRAeast);
//      sWt =stiDetectorISRAeast->getWeight()*1e-3;
      LOG_DEBUG << "StiIstDetectorBuilder::build east support block thin tube volume " << stiDetectorISRAeast->getName() << " at layer " << layer << endm;

      //west support block inner thin tube volume
      StiDetector *stiDetectorISRAwest = getDetectorFactory()->getInstance();
      buildTubeVolume(*stiDetectorISRAwest, "ISRAwest", halfDepth, thickness, outerRadius, openingAngle, 24.68995 + 0.15875, matISRA);
      layer = getNRows();
      add(layer, 0, stiDetectorISRAwest);
//      sWt+=stiDetectorISRAwest->getWeight()*1e-3;
      sWtTot +=sWt;
      LOG_DEBUG << "StiIstDetectorBuilder::build west support block thin tube volume " << stiDetectorISRAwest->getName() << " at layer " << layer << endm;
   }
//    printf("StiIstDetectorBuilder::buildInactiveVolumes(IST ) geoWeight=%g stiWeight=%g diff=%g(%d%%)\n"
//             ,gWtTot,sWtTot,sWtTot-gWtTot,int(fabs(sWtTot-gWtTot)/gWtTot*100));



}

void StiIstDetectorBuilder::buildPlanerVolume(StiDetector &detector, string detName, float halfDepth, float thickness, float halfWidth, float yShift, float rShift, float zShift, StiPlacement *placement, StiMaterial *mat)
{
   //planar shape definition
   string shapeName = detName + "_planar";
   StiPlanarShape *stiShapeN = new StiPlanarShape(shapeName.data(), halfDepth, thickness, halfWidth);
   add(stiShapeN);

   //plannar placement definition
   float yOffset = placement->getNormalYoffset() + yShift;
   float normalRadius = placement->getNormalRadius() + rShift;
   StiPlacement *stiPlacementN = new StiPlacement();
   stiPlacementN->setZcenter(placement->getZcenter() + zShift);
   stiPlacementN->setLayerRadius(sqrt(yOffset * yOffset + normalRadius * normalRadius));
   stiPlacementN->setLayerAngle(placement->getNormalRefAngle() - atan2(yOffset, normalRadius));
   stiPlacementN->setRegion(StiPlacement::kMidRapidity);
   stiPlacementN->setNormalRep(placement->getNormalRefAngle(), normalRadius, yOffset);

   //detector definition
   detector.setName(detName.data());
   detector.setIsOn(true);
   detector.setIsActive(new StiNeverActiveFunctor());
   detector.setIsContinuousMedium(false); // true for gases
   detector.setIsDiscreteScatterer(true); // true for anything other than gas
   detector.setShape(stiShapeN);
   detector.setPlacement(stiPlacementN);
   detector.setGas(getGasMat()); // XXX:ds: Not sure what this does
   detector.setMaterial(mat);
}
void StiIstDetectorBuilder::buildTubeVolume(StiDetector &detector, string detName, float halfDepth, float thickness, float outerRadius, float openingAngle, float zCenter, StiMaterial *mat)
{
   //tube shape definition
   string shapeName = detName + "_tube";
   StiShape *stiShapeN = new StiCylindricalShape(shapeName.data(), halfDepth, thickness, outerRadius, openingAngle);
   add(stiShapeN);

   //tube placement definition
   StiPlacement *stiPlacementN = new StiPlacement();
   stiPlacementN->setZcenter(zCenter);
   stiPlacementN->setLayerRadius(outerRadius - thickness / 2);
   stiPlacementN->setLayerAngle(0);
   stiPlacementN->setRegion(StiPlacement::kMidRapidity);
   stiPlacementN->setNormalRep(0, outerRadius - thickness / 2, 0);

   //detector definition
   detector.setName(detName.data());
   detector.setIsOn(true);
   detector.setIsActive(new StiNeverActiveFunctor());
   detector.setIsContinuousMedium(false); // true for gases
   detector.setIsDiscreteScatterer(true); // true for anything other than gas
   detector.setShape(stiShapeN);
   detector.setPlacement(stiPlacementN);
   detector.setGas(getGasMat()); // XXX:ds: Not sure what this does
   detector.setMaterial(mat);
}
