* $Id: fpdmgeo.g,v 1.3 2002/11/03 02:15:41 nevski Exp $
* $Name:  $
* $Log: fpdmgeo.g,v $
* Revision 1.3  2002/11/03 02:15:41  nevski
* clean-up parameters
*
* Revision 1.2  2002/11/02 19:37:01  nevski
* reduce mother, loop over fpd assembly
*
*
* Revision 1.14  2001/09/13 18:15:13  yqwang
*
* Endcap: West ON, Fpd: East ON, Lgd: East ON
*
* Revision 1.13  2001/08/29 22:44:54  yqwang
* Rotate Top and South Lgd, so their numberings agree with the actual
* labels.
*
* Revision 1.12  2001/08/23 12:15:41  yqwang
* Each (West (1) , East (2)) Endcap (Fpd) has possible
*                            XOffset, YOffset, and ZOffset.
* Each Lgd has its own ZPoz and XDis, YDis.
* Update Elgg_DGap value
*
* Revision 1.11  2001/08/20 21:17:41  yqwang
* South Lgd is tilted, but not the top and bottom
*
* Revision 1.10  2001/04/09 14:20:24  akio
* set default to de, not cherenkov, for fpd
*
* Revision 1.9  2001/04/06 18:12:20  akio
* Include the first version of FPD
*
******************************************************************************
Module FPDMGEO is the Forward Pion Detector Modules GEOmetry
Created   06 jan 2002
Author    Yiqun Wang
*
* Version 4.1, O. Akio                                        23 Jan 01
*               - Include forward pion detectors
* 		- FPDG_ChkvSim to do either cherenkov photon simulation
*                 or just take de above thresholds.
* Version 4.2, Yiqun Wang                                  31 July 01
*               - Update FPD Pb-glass geometry
*
* Version 5.0, Yiqun Wang
* 		- Put FPD (with LGD) in one volume FPDM, as suggested by Pavel Nevski
*
* Version 5.1, Yiqun Wang
*               - Put FPD (with LGD) in FPDM or FPDE, depending on FPDG_Onoff switch
* 
******************************************************************************
+CDE,AGECOM,GCONST,GCUNIT.
*
      Content    FPDM,FPDE,FWOL,FCVO,FMDI,FMSS,FAGA,FTOW,FPSE,FPER,FFLP,FXFP,FSHM,
                 FSSP,FTAR,FGTN,FSCI,FLED,FMOD,FXGT,FXSG,FALP,FHMS,
		 FLGD,FLGT,FWAL,FLGR,FPRB,FPCT,FUMT
*
      Structure  FPDG { Version,Onoff,ChkvSim, Zdis,Zlen,Radius,Rmin }
      Structure  FMCS { Int Type,Zorig,Zend,EtaMin,EtaMax,PhiMin,PhiMax,
			Nsupsec,Nsector,Nsection,Nslices,
			Front,Gten,Plate,PlateS,PlateB,
                        Hub,Rmshift,SMShift,GapPlt,GapCel,GapSMD}
*
      Structure  FMCC { Int Ipart,Zorig,Zend,XOffset,YOffset,ZOffset}
*
      Structure  FETR { Int Type,Etagr,Phigr,Neta,EtaBin(13)}
*
      Structure  FSEC { Isect, FPlmat, Cell, Scint(2), Nlayer }
*
      Structure  FMXG {Version,Sapex,Sbase}
*
      Structure  FXSE {Jsect,Swidth,Aplate}
*
      Structure  FLGG {Int version, Width,Depth,DGap,NPhi,NEta,
	               AlThick,SiRubDz,PhCathDz,PhCathR,MuMetDz,MuMetR}
*
      Structure  FLGC {Int Icopy,Zpos,Rdis,DipAng} 
*
      Structure  FLGM {Density,RadLen,Index,PbContent,CritEne,MoliereR}
*
      Integer    I_section,J_section,Ie,is,isec,i_str,Nstr,Type,ii,jj,Istat/0/,Ipos
*			
      Real       Secwid,Section,center,current,Plate,Gap,Cell,G10,
                 tan_low,tan_upp,Tanf,RBot,Rtop,Deta,etax,
                 dup,dd,d2,d3,rshift,dphi,orgkeep,endkeep,
                 maxcnt,msecwd,mxgten,curr,
		 curcl,EtaTop,EtaBot,zpos,zdis,
		 xleft,xright,yleft,yright,
		 sq2,sq3,rth,tng,len,p,xc,yc,diff,
		 xx,yy,rtot,ztot,fwoldz,alp

    Integer    N
    Parameter (N=12)
* --- Enegry bins
    real E(N)  /1.776e-9, 1.912e-9,  2.072e-9, 2.26e-9, 2.486e-9, 2.702e-9,
                2.825e-9, 2.96e-9, 3.108e-9,  3.271e-9,  3.551e-9, 3.767e-9/

* --- Refraction indexes
    real rindex_PbG(N)    /N*1.67/
    real rindex_SiRub(N)  /N*1.67/
    real rindex_PhCath(N) /N*1.67/
    real rindex_Alm(N)    /N*1.0/
    real rindex_MuMet(N)  /N*1.0/
* ---- Absorbtion lenghts (in cm)
    real absco_PbG(N)     /N*38.0/
    real absco_SiRub(N)   /N*38.0/ 
    real absco_PhCath(N)  /N*0.0001/
    real absco_Alm(N)     /N*0.0001/
    real absco_MuMet(N)   /N*0.0001/
