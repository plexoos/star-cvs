#ifndef STI_DETECTOR_BUILDER_H
#define STI_DETECTOR_BUILDER_H

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <math.h>
#include "StDetectorDbMaker/StiTrackingParameters.h"
#include "Sti/StiMapUtilities.h"
#include "Sti/Base/Named.h"
#include "StThreeVector.hh"
#include "StiVMCToolKit.h"
#include "TMath.h"
using namespace std;
class StiDetector;
class StiMaterial;
class StiShape;
class StMaker; 
class TDataSet;
class StiPlanarShape;
class StiCylindricalShape;
template<class Factorized>class Factory;

// Set up stl maps for by-name lookup of shapes and materials.
// Not used for placements because they are unique to each detector.
typedef map<NameMapKey, StiMaterial*> materialMap;
typedef materialMap::value_type materialMapValType;
typedef map<NameMapKey, StiShape*> shapeMap;
typedef shapeMap::value_type shapeMapValType;
typedef map<NameMapKey, StiDetector*> detectorMap;
typedef detectorMap::const_iterator detectorIterator;
typedef detectorMap::value_type detectorMapValType;

/*!
  Class defines the notion of a detector builder. It creates the various components of
  a detector and set their shape, placement, and material properties.
  <p>
  \author Ben Norman (Kent State University) Aug 1, 2001
  \author Claude Pruneau (Wayne State University) Oct 16, 2002
*/
class StiDetectorBuilder : public Named
{
public:

  StiDetectorBuilder(const string & name,bool active, const string & inputFile);
  virtual ~StiDetectorBuilder(); 
  detectorMap getDetectors(){ return mDetectorMap; }
  virtual StiMaterial * add(StiMaterial *material);
  virtual StiShape    * add(StiShape *shape);
  virtual StiDetector * add(StiDetector *detector);
  virtual StiDetector * add(unsigned int row, unsigned int sector, StiDetector *detector);
  virtual StiMaterial * findMaterial(const string& szName) const;
  virtual StiShape    * findShape(const string& szName) const;
  virtual StiDetector * findDetector(const string& szName) const;
  virtual StiDetector * getDetector(unsigned int layer, unsigned int sector) const;
  virtual void setDetector(unsigned int layer, unsigned int sector, StiDetector * detector);
///Returns the number of active rows in the detector
///Rows can be counted radially or longitudinally
  virtual void setNRows(unsigned int nRows) {if (_detectors.size() < nRows) _detectors.resize(nRows);}
  virtual unsigned int  getNRows() const {return _detectors.size();}
  virtual unsigned int  getNSectors(unsigned int row=0) const;
  virtual void setNSectors(unsigned int row, unsigned int nSectors) {
    setNRows(row+1);if (_detectors[row].size() < nSectors) _detectors[row].resize(nSectors);
  }
  virtual bool hasMore() const;
  virtual StiDetector* next();
  virtual void build(StMaker&source);
  virtual void buildDetectors(StMaker&source);

  double nice(double angle) const;
  void setGroupId(int id) { _groupId = id;}
  int  getGroupId() const {return _groupId;}
  void setTrackingParameters(const StiTrackingParameters & pars);
  StiTrackingParameters *getTrackingParameters() { return  _trackingParameters;}
  Factory<StiDetector>* getDetectorFactory() {return _detectorFactory;}
  void SetCurrentDetectorBuilder(StiDetectorBuilder *m) {fCurrentDetectorBuilder = m;}
  static StiDetectorBuilder *GetCurrentDetectorBuilder() {return fCurrentDetectorBuilder;}
  static  void MakeAverageVolume(TGeoPhysicalNode *nodeP) {
    if (fCurrentDetectorBuilder) fCurrentDetectorBuilder->AverageVolume(nodeP);
  }
  virtual void AverageVolume(TGeoPhysicalNode *nodeP);
  virtual void useVMCGeometry() {}
  void    setGasMat(StiMaterial    *m) {_gasMat = m;}
  StiMaterial *getGasMat()   {return _gasMat;}
  static void setDebug(int m = 0) {_debug = m;}
  static int  debug() {return _debug;}
 protected:
 
  int                 _groupId;
  bool                _active;
  materialMap         mMaterialMap;
  shapeMap            mShapeMap;
  detectorMap         mDetectorMap;
  detectorIterator    mDetectorIterator; 
  vector< vector<StiDetector*> > _detectors;
  Factory<StiDetector>*_detectorFactory;
  StiTrackingParameters *_trackingParameters;
  string _inputFile;
  static StiDetectorBuilder* fCurrentDetectorBuilder;
  StiMaterial    * _gasMat; // Mother Volume material
  static   int     _debug;
};

inline double StiDetectorBuilder::nice(double angle) const 
{
  while(angle >=  M_PI){ angle -= 2.*M_PI; }
  while(angle <  -M_PI){ angle += 2.*M_PI; }
  return angle;
}


#endif // ifndef STI_DETECTOR_BUILDER_H
