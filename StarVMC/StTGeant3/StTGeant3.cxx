   
/**************************************************************************
 * Copyright(c) 1998-2003, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* 
$Log: StTGeant3.cxx,v $
Revision 1.3  2005/03/09 18:35:35  perev
Regular update

Revision 1.2  2004/07/22 18:09:19  perev
WITHROOT + WITHG3 + WITHBOTH are obsolete

Revision 1.25  2004/03/23 11:16:44  brun
From Ivana
With the previous changes by Andrei, all fixes by Ivana were lost.
This patch merges Ivana and Andrei versions.

Revision 1.23  2004/03/15 12:18:45  brun
From Andrei Gheata:
 - minor modifications to cope with geometry retreival from file:
 - ConstructGeometry does not need to be called
 - CloseGeometry not needed

Revision 1.22  2004/02/03 12:47:34  brun
>From Andrei Gheata:
StTGeant3:

- calls to gtonly return now always a true value (G3 is seeing an ONLY
geometry with TGeo)
- IsSameLocation inside gtnext not yet eliminated, but I am getting only
1 exception instead of 400 now (when the location really changes)

Revision 1.21  2004/01/28 18:05:24  brun
New version from Peter Hristov adding the graphics interface

Revision 1.20  2004/01/28 08:30:54  brun
Change the call to TRandom::RndmArray in function grndm

Revision 1.19  2004/01/28 08:14:48  brun
Add a CPP option STATISTICS to monitor the fequency of calls to the geometry functions.

Revision 1.18  2003/12/10 15:39:37  brun
iFollowing recent improvements by Andrei, replace in ggperp
the computation of normals:
    Double_t *dblnorm = gGeoManager->FindNormal(kFALSE);
with :
    Double_t *dblnorm = gGeoManager->FindNormalFast();

Revision 1.17  2003/12/10 10:32:09  brun
Add a protection in StTGeant3::Gsmate in case the material density is null

Revision 1.16  2003/12/01 23:51:22  brun
>From Andrei and Peter:
add a few missing cases when compiling with the WITHROOT option.

Revision 1.15  2003/11/28 09:44:15  brun
New version of StTGeant3 supporting the options WITHG3 and WITHROOT

Revision 1.14  2003/10/09 06:28:45  brun
In StTGeant3::ParticleName, increase size of local array name[20] to name[21]

Revision 1.13  2003/09/26 15:01:08  brun
>From Ivana;
- implemented new functions from TVirtualMC
  enabling user to define own particles and ions
  + getter functions::
    DefineParticle(..)
    DefineIon(..)
    ParticleName(..) const
    ParticleMass(..) const
    Double_t  ParticleCharge(..) const
    Double_t  ParticleLifeTime(..) const
    TMCParticleType ParticleMCType(..) const
- corrected charge in AddParticlesToPdgDataBase

Revision 1.12  2003/07/22 06:53:28  brun
This version does not yet support TGeo geometry.
TVirtualMC must be initialized with the 3rd argument set to kFALSE

Revision 1.11  2003/07/18 10:22:50  brun
Changes to reflect the equivalent changes in the abstract classes in vmc
(thanks Peter Hristov)

Revision 1.10  2003/07/16 07:40:09  brun
>From Andreas Morsch

- default g3 specific initialisation moved to StTGeant3::Init()
  (This avoids the cast to StTGeant3* in the Config.C)
- "CKOV" added to SetProcess

Revision 1.9  2003/06/03 21:26:46  brun
New version of gustep by Andreas Morsch

Revision 1.8  2003/02/28 10:41:49  brun
>From Andreas
 In DefineParticles(): rho0 decay channel corrected

Revision 1.7  2003/02/04 17:50:34  brun
>From Ivana
 In Mixture(): pass abs(nlmat) to CreateFloatArray calls
 as nlmat can be negative.

Revision 1.6  2003/01/31 18:23:06  brun
Ivana suggested corrections.
- corrected tau pdg code
- Warning if external decayer needed but not defined.

Revision 1.5  2003/01/23 11:34:04  brun
In gustep, replace 
   gMC->TrackPosition(x,y,z);
by
   fgGeant3->TrackPosition(x,y,z);

Revision 1.4  2003/01/06 17:20:52  brun
Add new functions TrackPosition and TrackMomentum as alternative to the original
functions filling a TLorentzVector object.
Use these new functions in gustep and gudcay.
This makes a 25 per cent speed improvement in case of Alice.

Revision 1.3  2002/12/10 07:58:36  brun
Update by Federico for the calls to Grndm

Revision 1.2  2002/12/06 16:50:30  brun
>From Federico:
the following modifications provide an >6% improvement in speed for
AliRoot.

Revision 1.1.1.1  2002/07/24 15:56:26  rdm
initial import into CVS

Revision 1.5  2002/07/10 09:33:19  hristov
Array with variable size created by new

Revision 1.4  2002/07/10 08:38:54  alibrary
Cleanup of code  

*/

///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//  Interface Class to the Geant3.21 MonteCarlo                              //
//                                                                           //
//                                                                           //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

#include <ctype.h>
#include <stdlib.h>

#include "TROOT.h" 
#include "TClonesArray.h"
#include "TParticle.h"
#include "TDatabasePDG.h"
#include "TLorentzVector.h"
#include "TArrayI.h"

#include "StTGeant3.h" 

#include "TCallf77.h" 
#include "TVirtualMCDecayer.h" 
#include "TPDGCode.h" 

#include "TGeoManager.h" 
#include "TGeoMCGeometry.h"

#include "THIGZ.h"
#include "StTGeant3gu.h"
#include "G3Bridge.h"
#include "G3Z.h"

#ifndef WIN32 
# define g3zebra  g3zebra_ 
# define grfile  grfile_ 
# define g3pcxyz  g3pcxyz_ 
# define g3gclos  g3gclos_ 
# define g3last   g3last_ 
# define g3init   g3init_ 
# define g3cinit  g3cinit_ 
# define g3run    g3run_ 
# define g3trig   g3trig_ 
# define g3trigc  g3trigc_ 
# define g3trigi  g3trigi_ 
# define g3work   g3work_ 
# define g3zinit  g3zinit_ 
# define g3fmate  g3fmate_ 
# define g3fpart  g3fpart_ 
# define g3ftmed  g3ftmed_ 
# define g3ftmat  g3ftmat_ 
# define g3mate   g3mate_ 
# define g3part   g3part_ 
# define g3sdk    g3sdk_ 
# define g3smate  g3smate_ 
# define g3smixt  g3smixt_ 
# define g3spart  g3spart_ 
# define g3stmed  g3stmed_ 
# define g3sckov  g3sckov_
# define g3stpar  g3stpar_ 
# define g3fkine  g3fkine_ 
# define g3fvert  g3fvert_ 
# define g3skine  g3skine_ 
# define g3svert  g3svert_ 
# define g3physi  g3physi_ 
# define g3debug  g3debug_ 
# define g3ekbin  g3ekbin_ 
# define g3finds  g3finds_ 
# define g3sking  g3sking_ 
# define g3skpho  g3skpho_ 
# define g3sstak  g3sstak_ 
# define g3sxyz   g3sxyz_ 
# define g3many   g3many_ 
# define g3track  g3track_ 
# define g3treve  g3treve_ 
# define gtreveroot  gtreveroot_ 
# define grndm   grndm_ 
# define grndmq  grndmq_ 
# define g3dtom   g3dtom_ 
# define g3lmoth  g3lmoth_ 
# define g3media  g3media_ 
# define g3mtod   g3mtod_ 
# define g3sdvn   g3sdvn_ 
# define g3sdvn2  g3sdvn2_ 
# define g3sdvs   g3sdvs_ 
# define g3sdvs2  g3sdvs2_ 
# define g3sdvt   g3sdvt_ 
# define g3sdvt2  g3sdvt2_
# define g3sord   g3sord_ 
# define g3spos   g3spos_ 
# define g3sposp  g3sposp_ 
# define g3srotm  g3srotm_ 
# define g3protm  g3protm_ 
# define g3svolu  g3svolu_ 
# define g3print  g3print_ 
# define gdinit  gdinit_ 
# define gdopt   gdopt_ 
# define gdraw   gdraw_ 
# define gdrayt  gdrayt_
# define gdrawc  gdrawc_ 
# define gdrawx  gdrawx_ 
# define gdhead  gdhead_ 
# define gdwmn1  gdwmn1_ 
# define gdwmn2  gdwmn2_ 
# define gdwmn3  gdwmn3_ 
# define gdxyz   gdxyz_ 
# define gdcxyz  gdcxyz_ 
# define gdman   gdman_ 
# define gdspec  gdspec_ 
# define gdtree  gdtree_ 
# define gdelet  gdelet_ 
# define gdclos  gdclos_ 
# define gdshow  gdshow_ 
# define gdopen  gdopen_ 
# define dzshow  dzshow_ 
# define g3satt   g3satt_ 
# define g3fpara  g3fpara_
# define gckpar  gckpar_
# define g3ckmat  g3ckmat_
# define g3lvolu  g3lvolu_
# define geditv  geditv_
# define mzdrop  mzdrop_

# define ertrak  ertrak_
# define ertrgo  ertrgo_
 
# define setbomb setbomb_
# define setclip setclip_
# define gcomad gcomad_

# define g3brelm g3brelm_
# define g3prelm g3prelm_

# define rxgtrak rxgtrak_
# define rxouth  rxouth_
# define rxinh   rxinh_

// From gdraw
# define g3dinit  g3dinit_ 
# define g3dopt   g3dopt_ 
# define g3draw   g3draw_ 
# define g3drayt  g3drayt_
# define g3drawc  g3drawc_ 
# define g3drawx  g3drawx_ 
# define g3dhead  g3dhead_ 
# define g3dwmn1  g3dwmn1_ 
# define g3dwmn2  g3dwmn2_ 
# define g3dwmn3  g3dwmn3_ 
# define g3dxyz   g3dxyz_ 
# define g3dcxyz  g3dcxyz_ 
# define g3dman   g3dman_ 
# define g3dspec  g3dspec_ 
# define g3dtree  g3dtree_ 
# define g3delet  g3delet_ 
# define g3dclos  g3dclos_ 
# define g3dshow  g3dshow_ 
# define g3dopen  g3dopen_ 

#else 

# define gzebra  GZEBRA 
# define grfile  GRFILE 
# define gpcxyz  GPCXYZ 
# define ggclos  GGCLOS 
# define glast   GLAST 
# define ginit   GINIT 
# define g3cinit  G3CINIT 
# define grun    GRUN 
# define gtrig   GTRIG 
# define gtrigc  GTRIGC 
# define gtrigi  GTRIGI 
# define gwork   GWORK 
# define g3zinit  G3ZINIT 
# define gfmate  GFMATE 
# define gfpart  GFPART 
# define gftmed  GFTMED 
# define gftmat  GFTMAT
# define gmate   GMATE 
# define gpart   GPART 
# define gsdk    GSDK 
# define gsmate  GSMATE 
# define gsmixt  GSMIXT 
# define gspart  GSPART 
# define gstmed  GSTMED 
# define gsckov  GSCKOV
# define gstpar  GSTPAR 
# define gfkine  GFKINE 
# define gfvert  GFVERT 
# define gskine  GSKINE 
# define gsvert  GSVERT 
# define gphysi  GPHYSI 
# define gdebug  GDEBUG 
# define gekbin  GEKBIN 
# define gfinds  GFINDS 
# define gsking  GSKING 
# define gskpho  GSKPHO 
# define gsstak  GSSTAK 
# define gsxyz   GSXYZ 
# define gtrack  GTRACK 
# define gtreve  GTREVE 
# define gtreveroot  GTREVEROOT
# define grndm   GRNDM
# define grndmq  GRNDMQ
# define gdtom   GDTOM 
# define glmoth  GLMOTH 
# define gmedia  GMEDIA 
# define gmtod   GMTOD 
# define gsdvn   GSDVN 
# define gsdvn2  GSDVN2 
# define gsdvs   GSDVS 
# define gsdvs2  GSDVS2 
# define gsdvt   GSDVT 
# define gsdvt2  GSDVT2
# define gsord   GSORD 
# define gspos   GSPOS 
# define gsposp  GSPOSP 
# define gsrotm  GSROTM 
# define gprotm  GPROTM 
# define gsvolu  GSVOLU 
# define gprint  GPRINT 
# define gdinit  GDINIT
# define gdopt   GDOPT 
# define gdraw   GDRAW
# define gdrayt  GDRAYT
# define gdrawc  GDRAWC
# define gdrawx  GDRAWX 
# define gdhead  GDHEAD
# define gdwmn1  GDWMN1
# define gdwmn2  GDWMN2
# define gdwmn3  GDWMN3
# define gdxyz   GDXYZ
# define gdcxyz  GDCXYZ
# define gdman   GDMAN
# define gdfspc  GDFSPC
# define gdspec  GDSPEC
# define gdtree  GDTREE
# define gdelet  GDELET
# define gdclos  GDCLOS
# define gdshow  GDSHOW
# define gdopen  GDOPEN
# define dzshow  DZSHOW 
# define gsatt   GSATT 
# define gfpara  GFPARA
# define gckpar  GCKPAR
# define gckmat  GCKMAT
# define glvolu  GLVOLU
# define geditv  GEDITV
# define mzdrop  MZDROP 

# define ertrak  ERTRAK
# define ertrgo  ERTRGO
 
# define setbomb SETBOMB
# define setclip SETCLIP
# define gcomad  GCOMAD
 
# define gbrelm GBRELM
# define gprelm GPRELM

# define rxgtrak RXGTRAK 
# define rxouth  RXOUTH
# define rxinh   RXINH

// From gdraw
# define g3dinit  G3DINIT
# define g3dopt   G3DOPT
# define g3draw   G3DRAW
# define g3drayt  G3DRAYT
# define g3drawc  G3DRAWC
# define g3drawx  G3DRAWX
# define g3dhead  G3DHEAD
# define g3dwmn1  G3DWMN1
# define g3dwmn2  G3DWMN2
# define g3dwmn3  G3DWMN3
# define g3dxyz   G3DXYZ
# define g3dcxyz  G3DCXYZ
# define g3dman   G3DMAN
# define g3dspec  G3DSPEC
# define g3dtree  G3DTREE
# define g3delet  G3DELET
# define g3dclos  G3DCLOS
# define g3dshow  G3DSHOW
# define g3dopen  G3DOPEN

#endif 

//____________________________________________________________________________ 
extern "C" 
{
  //
  // Prototypes for GEANT functions
  //
  void type_of_call g3zebra(const int&); 

  void type_of_call g3pcxyz(); 

  void type_of_call g3gclos(); 

  void type_of_call g3last(); 

  void type_of_call g3init(); 

  void type_of_call g3cinit(); 

  void type_of_call g3run(); 

  void type_of_call g3trig(); 

  void type_of_call g3trigc(); 

  void type_of_call g3trigi(); 

  void type_of_call g3work(const int&); 

  void type_of_call g3zinit(); 

  void type_of_call g3mate(); 

  void type_of_call g3part(); 

  void type_of_call g3sdk(Int_t &, Float_t *, Int_t *); 

  void type_of_call g3fkine(Int_t &, Float_t *, Float_t *, Int_t &,
			   Int_t &, Float_t *, Int_t &); 

  void type_of_call g3fvert(Int_t &, Float_t *, Int_t &, Int_t &, 
			   Float_t &, Float_t *, Int_t &); 

  void type_of_call g3skine(Float_t *,Int_t &, Int_t &, Float_t *,
			   Int_t &, Int_t &); 

  void type_of_call g3svert(Float_t *,Int_t &, Int_t &, Float_t *,
			   Int_t &, Int_t &); 

  void type_of_call g3physi(); 

  void type_of_call g3debug(); 

  void type_of_call g3ekbin(); 

  void type_of_call g3finds(); 

  void type_of_call g3sking(Int_t &); 

  void type_of_call g3skpho(Int_t &); 

  void type_of_call g3sstak(Int_t &); 

  void type_of_call g3sxyz(); 
  
  void type_of_call g3many(); 

  void type_of_call g3track(); 

  void type_of_call g3treve(); 

  void type_of_call gtreveroot(); 


  void type_of_call g3dtom(Float_t *, Float_t *, Int_t &); 

  void type_of_call g3lmoth(DEFCHARD, Int_t &, Int_t &, Int_t *,
			   Int_t *, Int_t * DEFCHARL); 

  void type_of_call g3media(Float_t *, Int_t &, Int_t&); 

  void type_of_call g3mtod(Float_t *, Float_t *, Int_t &); 

  void type_of_call g3srotm(const Int_t &, const Float_t &, const Float_t &,
			   const Float_t &, const Float_t &, const Float_t &,
			   const Float_t &); 

  void type_of_call g3protm(const Int_t &); 

  void type_of_call g3rfile(const Int_t&, DEFCHARD, 
			   DEFCHARD DEFCHARL DEFCHARL); 

  void type_of_call g3fmate(const Int_t&, DEFCHARD, Float_t &, Float_t &,
			   Float_t &, Float_t &, Float_t &, Float_t *,
			   Int_t& DEFCHARL); 

  void type_of_call g3fpart(const Int_t&, DEFCHARD, Int_t &, Float_t &,
			   Float_t &, Float_t &, Float_t *, Int_t & DEFCHARL); 

  void type_of_call g3ftmed(const Int_t&, DEFCHARD, Int_t &, Int_t &, Int_t &,
			   Float_t &, Float_t &, Float_t &, Float_t &,
			   Float_t &, Float_t &, Float_t *, Int_t * DEFCHARL); 

  void type_of_call g3ftmat(const Int_t&, const Int_t&, DEFCHARD, const Int_t&,
			   Float_t*, Float_t*
			   ,Float_t *, Int_t & DEFCHARL);

  void type_of_call g3smate(const Int_t&, DEFCHARD, Float_t &, Float_t &,
			   Float_t &, Float_t &, Float_t &, Float_t *,
			   Int_t & DEFCHARL); 

  void type_of_call g3smixt(const Int_t&, DEFCHARD, const Float_t *, 
               const Float_t *, const Float_t &, const Int_t &, 
               Float_t * DEFCHARL); 

  void type_of_call g3spart(const Int_t&, DEFCHARD, Int_t &, Float_t &,
			   Float_t &, Float_t &, Float_t *, Int_t & DEFCHARL); 


  void type_of_call g3stmed(const Int_t&, DEFCHARD, Int_t &, Int_t &, Int_t &,
			   Float_t &, Float_t &, Float_t &, Float_t &,
			   Float_t &, Float_t &, Float_t *, Int_t & DEFCHARL); 

  void type_of_call g3sckov(Int_t &itmed, Int_t &npckov, Float_t *ppckov,
			   Float_t *absco, Float_t *effic, Float_t *rindex);
  void type_of_call g3stpar(const Int_t&, DEFCHARD, Float_t & DEFCHARL); 

  void type_of_call g3sdvn(DEFCHARD,DEFCHARD, Int_t &, Int_t &
			  DEFCHARL DEFCHARL); 

  void type_of_call g3sdvn2(DEFCHARD,DEFCHARD, Int_t &, Int_t &, Float_t &,
			   Int_t & DEFCHARL DEFCHARL); 

  void type_of_call g3sdvs(DEFCHARD,DEFCHARD, Float_t &, Int_t &, Int_t &
			  DEFCHARL DEFCHARL); 

  void type_of_call g3sdvs2(DEFCHARD,DEFCHARD, Float_t &, Int_t &, Float_t &,
			   Int_t & DEFCHARL DEFCHARL); 

  void type_of_call g3sdvt(DEFCHARD,DEFCHARD, Float_t &, Int_t &, Int_t &,
			  Int_t & DEFCHARL DEFCHARL); 

  void type_of_call g3sdvt2(DEFCHARD,DEFCHARD, Float_t &, Int_t &, Float_t&,
			   Int_t &, Int_t & DEFCHARL DEFCHARL); 

  void type_of_call g3sord(DEFCHARD, Int_t & DEFCHARL); 

  void type_of_call g3spos(DEFCHARD, Int_t &, DEFCHARD, Float_t &, Float_t &,
			  Float_t &, Int_t &, DEFCHARD DEFCHARL DEFCHARL
			  DEFCHARL); 

  void type_of_call g3sposp(DEFCHARD, Int_t &, DEFCHARD, Float_t &, Float_t &,
			   Float_t &, Int_t &, DEFCHARD,  
			   Float_t *, Int_t & DEFCHARL DEFCHARL DEFCHARL); 

  void type_of_call g3svolu(DEFCHARD, DEFCHARD, Int_t &, Float_t *, Int_t &,
			   Int_t & DEFCHARL DEFCHARL); 

  void type_of_call g3satt(DEFCHARD, DEFCHARD, Int_t & DEFCHARL DEFCHARL); 

  void type_of_call g3fpara(DEFCHARD , Int_t&, Int_t&, Int_t&, Int_t&, Float_t*,
			   Float_t* DEFCHARL);

  void type_of_call gckpar(Int_t&, Int_t&, Float_t*);

  void type_of_call g3ckmat(Int_t&, DEFCHARD DEFCHARL);

  void type_of_call g3lvolu(Int_t&, Int_t*, Int_t*, Int_t&);

  void type_of_call g3print(DEFCHARD,const int& DEFCHARL); 

  // From gdraw
  void type_of_call g3dinit(); 
  void type_of_call g3dopt(DEFCHARD,DEFCHARD DEFCHARL DEFCHARL); 
  
  void type_of_call g3draw(DEFCHARD,Float_t &,Float_t &, Float_t &,Float_t &,
			  Float_t &, Float_t &, Float_t & DEFCHARL); 
  void type_of_call g3drayt(DEFCHARD,Float_t &,Float_t &, Float_t &,Float_t &,
			   Float_t &, Float_t &, Float_t & DEFCHARL); 
  void type_of_call g3drawc(DEFCHARD,Int_t &, Float_t &, Float_t &, Float_t &,
                          Float_t &, Float_t & DEFCHARL); 
  void type_of_call g3drawx(DEFCHARD,Float_t &, Float_t &, Float_t &, Float_t &,
			   Float_t &, Float_t &, Float_t &, Float_t &,
			   Float_t & DEFCHARL); 
  void type_of_call g3dhead(Int_t &,DEFCHARD, Float_t & DEFCHARL);
  void type_of_call g3dxyz(Int_t &);
  void type_of_call g3dcxyz();
  void type_of_call g3dman(Float_t &, Float_t &);
  void type_of_call g3dwmn1(Float_t &, Float_t &);
  void type_of_call g3dwmn2(Float_t &, Float_t &);
  void type_of_call g3dwmn3(Float_t &, Float_t &);
  void type_of_call g3dspec(DEFCHARD DEFCHARL);
  void type_of_call g3dfspc(DEFCHARD, Int_t &, Int_t & DEFCHARL) {;}
  void type_of_call g3dtree(DEFCHARD, Int_t &, Int_t & DEFCHARL);

  void type_of_call g3dopen(Int_t &);
  void type_of_call g3dclos();
  void type_of_call g3delet(Int_t &);
  void type_of_call g3dshow(Int_t &);


  void type_of_call dzshow(DEFCHARD,const int&,const int&,DEFCHARD,const int&,
			   const int&, const int&, const int& DEFCHARL
			   DEFCHARL); 

  void type_of_call mzdrop(Int_t&, Int_t&, DEFCHARD DEFCHARL);

  void type_of_call setbomb(Float_t &);
  void type_of_call setclip(DEFCHARD, Float_t &,Float_t &,Float_t &,Float_t &,
			    Float_t &, Float_t & DEFCHARL); 
  void type_of_call gcomad(DEFCHARD, Int_t*& DEFCHARL);

  void type_of_call ertrak(const Float_t *const x1, const Float_t *const p1,
			   const Float_t *x2, const Float_t *p2,
			   const Int_t &ipa, DEFCHARD DEFCHARL);

  void type_of_call ertrgo();
  
    float type_of_call g3brelm(const Float_t &z, const Float_t& t, const Float_t& cut);
    float type_of_call g3prelm(const Float_t &z, const Float_t& t, const Float_t& cut);
}

//
// Geant3 global pointer
// 
TGeoNode  *StTGeant3::fgCurrentNode = 0;
Gctrak_t  *StTGeant3::fgGctrack     = 0;
Gcvolu_t  *StTGeant3::fgGcvolu      = 0;
Int_t      StTGeant3::fgMode        = 0;
StTGeant3 *StTGeant3::fgGeant3      = 0;

static const Int_t kDefSize = 600;
Int_t count_ginvol = 0;
Int_t count_gmedia = 0;
Int_t count_gtmedi = 0;
Int_t count_gtnext = 0;
Gcchan_t *gcchan = 0;

//#define STATISTICS
#ifdef STATISTICS
#include "TTree.h"
#include "TFile.h"
Float_t oldvect[6], oldstep, oldsafety;
Int_t statcode;
Double_t statsafety, statsnext;
TTree *stattree =0;
TFile *statfile=0;
#endif
 
//____________________________________________________________________________ 
StTGeant3::StTGeant3()
  : TVirtualMC(),
    fNG3Particles(0),
    fNPDGCodes(0),
    fMCGeo(0),
    fImportRootGeometry(kFALSE)
{ 
  //
  // Default constructor
  //
   fgGeant3 = this;
   InitBridge(); 
} 
 
//____________________________________________________________________________ 
StTGeant3::StTGeant3(const char *title, Int_t isrg, Int_t nwgeant) 
       : TVirtualMC("StTGeant3",title, isrg&kWithRoot),
         fImportRootGeometry(kFALSE)
{
  fgMode = isrg;
  //
  // Standard constructor for StTGeant3 with ZEBRA initialisation
  // 
   
#ifdef STATISTICS
   statfile = new TFile("stat.root","recreate");
   stattree = new TTree("stat","stat tree");
   stattree->Branch("statcode",&statcode,"statcode/I");
   stattree->Branch("oldvect",oldvect,"oldvect[6]/F");
   stattree->Branch("oldsafety",&oldsafety,"oldsafety/F");
   stattree->Branch("oldstep",&oldstep,"oldstep/F");
   stattree->Branch("snext",&statsnext,"statsnext/D");
   stattree->Branch("safety",&statsafety,"statsafety/D");
#endif
   
  fgGeant3 = this;
  fMCGeo = 0;
  if (Mode()==kWithG3)
  fMCGeo = new TGeoMCGeometry("MCGeo", "TGeo Implementation of VirtualMCGeometry");
  if (Mode()==kWithBoth)
  fMCGeo = new TGeoMCGeometry("MCGeo", "TGeo Implementation of VirtualMCGeometry",
                              true);
  
  if(nwgeant) {
    g3zebra(nwgeant); 
    g3init(); 
    g3zinit();
  } else {
    g3cinit();
  }
  //
  // Load Address of Geant3 commons    
  LoadAddress(); 
  InitBridge(); 
  //
  // Zero number of particles
  fNG3Particles = 0;
  fNPDGCodes=0;
} 
 
//____________________________________________________________________________ 
StTGeant3::~StTGeant3()
{
  if(fVolNames) {
    delete [] fVolNames;
    fVolNames=0;
  }
}
//____________________________________________________________________________ 
void StTGeant3::InitBridge() 
{
   StTGeant3gu::InitBridge();
   G3Bridge *b = G3Bridge::Instance();
   if (!b) return;
   b->m_rxgtrak = (void*)&Brxgtrak;
   b->m_rxouth  = (void*)&Brxouth;
   b->m_rxinh   = (void*)&Brxinh ;
   b->m_ginvol  = (void*)&Bginvol;
   b->m_gtmedi  = (void*)&Bgtmedi;
   b->m_gmedia  = (void*)&Bgmedia;
   b->m_gtmany  = (void*)&Bgtmany;
   b->m_gtonly  = (void*)&Bgtonly;
   b->m_glvolu  = (void*)&Bglvolu;
   b->m_gtnext  = (void*)&Bgtnext;
   b->m_ggperp  = (void*)&Bggperp;
   b->m_grndm   = (void*)&Bgrndm;
   b->m_grndmq   = (void*)&Bgrndmq;
}
//____________________________________________________________________________ 
Int_t StTGeant3::CurrentMaterial(Float_t &a, Float_t &z, Float_t &dens,
			       Float_t &radl, Float_t &absl) const
{
  //
  // Return the parameters of the current material during transport
  //
  z     = fGcmate->z;
  a     = fGcmate->a;
  dens  = fGcmate->dens;
  radl  = fGcmate->radl;
  absl  = fGcmate->absl;
//VP  return 1;  //this could be the number of elements in mixture
  return fGcmate->nmat;
}
   
//____________________________________________________________________________ 
void StTGeant3::DefaultRange()
{ 
  //
  // Set range of current drawing pad to 20x20 cm
  //
  if (!gHigz) {
    new THIGZ(kDefSize); 
    g3dinit();
  }
  gHigz->Range(0,0,20,20);
}

//____________________________________________________________________________ 
void StTGeant3::InitHIGZ() 
{ 
  //
  // Initialise HIGZ
  //
  if (!gHigz) {
    new THIGZ(kDefSize); 
    g3dinit();
  }
}
 