* ---- Detection efficiencies (quantum efficiency for Photo Cathode)
    real effic_PhCath(N)  / N*1.0/

*    real effic_PhCath(N)  / 0.06,  0.08,   0.105,  0.14,   0.155,  0.17,
*                            0.18,  0.2,    0.19,   0.175,  0.135,  0.09/
    real effic_all(N)    /N*0.0/
    EXTERNAL  FFPDSTEP,FPCTSTEP
* 
    Tanf(etax) = tan(2*atan(exp(-etax)))
*
* ----------------------------------------------------------------------------
*
Fill  FPDG                          ! Fpd EndCAp Calorimeter basic data 
      Version  = 5.0                ! Geometry version 
      OnOff    = 2                  ! =0 no, =1 west, =2 east, =3 both for FPDM, fpd and PbG
      ChkvSim  = 1                  ! = 0 de, = 1 Cherenkov simulation for PbG
      Zdis     =  600.0             ! z-distance of center of FPD mother
      Zlen     =  210.0             ! z-length  of FPD mother
      Radius   =  70.0              ! Radius of FPDM tube
      Rmin     =  10.0              ! FPDM inner radius
* ----------------------------------------------------------------------------
Fill  FMCC                          ! details of EM sector position
      ZOrig    =  750               ! Calorimeter sector origin in z in this setup
      ZEnd     =  786.507           ! Calorimeter sector end in z in this setup
      XOffset  =  60.0    	    ! offsets in x: signed according to global coords(60 for near, 20 - far)
      YOffset  =  0.0   	    ! offsets in y: signed according to global coords
      ZOffset  =  0.0   	    ! offsets in z: signed according to global coords
* ----------------------------------------------------------------------------
* ----------------------------------------------------------------------------
Fill  FMCS                          ! Sector of EM Endcap Calorimeter geometry
      Type     =  2                 ! =1 endcap, =2 fpd endcap prototype
      ZOrig    = 273.5              ! Nominal Calorimeter origin in z (ie Focus)
      ZEnd     = 310.007            ! Nominal Calorimeter end in z
      EtaMin   =  1.6317            ! upper feducial eta cut 
      EtaMax   =  2.0               ! lower feducial eta cut
      PhiMin   =  -9                ! Min phi 
      PhiMax   =   9                ! Max phi
      Nsupsec  =    1               ! Number of azimuthal supersectors        
      Nsector  =    3               ! Number of azimutal sectors (Phi granularity)
      Nslices  =    3               ! number of phi slices in supersector
      Nsection =    8               ! Number of readout sections
      Front    = 0.953              ! thickness of the front AL plates
      Gten     = 0.16               ! Fiber routing guides
      Plate    = 0.5                ! Lead radiator thickness
      PlateS   = 0.05               ! Laminated SS plate thickness
      PlateB   = 3.175              ! Back plate thickness SS
      Hub      = 2.5                ! thickness of EndCap hub
      Rmshift  = 1.228              ! radial shift of module
      smshift  = 0.12               ! radial shift of steel support walls
      GapPlt   = 0.3/2              ! HALF of the inter-plate gap in phi
      GapCel   = 0.03/2             ! HALF of the radial inter-cell gap
      GapSMD   = 3.2                ! space for SMD detector
* --------------------------------------------------------------------------
Fill  FETR                          ! Eta and Phi grid values
      Type     = 2                  ! =1 endcap, =2 fpd
      EtaGr    = 1.0536             ! eta_top/eta_bot tower granularity
      PhiGr    = 0.0981747          ! Phi granularity (radians)
      NEta     = 4                  ! Eta granularity
      EtaBin   = {2.0,1.9,1.8,1.7,1.6,1.6,1.6,1.6,1.6,1.6,1.6,1.6,1.6} ! Eta rapidities
*---------------------------------------------------------------------------
Fill FSEC           ! First EM section
      ISect    = 1                           ! Section number	
      Nlayer   = 2                           ! Number of Sci layers along z
      Cell     = 1.328                       ! Cell full width in z
      Scint    = {0.5,0.5}    		     ! Sci layer thickness
*
Fill FSEC           ! Second EM section
      ISect    = 2                           ! Section number
      Nlayer   = 4                           ! Number of Sci layers along z
      Cell     = 1.228                       ! Cell full width in z
      Scint    = {0.4,0.5}     		     ! Sci layer thickness
*
Fill FSEC           ! Third and further EM section
      ISect    = 3                           ! Section
      Nlayer   = 3                           ! Number of Sci layers along z
      Cell     = 1.228                       ! Cell full width in z
      Scint    = {0.4,0.5}      	     ! Sci layer thickness
*----------------------------------------------------------------------------
Fill FMXG           ! EM Endcap SMD basic data
     Version   = 1         		     ! Geometry version
     Sapex     = 0.5       		     ! Scintillator strip apex
     Sbase     = 1.0       		     ! Scintillator strip base
*
Fill FXSE           ! First SMD section
      JSect    = 1                           ! Section number
      Swidth   = 1.6                         ! Section width
      Aplate   = 0.15                        ! Width of Aluminium plate
*
Fill FXSE           ! Second SMD section
      JSect    = 2                           ! Section number	
      Swidth   = 1.6                         ! Section width
      Aplate   = 0.15                        ! Width of Aluminium plate
