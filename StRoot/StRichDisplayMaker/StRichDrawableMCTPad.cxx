/***************************************************************
 * $Id: StRichDrawableMCTPad.cxx,v 2.0 2000/08/09 16:28:02 gans Exp $
 *
 * Description:
 *
 ***************************************************************
 * $Log: StRichDrawableMCTPad.cxx,v $
 * Revision 2.0  2000/08/09 16:28:02  gans
 * Created New Maker for all drawable objects.
 *
 * Revision 1.2  2000/05/17 22:18:40  lasiuk
 * use charge() instead of adc()/amp()
 *
 * Revision 1.1  2000/04/05 15:55:03  lasiuk
 * Initial Revision
 *
 ***************************************************************/
#ifdef __ROOT__

#include "StRichDrawableMCTPad.h"
#include "StRrsMaker/StRichEnumeratedTypes.h"

ClassImp(StRichDrawableMCTPad)

StRichDrawableMCTPad::StRichDrawableMCTPad() {/*nopt*/}

StRichDrawableMCTPad::StRichDrawableMCTPad(double xl, double yl, double xu, double yu,
					   const StRichSingleMCPixel* mcPix)
    : StRichDrawableTPad(xl,yl,xu,yu,mcPix),
      mGID1(-1), mTrackp1(-1), mQ1(-1),mType1(eUnknown),
      mGID2(-1), mTrackp2(-1), mQ2(-1),mType2(eUnknown)
{
    anIDList::const_iterator iter;
    iter  = mcPix->MCInfo().begin();
    bool status = false;
    if(mcPix->MCInfo().size()>1) {
	status = true;
    }
    mGID1    = iter->mG_ID;
    mTrackp1 = iter->mTrackp;
    mQ1      = iter->mCharge;
    mType1   = iter->mSignalType;

    if(status) {
	iter++;
	mGID2    = iter->mG_ID;
	mTrackp2 = iter->mTrackp;
	mQ2      = iter->mCharge;
	mType2   = iter->mSignalType;
    }
}

StRichDrawableMCTPad::~StRichDrawableMCTPad()
{
//     delete [] mGID;
}

#endif /* ROOT */
