//StiDetectorContainer.cxx
//M.L. Miller (Yale Software)
//02/02/01

#include <iostream.h>
#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>
#include <string>

//StiGui
#include "StiGui/StiRootDrawableDetector.h"

//Sti
#include "StiMapUtilities.h"
#include "StiDetPolygon.h"
#include "StiDetector.h"
#include "StiMaterial.h"
#include "StiDetectorContainer.h"


StiDetectorContainer* StiDetectorContainer::sinstance = 0;

ostream& operator<<(ostream&, const DetectorMapKey&);
ostream& operator<<(ostream&, const MaterialMapKey&);

StiDetectorContainer* StiDetectorContainer::instance()
{
    return (sinstance) ? sinstance : new StiDetectorContainer();
}

void StiDetectorContainer::kill()
{
    if (sinstance) {
	delete sinstance;
	sinstance = 0;
    }
    return;
}

StiDetectorContainer::StiDetectorContainer() : mdraw(true)
{
    cout <<"StiDetectorContainer::StiDetectorContainer()"<<endl;
    sinstance = this;
}

StiDetectorContainer::~StiDetectorContainer()
{
    cout <<"StiDetectorContainer::~StiDetectorContainer()"<<endl;
    clearAndDestroy();
}

const materialmap& StiDetectorContainer::materialMap() const
{
    return mmaterialmap;
}

StiMaterial* StiDetectorContainer::material(const MaterialMapKey& key) const
{
    materialmap::const_iterator where = mmaterialmap.find(key);
    return (where!= mmaterialmap.end()) ? (*where).second : 0;
}

void StiDetectorContainer::push_back(StiDetPolygon* poly)
{
    insert( detectorMapValType( poly->radius(), poly ) );
    return;
}

void StiDetectorContainer::clearAndDestroy()
{
    for (detectormap::iterator it=begin(); it!=end(); ++it) {
	delete (*it).second;
	(*it).second = 0;
    }
    
    for (materialmap::iterator it=mmaterialmap.begin(); it!=mmaterialmap.end(); ++it) {
	delete (*it).second;
	(*it).second = 0;
    }
    return;
}

void StiDetectorContainer::setToDetector(double radius)
{
    reset(); //full reset
    //Look to see first instance with position greater than this
    detectormap::iterator where = lower_bound(radius);
    if (where==end()) { //Didn't find it, set to outermost layer
	--where;
    }

    mcurrent = where;

    //Now we have to check if one less postion is closer (as long as we're not at the beginning)
    if ( where!=begin()) { //Nothing below us
	double found_radius = (*mcurrent).second->operator*()->getCenterRadius();
	double one_less_radius = (*(--where)).second->operator*()->getCenterRadius();
	if ( fabs(one_less_radius - radius) < fabs(found_radius - radius) ) {
	    mcurrent = where;
	}
    }
    return;
}

void StiDetectorContainer::setToDetector(double radius, double angle)
{
    setToDetector(radius);
    (*mcurrent).second->setToAngle( angle);
}

void StiDetectorContainer::setToDetector(StiDetector*layer)
{
    setToDetector( layer->getCenterRadius(), layer->getCenterRefAngle() );
}

void StiDetectorContainer::push_back(StiDetector* layer)
{
    //Which radial layer should this detector belong to?
    detectormap::const_iterator where = find( layer->getCenterRadius() );
    if (where==end()) {
	cout <<"StiDetectorContainer::push_back(StiDetector*) !!!! Error: no layer for detector"<<endl;
	return;
    }
    (*where).second->push_back(layer);
    return;
}

void StiDetectorContainer::reset()
{
    mcurrent = begin();
    for (detectormap::iterator it=begin(); it!=end(); ++it) {
	(*it).second->reset();
    }
    return;
}

StiDetector* StiDetectorContainer::operator*() const
{
    if (mcurrent==end()) return 0;
    else {
	StiDetPolygon& rpoly = *((*mcurrent).second);
	return *rpoly;
    }
}

void StiDetectorContainer::moveIn()
{
    double currentangle = (*mcurrent).second->operator*()->getCenterRefAngle();
    if (mcurrent==begin()) return;
    
    else {
	--mcurrent;
	(*mcurrent).second->setToAngle(currentangle);
    }
    return;
}