*----------------------------------------------------------------------------
*----------------------------------------------------------------------------
Fill FLGG                                    ! PbG detector general geometry
      Width    = 3.8			     ! PbG width	
      Depth    = 45.0			     ! PbG depth
      DGap     = 0.40			     ! Gap between PbG
      NPhi     = 4			     ! # of tower in phi (test)
      NEta     = 4                           ! # of tower in eta (test)
      AlThick  = 0.002			     ! almunim wrap thinkness (real)
      SiRubDz  = 0.0			     ! silicon lubber thinkness (test)
      PhCathDz = 2.0 			     ! Photo Cathode thinkness
      PhCathR  = 1.9 			     ! Photo Cathode radius  (test)
      MuMetDz  = 11.0 			     ! Mu Metal Length
      MuMetR   = 1.9 			     ! Mu metal outer Radius  (real)
      MuMetR   = 2.0 			     ! Mu metal outer Radius  (test)
*
Fill FLGM				     ! PbG detector materials
      Density  = 3.86			     ! gdensity [/cm^3]
      RadLen   = 2.5			     ! radiation length [cm]
      PbContent= 65.4			     ! PbO content [%]
      CritEne  = 0.0158   		     ! critical energy [GeV]
      MoliereR = 3.32			     ! Moliere radius [cm]
*
FILL FLGC                                  ! one Copy of FPD
      Icopy    =  1                          ! 1- top, 2-south,3-bottom, 4-north
      Zpos     =  600.0                      ! Z positions
      RDis     =  25.6                       ! R-distances from beam
      DipAng   =  0.0                        ! Dip angle
FILL FLGC                                  ! one Copy of FPD
      Icopy    =  2                          ! 1- top, 2-south,3-bottom, 4-north
      Zpos     =  750.0                      ! Z positions
      RDis     =  31.0                       ! x-distances from beam
      DipAng   = 15.0                        ! Dip angle
FILL FLGC                                  ! one Copy of FPD
      Icopy    =  3                          ! 1- top, 2-south,3-bottom, 4-north
      Zpos     =  600.0                      ! Z positions
      RDis     =  25.6                       ! y-distances from beam
      DipAng   =  0.0                        ! Dip angle
FILL FLGC                                  ! one Copy of FPD
      Icopy    =  4                          ! 1- top, 2-south,3-bottom, 4-north
      Zpos     =  750.0                      ! Z positions
      RDis     =  31.0                       ! y-distances from beam
      DipAng   = -15.0                       ! Dip angle
*
*----------------------------------------------------------------------------
*
* PbG and general:
      Use    FPDG      " FPD master parameters  "
      Use    FLGG      " PbG detector general   "
      Use    FLGM      " PbG materials          "
* Endcap calorimeter sector:
      USE    FMCC      " actual positions       "
      USE    FMCS      " original parameters    "
      USE    FETR      " readout description    "

      prin1 fpdg_version; (' FPDMGEO version ', F4.2)
*     call  AgSSTEP(FFPDSTEP) 


* FPD Module
      if(fpdg_OnOff>0) then

        Create FPDM
	if (fpdg_OnOff==1 | fpdg_OnOff==3) then
	   Position FPDM in CAVE z=fpdg_zdis+fpdg_zlen/2      Konly='MANY'
	endif
* East FPD Module
	if (fpdg_OnOff==2 | fpdg_OnOff==3) then
           Position FPDM in CAVE z=-fpdg_zdis-fpdg_zlen/2 ThetaZ=180 Konly='MANY'
	endif
        prin1; ('FPDMGEO done')

      endif
*
* ----------------------------------------------------------------------------
Block FPDM is one FPD West volume 
      Material  Air
      Medium    standard
      Attribute PCON   seen=1 colo=7				!  lightblue
      shape     tube   dz=fpdg_zlen/2 rmin=fpdg_Rmin rmax=fpdg_Radius

* ECAL sector: dimensions are taken from original Endcap emcs_zorig and emcs_zend!!!
        diff    = fmcc_Zorig - fmcs_Zorig
	center  = (fmcc_Zorig+fmcc_Zend)/2
        Tan_Upp = tanf(fmcs_EtaMin)  
        Tan_Low = tanf(fmcs_EtaMax)
        rshift  = fmcs_Hub * sqrt(1. + Tan_Low*Tan_Low)
        dup     = fmcs_Rmshift*Tan_Upp
        dd      = fmcs_Rmshift*Tan_Low
        d2      = rshift + dd
        zdis    = fmcc_zorig-fpdg_Zdis
        dphi    = (fmcs_PhiMax-fmcs_PhiMin)/fmcs_Nsector
        fwoldz  = (fmcc_zend-fmcc_zorig)

	Create and Position FMSS z=-fpdg_zlen/2+fwoldz/2+zdis,
                                 x=fmcc_xOffset,
                                 AlphaZ=180

        If(section > fmcc_zend) then
          prin0 section,fmcc_zend
          (' FPDMGEO error: sum of sections exceeds maximum ',2F12.4)
        endif

* PbG detectors
* full half dimensions of FPD assembly
        ztot = (FLGG_Depth+FLGG_AlThick+FLGG_MuMetDz)/2.0
        rtot = (FLGG_NEta*FLGG_Width + (FLGG_NEta+1)*FLGG_DGap)/2.0

        do ipos = 1,3
          USE FLGC Icopy=Ipos
          alp  = 180-Ipos*90
