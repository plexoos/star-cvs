/**********************************************************
 * $Id: StRichRingCalculator.cxx,v 2.1 2000/09/29 01:35:37 horsley Exp $
 *
 * Description:
 *  
 *
 *  $Log: StRichRingCalculator.cxx,v $
 *  Revision 2.1  2000/09/29 01:35:37  horsley
 *  Many changes, added StRichRingHits, StRichMcSwitch, TpcHitvecUtilities
 *  Modified the StRichCalculator, StRichTracks, StRichMCTrack, StRichRingPoint
 *
 *  for individual photons, modified minimization routine to correct boundary
 *  problems
 *
 *  Revision 2.2  2000/09/29 17:55:51  horsley
 *  fixed bug in Minimization routine, included StMagF stuff (commented out)
 *  changed StRichRingPoint  HUGE_VALUE   ---> MAXFLOAT for default value
 *
 *  Revision 2.1  2000/09/29 01:35:37  horsley
 *  Many changes, added StRichRingHits, StRichMcSwitch, TpcHitvecUtilities
 *  Modified the StRichCalculator, StRichTracks, StRichMCTrack, StRichRingPoint
 *
 *  Revision 1.3  2000/06/16 02:37:12  horsley
 *  many additions, added features to pad plane display (MIPS, rings, etc)
 *  along with Geant info. Added StMcRichTrack. Modified access to hit collection.
 *
 *  Revision 1.2  2000/05/19 19:06:11  horsley
 *  many revisions here, updated area calculation ring calc, ring, tracks , etc...
 *
 *  Revision 1.1  2000/04/03 19:36:08  horsley
 *  initial revision


  mInnerRing = new StRichRingPoint(track,eInnerRing);
  mOuterRing = new StRichRingPoint(track,eOuterRing);
  mMeanRing  = new StRichRingPoint(track,eMeanRing);
 
  mInnerMinimization = new StRichMinimization(mInnerRing);
  mMeanMinimization  = new StRichMinimization(mMeanRing);
  mOuterMinimization = new StRichMinimization(mOuterRing);
  
  mConstantAreaAngle=M_PI;


StRichRingCalculator::StRichRingCalculator(StRichTrack* track, StParticleDefinition* particle)  {

StRichRingCalculator::StRichRingCalculator(StRichTrack* track, StParticleDefinition* particle) {
  mInnerRing = new StRichRingPoint(track,eInnerRing);
  mOuterRing = new StRichRingPoint(track,eOuterRing);
  mMeanRing  = new StRichRingPoint(track,eMeanRing);
 
  mInnerMinimization = new StRichMinimization(mInnerRing);
  mMeanMinimization  = new StRichMinimization(mMeanRing);
  mOuterMinimization = new StRichMinimization(mOuterRing);
  mInnerMinimization = new StRichMinimization(mInnerRing);
  mConstantAreaAngle=M_PI;
  setParticleType(particle);

  mRichMaterialsDb = StRichMaterialsDb::getDb();




  mMonteCarloArea.Set(18);
  delete mInnerRing;
  delete mOuterRing;
  delete mMeanRing;
    delete mMeanRing;
  delete mInnerMinimization;
  delete mOuterMinimization;
  delete mMeanMinimization;
    delete mMeanMinimization;

    mInnerMinimization = 0;
double StRichRingCalculator::calculateArea(double cut=0, bool gapCorrection=true) {

}

  mTotalAngle = areaCalc.getTotalAngle();
  double area = areaCalc.calculateArea(cut);
  mTotalConstantAngle = areaCalc.getTotalConstantAngle();
  }

  // clear vector of points
  vectorOfPtsToDraw     = areaCalc.getPtsToDraw();
  mTotalAngleOnPadPlane = areaCalc.getTotalAngleOnPadPlane();
}
  return area;
  mDrawRingPoints = flag;
}




double StRichRingCalculator::calculateConstantArea(double cut, bool gapCorrection, double& totArea) {
  StRichArea areaCalc(mInnerRing,mOuterRing);
  areaCalc.correctForGap(gapCorrection);

  double area           = areaCalc.calculateConstantArea(cut);
  mTotalAngleOnPadPlane = areaCalc.getTotalAngleOnPadPlane();
  mConstantAreaAngle    = areaCalc.getConstantAreaAngle();  
  totArea               = areaCalc.getTotalArea();

  /*
    vectorOfPtsToDraw.clear();
    vectorOfPtsToDraw.resize(0);
    vectorOfPtsToDraw     = areaCalc.getPtsToDraw();
  */

  return area;
  return mMonteCarloPoints;
}
void StRichRingCalculator::setMonteCarloSwitch(bool set) {




StRichRingPoint* StRichRingCalculator::getRing(StRichRingDefinition ringType) {
  if (ringType==eInnerRing)  return  mInnerRing;
  return  (closestInnerRingPoint - closestOuterRingPoint).mag();
  if (ringType==eMeanRing)   return  mMeanRing;
  return 0;

    if (ringType==eInnerRing)  return  mInnerRing;
double StRichRingCalculator::getRingWidth() {
    if (ringType==eMeanRing)   return  mMeanRing;
    return 0;
}
double StRichRingCalculator::getRingWidth() const {
void StRichRingCalculator::setParticleType(StParticleDefinition* particle) { 
  mInnerRing->setParticleType(particle);
  mOuterRing->setParticleType(particle);
  mMeanRing->setParticleType(particle);
void StRichRingCalculator::setParticleType(StParticleDefinition* particle) { 

    mMeanRing->setParticleType(particle);
double StRichRingCalculator::getInnerDistance(StThreeVectorF& testPoint,double& innerAngle) {
  closestInnerRingPoint = mInnerMinimization->rotatedMin(testPoint);
  innerAngle            = mInnerMinimization->getPsi();
  return (closestInnerRingPoint - testPoint).perp();
					      double& innerAngle) {

    return (closestInnerRingPoint - testPoint).perp();
double StRichRingCalculator::getOuterDistance(StThreeVectorF& testPoint,double& outerAngle) {
  closestOuterRingPoint = mOuterMinimization->rotatedMin(testPoint);
  outerAngle            = mOuterMinimization->getPsi();
  return (closestOuterRingPoint - testPoint).perp();
    return (closestOuterRingPoint - testPoint).perp();
double StRichRingCalculator::getMeanDistance(StThreeVectorF& testPoint,double& meanAngle) {
  closestMeanRingPoint = mMeanMinimization->rotatedMin(testPoint);
  meanAngle            = mMeanMinimization->getPsi();
  mMeanPathInQuartz   = mMeanMinimization->getMeanPathInQuartz();
  mMeanPathInRadiator = mMeanMinimization->getMeanPathInRadiator();
  return (closestMeanRingPoint - testPoint).perp();
    closestMeanRingPoint = mMeanMinimization->rotatedMin(testPoint);
    meanAngle            = mMeanMinimization->getPsi();

    mMeanPathInQuartz   = mMeanMinimization->getMeanPathInQuartz();
    mMeanPathInRadiator = mMeanMinimization->getMeanPathInRadiator();
    return (closestMeanRingPoint - testPoint).perp();
}

void StRichRingCalculator::clear() {
  
  StThreeVectorF temp(0,0,0);
  closestInnerRingPoint = temp;
double StRichRingCalculator::getMeanPathInRadiator() { return mMeanMinimization->getMeanPathInRadiator();}
double StRichRingCalculator::getMeanPathInQuartz() { return mMeanMinimization->getMeanPathInQuartz();}
StThreeVectorF StRichRingCalculator::getOuterRingPoint() { return closestOuterRingPoint;}
StThreeVectorF StRichRingCalculator::getInnerRingPoint() { return closestInnerRingPoint;}
StThreeVectorF StRichRingCalculator::getMeanRingPoint()  { return closestMeanRingPoint;}

double StRichRingCalculator::getMeanPathInRadiator() { return mMeanPathInRadiator;}
double StRichRingCalculator::getMeanPathInQuartz() { return mMeanPathInQuartz;}

double StRichRingCalculator::getConstantAreaAngle() {return mConstantAreaAngle;}

double StRichRingCalculator::getTotalArea() { return mTotalArea;}
double StRichRingCalculator::getTotalAngle() { return mTotalAngleOnPadPlane;}
double StRichRingCalculator::getPadPlaneArea() { return mPadPlaneArea;}


vector<StThreeVectorF >& StRichRingCalculator::getPtsToDraw() { return vectorOfPtsToDraw;}
  }

  return normalArea;
}

















