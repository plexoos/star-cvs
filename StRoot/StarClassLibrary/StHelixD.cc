/***************************************************************************
 *
 * $Id: StHelixD.cc,v 1.11 2003/06/26 17:15:56 ullrich Exp $
 *
 * Author: Thomas Ullrich, Jan 1999
 ***************************************************************************
 *
 * Description:
 *
 * Remarks:   This is a 'handmade' specialisation of StHelix
 *            with StThreeVector<T> replaced by StThreeVectorD
 *            and pair<T, T> replaced by pairD.
 *            This code contains no templates.
 *
 ***************************************************************************
 *
 * $Log: StHelixD.cc,v $
 * Revision 1.11  2003/06/26 17:15:56  ullrich
 * Changed local variable name in pathLenght.
 *
 * Revision 1.10  2002/06/21 17:49:25  genevb
 * Some minor speed improvements
 *
 * Revision 1.9  2000/07/17 21:44:24  ullrich
 * Fixed problem in pathLength(cyl_rad).
 *
 * Revision 1.8  2000/05/22 21:38:32  ullrich
 * Add parenthesis to make Linux compiler happy.
 *
 * Revision 1.7  2000/05/22 21:14:40  ullrich
 * In pathLength(StThreeVector&): Increased number of max iteration
 * in Newton method from 10 to 100. Improved initial guess in case
 * it is off by n period.
 *
 * Revision 1.6  2000/03/06 20:24:28  ullrich
 * Parameter h for case B=0 correctly handled now.
 *
 * Revision 1.5  1999/12/22 15:14:42  ullrich
 * Added analytical solution for dca between two helices
 * in the case for B=0.
 *
 * Revision 1.4  1999/08/18 12:37:57  ullrich
 * Fixed bug in pathLength(double). Swap of first and second
 * wasn't done correctly.
 *
 * Revision 1.3  1999/03/07 14:56:28  wenaus
 * fix scope problem
 *
 * Revision 1.2  1999/03/02 19:47:40  ullrich
 * Added method to find dca between two helices
 *
 * Revision 1.1  1999/01/30 03:59:02  fisyak
 * Root Version of StarClassLibrary
 *
 * Revision 1.1  1999/01/23 00:29:16  ullrich
 * Initial Revision
 *
 **************************************************************************/
#include <float.h>
#include "StHelixD.hh"
#include "PhysicalConstants.h" 
#include "SystemOfUnits.h"

#ifdef __ROOT__
ClassImp(StHelixD)
#endif

StHelixD::StHelixD()
{
    setParameters(0, 0, 0, StThreeVectorD(), -1);
}

StHelixD::StHelixD(double c, double d, double phase,
		   const StThreeVectorD& o, int h)
{
    setParameters(c, d, phase, o, h);
}

StHelixD::~StHelixD() { /* nop */ };

void StHelixD::setParameters(double c, double dip, double phase,
			    const StThreeVectorD& o, int h)
{
    //
    //  The order in which the parameters are set is important
    //  since setCurvature might have to adjust the others.
    //
    mH = (h>=0) ? 1 : -1;    // Default is: positive particle
                             //             positive field
    mOrigin   = o;
    setDipAngle(dip);
    setPhase(phase);

    //
    // Check for singularity and correct for negative curvature.           
    // May change mH and mPhase. Must therefore be set last.
    //
    setCurvature(c);         

    //
    // For the case B=0, h is ill defined. In the following we
    // always assume h = +1. Since phase = psi - h * pi/2
    // we have to correct the phase in case h = -1.
    // This assumes that the user uses the same h for phase
    // as the one he passed to the constructor.
    //
    if (mSingularity && mH == -1) {
	mH = +1;
	setPhase(mPhase-M_PI);
    }
}

void StHelixD::setCurvature(double val)
{
    if (val < 0) {
	mCurvature = -val;
	mH = -mH;
	setPhase(mPhase+M_PI);
    }
    else
	mCurvature = val;

    if (fabs(mCurvature) <= static_cast<double>(0))    
	mSingularity = 1;		// straight line
    else
	mSingularity = 0;          	// curved
}