* in FPDM mother z positions start from its center, its face is at -zlen/2
          zpos = -fpdg_zlen/2+ztot
          zdis = flgc_zpos-fpdg_zdis
         if (ipos==2) then 
          Create and Position FLGD z=zpos+zdis,
                                   x=(FLGC_Rdis+rtot)*cos(alp*degrad),
                                   y=(FLGC_Rdis+rtot)*sin(alp*degrad),
                                   AlphaY=FLGC_DipAng
          else
          Create and Position FLGD z=zpos+zdis,
                                   x=(FLGC_Rdis+rtot)*cos(alp*degrad),
                                   y=(FLGC_Rdis+rtot)*sin(alp*degrad),
                                   AlphaZ=180-Ipos*90
          endif
        enddo
EndBlock
* ----------------------------------------------------------------------------
* ----------------------------------------------------------------------------
Block FMSS is steel support of the EndCap module
      Attribute FMSS      seen=1    colo=1		! black	
      Material  Iron
      Shape     CONS   dz=(fmcc_zend-fmcc_zorig)/2,
                Rmn1=fmcs_Zorig*Tan_Low-d2  Rmn2=fmcs_Zend*Tan_Low-d2,
                Rmx1=fmcs_Zorig*Tan_Upp+dup Rmx2=fmcs_Zend*Tan_Upp+dup,
                phi1=fmcs_PhiMin/fmcs_Nsupsec,
		phi2=+fmcs_PhiMax/fmcs_Nsupsec
*
      Create and Position FMOD

EndBlock
* ----------------------------------------------------------------------------
Block FMOD is one module  of the EM EndCap
      Attribute FMOD      seen=1    colo=3			! green
      Material  Air
      Shape     CONS   dz=(fmcc_zend-fmcc_zorig)/2,
                Rmn1=fmcs_Zorig*Tan_Low-dd  Rmn2=fmcs_Zend*Tan_Low-dd,
                Rmx1=fmcs_Zorig*Tan_Upp+dup Rmx2=fmcs_Zend*Tan_Upp+dup
*
*    Running parameter 'section' contains the position of the current section
*     It should not be modified in daughters, use 'current' variable instead.
*     SecWid is used in all 'CONS' daughters to define dimensions.
*
      section = fmcc_zorig
      secwid  = fmcs_Front
      Create and Position FFLP     z=section-center+secwid/2
      section = section + secwid
*
      Do I_section =1,Fmcs_Nsection
*        number of section may be less then Fmcs_Nsection, keep the last available
         USE FSEC Isect=I_section  Stat=Istat  
*
         Secwid  = fsec_cell*fsec_Nlayer
         if (I_section == nint(Fmcs_Nsection)) then      ! Last section
          Secwid  = Secwid - fmcs_plate - 2*fmcs_plateS
	 endif	
         Create and position FPSE      z=section-center+secwid/2
         section = section + Secwid
* 
         if (I_section == 2) then      ! Shower Max section
            secwid  = fmcs_GapSMD
            Create and Position FSHM   z=section-center+secwid/2
            section = section + secwid
         endif
      enddo

      secwid  = fmcs_PlateB
      Create and Position FSSP      z=section-center+secwid/2
      section = section + secwid
endblock
* ----------------------------------------------------------------------------
Block FPSE is a single EM section
      Attribute FPSE   seen=1  colo=1 
      Material Air
      Material CAir Isvol=0
      Medium standard
*
      Shape     CONS  dz=secwid/2,  
                rmn1=(section-diff)*Tan_Low-dd  rmn2=(section+secwid-diff)*Tan_Low-dd,
                rmx1=(section-diff)*Tan_Upp+dup rmx2=(section+secwid-diff)*Tan_Upp+dup
      if (I_section==1) then
        Call GSTPAR (ag_imed,'CUTGAM',0.00001)
        Call GSTPAR (ag_imed,'CUTELE',0.00001)
        else
        Call GSTPAR (ag_imed,'CUTGAM',0.00008)
        Call GSTPAR (ag_imed,'CUTELE',0.001)
        Call GSTPAR (ag_imed,'BCUTE' ,0.0001)
      end if
*
      Do isec=1,nint(fmcs_Nslices)
        Create and Position FTOW AlphaZ=(isec-fmcs_Nslices/2.0-0.5)*dphi
      End Do 
Endblock
*---------------------------------------------------------------------------
Block FTOW is an individual 1/60 phi EM tower (section in fact)
      Attribute FTOW   seen=1  colo=1
      Shape     CONS   phi1=fmcs_PhiMin/fmcs_Nsector,
                       phi2=fmcs_PhiMax/fmcs_Nsector
      current = section
      Do is = 1,fsec_Nlayer
*        define actual Plate and cell thickness:         
         Plate  = fmcs_Plate + 2*fmcs_PlateS
         Gap = fsec_cell - Plate - fsec_scint(fmcs_Type)
         Cell = fsec_cell

* below: change from 3 to nint(Fmcs_Nsection)=8: 08/03/2001
         if (is==nint(fsec_Nlayer) & I_section == nint(Fmcs_Nsection)) then  
           Cell = fsec_cell - Plate  
	   Plate=0
         endif
*		 
         Create    FPER 
         Position  FPER  z=-secwid/2+(is-1)*fsec_cell+Cell/2 
         current = current + cell
      End Do
*
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block FPER  is a EM sesection period (super layer)
*
      Material  Air 
      Attribute FPER   seen=1  colo=1
      Shape     CONS   dz=Cell/2, 
                phi1=fmcs_PhiMin/fmcs_Nsector,
		phi2=+fmcs_PhiMax/fmcs_Nsector, 
                rmn1=(current-diff)*Tan_Low-dd   rmn2=(current+Cell-diff)*Tan_Low-dd,
                rmx1=(current-diff)*Tan_Upp+dup  rmx2=(current+Cell-diff)*Tan_Upp+dup
