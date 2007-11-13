* $Id: itspgeo.g,v 1.2 2007/11/13 21:30:51 perev Exp $
*
* $Log: itspgeo.g,v $
* Revision 1.2  2007/11/13 21:30:51  perev
* material ALKAP fixed
*
* Revision 1.1  2006/10/21 17:58:05  potekhin
* Adding a new crucial subsystem, which is the support structure
* for the tracking upgrade configurations.
* ased on the SVT conde design with a few simplifications. On the west, we
* have a mockup cone which is a slimmed-down version of the East one (which will
* stay). Copper cables have been removed for now.
*
*
*
*******************************************************************************
Module ITSPGEO  is the Inner Tracker Support
*-->Based on the SVTTGEO6 code

   Author  Maxim Potekhin
   Created 18 Oct 2006

+cde,AGECOM,GCONST,GCUNIT.
*     
      Content          ITSP,SGRA,STAP,SHMB,SCMY,SCAL
*
      structure SSUP { Version,   CabThk,   HosRmx,
		       WrpMyThk,  WrpAlThk, RodLen,    
                       GrphThk,   Cone1Zmn,
                       Con1IdMn,  Con3IdMn,  
                       Con4IdMn,  Con4IdMx,  Cone3zmx,  Cone4zmx}
*
      Integer        i,j
      Real           cone_thk1,cone_thk2,roffset,RsizeMax,deg,rad,c0
      Real	     cone_len,cone_sin,cone_cos,rmin,rmax,zmin,zmax,angle
      Real           xpos,ypos,zpos,clearance,rin,rou,elethk,tabLen,radmax
      Real           ang,wafpckLen,dthk,radtilt
*******************************************************************************
* Historical note from the recent version of the SVTT code (2006).
* which is useful to keep in mind. We have removed the extra version
* of this structure (obsoileted anyway) --maxim--
*------------------------------------------------------------
* This is a copy of the above (base) version with one difference which is
* the thickness of the copper layer which models the cables attached to
* the cones. Based on the new measured data from Dave Lynn, we normalize the
* effective thickness of this approximation, such that it matches the actual
* cable weight.It's a factor of 4.35 compared to the previous number. That
* previous number's origin is now impossible to determine.
* NOTE: historically in the code this number, CabThk, is divided by 2.
* I will keep it the same way to more easily compare the old with the new.
*
   Fill SSUP ! Support structures
      Version   = 1          ! geometry version
      CabThk    = 0.21       ! thickness of layer of cables on support cone
      HosRmx    = 0.95       ! outer radius of water hoses on support cone
      WrpMyThk  = 0.10       ! thickness of mylar wrap around cone (guess)
      WrpAlThk  = 0.01       ! thickness of Al on mylar wrap (guess)
      RodLen    = 110.8      ! Length of support rods
      GrphThk   = 0.16	     ! support cone thickness
      Cone1zmn  = 52.23      ! Cone z min (parts 1,2,3,4 in increasing z)
      Con1IdMn  = 15.67      ! Minimum id of cone 1 
      Con3IdMn  = 21.67      ! Minimum id of cone 3 (TBD)
      Con4IdMn  = 37.4       ! Minimum id of cone 4 (TBD)
      Con4IdMx  = 37.4       ! Maximum id of cone 4 (TBD)
      Cone3zmx  = 150.0      ! Maximum z of cone 3 (TBD)
      Cone4zmx  = 229.36     ! Maximum z of cone 4 (TBD)
   EndFill
   Fill SSUP ! Support structures
      Version   = 2          ! geometry version
      CabThk    = 0.21       ! thickness of layer of cables on support cone
      HosRmx    = 0.95       ! outer radius of water hoses on support cone
      WrpMyThk  = 0.10       ! thickness of mylar wrap around cone (guess)
      WrpAlThk  = 0.01       ! thickness of Al on mylar wrap (guess)
      GrphThk   = 0.16	     ! support cone thickness
      Cone1zmn  = 52.23      ! Cone z min (parts 1,2,3,4 in increasing z)
      Con1IdMn  = 7.835      ! Minimum id of cone 1 
      Con3IdMn  = 10.835     ! Minimum id of cone 3 (TBD)
      Con4IdMn  = 18.7       ! Minimum id of cone 4 (TBD)
      Con4IdMx  = 18.7       ! Maximum id of cone 4 (TBD)
      Cone3zmx  = 150.0      ! Maximum z of cone 3 (TBD)
      Cone4zmx  = 229.36     ! Maximum z of cone 4 (TBD)
   EndFill
