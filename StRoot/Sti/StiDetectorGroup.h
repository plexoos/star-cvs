#ifndef StiDetectorGroup_H_INCLUDED
#define StiDetectorGroup_H_INCLUDED

#include <stdexcept>
#include "Sti/Base/Named.h"
#include "Sti/Base/Factory.h"
#include "Sti/StiToolkit.h"
#include "Sti/StiDetectorBuilder.h"
template<class Event,class Detector> class StiHitLoader;
class StiDedxCalculator;
class StiElossCalculator;

template<class Event>
class StiDetectorGroup : public Named
{
  public:

  /// Get a hit loader appropriate for this detector group
  virtual StiHitLoader<Event,StiDetectorBuilder> * getHitLoader();
  
  /// Get a detector builder appropriate for this detector group
  virtual StiDetectorBuilder * getDetectorBuilder();

  virtual void initialize() {}

  /// Get a pid calculator appropriate for this detector group
  /// A dedx calculator is used after the track are fitted
  /// to determine the average (or appropriate measure) dedx.
  virtual StiDedxCalculator * getDedxCalculator();

  /// Get an energy loss calculator appropriate for this detector group
  /// An eloss calculator is used in the kalman propagation to determine
  /// the track energy loss.
  StiElossCalculator * getElossCalculator();
  void setGroupId(int id);
  int  getGroupId() const;

 protected:

  StiDetectorGroup(const string & name);
  StiDetectorGroup(const string & name,
		   StiHitLoader<Event,StiDetectorBuilder> * hitLoader,
		   StiDetectorBuilder * detectorBuilder,
		   StiDedxCalculator *  dedxCalculator,
		   StiElossCalculator * elossCalculator);
  ~StiDetectorGroup();
  StiHitLoader<Event,StiDetectorBuilder> * _hitLoader;
  StiDetectorBuilder * _detectorBuilder;
  StiDedxCalculator *  _dedxCalculator;
  StiElossCalculator * _elossCalculator; 
  /// Detector group identifier.
  int _groupId;
};


template<class Event>
StiDetectorGroup<Event>::StiDetectorGroup(const string & name,
					  StiHitLoader<Event,StiDetectorBuilder> * hitLoader,
					  StiDetectorBuilder * detectorBuilder,
					  StiDedxCalculator *  dedxCalculator,
					  StiElossCalculator * elossCalculator)
  :  Named(name),
     _hitLoader(hitLoader),
     _detectorBuilder(detectorBuilder),
     _dedxCalculator(dedxCalculator),
     _elossCalculator(elossCalculator),
     _groupId(-1)
{
  // If a loader was specified, make sure it uses the selected detector builder.
  if (_hitLoader)
    {
      _hitLoader->setDetector(detectorBuilder);
    }
  initialize();
}

template<class Event>
StiDetectorGroup<Event>::StiDetectorGroup(const string & name)
  : Named(name),
     _hitLoader(0),
     _detectorBuilder(0),
     _dedxCalculator(0),
     _elossCalculator(0),
     _groupId(-1)
{
  initialize();
}

template<class Event>
StiDetectorGroup<Event>::~StiDetectorGroup()
{
  delete _hitLoader;
  delete _detectorBuilder;
}

/// Get a hit loader appropraite for this detector group
template<class Event>
StiHitLoader<Event,StiDetectorBuilder> * StiDetectorGroup<Event>::getHitLoader()
{
  if (_detectorBuilder==0)
    {
      string message = "StiDetectorGroup::getHitLoader() - ERROR - Hit loader requested for non active detector:";
      message += getName();
      throw logic_error(message.c_str());
    }
  /*  if (_hitLoader==0)
    {
      string message = "StiDetectorGroup::getHitLoader() - ERROR - Hit loader == 0 for detector:";
      message += getName();
      throw logic_error(message.c_str());
      }*/
  return _hitLoader;
}


/// Get a detector builder appropriate for this detector group
template<class Event>
StiDetectorBuilder * StiDetectorGroup<Event>::getDetectorBuilder()
{
  if (_detectorBuilder==0)
    {
      string message = "StiDetectorGroup::getDetectorBuilder() - ERROR - builder == 0 for detector:";
      message += getName();
      throw logic_error(message.c_str());
    }
  return _detectorBuilder; 
}

/// Get a pid calculator appropriate for this detector group
/// A dedx calculator is used after the track are fitted
/// to determine the average (or appropriate measure) dedx.
template<class Event>
StiDedxCalculator * StiDetectorGroup<Event>::getDedxCalculator()
{ 
  if (_dedxCalculator==0)
    {
      string message = "StiDetectorGroup::getDedxCalculator() - ERROR - dedxCalculator == 0 for detector:";
      message += getName();
      throw logic_error(message.c_str());
    }
  return _dedxCalculator; 
}


/// Get an energy loss calculator appropriate for this detector group
/// An eloss calculator is used in the kalman propagation to determine
/// the track energy loss.
template<class Event>
StiElossCalculator * StiDetectorGroup<Event>::getElossCalculator()
{
  if (_elossCalculator==0)
    {
      string message = "StiDetectorGroup::getElossCalculator() - ERROR - elossCalculator == 0 for detector:";
      message += getName();
      throw logic_error(message.c_str());
    }
  return _elossCalculator; 
}

template<class Event>
inline void StiDetectorGroup<Event>::setGroupId(int id)
{
  if (_detectorBuilder) _detectorBuilder->setGroupId(id);
  _groupId = id;
}

template<class Event>
inline int  StiDetectorGroup<Event>::getGroupId() const
{
  return _groupId;
}

#endif

