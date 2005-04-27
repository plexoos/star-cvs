#ifndef STI_DETECTOR_BUILDER_H
#define STI_DETECTOR_BUILDER_H

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include <math.h>
#include "Sti/StiTrackingParameters.h"
#include "Sti/StiMapUtilities.h"
#include "Sti/Base/Named.h"
#include "Sti/Base/Loadable.h"
#include "StThreeVector.hh"
#include "StiVMCToolKit.h"
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
class StiDetectorBuilder : public Named, public Loadable
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
  virtual unsigned int  getNRows() const;
  virtual unsigned int  getNSectors(unsigned int row=0) const;
  virtual void setNRows(unsigned int nRows);
  virtual void setNSectors(unsigned int row, unsigned int nSectors);
  virtual bool hasMore() const;
  virtual StiDetector* next();
  virtual void build(StMaker&source);
  virtual void buildDetectors(StMaker&source);
	void load(const string & userFileName, StMaker & source)
		{
			Loadable::load(userFileName,source);
		}

  double nice(double angle) const;
  void setGroupId(int id);
  int  getGroupId() const;
  void setTrackingParameters(const TrackingParameters_st & pars);
  void setTrackingParameters(const StiTrackingParameters & pars);
  StiTrackingParameters & getTrackingParameters();
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
  StiTrackingParameters _trackingParameters;
  string _inputFile;
  static StiDetectorBuilder* fCurrentDetectorBuilder;
  StiMaterial    * _gasMat; // Mother Volume material
  static   int     _debug;
};

///Returns the number of active rows in the detector
///Rows can be counted radially or longitudinally
inline unsigned int  StiDetectorBuilder::getNRows() const
{
  return _detectors.size();
}

///Returns the number of sectors (or segments) in a the
///given row. Sector are expected to be azimuthally
///distributed.
inline unsigned int  StiDetectorBuilder::getNSectors(unsigned int row) const
{
  if (row>_detectors.size())
    {
      string message = "StiDetectorBuilder::getNSectors() - ERROR - argument row out of bound:";
      message+=row;
      message+=">";
      message+=_detectors.size();
      throw runtime_error(message.c_str());
    }
  return _detectors[row].size();
}

///Sets the number of the number of rows of active
///detectors.
inline void StiDetectorBuilder::setNRows(unsigned int nRows)
{
  if (_detectors.size() < nRows) _detectors.resize(nRows);
}

inline void StiDetectorBuilder::setNSectors(unsigned int row, unsigned int nSectors)
{
  setNRows(row+1);
  if (_detectors[row].size() < nSectors) _detectors[row].resize(nSectors);
}

inline double StiDetectorBuilder::nice(double angle) const 
{
  while(angle >=  M_PI){ angle -= 2.*M_PI; }
  while(angle <  -M_PI){ angle += 2.*M_PI; }
  return angle;
}

inline  StiDetector * StiDetectorBuilder::getDetector(unsigned int row, unsigned int sector) const
{
  if (row>_detectors.size())
    {
      string message = "StiDetectorBuilder::getDetector() - ERROR - argument row out of bound:";
      throw runtime_error(message.c_str());
    }
  if (sector>_detectors[row].size())
    {
      string message = "StiDetectorBuilder::getDetector() - ERROR - argument sector out of bound";
      throw runtime_error(message.c_str());
    }
  return _detectors[row][sector];
}


inline  void StiDetectorBuilder::setDetector(unsigned int row, unsigned int sector, StiDetector *detector)
{
  setNSectors(row+1,sector+1);
   _detectors[row][sector] = detector;
}

inline void StiDetectorBuilder::setGroupId(int id)
{
  _groupId = id;
}

inline int  StiDetectorBuilder::getGroupId() const
{
  return _groupId;
}

inline void StiDetectorBuilder::setTrackingParameters(const StiTrackingParameters & pars)
{
  _trackingParameters = pars;
}

inline  StiTrackingParameters & StiDetectorBuilder::getTrackingParameters()
{
  return  _trackingParameters;
}

#endif // ifndef STI_DETECTOR_BUILDER_H