void StHelixD::setPhase(double val)
{
    mPhase       = val;
    mCosPhase    = cos(mPhase);
    mSinPhase    = sin(mPhase);
    if (fabs(mPhase) > M_PI)
	mPhase = atan2(mSinPhase, mCosPhase);  // force range [-pi,pi]
}

void StHelixD::setDipAngle(double val)
{
    mDipAngle    = val;
    mCosDipAngle = cos(mDipAngle);
    mSinDipAngle = sin(mDipAngle);
}

double StHelixD::xcenter() const
{
    if (mSingularity)
	return 0;
    else
	return mOrigin.x()-mCosPhase/mCurvature;
}

double StHelixD::ycenter() const
{
    if (mSingularity)
	return 0;
    else
	return mOrigin.y()-mSinPhase/mCurvature;
}

double StHelixD::fudgePathLength(const StThreeVectorD& p) const
{
    double s;
    double dx = p.x()-mOrigin.x();
    double dy = p.y()-mOrigin.y();
    
    if (mSingularity) {
	s = (dy*mCosPhase - dx*mSinPhase)/mCosDipAngle;
    }
    else {
	s = atan2(dy*mCosPhase - dx*mSinPhase,
		  1/mCurvature + dx*mCosPhase+dy*mSinPhase)/
	    (mH*mCurvature*mCosDipAngle);
    }
    return s;
}

double StHelixD::distance(const StThreeVectorD& p) const
{
    return abs(this->at(pathLength(p))-p);
}

double StHelixD::pathLength(const StThreeVectorD& p) const 
{
    //
    //  Returns the path length at the distance of closest 
    //  approach between the helix and point p. 
    //  For the case of B=0 (straight line) the path length
    //  can be calculated analytically. For B>0 there is
    //  unfortunately no easy solution to the problem.
    //  Here we use the Newton method to find the root of the
    //  referring equation. The 'fudgePathLength' serves
    //  as a starting value.
    //
    double s;
    double dx = p.x()-mOrigin.x();
    double dy = p.y()-mOrigin.y();
    double dz = p.z()-mOrigin.z();

    if (mSingularity) {
	s = mCosDipAngle*(mCosPhase*dy-mSinPhase*dx) +
	    mSinDipAngle*dz;
    }
    else { //
#ifndef ST_NO_NAMESPACES
	{
	    using namespace units;
#endif
	    const double MaxPrecisionNeeded = micrometer;
	    const int    MaxIterations      = 100;

	    //
	    // The math is taken from Maple with C(expr,optimized) and
	    // some hand-editing. It is not very nice but efficient.
	    //
	    double t34 = mCurvature*mCosDipAngle*mCosDipAngle;
	    double t41 = mSinDipAngle*mSinDipAngle;
	    double t6, t7, t11, t12, t19;
	    
	    //
	    // Get a first guess by using the dca in 2D. Since
	    // in some extreme cases we might be off by n periods
	    // we add (subtract) periods in case we get any closer.
	    // 
	    s = fudgePathLength(p);

	    double ds = period();
	    int    j, jmin = 0;
	    double d, dmin = abs(at(s) - p);
	    for(j=1; j<MaxIterations; j++) {
		if ((d = abs(at(s+j*ds) - p)) < dmin) {
		    dmin = d;
		    jmin = j;
		}
		else
		    break;
	    }
	    for(j=-1; -j<MaxIterations; j--) {
		if ((d = abs(at(s+j*ds) - p)) < dmin) {
		    dmin = d;
		    jmin = j;
		}
		else
		    break;
	    }
	    if (jmin) s += jmin*ds;

	    //
	    // Newtons method:
	    // Stops after MaxIterations iterations or if the required
	    // precision is obtained. Whatever comes first.
	    //
	    double sOld = s;
	    for (int i=0; i<MaxIterations; i++) {
		t6  = mPhase+s*mH*mCurvature*mCosDipAngle;
		t7  = cos(t6);
		t11 = dx-(1/mCurvature)*(t7-mCosPhase);
		t12 = sin(t6);
		t19 = dy-(1/mCurvature)*(t12-mSinPhase);
		s  -= (t11*t12*mH*mCosDipAngle-t19*t7*mH*mCosDipAngle -
		       (dz-s*mSinDipAngle)*mSinDipAngle)/
		    (t12*t12*mCosDipAngle*mCosDipAngle+t11*t7*t34 +
		     t7*t7*mCosDipAngle*mCosDipAngle +
		     t19*t12*t34+t41);
		if (fabs(sOld-s) < MaxPrecisionNeeded) break;
		sOld = s;
	    }
#ifndef ST_NO_NAMESPACES
	}
#endif
    }
    return s;
}