//____________________________________________________________________________ 
void StTGeant3::LoadAddress() 
{ 
  //
  // Assigns the address of the GEANT common blocks to the structures
  // that allow their access from C++
  //
   Int_t *addr;
   gcomad(PASSCHARD("QUEST"), (int*&) fQuest PASSCHARL("QUEST"));
   gcomad(PASSCHARD("GCBANK"),(int*&) fGcbank  PASSCHARL("GCBANK"));
   gcomad(PASSCHARD("GCLINK"),(int*&) fGclink  PASSCHARL("GCLINK"));
   gcomad(PASSCHARD("GCCUTS"),(int*&) fGccuts  PASSCHARL("GCCUTS"));
   gcomad(PASSCHARD("GCMULO"),(int*&) fGcmulo  PASSCHARL("GCMULO"));
   gcomad(PASSCHARD("GCFLAG"),(int*&) fGcflag  PASSCHARL("GCFLAG"));
   gcomad(PASSCHARD("GCKINE"),(int*&) fGckine  PASSCHARL("GCKINE"));
   gcomad(PASSCHARD("GCKING"),(int*&) fGcking  PASSCHARL("GCKING"));
   gcomad(PASSCHARD("GCKIN2"),(int*&) fGckin2  PASSCHARL("GCKIN2"));
   gcomad(PASSCHARD("GCKIN3"),(int*&) fGckin3  PASSCHARL("GCKIN3"));
   gcomad(PASSCHARD("GCMATE"),(int*&) fGcmate  PASSCHARL("GCMATE"));
   gcomad(PASSCHARD("GCTMED"),(int*&) fGctmed  PASSCHARL("GCTMED"));
   gcomad(PASSCHARD("GCTRAK"),(int*&) fGctrak  PASSCHARL("GCTRAK"));
   gcomad(PASSCHARD("GCTPOL"),(int*&) fGctpol  PASSCHARL("GCTPOL"));
   gcomad(PASSCHARD("GCVOLU"),(int*&) fGcvolu  PASSCHARL("GCVOLU"));
   gcomad(PASSCHARD("GCNUM"), (int*&) fGcnum   PASSCHARL("GCNUM"));
   gcomad(PASSCHARD("GCSETS"),(int*&) fGcsets  PASSCHARL("GCSETS"));
   gcomad(PASSCHARD("GCPHYS"),(int*&) fGcphys  PASSCHARL("GCPHYS"));
   gcomad(PASSCHARD("GCPHLT"),(int*&) fGcphlt  PASSCHARL("GCPHLT"));
   gcomad(PASSCHARD("GCOPTI"),(int*&) fGcopti  PASSCHARL("GCOPTI"));
   gcomad(PASSCHARD("GCTLIT"),(int*&) fGctlit  PASSCHARL("GCTLIT"));
   gcomad(PASSCHARD("GCVDMA"),(int*&) fGcvdma  PASSCHARL("GCVDMA"));
   gcomad(PASSCHARD("GCCHAN"),(int*&) gcchan   PASSCHARL("GCCHAN"));
   
   // Commons for GEANE
   gcomad(PASSCHARD("ERTRIO"),(int*&) fErtrio  PASSCHARL("ERTRIO"));
   gcomad(PASSCHARD("EROPTS"),(int*&) fEropts  PASSCHARL("EROPTS"));
   gcomad(PASSCHARD("EROPTC"),(int*&) fEroptc  PASSCHARL("EROPTC"));
   gcomad(PASSCHARD("ERWORK"),(int*&) fErwork  PASSCHARL("ERWORK"));

   // Variables for ZEBRA store
   gcomad(PASSCHARD("IQ"), addr  PASSCHARL("IQ"));
   fZiq = addr;
   gcomad(PASSCHARD("LQ"), addr  PASSCHARL("LQ"));
   fZlq = addr;
   fZq       = (float*)fZiq; 
   fgGctrack = fGctrak;
   fgGcvolu = fGcvolu;
   Z_t::Init(fZiq+1);
} 

//_____________________________________________________________________________
void StTGeant3::GeomIter()
{
  //
  // Geometry iterator for moving upward in the geometry tree
  // Initialise the iterator
  //
  if (Mode()&kWithG3   ) {fNextVol=fGcvolu->nlevel        ;return;}
  if (Mode()==kWithRoot) {fNextVol=gGeoManager->GetLevel();return;}
}

//____________________________________________________________________________ 
Int_t StTGeant3::NextVolUp(Text_t *name, Int_t &copy)
{
  //
  // Geometry iterator for moving upward in the geometry tree
  // Return next volume up
  //
  fNextVol--;
  if (Mode()&kWithG3){
    Int_t i, gname;
    if(fNextVol>=0) {
      gname=fGcvolu->names[fNextVol];
      copy=fGcvolu->number[fNextVol];
      i=fGcvolu->lvolum[fNextVol];
      name = fVolNames[i-1];
      if(gname == fZiq[fGclink->jvolum+i]) return i;
      else printf("GeomTree: Volume %s not found in bank\n",name);
    }
    return 0;
  }
  if (Mode()==kWithRoot){
    if (fNextVol>=0) {
       Int_t level = gGeoManager->GetLevel();
       if (level<=fNextVol) return 0;
       TGeoNode *mother = gGeoManager->GetMother(level-fNextVol);
       if (!mother) return 0;
       sprintf(name, "%s", mother->GetVolume()->GetName());
       copy = mother->GetNumber();
       return mother->GetVolume()->GetNumber();
    }     
    return 0;
  }
  return 0;
}

//_____________________________________________________________________________
void StTGeant3::BuildPhysics()
{
  Gphysi();
}

//___________________________________________________________________________$
void StTGeant3::AddParticlesToPdgDataBase() const
{
    
//
// Add particles to the PDG data base
  
    TDatabasePDG *pdgDB = TDatabasePDG::Instance();
 
    const Int_t kion=10000000;
    const Int_t kspe=50000000;

    const Double_t kAu2Gev=0.9314943228;
    const Double_t khSlash = 1.0545726663e-27;
    const Double_t kErg2Gev = 1/1.6021773349e-3;
    const Double_t khShGev = khSlash*kErg2Gev;
    const Double_t kYear2Sec = 3600*24*365.25;
//
// Bottom mesons
// mass and life-time from PDG
//
// Done by default now from Pythia6 table!
//
//
// Ions
//

  pdgDB->AddParticle("Deuteron","Deuteron",2*kAu2Gev+8.071e-3,kTRUE,
                     0,3,"Ion",kion+10020);
  pdgDB->AddParticle("Triton","Triton",3*kAu2Gev+14.931e-3,kFALSE,
                     khShGev/(12.33*kYear2Sec),3,"Ion",kion+10030);
  pdgDB->AddParticle("Alpha","Alpha",4*kAu2Gev+2.424e-3,kTRUE,
                     khShGev/(12.33*kYear2Sec),6,"Ion",kion+20040);
  pdgDB->AddParticle("HE3","HE3",3*kAu2Gev+14.931e-3,kFALSE,
                     0,6,"Ion",kion+20030);

// Special particles
//
  pdgDB->AddParticle("Cherenkov","Cherenkov",0,kFALSE,
                     0,0,"Special",kspe+50);  
  pdgDB->AddParticle("FeedbackPhoton","FeedbackPhoton",0,kFALSE,
                     0,0,"Special",kspe+51);

} 


//_____________________________________________________________________________
Int_t StTGeant3::CurrentVolID(Int_t &copy) const
{
  //
  // Returns the current volume ID and copy number
  //
  if (Mode()&kWithG3){
    Int_t i, gname;
    if( (i=fGcvolu->nlevel-1) < 0 ) {
      Warning("CurrentVolID","Stack depth only %d\n",fGcvolu->nlevel);
    } else {
      gname=fGcvolu->names[i];
      copy=fGcvolu->number[i];
      i=fGcvolu->lvolum[i];   
      if(gname == fZiq[fGclink->jvolum+i]) return i;
      else Warning("CurrentVolID","Volume %4s not found\n",(char*)&gname);
    }
    return 0;
  }
  if (Mode()==kWithRoot){
    if (gGeoManager->IsOutside()) return 0;
    TGeoNode *node = gGeoManager->GetCurrentNode();
    copy = node->GetNumber();
    Int_t id = node->GetVolume()->GetNumber();
    return id;
  }
  return 0;
}

//_____________________________________________________________________________
Int_t StTGeant3::CurrentVolOffID(Int_t off, Int_t &copy) const
{
  //
  // Return the current volume "off" upward in the geometrical tree 
  // ID and copy number
  //
  if (Mode()&kWithG3){
    Int_t i, gname;
    if( (i=fGcvolu->nlevel-off-1) < 0 ) {
      Warning("CurrentVolOffID","Offset requested %d but stack depth %d\n",
	      off,fGcvolu->nlevel);
    } else {
      gname=fGcvolu->names[i];
      copy=fGcvolu->number[i];          
      i=fGcvolu->lvolum[i];    
      if(gname == fZiq[fGclink->jvolum+i]) return i;
      else Warning("CurrentVolOffID","Volume %4s not found\n",(char*)&gname);
    }
    return 0;
  }
  if (Mode()==kWithRoot){
    if (off<0 || off>gGeoManager->GetLevel()) return 0;
    if (off==0) return CurrentVolID(copy);
    TGeoNode *node = gGeoManager->GetMother(off);
    if (!node) return 0;
    copy = node->GetNumber();
    return node->GetVolume()->GetNumber();
  }
  return 0;
}
//_____________________________________________________________________________
Int_t StTGeant3::CurrentVolOffID(Int_t off, Int_t &copy, Int_t &idx, Int_t &ndx) const
{
  copy = 0; idx = 0; ndx=0;
  int lv, iv;
  if (Mode()&kWithG3){
    if( (lv=fGcvolu->nlevel-off-1) < 0 ) return 0;
    copy =fGcvolu->number[lv];          
    iv   =fGcvolu->lvolum[lv];    
    if (!lv) return iv;
    JFan_t *Jfun = (JFan_t*)Z_t::ZObj(fGclink->jvolum);
    int ivm = fGcvolu->number[lv-1];
    JVolu_t *Jmoth = (JVolu_t*)Jfun->GetObj(ivm);
    assert(Jmoth);
    ndx = abs((int)Jmoth->NIN);
    idx = copy;
    if (Jmoth->GetJDiv()) return iv;
    for (int i=1;i<=ndx;i++) {
      JPos_t *Jpos = Jmoth->GetJPos(i);
      if (iv   != (int)Jpos->IVO) continue;
      if (copy != (int)Jpos->NR ) continue;
      idx = i; return iv;
    }
    assert(0);
  } else {
    lv = gGeoManager->GetLevel();
    if (off<0 || off>lv) return 0;
    TGeoNode *node = gGeoManager->GetMother(off);
    if (!node) return 0;
    copy = node->GetNumber();
    iv   = node->GetVolume()->GetNumber();
    TGeoVolume *moth = node->GetMotherVolume();   
    if (!moth) return iv;
    ndx = moth->GetNdaughters();
    idx = moth->GetIndex(node)+1;
    return iv;
  }
}
//_____________________________________________________________________________
StMCPath StTGeant3::CurrentPath() const
{
   StMCPath P;
   Int_t iv,copy,idx,ndx;
   int nlev = GetLevel();
   for (int off=0;off<nlev;off++) {
     iv = CurrentVolOffID(off,copy,idx,ndx);
     P.SetLevel(nlev-off-1,iv,copy,idx,ndx);
   }
   return P;
}	
//_____________________________________________________________________________
ULong64_t StTGeant3::CurrentPathID() const
{
   ULong64_t U = 0;
   Int_t iv,copy,idx,ndx,nbbits=0;
   for (int off=0;1;off++) {
     iv = CurrentVolOffID(off,copy,idx,ndx);
     if(!ndx) break;
     int n2dx=2,nbits=1; for (;n2dx-1<ndx; n2dx*=2,nbits++){}
     U <<= nbits;  U|=idx; nbbits+=nbits;
     if (nbbits> 32) printf("off=%d iv,copy,idx,ndx,nbb = %d %d %d %d %d\n",off,iv,copy,idx,ndx,nbbits);
     Assert(nbbits<=64);
   }
   return U;
}	

//_____________________________________________________________________________
Int_t StTGeant3::SetCurrentPath(ULong64_t P)
{
  int iv,copy,ndx,idx,n2dx,nbits;
  if (Mode()&kWithG3){
    int LNAM[100],LNUM[100],NLEV;
    JFan_t *fan = (JFan_t*)Z_t::ZObj(fGclink->jvolum);
    JVolu_t *Jvolu = (JVolu_t*)fan->GetObj(1);
    ndx = (int)Jvolu->NIN;
    for (NLEV=0;;NLEV++)
    {
      for (n2dx=2,nbits=1;n2dx-1<ndx; n2dx*=2,nbits++){}
      idx = P&(n2dx-1); P <<=nbits;
      if (!idx) break;
      iv = Jvolu->SubVoluID(idx,copy);
      LNAM[NLEV]=fan->iDat[iv];
      LNUM[NLEV]=copy;
      Jvolu = (JVolu_t*)fan->GetObj(idx);
    }
    return Glvolu(NLEV,LNAM,LNUM);

  } else {
    gGeoManager->CdTop();
    while(1) {
       TGeoVolume *vol = gGeoManager->GetCurrentVolume();
       ndx = vol->GetNdaughters();
       for (n2dx=2,nbits=1;n2dx-1<ndx; n2dx*=2,nbits++){}
       idx = P&(n2dx-1); P <<=nbits;
       if (!idx) break;
       gGeoManager->CdDown(idx-1);
    }
    return 0;
  }
}	
//_____________________________________________________________________________
const char* StTGeant3::CurrentVolName() const
{
  //
  // Returns the current volume name
  //
  if (Mode()&kWithG3){
    Int_t i;
    if( (i=fGcvolu->nlevel-1) < 0 ) {
      Warning("CurrentVolName","Stack depth %d\n",fGcvolu->nlevel);
      return 0;
    }
    Int_t gname=fGcvolu->names[i];
    i=fGcvolu->lvolum[i];   
    if(gname == fZiq[fGclink->jvolum+i]) return fVolNames[i-1];
    else Warning("CurrentVolName","Volume %4s not found\n",(char*) &gname);
    return 0;
  }
  if (Mode()==kWithRoot){
    if (gGeoManager->IsOutside()) return 0;
    return gGeoManager->GetCurrentVolume()->GetName();
  }
  return 0;
}

//_____________________________________________________________________________
const char* StTGeant3::CurrentVolOffName(Int_t off) const
{
  //
  // Return the current volume "off" upward in the geometrical tree 
  // ID, name and copy number
  // if name=0 no name is returned
  //
  Int_t i;
  if( (i=fGcvolu->nlevel-off-1) < 0 ) {
    Warning("CurrentVolOffName",
	    "Offset requested %d but stack depth %d\n",off,fGcvolu->nlevel);
    return 0;
  }
  if (Mode()&kWithG3){
    Int_t gname=fGcvolu->names[i];
    i=fGcvolu->lvolum[i];    
    if(gname == fZiq[fGclink->jvolum+i]) return fVolNames[i-1];
    else Warning("CurrentVolOffName","Volume %4s not found\n",(char*)&gname);
    return 0;
  }
    if (Mode()==kWithRoot){
    if (off<0 || off>gGeoManager->GetLevel()) return 0;
    if (off==0) return CurrentVolName();
    TGeoNode *node = gGeoManager->GetMother(off);
    if (!node) return 0;
    return node->GetVolume()->GetName();
  }
  return 0;
}

//_____________________________________________________________________________
Int_t StTGeant3::IdFromPDG(Int_t pdg) const 
{
  //
  // Return Geant3 code from PDG and pseudo ENDF code
  //
  for(Int_t i=0;i<fNPDGCodes;++i)
    if(pdg==fPDGCode[i]) return i;
  return -1;
}

//_____________________________________________________________________________
Int_t StTGeant3::PDGFromId(Int_t id) const 
{
  //
  // Return PDG code and pseudo ENDF code from Geant3 code
  //
  if(id>0 && id<fNPDGCodes) return fPDGCode[id];
  else return -1;
}

//_____________________________________________________________________________
void StTGeant3::DefineParticles() 
{
  //
  // Define standard Geant 3 particles
  Gpart();
  //
  // Load standard numbers for GEANT particles and PDG conversion
  fPDGCode[fNPDGCodes++]=-99;   //  0 = unused location
  fPDGCode[fNPDGCodes++]=22;    //  1 = photon
  fPDGCode[fNPDGCodes++]=-11;   //  2 = positron
  fPDGCode[fNPDGCodes++]=11;    //  3 = electron
  fPDGCode[fNPDGCodes++]=12;    //  4 = neutrino e
  fPDGCode[fNPDGCodes++]=-13;   //  5 = muon +
  fPDGCode[fNPDGCodes++]=13;    //  6 = muon -
  fPDGCode[fNPDGCodes++]=111;   //  7 = pi0
  fPDGCode[fNPDGCodes++]=211;   //  8 = pi+
  fPDGCode[fNPDGCodes++]=-211;  //  9 = pi-
  fPDGCode[fNPDGCodes++]=130;   // 10 = Kaon Long
  fPDGCode[fNPDGCodes++]=321;   // 11 = Kaon +
  fPDGCode[fNPDGCodes++]=-321;  // 12 = Kaon -
  fPDGCode[fNPDGCodes++]=2112;  // 13 = Neutron
  fPDGCode[fNPDGCodes++]=2212;  // 14 = Proton
  fPDGCode[fNPDGCodes++]=-2212; // 15 = Anti Proton
  fPDGCode[fNPDGCodes++]=310;   // 16 = Kaon Short
  fPDGCode[fNPDGCodes++]=221;   // 17 = Eta
  fPDGCode[fNPDGCodes++]=3122;  // 18 = Lambda
  fPDGCode[fNPDGCodes++]=3222;  // 19 = Sigma +
  fPDGCode[fNPDGCodes++]=3212;  // 20 = Sigma 0
  fPDGCode[fNPDGCodes++]=3112;  // 21 = Sigma -
  fPDGCode[fNPDGCodes++]=3322;  // 22 = Xi0
  fPDGCode[fNPDGCodes++]=3312;  // 23 = Xi-
  fPDGCode[fNPDGCodes++]=3334;  // 24 = Omega-
  fPDGCode[fNPDGCodes++]=-2112; // 25 = Anti Proton
  fPDGCode[fNPDGCodes++]=-3122; // 26 = Anti Proton
  fPDGCode[fNPDGCodes++]=-3222; // 27 = Anti Sigma -
  fPDGCode[fNPDGCodes++]=-3212; // 28 = Anti Sigma 0
  fPDGCode[fNPDGCodes++]=-3112; // 29 = Anti Sigma 0
  fPDGCode[fNPDGCodes++]=-3322; // 30 = Anti Xi 0
  fPDGCode[fNPDGCodes++]=-3312; // 31 = Anti Xi +
  fPDGCode[fNPDGCodes++]=-3334; // 32 = Anti Omega +


  Int_t mode[6];
  Int_t kz, ipa;
  Float_t bratio[6];

  fNG3Particles = 33;

  /* --- Define additional particles */
  Gspart(fNG3Particles++, "OMEGA(782)", 3, 0.782, 0., 7.836e-23);  // 33 = OMEGA(782)
  fPDGCode[fNPDGCodes++]=223;   // 33 = Omega(782)
  
  Gspart(fNG3Particles++, "PHI(1020)", 3, 1.019, 0., 1.486e-22); // 34 = PHI(1020)
  fPDGCode[fNPDGCodes++]=333;   // 34 = PHI (1020)

  Gspart(fNG3Particles++, "D +", 4, 1.87, 1., 1.066e-12);        // 35 = D+
  fPDGCode[fNPDGCodes++]=411;   // 35 = D+

  Gspart(fNG3Particles++, "D -", 4, 1.87, -1., 1.066e-12);       // 36 = D-
  fPDGCode[fNPDGCodes++]=-411;  // 36 = D-

  Gspart(fNG3Particles++, "D 0", 3, 1.865, 0., 4.2e-13);         // 37 = D0
  fPDGCode[fNPDGCodes++]=421;   // 37 = D0

  Gspart(fNG3Particles++, "ANTI D 0", 3, 1.865, 0., 4.2e-13);    // 38 = Anti D0
  fPDGCode[fNPDGCodes++]=-421;  // 38 = D0 bar


  fNG3Particles++;
  fPDGCode[fNPDGCodes++]=-99;  // 39 = unassigned

  fNG3Particles++;
  fPDGCode[fNPDGCodes++]=-99;  // 40 = unassigned

  fNG3Particles++;
  fPDGCode[fNPDGCodes++]=-99;  // 41 = unassigned

  Gspart(fNG3Particles++, "RHO +", 4, 0.768, 1., 4.353e-24);  // 42 = Rho+
  fPDGCode[fNPDGCodes++]=213;   // 42 = RHO+

  Gspart(fNG3Particles++, "RHO -", 4, 0.768, -1., 4.353e-24); // 43 = Rho-
  fPDGCode[fNPDGCodes++]=-213;   // 43 = RHO-

  Gspart(fNG3Particles++, "RHO 0", 3, 0.768, 0., 4.353e-24);  // 44 = Rho0
  fPDGCode[fNPDGCodes++]=113;   //  44 = RHO0

  //
  // Use ENDF-6 mapping for ions = 10000*z+10*a+iso
  // and add 1 000 000
  // and numbers above 5 000 000 for special applications
  //

  const Int_t kion=10000000;

  const Int_t kspe=50000000;

//
// Ions 

  fNG3Particles++;
  fPDGCode[fNPDGCodes++]=kion+10020;   // 45 = Deuteron

  fNG3Particles++;
  fPDGCode[fNPDGCodes++]=kion+10030;   // 46 = Triton

  fNG3Particles++;
  fPDGCode[fNPDGCodes++]=kion+20040;   // 47 = Alpha

  fNG3Particles++;
  fPDGCode[fNPDGCodes++]=0;            // 48 = geantino mapped to rootino

  fNG3Particles++;
  fPDGCode[fNPDGCodes++]=kion+20030;   // 49 = HE3

  fNG3Particles++;
  fPDGCode[fNPDGCodes++]=kspe+50;      // 50 = Cherenkov
// special 
  Gspart(fNG3Particles++, "FeedbackPhoton", 7, 0., 0.,1.e20 );
  fPDGCode[fNPDGCodes++]=kspe+51;      // 51 = FeedbackPhoton
//
  Gspart(fNG3Particles++, "Lambda_c+", 4, 2.2849, +1., 2.06e-13);
  fPDGCode[fNPDGCodes++]=4122;         //52 = Lambda_c+

  Gspart(fNG3Particles++, "Lambda_c-", 4, 2.2849, -1., 2.06e-13);
  fPDGCode[fNPDGCodes++]=-4122;        //53 = Lambda_c-  

  Gspart(fNG3Particles++, "D_s+", 4, 1.9685, +1., 4.67e-13);
  fPDGCode[fNPDGCodes++]=431;          //54 = D_s+

  Gspart(fNG3Particles++, "D_s-", 4, 1.9685, -1., 4.67e-13);
  fPDGCode[fNPDGCodes++]=-431;         //55 = D_s-

  Gspart(fNG3Particles++, "Tau+", 5, 1.77705, +1., 2.9e-13);
  fPDGCode[fNPDGCodes++]=15;           //56 = Tau+

  Gspart(fNG3Particles++, "Tau-", 5, 1.77705, -1., 2.9e-13);
  fPDGCode[fNPDGCodes++]=-15;          //57 = Tau-  

  Gspart(fNG3Particles++, "B0",     3, 5.2792, +0., 1.56e-12);
  fPDGCode[fNPDGCodes++]=511;          //58 = B0

  Gspart(fNG3Particles++, "B0 bar", 3, 5.2792, -0., 1.56e-12);
  fPDGCode[fNPDGCodes++]=-511;         //58 = B0bar

  Gspart(fNG3Particles++, "B+",     4, 5.2789, +1., 1.65e-12);
  fPDGCode[fNPDGCodes++]=521;          //60 = B+

  Gspart(fNG3Particles++, "B-",     4, 5.2789, -1., 1.65e-12);
  fPDGCode[fNPDGCodes++]=-521;         //61 = B-

  Gspart(fNG3Particles++, "Bs",     3, 5.3693, +0., 1.54e-12);
  fPDGCode[fNPDGCodes++]=531;          //62 = B_s

  Gspart(fNG3Particles++, "Bs bar", 3, 5.3693, -0., 1.54e-12);
  fPDGCode[fNPDGCodes++]=-531;         //63 = B_s bar

  Gspart(fNG3Particles++, "Lambda_b",     3, 5.624, +0., 1.24e-12);
  fPDGCode[fNPDGCodes++]=5122;         //64 = Lambda_b

  Gspart(fNG3Particles++, "Lambda_b bar", 3, 5.624, -0., 1.24e-12);
  fPDGCode[fNPDGCodes++]=-5122;        //65 = Lambda_b bar

  Gspart(fNG3Particles++, "J/Psi",       3, 3.09688, 0., 0.);
  fPDGCode[fNPDGCodes++]=443;          // 66 = J/Psi

  Gspart(fNG3Particles++, "Psi Prime",   3, 3.686,   0., 0.);
  fPDGCode[fNPDGCodes++]=20443;        // 67 = Psi prime

  Gspart(fNG3Particles++, "Upsilon(1S)", 3, 9.46037, 0., 0.);
  fPDGCode[fNPDGCodes++]=553;          // 68 = Upsilon(1S)

  Gspart(fNG3Particles++, "Upsilon(2S)", 3, 10.0233, 0., 0.);
  fPDGCode[fNPDGCodes++]=20553;        // 69 = Upsilon(2S)

  Gspart(fNG3Particles++, "Upsilon(3S)", 3, 10.3553, 0., 0.);
  fPDGCode[fNPDGCodes++]=30553;        // 70 = Upsilon(3S)

  Gspart(fNG3Particles++, "Anti Neutrino (e)",       3, 0., 0., 1.e20);
  fPDGCode[fNPDGCodes++]=-12;          // 71 = anti electron neutrino 

  Gspart(fNG3Particles++, "Neutrino (mu)",           3, 0., 0., 1.e20);
  fPDGCode[fNPDGCodes++]=14;           // 72 = muon neutrino 

  Gspart(fNG3Particles++, "Anti Neutrino (mu)", 3, 0., 0., 1.e20);
  fPDGCode[fNPDGCodes++]=-14;          // 73 = anti muon neutrino

  Gspart(fNG3Particles++, "Neutrino (tau)",     3, 0., 0., 1.e20);
  fPDGCode[fNPDGCodes++]=16;           // 74 = tau neutrino 

  Gspart(fNG3Particles++, "Anti Neutrino (tau)",3, 0., 0., 1.e20);
  fPDGCode[fNPDGCodes++]=-16;          // 75 = anti tau neutrino

/* --- Define additional decay modes --- */
/* --- omega(783) --- */
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 33;
    bratio[0] = 89.;
    bratio[1] = 8.5;
    bratio[2] = 2.5;
    mode[0] = 70809;
    mode[1] = 107;
    mode[2] = 908;
    Gsdk(ipa, bratio, mode);
/* --- phi(1020) --- */
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 34;
    bratio[0] = 49.;
    bratio[1] = 34.4;
    bratio[2] = 12.9;
    bratio[3] = 2.4;
    bratio[4] = 1.3;
    mode[0] = 1112;
    mode[1] = 1610;
    mode[2] = 4407;
    mode[3] = 90807;
    mode[4] = 1701;
    Gsdk(ipa, bratio, mode);
/* --- D+ --- */
    /*
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 35;
    bratio[0] = 25.;
    bratio[1] = 25.;
    bratio[2] = 25.;
    bratio[3] = 25.;
    mode[0] = 80809;
    mode[1] = 120808;
    mode[2] = 111208;
    mode[3] = 110809;
    Gsdk(ipa, bratio, mode);
    */
/* --- D- --- */
    /*
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 36;
    bratio[0] = 25.;
    bratio[1] = 25.;
    bratio[2] = 25.;
    bratio[3] = 25.;
    mode[0] = 90908;
    mode[1] = 110909;
    mode[2] = 121109;
    mode[3] = 120908;
    Gsdk(ipa, bratio, mode);
    */
/* --- D0 --- */
    /*
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 37;
    bratio[0] = 33.;
    bratio[1] = 33.;
    bratio[2] = 33.;
    mode[0] = 809;
    mode[1] = 1208;
    mode[2] = 1112;
    Gsdk(ipa, bratio, mode);
    */
/* --- Anti D0 --- */
    /*
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 38;
    bratio[0] = 33.;
    bratio[1] = 33.;
    bratio[2] = 33.;
    mode[0] = 809;
    mode[1] = 1109;
    mode[2] = 1112;
    Gsdk(ipa, bratio, mode);
    */
/* --- rho+ --- */
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 42;
    bratio[0] = 100.;
    mode[0] = 807;
    Gsdk(ipa, bratio, mode);
/* --- rho- --- */
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 43;
    bratio[0] = 100.;
    mode[0] = 907;
    Gsdk(ipa, bratio, mode);
/* --- rho0 --- */
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 44;
    bratio[0] = 100.;
    mode[0] = 809;
    Gsdk(ipa, bratio, mode);
    /*
// --- jpsi ---
    for (kz = 0; kz < 6; ++kz) {
	bratio[kz] = 0.;
	mode[kz] = 0;
    }
    ipa = 113;
    bratio[0] = 50.;
    bratio[1] = 50.;
    mode[0] = 506;
    mode[1] = 605;
    Gsdk(ipa, bratio, mode);
// --- upsilon --- 
    ipa = 114;
    Gsdk(ipa, bratio, mode);
// --- phi --- 
    ipa = 115;
    Gsdk(ipa, bratio, mode);
    */
//
    AddParticlesToPdgDataBase();

}

