/***************************************************************************
 *
 * $Id: StHbtPair.cc,v 1.17 2001/03/28 22:35:20 flierl Exp $
 *
 * Author: Brian Laziuk, Yale University
 *         slightly modified by Mike Lisa
 ***************************************************************************
 *
 * Description: part of STAR HBT Framework: StHbtMaker package
 *    the Pair object is passed to the PairCuts for verification, and
 *    then to the AddRealPair and AddMixedPair methods of the
 *    Correlation Functions
 *
 ***************************************************************************
 *
 * $Log: StHbtPair.cc,v $
 * Revision 1.17  2001/03/28 22:35:20  flierl
 * changes and bugfixes in qYKP*
 * add pairrapidity
 *
 * Revision 1.16  2001/02/15 19:23:00  rcwells
 * Fixed sign in qSideCMS
 *
 * Revision 1.15  2001/01/22 22:56:41  laue
 * Yano-Koonin-Podgoretskii Parametrisation added
 *
 * Revision 1.14  2000/12/11 21:44:30  rcwells
 * Corrected qSideCMS function
 *
 * Revision 1.13  2000/10/26 16:09:16  lisa
 * Added OpeningAngle PairCut class and method to StHbtPair
 *
 * Revision 1.12  2000/10/05 23:09:05  lisa
 * Added kT-dependent radii to mixed-event simulator AND implemented AverageSeparation Cut and CorrFctn
 *
 * Revision 1.11  2000/07/17 20:03:16  lisa
 * Implemented tools for addressing and assessing trackmerging
 *
 * Revision 1.10  2000/04/04 16:27:03  rcwells
 * Removed an errant cout in StHbtPair.cc
 *
 * Revision 1.9  2000/04/04 16:13:09  lisa
 * StHbtPair:quality() now returns normalized value (and so is double) and add a CorrFctn which looks at quality()
 *
 * Revision 1.8  2000/04/03 22:09:12  rcwells
 * Add member function ... quality().
 *
 * Revision 1.7  2000/02/13 21:13:33  lisa
 * changed ambiguous StHbtPair::fourMomentum() to fourMomentumSum() and fourMomentumDiff() and fixed related bug in QvecCorrFctn
 *
 * Revision 1.6  1999/07/29 16:16:34  lisa
 * Selemons upgrade of StHbtPair class
 *
 * Revision 1.5  1999/07/22 18:49:10  lisa
 * Implement idea of Fabrice to not create and delete StHbtPair all the time
 *
 * Revision 1.4  1999/07/12 18:57:05  lisa
 * fixed small bug in fourMomentum method of StHbtPair
 *
 * Revision 1.3  1999/07/06 22:33:22  lisa
 * Adjusted all to work in pro and new - dev itself is broken
 *
 * Revision 1.2  1999/06/29 17:50:27  fisyak
 * formal changes to account new StEvent, does not complie yet
 *
 * Revision 1.1.1.1  1999/06/29 16:02:57  lisa
 * Installation of StHbtMaker
 *
 **************************************************************************/

#include "StHbtMaker/Infrastructure/StHbtPair.hh"

StHbtPair::StHbtPair(){
  mTrack1 = 0;
  mTrack2 = 0;
}

StHbtPair::StHbtPair(StHbtParticle* a, StHbtParticle* b)
  : mTrack1(a), mTrack2(b)
{ }


StHbtPair::~StHbtPair() {/* no-op */}

//StHbtPair::StHbtPair(const StHbtPair &a) {/* missing */}

//StHbtPair& StHbtPair::operator=(const StHbtPair &a)