void StiDetectorContainer::moveOut()
{
    if (mcurrent==end() ) {
	return; //Nowhere to go
    }
    
    else {
    double currentangle = (*mcurrent).second->operator*()->getCenterRefAngle();
	++mcurrent;
	if (mcurrent==end()) {
	    --mcurrent;
	}
	(*mcurrent).second->setToAngle(currentangle);
    }
    return;
}

void StiDetectorContainer::movePlusPhi()
{
    (*mcurrent).second->operator++();
}

void StiDetectorContainer::moveMinusPhi()
{
    (*mcurrent).second->operator--();
}

// Load all material definition files from a given directory
void StiDetectorContainer::buildMaterials(const char* buildDirectory){
  char* buildfile = new char[200];

  DIR *pDir = opendir(buildDirectory);
  struct dirent *pDirEnt;
  struct stat fileStat;

  while( (pDirEnt = readdir(pDir)) != 0){
    sprintf(buildfile, "%s/%s", buildDirectory, pDirEnt->d_name);

    // get file attributes
    stat(buildfile, &fileStat);

    // if regular file, process as material
    if(S_ISREG(fileStat.st_mode)){
      StiMaterial *pMaterial = new StiMaterial();
      pMaterial->build(buildfile);

      // add to materials map by name
      MaterialMapKey key(pMaterial->getName());
      mmaterialmap.insert( materialMapValType(key, pMaterial) );

      cout << "materialMap[" << key.name << "]='" 
           << *(mmaterialmap[key]) << "'" << endl;
    } // if is regular file
  }

  closedir(pDir);
}// buildMaterials

void StiDetectorContainer::buildPolygons(const char* buildDirectory)
{
    char* buildfile = new char[200];
    
    DIR *pDir = opendir(buildDirectory);
    struct dirent *pDirEnt;
    struct stat fileStat;
    
    while( (pDirEnt = readdir(pDir)) != 0){
	sprintf(buildfile, "%s/%s", buildDirectory, pDirEnt->d_name);
	
	// get file attributes
	stat(buildfile, &fileStat);
	
	// if regular file, process
	if(S_ISREG(fileStat.st_mode)){

	    //do action here
	    StiDetPolygon* poly = new StiDetPolygon();
	    poly->build(buildfile);
	    //cout <<"Building Polygon from "<<buildfile<<endl;
	    //cout <<(*poly)<<endl;
	    push_back(poly);

	} // if is regular file
    }
    
    closedir(pDir);
    return;
}

// Recursively load all detector definition files from the given directory.
void StiDetectorContainer::buildDetectors(const char* buildDirectory)
{
  char* buildfile = new char[200];

  DIR *pDir = opendir(buildDirectory);
  struct dirent *pDirEnt;
  struct stat fileStat;

  while( (pDirEnt = readdir(pDir)) != 0){
    sprintf(buildfile, "%s/%s", buildDirectory, pDirEnt->d_name);
    
    // get file attributes
    stat(buildfile, &fileStat);

    // is this a directory?  if so, recursively build directory
    if((S_ISDIR(fileStat.st_mode)) && pDirEnt->d_name[0] != '.'){
      buildDetectors(buildfile);
    } // if is directory
    
    // if regular file, process as detector
    if(S_ISREG(fileStat.st_mode)){
      StiDetector* layer = makeDetectorObject();
      layer->build(buildfile);
      if (layer->isOn()) push_back(layer);

    } // if is regular file

  }

  closedir(pDir);
 
  return;
}

StiDetector* StiDetectorContainer::makeDetectorObject() const
{
    StiDetector* layer = 0;
    if (mdraw) {
        layer = new StiRootDrawableDetector();
    }else {
        layer = new StiDetector();
    }
    return layer;
}

void StiDetectorContainer::print() const
{
    cout <<"\nStiDetecotrContainer::print()\n"<<endl;
    for (detectormap::const_iterator it=begin(); it!=end(); ++it) {
	cout <<"\t"<<(*it).first<<"\t";
	(*it).second->print();
    }
}