double StHelixD::period() const
{
    if (mSingularity)
	return DBL_MAX;
    else	
	return fabs(2*M_PI/(mH*mCurvature*mCosDipAngle)); 
}

pairD StHelixD::pathLength(double r) const
{
    pairD value;
    //
    // The math is taken from Maple with C(expr,optimized) and
    // some hand-editing. It is not very nice but efficient.
    // 'first' is the smallest of the two solutions (may be negative)
    // 'second' is the other.
    //
    if (mSingularity) {
	double t1 = mCosDipAngle*(mOrigin.x()*mSinPhase-mOrigin.y()*mCosPhase);
	double t12 = mOrigin.y()*mOrigin.y();
	double t13 = mCosPhase*mCosPhase;
	double t15 = r*r;
	double t16 = mOrigin.x()*mOrigin.x();
	double t20 = sqrt(-mCosDipAngle*mCosDipAngle*(2.0*mOrigin.x()*mSinPhase*mOrigin.y()*mCosPhase +
				 t12-t12*t13-t15+t13*t16));
	value.first  = (t1-t20)/(mCosDipAngle*mCosDipAngle);
	value.second = (t1+t20)/(mCosDipAngle*mCosDipAngle);
    }
    else {
	double t1 = mOrigin.y()*mCurvature;
	double t2 = mSinPhase;
	double t3 = mCurvature*mCurvature;
	double t4 = mOrigin.y()*t2;
	double t5 = mCosPhase;
	double t6 = mOrigin.x()*t5;
	double t8 = mOrigin.x()*mOrigin.x();
	double t11 = mOrigin.y()*mOrigin.y();
	double t14 = r*r;
	double t15 = t14*mCurvature;
	double t17 = t8*t8;
	double t19 = t11*t11;
	double t21 = t11*t3;
	double t23 = t5*t5;
	double t32 = t14*t14;
	double t35 = t14*t3;
	double t38 = 8.0*t4*t6 - 4.0*t1*t2*t8 - 4.0*t11*mCurvature*t6 +
	             4.0*t15*t6 + t17*t3 + t19*t3 + 2.0*t21*t8 + 4.0*t8*t23 -
	             4.0*t8*mOrigin.x()*mCurvature*t5 - 4.0*t11*t23 -
	             4.0*t11*mOrigin.y()*mCurvature*t2 + 4.0*t11 - 4.0*t14 +
	             t32*t3 + 4.0*t15*t4 - 2.0*t35*t11 - 2.0*t35*t8;
	double t40 = sqrt(-t3*t38);
	double t43 = mOrigin.x()*mCurvature;
	double t45 = 2.0*t5 - t35 + t21 + 2.0 - 2.0*t1*t2 -2.0*t43 - 2.0*t43*t5 + t8*t3;
	double t46 = mH*mCosDipAngle*mCurvature;
	
	value.first = (-mPhase + 2.0*atan((-2.0*t1 + 2.0*t2 + t40)/t45))/t46;
	value.second = -(mPhase + 2.0*atan((2.0*t1 - 2.0*t2 + t40)/t45))/t46;

	//
	//   Solution can be off by +/- one period, select smallest
	//
	double p = period();
	if (!isnan(value.first)) {
	    if (fabs(value.first-p) < fabs(value.first)) value.first = value.first-p;
	    else if (fabs(value.first+p) < fabs(value.first)) value.first = value.first+p;
	}
	if (!isnan(value.second)) {
	    if (fabs(value.second-p) < fabs(value.second)) value.second = value.second-p;
	    else if (fabs(value.second+p) < fabs(value.second)) value.second = value.second+p;
	}
    }
    if (value.first > value.second) {
	double tmp = value.first;
	value.first = value.second;
	value.second = tmp;
    }
    return(value);
}