//_________________
double StHbtPair::qInv() const
{
    double dq = abs(mTrack1->FourMomentum() - mTrack2->FourMomentum());
    return (dq);
}
//_________________
double StHbtPair::mInv() const
{
    double InvariantMass = abs(mTrack1->FourMomentum() + mTrack2->FourMomentum());
    return (InvariantMass);
}
//_________________
double StHbtPair::kT() const
{

  double  tmp = 
    (mTrack1->FourMomentum() + mTrack2->FourMomentum()).perp();
  tmp *= .5;

  return (tmp);
}
//_________________
double StHbtPair::rap() const
{
  // longitudinal pair rapidity : Y = 0.5 log( E1 + E2 + pz1 + pz2 / E1 + E2 - pz1 - pz2 )
  double  tmp = 0.5 * log (
			   (mTrack1->FourMomentum().e() + mTrack2->FourMomentum().e() + mTrack1->FourMomentum().z() + mTrack2->FourMomentum().z()) / 
			   (mTrack1->FourMomentum().e() + mTrack2->FourMomentum().e() - mTrack1->FourMomentum().z() - mTrack2->FourMomentum().z()) 
			   ) ;
  return (tmp);
}
//_________________
// get rid of ambiguously-named method fourMomentum() and replace it with
// fourMomentumSum() and fourMomentumDiff() - mal 13feb2000
StHbtLorentzVector StHbtPair::fourMomentumSum() const
{
  StHbtLorentzVector temp = mTrack1->FourMomentum()+mTrack2->FourMomentum();
  return temp;
}
StHbtLorentzVector StHbtPair::fourMomentumDiff() const
{
  StHbtLorentzVector temp = mTrack1->FourMomentum()-mTrack2->FourMomentum();
  return temp;
}
//__________________________________
// Yano-Koonin-Podgoretskii Parametrisation in CMS
void StHbtPair::qYKPCMS(double& qP, double& qT, double& q0) const
{
  ////
  // calculate momentum difference in source rest frame (= lab frame)
  ////
  StHbtLorentzVector l1 = mTrack1->FourMomentum() ;
  StHbtLorentzVector l2 = mTrack2->FourMomentum() ;
  StHbtLorentzVector  l ;
  // random ordering of the particles
  if ( rand()/(double)RAND_MAX > 0.50 )  
    { l = l1-l2 ; } 
  else 
    { l = l2-l1 ; } ;
  // fill momentum differences into return variables
  qP = l.z() ;
  qT = l.vect().perp() ;
  q0 = l.e() ;
}
//___________________________________
// Yano-Koonin-Podgoretskii Parametrisation in LCMS
void StHbtPair::qYKPLCMS(double& qP, double& qT, double& q0) const
{
  ////
  //  calculate momentum difference in LCMS : frame where pz1 + pz2 = 0
  ////
  StHbtLorentzVector l1 = mTrack1->FourMomentum() ;
  StHbtLorentzVector l2 = mTrack2->FourMomentum() ;
  // determine beta to LCMS
  double beta = (l1.z()+l2.z()) / (l1.e()+l2.e()) ;
  double beta2 =  beta*beta ;
  // unfortunately STAR Class lib knows only boost(particle) not boost(beta) :(
  // -> create particle with velocity beta and mass 1.0
  // actually this is : dummyPz = sqrt( (dummyMass*dummyMass*beta2) / (1-beta2) ) ; 
  double dummyPz = sqrt( (beta2) / (1-beta2) ) ;
  // boost in the correct direction
  if (beta>0.0) { dummyPz = -dummyPz; } ;
  // create dummy particle
  StHbtLorentzVector  l(0.0, 0.0, dummyPz) ; 
  double dummyMass = 1.0 ;
  l.setE(l.vect().massHypothesis(dummyMass) );
  // boost particles along the beam into a frame with velocity beta 
  StHbtLorentzVector l1boosted = l1.boost(l) ;
  StHbtLorentzVector l2boosted = l2.boost(l) ;
  // caculate the momentum difference with random ordering of the particle
  if ( rand()/(double)RAND_MAX >0.50)  
    { l = l1boosted-l2boosted ; } 
  else 
    { l = l2boosted-l1boosted ;} ;
  // fill momentum differences into return variables
  qP = l.z() ;
  qT = l.vect().perp() ;
  q0 = l.e() ;
}
//___________________________________
// Yano-Koonin-Podgoretskii Parametrisation in pair rest frame
void StHbtPair::qYKPPF(double& qP, double& qT, double& q0) const
{
  ////
  //  calculate momentum difference in pair rest frame : frame where (pz1 + pz2, py1 + py2, px1 + px2) = (0,0,0)
  ////
  StHbtLorentzVector l1 = mTrack1->FourMomentum() ;
  StHbtLorentzVector l2 = mTrack2->FourMomentum() ;
  // the center of gravity of the pair travels with l
  StHbtLorentzVector  l = l1 + l2 ; 
  l = -l ;
  l.setE(-l.e()) ;
  // boost particles  
  StHbtLorentzVector l1boosted = l1.boost(l) ;
  StHbtLorentzVector l2boosted = l2.boost(l) ;
  // caculate the momentum difference with random ordering of the particle
  if ( rand()/(double)RAND_MAX > 0.50)  
    { l = l1boosted-l2boosted ; } 
  else 
    { l = l2boosted-l1boosted ;} ;
  // fill momentum differences into return variables
  qP = l.z();
  qT = l.vect().perp();
  q0 = l.e();
}
//_________________
double StHbtPair::qOutCMS() const
{
    StHbtThreeVector tmp1 = mTrack1->FourMomentum().vect();
    StHbtThreeVector tmp2 = mTrack2->FourMomentum().vect();

    double dx = tmp1.x() - tmp2.x();
    double xt = tmp1.x() + tmp2.x();
    
    double dy = tmp1.y() - tmp2.y();
    double yt = tmp1.y() + tmp2.y();

    double k1 = (sqrt(xt*xt+yt*yt));
    double k2 = (dx*xt+dy*yt);
    double tmp = k2/k1;
    return (tmp);
}
//_________________
double StHbtPair::qSideCMS() const
{
    StHbtThreeVector tmp1 = mTrack1->FourMomentum().vect();
    StHbtThreeVector tmp2 = mTrack2->FourMomentum().vect();

    double x1 = tmp1.x();  double y1 = tmp1.y();
    double x2 = tmp2.x();  double y2 = tmp2.y();

    double xt = x1+x2;  double yt = y1+y2;
    double k1 = sqrt(xt*xt+yt*yt);

    double tmp = 2.0*(x2*y1-x1*y2)/k1;
    return (tmp);
}