//_____________________________________________________________________________
Int_t StTGeant3::VolId(const Text_t *name) const
{
  //
  // Return the unique numeric identifier for volume name
  //
  if (Mode()&kWithG3){
    Int_t gname, i;
    strncpy((char *) &gname, name, 4);
    for(i=1; i<=fGcnum->nvolum; i++)
      if(gname == fZiq[fGclink->jvolum+i]) return i;
    printf("VolId: Volume %s not found\n",name);
    return 0;
  }
  if (Mode()==kWithRoot){
    return fMCGeo->VolId(name);
  }
  return 0;
}

//_____________________________________________________________________________
Int_t StTGeant3::NofVolumes() const 
{
  //
  // Return total number of volumes in the geometry
  //
  if (Mode()&kWithG3){
    return fGcnum->nvolum;
  } else {
    return fMCGeo->NofVolumes();
  }
}

//_____________________________________________________________________________
Int_t StTGeant3::VolId2Mate(Int_t id) const 
{
  //
  // Return material number for a given volume id
  //
  if (Mode()&kWithG3){
    if(id<1 || id > fGcnum->nvolum || fGclink->jvolum<=0) 
      return 0;
    else {
      Int_t jvo = fZlq[fGclink->jvolum-id];
      return Int_t(fZq[jvo+4]);
    }
  } else {
    return fMCGeo->VolId2Mate(id);
  }
}

//_____________________________________________________________________________
const char* StTGeant3::VolName(Int_t id) const
{
  //
  // Return the volume name given the volume identifier
  //
  if (Mode()&kWithG3){
      if(id<1 || id > fGcnum->nvolum || fGclink->jvolum<=0) 
	return fVolNames[fGcnum->nvolum];
      else
	return fVolNames[id-1];
  }
  if (Mode()==kWithRoot){
      return fMCGeo->VolName(id);
  }
  return 0;
}

//_____________________________________________________________________________
Bool_t    StTGeant3::SetCut(const char* cutName, Double_t cutValue)
{
  //
  // Set transport cuts for particles
  //
  if(!strcmp(cutName,"CUTGAM")) 
    fGccuts->cutgam=cutValue; 
  else if(!strcmp(cutName,"CUTELE")) 
    fGccuts->cutele=cutValue; 
  else if(!strcmp(cutName,"CUTNEU")) 
    fGccuts->cutneu=cutValue; 
  else if(!strcmp(cutName,"CUTHAD")) 
    fGccuts->cuthad=cutValue; 
  else if(!strcmp(cutName,"CUTMUO")) 
    fGccuts->cutmuo=cutValue; 
  else if(!strcmp(cutName,"BCUTE")) 
    fGccuts->bcute=cutValue; 
  else if(!strcmp(cutName,"BCUTM")) 
    fGccuts->bcutm=cutValue; 
  else if(!strcmp(cutName,"DCUTE")) 
    fGccuts->dcute=cutValue; 
  else if(!strcmp(cutName,"DCUTM")) 
    fGccuts->dcutm=cutValue; 
  else if(!strcmp(cutName,"PPCUTM")) 
    fGccuts->ppcutm=cutValue; 
  else if(!strcmp(cutName,"TOFMAX")) 
    fGccuts->tofmax=cutValue; 
  else { Warning("SetCut","Cut %s not implemented\n",cutName);
      return 1;}
  return 0;
}

//_____________________________________________________________________________
Bool_t StTGeant3::SetProcess(const char* flagName, Int_t flagValue)
{
  //
  // Set thresholds for different processes
  //
  if(!strcmp(flagName,"PAIR")) 
    fGcphys->ipair=flagValue;
  else if(!strcmp(flagName,"COMP")) 
    fGcphys->icomp=flagValue;
  else if(!strcmp(flagName,"PHOT")) 
    fGcphys->iphot=flagValue;
  else if(!strcmp(flagName,"PFIS")) 
    fGcphys->ipfis=flagValue;
  else if(!strcmp(flagName,"DRAY")) 
    fGcphys->idray=flagValue;
  else if(!strcmp(flagName,"ANNI")) 
    fGcphys->ianni=flagValue;
  else if(!strcmp(flagName,"BREM")) 
    fGcphys->ibrem=flagValue;
  else if(!strcmp(flagName,"HADR")) 
    fGcphys->ihadr=flagValue;
  else if(!strcmp(flagName,"MUNU")) 
    fGcphys->imunu=flagValue;
  else if(!strcmp(flagName,"DCAY")) 
    fGcphys->idcay=flagValue;
  else if(!strcmp(flagName,"LOSS")) 
    fGcphys->iloss=flagValue;
  else if(!strcmp(flagName,"MULS")) 
    fGcphys->imuls=flagValue;
  else if(!strcmp(flagName,"RAYL")) 
    fGcphys->irayl=flagValue;
  else if(!strcmp(flagName,"STRA")) 
    fGcphlt->istra=flagValue;
  else if(!strcmp(flagName,"SYNC")) 
    fGcphlt->isync=flagValue;
  else if(!strcmp(flagName,"CKOV"))
    fGctlit->itckov = flagValue;
  else  Warning("SetFlag","Flag %s not implemented\n",flagName);
  return 0;
}
 
 //_____________________________________________________________________________
Bool_t StTGeant3::DefineParticle(Int_t pdg, const char* name, TMCParticleType type,
                    Double_t mass, Double_t charge, Double_t lifetime)
{
// 
// Set a user defined particle
// Function is ignored if particle with specified pdg
// aready exists and error report is printed.  
// ---

  // Check if particle with specified pdg aready exists
  // in StTGeant3
  if (IdFromPDG(pdg) > 0) {
    Error("SetParticle", "Particle already exists.");
    return 0;
  }  

  // Check if particle type is known to Geant3
  Int_t itrtyp = TransportMethod(type);
  if (itrtyp < 0) {
    Error("SetParticle", "Unknown particle transport.");
    return 0;
  }

  // Add particle to Geant3  
  Gspart(fNG3Particles++, name, itrtyp, mass, charge, lifetime);
 
  // Add particle to TDatabasePDG
  // (if it does not yet exist here)
  if (!TDatabasePDG::Instance()->GetParticle(pdg))
    TDatabasePDG::Instance()
      ->AddParticle(name, name, mass, kTRUE, 0, charge*3, 
                    ParticleClass(type).Data(), pdg);
  fPDGCode[fNPDGCodes++] = pdg;
  return 0;
}

//_____________________________________________________________________________
Bool_t  StTGeant3::DefineIon(const char* name, Int_t Z, Int_t A, Int_t Q, 
                         Double_t excEnergy, Double_t mass)
{
//
// Set a user defined ion.
// ---

  // Define pdgEncoding
  //
  Int_t pdg = 10000000 + 10000*Z + 10*A;
  Int_t pdgMax = pdg + 9;
  
  // Find isomer number which is not yet used
  while (TDatabasePDG::Instance()->GetParticle(pdg) &&
         pdg < pdgMax) 
      pdg++;
  if (TDatabasePDG::Instance()->GetParticle(pdg))
      Fatal("SetIon", "All isomer numbers are already used");

  // Particle properties
        // excitation energy not used by G3
  if (mass < 1e-09) mass = 0.9382723 * A;
     // approximative mass if not specified by user
  Double_t charge = Q;
  TMCParticleType partType = kPTIon;  
  Double_t lifetime = 1.e20;
  
  // Call DefineParticle now
  DefineParticle(pdg, name, partType, mass, charge, lifetime);
  return 0;
}		       

//_____________________________________________________________________________
TString  StTGeant3::ParticleName(Int_t pdg) const
{
//  Return G3 particle name
// ---

  char name[21];
  Int_t itrtyp;
  Float_t amass, charge, tlife;
  Gfpart(pdg, name, itrtyp,amass, charge, tlife);
  name[20] = '\0';
  
  return TString(name);  
}
	  
//_____________________________________________________________________________
Double_t  StTGeant3::ParticleMass(Int_t pdg) const	  
{
//  Return G3 particle mass
// ---

  char name[20];
  Int_t itrtyp;
  Float_t mass, charge, tlife;
  Gfpart(pdg,name, itrtyp, mass, charge, tlife);
  
  return mass;  
}
	  
//_____________________________________________________________________________
Double_t  StTGeant3::ParticleCharge(Int_t pdg) const	  
{
// Return G3 particle charge (in e)
// ---

  char name[20];
  Int_t itrtyp;
  Float_t mass, charge, tlife;
  Gfpart(pdg,name, itrtyp, mass, charge, tlife);
  
  return charge;  
}

//_____________________________________________________________________________
Double_t  StTGeant3::ParticleLifeTime(Int_t pdg) const	  
{
// Return G3 particle life time
// ---

  char name[20];
  Int_t itrtyp;
  Float_t mass, charge, tlife;
  Gfpart(pdg, name, itrtyp, mass, charge, tlife);
  
  return tlife;  
}

//_____________________________________________________________________________
TMCParticleType StTGeant3::ParticleMCType(Int_t pdg) const
{
// Return MC particle type 
// ---

  char name[20];
  Int_t itrtyp;
  Float_t mass, charge, tlife;
  Gfpart(pdg,name, itrtyp, mass, charge, tlife);
  
  return ParticleType(itrtyp);  
}


//_____________________________________________________________________________
Double_t StTGeant3::Xsec(char* reac, Double_t /* energy */, 
		      Int_t part, Int_t /* mate */)
{
  //
  // Calculate X-sections -- dummy for the moment
  //
  if(!strcmp(reac,"PHOT"))
  {
    if(part!=22) {
      Error("Xsec","Can calculate photoelectric only for photons\n");
    }
  }
  return 0;
}

//_____________________________________________________________________________
void StTGeant3::TrackPosition(TLorentzVector &xyz) const
{
  //
  // Return the current position in the master reference frame of the
  // track being transported
  //
  xyz[0]=fGctrak->vect[0];
  xyz[1]=fGctrak->vect[1];
  xyz[2]=fGctrak->vect[2];
  xyz[3]=fGctrak->tofg;
}

//_____________________________________________________________________________
void StTGeant3::TrackPosition(Double_t &x, Double_t &y, Double_t &z) const
{
  //
  // Return the current position in the master reference frame of the
  // track being transported
  //
  x=fGctrak->vect[0];
  y=fGctrak->vect[1];
  z=fGctrak->vect[2];
}

//_____________________________________________________________________________
Double_t StTGeant3::TrackTime() const
{
  //
  // Return the current time of flight of the track being transported
  //
  return fGctrak->tofg;
}

//_____________________________________________________________________________
void StTGeant3::TrackMomentum(TLorentzVector &xyz) const
{
  //
  // Return the direction and the momentum (GeV/c) of the track
  // currently being transported
  //
  Double_t ptot=fGctrak->vect[6];
  xyz[0]=fGctrak->vect[3]*ptot;
  xyz[1]=fGctrak->vect[4]*ptot;
  xyz[2]=fGctrak->vect[5]*ptot;
  xyz[3]=fGctrak->getot;
}

//_____________________________________________________________________________
void StTGeant3::TrackMomentum(Double_t &px, Double_t &py, Double_t &pz, Double_t &etot) const
{
  //
  // Return the direction and the momentum (GeV/c) of the track
  // currently being transported
  //
  Double_t ptot=fGctrak->vect[6];
  px  =fGctrak->vect[3]*ptot;
  py  =fGctrak->vect[4]*ptot;
  pz  =fGctrak->vect[5]*ptot;
  etot=fGctrak->getot;
}

//_____________________________________________________________________________
Double_t StTGeant3::TrackCharge() const
{
  //
  // Return charge of the track currently transported
  //
  return fGckine->charge;
}

//_____________________________________________________________________________
Double_t StTGeant3::TrackMass() const
{
  //
  // Return the mass of the track currently transported
  //
  return fGckine->amass;
}

//_____________________________________________________________________________
Int_t StTGeant3::TrackPid() const
{
  //
  // Return the id of the particle transported
  //
  return PDGFromId(fGckine->ipart);
}

//_____________________________________________________________________________
Double_t StTGeant3::TrackStep() const
{
  //
  // Return the length in centimeters of the current step
  //
  return fGctrak->step;
}

//_____________________________________________________________________________
Double_t StTGeant3::TrackLength() const
{
  //
  // Return the length of the current track from its origin
  //
  return fGctrak->sleng;
}

//_____________________________________________________________________________
Bool_t StTGeant3::IsNewTrack() const
{
  //
  // True if the track is not at the boundary of the current volume
  //
  return (fGctrak->sleng==0);
}

//_____________________________________________________________________________
Bool_t StTGeant3::IsTrackInside() const
{
  //
  // True if the track is not at the boundary of the current volume
  //
  return (fGctrak->inwvol==0);
}

//_____________________________________________________________________________
Bool_t StTGeant3::IsTrackEntering() const
{
  //
  // True if this is the first step of the track in the current volume
  //
  return (fGctrak->inwvol==1);
}

//_____________________________________________________________________________
Bool_t StTGeant3::IsTrackExiting() const
{
  //
  // True if this is the last step of the track in the current volume
  //
  return (fGctrak->inwvol==2);
}

//_____________________________________________________________________________
Bool_t StTGeant3::IsTrackOut() const
{
  //
  // True if the track is out of the setup
  //
  return (fGctrak->inwvol==3);
}

//_____________________________________________________________________________
Bool_t StTGeant3::IsTrackStop() const
{
  //
  // True if the track energy has fallen below the threshold 
  //
  return (fGctrak->istop==2);
}

//_____________________________________________________________________________
Int_t   StTGeant3::NSecondaries() const
{
  //
  // Number of secondary particles generated in the current step
  //
  return fGcking->ngkine;
}

//_____________________________________________________________________________
Int_t   StTGeant3::CurrentEvent() const
{
  //
  // Number of the current event
  //
  return fGcflag->idevt;
}

//_____________________________________________________________________________
TMCProcess StTGeant3::ProdProcess(Int_t ) const
{
  //
  // Name of the process that has produced the secondary particles
  // in the current step
  //
  const TMCProcess kIpProc[13] = { kPDecay, kPPair, kPCompton, 
			      kPPhotoelectric, kPBrem, kPDeltaRay,
			      kPAnnihilation, kPHadronic, 
			      kPMuonNuclear, kPPhotoFission,
			      kPRayleigh, kPCerenkov, kPSynchrotron};
  Int_t km, im;
  //
  if(fGcking->ngkine>0) 
    for (km = 0; km < fGctrak->nmec; ++km) 
      for (im = 0; im < 13; ++im) 
	if (G3toVMC(fGctrak->lmec[km]) == kIpProc[im]) 
	    return kIpProc[im];
  //  
  return kPNoProcess;
}

//_____________________________________________________________________________
Int_t StTGeant3::StepProcesses(TArrayI &proc) const
{
  //
  // Return processes active in the current step
  //
  Int_t i;
  Int_t nproc=Gctrak()->nmec;
  //
  proc.Set(nproc);
  Int_t nvproc=0;
  //
  for (i=0; i<nproc; ++i) 
    if((proc[nvproc]=G3toVMC(Gctrak()->lmec[i]))!=kPNoProcess) nvproc++;
  //
  proc.Set(nvproc);
  //
  return nvproc;
}
      
//_____________________________________________________________________________
TMCProcess StTGeant3::G3toVMC(Int_t iproc) const
{
  //
  // Conversion between GEANT and TMC processes
  //
  
  const TMCProcess kPG2MC1[30] = {kPNoProcess, kPMultipleScattering, kPEnergyLoss, kPMagneticFieldL, kPDecay, 
			     kPPair, kPCompton, kPPhotoelectric, kPBrem, kPDeltaRay,
			     kPAnnihilation, kPHadronic, kPNoProcess, kPEvaporation, kPNuclearFission, 
			     kPNuclearAbsorption, kPPbarAnnihilation, kPNCapture, kPHElastic, kPHInhelastic,
			     kPMuonNuclear, kPTOFlimit, kPPhotoFission, kPNoProcess, kPRayleigh,
			     kPNoProcess, kPNoProcess, kPNoProcess, kPNull, kPStop};
  
  const TMCProcess kPG2MC2[9] = {kPLightAbsorption, kPLightScattering, kStepMax, kPNoProcess, kPCerenkov,
			    kPLightReflection, kPLightRefraction, kPSynchrotron, kPNoProcess};

  TMCProcess proc=kPNoProcess;
  if(1<iproc && iproc<=30) proc= kPG2MC1[iproc-1];
  else if(101<=iproc && iproc<=109) proc= kPG2MC2[iproc-100-1];
  return proc;
}


//_____________________________________________________________________________
void    StTGeant3::GetSecondary(Int_t isec, Int_t& ipart, 
			      TLorentzVector &x, TLorentzVector &p)
{
  //
  // Get the parameters of the secondary track number isec produced
  // in the current step
  //
  Int_t i;
  if(-1<isec && isec<fGcking->ngkine) {
    ipart=Int_t (fGcking->gkin[isec][4] +0.5);
    for(i=0;i<3;i++) {
      x[i]=fGckin3->gpos[isec][i];
      p[i]=fGcking->gkin[isec][i];
    }
    x[3]=fGcking->tofd[isec];
    p[3]=fGcking->gkin[isec][3];
  } else {
    printf(" * StTGeant3::GetSecondary * Secondary %d does not exist\n",isec);
    x[0]=x[1]=x[2]=x[3]=p[0]=p[1]=p[2]=p[3]=0;
    ipart=0;
  }
}

//_____________________________________________________________________________
void StTGeant3::InitLego()
{
  //
  // Set switches for lego transport
  //
  SetSWIT(4,0);
  SetDEBU(0,0,0);  //do not print a message 
}

//_____________________________________________________________________________
Bool_t StTGeant3::IsTrackDisappeared() const
{
  //
  // True if the current particle has disappered
  // either because it decayed or because it underwent
  // an inelastic collision
  //
  return (fGctrak->istop==1);
}

//_____________________________________________________________________________
Bool_t StTGeant3::IsTrackAlive() const
{
  //
  // True if the current particle is alive and will continue to be
  // transported
  //
  return (fGctrak->istop==0);
}

//_____________________________________________________________________________
void StTGeant3::StopTrack()
{
  //
  // Stop the transport of the current particle and skip to the next
  //
  fGctrak->istop=1;
}

//_____________________________________________________________________________
void StTGeant3::StopEvent()
{
  //
  // Stop simulation of the current event and skip to the next
  //
  fGcflag->ieotri=1;
}

//_____________________________________________________________________________
Double_t StTGeant3::MaxStep() const
{
  //
  // Return the maximum step length in the current medium
  //
  return fGctmed->stemax;
}

//_____________________________________________________________________________
void StTGeant3::SetMaxStep(Double_t maxstep)
{
  //
  // Set the maximum step allowed till the particle is in the current medium
  //
  fGctmed->stemax=maxstep;
}

//_____________________________________________________________________________
void StTGeant3::SetMaxNStep(Int_t maxnstp)
{
  //
  // Set the maximum number of steps till the particle is in the current medium
  //
  fGctrak->maxnst=maxnstp;
}

//_____________________________________________________________________________
Int_t StTGeant3::GetMaxNStep() const
{
  //
  // Maximum number of steps allowed in current medium
  //
  return fGctrak->maxnst;
}

//_____________________________________________________________________________
void StTGeant3::G3Material(Int_t& kmat, const char* name, Double_t a, Double_t z,
		         Double_t dens, Double_t radl, Double_t absl, Float_t* buf,
		         Int_t nwbuf)
{
  //
  // Defines a Material
  // 
  //  kmat               number assigned to the material
  //  name               material name
  //  a                  atomic mass in au
  //  z                  atomic number
  //  dens               density in g/cm3
  //  absl               absorbtion length in cm
  //                     if >=0 it is ignored and the program 
  //                     calculates it, if <0. -absl is taken
  //  radl               radiation length in cm
  //                     if >=0 it is ignored and the program 
  //                     calculates it, if <0. -radl is taken
  //  buf                pointer to an array of user words
  //  nbuf               number of user words
  //
  Int_t jmate=fGclink->jmate;
  kmat=1;
  if(jmate>0) {
    Int_t ns = fZiq[jmate-2];
    kmat=ns+1;
    for(int i=1; i<=ns; i++) {
      if(fZlq[jmate-i]==0) {
	kmat=i;
	break;
      }
    }
  }
  Float_t fa = a;
  Float_t fz = z;
  Float_t fdens = dens;
  Float_t fradl = radl;
  Float_t fabsl = absl;
  
  g3smate(kmat,PASSCHARD(name), fa,  fz, fdens, fradl, fabsl, buf,
	 nwbuf PASSCHARL(name)); 
}

//_____________________________________________________________________________
void StTGeant3::Material(Int_t& kmat, const char* name, Double_t a, Double_t z,
		       Double_t dens, Double_t radl, Double_t absl, Float_t* buf,
		       Int_t nwbuf)
{
  //
  // Defines a Material
  // 
  //  kmat               number assigned to the material
  //  name               material name
  //  a                  atomic mass in au
  //  z                  atomic number
  //  dens               density in g/cm3
  //  absl               absorbtion length in cm
  //                     if >=0 it is ignored and the program 
  //                     calculates it, if <0. -absl is taken
  //  radl               radiation length in cm
  //                     if >=0 it is ignored and the program 
  //                     calculates it, if <0. -radl is taken
  //  buf                pointer to an array of user words
  //  nbuf               number of user words
  //

  G3Material(kmat, name, a, z, dens, radl, absl, buf, nwbuf);
  
  if (Mode()&kWithRoot){
    fMCGeo->Material(kmat, name, a, z, dens, radl, absl, buf, nwbuf);
  }
}

//_____________________________________________________________________________
void StTGeant3::Material(Int_t& kmat, const char* name, Double_t a, Double_t z,
		       Double_t dens, Double_t radl, Double_t absl, Double_t* buf,
		       Int_t nwbuf)
{
  //
  // Defines a Material
  // 
  //  kmat               number assigned to the material
  //  name               material name
  //  a                  atomic mass in au
  //  z                  atomic number
  //  dens               density in g/cm3
  //  absl               absorbtion length in cm
  //                     if >=0 it is ignored and the program 
  //                     calculates it, if <0. -absl is taken
  //  radl               radiation length in cm
  //                     if >=0 it is ignored and the program 
  //                     calculates it, if <0. -radl is taken
  //  buf                pointer to an array of user words
  //  nbuf               number of user words
  //

  
  Float_t* fbuf = CreateFloatArray(buf, nwbuf);  
  G3Material(kmat, name, a, z, dens, radl, absl, fbuf, nwbuf);
  delete [] fbuf;

  if (Mode()&kWithRoot){
    fMCGeo->Material(kmat, name, a, z, dens, radl, absl, buf, nwbuf);
  }
}

//_____________________________________________________________________________
void StTGeant3::G3Mixture(Int_t& kmat, const char* name, Float_t* a, Float_t* z, 
		        Double_t dens, Int_t nlmat, Float_t* wmat)
{
  //
  // Defines mixture OR COMPOUND IMAT as composed by 
  // THE BASIC NLMAT materials defined by arrays A,Z and WMAT
  // 
  // If NLMAT > 0 then wmat contains the proportion by
  // weights of each basic material in the mixture. 
  // 
  // If nlmat < 0 then WMAT contains the number of atoms 
  // of a given kind into the molecule of the COMPOUND
  // In this case, WMAT in output is changed to relative
  // weigths.
  //
  Int_t jmate=fGclink->jmate;
  kmat=1;
  if(jmate>0) {
    Int_t ns = fZiq[jmate-2];
    kmat=ns+1;
    for(int i=1; i<=ns; i++) {
      if(fZlq[jmate-i]==0) {
	kmat=i;
	break;
      }
    }
  }

  g3smixt(kmat,PASSCHARD(name), a, z,Float_t(dens), nlmat,wmat PASSCHARL(name)); 
  
  if (nlmat < 0) {
     nlmat = - nlmat;
     Double_t amol = 0;
     for (int i=0;i<nlmat;i++) {
        amol += a[i]*wmat[i];
     }
     for (int i=0;i<nlmat;i++) {
        wmat[i] *= a[i]/amol;
     }
  }
}

//_____________________________________________________________________________
void StTGeant3::Mixture(Int_t& kmat, const char* name, Float_t* a, Float_t* z, 
		      Double_t dens, Int_t nlmat, Float_t* wmat)
{
  //
  // Defines mixture OR COMPOUND IMAT as composed by 
  // THE BASIC NLMAT materials defined by arrays A,Z and WMAT
  // 
  // If NLMAT > 0 then wmat contains the proportion by
  // weights of each basic material in the mixture. 
  // 
  // If nlmat < 0 then WMAT contains the number of atoms 
  // of a given kind into the molecule of the COMPOUND
  // In this case, WMAT in output is changed to relative
  // weigths.
  //

  G3Mixture(kmat, name, a, z, dens, nlmat, wmat);
  
  if (Mode()&kWithRoot){
    fMCGeo->Mixture(kmat, name, a, z, dens, TMath::Abs(nlmat), wmat);
  }
}

//_____________________________________________________________________________
void StTGeant3::Mixture(Int_t& kmat, const char* name, Double_t* a, Double_t* z, 
		      Double_t dens, Int_t nlmat, Double_t* wmat)
{
  //
  // Defines mixture OR COMPOUND IMAT as composed by 
  // THE BASIC NLMAT materials defined by arrays A,Z and WMAT
  // 
  // If NLMAT > 0 then wmat contains the proportion by
  // weights of each basic material in the mixture. 
  // 
  // If nlmat < 0 then WMAT contains the number of atoms 
  // of a given kind into the molecule of the COMPOUND
  // In this case, WMAT in output is changed to relative
  // weigths.
  //

  Float_t* fa = CreateFloatArray(a, TMath::Abs(nlmat));  
  Float_t* fz = CreateFloatArray(z, TMath::Abs(nlmat));  
  Float_t* fwmat = CreateFloatArray(wmat, TMath::Abs(nlmat));  

  G3Mixture(kmat, name, fa, fz, dens, nlmat, fwmat);
  Int_t i;
  for (i=0; i<TMath::Abs(nlmat); i++) {
    a[i] = fa[i]; z[i] = fz[i]; wmat[i] = fwmat[i];
  }  

  delete [] fa;
  delete [] fz;
  delete [] fwmat;

  if (Mode()&kWithRoot){
    fMCGeo->Mixture(kmat, name, a, z, dens, TMath::Abs(nlmat), wmat);
  }
}

//_____________________________________________________________________________
void StTGeant3::G3Medium(Int_t& kmed, const char* name, Int_t nmat, Int_t isvol,
		     Int_t ifield, Double_t fieldm, Double_t tmaxfd,
		     Double_t stemax, Double_t deemax, Double_t epsil,
		     Double_t stmin, Float_t* ubuf, Int_t nbuf)
{
  //
  //  kmed      tracking medium number assigned
  //  name      tracking medium name
  //  nmat      material number
  //  isvol     sensitive volume flag
  //  ifield    magnetic field
  //  fieldm    max. field value (kilogauss)
  //  tmaxfd    max. angle due to field (deg/step)
  //  stemax    max. step allowed
  //  deemax    max. fraction of energy lost in a step
  //  epsil     tracking precision (cm)
  //  stmin     min. step due to continuous processes (cm)
  //
  //  ifield = 0 if no magnetic field; ifield = -1 if user decision in guswim;
  //  ifield = 1 if tracking performed with g3rkuta; ifield = 2 if tracking
  //  performed with g3helix; ifield = 3 if tracking performed with g3helx3.
  //  
  Int_t jtmed=fGclink->jtmed;
  kmed=1;
  if(jtmed>0) {
    Int_t ns = fZiq[jtmed-2];
    kmed=ns+1;
    for(int i=1; i<=ns; i++) {
      if(fZlq[jtmed-i]==0) {
	kmed=i;
	break;
      }
    }
  }
  Float_t ffieldm = fieldm;
  Float_t ftmaxfd = tmaxfd;
  Float_t fstemax = stemax;
  Float_t fdeemax = deemax;
  Float_t fepsil  = epsil;
  Float_t fstmin =  stmin;  
  g3stmed(kmed, PASSCHARD(name), nmat, isvol, ifield, ffieldm, ftmaxfd, fstemax,
	 fdeemax, fepsil, fstmin, ubuf, nbuf PASSCHARL(name)); 
}