pairD StHelixD::pathLength(double r, double x, double y) 
{   
    double x0 = mOrigin.x();
    double y0 = mOrigin.y();
    mOrigin.setX(x0-x);
    mOrigin.setY(y0-y);
    pairD result = this->pathLength(r);
    mOrigin.setX(x0);
    mOrigin.setY(y0);
    return result;
}

double StHelixD::pathLength(const StThreeVectorD& r,
		           const StThreeVectorD& n) const
{
    //
    // Vector 'r' defines the position of the center and
    // vector 'n' the normal vector of the plane.
    // For a straight line there is a simple analytical
    // solution. For curvatures > 0 the root is determined
    // by Newton method. In case no valid s can be found
    // the max. largest value for s is returned.
    //
    double s;
    const double NoSolution = DBL_MAX;

    if (mSingularity) {
	double t = n.z()*mSinDipAngle +
	           n.y()*mCosDipAngle*mCosPhase -
	           n.x()*mCosDipAngle*mSinPhase;
	if (t == 0)
	    s = NoSolution;
	else
	    s = (r*n - mOrigin*n)/t;
    }
    else {
        const double MaxPrecisionNeeded = micrometer;
        const int    MaxIterations      = 20;
	
	double A = mCurvature*(mOrigin*n - r*n) -
	           n.x()*mCosPhase - 
	           n.y()*mSinPhase;
	double t = mH*mCurvature*mCosDipAngle;
	
	double a, f, fp;
	double sOld = s = 0;  
    int i;
	for (i=0; i<MaxIterations; i++) {
	    a  = t*s+mPhase;
	    f  = A +
		 n.x()*cos(a) +
		 n.y()*sin(a) +
		 n.z()*mCurvature*mSinDipAngle*s;
	    fp = -n.x()*sin(a)*t +
		  n.y()*cos(a)*t +
		  n.z()*mCurvature*mSinDipAngle;
	    s -= f/fp;
	    if (fabs(sOld-s) < MaxPrecisionNeeded) break;
	    sOld = s;
	}
	if (i == MaxIterations) s = NoSolution;
    }
    return s;
}

