* $Id: gstar_part.g,v 1.15 2007/03/02 21:13:16 potekhin Exp $
*
* $Log: gstar_part.g,v $
* Revision 1.15  2007/03/02 21:13:16  potekhin
* Corrected wrong masses for B-mesons, and adding the
* omitted comment about moving the stranglet definitions
* into a separate file in the same directory (stranglet.txt).
*
* Revision 1.14  2007/03/02 19:41:57  potekhin
* Added D0bar, Dstar+-0 and 0 bar, B+-0 and 0bar
*
* Revision 1.13  2007/02/06 16:48:21  potekhin
* Bring the particle definitions up to date,
* with new Upsilon levels and strange lambda plus
*
* Revision 1.12  2004/12/20 18:04:22  potekhin
* Corrected a typo (missing comma)
*
* Revision 1.11  2004/12/20 17:55:31  potekhin
* Requested by Sevil and heavily used in embedding,
* sigma(1385) family has been added to the particle list
*
* Revision 1.10  2004/06/06 14:08:35  fisyak
* make LASERINO charged
*
* Revision 1.9  2003/12/19 17:51:11  potekhin
* Added the CVS tag, and repeating the comment from the previous check-in:
* Including the straglet nomenclature developed by Ron, with a few corresctions.
* The PDG codes assigned are essentialy our declared GEANT codes for
* simplicity.
*
***********************************************************************
*
                      Subroutine gstar_PART
*
* Remark: the particle ID numbers used by the original STAR Geant v3.15 
* can no longer be used in v3.21 because of a significant expansion of 
* idenitfied particles in the core Geant (CONS300-1). 
* Gstar uses particles codes up to 50 as defined by GEANT standards
* All remaining particles (including quarks) are unknown to GEANT, 
* BUT nevertheless, they all ARE loaded in the program KINE bank
* as geantinos with IPART=1000000+Icode
* You can redefine their properties BEFORE tracking in aGuTRAC routine
* You can decay particles provided without decay modes in aGuDCAY routine
*
* Branching ratios taken from Particle Data Book of July '94. pmj 15/2/95
***********************************************************************
+CDE,agecom,gconst.
   print *,' *             omega,phi,rho(0,+,-) and laserino defined. *'
   print *,' *             Read program comments for more information *'
*
* --------------------------------------------------------------------------
*
* redefinition of pre-coded D's with the desired branchings
*
* (K-)+2(pi+) decay ;  Alt.  K0_S + pi+   pdg=411  bratio= { 1, }       mode= { 1608, }
  Particle D_plus    code=35  TrkTyp=4 mass=1.869  charge=1 tlife=1.057e-12,
                     pdg=411  bratio= { 1, }       mode= { 120808, }

* (K+)+2(pi-) decay
  Particle D_minus   code=36  TrkTyp=4 mass=1.869  charge=1 tlife=1.057e-12,
                     pdg=-411  bratio= { 1, }       mode= { 110909, }

 
* (K-)+(pi+) decay
  Particle D0        code=37  TrkTyp=3 mass=1.865  charge=0 tlife=0.415e-12,
                     pdg=421  bratio= { 1, }       mode= { 1208, }

* (K+)+(pi-) decay
  Particle D0_bar    code=38  TrkTyp=3 mass=1.865  charge=0 tlife=0.415e-12,
                     pdg=-421  bratio= { 1, }      mode= { 1109, }

************************************************************************************ 
* D0 and pi+, D+ and pi0
  Particle D_star_plus   code=60  TrkTyp=4 mass=2.010  charge=1  tlife=6.86e-21,
                     pdg=413  bratio= { 0.69, 0.31}       mode= { 3708, 3507 }

* decay into D0 and pi-, D- and pi0
*
  Particle D_star_minus  code=61  TrkTyp=4 mass=2.010  charge=-1 tlife=6.86e-21,
                     pdg=-413 bratio= { 0.69, 0.31}       mode= { 3709, 3607 }

* decay into D0 and pi0, D0 and gamma
  Particle D_star_0       code=62  TrkTyp=4 mass=2.007  charge=0 tlife=3.13e-22,
                     pdg=423  bratio= { 0.62, 0.38}       mode= { 3507, 3501}

* D0_bar and pi0, D0_bar and gamma
  Particle D_star_0_bar   code=63  TrkTyp=4 mass=2.007  charge=0 tlife=3.13e-22,
                     pdg=-423 bratio= { 0.62, 0.38}       mode= { 3807, 3801}
************************************************************************************ 

* decay into D_star_0_bar, positron and neutrino
  Particle B_plus    code=70         TrkTyp=4 mass=5.279  charge=1 tlife=1.671e-12,
                     pdg=521  bratio= { 1, }       mode= { 630204, }

* decay into D_star_0,     electron and neutrino
  Particle B_minus   code=71         TrkTyp=4 mass=5.279 charge=-1 tlife=1.671e-12,
                     pdg=-521 bratio= { 1, }       mode= { 620304, }

* decay into D_star_minus, positron and neutrino
  Particle B0        code=72         TrkTyp=4 mass=5.279  charge=0 tlife=1.536e-12,
                     pdg=511  bratio= { 1, }       mode= { 610204, }