//_____________________________________________________________________________
void StTGeant3::Medium(Int_t& kmed, const char* name, Int_t nmat, Int_t isvol,
		     Int_t ifield, Double_t fieldm, Double_t tmaxfd,
		     Double_t stemax, Double_t deemax, Double_t epsil,
		     Double_t stmin, Float_t* ubuf, Int_t nbuf)
{
  //
  //  kmed      tracking medium number assigned
  //  name      tracking medium name
  //  nmat      material number
  //  isvol     sensitive volume flag
  //  ifield    magnetic field
  //  fieldm    max. field value (kilogauss)
  //  tmaxfd    max. angle due to field (deg/step)
  //  stemax    max. step allowed
  //  deemax    max. fraction of energy lost in a step
  //  epsil     tracking precision (cm)
  //  stmin     min. step due to continuous processes (cm)
  //
  //  ifield = 0 if no magnetic field; ifield = -1 if user decision in guswim;
  //  ifield = 1 if tracking performed with g3rkuta; ifield = 2 if tracking
  //  performed with g3helix; ifield = 3 if tracking performed with g3helx3.
  //  

  G3Medium(kmed, name, nmat, isvol, ifield, fieldm, tmaxfd, stemax, deemax, epsil,
           stmin, ubuf, nbuf);

  if (Mode()&kWithRoot){
    fMCGeo->Medium(kmed, name, nmat, isvol, ifield, fieldm, tmaxfd, stemax, deemax, 
                   epsil, stmin, ubuf, nbuf);
  }
}

//_____________________________________________________________________________
void StTGeant3::Medium(Int_t& kmed, const char* name, Int_t nmat, Int_t isvol,
		     Int_t ifield, Double_t fieldm, Double_t tmaxfd,
		     Double_t stemax, Double_t deemax, Double_t epsil,
		     Double_t stmin, Double_t* ubuf, Int_t nbuf)
{
  //
  //  kmed      tracking medium number assigned
  //  name      tracking medium name
  //  nmat      material number
  //  isvol     sensitive volume flag
  //  ifield    magnetic field
  //  fieldm    max. field value (kilogauss)
  //  tmaxfd    max. angle due to field (deg/step)
  //  stemax    max. step allowed
  //  deemax    max. fraction of energy lost in a step
  //  epsil     tracking precision (cm)
  //  stmin     min. step due to continuos processes (cm)
  //
  //  ifield = 0 if no magnetic field; ifield = -1 if user decision in guswim;
  //  ifield = 1 if tracking performed with g3rkuta; ifield = 2 if tracking
  //  performed with g3helix; ifield = 3 if tracking performed with g3helx3.
  //  

  Float_t* fubuf = CreateFloatArray(ubuf, nbuf);  
  G3Medium(kmed, name, nmat, isvol, ifield, fieldm, tmaxfd, stemax, deemax, epsil,
           stmin, fubuf, nbuf);
  delete [] fubuf;	 

  if (Mode()&kWithRoot){
    fMCGeo->Medium(kmed, name, nmat, isvol, ifield, fieldm, tmaxfd, stemax, deemax, 
                 epsil, stmin, ubuf, nbuf);
  }		 
}

//_____________________________________________________________________________
void StTGeant3::Matrix(Int_t& krot, Double_t thex, Double_t phix, Double_t they,
		     Double_t phiy, Double_t thez, Double_t phiz)
{
  //
  //  krot     rotation matrix number assigned
  //  theta1   polar angle for axis i
  //  phi1     azimuthal angle for axis i
  //  theta2   polar angle for axis ii
  //  phi2     azimuthal angle for axis ii
  //  theta3   polar angle for axis iii
  //  phi3     azimuthal angle for axis iii
  //
  //  it defines the rotation matrix number irot.
  //  
  krot = -1;
  if (Mode()&kWithG3){
    Int_t jrotm=fGclink->jrotm;
    krot=1;
    Int_t ns, i;
    if(jrotm>0) {
      ns=fZiq[jrotm-2];
      krot=ns+1;
      for(i=1; i<=ns; i++) {
	if(fZlq[jrotm-i]==0) {
	  krot=i;
	  break;
	}
      }
  }
  g3srotm(krot, thex, phix, they, phiy, thez, phiz);
  } else {
    fMCGeo->Matrix(krot, thex, phix, they, phiy, thez, phiz);  
  }
}

//_____________________________________________________________________________
Int_t StTGeant3::GetMedium() const
{
  //
  // Return the number of the current medium
  //
  if (Mode()&kWithRoot){
  Int_t imed = 0;
  TGeoNode *node = gGeoManager->GetCurrentNode();
  if (!node) imed = gGeoManager->GetTopNode()->GetVolume()->GetMedium()->GetId();
  else       imed = node->GetVolume()->GetMedium()->GetId();
  //printf("==GetMedium: ROOT id=%i  numed=%i\n", imed,fGctmed->numed);
  return imed;
}
  return fGctmed->numed;
}

//_____________________________________________________________________________
void  StTGeant3::SetRootGeometry()
{
// Notify Geant3 about use of TGeo geometry.
// The materials and tracking medias will be imported from
// TGeo at FinishGeometry().

  fImportRootGeometry = kTRUE;
}  			

//_____________________________________________________________________________
Int_t StTGeant3::GetLevel() const
{
  if (Mode()&kWithG3) return fGcvolu->nlevel; 
  else                return gGeoManager->GetLevel();  
}      
//_____________________________________________________________________________
const char *StTGeant3::GetPath()
{
// Get current path inside G3 geometry
  if (Mode()&kWithG3){
   Int_t i,j;
   if ((i=fGcvolu->nlevel-1)<0) {
      Warning("GetPath", "level null");
      return fPath;
   }
   fPath[0] = '/';
   char name[10];
   char *namcur = fPath+1;
   Int_t gname, copy;
   Int_t nch=0;
   for (j=0; j<i+1; j++) { 
      gname = fGcvolu->names[j];
      copy = fGcvolu->number[j];   
      memcpy(name, &gname, 4);
      name[4]=0;
      sprintf(namcur, "%s_%d/", name, copy);
      nch = strlen(fPath);
      namcur = fPath+nch;
   }
   fPath[nch-1]=0;
   return fPath;
  } else {
   return gGeoManager->GetPath();
  }
}      

//_____________________________________________________________________________
const char *StTGeant3::GetNodeName()
{
// Get name of current G3 node
  if (Mode()&kWithG3){
     Int_t i=fGcvolu->nlevel-1;
     if (i<0) return "";
     Int_t gname = fGcvolu->names[i];
     Int_t copy = fGcvolu->number[i];
     char name[10];
     memcpy(name, &gname, 4);
     name[4] = 0;
     sprintf(fPath, "%s_%d", name, copy);
     return fPath;
  } else {
     if (gGeoManager->IsOutside()) return "";
     return gGeoManager->GetCurrentNode()->GetName();
  }
}

//_____________________________________________________________________________
Double_t StTGeant3::Edep() const
{
  //
  // Return the energy lost in the current step
  //
  return fGctrak->destep;
}

//_____________________________________________________________________________
Double_t StTGeant3::Etot() const
{
  //
  // Return the total energy of the current track
  //
  return fGctrak->getot;
}

//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//                        Functions from GBASE
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

//____________________________________________________________________________ 
void  StTGeant3::Gfile(const char *filename, const char *option) 
{ 
  //
  //    Routine to open a GEANT/RZ data base. 
  //
  //    LUN logical unit number associated to the file 
  //
  //    CHFILE RZ file name   
  //  
  //    CHOPT is a character string which may be  
  //        N  To create a new file 
  //        U  to open an existing file for update 
  //       " " to open an existing file for read only
  //        Q  The initial allocation (default 1000 records) 
  //           is given in IQUEST(10)
  //        X  Open the file in exchange format
  //        I  Read all data structures from file to memory 
  //        O  Write all data structures from memory to file 
  // 
  // Note:
  //      If options "I"  or "O" all data structures are read or
  //         written from/to file and the file is closed. 
  //      See routine GRMDIR to create subdirectories  
  //      See routines GROUT,GRIN to write,read objects 
  //  
  //g3rfile(21, PASSCHARD(filename), PASSCHARD(option) PASSCHARL(filename)
//	 PASSCHARL(option)); 
} 
 
