// Hey Emacs this is -*-c++-*-
#ifndef STAR_EEmcTTDisplay
#define STAR_EEmcTTDisplay
// $Id: EEmcTTDisplay.h,v 1.7 2004/04/13 14:53:38 zolnie Exp $

/*!
 *                                                                     
 * \class  EEmcTTDisplay
 * \author Piotr A. Zolnierczuk
 * \date   2003/12/08
 *
 * \brief  EEmc Tower and Track display
 *
 * This is a simple root TGeoXXXX based class. It displays EEMC tower geometry
 * (the class inherits from EEmcGeomSimple) and allows for "turining on/off" individual
 * towers as well as displays muDST tracks (StMuTrack).
 * 
 * use 
 *  
 * example see StRoot/StEEmcPool/macros/TTM/show.C
 * \image html snapshot.jpg "Sample snapshot"
 */                                                                      

#include "StEEmcUtil/EEmcGeom/EEmcGeomSimple.h"
class TString;
class TGeoVolume;
class TGeoMatrix;
class TList;
//
class StMuTrack;
class EEmcTower;

class EEmcTTDisplay : public EEmcGeomSimple {
public: 
  //! the constructor
  /// \param name  the top EEMC volume (TGeoVolume) label
  EEmcTTDisplay(const char *name="eemc");

  //! the destructor
  ~EEmcTTDisplay();
  
  /// returns top EEMC TGeoVolume 
  TGeoVolume* operator() () { return mEEmc; }; 
  /// returns top TGeoVolume 
  TGeoVolume* GetVolume()   { return mEEmc; }; 

  //! adds tower to the list
  /// \param tile name (in the form of 05TC11) 
  /// \return      kTRUE on success and kFALSE on failure
  Bool_t       AddTower(const char *tile);

  //! adds tower to the list
  /// \param sec     sector index    [0,mNumSec)
  /// \param sub     subsector index [0,mNumSSec)
  /// \param eta     eta index       [0,mNumEta)
  /// \return      kTRUE on success and kFALSE on failure
  Bool_t       AddTower(int sec, int sub, int eta) {   
    return AddTower(volumeName(sec,sub,eta)); }  

  //! adds tower to the list 
  /// \param tower a reference to EEmcTower 
  /// \return      kTRUE on success and kFALSE on failure
  Bool_t       AddTower(const EEmcTower& tower)       ;

  //! adds track to the list 
  /// \param x     x-component of the track origin
  /// \param y     y-component of the track origin
  /// \param z     z-component of the track origin
  /// \param px    x-component of the track momentum
  /// \param py    y-component of the track momentum
  /// \param pz    z-component of the track momentum
  /// \param qB    sign sensitive product of the particle charge and magnetic field 
  /// \return      kTRUE on success and kFALSE on failure
  Bool_t       AddTrack(Double_t x, Double_t y, Double_t z, 
			   Double_t px, Double_t py, Double_t pz, Double_t qB);
  //! adds track to the list
  /// \param track a refence to StMuTrack
  /// \return      kTRUE on success and kFALSE on failure
  Bool_t       AddTrack(const StMuTrack& track);

  //! draws towers/tracks from the list
  /// \param option - not uset at the moment
  void         Draw ( const Option_t* = "");

  //! clears tower/track lists
  /// \param option - not uset at the moment
  void         Clear( const Option_t* = ""); 

  /// prints StMuTrack and EEmcTower to ostream
  /// \param out    a reference to std::ostream
  /// \param track  a reference to class  StMuTrack
  /// \param tower  a reference to struct EEmcTower
  void         Out(ostream &out, const StMuTrack &track, const EEmcTower &tower);
  void         Out(TString &out, const StMuTrack &track, const EEmcTower &tower);
  
  //! sets STAR magnetic field 
  /// \param B magnetic field (sign sensitive) in Tesla
  void   SetMagneticField(double B) { mBField = B;    }
  //! returns STAR magnetic fiels in Tesla
  double GetMagneticField()         { return mBField; }


protected:
  //! initializes EEMC geometry: sector,subsectors and towers
  /// \param topName a label of a top most volume
  void  initGeometry(const char *topName);

  /// volume name based on sec, subsector and eta
  /// \param sec     sector index    [0,mNumSec)
  /// \param sub     subsector index [0,mNumSSec)
  /// \param eta     eta index       [0,mNumEta)
  /// \return a pointer to a static string (fixit)
  char *volumeName(int sec, int sub=-1, int eta=-1);
  /// volume name based on tower structure
  /// \param tower  a reference to tower structure
  /// \return a pointer to a static string (fixit)
  char *volumeName(const EEmcTower& tower);

  TGeoVolume* mEEmc; /**<- top TGeoVolume        */
  TList *mTowerHits; /**<- list of towers        */
  TList *mTrackHits; /**<- list of tracks        */
  double mBField   ; /**<- magnetic field (in T) */ 
  

public:
  ClassDef(EEmcTTDisplay, 1)   // 
};


#endif


// $Log: EEmcTTDisplay.h,v $
// Revision 1.7  2004/04/13 14:53:38  zolnie
// *** empty log message ***
//
// Revision 1.6  2004/01/27 20:38:41  zolnie
// more docs
//
// Revision 1.5  2004/01/27 16:26:14  zolnie
// polished doxygen documentation
//
// Revision 1.4  2004/01/26 22:54:15  zolnie
// after name cleanup
//
// Revision 1.3  2004/01/26 21:51:53  zolnie
// shorter names
//
// Revision 1.2  2004/01/26 21:08:31  zolnie
// working track/tower display (before big farewell cleanup)
//
// Revision 1.1  2004/01/19 22:07:50  zolnie
// toward track/tower display
//