*------------------------------  SHALL WE BEGIN? -------------------------------------------

* introduce common materials here
*
*     G10 is about 60% SiO2 and 40% epoxy (stolen from ftpcgeo.g)
        Component Si  A=28.08  Z=14   W=0.6*1*28./60.
        Component O   A=16     Z=8    W=0.6*2*16./60.
        Component C   A=12     Z=6    W=0.4*8*12./174.
        Component H   A=1      Z=1    W=0.4*14*1./174.
        Component O   A=16     Z=8    W=0.4*4*16./174.
      Mixture   G10   Dens=1.7

* use aluminized mylar mixture instead of kapton
        Component C5  A=12    Z=6  W=5
        Component H4  A=1     Z=1  W=4
        Component O2  A=16    Z=8  W=2
        Component Al  A=27    Z=13 W=0.2302
      Mixture  ALKAP  Dens=1.432
*
      USE SSUP version=1
      write(*,*) '********* ITSP Support Cone Cable layer thickness: ', SSUP_CabThk
*     EAST
      Create and Position ITSP in Cave ThetaZ=180 

      USE SSUP version=2
      write(*,*) '********* ITSP Support Cone Cable layer thickness: ', SSUP_CabThk
*     WEST
      Create and Position ITSP in Cave kOnly='ONLY'

*
*******************************************************************************
*
Block ITSP is the Inner Tracker SuPport cone mother volume
* There is a graphite-epoxy cone SGRA (guess = carbon) 
* On top of that there are cones of plastic and copper for Twin-Ax STAP
* On top of that there are 10 water hoses SHOS, with water inside SHSW
* On top of that there is a layer of mylar SCMY
* On top of that there is a layer of aluminum SCAL
*
* The parts of the cones are labeled 1 through 4 as you go away from the 
* interaction point 
*
      cone_thk1=ssup_GrphThk
      cone_thk2=cone_thk1+ssup_CabThk+2*ssup_HosRmx
      cone_thk2=cone_thk2+ssup_WrpMyThk+ssup_WrpAlThk

      Material  Air
      attribute ITSP    seen=1 colo=1
      SHAPE     PCON    Phi1=0  Dphi=360  Nz=7,
      zi ={ssup_Cone1zmn,
           ssup_Rodlen/2, 
           ssup_Rodlen/2, 
           ssup_Rodlen/2+ssup_GrphThk, 
           ssup_Rodlen/2+ssup_GrphThk, 
	   ssup_Cone3zmx, 
	   ssup_Cone4zmx},
      Rmx={ssup_Con1IdMn+cone_thk1,
           ssup_Con1IdMn+cone_thk1, 
           ssup_Con3IdMn+cone_thk1,
           ssup_Con3IdMn+cone_thk1,
           ssup_Con3IdMn+cone_thk2,
	   ssup_Con4IdMn+cone_thk2,
	   ssup_Con4IdMx+cone_thk2},
      Rmn={ssup_Con1IdMn,
           ssup_Con1IdMn, 
           ssup_Con1IdMn,
           ssup_Con1IdMn,
           ssup_Con3IdMn, 
	   ssup_Con4IdMn,
	   ssup_Con4IdMx}
*
      Create    SGRA  " graphite/epoxy support cone (lowest layer)"
      Position  SGRA
      Create    STAP  " twinax cable approximation, plastic"
      Position  STAP
      Create    SCMY  " support cone mylar wrap"
      Position  SCMY
      Create    SCAL  " aluminization on support cone mylar wrap (top layer)"
      Position  SCAL