*
* --- Divide module (section) into radial blocks 
* 
      curcl = current+Cell/2
      Do ie = 1,fetr_NEta
        EtaBot  = fetr_EtaBin(ie)
        EtaTop  = fetr_EtaBin(ie+1)

        if(ie == 1) then         ! Lower slice
          RBot=(current-diff)*Tan_Low
        else                     ! 
          RBot=(curcl-diff)*Tanf(EtaBot)
        endif
*
        if(Plate > 0) then       ! Ordinary Sci layer
          RTop=min((curcl-diff)*Tanf(EtaTop), ((current-diff)*Tan_Upp+dup))
        else                     ! last Sci layer in section
          RTop=min((curcl-diff)*Tanf(EtaTop), ((current-diff)*Tan_Upp+dup))
        endif
        check RBot<RTop
*
	xx=tan(pi*fmcs_PhiMax/180.0/fmcs_Nsector)
        yy=cos(pi*fmcs_PhiMax/180.0/fmcs_Nsector)
	Create and Position  FTAR    x=(RBot+RTop)/2  ORT=YZX 
*         prin0 ie,EtaTop,EtaBot,rbot,rtop
*         (' EPER : ie,EtaTop,EtaBot,rbot,rtop ',i3,4F12.4)
      enddo
*
EndBlock
*  - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block FTAR is one CELL of scintillator, fiber and laminated lead
*
      Attribute FTAR   seen=1  colo=2				! violet
*     local z goes along the radius, y is the thickness
      Shape     TRD1   dy=Cell/2   dz=(RTop-RBot)/2,
                       dx1=RBot*xx-fmcs_GapCel/yy,
                       dx2=RTop*xx-fmcs_GapCel/yy
*
      G10 = fmcs_Gten
      Create and Position    FSCI        y=(-cell + fsec_scint(fmcs_type))/2
      Create and Position    FGTN        y=(-cell+G10)/2+fsec_scint(fmcs_type)
      if (Plate>0) then
        Create and Position    FXFP        y=(cell + fmcs_plateS)/2 - plate
        Create and Position    FLED        y=(cell - plate)/2
        Create and Position    FXFP        y=(cell - fmcs_plateS)/2
      end if
EndBlock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block FGTN  is the G10 layer  
*
*     G10 is about 60% SiO2 and 40% epoxy
      Component Si    A=28.08  Z=14   W=0.6*1*28./60.
      Component O     A=16     Z=8    W=0.6*2*16./60.
      Component C     A=12     Z=6    W=0.4*8*12./174.
      Component H     A=1      Z=1    W=0.4*14*1./174.
      Component O     A=16     Z=8    W=0.4*4*16./174.
      Mixture   g10   Dens=1.7
      Attribute FGTN   seen=1   colo=4			! blue       
*     local z goes along the radius, y is the thickness
      Shape     TRD1   dy=Fmcs_GTen/2  dz=(RTop-RBot)/2
EndBlock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block FSCI  is the active scintillator (polystyren) layer  
*
  Material  POLYSTYREN
      Material  Cpolystyren   Isvol=1
      Attribute FSCI   seen=1   colo=7   fill=0   	! lightblue
*     local z goes along the radius, y is the thickness
      Shape     TRD1   dy=fsec_scint(fmcs_type)/2  dz=(RTop-RBot)/2-fmcs_GapCel
      Call GSTPAR (ag_imed,'CUTGAM',0.00008)
      Call GSTPAR (ag_imed,'CUTELE',0.001)
      Call GSTPAR (ag_imed,'BCUTE' ,0.0001)
      Call GSTPAR (ag_imed,'CUTNEU',0.001)
      Call GSTPAR (ag_imed,'CUTHAD',0.001)
      Call GSTPAR (ag_imed,'CUTMUO',0.001)
* define Birks law parameters
      Call GSTPAR (ag_imed,'BIRK1',1.)
      Call GSTPAR (ag_imed,'BIRK2',0.013)
      Call GSTPAR (ag_imed,'BIRK3',9.6E-6)
*     
      HITS FSCI   Birk:0:(0,10)
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block FLED  is lead absorber Plate 
*
      Material  Lead
      Material  CLead Isvol=0
      Attribute FLED   seen=1   colo=3  fill=1			! green
      Shape     TRD1   dy=fmcs_Plate/2  dz=(RTop-RBot)/2
      Call GSTPAR (ag_imed,'CUTGAM',0.00008)
      Call GSTPAR (ag_imed,'CUTELE',0.001)
      Call GSTPAR (ag_imed,'BCUTE' ,0.0001)
      Call GSTPAR (ag_imed,'CUTNEU',0.001)
      Call GSTPAR (ag_imed,'CUTHAD',0.001)
      Call GSTPAR (ag_imed,'CUTMUO',0.001)

endblock
* ----------------------------------------------------------------------------
Block FFLP  is First Aluminium plate 
*
      Material  Aluminium
      Attribute FFLP   seen=1  colo=3 fill=1			! green
      Shape     TUBS   dz=SecWid/2,
                       rmin=(section-diff)*Tan_Low,
                       rmax=(section-diff)*Tan_Upp+dup,
                       phi1=fmcs_PhiMin/fmcs_Nsupsec,
		       phi2=fmcs_PhiMax/fmcs_Nsupsec