//_________________________
double StHbtPair::qLongCMS() const
{
    StHbtLorentzVector tmp1 = mTrack1->FourMomentum();
    StHbtLorentzVector tmp2 = mTrack2->FourMomentum();

    double dz = tmp1.z() - tmp2.z();
    double zz = tmp1.z() + tmp2.z();

    double dt = tmp1.t() - tmp2.t();
    double tt = tmp1.t() + tmp2.t();

    double beta = zz/tt;
    double gamma = 1.0/sqrt(1.0 - beta*beta);

    double temp = gamma*(dz - beta*dt);
    return (temp);
}

//________________________________
double StHbtPair::qOutPf() const
{
 StHbtLorentzVector tmp1 = mTrack1->FourMomentum();
 StHbtLorentzVector tmp2 = mTrack2->FourMomentum();

    double dt = tmp1.t() - tmp2.t();
    double tt = tmp1.t() + tmp2.t();

    double xt = tmp1.x() + tmp2.x();
    double yt = tmp1.y() + tmp2.y();

    double k1 = sqrt(xt*xt + yt*yt);
    double bOut = k1/tt;
    double gOut = 1.0/sqrt(1.0 - bOut*bOut);

    double temp = gOut*(this->qOutCMS() - bOut*dt);
    return (temp);
}

//___________________________________
double StHbtPair::qSidePf() const
{
 return(this->qSideCMS());
}