* decay into D_star_plus,  electron and neutrino
  Particle B0_bar    code=73         TrkTyp=4 mass=5.279  charge=0 tlife=1.536e-12,
                     pdg=-511  bratio= { 1, }      mode= { 600304, }

************************************************************************************ 
  Particle omega     code=150 TrkTyp=3 mass=.782   charge=0  tlife=7.79E-23,
                     pdg=223  bratio  = { .888, .085, .021 },  
                              mode    = { 70809, 107,  809 }

* group rho-pi and pi-pi-pi together (in practice indistinguishable)
  Particle phi       code=151 TrkTyp=3 mass=1.0194 charge=0  tlife=1.482e-22,
                     pdg=333  bratio = { .491, .343, .154, .0128, .00131 },
                              mode   = { 1112, 1016, 70809, 1701,   701  }
 
  Particle rho       code=152 TrkTyp=3 mass=.770   charge=0  tlife=4.35E-24,
                     pdg=113  bratio= { 1, }       mode= { 809, }
 
  Particle rho_plus  code=153 TrkTyp=4 mass=.767   charge=1  tlife=4.35E-24,
                     pdg=213  bratio= { 1, }       mode= { 807, }
 
  Particle rho_minus code=154 TrkTyp=4 mass=.767   charge=-1 tlife=4.35E-24,
                     pdg=-213 bratio= { 1, }       mode= { 907, }
 
  Particle K0        code=155 TrkTyp=4 mass=.4977  charge=0  tlife= 1.e-24,
                     pdg=311  bratio= { .5, .5}    mode= { 16, 10 }

  Particle anti_K0   code=156 TrkTyp=4 mass=.4977  charge=0  tlife= 1.e-24,
                     pdg=-311  bratio= { .5, .5}    mode= { 16, 10 }
************************************************************************************ 
* Heavy flavor embedding studies -- July 2003, M.Potekhin
  Particle Jpsi      code=160 TrkTyp=4 mass=3.096  charge=0  tlife=7.48e-21,
                     pdg=443  bratio= { 1, }       mode= { 203, }
  Particle Upsilon   code=161 TrkTyp=4 mass=9.460   charge=0  tlife=1.254e-20,
                     pdg=553  bratio= { 1, }       mode= { 203, }
  Particle Ups2S     code=162 TrkTyp=4 mass=10.023  charge=0  tlife=1.545e-20,
                     pdg=100553  bratio= { 1, }    mode= { 203, }
  Particle Ups3S     code=163 TrkTyp=4 mass=10.355  charge=0  tlife=2.556e-20,
                     pdg=200553  bratio= { 1, }    mode= { 203, }
************************************************************************************ 

  Particle LASERINO  code=170         TrkTyp=6 mass=0     charge=1 tlife=big

  Particle chasrino  code=171         TrkTyp=6 mass=0     charge=1 tlife=big

************************************************************************************ 
  Particle Lac2      code=207       TrkTyp=3 mass=2.285   charge=1  tlife=0.206e-12,
                     pdg=4122       bratio= { 1, }       mode= { 141208, }


  Particle S1385plus     code=701   TrkTyp=4 mass=1.3828  charge=1   tlife=1.84e-23,
                         pdg=3224   bratio= { 1, }       mode= { 1808, }

  Particle S1385minus    code=702   TrkTyp=4 mass=1.3872  charge=-1  tlife=1.67e-23,
                         pdg=3114   bratio= { 1, }       mode= { 1809, }

  Particle S1385barplus  code=703   TrkTyp=4 mass=1.3872  charge=-1  tlife=1.67e-23,
                         pdg=-3114  bratio= { 1, }       mode= { 2608, }

  Particle S1385barminus code=704   TrkTyp=4 mass=1.3828  charge=1   tlife=1.84e-23,
                         pdg=-3224  bratio= { 1, }       mode= { 2609, }

  Particle Monopole      code=800       TrkTyp=9 mass=1.0    charge=0 tlife=big


 End
*
* --------------------------------------------------------------------------
*
* Subroutine aGuTRACK
*  check here for IPART: if>200, this particle is unknown to GEANT  
*  and needs correct tracking parameters (TrkTyp,mass,charge,tlife) 
*  to be set here.
* end
*
* --------------------------------------------------------------------------
*
      Subroutine aGuDCAY
+CDE,gctrak,gckine,gcking.
*
*  If decay modes are not set, this routine will be called to decay the particle
*
*  What follows is a snippet of code to explore a few variables the
* user might need:

*      real P_PART(4)

*      DO I=1,4
*         P_PART(I)=VECT(I+3)*VECT(7)
*      ENDDO

*      write(*,*) 'ivert, ipart: ', ivert, ipart
*      write(*,*) 'p1, p2, p3: ', P_PART(1), P_PART(2), P_PART(3)

*      NGKINE = NGKINE + 1
*      DO I = 1, 4
*          GKIN(I,NGKINE) = 1.0
*      END DO
*     GKIN(5,NGKINE)   = 2
*     TOFD(NGKINE)     = 0.
* copy over the decay position:
*     DO I = 1, 3
*         GPOS(I,NGKINE) = VECT(I)
*     ENDDO

      end
*
* --------------------------------------------------------------------------
*
 
 