*                rmn1=(section-diff)*Tan_Low-dd rmn2=(section+secwid-diff)*Tan_Low-dd,
*                rmx1=(section-diff)*Tan_Upp-dd rmx2=(section+secwid-diff)*Tan_Upp-dd
endblock
* ----------------------------------------------------------------------------
Block FXFP  is SS laminated plate
*
      Material  Iron
      Attribute FXFP   seen=1  colo=6 fill=1		! violet
      Shape     TRD1   dy=fmcs_PlateS/2  dz=(RTop-RBot)/2
endblock
* ----------------------------------------------------------------------------
Block FSHM  is the SHower Max  section
*
      Material  Air Isvol=0
      Attribute FSHM   seen=1   colo=4			!  blue
      Shape     CONS   dz=SecWid/2,
                phi1=fmcs_PhiMin/fmcs_Nsupsec,
		phi2=+fmcs_PhiMax/fmcs_Nsupsec,
                rmn1=(section-diff)*Tan_Low-dd rmn2=(section+secwid-diff)*Tan_Low-dd,
                rmx1=(section-diff)*Tan_Upp+dup rmx2=(section+secwid-diff)*Tan_Upp+dup
*      Call GSTPAR (ag_imed,'CUTGAM',0.00001)
*      Call GSTPAR (ag_imed,'CUTELE',0.00001)
*      Call GSTPAR (ag_imed,'LOSS',1.)
*      Call GSTPAR (ag_imed,'STRA',1.)
*
      USE FMXG Version=1
      curr =  section
      maxcnt = curr+fmcs_GapSMD/2 
*
      Do J_section = 1,2
*       
         USE FXSE Jsect=J_section
*
	 msecwd=fxse_Swidth - fxse_Aplate - fmxg_Sapex
         Create and Position FXGT      z=curr-maxcnt+msecwd/2
         curr = curr + msecwd
*
         msecwd  = fmxg_Sapex
	 if (J_section =1) then
  	   rtop = curr+msecwd/2
*          prin0 rtop
*          (' Z for u:  ',F12.4)
           Create and position FXSG   z=curr-maxcnt+msecwd/2
	 else
	   rtop = curr+msecwd/2
*          prin0 rtop
*          (' Z for v:  ',F12.4)
           Create and position FXSG   z=curr-maxcnt+msecwd/2 AlphaX=180
				 
         endif
           curr = curr + msecwd
*
           msecwd  = fxse_Aplate
           Create and Position FALP   z=curr-maxcnt+msecwd/2
           curr = curr + msecwd
      End do

Endblock
* ----------------------------------------------------------------------------
Block FXGT  is the G10 layer in the SMax  
*
*     G10 is about 60% SiO2 and 40% epoxy
      Component Si    A=28.08  Z=14   W=0.6*1*28./60.
      Component O     A=16     Z=8    W=0.6*2*16./60.
      Component C     A=12     Z=6    W=0.4*8*12./174.
      Component H     A=1      Z=1    W=0.4*14*1./174.
      Component O     A=16     Z=8    W=0.4*4*16./174.
      Mixture   g10   Dens=1.7
      Attribute FXGT   seen=1   colo=7
      Shape     CONS   dz=msecwd/2,
                phi1=fmcs_PhiMin/fmcs_Nsupsec,
		phi2=fmcs_PhiMax/fmcs_Nsupsec,
                rmn1=(curr-diff)*Tan_Low-dd rmn2=(curr+msecwd-diff)*Tan_Low-dd,
                rmx1=(curr-diff)*Tan_Upp-dd rmx2=(curr+msecwd-diff)*Tan_Upp-dd    
      Call GSTPAR (ag_imed,'CUTGAM',0.00001)
      Call GSTPAR (ag_imed,'CUTELE',0.00001)
EndBlock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block FXSG  is the Shower max  Gap for scintillator strips
*
      Attribute FXSG   seen=1   colo=7			! black
      Material  Air   Isvol=0
      Shape     CONS   dz=msecwd/2,
                phi1=fmcs_PhiMin/fmcs_Nsupsec,
		phi2=fmcs_PhiMax/fmcs_Nsupsec,
                rmn1=(curr-diff)*Tan_Low-dd  rmn2=(curr+msecwd-diff)*Tan_Low-dd,
                rmx1=(curr-diff)*Tan_Upp+dup rmx2=(curr+msecwd-diff)*Tan_Upp+dup
*
      sq3 = sqrt(3.)
      sq2 = sqrt(2.)
      Rbot = (curr-diff+msecwd/2.)*Tan_Low-dd
      Rtop = (curr-diff+msecwd/2.)*Tan_Upp+dup
*
      if (fmcs_Type==1) then
        rth = Pi/fmcs_Nsupsec
        tng = tan(rth)
	Nstr = nint((rtop*sq3 - rbot)/fmxg_Sbase)
	if(Nstr > 300) Nstr = 300
        Do i_str = 1,nstr
        	p = .5*(i_str*fmxg_Sbase + rbot)
*
		if (p <= .5*rbot*sq3) then
        		yleft = .5*(sq2*p - sqrt(2.*rbot*rbot - 2.*p*p))
			xleft = sq2*p - yleft
   			xright = sq2*p/(1. - tng )
   			yright = -tng*xright
		else if (.5*rbot < p <= .5*Rtop) then 
   			xleft = sq2*p/(1. + tng )
   			yleft = tng*xleft
   			xright = sq2*p/(1. - tng )
   			yright = -tng*xright
		else if (.5*rtop < p <= .5*rtop*sq3) then
   			xleft = sq2*p/(1. + tng )
   			yleft = tng*xleft
        	 	yright = .5*(sq2*p - sqrt(2.*rtop*rtop - 2.*p*p))
			xright = sq2*p - yright
		endif

		len = sqrt((xleft-xright)**2 + (yleft-yright)**2 )
		xc = .5*(xright+xleft)
		yc = .5*(yright+yleft)