//___________________________________

double StHbtPair::qLongPf() const
{
 return(this->qLongCMS());
}

//___________________________________
double StHbtPair::qOutBf(double beta) const
{
 return(this->qOutCMS());
}

//___________________________________

double StHbtPair::qSideBf(double beta) const
{
 return(this->qSideCMS());
}

//___________________________________
double StHbtPair::qLongBf(double beta) const
{
    StHbtLorentzVector tmp1 = mTrack1->FourMomentum();
    StHbtLorentzVector tmp2 = mTrack2->FourMomentum();

    double dz = tmp1.z() - tmp2.z();
    double dt = tmp1.t() + tmp2.t();

    double gamma = 1.0/sqrt(1.0 - beta*beta);

    double temp = gamma*(dz - beta*dt);
    return (temp);
}

double StHbtPair::quality() const {
  unsigned long mapMask0 = 0xFFFFFF00;
  unsigned long mapMask1 = 0x1FFFFF;
  unsigned long padRow1To24Track1 = mTrack1->TopologyMap(0) & mapMask0;
  unsigned long padRow25To45Track1 = mTrack1->TopologyMap(1) & mapMask1;
  unsigned long padRow1To24Track2 = mTrack2->TopologyMap(0) & mapMask0;
  unsigned long padRow25To45Track2 = mTrack2->TopologyMap(1) & mapMask1;
  // AND logic
  unsigned long bothPads1To24 = padRow1To24Track1 & padRow1To24Track2;
  unsigned long bothPads25To45 = padRow25To45Track1 & padRow25To45Track2;
  // XOR logic
  unsigned long onePad1To24 = padRow1To24Track1 ^ padRow1To24Track2;
  unsigned long onePad25To45 = padRow25To45Track1 ^ padRow25To45Track2;
  unsigned long bitI;
  int ibits;
  int Quality = 0;
  double normQual = 0.0;
  int MaxQuality = mTrack1->NumberOfHits() + mTrack2->NumberOfHits();
  for (ibits=8;ibits<=31;ibits++) {
    bitI = 0;
    bitI |= 1UL<<(ibits);
    if ( onePad1To24 & bitI ) {
      Quality++;
      continue;
    }
    else{
      if ( bothPads1To24 & bitI ) Quality--;
    }
  }
  for (ibits=0;ibits<=20;ibits++) {
    bitI = 0;
    bitI |= 1UL<<(ibits);
    if ( onePad25To45 & bitI ) {
      Quality++;
      continue;
    }
    else{
      if ( bothPads25To45 & bitI ) Quality--;
    }
  }
  normQual = (double)Quality/( (double) MaxQuality );
  return ( normQual );

}

double StHbtPair::NominalTpcExitSeparation() const {
  StHbtThreeVector diff = mTrack1->NominalTpcExitPoint() - mTrack2->NominalTpcExitPoint();
  return (diff.mag());
}

double StHbtPair::NominalTpcEntranceSeparation() const {
  StHbtThreeVector diff = mTrack1->NominalTpcEntrancePoint() - mTrack2->NominalTpcEntrancePoint();
  return (diff.mag());
}

double StHbtPair::NominalTpcAverageSeparation() const {
  StHbtThreeVector diff;
  double AveSep = 0.0;
  for (int ipt=0; ipt<11; ipt++){
    diff = mTrack1->mNominalPosSample[ipt] - mTrack2->mNominalPosSample[ipt];
    AveSep += diff.mag();
  }
  AveSep = AveSep/11.0;
  return (AveSep);
}

double StHbtPair::OpeningAngle() const {
  StHbtThreeVector p1 = mTrack1->FourMomentum().vect();
  StHbtThreeVector p2 = mTrack2->FourMomentum().vect();
  double dAngInv = 57.296*acos((p1.dot(p2))/(p1.mag()*p2.mag()));
  return (dAngInv);
}