//____________________________________________________________________________ 
void  StTGeant3::Gpcxyz() 
{ 
  //
  //    Print track and volume parameters at current point
  //
    
    g3pcxyz(); 
} 
//_____________________________________________________________________________
void  StTGeant3::Ggclos() 
{ 
  //
  //   Closes off the geometry setting.
  //   Initializes the search list for the contents of each
  //   volume following the order they have been positioned, and
  //   inserting the content '0' when a call to GSNEXT (-1) has
  //   been required by the user.
  //   Performs the development of the JVOLUM structure for all 
  //   volumes with variable parameters, by calling GGDVLP. 
  //   Interprets the user calls to GSORD, through GGORD.
  //   Computes and stores in a bank (next to JVOLUM mother bank)
  //   the number of levels in the geometrical tree and the
  //   maximum number of contents per level, by calling GGNLEV.
  //   Sets status bit for CONCAVE volumes, through GGCAVE.
  //   Completes the JSET structure with the list of volume names 
  //   which identify uniquely a given physical detector, the
  //   list of bit numbers to pack the corresponding volume copy 
  //   numbers, and the generic path(s) in the JVOLUM tree, 
  //   through the routine GHCLOS. 
  //
  if (Mode()&kWithG3){
  g3gclos(); 
  // Create internal list of volumes
  fVolNames = new char[fGcnum->nvolum+1][5];
  Int_t i;
  for(i=0; i<fGcnum->nvolum; ++i) {
    strncpy(fVolNames[i], (char *) &fZiq[fGclink->jvolum+i+1], 4);
    fVolNames[i][4]='\0';
  }
  strcpy(fVolNames[fGcnum->nvolum],"NULL");
  } else {
    fVolNames = 0;
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Glast() 
{ 
  //
  // Finish a Geant run
  //
  g3last();
  printf("count_gmedia= %8d\n",count_gmedia);
  printf("count_gtmedi= %8d\n",count_gtmedi);
  printf("count_ginvol= %8d\n",count_ginvol);
  printf("count_gtnext= %8d\n",count_gtnext);
#ifdef STATISTICS
   stattree->AutoSave();
#endif
   
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gprint(const char *name) 
{ 
  //
  // Routine to print data structures
  // CHNAME   name of a data structure
  // 
  char vname[5];
  Vname(name,vname);
  if (Mode()&kWithG3){
  g3print(PASSCHARD(vname),0 PASSCHARL(vname)); 
  }
} 

//_____________________________________________________________________________
void  StTGeant3::Grun() 
{ 
  //
  // Steering function to process one run
  //
  g3run(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gtrig() 
{ 
  //
  // Steering function to process one event
  //  
  g3trig();
   
  //printf("count_gmedia= %8d\n",count_gmedia);
  //printf("count_gtmedi= %8d\n",count_gtmedi);
  //printf("count_ginvol= %8d\n",count_ginvol);
  //printf("count_gtnext= %8d\n",count_gtnext);
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gtrigc() 
{ 
  //
  // Clear event partition
  //
  g3trigc(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gtrigi() 
{ 
  //
  // Initialises event partition
  //
  g3trigi(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gwork(Int_t nwork) 
{ 
  //
  // Allocates workspace in ZEBRA memory
  //
  g3work(nwork); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gzinit() 
{ 
  //
  // To initialise GEANT/ZEBRA data structures
  //
  g3zinit(); 
} 
 
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//                        Functions from GCONS
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 
//_____________________________________________________________________________
void  StTGeant3::Gfmate(Int_t imat, char *name, Float_t &a, Float_t &z,  
		      Float_t &dens, Float_t &radl, Float_t &absl,
		      Float_t* ubuf, Int_t& nbuf) 
{ 
  //
  // Return parameters for material IMAT 
  //
  char cbuf[41];
  memset(cbuf,'@',40); cbuf[40]=0;
  g3fmate(imat, PASSCHARD(cbuf), a, z, dens, radl, absl, ubuf, nbuf
	 PASSCHARL(cbuf)); 
  char *bla = strchr(cbuf,'@');
  if (bla) *bla=0; 
  bla = cbuf+strlen(cbuf)-1;
  while(*bla==' ') { (bla--)[0]=0;}
  strcpy(name,cbuf);
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gfmate(Int_t imat, char *name, Double_t &a, Double_t &z,  
		      Double_t &dens, Double_t &radl, Double_t &absl,
		      Double_t* ubuf, Int_t& nbuf) 
{ 
  //
  // Return parameters for material IMAT 
  //
  Float_t fa = a;
  Float_t fz = z;
  Float_t fdens = dens;
  Float_t fradl = radl;
  Float_t fabsl = absl;  
  Float_t* fubuf = CreateFloatArray(ubuf, nbuf);
    
  Gfmate(imat, name, fa, fz, fdens, fradl, fabsl, fubuf, nbuf);

  a = fa;
  z = fz;
  dens = fdens;
  radl = fradl;
  absl = fabsl;	 
  for (Int_t i=0; i<nbuf; i++) ubuf[i] = fubuf[i];
  
  delete [] fubuf;
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gfpart(Int_t ipart, char *name, Int_t &itrtyp,  
		   Float_t &amass, Float_t &charge, Float_t &tlife) const
{ 
  //
  // Return parameters for particle of type IPART
  //
  Float_t *ubuf=0; 
  Int_t   nbuf;
  Int_t igpart = IdFromPDG(ipart);
  char cbuf[41];
  memset(cbuf,'@',40); cbuf[40]=0;
  g3fpart(igpart, PASSCHARD(cbuf), itrtyp, amass, charge, tlife, ubuf, nbuf
	 PASSCHARL(cbuf)); 
  char *bla = strchr(cbuf,'@');
  if (bla) *bla=0; 
  bla = cbuf+strlen(cbuf)-1;
  while(*bla==' ') { (bla--)[0]=0;}
  strcpy(name,cbuf);
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gftmed(Int_t numed, char *name, Int_t &nmat, Int_t &isvol,  
		   Int_t &ifield, Float_t &fieldm, Float_t &tmaxfd, 
		    Float_t &stemax, Float_t &deemax, Float_t &epsil, 
		    Float_t &stmin, Float_t *ubuf, Int_t *nbuf) 
{ 
  //
  // Return parameters for tracking medium NUMED
  //
  char cbuf[41];
  memset(cbuf,'@',40); cbuf[40]=0;
  g3ftmed(numed, PASSCHARD(cbuf), nmat, isvol, ifield, fieldm, tmaxfd, stemax,  
         deemax, epsil, stmin, ubuf, nbuf PASSCHARL(cbuf)); 
  
  char *bla = strchr(cbuf,'@');
  if (bla) *bla=0; 
  bla = cbuf+strlen(cbuf)-1;
  while(*bla==' ') { (bla--)[0]=0;}
  strcpy(name,cbuf);
}

 
//_____________________________________________________________________________
 void  StTGeant3::Gftmat(Int_t imate, Int_t ipart, char *chmeca, Int_t kdim,
		      Float_t* tkin, Float_t* value, Float_t* pcut,
		      Int_t &ixst)
{ 
  //
  // Return parameters for material imate
  //
  char cbuf[41];
  memset(cbuf,'@',40); cbuf[40]=0;
   g3ftmat(imate, ipart, PASSCHARD(cbuf), kdim,
	 tkin, value, pcut, ixst PASSCHARL(cbuf));
  char *bla = strchr(cbuf,'@');
  if (bla) *bla=0; 
  bla = cbuf+strlen(cbuf)-1;
  while(*bla==' ') { (bla--)[0]=0;}
  strcpy(chmeca,cbuf);

} 

//_____________________________________________________________________________
Float_t StTGeant3::Gbrelm(Float_t z, Float_t t, Float_t bcut)
{
  //
  // To calculate energy loss due to soft muon BREMSSTRAHLUNG
  //
  return g3brelm(z,t,bcut);
}

//_____________________________________________________________________________
Float_t StTGeant3::Gprelm(Float_t z, Float_t t, Float_t bcut)
{
  //
  // To calculate DE/DX in GeV*barn/atom for direct pair production by muons
  //
  return g3prelm(z,t,bcut);
}
 
//_____________________________________________________________________________
void  StTGeant3::Gmate() 
{ 
  //
  // Define standard GEANT materials
  //
  g3mate(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gpart() 
{ 
  //
  //  Define standard GEANT particles plus selected decay modes
  //  and branching ratios.
  //
  g3part(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsdk(Int_t ipart, Float_t *bratio, Int_t *mode) 
{ 
//  Defines branching ratios and decay modes for standard
//  GEANT particles.
   g3sdk(ipart,bratio,mode); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsmate(Int_t imat, const char *name, Float_t a, Float_t z,  
		   Float_t dens, Float_t radl, Float_t absl) 
{ 
  //
  // Defines a Material
  // 
  //  kmat               number assigned to the material
  //  name               material name
  //  a                  atomic mass in au
  //  z                  atomic number
  //  dens               density in g/cm3
  //  absl               absorbtion length in cm
  //                     if >=0 it is ignored and the program 
  //                     calculates it, if <0. -absl is taken
  //  radl               radiation length in cm
  //                     if >=0 it is ignored and the program 
  //                     calculates it, if <0. -radl is taken
  //  buf                pointer to an array of user words
  //  nbuf               number of user words
  //
  Float_t *ubuf=0; 
  Int_t   nbuf=0; 
  if (dens <= 0 && a != 0 && z != 0) {
     Warning("Gsmate","Density was o, set to 0.01 for imat=%d, name=%s",imat,name);
     dens = 0.01;
  }
  g3smate(imat,PASSCHARD(name), a, z, dens, radl, absl, ubuf, nbuf
	 PASSCHARL(name)); 
  
  if (Mode()&kWithRoot){
    gGeoManager->Material(name,a,z,dens,imat);
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsmixt(Int_t imat, const char *name, Float_t *a, Float_t *z,  
		   Float_t dens, Int_t nlmat, Float_t *wmat) 
{ 
  //
  //       Defines mixture OR COMPOUND IMAT as composed by 
  //       THE BASIC NLMAT materials defined by arrays A,Z and WMAT
  // 
  //       If NLMAT.GT.0 then WMAT contains the PROPORTION BY
  //       WEIGTHS OF EACH BASIC MATERIAL IN THE MIXTURE. 
  // 
  //       If NLMAT.LT.0 then WMAT contains the number of atoms 
  //       of a given kind into the molecule of the COMPOUND
  //       In this case, WMAT in output is changed to relative
  //       weigths.
  //
  g3smixt(imat,PASSCHARD(name), a, z,dens, nlmat,wmat PASSCHARL(name)); 
  
  if (Mode()&kWithRoot){ 
    Int_t i;
    if (nlmat < 0) {
       nlmat = - nlmat;
       Double_t amol = 0;
       for (i=0;i<nlmat;i++) {
          amol += a[i]*wmat[i];
       }
       for (i=0;i<nlmat;i++) {
          wmat[i] *= a[i]/amol;
       }
    }
    gGeoManager->Mixture(name, a, z, dens, nlmat, wmat, imat);
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gspart(Int_t ipart, const char *name, Int_t itrtyp,  
		   Double_t amass, Double_t charge, Double_t tlife) 
{ 
  //
  // Store particle parameters
  //
  // ipart           particle code
  // name            particle name
  // itrtyp          transport method (see GEANT manual)
  // amass           mass in GeV/c2
  // charge          charge in electron units
  // tlife           lifetime in seconds
  //
  Float_t *ubuf=0; 
  Int_t   nbuf=0; 
  Float_t fmass = amass;
  Float_t fcharge = charge;
  Float_t flife = tlife;
  
  g3spart(ipart,PASSCHARD(name), itrtyp, fmass, fcharge, flife, ubuf, nbuf
	 PASSCHARL(name)); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gstmed(Int_t numed, const char *name, Int_t nmat, Int_t isvol,  
		      Int_t ifield, Float_t fieldm, Float_t tmaxfd,
		      Float_t stemax, Float_t deemax, Float_t epsil,
		      Float_t stmin) 
{ 
  //
  //  NTMED  Tracking medium number
  //  NAME   Tracking medium name
  //  NMAT   Material number
  //  ISVOL  Sensitive volume flag
  //  IFIELD Magnetic field
  //  FIELDM Max. field value (Kilogauss)
  //  TMAXFD Max. angle due to field (deg/step)
  //  STEMAX Max. step allowed
  //  DEEMAX Max. fraction of energy lost in a step
  //  EPSIL  Tracking precision (cm)
  //  STMIN  Min. step due to continuous processes (cm)
  //
  //  IFIELD = 0 if no magnetic field; IFIELD = -1 if user decision in GUSWIM;
  //  IFIELD = 1 if tracking performed with G3RKUTA; IFIELD = 2 if tracking
  //  performed with G3HELIX; IFIELD = 3 if tracking performed with G3HELX3.
  //  
  Float_t *ubuf=0; 
  Int_t   nbuf=0; 
  g3stmed(numed,PASSCHARD(name), nmat, isvol, ifield, fieldm, tmaxfd, stemax,
	 deemax, epsil, stmin, ubuf, nbuf PASSCHARL(name)); 
  
  //printf("Creating medium: %s, numed=%d, nmat=%d\n",name,numed,nmat);
  if (Mode()&kWithRoot){
    gGeoManager->Medium(name,numed,nmat, isvol, ifield, fieldm, tmaxfd, stemax,deemax, epsil, stmin);
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsckov(Int_t itmed, Int_t npckov, Float_t *ppckov,
			   Float_t *absco, Float_t *effic, Float_t *rindex)
{ 
  //
  //    Stores the tables for UV photon tracking in medium ITMED 
  //    Please note that it is the user's responsability to 
  //    provide all the coefficients:
  //
  //
  //       ITMED       Tracking medium number
  //       NPCKOV      Number of bins of each table
  //       PPCKOV      Value of photon momentum (in GeV)
  //       ABSCO       Absorbtion coefficients 
  //                   dielectric: absorbtion length in cm
  //                   metals    : absorbtion fraction (0<=x<=1)
  //       EFFIC       Detection efficiency for UV photons 
  //       RINDEX      Refraction index (if=0 metal)
  //
  g3sckov(itmed,npckov,ppckov,absco,effic,rindex);
}

//_____________________________________________________________________________
void  StTGeant3::SetCerenkov(Int_t itmed, Int_t npckov, Float_t *ppckov,
			   Float_t *absco, Float_t *effic, Float_t *rindex)
{ 
  //
  //    Stores the tables for UV photon tracking in medium ITMED 
  //    Please note that it is the user's responsability to 
  //    provide all the coefficients:
  //
  //
  //       ITMED       Tracking medium number
  //       NPCKOV      Number of bins of each table
  //       PPCKOV      Value of photon momentum (in GeV)
  //       ABSCO       Absorbtion coefficients 
  //                   dielectric: absorbtion length in cm
  //                   metals    : absorbtion fraction (0<=x<=1)
  //       EFFIC       Detection efficiency for UV photons 
  //       RINDEX      Refraction index (if=0 metal)
  //
  g3sckov(itmed,npckov,ppckov,absco,effic,rindex);
}

//_____________________________________________________________________________
void  StTGeant3::SetCerenkov(Int_t itmed, Int_t npckov, Double_t *ppckov,
			   Double_t *absco, Double_t *effic, Double_t *rindex)
{ 
  //
  //    Stores the tables for UV photon tracking in medium ITMED 
  //    Please note that it is the user's responsability to 
  //    provide all the coefficients:
  //
  //
  //       ITMED       Tracking medium number
  //       NPCKOV      Number of bins of each table
  //       PPCKOV      Value of photon momentum (in GeV)
  //       ABSCO       Absorbtion coefficients 
  //                   dielectric: absorbtion length in cm
  //                   metals    : absorbtion fraction (0<=x<=1)
  //       EFFIC       Detection efficiency for UV photons 
  //       RINDEX      Refraction index (if=0 metal)
  //

  Float_t* fppckov = CreateFloatArray(ppckov, npckov);
  Float_t* fabsco  = CreateFloatArray(absco, npckov);
  Float_t* feffic  = CreateFloatArray(effic, npckov);
  Float_t* frindex = CreateFloatArray(rindex, npckov);

  SetCerenkov(itmed, npckov, fppckov, fabsco, feffic, frindex);
  
  delete [] fppckov;
  delete [] fabsco;
  delete [] feffic;
  delete [] frindex;
}

//_____________________________________________________________________________
void  StTGeant3::Gstpar(Int_t itmed, const char *param, Double_t parval) 
{ 
  //
  //  To change the value of cut  or mechanism "CHPAR"
  //      to a new value PARVAL  for tracking medium ITMED
  //    The  data   structure  JTMED   contains  the   standard  tracking
  //  parameters (CUTS and flags to control the physics processes)  which
  //  are used  by default  for all  tracking media.   It is  possible to
  //  redefine individually  with GSTPAR  any of  these parameters  for a
  //  given tracking medium. 
  //  ITMED     tracking medium number 
  //  CHPAR     is a character string (variable name) 
  //  PARVAL    must be given as a floating point.
  //
  
  Float_t fparval = parval; 
  g3stpar(itmed,PASSCHARD(param), fparval PASSCHARL(param)); 
} 
 
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//                        Functions from GCONS
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 
//_____________________________________________________________________________
void  StTGeant3::Gfkine(Int_t itra, Float_t *vert, Float_t *pvert, Int_t &ipart,
		      Int_t &nvert) 
{ 
  //           Storing/Retrieving Vertex and Track parameters
  //           ---------------------------------------------- 
  //
  //  Stores vertex parameters. 
  //  VERT      array of (x,y,z) position of the vertex 
  //  NTBEAM    beam track number origin of the vertex 
  //            =0 if none exists  
  //  NTTARG    target track number origin of the vertex
  //  UBUF      user array of NUBUF floating point numbers
  //  NUBUF       
  //  NVTX      new vertex number (=0 in case of error). 
  //  Prints vertex parameters.
  //  IVTX      for vertex IVTX.
  //            (For all vertices if IVTX=0) 
  //  Stores long life track parameters.
  //  PLAB      components of momentum 
  //  IPART     type of particle (see GSPART)
  //  NV        vertex number origin of track
  //  UBUF      array of NUBUF floating point user parameters 
  //  NUBUF
  //  NT        track number (if=0 error).
  //  Retrieves long life track parameters.
  //  ITRA      track number for which parameters are requested
  //  VERT      vector origin of the track  
  //  PVERT     4 momentum components at the track origin 
  //  IPART     particle type (=0 if track ITRA does not exist)
  //  NVERT     vertex number origin of the track 
  //  UBUF      user words stored in GSKINE. 
  //  Prints initial track parameters. 
  //  ITRA      for track ITRA 
  //            (For all tracks if ITRA=0) 
  //
  Float_t *ubuf=0; 
  Int_t   nbuf; 
  g3fkine(itra,vert,pvert,ipart,nvert,ubuf,nbuf); 
} 

//_____________________________________________________________________________
void  StTGeant3::Gfvert(Int_t nvtx, Float_t *v, Int_t &ntbeam, Int_t &nttarg,
		      Float_t &tofg) 
{ 
  //
  //       Retrieves the parameter of a vertex bank
  //       Vertex is generated from tracks NTBEAM NTTARG
  //       NVTX is the new vertex number 
  //
  Float_t *ubuf=0; 
  Int_t   nbuf; 
  g3fvert(nvtx,v,ntbeam,nttarg,tofg,ubuf,nbuf); 
} 
 
//_____________________________________________________________________________
Int_t StTGeant3::Gskine(Float_t *plab, Int_t ipart, Int_t nv, Float_t *buf,
		      Int_t nwbuf) 
{ 
  //
  //       Store kinematics of track NT into data structure
  //       Track is coming from vertex NV
  //
  Int_t nt = 0; 
  g3skine(plab, ipart, nv, buf, nwbuf, nt); 
  return nt; 
} 
 
//_____________________________________________________________________________
Int_t StTGeant3::Gsvert(Float_t *v, Int_t ntbeam, Int_t nttarg, Float_t *ubuf,
		      Int_t nwbuf) 
{ 
  //
  //       Creates a new vertex bank 
  //       Vertex is generated from tracks NTBEAM NTTARG 
  //       NVTX is the new vertex number
  //
  Int_t nwtx = 0; 
  g3svert(v, ntbeam, nttarg, ubuf, nwbuf, nwtx); 
  return nwtx; 
} 
 
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//                        Functions from GPHYS
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

//_____________________________________________________________________________
void  StTGeant3::Gphysi() 
{ 
  //
  //       Initialise material constants for all the physics
  //       mechanisms used by GEANT
  //
  g3physi(); 
} 
 
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//                        Functions from GTRAK
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 
//_____________________________________________________________________________
void  StTGeant3::Gdebug() 
{ 
  //
  // Debug the current step
  //
  g3debug(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gekbin() 
{ 
  //
  //       To find bin number in kinetic energy table
  //       stored in ELOW(NEKBIN)
  //
  g3ekbin(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gfinds() 
{ 
  //
  //       Returns the set/volume parameters corresponding to 
  //       the current space point in /GCTRAK/
  //       and fill common /GCSETS/
  // 
  //       IHSET  user set identifier 
  //       IHDET  user detector identifier 
  //       ISET set number in JSET  
  //       IDET   detector number in JS=LQ(JSET-ISET) 
  //       IDTYPE detector type (1,2)  
  //       NUMBV  detector volume numbers (array of length NVNAME)
  //       NVNAME number of volume levels
  //
  g3finds(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsking(Int_t igk) 
{ 
  //
  //   Stores in stack JSTAK either the IGKth track of /GCKING/,
  //    or the NGKINE tracks when IGK is 0.
  //
  g3sking(igk); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gskpho(Int_t igk) 
{ 
  //
  //  Stores in stack JSTAK either the IGKth Cherenkov photon of  
  //  /GCKIN2/, or the NPHOT tracks when IGK is 0.                
  //
  g3skpho(igk); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsstak(Int_t iflag) 
{ 
  //
  //   Stores in auxiliary stack JSTAK the particle currently 
  //    described in common /GCKINE/. 
  // 
  //   On request, creates also an entry in structure JKINE :
  //    IFLAG =
  //     0 : No entry in JKINE structure required (user) 
  //     1 : New entry in JVERTX / JKINE structures required (user)
  //    <0 : New entry in JKINE structure at vertex -IFLAG (user)
  //     2 : Entry in JKINE structure exists already (from GTREVE)
  //
  g3sstak(iflag); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsxyz() 
{ 
  //
  //   Store space point VECT in banks JXYZ 
  //
  g3sxyz(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gtrack() 
{ 
  //
  //   Controls tracking of current particle 
  //
  g3track(); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gtreve() 
{ 
  //
  //   Controls tracking of all particles belonging to the current event
  //
  g3treve(); 
} 

//_____________________________________________________________________________
void  StTGeant3::GtreveRoot() 
{ 
  //
  //   Controls tracking of all particles belonging to the current event
  //
  gtreveroot(); 
} 

//_____________________________________________________________________________
void  StTGeant3::Grndm(Float_t *rvec, const Int_t len) const 
{
  //
  //  To set/retrieve the seed of the random number generator
  //
  TRandom* r=gMC->GetRandom();
  for(Int_t i=0; i<len; rvec[i++]=r->Rndm());
}

//_____________________________________________________________________________
void  StTGeant3::Grndmq(Int_t &/*is1*/, Int_t &/*is2*/, const Int_t /*iseq*/,
		      const Text_t */*chopt*/)
{
  //
  //  To set/retrieve the seed of the random number generator
  //
  /*printf("Dummy grndmq called\n");*/
}

//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//                        Functions from GDRAW
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

//_____________________________________________________________________________
void  StTGeant3::Gdxyz(Int_t it)
{
  //
  // Draw the points stored with Gsxyz relative to track it
  //
  g3dxyz(it);
}

//_____________________________________________________________________________
void  StTGeant3::Gdcxyz()
{
  //
  // Draw the position of the current track
  //
  g3dcxyz();
}

//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//                        Functions from GGEOM
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

//_____________________________________________________________________________
void  StTGeant3::Gdtom(Float_t *xd, Float_t *xm, Int_t iflag) 
{ 
  //
  //  Computes coordinates XM (Master Reference System
  //  knowing the coordinates XD (Detector Ref System)
  //  The local reference system can be initialized by
  //    - the tracking routines and GDTOM used in GUSTEP
  //    - a call to GSCMED(NLEVEL,NAMES,NUMBER)
  //        (inverse routine is GMTOD)
  // 
  //   If IFLAG=1  convert coordinates
  //      IFLAG=2  convert direction cosinus
  //
  if (Mode()&kWithG3){
    g3dtom(xd, xm, iflag);
  } else {
    Double_t XM[3], XD[3];
    Int_t i;
    for (i=0;i<3;i++) XD[i] = xd[i];
    if (iflag == 1) gGeoManager->LocalToMaster(XD,XM);
    else            gGeoManager->LocalToMasterVect(XD,XM);
    for (i=0;i<3;i++) xm[i]=XM[i];
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gdtom(Double_t *xd, Double_t *xm, Int_t iflag) 
{ 
  //
  //  Computes coordinates XM (Master Reference System
  //  knowing the coordinates XD (Detector Ref System)
  //  The local reference system can be initialized by
  //    - the tracking routines and GDTOM used in GUSTEP
  //    - a call to GSCMED(NLEVEL,NAMES,NUMBER)
  //        (inverse routine is GMTOD)
  // 
  //   If IFLAG=1  convert coordinates
  //      IFLAG=2  convert direction cosinus
  //
  
  if (Mode()&kWithG3){
    Float_t fxd[3],fxm[3];
    for (int i=0; i<3; i++) {fxd[i] = xd [i];}
    Gdtom(fxd, fxm, iflag) ;

    for (int i=0; i<3; i++) {xm [i] = fxm[i];}

  } else {
     if (iflag == 1) gGeoManager->LocalToMaster(xd,xm);
     else            gGeoManager->LocalToMasterVect(xd,xm);
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Glmoth(const char* iudet, Int_t iunum, Int_t &nlev, Int_t *lvols,
		      Int_t *lindx) 
{ 
  //
  //   Loads the top part of the Volume tree in LVOLS (IVO's),
  //   LINDX (IN indices) for a given volume defined through
  //   its name IUDET and number IUNUM.
  // 
  //   The routine stores only upto the last level where JVOLUM
  //   data structure is developed. If there is no development
  //   above the current level, it returns NLEV zero.
  Int_t *idum=0; 
  g3lmoth(PASSCHARD(iudet), iunum, nlev, lvols, lindx, idum PASSCHARL(iudet)); 
} 

//_____________________________________________________________________________
void  StTGeant3::Gmedia(Float_t *x, Int_t &numed) 
{ 
  //
  //   Finds in which volume/medium the point X is, and updates the
  //    common /GCVOLU/ and the structure JGPAR accordingly. 
  // 
  //   NUMED returns the tracking medium number, or 0 if point is
  //         outside the experimental setup.
  //

  if (Mode()&kWithG3){
    static Int_t check = 0;
    g3media(x,numed,check); 
  } else {

    gGeoManager->InitTrack(x[0],x[1],x[2],0,0,0);
    gGeoManager->FindNode();
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gmtod(Float_t *xm, Float_t *xd, Int_t iflag) 
{ 
  //
  //       Computes coordinates XD (in DRS) 
  //       from known coordinates XM in MRS 
  //       The local reference system can be initialized by
  //         - the tracking routines and GMTOD used in GUSTEP
  //         - a call to GMEDIA(XM,NUMED,CHECK)
  //         - a call to GLVOLU(NLEVEL,NAMES,NUMBER,IER) 
  //             (inverse routine is GDTOM) 
  //
  //        If IFLAG=1  convert coordinates 
  //           IFLAG=2  convert direction cosinus
  //
  if (Mode()&kWithG3){
    g3mtod(xm, xd, iflag); 
  } else {
     Double_t XM[3], XD[3];
     Int_t i;
     for (i=0;i<3;i++) XM[i]=xm[i];
     if (iflag == 1) gGeoManager->MasterToLocal(XM,XD);
     else            gGeoManager->MasterToLocalVect(XM,XD);
     for (i=0;i<3;i++) xd[i] = XD[i];
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gmtod(Double_t *xm, Double_t *xd, Int_t iflag) 
{ 
  //
  //       Computes coordinates XD (in DRS) 
  //       from known coordinates XM in MRS 
  //       The local reference system can be initialized by
  //         - the tracking routines and GMTOD used in GUSTEP
  //         - a call to GMEDIA(XM,NUMED,CHECK)
  //         - a call to GLVOLU(NLEVEL,NAMES,NUMBER,IER) 
  //             (inverse routine is GDTOM) 
  //
  //        If IFLAG=1  convert coordinates 
  //           IFLAG=2  convert direction cosinus
  //


  if (Mode()&kWithG3){
    Float_t fxm[3],fxd[3];
    for (int i=0; i<3; i++) {fxm[i]=(Float_t)xm[i];}

    Gmtod(fxm, fxd, iflag) ;

    for (int i=0; i<3; i++) {xd[i] = fxd[i];}

  } else {   
    if (iflag == 1)  gGeoManager->MasterToLocal    (xm,xd);
    else             gGeoManager->MasterToLocalVect(xm,xd);
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsdvn(const char *name, const char *mother, Int_t ndiv,
		     Int_t iaxis) 
{ 
  //
  // Create a new volume by dividing an existing one
  // 
  //  NAME   Volume name
  //  MOTHER Mother volume name
  //  NDIV   Number of divisions
  //  IAXIS  Axis value
  //
  //  X,Y,Z of CAXIS will be translated to 1,2,3 for IAXIS.
  //  It divides a previously defined volume.
  //  
  char vname[5];
  Vname(name,vname);
  char vmother[5];
  Vname(mother,vmother);
 
  if (Mode()&kWithG3){
    g3sdvn(PASSCHARD(vname), PASSCHARD(vmother), ndiv, iaxis PASSCHARL(vname)
	  PASSCHARL(vmother)); 
  } else {

    fMCGeo->Gsdvn(name, mother, ndiv, iaxis);
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsdvn2(const char *name, const char *mother, Int_t ndiv,
		      Int_t iaxis, Double_t c0i, Int_t numed) 
{ 
  //
  // Create a new volume by dividing an existing one
  // 
  // Divides mother into ndiv divisions called name
  // along axis iaxis starting at coordinate value c0.
  // the new volume created will be medium number numed.
  //
  char vname[5];
  Vname(name,vname);
  char vmother[5];
  Vname(mother,vmother);
  
  if (Mode()&kWithG3){
    Float_t fc0i = c0i;
    g3sdvn2(PASSCHARD(vname), PASSCHARD(vmother), ndiv, iaxis, fc0i, numed
	   PASSCHARL(vname) PASSCHARL(vmother)); 
  } else {
    fMCGeo->Gsdvn2(name, mother, ndiv, iaxis, c0i, numed);
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsdvs(const char *name, const char *mother, Float_t step,
		     Int_t iaxis, Int_t numed) 
{ 
  //
  // Create a new volume by dividing an existing one
  // 
  char vname[5];
  Vname(name,vname);
  char vmother[5];
  Vname(mother,vmother);

  if (Mode()&kWithG3){
    g3sdvs(PASSCHARD(vname), PASSCHARD(vmother), step, iaxis, numed
	  PASSCHARL(vname) PASSCHARL(vmother)); 
  } else {
    gGeoManager->Division(vname,vmother,iaxis,0,0,step,numed,"s");
  }  
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsdvs2(const char *name, const char *mother, Float_t step,
		      Int_t iaxis, Float_t c0, Int_t numed) 
{ 
  //
  // Create a new volume by dividing an existing one
  // 
  char vname[5];
  Vname(name,vname);
  char vmother[5];
  Vname(mother,vmother);

  if (Mode()&kWithG3){
    g3sdvs2(PASSCHARD(vname), PASSCHARD(vmother), step, iaxis, c0, numed
	   PASSCHARL(vname) PASSCHARL(vmother)); 
  } else {
    gGeoManager->Division(vname,vmother,iaxis,0,c0,step,numed,"sx");
  } 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsdvt(const char *name, const char *mother, Double_t step,
		     Int_t iaxis, Int_t numed, Int_t ndvmx) 
{ 
  //
  // Create a new volume by dividing an existing one
  // 
  //       Divides MOTHER into divisions called NAME along
  //       axis IAXIS in steps of STEP. If not exactly divisible 
  //       will make as many as possible and will centre them 
  //       with respect to the mother. Divisions will have medium 
  //       number NUMED. If NUMED is 0, NUMED of MOTHER is taken.
  //       NDVMX is the expected maximum number of divisions
  //          (If 0, no protection tests are performed) 
  //
  char vname[5];
  Vname(name,vname);
  char vmother[5];
  Vname(mother,vmother);
  
  if (Mode()&kWithG3){
    Float_t fstep = step;
    g3sdvt(PASSCHARD(vname), PASSCHARD(vmother), fstep, iaxis, numed, ndvmx
	  PASSCHARL(vname) PASSCHARL(vmother)); 
  } else {
    fMCGeo->Gsdvt(name, mother, step, iaxis, numed, ndvmx);
  }  
} 

//_____________________________________________________________________________
void  StTGeant3::Gsdvt2(const char *name, const char *mother, Double_t step,
		      Int_t iaxis, Double_t c0, Int_t numed, Int_t ndvmx) 
{ 
  //
  // Create a new volume by dividing an existing one
  //                                                                    
  //           Divides MOTHER into divisions called NAME along          
  //            axis IAXIS starting at coordinate value C0 with step    
  //            size STEP.                                              
  //           The new volume created will have medium number NUMED.    
  //           If NUMED is 0, NUMED of mother is taken.                 
  //           NDVMX is the expected maximum number of divisions        
  //             (If 0, no protection tests are performed)              
  //
  char vname[5];
  Vname(name,vname);
  char vmother[5];
  Vname(mother,vmother);
  
  if (Mode()&kWithG3){
    Float_t fstep = step;
    Float_t fc0 = c0;
    g3sdvt2(PASSCHARD(vname), PASSCHARD(vmother), fstep, iaxis, fc0,
	   numed, ndvmx PASSCHARL(vname) PASSCHARL(vmother)); 
  } else {
    fMCGeo->Gsdvt2(name, mother, step, iaxis, c0, numed, ndvmx);
  }  
} 

//_____________________________________________________________________________
void  StTGeant3::Gsord(const char *name, Int_t iax) 
{ 
  //
  //    Flags volume CHNAME whose contents will have to be ordered 
  //    along axis IAX, by setting the search flag to -IAX
  //           IAX = 1    X axis 
  //           IAX = 2    Y axis 
  //           IAX = 3    Z axis 
  //           IAX = 4    Rxy (static ordering only  -> GTMEDI)
  //           IAX = 14   Rxy (also dynamic ordering -> GTNEXT)
  //           IAX = 5    Rxyz (static ordering only -> GTMEDI)
  //           IAX = 15   Rxyz (also dynamic ordering -> GTNEXT)
  //           IAX = 6    PHI   (PHI=0 => X axis)
  //           IAX = 7    THETA (THETA=0 => Z axis)
  //

  if (Mode()&kWithG3){
    char vname[5];
    Vname(name,vname);
    g3sord(PASSCHARD(vname), iax PASSCHARL(vname)); 
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gspos(const char *name, Int_t nr, const char *mother, Double_t x,
		     Double_t y, Double_t z, Int_t irot, const char *konly) 
{ 
  //
  // Position a volume into an existing one
  //
  //  NAME   Volume name
  //  NUMBER Copy number of the volume
  //  MOTHER Mother volume name
  //  X      X coord. of the volume in mother ref. sys.
  //  Y      Y coord. of the volume in mother ref. sys.
  //  Z      Z coord. of the volume in mother ref. sys.
  //  IROT   Rotation matrix number w.r.t. mother ref. sys.
  //  ONLY   ONLY/MANY flag
  //
  //  It positions a previously defined volume in the mother.
  //  
    
  TString only = konly;
  only.ToLower();
  Bool_t isOnly = kFALSE;
  if (only.Contains("only")) isOnly = kTRUE;
  char vname[5];
  Vname(name,vname);
  char vmother[5];
  Vname(mother,vmother);
  
  if (Mode()&kWithG3){
    Float_t fx = x;
    Float_t fy = y;
    Float_t fz = z;  
    g3spos(PASSCHARD(vname), nr, PASSCHARD(vmother), fx, fy, fz, irot,
	  PASSCHARD(konly) PASSCHARL(vname) PASSCHARL(vmother)
	  PASSCHARL(konly)); 
  } else {

    fMCGeo->Gspos(name, nr, mother, x, y, z, irot, konly);
  } 
} 
 
//_____________________________________________________________________________
void  StTGeant3::G3Gsposp(const char *name, Int_t nr, const char *mother,  
		      Double_t x, Double_t y, Double_t z, Int_t irot,
		      const char *konly, Float_t *upar, Int_t np ) 
{ 
  //
  //      Place a copy of generic volume NAME with user number
  //      NR inside MOTHER, with its parameters UPAR(1..NP)
  //
  TString only = konly;
  only.ToLower();
  Bool_t isOnly = kFALSE;
  if (only.Contains("only")) isOnly = kTRUE;
  char vname[5];
  Vname(name,vname);
  char vmother[5];
  Vname(mother,vmother);

  Float_t fx = x;
  Float_t fy = y;
  Float_t fz = z;  
  g3sposp(PASSCHARD(vname), nr, PASSCHARD(vmother), fx, fy, fz, irot,
	 PASSCHARD(konly), upar, np PASSCHARL(vname) PASSCHARL(vmother)
	 PASSCHARL(konly)); 
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsposp(const char *name, Int_t nr, const char *mother,  
		      Double_t x, Double_t y, Double_t z, Int_t irot,
		      const char *konly, Float_t *upar, Int_t np ) 
{ 
  //
  //      Place a copy of generic volume NAME with user number
  //      NR inside MOTHER, with its parameters UPAR(1..NP)
  //

  if (Mode()&kWithG3){
    G3Gsposp(name, nr, mother, x, y, z, irot, konly, upar, np); 
  } else {

    fMCGeo->Gsposp(name, nr, mother, x, y, z, irot, konly, upar, np);
  }  
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsposp(const char *name, Int_t nr, const char *mother,  
		      Double_t x, Double_t y, Double_t z, Int_t irot,
		      const char *konly, Double_t *upar, Int_t np ) 
{ 
  //
  //      Place a copy of generic volume NAME with user number
  //      NR inside MOTHER, with its parameters UPAR(1..NP)
  //

  if (Mode()&kWithG3){
    Float_t* fupar = CreateFloatArray(upar, np);
    G3Gsposp(name, nr, mother, x, y, z, irot, konly, fupar, np); 
    delete [] fupar;
  } else {
    fMCGeo->Gsposp(name, nr, mother, x, y, z, irot, konly, upar, np);
  }
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gsrotm(Int_t nmat, Float_t theta1, Float_t phi1, Float_t theta2,
		      Float_t phi2, Float_t theta3, Float_t phi3) 
{ 
  //
  //  nmat   Rotation matrix number
  //  THETA1 Polar angle for axis I
  //  PHI1   Azimuthal angle for axis I
  //  THETA2 Polar angle for axis II
  //  PHI2   Azimuthal angle for axis II
  //  THETA3 Polar angle for axis III
  //  PHI3   Azimuthal angle for axis III
  //
  //  It defines the rotation matrix number IROT.
  //  

  if (Mode()&kWithG3){
    g3srotm(nmat, theta1, phi1, theta2, phi2, theta3, phi3); 
  } else {
    gGeoManager->Matrix(nmat, theta1, phi1, theta2, phi2, theta3, phi3);
  }  
} 
 
//_____________________________________________________________________________
void  StTGeant3::Gprotm(Int_t nmat) 
{ 
  //
  //    To print rotation matrices structure JROTM
  //     nmat     Rotation matrix number
  //
  if (Mode()&kWithG3){
    g3protm(nmat); 
  } else {
    TIter next(gGeoManager->GetListOfMatrices());
    TGeoMatrix *matrix;
    while ((matrix = (TGeoMatrix*)next())) {
       if (UInt_t(nmat) == matrix->GetUniqueID()) {
          matrix->Print();
	  return;
       }
    }     	
    Error("Gprotm","Rotation with id=%i not found", nmat);
  }
 } 
 
//_____________________________________________________________________________
Int_t StTGeant3::G3Gsvolu(const char *name, const char *shape, Int_t nmed,  
		      Float_t *upar, Int_t npar) 
{ 
  //
  //  NAME   Volume name
  //  SHAPE  Volume type
  //  NUMED  Tracking medium number
  //  NPAR   Number of shape parameters
  //  UPAR   Vector containing shape parameters
  //
  //  It creates a new volume in the JVOLUM data structure.
  //  
  Int_t ivolu = 0; 
  char vname[5];
  Vname(name,vname);
  char vshape[5];
  Vname(shape,vshape);

  g3svolu(PASSCHARD(vname), PASSCHARD(vshape), nmed, upar, npar, ivolu
	 PASSCHARL(vname) PASSCHARL(vshape)); 

  return ivolu; 
} 
 
//_____________________________________________________________________________
Int_t StTGeant3::Gsvolu(const char *name, const char *shape, Int_t nmed,  
		      Float_t *upar, Int_t npar) 
{ 
  //
  //  NAME   Volume name
  //  SHAPE  Volume type
  //  NUMED  Tracking medium number
  //  NPAR   Number of shape parameters
  //  UPAR   Vector containing shape parameters
  //
  //  It creates a new volume in the JVOLUM data structure.
  //  

  Int_t ivolu = 0; 
  if (Mode()&kWithG3){
    ivolu = G3Gsvolu(name, shape, nmed, upar, npar);
  } else {
    ivolu = fMCGeo->Gsvolu(name, shape, nmed, upar, npar);
  }  
  return ivolu; 

} 
 
//_____________________________________________________________________________
Int_t StTGeant3::Gsvolu(const char *name, const char *shape, Int_t nmed,  
		      Double_t *upar, Int_t npar) 
{ 
  //
  //  NAME   Volume name
  //  SHAPE  Volume type
  //  NUMED  Tracking medium number
  //  NPAR   Number of shape parameters
  //  UPAR   Vector containing shape parameters
  //
  //  It creates a new volume in the JVOLUM data structure.
  //  


  Int_t ivolu = 0; 
  if (Mode()&kWithG3){
    Float_t* fupar = CreateFloatArray(upar, npar);
    ivolu = G3Gsvolu(name, shape, nmed, fupar, npar);
    delete [] fupar;  
  } else {
    ivolu = fMCGeo->Gsvolu(name, shape, nmed, upar, npar);
  }  
  return ivolu; 
} 
 
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
//
//           T H E    D R A W I N G   P A C K A G E
//           ======================================
//  Drawing functions. These functions allow the visualization in several ways
//  of the volumes defined in the geometrical data structure. It is possible
//  to draw the logical tree of volumes belonging to the detector (DTREE),
//  to show their geometrical specification (DSPEC,DFSPC), to draw them
//  and their cut views (DRAW, DCUT). Moreover, it is possible to execute
//  these commands when the hidden line removal option is activated; in
//  this case, the volumes can be also either translated in the space
//  (SHIFT), or clipped by boolean operation (CVOL). In addition, it is
//  possible to fill the surfaces of the volumes
//  with solid colours when the shading option (SHAD) is activated.
//  Several tools (ZOOM, LENS) have been developed to zoom detailed parts
//  of the detectors or to scan physical events as well.
//  Finally, the command MOVE will allow the rotation, translation and zooming
//  on real time parts of the detectors or tracks and hits of a simulated event.
//  Ray-tracing commands. In case the command (DOPT RAYT ON) is executed,
//  the drawing is performed by the Geant ray-tracing;
//  automatically, the color is assigned according to the tracking medium of each
//  volume and the volumes with a density lower/equal than the air are considered
//  transparent; if the option (USER) is set (ON) (again via the command (DOPT)),
//  the user can set color and visibility for the desired volumes via the command
//  (SATT), as usual, relatively to the attributes (COLO) and (SEEN).
//  The resolution can be set via the command (SATT * FILL VALUE), where (VALUE)
//  is the ratio between the number of pixels drawn and 20 (user coordinates).
//  Parallel view and perspective view are possible (DOPT PROJ PARA/PERS); in the
//  first case, we assume that the first mother volume of the tree is a box with
//  dimensions 10000 X 10000 X 10000 cm and the view point (infinetely far) is
//  5000 cm far from the origin along the Z axis of the user coordinates; in the
//  second case, the distance between the observer and the origin of the world
//  reference system is set in cm by the command (PERSP NAME VALUE); grand-angle
//  or telescopic effects can be achieved changing the scale factors in the command
//  (DRAW). When the final picture does not occupy the full window,
//  mapping the space before tracing can speed up the drawing, but can also
//  produce less precise results; values from 1 to 4 are allowed in the command
//  (DOPT MAPP VALUE), the mapping being more precise for increasing (VALUE); for
//  (VALUE = 0) no mapping is performed (therefore max precision and lowest speed).
//  The command (VALCUT) allows the cutting of the detector by three planes
//  ortogonal to the x,y,z axis. The attribute (LSTY) can be set by the command
//  SATT for any desired volume and can assume values from 0 to 7; it determines
//  the different light processing to be performed for different materials:
//  0 = dark-matt, 1 = bright-matt, 2 = plastic, 3 = ceramic, 4 = rough-metals,
//  5 = shiny-metals, 6 = glass, 7 = mirror. The detector is assumed to be in the
//  dark, the ambient light luminosity is 0.2 for each basic hue (the saturation
//  is 0.9) and the observer is assumed to have a light source (therefore he will
//  produce parallel light in the case of parallel view and point-like-source
//  light in the case of perspective view).
//
//*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

//_____________________________________________________________________________
void StTGeant3::Gsatt(const char *name, const char *att, Int_t val)
{ 
  //
  //  NAME   Volume name
  //  IOPT   Name of the attribute to be set
  //  IVAL   Value to which the attribute is to be set
  //
  //  name= "*" stands for all the volumes.
  //  iopt can be chosen among the following :
  //  
  //     WORK   0=volume name is inactive for the tracking
  //            1=volume name is active for the tracking (default)
  //  
  //     SEEN   0=volume name is invisible
  //            1=volume name is visible (default)
  //           -1=volume invisible with all its descendants in the tree
  //           -2=volume visible but not its descendants in the tree
  //  
  //     LSTY   line style 1,2,3,... (default=1)
  //            LSTY=7 will produce a very precise approximation for
  //            revolution bodies.
  //  
  //     LWID   line width -7,...,1,2,3,..7 (default=1)
  //            LWID<0 will act as abs(LWID) was set for the volume
  //            and for all the levels below it. When SHAD is 'ON', LWID
  //            represent the linewidth of the scan lines filling the surfaces
  //            (whereas the FILL value represent their number). Therefore
  //            tuning this parameter will help to obtain the desired
  //            quality/performance ratio.
  //  
  //     COLO   colour code -166,...,1,2,..166 (default=1)
  //            n=1=black
  //            n=2=red;    n=17+m, m=0,25, increasing luminosity according to 'm';
  //            n=3=green;  n=67+m, m=0,25, increasing luminosity according to 'm';
  //            n=4=blue;   n=117+m, m=0,25, increasing luminosity according to 'm';
  //            n=5=yellow; n=42+m, m=0,25, increasing luminosity according to 'm';
  //            n=6=violet; n=142+m, m=0,25, increasing luminosity according to 'm';
  //            n=7=lightblue; n=92+m, m=0,25, increasing luminosity according to 'm';
  //            colour=n*10+m, m=1,2,...9, will produce the same colour
  //            as 'n', but with increasing luminosity according to 'm';
  //            COLO<0 will act as if abs(COLO) was set for the volume
  //            and for all the levels below it.
  //            When for a volume the attribute FILL is > 1 (and the
  //            option SHAD is on), the ABS of its colour code must be < 8
  //            because an automatic shading of its faces will be
  //            performed.
  //  
  //     FILL  (1992) fill area  -7,...,0,1,...7 (default=0)
  //            when option SHAD is "on" the FILL attribute of any
  //            volume can be set different from 0 (normal drawing);
  //            if it is set to 1, the faces of such volume will be filled
  //            with solid colours; if ABS(FILL) is > 1, then a light
  //            source is placed along the observer line, and the faces of
  //            such volumes will be painted by colours whose luminosity
  //            will depend on the amount of light reflected;
  //            if ABS(FILL) = 1, then it is possible to use all the 166
  //            colours of the colour table, becouse the automatic shading
  //            is not performed;
  //            for increasing values of FILL the drawing will be performed
  //            with higher and higher resolution improving the quality (the
  //            number of scan lines used to fill the faces increases with FILL);
  //            it is possible to set different values of FILL
  //            for different volumes, in order to optimize at the same time
  //            the performance and the quality of the picture;
  //            FILL<0 will act as if abs(FILL) was set for the volume
  //            and for all the levels below it.
  //            This kind of drawing can be saved in 'picture files'
  //            or in view banks.
  //            0=drawing without fill area
  //            1=faces filled with solid colours and resolution = 6
  //            2=lowest resolution (very fast)
  //            3=default resolution
  //            4=.................
  //            5=.................
  //            6=.................
  //            7=max resolution
  //            Finally, if a coloured background is desired, the FILL
  //            attribute for the first volume of the tree must be set
  //            equal to -abs(colo), colo being >0 and <166.
  //  
  //     SET   set number associated to volume name
  //     DET   detector number associated to volume name
  //     DTYP  detector type (1,2)
  //  
//  InitHIGZ();
  char vname[5];
  Vname(name,vname);
  char vatt[5];
  Vname(att,vatt);
  if (Mode()&kWithG3){
    g3satt(PASSCHARD(vname), PASSCHARD(vatt), val PASSCHARL(vname)
	  PASSCHARL(vatt)); 
  } else {
    gGeoManager->SetVolumeAttribute(vname, vatt, val);
  }
} 

//_____________________________________________________________________________
void StTGeant3::Gfpara(const char *name, Int_t number, Int_t intext, Int_t& npar,
			 Int_t& natt, Float_t* par, Float_t* att)
{
  //
  // Find the parameters of a volume
  //
  g3fpara(PASSCHARD(name), number, intext, npar, natt, par, att
	 PASSCHARL(name));
}

//_____________________________________________________________________________
void StTGeant3::Gckpar(Int_t ish, Int_t npar, Float_t* par)
{
  //
  // Check the parameters of a shape
  //
  gckpar(ish,npar,par);
}

//_____________________________________________________________________________
void StTGeant3::Gckmat(Int_t itmed, char* natmed)
{
  //
  // Check the parameters of a tracking medium
  //
  g3ckmat(itmed, PASSCHARD(natmed) PASSCHARL(natmed));
}

//_____________________________________________________________________________
Int_t StTGeant3::Glvolu(Int_t nlev, Int_t *lnam,Int_t *lnum) 
{ 
  //
  //  nlev   number of leveles deap into the volume tree
  //         size of the arrays lnam and lnum
  //  lnam   an integer array whos 4 bytes contain the askii code for the
  //         volume names
  //  lnum   an integer array containing the copy numbers for that specific
  //         volume
  //
  //  This routine fills the volulme paramters in common /fgGcvolu/ for a
  //  physical tree, specified by the list lnam and lnum of volume names
  //  and numbers, and for all its ascendants up to level 1. This routine
  //  is optimsed and does not re-compute the part of the history already
  //  available in GCVOLU. This means that if it is used in user programs
  //  outside the usual framwork of the tracking, the user has to initilise
  //  to zero NLEVEL in the common GCVOLU. It return 0 if there were no
  //  problems in make the call.
  //
  if (Mode()&kWithG3){
    Int_t ier;
    g3lvolu(nlev, lnam, lnum, ier); 
    return ier;
  } else {
    Warning("Glvolu","not yet implemented");
    return 0;
  }
}

//_____________________________________________________________________________
void StTGeant3::Gdelete(Int_t iview)
{ 
  //
  //  IVIEW  View number
  //
  //  It deletes a view bank from memory.
  //
  g3delet(iview);
}
 
//_____________________________________________________________________________
void StTGeant3::Gdopen(Int_t iview)
{ 
  //
  //  IVIEW  View number
  //
  //  When a drawing is very complex and requires a long time to be
  //  executed, it can be useful to store it in a view bank: after a
  //  call to DOPEN and the execution of the drawing (nothing will
  //  appear on the screen), and after a necessary call to DCLOSE,
  //  the contents of the bank can be displayed in a very fast way
  //  through a call to DSHOW; therefore, the detector can be easily
  //  zoomed many times in different ways. Please note that the pictures
  //  with solid colours can now be stored in a view bank or in 'PICTURE FILES'
  //
  InitHIGZ();
  gHigz->Clear();
  g3dopen(iview);
}
 
//_____________________________________________________________________________
void StTGeant3::Gdclose()
{ 
  //
  //  It closes the currently open view bank; it must be called after the
  //  end of the drawing to be stored.
  //
  g3dclos();
}
 
//_____________________________________________________________________________
void StTGeant3::Gdshow(Int_t iview)
{ 
  //
  //  IVIEW  View number
  //
  //  It shows on the screen the contents of a view bank. It
  //  can be called after a view bank has been closed.
  //
  g3dshow(iview);
} 

//_____________________________________________________________________________
void StTGeant3::Gdopt(const char *name,const char *value)
{ 
  //
  //  NAME   Option name
  //  VALUE  Option value
  //
  //  To set/modify the drawing options.
  //     IOPT   IVAL      Action
  //  
  //     THRZ    ON       Draw tracks in R vs Z
  //             OFF (D)  Draw tracks in X,Y,Z
  //             180
  //             360
  //     PROJ    PARA (D) Parallel projection
  //             PERS     Perspective
  //     TRAK    LINE (D) Trajectory drawn with lines
  //             POIN       " " with markers
  //     HIDE    ON       Hidden line removal using the CG package
  //             OFF (D)  No hidden line removal
  //     SHAD    ON       Fill area and shading of surfaces.
  //             OFF (D)  Normal hidden line removal.
  //     RAYT    ON       Ray-tracing on.
  //             OFF (D)  Ray-tracing off.
  //     EDGE    OFF      Does not draw contours when shad is on.
  //             ON  (D)  Normal shading.
  //     MAPP    1,2,3,4  Mapping before ray-tracing.
  //             0   (D)  No mapping.
  //     USER    ON       User graphics options in the raytracing.
  //             OFF (D)  Automatic graphics options.
  //  
  if (Mode()==kWithRoot) return;
  InitHIGZ();
  char vname[5];
  Vname(name,vname);
  char vvalue[5];
  Vname(value,vvalue);
  g3dopt(PASSCHARD(vname), PASSCHARD(vvalue) PASSCHARL(vname)
	PASSCHARL(vvalue)); 
} 
 
//_____________________________________________________________________________
void StTGeant3::Gdraw(const char *name,Double_t theta, Double_t phi, Double_t psi,
		    Double_t u0,Double_t v0,Double_t ul,Double_t vl)
{ 
  //
  //  NAME   Volume name
  //  +
  //  THETA  Viewing angle theta (for 3D projection)
  //  PHI    Viewing angle phi (for 3D projection)
  //  PSI    Viewing angle psi (for 2D rotation)
  //  U0     U-coord. (horizontal) of volume origin
  //  V0     V-coord. (vertical) of volume origin
  //  SU     Scale factor for U-coord.
  //  SV     Scale factor for V-coord.
  //
  //  This function will draw the volumes,
  //  selected with their graphical attributes, set by the Gsatt
  //  facility. The drawing may be performed with hidden line removal
  //  and with shading effects according to the value of the options HIDE
  //  and SHAD; if the option SHAD is ON, the contour's edges can be
  //  drawn or not. If the option HIDE is ON, the detector can be
  //  exploded (BOMB), clipped with different shapes (CVOL), and some
  //  of its parts can be shifted from their original
  //  position (SHIFT). When HIDE is ON, if
  //  the drawing requires more than the available memory, the program
  //  will evaluate and display the number of missing words
  //  (so that the user can increase the
  //  size of its ZEBRA store). Finally, at the end of each drawing (with HIDE on),
  //  the program will print messages about the memory used and
  //  statistics on the volumes' visibility.
  //  The following commands will produce the drawing of a green
  //  volume, specified by NAME, without using the hidden line removal
  //  technique, using the hidden line removal technique,
  //  with different linewidth and colour (red), with
  //  solid colour, with shading of surfaces, and without edges.
  //  Finally, some examples are given for the ray-tracing. (A possible
  //  string for the NAME of the volume can be found using the command DTREE).
  //
  if (Mode()==kWithRoot)return;
  
  InitHIGZ();
  gHigz->Clear();
  char vname[5];
  Vname(name,vname);
  Float_t ftheta = theta;
  Float_t fphi = phi;
  Float_t fpsi = psi;
  Float_t fu0 = u0;
  Float_t fv0 = v0;
  Float_t ful = ul;
  Float_t fvl = vl;  
  if (fGcvdma->raytra != 1) {
    g3draw(PASSCHARD(vname), ftheta,fphi,fpsi,fu0,fv0,ful,fvl PASSCHARL(vname)); 
  } else {
    g3drayt(PASSCHARD(vname), ftheta,fphi,fpsi,fu0,fv0,ful,fvl PASSCHARL(vname)); 
  }
} 
 
//_____________________________________________________________________________
void StTGeant3::Gdrawc(const char *name,Int_t axis, Float_t cut,Float_t u0,
		     Float_t v0,Float_t ul,Float_t vl)
{ 
  //
  //  NAME   Volume name
  //  CAXIS  Axis value
  //  CUTVAL Cut plane distance from the origin along the axis
  //  +
  //  U0     U-coord. (horizontal) of volume origin
  //  V0     V-coord. (vertical) of volume origin
  //  SU     Scale factor for U-coord.
  //  SV     Scale factor for V-coord.
  //
  //  The cut plane is normal to caxis (X,Y,Z), corresponding to iaxis (1,2,3),
  //  and placed at the distance cutval from the origin.
  //  The resulting picture is seen from the the same axis.
  //  When HIDE Mode is ON, it is possible to get the same effect with
  //  the CVOL/BOX function.
  //  
  if (Mode()==kWithRoot) return;
  

  InitHIGZ();
  gHigz->Clear();
  char vname[5];
  Vname(name,vname);
  g3drawc(PASSCHARD(vname), axis,cut,u0,v0,ul,vl PASSCHARL(vname)); 
} 
 
//_____________________________________________________________________________
void StTGeant3::Gdrawx(const char *name,Float_t cutthe, Float_t cutphi,
		     Float_t cutval, Float_t theta, Float_t phi, Float_t u0,
		     Float_t v0,Float_t ul,Float_t vl)
{ 
  //
  //  NAME   Volume name
  //  CUTTHE Theta angle of the line normal to cut plane
  //  CUTPHI Phi angle of the line normal to cut plane
  //  CUTVAL Cut plane distance from the origin along the axis
  //  +
  //  THETA  Viewing angle theta (for 3D projection)
  //  PHI    Viewing angle phi (for 3D projection)
  //  U0     U-coord. (horizontal) of volume origin
  //  V0     V-coord. (vertical) of volume origin
  //  SU     Scale factor for U-coord.
  //  SV     Scale factor for V-coord.
  //
  //  The cut plane is normal to the line given by the cut angles
  //  cutthe and cutphi and placed at the distance cutval from the origin.
  //  The resulting picture is seen from the viewing angles theta,phi.
  //
  if (Mode()==kWithRoot)return;
  

  InitHIGZ();
  gHigz->Clear();
  char vname[5];
  Vname(name,vname);
  g3drawx(PASSCHARD(vname), cutthe,cutphi,cutval,theta,phi,u0,v0,ul,vl
	  PASSCHARL(vname)); 
}
 
//_____________________________________________________________________________
void StTGeant3::Gdhead(Int_t isel, const char *name, Double_t chrsiz)
{ 
  //
  //  Parameters
  //  +
  //  ISEL   Option flag  D=111110
  //  NAME   Title
  //  CHRSIZ Character size (cm) of title NAME D=0.6
  //
  //  ISEL =
  //   0      to have only the header lines
  //   xxxxx1 to add the text name centered on top of header
  //   xxxx1x to add global detector name (first volume) on left
  //   xxx1xx to add date on right
  //   xx1xxx to select thick characters for text on top of header
  //   x1xxxx to add the text 'EVENT NR x' on top of header
  //   1xxxxx to add the text 'RUN NR x' on top of header
  //  NOTE that ISEL=x1xxx1 or ISEL=1xxxx1 are illegal choices,
  //  i.e. they generate overwritten text.
  //
  
  Float_t fchrsiz = chrsiz; 
  g3dhead(isel,PASSCHARD(name),fchrsiz PASSCHARL(name));
}

//_____________________________________________________________________________
void StTGeant3::Gdman(Double_t u, Double_t v, const char *type)
{ 
  //
  //  Draw a 2D-man at position (U0,V0)
  //  Parameters
  //  U      U-coord. (horizontal) of the centre of man' R
  //  V      V-coord. (vertical) of the centre of man' R
  //  TYPE   D='MAN' possible values: 'MAN,WM1,WM2,WM3'
  // 
  //   CALL GDMAN(u,v),CALL GDWMN1(u,v),CALL GDWMN2(u,v),CALL GDWMN2(u,v)
  //  It superimposes the picure of a man or of a woman, chosen among
  //  three different ones, with the same scale factors as the detector
  //  in the current drawing.
  //
  
  Float_t fu = u;
  Float_t fv = v;  
  TString opt = type;
  if (opt.Contains("WM1")) {
    g3dwmn1(fu,fv);
  } else if (opt.Contains("WM3")) {
    g3dwmn3(fu,fv);
  } else if (opt.Contains("WM2")) {
    g3dwmn2(fu,fv);
  } else {
    g3dman(fu,fv);
  }
}
 
//_____________________________________________________________________________
void StTGeant3::Gdspec(const char *name)
{ 
  //
  //  NAME   Volume name
  //
  //  Shows 3 views of the volume (two cut-views and a 3D view), together with
  //  its geometrical specifications. The 3D drawing will
  //  be performed according the current values of the options HIDE and
  //  SHAD and according the current SetClipBox clipping parameters for that
  //  volume.
  //  
   if (Mode()==kWithRoot) return;

  InitHIGZ();
  gHigz->Clear();
  char vname[5];
  Vname(name,vname);
  g3dspec(PASSCHARD(vname) PASSCHARL(vname)); 
} 
 
//_____________________________________________________________________________
void StTGeant3::DrawOneSpec(const char *name)
{ 
  //
  //  Function called when one double-clicks on a volume name
  //  in a TPavelabel drawn by Gdtree.
  //
  if (Mode()==kWithRoot) return;

  THIGZ *higzSave = gHigz;
  higzSave->SetName("higzSave");
  THIGZ *higzSpec = (THIGZ*)gROOT->FindObject("higzSpec");
 //  printf("DrawOneSpec, gHigz=%x, higzSpec=%x\n",gHigz,higzSpec);
  if (higzSpec) gHigz     = higzSpec;
  else          higzSpec = new THIGZ(kDefSize);
  higzSpec->SetName("higzSpec");
  higzSpec->cd();
  higzSpec->Clear();
  char vname[5];
  Vname(name,vname);
  g3dspec(PASSCHARD(vname) PASSCHARL(vname)); 
  higzSpec->Update();
  higzSave->cd();
  higzSave->SetName("higz");
  gHigz = higzSave;
} 

//_____________________________________________________________________________
void StTGeant3::Gdtree(const char *name,Int_t levmax, Int_t isel)
{ 
  //
  //  NAME   Volume name
  //  LEVMAX Depth level
  //  ISELT  Options
  //
  //  This function draws the logical tree,
  //  Each volume in the tree is represented by a TPaveTree object.
  //  Double-clicking on a TPaveTree draws the specs of the corresponding volume.
  //  Use TPaveTree pop-up menu to select:
  //    - drawing specs
  //    - drawing tree
  //    - drawing tree of parent
  //  
  if (Mode()==kWithRoot) return;

  InitHIGZ();
  gHigz->Clear();
  char vname[5];
  Vname(name,vname);
  g3dtree(PASSCHARD(vname), levmax, isel PASSCHARL(vname)); 
  gHigz->SetPname("");
} 

//_____________________________________________________________________________
void StTGeant3::GdtreeParent(const char *name,Int_t levmax, Int_t isel)
{ 
  //
  //  NAME   Volume name
  //  LEVMAX Depth level
  //  ISELT  Options
  //
  //  This function draws the logical tree of the parent of name.
  //  
  if (Mode()==kWithRoot) return;


  InitHIGZ();
  gHigz->Clear();
  // Scan list of volumes in JVOLUM
  if (Mode()&kWithG3){
    char vname[5];
    Int_t gname, i, jvo, in, nin, jin, num;
    strncpy((char *) &gname, name, 4);
    for(i=1; i<=fGcnum->nvolum; i++) {
      jvo = fZlq[fGclink->jvolum-i];
      nin = Int_t(fZq[jvo+3]);
      if (nin == -1) nin = 1;
      for (in=1;in<=nin;in++) {
	jin = fZlq[jvo-in];
	num = Int_t(fZq[jin+2]);
	if(gname == fZiq[fGclink->jvolum+num]) {
	  strncpy(vname,(char*)&fZiq[fGclink->jvolum+i],4);
	  vname[4] = 0;           
	  g3dtree(PASSCHARD(vname), levmax, isel PASSCHARL(vname)); 
	  gHigz->SetPname("");
	  return;
	}
      }
    }
  }
} 
 
//_____________________________________________________________________________
void StTGeant3::SetABAN(Int_t par)
{
  //
  // par = 1 particles will be stopped according to their residual
  //         range if they are not in a sensitive material and are
  //         far enough from the boundary
  //       0 particles are transported normally
  //
  fGcphys->dphys1 = par;
  SetBit(kABAN);
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetANNI(Int_t par)
{
  //
  //   To control positron annihilation.
  //    par =0 no annihilation
  //        =1 annihilation. Decays processed.
  //        =2 annihilation. No decay products stored.
  //
  fGcphys->ianni = par;
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetAUTO(Int_t par)
{
  //
  //  To control automatic calculation of tracking medium parameters:
  //   par =0 no automatic calculation;
  //       =1 automati calculation.
  //  
  fGctrak->igauto = par;
  SetBit(kAUTO);
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetBOMB(Float_t boom)
{
  //
  //  BOOM  : Exploding factor for volumes position 
  // 
  //  To 'explode' the detector. If BOOM is positive (values smaller
  //  than 1. are suggested, but any value is possible)
  //  all the volumes are shifted by a distance
  //  proportional to BOOM along the direction between their centre
  //  and the origin of the MARS; the volumes which are symmetric
  //  with respect to this origin are simply not shown.
  //  BOOM equal to 0 resets the normal mode.
  //  A negative (greater than -1.) value of
  //  BOOM will cause an 'implosion'; for even lower values of BOOM
  //  the volumes' positions will be reflected respect to the origin.
  //  This command can be useful to improve the 3D effect for very
  //  complex detectors. The following commands will make explode the
  //  detector:
  //
//  InitHIGZ();
//  setbomb(boom);
}
 
//_____________________________________________________________________________
void StTGeant3::SetBREM(Int_t par)
{
  //
  //  To control bremstrahlung.
  //   par =0 no bremstrahlung
  //       =1 bremstrahlung. Photon processed.
  //       =2 bremstrahlung. No photon stored.
  //  
  fGcphys->ibrem = par;
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetCKOV(Int_t par)
{
  //
  //  To control Cerenkov production
  //   par =0 no Cerenkov;
  //       =1 Cerenkov;
  //       =2 Cerenkov with primary stopped at each step.
  //  
  fGctlit->itckov = par;
}
 
 
//_____________________________________________________________________________
void  StTGeant3::SetClipBox(const char *name,Double_t xmin,Double_t xmax,
			  Double_t ymin,Double_t ymax,Double_t zmin,Double_t zmax)
{
  //
  //  The hidden line removal technique is necessary to visualize properly
  //  very complex detectors. At the same time, it can be useful to visualize
  //  the inner elements of a detector in detail. This function allows
  //  subtractions (via boolean operation) of BOX shape from any part of
  //  the detector, therefore showing its inner contents.
  //  If "*" is given as the name of the
  //  volume to be clipped, all volumes are clipped by the given box.
  //  A volume can be clipped at most twice.
  //  if a volume is explicitely clipped twice,
  //  the "*" will not act on it anymore. Giving "." as the name
  //  of the volume to be clipped will reset the clipping.
  //  Parameters
  //  NAME   Name of volume to be clipped 
  //  +
  //  XMIN   Lower limit of the Shape X coordinate
  //  XMAX   Upper limit of the Shape X coordinate
  //  YMIN   Lower limit of the Shape Y coordinate
  //  YMAX   Upper limit of the Shape Y coordinate
  //  ZMIN   Lower limit of the Shape Z coordinate
  //  ZMAX   Upper limit of the Shape Z coordinate
  //
  //  This function performs a boolean subtraction between the volume
  //  NAME and a box placed in the MARS according the values of the given
  //  coordinates.
  
//  InitHIGZ();
  if (Mode()==kWithRoot) return;


  char vname[5];
  Vname(name,vname);
  Float_t fxmin = xmin;
  Float_t fxmax = xmax;
  Float_t fymin = ymin;
  Float_t fymax = ymax;
  Float_t fzmin = zmin;
  Float_t fzmax = zmax;  
  setclip(PASSCHARD(vname),fxmin,fxmax,fymin,fymax,fzmin,fzmax PASSCHARL(vname));   
} 

//_____________________________________________________________________________
void StTGeant3::SetCOMP(Int_t par)
{
  //
  //  To control Compton scattering
  //   par =0 no Compton
  //       =1 Compton. Electron processed.
  //       =2 Compton. No electron stored.
  //  
  //
  fGcphys->icomp = par;
}
  
//_____________________________________________________________________________
void StTGeant3::SetCUTS(Float_t cutgam,Float_t cutele,Float_t cutneu,
		      Float_t cuthad,Float_t cutmuo ,Float_t bcute ,
		      Float_t bcutm ,Float_t dcute ,Float_t dcutm ,
		      Float_t ppcutm, Float_t tofmax)
{
  //
  //  CUTGAM   Cut for gammas              D=0.001
  //  CUTELE   Cut for electrons           D=0.001
  //  CUTHAD   Cut for charged hadrons     D=0.01
  //  CUTNEU   Cut for neutral hadrons     D=0.01
  //  CUTMUO   Cut for muons               D=0.01
  //  BCUTE    Cut for electron brems.     D=-1.
  //  BCUTM    Cut for muon brems.         D=-1.
  //  DCUTE    Cut for electron delta-rays D=-1.
  //  DCUTM    Cut for muon delta-rays     D=-1.
  //  PPCUTM   Cut for e+e- pairs by muons D=0.01
  //  TOFMAX   Time of flight cut          D=1.E+10
  //
  //   If the default values (-1.) for       BCUTE ,BCUTM ,DCUTE ,DCUTM
  //   are not modified, they will be set to CUTGAM,CUTGAM,CUTELE,CUTELE
  //   respectively.
  //  If one of the parameters from CUTGAM to PPCUTM included
  //  is modified, cross-sections and energy loss tables must be
  //  recomputed via the function Gphysi.
  //
  fGccuts->cutgam = cutgam;
  fGccuts->cutele = cutele;
  fGccuts->cutneu = cutneu;
  fGccuts->cuthad = cuthad;
  fGccuts->cutmuo = cutmuo;
  fGccuts->bcute  = bcute;
  fGccuts->bcutm  = bcutm;
  fGccuts->dcute  = dcute;
  fGccuts->dcutm  = dcutm;
  fGccuts->ppcutm = ppcutm;
  fGccuts->tofmax = tofmax;   
}

//_____________________________________________________________________________
void StTGeant3::SetDCAY(Int_t par)
{
  //
  //  To control Decay mechanism.
  //   par =0 no decays.
  //       =1 Decays. secondaries processed.
  //       =2 Decays. No secondaries stored.
  //  
  fGcphys->idcay = par;
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetDEBU(Int_t emin, Int_t emax, Int_t emod)
{
  //
  // Set the debug flag and frequency
  // Selected debug output will be printed from
  // event emin to even emax each emod event
  //
  fGcflag->idemin = emin;
  fGcflag->idemax = emax;
  fGcflag->itest  = emod;
  SetBit(kDEBU);
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetDRAY(Int_t par)
{
  //
  //  To control delta rays mechanism.
  //   par =0 no delta rays.
  //       =1 Delta rays. secondaries processed.
  //       =2 Delta rays. No secondaries stored.
  //  
  fGcphys->idray = par;
}
 
//_____________________________________________________________________________
void StTGeant3::SetERAN(Float_t ekmin, Float_t ekmax, Int_t nekbin)
{
  //
  //  To control cross section tabulations
  //   ekmin = minimum kinetic energy in GeV
  //   ekmax = maximum kinetic energy in GeV
  //   nekbin = number of logatithmic bins (<200)
  //  
  fGcmulo->ekmin = ekmin;
  fGcmulo->ekmax = ekmax;
  fGcmulo->nekbin = nekbin;
  SetBit(kERAN);
}
 
//_____________________________________________________________________________
void StTGeant3::SetHADR(Int_t par)
{
  //
  //  To control hadronic interactions.
  //   par =0 no hadronic interactions.
  //       =1 Hadronic interactions. secondaries processed.
  //       =2 Hadronic interactions. No secondaries stored.
  //  
  fGcphys->ihadr = par;
}
 
//_____________________________________________________________________________
void StTGeant3::SetKINE(Int_t kine, Float_t xk1, Float_t xk2, Float_t xk3,
		      Float_t xk4, Float_t xk5, Float_t xk6, Float_t xk7,
		      Float_t xk8, Float_t xk9, Float_t xk10)
{
  //
  // Set the variables in /GCFLAG/ IKINE, PKINE(10)
  // Their meaning is user defined
  //
  fGckine->ikine    = kine;
  fGckine->pkine[0] = xk1;
  fGckine->pkine[1] = xk2;
  fGckine->pkine[2] = xk3;
  fGckine->pkine[3] = xk4;
  fGckine->pkine[4] = xk5;
  fGckine->pkine[5] = xk6;
  fGckine->pkine[6] = xk7;
  fGckine->pkine[7] = xk8;
  fGckine->pkine[8] = xk9;
  fGckine->pkine[9] = xk10;
}
 
//_____________________________________________________________________________
void StTGeant3::SetLOSS(Int_t par)
{
  //
  //  To control energy loss.
  //   par =0 no energy loss;
  //       =1 restricted energy loss fluctuations;
  //       =2 complete energy loss fluctuations;
  //       =3 same as 1;
  //       =4 no energy loss fluctuations.
  //  If the value ILOSS is changed, then cross-sections and energy loss
  //  tables must be recomputed via the command 'PHYSI'.
  //  
  fGcphys->iloss = par;
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetMULS(Int_t par)
{
  //
  //  To control multiple scattering.
  //   par =0 no multiple scattering.
  //       =1 Moliere or Coulomb scattering.
  //       =2 Moliere or Coulomb scattering.
  //       =3 Gaussian scattering.
  //  
  fGcphys->imuls = par;
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetMUNU(Int_t par)
{
  //
  //  To control muon nuclear interactions.
  //   par =0 no muon-nuclear interactions.
  //       =1 Nuclear interactions. Secondaries processed.
  //       =2 Nuclear interactions. Secondaries not processed.
  //  
  fGcphys->imunu = par;
}
 
//_____________________________________________________________________________
void StTGeant3::SetOPTI(Int_t par)
{
  //
  //  This flag controls the tracking optimisation performed via the
  //  GSORD routine:
  //      1 no optimisation at all; GSORD calls disabled;
  //      0 no optimisation; only user calls to GSORD kept;
  //      1 all non-GSORDered volumes are ordered along the best axis;
  //      2 all volumes are ordered along the best axis.
  //  
  fGcopti->ioptim = par;
  SetBit(kOPTI);
}
 
//_____________________________________________________________________________
void StTGeant3::SetPAIR(Int_t par)
{
  //
  //  To control pair production mechanism.
  //   par =0 no pair production.
  //       =1 Pair production. secondaries processed.
  //       =2 Pair production. No secondaries stored.
  //  
  fGcphys->ipair = par;
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetPFIS(Int_t par)
{
  //
  //  To control photo fission mechanism.
  //   par =0 no photo fission.
  //       =1 Photo fission. secondaries processed.
  //       =2 Photo fission. No secondaries stored.
  //  
  fGcphys->ipfis = par;
}
  
//_____________________________________________________________________________
void StTGeant3::SetPHOT(Int_t par)
{
  //
  //  To control Photo effect.
  //   par =0 no photo electric effect.
  //       =1 Photo effect. Electron processed.
  //       =2 Photo effect. No electron stored.
  //  
  fGcphys->iphot = par;
}
 
//_____________________________________________________________________________
void StTGeant3::SetRAYL(Int_t par)
{
  //
  //  To control Rayleigh scattering.
  //   par =0 no Rayleigh scattering.
  //       =1 Rayleigh.
  //  
  fGcphys->irayl = par;
}
 
//_____________________________________________________________________________
void StTGeant3::SetSTRA(Int_t par)
{
  //
  //  To control energy loss fluctuations
  //  with the PhotoAbsorption Ionisation model.
  //   par =0 no Straggling.
  //       =1 Straggling yes => no Delta rays.
  //  
  fGcphlt->istra = par;
}
 
//_____________________________________________________________________________
void StTGeant3::SetSWIT(Int_t sw, Int_t val)
{
  //
  //  sw    Switch number
  //  val   New switch value
  //
  //  Change one element of array ISWIT(10) in /GCFLAG/
  //  
  if (sw <= 0 || sw > 10) return;
  fGcflag->iswit[sw-1] = val;
  SetBit(kSWIT);
}
 
 
//_____________________________________________________________________________
void StTGeant3::SetTRIG(Int_t nevents)
{
  //
  // Set number of events to be run
  //
  fGcflag->nevent = nevents;
  SetBit(kTRIG);
}
 
//_____________________________________________________________________________
void StTGeant3::SetUserDecay(Int_t pdg)
{
  //
  // Force the decays of particles to be done with Pythia
  // and not with the Geant routines. 
  // just kill pointers doing mzdrop
  //
  Int_t ipart = IdFromPDG(pdg);
  if(ipart<0) {
    printf("Particle %d not in geant\n",pdg);
    return;
  }
  Int_t jpart=fGclink->jpart;
  Int_t jpa=fZlq[jpart-ipart];
  //
  if(jpart && jpa) {
    Int_t jpa1=fZlq[jpa-1];
    if(jpa1)
      mzdrop(fGcbank->ixcons,jpa1,PASSCHARD(" ") PASSCHARL(" "));
    Int_t jpa2=fZlq[jpa-2];
    if(jpa2)
      mzdrop(fGcbank->ixcons,jpa2,PASSCHARD(" ") PASSCHARL(" "));
  }
}

//______________________________________________________________________________
void StTGeant3::Vname(const char *name, char *vname)
{
  //
  //  convert name to upper case. Make vname at least 4 chars
  //
  Int_t l = strlen(name);
  Int_t i;
  l = l < 4 ? l : 4;
  for (i=0;i<l;i++) vname[i] = toupper(name[i]);
  for (i=l;i<4;i++) vname[i] = ' ';
  vname[4] = 0;      
}
 
//______________________________________________________________________________
void StTGeant3::Ertrgo()
{
  //
  // Perform the tracking of the track Track parameters are in VECT
  //
  ertrgo();
}

//______________________________________________________________________________
void StTGeant3::Ertrak(const Float_t *const x1, const Float_t *const p1, 
			const Float_t *x2, const Float_t *p2,
			Int_t ipa,  Option_t *chopt)
{
  //************************************************************************
  //*                                                                      *
  //*          Perform the tracking of the track from point X1 to          *
  //*                    point X2                                          *
  //*          (Before calling this routine the user should also provide   *
  //*                    the input informations in /EROPTS/ and /ERTRIO/   *
  //*                    using subroutine EUFIL(L/P/V)                     *
  //*                 X1       - Starting coordinates (Cartesian)          *
  //*                 P1       - Starting 3-momentum  (Cartesian)          *
  //*                 X2       - Final coordinates    (Cartesian)          *
  //*                 P2       - Final 3-momentum     (Cartesian)          *
  //*                 IPA      - Particle code (a la GEANT) of the track   *
  //*                                                                      *
  //*                 CHOPT                                                *
  //*                     'B'   'Backward tracking' - i.e. energy loss     *
  //*                                        added to the current energy   *
  //*                     'E'   'Exact' calculation of errors assuming     *
  //*                                        helix (i.e. pathlength not    *
  //*                                        assumed as infinitesimal)     *
  //*                     'L'   Tracking upto prescribed Lengths reached   *
  //*                     'M'   'Mixed' prediction (not yet coded)         *
  //*                     'O'   Tracking 'Only' without calculating errors *
  //*                     'P'   Tracking upto prescribed Planes reached    *
  //*                     'V'   Tracking upto prescribed Volumes reached   *
  //*                     'X'   Tracking upto prescribed Point approached  *
  //*                                                                      *
  //*                Interface with GEANT :                                *
  //*             Track parameters are in /CGKINE/ and /GCTRAK/            *
  //*                                                                      *
  //*          ==>Called by : USER                                         *
  //*             Authors   M.Maire, E.Nagy  ********//*                     *
  //*                                                                      *
  //************************************************************************
  ertrak(x1,p1,x2,p2,ipa,PASSCHARD(chopt) PASSCHARL(chopt));
}
        
//_____________________________________________________________________________
void StTGeant3::WriteEuclid(const char* filnam, const char* topvol,
			  Int_t number, Int_t nlevel)
{
  //
  //
  //     ******************************************************************
  //     *                                                                *
  //     *  Write out the geometry of the detector in EUCLID file format  *
  //     *                                                                *
  //     *       filnam : will be with the extension .euc                 *
  //     *       topvol : volume name of the starting node                *
  //     *       number : copy number of topvol (relevant for gsposp)     *
  //     *       nlevel : number of  levels in the tree structure         *
  //     *                to be written out, starting from topvol         *
  //     *                                                                *
  //     *       Author : M. Maire                                        *
  //     *                                                                *
  //     ******************************************************************
  //
  //     File filnam.tme is written out with the definitions of tracking
  //     medias and materials.
  //     As to restore original numbers for materials and medias, program
  //     searches in the file euc_medi.dat and comparing main parameters of
  //     the mat. defined inside geant and the one in file recognizes them
  //     and is able to take number from file. If for any material or medium,
  //     this procedure fails, ordering starts from 1.
  //     Arrays IOTMED and IOMATE are used for this procedure
  //
  const char kShape[][5]={"BOX ","TRD1","TRD2","TRAP","TUBE","TUBS","CONE",
			 "CONS","SPHE","PARA","PGON","PCON","ELTU","HYPE",
			 "GTRA","CTUB"};
  Int_t i, end, itm, irm, jrm, k, nmed;
  Int_t imxtmed=0;
  Int_t imxmate=0;
  FILE *lun;
  char *filext, *filetme;
  char natmed[21], namate[21];
  char natmedc[21], namatec[21];
  char key[5], name[5], mother[5], konly[5];
  char card[133];
  Int_t iadvol, iadtmd, iadrot, nwtot, iret;
  Int_t mlevel, numbr, natt, numed, nin, ndata;
  Int_t iname, ivo, ish, jvo, nvstak, ivstak;
  Int_t jdiv, ivin, in, jin, jvin, irot;
  Int_t jtm, imat, jma, flag=0, imatc;
  Float_t az, dens, radl, absl, a, step, x, y, z;
  Int_t npar, ndvmx, left;
  Float_t zc, densc, radlc, abslc, c0, tmaxfd;
  Int_t nparc, numb;
  Int_t iomate[100], iotmed[100];
  Float_t par[100], att[20], ubuf[50];
  Float_t *qws;
  Int_t   *iws;
  Int_t level, ndiv, iaxe;
  Int_t itmedc, nmatc, isvolc, ifieldc, nwbufc, isvol, nmat, ifield, nwbuf;
  Float_t fieldmc, tmaxfdc, stemaxc, deemaxc, epsilc, stminc, fieldm;
  Float_t tmaxf, stemax, deemax, epsil, stmin;
  const char *k10000="!\n%s\n!\n";
  //Open the input file
  end=strlen(filnam);
  for(i=0;i<end;i++) if(filnam[i]=='.') {
    end=i;
    break;
  }
  filext=new char[end+5];
  filetme=new char[end+5];
  strncpy(filext,filnam,end);
  strncpy(filetme,filnam,end);
  //
  // *** The output filnam name will be with extension '.euc'
  strcpy(&filext[end],".euc");
  strcpy(&filetme[end],".tme");
  lun=fopen(filext,"w");
  //
  // *** Initialisation of the working space
  iadvol=fGcnum->nvolum;
  iadtmd=iadvol+fGcnum->nvolum;
  iadrot=iadtmd+fGcnum->ntmed;
  if(fGclink->jrotm) {
    fGcnum->nrotm=fZiq[fGclink->jrotm-2];
  } else {
    fGcnum->nrotm=0;
  }
  nwtot=iadrot+fGcnum->nrotm;
  qws = new float[nwtot+1];
  for (i=0;i<nwtot+1;i++) qws[i]=0;
  iws = (Int_t*) qws;
  mlevel=nlevel;
  if(nlevel==0) mlevel=20;
  //
  // *** find the top volume and put it in the stak
  numbr = number>0 ? number : 1;
  Gfpara(topvol,numbr,1,npar,natt,par,att);
  if(npar <= 0) {
    printf(" *** GWEUCL *** top volume : %s number : %3d can not be a valid root\n",
	   topvol, numbr);
    return;
  }
  //
  // ***  authorized shape ?
  strncpy((char *)&iname, topvol, 4);
  ivo=0;
  for(i=1; i<=fGcnum->nvolum; i++) if(fZiq[fGclink->jvolum+i]==iname) {
    ivo=i;
    break;
  }
  jvo = fZlq[fGclink->jvolum-ivo];
  ish = Int_t (fZq[jvo+2]);
  if(ish > 12) {
    printf(" *** GWEUCL *** top volume : %s number : %3d can not be a valid root\n",
	   topvol, numbr);
  }
  //
  level = 1;
  nvstak = 1;
  iws[nvstak]     = ivo;
  iws[iadvol+ivo] = level;
  ivstak = 0;
  //
  //*** flag all volumes and fill the stak
  //
 L10:
  //
  //    pick the next volume in stak
  ivstak += 1;
  ivo   = TMath::Abs(iws[ivstak]);
  jvo   = fZlq[fGclink->jvolum - ivo];
  //
  //     flag the tracking medium
  numed =  Int_t (fZq[jvo + 4]);
  iws[iadtmd + numed] = 1;
  //
  //    get the daughters ...
  level = iws[iadvol+ivo];
  if (level < mlevel) {
    level +=  1;
    nin = Int_t (fZq[jvo + 3]);
    //
    //       from division ...
    if (nin < 0) {
      jdiv = fZlq[jvo  - 1];
      ivin =  Int_t (fZq[jdiv + 2]);
      nvstak += 1;
      iws[nvstak]      = -ivin;
      iws[iadvol+ivin] =  level;
      //
      //       from position ...
    } else if (nin > 0) {
      for(in=1; in<=nin; in++) {
	jin  = fZlq[jvo - in];
	ivin =  Int_t (fZq[jin + 2 ]);
	jvin = fZlq[fGclink->jvolum - ivin];
	ish  =  Int_t (fZq[jvin + 2]);
	//              authorized shape ?
	if (ish <= 12) {
	  //                 not yet flagged ?
	  if (iws[iadvol+ivin]==0) {
	    nvstak += 1;
	    iws[nvstak]      = ivin;
	    iws[iadvol+ivin] = level;
	  }
	  //                 flag the rotation matrix
	  irot = Int_t ( fZq[jin + 4 ]);
	  if (irot > 0) iws[iadrot+irot] = 1;
	}
      }
    }
  }
  //
  //     next volume in stak ?
  if (ivstak < nvstak) goto L10;
  //
  // *** restore original material and media numbers
  // file euc_medi.dat is needed to compare materials and medias
  //
  FILE* luncor=fopen("euc_medi.dat","r");
  //
  if(luncor) {
    for(itm=1; itm<=fGcnum->ntmed; itm++) {
      if (iws[iadtmd+itm] > 0) {
	jtm = fZlq[fGclink->jtmed-itm];
	strncpy(natmed,(char *)&fZiq[jtm+1],20);
	imat =  Int_t (fZq[jtm+6]);
	jma  = fZlq[fGclink->jmate-imat];
	if (jma <= 0) {
	  printf(" *** GWEUCL *** material not defined for tracking medium %5i %s\n",itm,natmed);
	  flag=1;
	} else {
	  strncpy(namate,(char *)&fZiq[jma+1],20);
	}
	//*
	//** find the material original number
	rewind(luncor);
      L23:
	iret=fscanf(luncor,"%4s,%130s",key,card);
	if(iret<=0) goto L26;
	flag=0;
	if(!strcmp(key,"MATE")) {
	  sscanf(card,"%d %s %f %f %f %f %f %d",&imatc,namatec,&az,&zc,&densc,&radlc,&abslc,&nparc);
	  Gfmate(imat,namate,a,z,dens,radl,absl,par,npar);
	  if(!strcmp(namatec,namate)) {
	    if(az==a && zc==z && densc==dens && radlc==radl 
	       && abslc==absl && nparc==nparc) {
	      iomate[imat]=imatc;
	      flag=1;
	      printf("*** GWEUCL *** material : %3d '%s' restored as %3d\n",imat,namate,imatc);
	    } else {
	      printf("*** GWEUCL *** different definitions for material: %s\n",namate);
	    }
	  }
	}
	if(strcmp(key,"END") && !flag) goto L23;
	if (!flag) {
	  printf("*** GWEUCL *** cannot restore original number for material: %s\n",namate);
	}
	//*
	//*
	//***  restore original tracking medium number
	rewind(luncor);
      L24:
	iret=fscanf(luncor,"%4s,%130s",key,card);
	if(iret<=0) goto L26;
	flag=0;
	if (!strcmp(key,"TMED")) {
	  sscanf(card,"%d %s %d %d %d %f %f %f %f %f %f %d\n",
		 &itmedc,natmedc,&nmatc,&isvolc,&ifieldc,&fieldmc,
		 &tmaxfdc,&stemaxc,&deemaxc,&epsilc,&stminc,&nwbufc);
	  Gftmed(itm,natmed,nmat,isvol,ifield,fieldm,tmaxf,stemax,deemax,
			epsil,stmin,ubuf,&nwbuf);
	  if(!strcmp(natmedc,natmed)) {
	    if (iomate[nmat]==nmatc && nwbuf==nwbufc) {
	      iotmed[itm]=itmedc;
	      flag=1;
	      printf("*** GWEUCL *** medium   : %3d '%20s' restored as %3d\n",
		     itm,natmed,itmedc);
	    } else {
	      printf("*** GWEUCL *** different definitions for tracking medium: %s\n",natmed);
	    }
	  }
	}
	if(strcmp(key,"END") && !flag) goto L24;
	if(!flag) {
	  printf("cannot restore original number for medium : %s\n",natmed);
	  goto L27;
	}
      }
    }
    goto L29;
    //*
  }
 L26:   printf("*** GWEUCL *** cannot read the data file\n");
 L27:   flag=2;
 L29:   if(luncor) fclose (luncor);
  //
  //
  // *** write down the tracking medium definition
  //
  strcpy(card,"!       Tracking medium");
  fprintf(lun,k10000,card);
  //
  for(itm=1;itm<=fGcnum->ntmed;itm++) {
    if (iws[iadtmd+itm]>0) {
      jtm  = fZlq[fGclink->jtmed-itm];
      strncpy(natmed,(char *)&fZiq[jtm+1],20);
      natmed[20]='\0';
      imat =  Int_t (fZq[jtm+6]);
      jma  = fZlq[fGclink->jmate-imat];
      //*  order media from one, if comparing with database failed
      if (flag==2) {
	iotmed[itm]=++imxtmed;
	iomate[imat]=++imxmate;
      }
      //*
      if(jma<=0) {
	strcpy(namate,"                  ");
	printf(" *** GWEUCL *** material not defined for tracking medium %5d %s\n",
	       itm,natmed);
      } else {
	strncpy(namate,(char *)&fZiq[jma+1],20);
	namate[20]='\0';
      }
      fprintf(lun,"TMED %3d '%20s' %3d '%20s'\n",iotmed[itm],natmed,iomate[imat],namate);
    }
  }
  //*
      //* *** write down the rotation matrix
  //*
  strcpy(card,"!       Reperes");
  fprintf(lun,k10000,card);
  //
  for(irm=1;irm<=fGcnum->nrotm;irm++) {
    if (iws[iadrot+irm]>0) {
      jrm  = fZlq[fGclink->jrotm-irm];
      fprintf(lun,"ROTM %3d",irm);
      for(k=11;k<=16;k++) fprintf(lun," %8.3f",fZq[jrm+k]);
      fprintf(lun,"\n");
    }
  }
  //*
  //* *** write down the volume definition
  //*
  strcpy(card,"!       Volumes");
  fprintf(lun,k10000,card);
  //*
  for(ivstak=1;ivstak<=nvstak;ivstak++) {
    ivo = iws[ivstak];
    if (ivo>0) {
      strncpy(name,(char *)&fZiq[fGclink->jvolum+ivo],4);
      name[4]='\0';
      jvo  = fZlq[fGclink->jvolum-ivo];
      ish   = Int_t (fZq[jvo+2]);
      nmed  = Int_t (fZq[jvo+4]);
      npar  = Int_t (fZq[jvo+5]);
      if (npar>0) {
	if (ivstak>1) for(i=0;i<npar;i++) par[i]=fZq[jvo+7+i];
	Gckpar (ish,npar,par);
	fprintf(lun,"VOLU '%4s' '%4s' %3d %3d\n",name,kShape[ish-1],iotmed[nmed],npar);
	for(i=0;i<(npar-1)/6+1;i++) {
	  fprintf(lun,"     ");
	  left=npar-i*6;
	  for(k=0;k<(left<6?left:6);k++) fprintf(lun," %11.5f",par[i*6+k]);
	  fprintf(lun,"\n");
	}
      } else {
	fprintf(lun,"VOLU '%4s' '%4s' %3d %3d\n",name,kShape[ish-1],iotmed[nmed],npar);
      }
    }
  }
  //*
  //* *** write down the division of volumes
  //*
  fprintf(lun,k10000,"!       Divisions");
  for(ivstak=1;ivstak<=nvstak;ivstak++) {
    ivo = TMath::Abs(iws[ivstak]);
    jvo  = fZlq[fGclink->jvolum-ivo];
    ish  = Int_t (fZq[jvo+2]);
    nin  = Int_t (fZq[jvo+3]);
    //*        this volume is divided ...
    if (nin<0) {
      jdiv = fZlq[jvo-1];
      iaxe = Int_t ( fZq[jdiv+1]);
      ivin = Int_t ( fZq[jdiv+2]);
      ndiv = Int_t ( fZq[jdiv+3]);
      c0   =  fZq[jdiv+4];
      step =  fZq[jdiv+5];
      jvin = fZlq[fGclink->jvolum-ivin];
      nmed = Int_t ( fZq[jvin+4]);
      strncpy(mother,(char *)&fZiq[fGclink->jvolum+ivo ],4);
      mother[4]='\0';
      strncpy(name,(char *)&fZiq[fGclink->jvolum+ivin],4);
      name[4]='\0';
      if ((step<=0.)||(ish>=11)) {
	//*              volume with negative parameter or gsposp or pgon ...
	fprintf(lun,"DIVN '%4s' '%4s' %3d %3d\n",name,mother,ndiv,iaxe);
      } else if ((ndiv<=0)||(ish==10)) {
	//*              volume with negative parameter or gsposp or para ...
	ndvmx = TMath::Abs(ndiv);
	fprintf(lun,"DIVT '%4s' '%4s' %11.5f %3d %3d %3d\n",
		name,mother,step,iaxe,iotmed[nmed],ndvmx);
      } else {
	//*              normal volume : all kind of division are equivalent
	fprintf(lun,"DVT2 '%4s' '%4s' %11.5f %3d %11.5f %3d %3d\n",
		name,mother,step,iaxe,c0,iotmed[nmed],ndiv);
      }
    }
  }
  //*
  //* *** write down the the positionnement of volumes
  //*
  fprintf(lun,k10000,"!       Positionnements\n");
  //
  for(ivstak = 1;ivstak<=nvstak;ivstak++) {
    ivo = TMath::Abs(iws[ivstak]);
    strncpy(mother,(char*)&fZiq[fGclink->jvolum+ivo ],4);
    mother[4]='\0';
    jvo  = fZlq[fGclink->jvolum-ivo];
    nin  = Int_t( fZq[jvo+3]);
    //*        this volume has daughters ...
    if (nin>0) {
      for (in=1;in<=nin;in++) {
	jin  = fZlq[jvo-in];
	ivin =  Int_t (fZq[jin +2]);
	numb =  Int_t (fZq[jin +3]);
	irot =  Int_t (fZq[jin +4]);
	x    =  fZq[jin +5];
	y    =  fZq[jin +6];
	z    =  fZq[jin +7];
	strcpy(konly,"ONLY");
	if (fZq[jin+8]!=1.) strcpy(konly,"MANY");
	strncpy(name,(char*)&fZiq[fGclink->jvolum+ivin],4);
	name[4]='\0';
	jvin = fZlq[fGclink->jvolum-ivin];
	ish  = Int_t (fZq[jvin+2]);
	//*              gspos or gsposp ?
	ndata = fZiq[jin-1];
	if (ndata==8) {
	  fprintf(lun,"POSI '%4s' %4d '%4s' %11.5f %11.5f %11.5f %3d '%4s'\n",
		  name,numb,mother,x,y,z,irot,konly);
	} else {
	  npar =  Int_t (fZq[jin+9]);
	  for(i=0;i<npar;i++) par[i]=fZq[jin+10+i];
	  Gckpar (ish,npar,par);
	  fprintf(lun,"POSP '%4s' %4d '%4s' %11.5f %11.5f %11.5f %3d '%4s' %3d\n",
		  name,numb,mother,x,y,z,irot,konly,npar);
	  fprintf(lun,"     ");
	  for(i=0;i<npar;i++) fprintf(lun," %11.5f",par[i]);
	  fprintf(lun,"\n");
	}
      }
    }
  }
  //*
  fprintf(lun,"END\n");
  fclose(lun);
  //*
  //****** write down the materials and medias *****
  //*
  lun=fopen(filetme,"w");
  //*
  for(itm=1;itm<=fGcnum->ntmed;itm++) {
    if (iws[iadtmd+itm]>0) {
      jtm  = fZlq[fGclink->jtmed-itm];
      strncpy(natmed,(char*)&fZiq[jtm+1],4);
      imat =  Int_t (fZq[jtm+6]);
      jma  =  Int_t (fZlq[fGclink->jmate-imat]);
      //*  material
      Gfmate (imat,namate,a,z,dens,radl,absl,par,npar);
      fprintf(lun,"MATE %4d '%20s'%11.5E %11.5E %11.5E %11.5E %11.5E %3d\n",
	     iomate[imat],namate,a,z,dens,radl,absl,npar);
      //*
      if (npar>0) {
	  fprintf(lun,"     ");
	  for(i=0;i<npar;i++) fprintf(lun," %11.5f",par[i]);
	  fprintf(lun,"\n");
      }
      //*  medium
      Gftmed(itm,natmed,nmat,isvol,ifield,fieldm,tmaxfd,stemax,deemax,epsil,stmin,par,&npar);
      fprintf(lun,"TMED %4d '%20s' %3d %1d %3d %11.5f %11.5f %11.5f %11.5f %11.5f %11.5f %3d\n",
	      iotmed[itm],natmed,iomate[nmat],isvol,ifield,
	      fieldm,tmaxfd,stemax,deemax,epsil,stmin,npar);
      //*
      if (npar>0) {
	  fprintf(lun,"     ");
	  for(i=0;i<npar;i++) fprintf(lun," %11.5f",par[i]);
	  fprintf(lun,"\n");
      }
      
    }
  }
  fprintf(lun,"END\n");
  fclose(lun);
  printf(" *** GWEUCL *** file: %s is now written out\n",filext);
  printf(" *** GWEUCL *** file: %s is now written out\n",filetme);
  // Clean up
  delete [] filext;
  delete [] filetme;
  delete [] qws;
  iws=0;
  return;
}
 
//_____________________________________________________________________________
Int_t  StTGeant3::TransportMethod(TMCParticleType particleType) const
{
// 
// Returns G3 transport method code for the specified MCParticleType
// ---

  switch (particleType) {
    case kPTGamma:    return 1;    
    case kPTElectron: return 2;      
    case kPTNeutron:  return 3;
    case kPTHadron:   return 4;
    case kPTMuon:     return 5;
    case kPTGeantino: return 6;
    case kPTOpticalPhoton: return 7;
    case kPTIon:      return 8;
    default:          return -1; 
  }
}

//_____________________________________________________________________________
TMCParticleType  StTGeant3::ParticleType(Int_t itrtyp) const
{
// 
// Returns MCParticleType for the specified G3 transport method code
// ---

  switch (itrtyp) {
    case 1:  return  kPTGamma;
    case 2:  return  kPTElectron;
    case 3:  return  kPTNeutron;
    case 4:  return  kPTHadron;
    case 5:  return  kPTMuon;
    case 6:  return  kPTGeantino;
    case 7:  return  kPTOpticalPhoton;
    case 8:  return  kPTIon;
    default: return  kPTUndefined; 
  }
}

//_____________________________________________________________________________
TString  StTGeant3::ParticleClass(TMCParticleType particleType) const
{
// 
// Returns particle class name (used in TDatabasePDG) for 
// the specified MCParticleType
// ---

  // CHECK
  switch (particleType) {
    case kPTGamma:    return TString("Photon");    
    case kPTElectron: return TString("Lepton");      
    case kPTNeutron:  return TString("Hadron");
    case kPTHadron:   return TString("Hadron");
    case kPTMuon:     return TString("Lepton");
    case kPTGeantino: return TString("Special");
    case kPTIon:      return TString("Ion");
    case kPTOpticalPhoton: return TString("Photon");
    default:          return TString("Unknown");
  }
}

//____________________________________________________________________________
void StTGeant3::FinishGeometry()
{
  //
  // Finalise geometry construction
  //

  //Close the geometry structure
  if (gDebug > 0) printf("FinishGeometry, calling ggclos\n");
  Ggclos();

  if (Mode()&kWithRoot){
    if (fImportRootGeometry) {
      // Materials
      TIter next1(gGeoManager->GetListOfMaterials());
      TGeoMaterial* mat=0;
      while ((mat=(TGeoMaterial*)next1())) {
	Int_t kmat=mat->GetUniqueID();
	TGeoMixture* mixt = dynamic_cast<TGeoMixture*>(mat);
	if (mixt) {
          Int_t nlmat = mixt->GetNelements(); 
          Float_t* fa = CreateFloatArray(mixt->GetAmixt(), TMath::Abs(nlmat));  
          Float_t* fz = CreateFloatArray(mixt->GetZmixt(), TMath::Abs(nlmat));  
          Float_t* fwmat = CreateFloatArray(mixt->GetWmixt(), TMath::Abs(nlmat));  
          G3Mixture(kmat, mixt->GetName(), fa, fz, mixt->GetDensity(), nlmat, fwmat); 
          delete [] fa;
          delete [] fz;
          delete [] fwmat;
	} 
	else {
          Float_t* buf = 0;
          G3Material(kmat, mat->GetName(), mat->GetA(), mat->GetZ(),
                     mat->GetDensity(), mat->GetRadLen(), mat->GetIntLen(), buf, 0);
                                                  // Is fIntLen == absl ??
	}
	mat->SetUniqueID(kmat);
      }  	         	        

      // Media
      TIter next2(gGeoManager->GetListOfMedia());
      TGeoMedium* med;
      while ((med=(TGeoMedium*)next2())) {
	Int_t kmed = med->GetId();
	Int_t nmat = med->GetMaterial()->GetUniqueID();
	Int_t isvol  = (Int_t) med->GetParam(0);
	Int_t ifield = (Int_t) med->GetParam(1);
	Double_t fieldm = med->GetParam(2);
	Double_t tmaxfd = med->GetParam(3);
	Double_t stemax = med->GetParam(4);
	Double_t deemax = med->GetParam(5);
	Double_t epsil  = med->GetParam(6);
	Double_t stmin  = med->GetParam(7);
	G3Medium(kmed, med->GetName(), nmat, isvol, ifield, fieldm, tmaxfd,
        	 stemax,deemax, epsil, stmin);
	med->SetUniqueID(kmed);	         	        
      }
      if (gDebug > 0) printf("FinishGeometry, geometry retreived from file, materials/media mapped to G3\n");
    } else {
      TGeoVolume *top = (TGeoVolume*)gGeoManager->GetListOfVolumes()->First();
      gGeoManager->SetTopVolume(top);
      if (gDebug > 0) printf("FinishGeometry, calling CloseGeometry\n");
      gGeoManager->CloseGeometry();  
    }  
}

  //  gROOT->GetListOfBrowsables()->Add(gGeoManager);
  if (gDebug > 0) printf("FinishGeometry, calling SetColors\n");

  //Create the color table
  SetColors();
  if (gDebug > 0) printf("FinishGeometry, returning\n");
}

//____________________________________________________________________________
void StTGeant3::Init()
{
    //
    //=================Create Materials and geometry
    //

    //  Some default settings, if not changed by user
    if (!TestBit(kTRIG)) SetTRIG(1);         // Number of events to be processed
    if (!TestBit(kSWIT)) SetSWIT(4, 10);     //
    if (!TestBit(kDEBU)) SetDEBU(0, 0, 1);   //
    if (!TestBit(kAUTO)) SetAUTO(1);         // Select automatic STMIN etc... calc. (AUTO 1) or manual (AUTO 0)
    if (!TestBit(kABAN)) SetABAN(0);         // Restore 3.16 behaviour for abandoned tracks
    if (!TestBit(kOPTI)) SetOPTI(2);         // Select optimisation level for GEANT geometry searches (0,1,2)
    if (!TestBit(kERAN)) SetERAN(5.e-7);     //

    DefineParticles();   
    fApplication->AddParticles();
    fApplication->ConstructGeometry();
    FinishGeometry();
    fApplication->InitGeometry();
}

//____________________________________________________________________________
Bool_t StTGeant3::ProcessRun(Int_t nevent)
{
  //
  // Process the run
  //
  
  Int_t todo = TMath::Abs(nevent);
  for (Int_t i=0; i<todo; i++) {
  // Process one run (one run = one event)
     fGcflag->idevt  = i;
     fGcflag->ievent = i+1;
     fApplication->BeginEvent();
     ProcessEvent();
     fApplication->FinishEvent();
  }
   return 0;
}

//_____________________________________________________________________________
void StTGeant3::ProcessEvent()
{
  //
  // Process one event
  //
  Gtrigi();
  Gtrigc();
  Gtrig();
}

//_____________________________________________________________________________
void StTGeant3::SetColors()
{
  //
  // Set the colors for all the volumes
  // this is done sequentially for all volumes
  // based on the number of their medium
  //
  if (Mode()&kWithG3){
    Int_t kv, icol;
    Int_t jvolum=fGclink->jvolum;
    //Int_t jtmed=fGclink->jtmed;
    //Int_t jmate=fGclink->jmate;
    Int_t nvolum=fGcnum->nvolum;
    char name[5];
    //
    //    Now for all the volumes
    for(kv=1;kv<=nvolum;kv++) {
      //     Get the tracking medium
      Int_t itm=Int_t (fZq[fZlq[jvolum-kv]+4]);
      //     Get the material
      //Int_t ima=Int_t (fZq[fZlq[jtmed-itm]+6]);
      //     Get z
      //Float_t z=fZq[fZlq[jmate-ima]+7];
      //     Find color number
      //icol = Int_t(z)%6+2;
      //icol = 17+Int_t(z*150./92.);
      //icol = kv%6+2;
      icol = itm%6+2;
      strncpy(name,(char*)&fZiq[jvolum+kv],4);
      name[4]='\0';
      Gsatt(name,"COLO",icol);
    }
}
  if (Mode()==kWithRoot){
    TIter next(gGeoManager->GetListOfVolumes());
    TGeoVolume *volume;
    while ((volume = (TGeoVolume*)next())) {
       TGeoMedium *medium = (TGeoMedium*)volume->GetMedium();
       Int_t icol = medium->GetId()%6+2;
       volume->SetLineColor(icol);
    }
  }
}

//_____________________________________________________________________________
void StTGeant3::SetTrack(Int_t done, Int_t parent, Int_t pdg, Float_t *pmom,
		        Float_t *vpos, Float_t *polar, Float_t tof,
		        TMCProcess mech, Int_t &ntr, Float_t weight, Int_t is)
{ 
  //
  // Load a track on the stack
  //
  // done     0 if the track has to be transported
  //          1 if not
  // parent   identifier of the parent track. -1 for a primary
  // pdg    particle code
  // pmom     momentum GeV/c
  // vpos     position 
  // polar    polarisation 
  // tof      time of flight in seconds
  // mecha    production mechanism
  // ntr      on output the number of the track stored
  //

  //  const Float_t tlife=0;
  
  //
  // Here we get the static mass
  // For MC is ok, but a more sophisticated method could be necessary
  // if the calculated mass is required
  // also, this method is potentially dangerous if the mass
  // used in the MC is not the same of the PDG database
  //
  Float_t mass = TDatabasePDG::Instance()->GetParticle(pdg)->Mass();
  Float_t e=TMath::Sqrt(mass*mass+pmom[0]*pmom[0]+
			pmom[1]*pmom[1]+pmom[2]*pmom[2]);
  
//    printf("Loading  mass %f ene %f No %d ip %d parent %d done %d pos %f %f %f mom %f %f %f kS %d m \n",
//	   mass,e,fNtrack,pdg,parent,done,vpos[0],vpos[1],vpos[2],pmom[0],pmom[1],pmom[2],kS);
  

  GetStack()->PushTrack(done, parent, pdg, pmom[0], pmom[1], pmom[2], e,
                       vpos[0], vpos[1], vpos[2], tof, polar[0], polar[1], polar[2],
                       mech, ntr, weight, is);
}


//_____________________________________________________________________________
Float_t* StTGeant3::CreateFloatArray(Double_t* array, Int_t size) const
{
// Converts Double_t* array to Float_t*,
// !! The new array has to be deleted by user.
// ---

  Float_t* floatArray;
  if (size>0) {
    floatArray = new Float_t[size]; 
    for (Int_t i=0; i<size; i++) floatArray[i] = array[i];
  }
  else {
    //floatArray = 0; 
    floatArray = new Float_t[1]; 
  }  
  return floatArray;
}


//_____________________________________________________________________________
//
//                 Interfaces to Fortran
//
//_____________________________________________________________________________


//_____________________________________________________________________________

void StTGeant3::Brxgtrak (Int_t &mtrack, Int_t &ipart, Float_t *pmom, 
				       Float_t &e, Float_t *vpos, Float_t *polar,
				       Float_t &tof)
{
  //
  //     Fetches next track from the ROOT stack for transport. Called by the
  //     modified version of GTREVE.
  //
  //              Track number in the ROOT stack. If MTRACK=0 no
  //      mtrack  more tracks are left in the stack to be
  //              transported.
  //      ipart   Particle code in the GEANT conventions.
  //      pmom[3] Particle momentum in GeV/c
  //      e       Particle energy in GeV
  //      vpos[3] Particle position
  //      tof     Particle time of flight in seconds
  //
  
  TParticle* track = gMC->GetStack()->PopNextTrack(mtrack);

  if (track) {
    // fill G3 arrays
    pmom[0] = track->Px(); 
    pmom[1] = track->Py(); 
    pmom[2] = track->Pz();
    e = track->Energy();
    vpos[0] = track->Vx();; 
    vpos[1] = track->Vy();
    vpos[2] = track->Vz();
    tof = track->T();
    TVector3 pol;
    track->GetPolarisation(pol);
    polar[0] = pol.X(); 
    polar[1] = pol.Y(); 
    polar[2] = pol.Z();
    ipart = gMC->IdFromPDG(track->GetPdgCode());
  }
  
  mtrack++;
}


//_____________________________________________________________________________
void StTGeant3::Brxouth ()
{
  //
  // Called by Gtreve at the end of each primary track
  //
  TVirtualMCApplication::Instance()->FinishPrimary();
}

//_____________________________________________________________________________
void StTGeant3::Brxinh ()
{
  //
  // Called by Gtreve at the beginning of each primary track
  //
  TVirtualMCApplication::Instance()->BeginPrimary();
}

#ifndef WIN32
#  define gudigi gudigi_
#  define guhadr guhadr_
#  define guout  guout_
#  define guphad guphad_
#  define gudcay gudcay_
#  define guiget guiget_
#  define guinme guinme_
#  define guinti guinti_
#  define gunear gunear_
#  define guskip guskip_
#  define guview guview_
#  define gupara gupara_
#  define gudtim gudtim_
#  define guplsh guplsh_
#  define gutrev gutrev_
#  define gutrak gutrak_
#  define guswim guswim_
#  define gufld  gufld_
#  define gustep gustep_
#  define gukine gukine_

#  define gheish gheish_
#  define flufin flufin_
#  define gfmfin gfmfin_
#  define gpghei gpghei_
#  define fldist fldist_
#  define gfmdis gfmdis_
#  define g3helx3 g3helx3_
#  define g3helix g3helix_
#  define g3rkuta g3rkuta_
#  define g3track g3track_
#  define gtreveroot gtreveroot_
#  define g3last  g3last_
#  define g3invol g3invol_
#  define g3tmedi g3tmedi_
#  define g3media g3media_
#  define g3tmany g3tmany_
#  define g3tnext g3tnext_
#  define g3gperp g3gperp_
#  define ginvol ginvol_
#  define gtmedi gtmedi_
#  define gtmany gtmany_
#  define gtonly gtonly_
#  define gmedia gmedia_
#  define glvolu glvolu_
#  define gtnext gtnext_
#  define ggperp ggperp_

#else
#  define gudigi GUDIGI
#  define guhadr GUHADR
#  define guout  GUOUT
#  define guphad GUPHAD
#  define gudcay GUDCAY
#  define guiget GUIGET
#  define guinme GUINME
#  define guinti GUINTI
#  define gunear GUNEAR
#  define guskip GUSKIP
#  define guview GUVIEW
#  define gupara GUPARA
#  define gudtim GUDTIM
#  define guplsh GUPLSH
#  define gutrev GUTREV
#  define gutrak GUTRAK
#  define guswim GUSWIM
#  define gufld  GUFLD
#  define gustep GUSTEP
#  define gukine GUKINE

#  define gheish GHEISH
#  define flufin FLUFIN
#  define gfmfin GFMFIN
#  define gpghei GPGHEI
#  define fldist FLDIST
#  define gfmdis GFMDIS
#  define g3helx3 G3HELX3
#  define g3helix G3HELIX
#  define g3gperp G3GPERP
#  define g3rkuta G3RKUTA
#  define gtrack GTRACK
#  define gtreveroot GTREVEROOT
#  define glast  GLAST
#  define ginvol GINVOL
#  define gtmedi GTMEDI
#  define gtmany GTMANY
#  define gmedia GMEDIA
#  define glvolu GLVOLU
#  define gtnext GTNEXT 
#  define ggperp GGPERP

#endif

extern "C" type_of_call void gheish();
extern "C" type_of_call void flufin();
extern "C" type_of_call void gfmfin();
extern "C" type_of_call void gpghei();
extern "C" type_of_call void fldist();
extern "C" type_of_call void gfmdis();
extern "C" type_of_call void g3helx3(Float_t&, Float_t&, Float_t*, Float_t*);
extern "C" type_of_call void g3helix(Float_t&, Float_t&, Float_t*, Float_t*);
extern "C" type_of_call void g3rkuta(Float_t&, Float_t&, Float_t*, Float_t*);
extern "C" type_of_call void g3gperp(Float_t*, Float_t*, Int_t&);
extern "C" type_of_call void g3track();
extern "C" type_of_call void gtreveroot();
extern "C" type_of_call void g3last();
extern "C" type_of_call void g3invol(Float_t*, Int_t&);
extern "C" type_of_call void g3tmedi(Float_t*, Int_t&);
extern "C" type_of_call void g3tmany(Int_t&);
extern "C" type_of_call void g3media(Float_t*, Int_t&, Int_t&);
extern "C" type_of_call void g3tnext();
extern "C" type_of_call void ginvol(Float_t*, Int_t&);
extern "C" type_of_call void gtmedi(Float_t*, Int_t&);
extern "C" type_of_call void gtmany(Int_t&);
extern "C" type_of_call void gtonly(Int_t&);
extern "C" type_of_call void gmedia(Float_t*, Int_t&, Int_t&);
extern "C" type_of_call void glvolu(Int_t &nlev, Int_t *lnam,Int_t *lnum, Int_t &ier);
extern "C" type_of_call void gtnext();
extern "C" type_of_call void ggperp(Float_t*, Float_t*, Int_t&);


//______________________________________________________________________
void StTGeant3::Bginvol(Float_t *x, Int_t &isame)
{
 
//count_ginvol++;
#ifdef STATISTICS
   statcode = 0;
   for (int j=0;j<6;j++) if (j <3) oldvect[j] = x[j]; else oldvect[j]=0;
   oldsafety = fgGctrack->safety;
   oldstep   = fgGctrack->step;
#endif
  if (Mode()==kWithBoth){
     g3invol(x,isame);

     const char *gnode = fgGeant3->GetNodeName();
     if (fgCurrentNode) {
	// make sure the reference paths are the same, otherwise comparison makes no sense
	if (strcmp(fgCurrentNode->GetName(), gnode)) return;
     }   

     Int_t rsame;
     TGeoNode *node = gGeoManager->FindNode(x[0], x[1], x[2]);
     if (gGeoManager->IsOutside()) node=0;
     rsame = (node==fgCurrentNode)?1:0;

     static Int_t count_err = 0;
     if (isame != rsame) {
	// same starting node, different paths -> error
	count_err++;
	if (isame) {
           printf("%6d INVOL ERR=%d (%g, %g, %g): only TGeo have crossed a boundary after step\n", count_ginvol, count_err, x[0], x[1], x[2]);
           printf("         gpath: %s\n", fgGeant3->GetPath());
	} else {
           printf("%6d INVOL ERR=%d (%g, %g, %g): only G3 have crossed a boundary after step\n", count_ginvol, count_err, x[0], x[1], x[2]);
           printf("         gpath: %s\n", fgGeant3->GetPath());
	}   
     }   
  }
  if (Mode()==kWithG3){
    g3invol(x,isame);
  }
  if (Mode()==kWithRoot){
    if (gGeoManager->IsSameLocation(x[0], x[1], x[2])) isame = 1;
    else isame = 0;
  }
#ifdef STATISTICS
   statsnext=fgGctrack->snext;
   statsafety=fgGctrack->safety;
   stattree->Fill();
   count_ginvol++;
#endif
}


//______________________________________________________________________
void StTGeant3::Bgtmedi(Float_t *x, Int_t &numed)
{
//count_gtmedi++;
#ifdef STATISTICS
   statcode = 2;
   for (int j=0;j<6;j++) if (j <3) oldvect[j] = x[j]; else oldvect[j]=0;
   oldsafety = fgGctrack->safety;
   oldstep   = fgGctrack->step;
#endif
   // printf("GTMEDI %i:\n", count_gtmedi);
  if (Mode()==kWithBoth){
     static Int_t count_berr = 0;
     static Int_t count_errors = 0;
     g3tmedi(x,numed);

     fgCurrentNode = gGeoManager->FindNode(x[0],x[1],x[2]);
     if (!fgCurrentNode) {numed=0; return;};
     fgGcvolu->nlevel = 1 + gGeoManager->GetLevel();
     gGeoManager->GetBranchNames(fgGcvolu->names);
     gGeoManager->GetBranchNumbers(fgGcvolu->number,fgGcvolu->lvolum);
     TGeoVolume *vol = fgCurrentNode->GetVolume();
     Int_t rmed=0;
     if (vol) {
	TGeoMedium *medium = vol->GetMedium();
	if (medium) rmed = medium->GetId();
     } else {
	printf("found case with vol=0 in gtmedi\n");
     }
     char rpath[512];
     sprintf(rpath, "%s", gGeoManager->GetPath());
     const char *gpath = fgGeant3->GetPath();
     Bool_t rdeeper = (strstr(rpath, gpath))?kTRUE:kFALSE;
     Bool_t gdeeper = (strstr(gpath, rpath))?kTRUE:kFALSE;
     if (strcmp(gpath, rpath) && strlen(rpath)>1) {
	// paths are different
	//---> see if crossing first boundary forward-backward leads to G3 path
	Double_t dir[3];
	Bool_t found = kFALSE, foundp=kFALSE, foundm=kFALSE;
	Double_t berrplus=1E4, berrminus=1E4, berr; 
	// save modeller state
	gGeoManager->PushPoint(); 
	memcpy(dir, gGeoManager->GetCurrentDirection(), 3*sizeof(Double_t));
	//--->Find next boundary forward
	gGeoManager->FindNextBoundary();
	berr = gGeoManager->GetStep();
	gGeoManager->Step();
	if (!strcmp(gpath, gGeoManager->GetPath())) {
           // G3 boundary forward at berr
		 berrplus = berr;
		 foundp = kTRUE;
	}
	//--->Find backward the closest boundary
	gGeoManager->PopPoint();
	gGeoManager->PushPoint();
	gGeoManager->SetCurrentDirection(-dir[0], -dir[1], -dir[2]);
	gGeoManager->FindNextBoundary();
	berr = gGeoManager->GetStep();
	gGeoManager->Step();
	if (!strcmp(gpath, gGeoManager->GetPath())) {
           // G3 boundary backward at berr
		 berrminus = berr;
		 foundm = kTRUE;
	}
	gGeoManager->PopPoint();
	gGeoManager->SetCurrentDirection(-dir[0], -dir[1], -dir[2]);
	found = (foundm || foundp)?kTRUE:kFALSE;
	berr = TMath::Min(berrplus, berrminus);
	if (found && berr<1e-2) { // cut at 100 microns
           count_berr++;
           if (berr==berrplus) {
              printf("%6d LOCATE OK ibdr=%d (%g, %g, %g) -> G3 boundary forward at %g\n",
	                   count_gtmedi, count_berr, x[0],x[1],x[2], berr);
           } else {		
  	          printf("%6d LOCATE OK ibdr=%d (%g, %g, %g) -> G3 boundary backward at %g\n",
	                   count_gtmedi, count_berr, x[0],x[1],x[2], berr);
		 }
		 printf("         gpath=%s\n", gpath);
		 printf("         rpath=%s\n", rpath);
		 return;
	}
	if (found && berr>=1E-2) {
           printf("       ---> G3 boundary found at %g -> check following errors of %d\n", berr, count_gtmedi);
	}   	 	
	//---> check if current G3 path is known by TGeo
	gGeoManager->PushPoint();
	if (gGeoManager->cd(gpath)) {
           Double_t pt[3];
           gGeoManager->MasterToLocal(gGeoManager->GetCurrentPoint(), pt);
           if (gGeoManager->GetCurrentNode()->GetVolume()->GetShape()->Contains(pt)) {
              // Both G3 and TGeo paths contain current point
              //---> check if current node is MANY
              if (gGeoManager->IsCurrentOverlapping()) {
        	 printf("%6d LOCATE OK (%g, %g, %g)->in overlapping region of MANY\n",count_gtmedi, x[0],x[1],x[2]);
        	 printf("         gpath: %s\n", gpath);
        	 printf("         rpath: %s\n", rpath); 
              } else {
        	 count_errors++;
        	 if (rdeeper || gdeeper) {
                    if (rdeeper) printf("%6d LOCATE ERR=%d (%g, %g, %g) -> ROOT path deeper than G3\n", 
                                	 count_gtmedi, count_errors, x[0],x[1],x[2]);
                    else printf("%6d LOCATE ERR=%d (%g, %g, %g) -> G3 path deeper than ROOT\n", 
                                	 count_gtmedi, count_errors, x[0],x[1],x[2]); 
                    printf("         gpath: %s\n", gpath);
                    printf("         rpath: %s\n", rpath); 
                    gGeoManager->PopPoint();
                    return;
        	 }
        	 printf("%6d LOCATE ERR=%d (%g, %g, %g)->both ONLY paths valid !!!\n", count_gtmedi,count_errors,x[0],x[1],x[2]);
        	 printf("         gpath: %s\n", gpath);
        	 printf("         rpath: %s\n", rpath);
              }          
              gGeoManager->PopPoint();
              return;
           }
	} else {
           count_errors++;
           printf("%6d LOCATE ERR!!!=%d (%g,%g,%g)-> ERROR in TGeo: G3 path not known!!!!\n", count_gtmedi, count_errors,x[0],x[1],x[2]);
           printf("         gpath: %s\n", gpath);
           gGeoManager->PopPoint();
           return;
	}
	gGeoManager->PopPoint();   
	count_errors++;
	printf("%6d LOCATE ERR=%d (%g, %g, %g)->DIFFERENCE in paths: \n", count_gtmedi, count_errors, x[0],x[1],x[2]); 
	printf("         gpath=%s\n",gpath);
	printf("         rpath=%s\n",rpath);
     }   
}
  if (Mode()==kWithG3){
   g3tmedi(x,numed);
   // printf("   imed=%i\n", numed);
  }
  if (Mode()==kWithRoot){
  {
     //TGeoNode *old = fgCurrentNode;
     gcchan->lsamvl = kTRUE;
     fgCurrentNode = gGeoManager->FindNode(x[0],x[1],x[2]);
     if (gGeoManager->IsOutside()) {
  //printf("found numed=0\n");
	numed=0; 
  #ifdef STATISTICS
	goto stats;
  #else
	return;
  #endif
     }
     fgGcvolu->nlevel = 1 + gGeoManager->GetLevel();
     gGeoManager->GetBranchNames(fgGcvolu->names);
     gGeoManager->GetBranchNumbers(fgGcvolu->number,fgGcvolu->lvolum);
     TGeoVolume *vol = fgCurrentNode->GetVolume();
     //if(fgCurrentNode != old) gcchan->lsamvl = kFALSE;   
     //if (gcchan->lsamvl != gGeoManager->IsSameLocation()) {
     //   printf("lsamvl=%d, issameloc=%d\n",gcchan->lsamvl,gGeoManager->IsSameLocation());
     //}
     gcchan->lsamvl = gGeoManager->IsSameLocation();   
     if (vol) {
	TGeoMedium *medium = vol->GetMedium();
	if (medium) numed = medium->GetId();
     } else {
	printf("found case with vol=0 in gtmedi\n");
     }
  //printf("found3 numed=%d\n",numed);
     // printf("   imed=%i\n", numed);   
  }
  }
  #ifdef STATISTICS
  stats:
     statsnext=fgGctrack->snext;
     statsafety=fgGctrack->safety;
     stattree->Fill();
     count_gtmedi++;
  #endif
  }


//______________________________________________________________________
void StTGeant3::Bgmedia(Float_t *x, Int_t &numed, Int_t &check)
{
//count_gmedia++;
#ifdef STATISTICS
   statcode = 1;
   for (int j=0;j<6;j++) if (j <3) oldvect[j] = x[j]; else oldvect[j]=0;
   oldsafety = fgGctrack->safety;
   oldstep   = fgGctrack->step;
#endif
   // printf("GMEDIA %i:\n", count_gmedia);
  if (Mode()==kWithBoth){
     g3media(x,numed,check);

     fgCurrentNode = gGeoManager->FindNode(x[0],x[1],x[2]);
     if (!fgCurrentNode) {numed=0; return;};
     fgGcvolu->nlevel = 1 + gGeoManager->GetLevel();
     gGeoManager->GetBranchNames(fgGcvolu->names);
     gGeoManager->GetBranchNumbers(fgGcvolu->number,fgGcvolu->lvolum);
     TGeoVolume *vol = fgCurrentNode->GetVolume();
     Int_t rmed = 0;
     if (vol) {
	TGeoMedium *medium = vol->GetMedium();
	if (medium) rmed = medium->GetId();
     } else {
	printf("found case with vol=0 in gmedia\n");
     }
     if (rmed != numed) {
	const char *path = gGeoManager->GetPath();
	printf("      GMEDIA ERR : (%g, %g,%g), numed=%d, rmed=%d, path=%s\n",x[0],x[1],x[2],numed,rmed,path);
     }
  }
  if (Mode()==kWithG3){
     g3media(x,numed,check);
     // printf("   imed=%i\n", numed);
  }
  if (Mode()==kWithRoot){
  {
   fgCurrentNode = gGeoManager->FindNode(x[0],x[1],x[2]);
   if (gGeoManager->IsOutside()) {
      numed=0; 
#ifdef STATISTICS
      goto stats;
#else
      return;
#endif
   }
   fgGcvolu->nlevel = 1 + gGeoManager->GetLevel();
   gGeoManager->GetBranchNames(fgGcvolu->names);
   gGeoManager->GetBranchNumbers(fgGcvolu->number,fgGcvolu->lvolum);
   TGeoVolume *vol = fgCurrentNode->GetVolume();
   if (vol) {
      TGeoMedium *medium = vol->GetMedium();
      if (medium) numed = medium->GetId();
   } else {
      printf("found case with vol=0 in gmedia\n");
   }
   // printf("   imed=%i\n", numed);
  }
}
#ifdef STATISTICS
stats:
   statsnext=fgGctrack->snext;
   statsafety=fgGctrack->safety;
   stattree->Fill();
   count_gmedia++;
#endif
}

//______________________________________________________________________
void StTGeant3::Bgtmany(Int_t &level1)
{
  if (Mode()&kWithG3){
   g3tmany(level1); //not necessary with TGeo
  }
}

//______________________________________________________________________
void StTGeant3::Bgtonly(Int_t &isOnly)
{
   //with Geant3, return gonly(nlevel);
  if (Mode()==kWithBoth){
    isOnly = (Int_t)fgGcvolu->gonly[fgGcvolu->nlevel-1];
    //with TGeo
    Int_t kOnly;
    if (gGeoManager->IsCurrentOverlapping()) kOnly = 0;
    else kOnly = 1;
    if (isOnly != kOnly) {
       printf("kOnly != isOnly\n");
    }
  }
  if (Mode()==kWithG3){
     isOnly = (Int_t)fgGcvolu->gonly[fgGcvolu->nlevel-1];
  }
  if (Mode()==kWithRoot){
//   if (gGeoManager->IsCurrentOverlapping()) isOnly = 0;
//   else isOnly = 1;
   // With TGeo, G3 is seeing a ONLY geometry
   isOnly = 1;
  }
}

//_____________________________________________________________________________
void StTGeant3::Bglvolu(Int_t &nlev, Int_t *lnam,Int_t *lnum, Int_t &ier) 
{ 
  //
  //  nlev   number of levels deap into the volume tree
  //         size of the arrays lnam and lnum
  //  lnam   an integer array whos 4 bytes contain the askii code for the
  //         volume names
  //  lnum   an integer array containing the copy numbers for that specific
  //         volume
  //
  //  This routine fills the volume parameters in common /fgGcvolu/ for a
  //  physical tree, specified by the list lnam and lnum of volume names
  //  and numbers, and for all its ascendants up to level 1. This routine
  //  is optimised and does not re-compute the part of the history already
  //  available in GCVOLU. This means that if it is used in user programs
  //  outside the usual framework of the tracking, the user has to initialize
  //  to zero NLEVEL in the common GCVOLU. It return 0 if there were no
  //  problems in make the call.
  //
// printf("glvolu called\n");
  if (Mode()&kWithG3){
    g3lvolu(nlev, lnam, lnum, ier); 
  }

  if (Mode()==kWithBoth){
    gGeoManager->cd(fgGeant3->GetPath());
  }
 }


//______________________________________________________________________
void StTGeant3::Bgtnext()
{
//   count_gtnext++;
#ifdef STATISTICS
   statcode = 3;
   for (int j=0;j<6;j++) oldvect[j] = fgGctrack->vect[j];
   oldsafety = fgGctrack->safety;
   oldstep   = fgGctrack->step;
#endif
  if (Mode()==kWithBoth){
     static Int_t count_virtual = 0;
     static Int_t count_errors = 0;
     Double_t step = fgGctrack->step;
     Float_t *x = fgGctrack->vect;

     g3tnext();


     gGeoManager->SetCurrentPoint(x[0],x[1],x[2]);
     gGeoManager->SetCurrentDirection(x[3],x[4],x[5]);
     Double_t gsnext  = fgGctrack->snext;
     Double_t gsafety = fgGctrack->safety;
     gGeoManager->FindNextBoundary(step);
     Double_t rsnext  = gGeoManager->GetStep();
     Double_t rsafety = gGeoManager->GetSafeDistance();
     const char *rpath = gGeoManager->GetPath();
     const char *gpath = fgGeant3->GetPath();
     Bool_t samepath = (strcmp(gpath,rpath) && strlen(gpath)>1)?kFALSE:kTRUE;

     // CHECK :
     // - G3 step was due to geometry
     // - boundary error > 1e-2
     // - G3 safety > 1e-10 (otherwise G3 does not check distance to get outside current volume
     // - both G3 and TGeo steps were computed starting from the same path

     if (gsnext != step && TMath::Abs(rsnext-gsnext)>1e-2 && gsafety>1e-10 && samepath) {
	TGeoShape *shape = gGeoManager->GetCurrentVolume()->GetShape();
	if (shape->TestBit(TGeoShape::kGeoPcon) && (gsnext<rsnext)) {
           count_virtual++;
           printf("%6d SNEXT OK ->virtual boundary of pcon/pgon %s crossed\n", count_virtual, gGeoManager->GetCurrentVolume()->GetName());
           if (gDebug>0) {
              printf("         igsnext=%d, gsnext=%g, gsafety=%g, rsnext=%g, rsafety=%g, step=%g, x,y,z=(%g,%g,%g,%g,%g,%g)\n" ,fgGctrack->ignext,gsnext,gsafety,rsnext,rsafety,step,x[0],x[1],x[2],x[3],x[4],x[5]); 
              printf("         rpath=%s\n", rpath);
           }   
	} else {    
           count_errors++;
           printf("%6d SNEXT ERR=%d (%g,%g,%g,%g,%g,%g)->DIFFERENT DISTANCE\n", count_gtnext,count_errors,x[0],x[1],x[2],x[3],x[4],x[5]);
           printf("         rpath=%s :\n", rpath);
           printf("         ignext=%d, gsnext=%g, gsafety=%g, rsnext=%g, rsafety=%g, step=%g, \n" ,fgGctrack->ignext,gsnext,gsafety,rsnext,rsafety,step);
	}   
     }   
  }
  if (Mode()==kWithG3){
     g3tnext();
  #ifdef STATISTICS
	goto stats;
  #else
	return;
  #endif
  }
    if (Mode()==kWithRoot)
  {
     Float_t *x = fgGctrack->vect;   
     Double_t step = fgGctrack->step;
     gGeoManager->SetCurrentPoint(x[0],x[1],x[2]);
  //fgGeant3->Gpcxyz();
     if (!gGeoManager->IsSameLocation(x[0],x[1],x[2])) {
  //static int acount=0;
  //acount++;
  //printf("acount=%5d, x=%g, %g, %g, sleng=%g\n",acount,x[0],x[1],x[2],fgGctrack->sleng);
  //fgGeant3->Gpcxyz();
	fgGctrack->safety = 0.;
	fgGctrack->snext = 0.;
	fgGctrack->ignext = 1;
  #ifdef STATISTICS
	goto stats;
  #else
	return;
  #endif
     }
     gGeoManager->SetCurrentDirection(x[3],x[4],x[5]);
     gGeoManager->SetLastPoint(x[0],x[1],x[2]);
     gGeoManager->FindNextBoundary(step);
     fgGctrack->safety = gGeoManager->GetSafeDistance();
     Double_t snext  = gGeoManager->GetStep();
  //   printf("%7d SNEXT (%f,%f,%f,%f,%f,%f) step=%g snext=%g safety=%g\n", count_gtnext, x[0], x[1], x[2], x[3],x[4],x[5],step, snext,fgGctrack->safety);
  //   if (snext<0) exit(1);   
     if (snext < step) {
	fgGctrack->snext  = snext;
	fgGctrack->ignext = 1;
  #ifdef STATISTICS
	goto stats;
  #else
	return;
  #endif
     }
     fgGctrack->ignext = 0;
     fgGctrack->snext = fgGctrack->step;
  }
#ifdef STATISTICS
stats:
   statsnext=fgGctrack->snext;
   statsafety=fgGctrack->safety;
   stattree->Fill();
   count_gtnext++;
#endif
}


//______________________________________________________________________
void StTGeant3::Bggperp(Float_t *x, Float_t *norm, Int_t &ierr)
{
// Computes the normal to the next crossed surface, assuming that 
// FindNextBoundary() was already called.
  if (Mode()==kWithG3){
   g3gperp(x,norm,ierr);
//   printf("norm(%f, %f, %f)\n",norm[0],norm[1],norm[2]);
  }

  if (Mode()==kWithRoot){
   ierr = 0;
//   Double_t *dir = gGeoManager->GetCurrentDirection();
//   gGeoManager->SetCurrentPoint(x[0],x[1],x[2]);
//   Double_t *pt = gGeoManager->GetCurrentPoint();
   Double_t *dblnorm = gGeoManager->FindNormalFast();
   if (!dblnorm) {
      ierr = 1;
      return;
   }   
   norm[0] = -dblnorm[0];
   norm[1] = -dblnorm[1];
   norm[2] = -dblnorm[2];
//   printf("norm(%f, %f, %f)\n",norm[0],norm[1],norm[2]);
  }
}
//______________________________________________________________________
void StTGeant3::Bgrndm(Float_t *r, const Int_t &n)
{  
   gRandom->RndmArray(n,r);
}

//______________________________________________________________________
void StTGeant3::Bgrndmq(Int_t &is1, Int_t &is2, const Int_t &,const char*, int )
{is1=0; is2=0;
 /*printf("Dummy grndmq called\n");*/
}