*         	prin0 i_str,xleft,yleft,xright,yright,len
*		      (' FXSG: i,xleft,yleft,xright,yright,len',i3,5F12.4)
			 
      		if (mod(i_str,2) != 0 ) then			 
       			Create and Position FHMS  x=xc y=yc AlphaZ=45
		else
       			Create and Position FHMS  x=xc y=yc AlphaZ=45 AlphaX=180
		endif
	End do
      else
	if (J_section=1) then
*	  Nstr = 60
	  Nstr = 100
	  len  = 36
          Do i_str = 1,nstr
		xc = (Rbot+Rtop)/2 + (i_str-Nstr/2.0)*fmxg_Sbase/2.0
	        yc = 0.0
      		if (mod(i_str,2) != 0 ) then			 
       			Create and Position FHMS  x=xc y=yc
		else
       			Create and Position FHMS  x=xc y=yc AlphaX=180
		endif
	  End do
	else
*	  Nstr = 100
	  Nstr = 60
	  len  = 36
          Do i_str = 1,nstr
		xc = (Rbot+Rtop)/2.0
		yc = (i_str-Nstr/2.0)*fmxg_Sbase/2.0
      		if (mod(i_str,2) != 0 ) then			 
       			Create and Position FHMS  x=xc y=yc AlphaZ=90
		else
       			Create and Position FHMS  x=xc y=yc AlphaZ=90 AlphaX=180
		endif
	  End do
	endif
      endif

*     dcut exsg z 0 0 10 0.1 0.1
*     dcut exsg y 0 10 -50 0.7 0.7

endblock
* ----------------------------------------------------------------------------
Block FHMS is  sHower Max Strip
*
      Material  POLYSTYREN
      Material  Cpolystyren   Isvol=1
      Attribute FHMS      seen=1    colo=2		! red
      Shape     TRD1 dx1=0 dx2=fmxg_Sbase/2 dy=len/2 dz=fmxg_Sapex/2
      Call GSTPAR (ag_imed,'CUTGAM',0.00008)
      Call GSTPAR (ag_imed,'CUTELE',0.001)
      Call GSTPAR (ag_imed,'BCUTE', 0.0001)
* define Birks law parameters
      Call GSTPAR (ag_imed,'BIRK1',1.)
      Call GSTPAR (ag_imed,'BIRK2',0.0130)
      Call GSTPAR (ag_imed,'BIRK3',9.6E-3)
*
      HITS FHMS     Birk:0:(0,10)
Endblock
* ----------------------------------------------------------------------------
Block FALP  is ALuminium  Plate in shower max 
*
      Material  Aluminium
      Material  CAluminium   Isvol=0
      Attribute FALP   seen=1  colo=1
      Shape     CONS   dz=msecwd/2,
                phi1=fmcs_PhiMin/fmcs_Nsupsec,
		phi2=+fmcs_PhiMax/fmcs_Nsupsec,
                rmn1=(curr-diff)*Tan_Low-dd rmn2=(curr+msecwd/2-diff)*Tan_Low-dd,
                rmx1=(curr-diff)*Tan_Upp-dd rmx2=(curr+msecwd/2-diff)*Tan_Upp-dd
      Call GSTPAR (ag_imed,'CUTGAM',0.00001)
      Call GSTPAR (ag_imed,'CUTELE',0.00001)
      Call GSTPAR (ag_imed,'LOSS',1.)
      Call GSTPAR (ag_imed,'STRA',1.)
endblock
* ----------------------------------------------------------------------------
Block FSSP  is stainless steel  Plate 
*
      Material  Iron      
      Attribute FSSP   seen=1  colo=6 fill=1	
      Shape     CONS   dz=Fmcs_PlateB/2,
                phi1=fmcs_PhiMin/fmcs_Nsupsec,
		phi2=+fmcs_PhiMax/fmcs_Nsupsec,
                rmn1=(section-diff)*Tan_Low-dd rmn2=(section+secwid-diff)*Tan_Low-dd,
                rmx1=(section-diff)*Tan_Upp+dup rmx2=(section+secwid-diff)*Tan_Upp+dup
endblock
* ----------------------------------------------------------------------------
* FWOL nice views: dcut ecvo x 1       10 -5  .5 .1
*                  draw emdi 105 0 160  2 13  .2 .1
*                  draw emdi 120 180 150  1 14  .12 .12
* ---------------------------------------------------------------------------
Block FLGD is one Pb-Glass fpd detector
      Material  Air
      Medium    standard
      Attribute FLGD seen=1 colo=1
      shape box dz= ztot,
                dy=(FLGG_NEta*FLGG_Width + (FLGG_NEta+1.0)*FLGG_DGap)/2.0,
                dx=(FLGG_NPhi*FLGG_Width + (FLGG_NPhi+1.0)*FLGG_DGap)/2.0

      do ii=1, FLGG_NEta
	yy = (ii-FLGG_NEta/2.0-0.5)*(FLGG_Width+FLGG_DGap)
        do jj=1, FLGG_NPhi
	  xx = (jj-FLGG_NPhi/2.0-0.5)*(FLGG_Width+FLGG_DGap)
            Create and Position FLGT x=xx y=yy
	enddo
      enddo