pairD
StHelixD::pathLengths(const StHelixD& h) const
{
    const double NoSolution = DBL_MAX;

    //
    //	Cannot handle case where one is a helix
    //  and the other one is a straight line.
    //
    if (mSingularity != h.mSingularity) 
	return pairD(NoSolution, NoSolution);

    double s1, s2;

    if (mSingularity) {
	//
	//  Analytic solution
	//
	StThreeVectorD dv = h.mOrigin - mOrigin;
	StThreeVectorD a(-mCosDipAngle*mSinPhase,
			 mCosDipAngle*mCosPhase,
			 mSinDipAngle);
	StThreeVectorD b(-h.mCosDipAngle*h.mSinPhase,
			 h.mCosDipAngle*h.mCosPhase,
			 h.mSinDipAngle);	
	double ab = a*b;
	double g  = dv*a;
	double k  = dv*b;
	s2 = (k-ab*g)/(ab*ab-1.);
	s1 = g+s2*ab;
	return pairD(s1, s2);
    }
    else {	
	//
	//  First step: get dca in the xy-plane as start value
	//
	double dx = h.xcenter() - xcenter();
	double dy = h.ycenter() - ycenter();
	double dd = sqrt(dx*dx + dy*dy);
	double r1 = 1/curvature();
	double r2 = 1/h.curvature();
	
	double cosAlpha = (r1*r1 + dd*dd - r2*r2)/(2*r1*dd);
	
	double s;
	double x, y;
	if (fabs(cosAlpha) < 1) {           // two solutions
	    double sinAlpha = sin(acos(cosAlpha));
	    x = xcenter() + r1*(cosAlpha*dx - sinAlpha*dy)/dd;
	    y = ycenter() + r1*(sinAlpha*dx + cosAlpha*dy)/dd;
	    s = pathLength(x, y);
	    x = xcenter() + r1*(cosAlpha*dx + sinAlpha*dy)/dd;
	    y = ycenter() + r1*(cosAlpha*dy - sinAlpha*dx)/dd;
	    double a = pathLength(x, y);
	    if (h.distance(at(a)) < h.distance(at(s))) s = a;
	}
	else {                              // no intersection (or exactly one)
	    x = xcenter() + r1*dx/dd;
	    y = ycenter() + r1*dy/dd;
	    s = pathLength(x, y);
	}
	
	//
	//   Second step: scan in decreasing intervals around seed 's'
	// 
	const double MinStepSize = 10*micrometer;
	const double MinRange    = 10*centimeter;    
	double dmin              = h.distance(at(s));
	double range             = 2*dmin > MinRange ? 2*dmin : MinRange;
	s1                       = s - range/2.;
	s2                       = s + range/2.;
	double ds                = range/10;
	double slast, ss, d;
	
	while (ds > MinStepSize) {
	    for (ss=s1; ss<s2+ds; ss+=ds) {
		d = h.distance(at(ss));
		if (d < dmin) {
		    dmin = d;
		    s = ss;
		}
		slast = ss;
	    }
	    //
	    //  In the rare cases where the minimum is at the
	    //  the border of the current range we shift the range
	    //  and start all over, i.e we do not decrease 'ds'.
	    //  Else we decrease the search intervall around the
	    //  current minimum and redo the scan in smaller steps.
	    //
	    if (s == s1) {
		d = 0.8*(s2-s1);
		s1 -= d;
		s2 -= d;
	    }
	    else if (s == slast) {
		d = 0.8*(s2-s1);
		s1 += d;
		s2 += d;
	    }
	    else {           
		s1 = s-ds;
		s2 = s+ds;
		ds /= 10;
	    }
	}
	return pairD(s, h.pathLength(at(s)));
    }
}

int StHelixD::valid() const
{
    return (fabs(mDipAngle) != M_PI/2 &&
	    abs(mH) == 1              &&
	    mCurvature >= 0            );
}

void StHelixD::moveOrigin(double s)
{
    if (mSingularity)
	mOrigin	= at(s);
    else {
	StThreeVectorD newOrigin = at(s);
	double newPhase = atan2(newOrigin.y() - ycenter(),
				newOrigin.x() - xcenter());
	mOrigin = newOrigin;
	setPhase(newPhase);	        
    }
}

int operator== (const StHelixD& a, const StHelixD& b)
{
    //
    // Checks for numerical identity only !
    //
    return (a.origin()    == b.origin()    &&
	    a.dipAngle()  == b.dipAngle()  &&
	    a.curvature() == b.curvature() &&
	    a.phase()     == b.phase()     &&
	    a.h()         == b.h());
}

int operator!= (const StHelixD& a, const StHelixD& b) {return !(a == b);}

ostream& operator<<(ostream& os, const StHelixD& h)
{
    return os << '('
	      << "curvature = "  << h.curvature() << ", " 
	      << "dip angle = "  << h.dipAngle()  << ", "
	      << "phase = "      << h.phase()     << ", "  
	      << "h = "          << h.h()         << ", "    
	      << "origin = "     << h.origin()    << ')';
}