endblock
*
*------------------------------------------------------------------------------
* -keep
Block SGRA is the graphite/epoxy support cone
      Material   Carbon
      Attribute SGRA   Seen=1   Colo=6
      SHAPE     PCON   Phi1=0   Dphi=360   Nz=7,
      zi ={ssup_Rodlen/2, 
           ssup_Rodlen/2, 
           ssup_Rodlen/2+ssup_GrphThk, 
           ssup_Rodlen/2+ssup_GrphThk, 
	   ssup_Cone3zmx, 
	   ssup_Cone4zmx},
      Rmx={ssup_Con1IdMn+ssup_GrphThk, 
           ssup_Con3IdMn+ssup_GrphThk,
           ssup_Con3IdMn+ssup_GrphThk,
           ssup_Con3IdMn+ssup_GrphThk,
	   ssup_Con4IdMn+ssup_GrphThk,
	   ssup_Con4IdMx+ssup_GrphThk},
      Rmn={ssup_Con1IdMn, 
           ssup_Con1IdMn,
           ssup_Con1IdMn,
           ssup_Con3IdMn, 
	   ssup_Con4IdMn,
	   ssup_Con4IdMx}
endblock
*
*------------------------------------------------------------------------------
* -keep
Block STAP is the plastic part of the twin-ax cable layer (guess polyethylene)
      roffset=ssup_GrphThk
      Component C      A=12  Z=6  W=1
      Component H2     A=1   Z=1  W=2
      Mixture   CH2    Dens=0.935
      Attribute STAP   Seen=1   Colo=3
      SHAPE     PCON   Phi1=0   Dphi=360  Nz=3,
      zi ={ssup_Rodlen/2+ssup_GrphThk, 
	   ssup_Cone3zmx, 
	   ssup_Cone4zmx},
      Rmx={ssup_Con3IdMn+roffset+ssup_CabThk/2,
	   ssup_Con4IdMn+roffset+ssup_CabThk/2,
	   ssup_Con4IdMx+roffset+ssup_CabThk/2},
      Rmn={ssup_Con3IdMn+roffset, 
	   ssup_Con4IdMn+roffset,
	   ssup_Con4IdMx+roffset}
endblock
*
*------------------------------------------------------------------------------
* -keep
Block SCMY is a mylar wrap around the support cone
      roffset=ssup_GrphThk+ssup_CabThk+2.0*ssup_HosRmx
      Component C5     A=12  Z=6  W=5
      Component H4     A=1   Z=1  W=4
      Component O2     A=16  Z=8  W=2
      Mixture   Mylar  Dens=1.39
      Attribute SCMY   Seen=1   Colo=3
      SHAPE     PCON   Phi1=0   Dphi=360  Nz=3,
      zi ={ssup_Rodlen/2+ssup_GrphThk, 
	   ssup_Cone3zmx, 
	   ssup_Cone4zmx},
      Rmx={ssup_Con3IdMn+roffset+ssup_WrpMyThk,
	   ssup_Con4IdMn+roffset+ssup_WrpMyThk,
	   ssup_Con4IdMx+roffset+ssup_WrpMyThk},
      Rmn={ssup_Con3IdMn+roffset, 
	   ssup_Con4IdMn+roffset,
	   ssup_Con4IdMx+roffset}
endblock
*
*------------------------------------------------------------------------------
* -keep
Block SCAL is the aluminization on the mylar wrap around the support cone
*     note combine with mylar a la Pavel's example
      roffset=ssup_GrphThk+ssup_CabThk+2.0*ssup_HosRmx+ssup_WrpMyThk
      Material  Aluminium
      Attribute SCAL   Seen=1   Colo=2
      SHAPE     PCON   Phi1=0   Dphi=360  Nz=3,
      zi ={ssup_Rodlen/2+ssup_GrphThk, 
	   ssup_Cone3zmx, 
	   ssup_Cone4zmx},
      Rmx={ssup_Con3IdMn+roffset+ssup_WrpAlThk,
	   ssup_Con4IdMn+roffset+ssup_WrpAlThk,
	   ssup_Con4IdMx+roffset+ssup_WrpAlThk},
      Rmn={ssup_Con3IdMn+roffset, 
	   ssup_Con4IdMn+roffset,
	   ssup_Con4IdMx+roffset}
endblock
*
*------------------------------------------------------------------------------
*
      End