EndBlock
* ----------------------------------------------------------------------------
Block FLGT is one PbG Tower
      material Air
      Attribute FLGT seen=1 colo=2
      Shape box	dz=ztot,
		dx=FLGG_Width/2.0+FLGG_AlThick,
		dy=FLGG_Width/2.0+FLGG_AlThick 

      Create and Position FWAL z=-ztot+(FLGG_AlThick+FLGG_depth)/2.0
      Create and Position FUMT z=-ztot+FLGG_AlThick+FLGG_depth+FLGG_MuMetDz/2.0
      Create and Position FPRB z=-ztot+FLGG_AlThick+FLGG_depth+FLGG_SiRubDz/2.0
      Create and Position FPCT z=-ztot+FLGG_AlThick+FLGG_depth+FLGG_SiRubDz
     >                                +FLGG_PhCathDz/2.0

Endblock
* ----------------------------------------------------------------------------
Block FWAL is almunum wrapper
      material Aluminium
      Attribute FWAL seen=1 colo=3
      Shape box	dz=FLGG_Depth/2.0+FLGG_AlThick/2.0,
		dx=FLGG_Width/2.0+FLGG_AlThick,
		dy=FLGG_Width/2.0+FLGG_AlThick 
      if(fpdg_ChkvSim==1) CALL GSCKOV(%Imed,N,E,ABSCO_Alm,EFFIC_all,RINDEX_Alm)

      Create and Position FLGR z=+FLGG_AlThick/2.0

Endblock
* ----------------------------------------------------------------------------
Block FLGR is Lead Glass detector
*     PbG is about 65% Pb 
      Component Pb    A=207.19 Z=82   W=.60712
      Component K     A=39.102 Z=19   W=.02324
      Component Si    A=28.088 Z=14   W=.14771
      Component O     A=15.999 Z=8    W=.22041
      Component As    A=74.922 Z=33   W=.00152
      Mixture   PbG   Dens=FLGM_Density Radl=FLGM_RadLen
      Medium leadglass ISVOL=1
      Attribute FLGR  seen=1    colo=4		! red
      Shape     box dz=FLGG_depth/2 dx=FLGG_Width/2 dy=FLGG_Width/2

      if(fpdg_ChkvSim==1) then
	CALL GSCKOV(%Imed,N,E,ABSCO_PbG,EFFIC_All,RINDEX_PbG)
      else 
        Call GSTPAR (ag_imed,'CUTELE', flgm_CritEne)
	HITS FLGR  ELoss:0:(0,10)
      endif
Endblock
* ----------------------------------------------------------------------------
Block FPRB is silicon rubber
      material silicon
      Attribute FPRB seen=1 colo=5
      Shape tube dz=FLGG_SiRubDz/2.0 rmin=0 rmax=FLGG_PhCathR
      if(fpdg_ChkvSim==1) CALL GSCKOV(%Imed,N,E,ABSCO_SiRub,EFFIC_All,RINDEX_SiRub)
Endblock
* ----------------------------------------------------------------------------
Block FPCT is Photo Cathode
      material Air
      Medium PhotCath ISVOL=1
      Attribute LPCT seen=1 colo=6
      Shape tube dz=FLGG_PhCathDz/2.0 rmin=0 rmax=FLGG_PhCathR
      if(fpdg_ChkvSim==1) then
	CALL GSCKOV(%Imed,N,E,ABSCO_PhCath,EFFIC_PhCath,RINDEX_PhCath)
	HITS FPCT USER:0:(0,10000)
*	HITS FPCT USER:0:(0,100000)
      endif
Endblock
* ----------------------------------------------------------------------------
Block FUMT is mu metal
      material iron
      Attribute LUMT  seen=1    colo=1
      Shape tube dz=FLGG_MuMetDz/2.0 rmin=FLGG_PhCathR rmax=FLGG_MuMetR
      if(fpdg_ChkvSim==1) CALL GSCKOV(%Imed,N,E,ABSCO_MuMet,EFFIC_All,RINDEX_MuMet)
Endblock
* ----------------------------------------------------------------------------
end

      subroutine FFPDSTEP
* discard cerenkov photon absorption hits everywhere except for
* photocathode
+CDE,TYPING,GCBANK,GCONST,GCUNIT,GCTMED,GCTRAK,GCKINE,GCSETS,AGCSTEP.
      character  Cmed*13

      check ISVOL>0 
      CALL UHTOC(NATMED,4,Cmed,8)
*     if (istop==2) write (*,*) '***',Cmed,isvol,Ipart,AdEstep,istop,destep
*      if(Cmed=='FWOL_PhotCath') then
*	 if(Ipart!=50) then
*          {Step,dEstep,aStep,AdEstep} = 0
*         endif
*      else if(Cmed=='FWOL_FLGR') then
*      endif
      return
      end

      subroutine FPCTSTEP(JJ,HIT)
* define hit element USER as # of cherenkov photon detected
+CDE,TYPING,GCBANK,GCONST,GCUNIT,GCTRAK,GCKINE,GCSETS,AGCSTEP.
      Integer JJ
      Real    HIT
*      print *,'FPCTSTEP is here!!!',deStep,Istop
      if(Ipart == 50 & Istop ==2 & deStep>0.0) then
	hit=1.0
	return
      endif
      hit = 0.0
      return
      end 
      
 
