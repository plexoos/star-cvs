* $Id: richgeo.g,v 1.12 2001/04/09 15:31:37 nevski Exp $
*
* $Log: richgeo.g,v $
* Revision 1.12  2001/04/09 15:31:37  nevski
* second version of cerenkov light properties introduced
*
* Revision 1.11  2001/03/13 23:33:42  nevski
* quartz absco corrected back
*
* Revision 1.10  2001/03/13 20:56:33  nevski
* variable rich position taken from DB
*
* Revision 1.9  2000/02/22 20:34:14  nevski
* LGAM added to HITs definition
*
* Revision 1.8  2000/01/28 21:53:37  nevsky
* secondaries in Rich can be discarded
*
* Revision 2.0  2000/10/12 kunde
*
***************************************************************************
module  RICHGEO defines Ring Image Cerenkov geometry
author  Gerd Kunde
created June 1, 1998 supposely
***************************************************************************
*  medium vs material table at my understanding:
*       air   honey  quarz  freon methan   csi    gri   opaco   Gap    Al
*  med: 101    102    103    104    105    106    107    108    109    110
*  mat: 101    106    120    130    140    106    111    121    140    150
*       air     C    SiO2   C6F14   CH4     C     Cu    SiO2    CH4    Al
*             Radl?                       Radl?

+CDE,AGECOM,GCUNIT,GCONST.

    Content   RICH,SRIC,ALUM,HONE,OQUA,OQUF,QUAR,RGAP,RCSI,
              BARR,SPAC,FREO,ALMF,BORD,HOLE ",META"

    Structure Rich {int version, int position, int cversion, dx, dy, dz }
    Structure Rpos {int position, phi, rpos, ypos, zpos, tx, ty, tz }
    Structure RANG {int version, int length, ppckov(30) }
    Structure Rmat {char Mate, rindex(30), absco(30), effic(30)  }

    Integer   N,i,IsOpt

* double precision is needed to keep angle<->cosinus conversion correct
    Real*8    rot(3,3), tx, ty, tz
    EXTERNAL  RICHSTEP,RcsiStep
*
    Begin   

    fill RICH  ! Cerenkov detector parameters
        version  = 1       ! complexity version
        position = 1       ! actual position in STAR
        cversion = 1       ! version of Cerenkov light tables
        dx       = 49.45   ! Half width (long dimension)
        dy       = 11.325  ! Rich half thickness
        dz       = 73.15   ! Half length (short dimension)

    fill RPOS  ! Cerenkov detector position 
        position = 1          ! nominal position in STAR
        phi      = 300        ! pad plane center phi position
        rpos     = 235.0      ! distance to center  
        ypos     =  0         ! pad plane center offset
        zpos     =  0         ! z position of the pad plane center
        tx       =  0         ! rich orientation x-angle (deg)
        ty       =  0         ! rich orientation y-angle (deg)
        tz       =  30        ! rich orientation z-angle (deg)

    fill RPOS  ! Cerenkov detector position 
        position = 2          ! actual position in STAR
        phi      = -59.95062  ! pad plane center phi position
        rpos     = 240.22755  ! distance to the pad pane center
        ypos     =   8.125    ! pad plane center offset
        zpos     = -.3089772  ! z position of the pad plane center
        tx       = -.02003531 ! rich orientation x-angle (deg)
        ty       =  .03788445 ! rich orientation y-angle (deg)
        tz       =   29.81715 ! rich orientation z-angle (deg)
    endfill

*
    fill RANG(1)       ! Cerenkov light energy range
        Version =  1                           ! table version
        Length  = 13	                       ! table length
        ppckov  = { 5.63e-9, 5.77e-9, 5.9e-9, 6.05e-9, 6.2e-9, 6.36e-9, 
                    6.52e-9, 6.7e-9, 6.88e-9, 7.08e-9, 7.3e-9, 7.52e-9,
                    7.75e-9 }                  !  photon energies  

    fill RANG(1)/RMAT  ! Material property for Cerenkov light
        MATE   = 'GRI'                         ! default materials
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1 } ! Detection efficiencies
        Rindex = { 1,1,1,1,1,1,1,1,1,1,1,1,1 } ! Refraction index
        Absco  = { 1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,
                   1.e-4,1.e-4,1.e-4 }         ! Absorbtion lenghts (in cm)

    fill RANG(1)/RMAT  ! Material property for Cerenkov light
        MATE   = 'QRZO'                        ! Opaque quarz
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1 } ! Detection efficiencies
        Rindex = { 1,1,1,1,1,1,1,1,1,1,1,1,1 } ! Refraction index
        Absco  = { 1.e-5,1.e-5,1.e-5,1.e-5,1.e-5,1.e-5,1.e-5,1.e-5,1.e-5,1.e-5,
                   1.e-5,1.e-5,1.e-5 }         ! Absorbtion lenghts (in cm)

    fill RANG(1)/RMAT  ! Material property for Cerenkov light
        MATE   = 'META'                        ! material - methane
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1 } ! Detection efficiencies
        Rindex = { 1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444 }                  ! Refraction index
        Absco  = { 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6,
                   1.e6, 1.e6, 1.e6}           ! Absorbtion lenghts (in cm)

    fill RANG(1)/RMAT  ! Material property for Cerenkov light
        MATE   = 'QARZ'                        ! material - quartz
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1 } ! Detection efficiencies
        Rindex = { 1.528309, 1.533333, 1.538243, 1.544223,1.550568, 1.557770, 
                   1.565463, 1.574765, 1.584831, 1.597027, 1.611858, 1.62366, 
                   1.63815}                    ! Refraction index
        Absco  = { 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 29.85, 7.34, 4.134, 1.273,
                    .722, .365, .10051, 0.001} ! Absorbtion lenghts (in cm)

    fill RANG(1)/RMAT  ! Material property for Cerenkov light
        MATE   = 'FREO'                        ! material
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1 } ! Detection efficiencies
        Rindex = { 1.269946, 1.271246, 1.272452, 1.273844, 1.275236, 1.276721,
                   1.278206, 1.279876, 1.281546, 1.283402, 1.285444, 1.28731,
                   1.28936 }                   ! Refraction index
        Absco  = { 179.0987, 179.0987, 179.0987, 179.0987, 179.0987, 121.9547,
                   43.40067, 15.7394, 9.417928, 5.195241,  1.415808, 0.36728,
                   0.001}                      ! Absorbtion lenghts (in cm)

    fill RANG(1)/RMAT  ! Material property for Cerenkov light
        MATE   = 'CSI'                         ! material - Cesium Iodide
        Effic  = { 3.15e-4, 4.50e-4, 6.75e-3, 1.125e-2, 2.115e-2, 3.60e-2, 
                   8.46e-2,  .15533,  .20286,   .24745,   .27881,  0.3060,
                   0.3250 }                    ! quantum efficiencies
        Rindex = { 1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444 }                  ! Refraction index
        Absco  = { 1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,
                   1.e-4,1.e-4,1.e-4 }         ! Absorbtion lenghts (in cm)
    endfill

*
    fill RANG(2)       ! Cerenkov light energy range
        Version =  2                           ! table version
        Length  = 26	                       ! table length
        ppckov  = { 5.5e-9, 5.6e-9, 5.7e-9, 5.8e-9, 5.9e-9, 6.0e-9, 
                    6.1e-9, 6.2e-9, 6.3e-9, 6.4e-9, 6.5e-9, 6.6e-9,
		    6.7e-9, 6.8e-9, 6.9e-9, 7.0e-9, 7.1e-9, 7.2e-9,
                    7.3e-9, 7.4e-9, 7.5e-9, 7.6e-9, 7.7e-9, 7.8e-9,
		    7.9e-9, 8.0e-9 }           !  photon energies  

    fill RANG(2)/RMAT  ! Material property for Cerenkov light
        MATE   = 'GRI'                         ! default materials
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1,
		   1,1,1,1,1,1,1,1,1,1,1,1,1}  ! Detection efficiencies
        Rindex = { 1,1,1,1,1,1,1,1,1,1,1,1,1,
		   1,1,1,1,1,1,1,1,1,1,1,1,1}  ! Refraction index
        Absco  = { 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4,
                   1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4,
		   1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4, 1.e-4,
                   1.e-4, 1.e-4 }              ! Absorbtion lenghts (in cm)

    fill RANG(2)/RMAT  ! Material property for Cerenkov light
        MATE   = 'QRZO'                        ! Opaque quarz
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1,
		   1,1,1,1,1,1,1,1,1,1,1,1}    ! Detection efficiencies
        Rindex = { 1,1,1,1,1,1,1,1,1,1,1,1,1,
		   1,1,1,1,1,1,1,1,1,1,1,1}    ! Refraction index
        Absco  = { 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5,
		   1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5,
		   1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5, 1.e-5,
		   1.e-5, 1.e-5 }              ! Absorbtion lenghts (in cm)

    fill RANG(2)/RMAT  ! Material property for Cerenkov light
        MATE   = 'META'                        ! material - methane
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1,
		   1,1,1,1,1,1,1,1,1,1,1,1,1}  ! Detection efficiencies
        Rindex = { 1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
		   1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
		   1.000444, 1.000444 }        ! Refraction index
        Absco  = { 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 
                   1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6,
                   1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6, 1.e6,
                   1.e6, 1.e6 }                ! Absorbtion lenghts (in cm)

    fill RANG(2)/RMAT  ! Material property for Cerenkov light
        MATE   = 'QARZ'                        ! material - quartz
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1,
		   1,1,1,1,1,1,1,1,1,1,1,1,1}  ! Detection efficiencies
        Rindex = { 1.52398,  1.52738, 1.5309,  1.5346,  1.53835,
	           1.542302, 1.54641, 1.55067, 1.55513, 1.55976,
		   1.56458,  1.56962, 1.57488, 1.58037, 1.58611,
		   1.59212,  1.59842, 1.60503, 1.61197, 1.61927,
		   1.62696,  1.63506, 1.64362, 1.65269, 1.662295,
		   1.6725}		       ! Refraction index
        Absco  = { 105.8,   65.52,   48.58,   42.85,   35.79,
		   31.262,  28.598,  27.527,  25.007,  22.815,
		   21.04,   19.266,  17.525,  15.878,  14.177,
		   11.719,  9.282,   6.62,    4.0925,  2.601,
		   1.149,   0.667,   0.3627,  0.192,   0.1497,
		   0.10857}                    ! Absorbtion lenghts (in cm)

    fill RANG(2)/RMAT  ! Material property for Cerenkov light
        MATE   = 'FREO'                        ! material
        Effic  = { 1,1,1,1,1,1,1,1,1,1,1,1,1,
		   1,1,1,1,1,1,1,1,1,1,1,1,1}  ! Detection efficiencies
        Rindex = {1.2716, 1.27332, 1.27504, 1.27676, 1.27848, 
                  1.2802, 1.28192, 1.28364, 1.28536, 1.28708,
		  1.2888, 1.29052, 1.29224, 1.29396, 1.29568,
		  1.2974, 1.29912, 1.30084, 1.30256, 1.30428, 
		  1.306,  1.30772, 1.30944, 1.31116, 1.31288, 
		  1.3146}                      ! Refraction index
        Absco  = { 176.4219, 176.4219, 176.4219, 176.4219, 176.4219,
                   176.4219, 176.4219, 176.4219, 176.4219, 176.4219,
                   176.4219, 176.4219, 176.4219, 176.4219,  58.322,
                    41.6371,  22.5158,  10.9767,  4.01237,  1.65053,
                   0.783613, 0.452046, 0.312553,  0.15627,  0.0, 
                   0.0}                        ! Absorbtion lenghts (in cm)

    fill RANG(2)/RMAT  ! Material property for Cerenkov light
        MATE   = 'CSI'                         ! material - Cesium Iodide
        Effic  = { 0.0002,   0.0006,  0.0007,  0.005,   0.0075,
                   0.010125, 0.0243,  0.0405,  0.06885, 0.1053,
                   0.1215,   0.1435,  0.16,    0.164,   0.1681, 0.17,
                   0.1785,   0.1811,  0.1836,  0.187,   0.1904, 0.1921,
                   0.1938,   0.1955,  0.2125,
		   0.221}                      ! quantum efficiencies
        Rindex = { 1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444, 1.000444, 1.000444, 1.000444, 1.000444, 1.000444,
                   1.000444, 1.000444}         ! Refraction index
        Absco  = { 1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,
                   1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,
                   1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,1.e-4,
                   1.e-4,1.e-4}                ! Absorbtion lenghts (in cm)
    endfill

* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

    call  AgSSTEP(RICHSTEP) 

    USE RICH
    isopt=0
    if (rich_version>1) isopt=1

* select a DB parameter version and make this bank unique

    USE RPOS position=rich_position Oper=U
    USE RANG version =rich_cversion Oper=U

* make selected RANG Current Working Directory

    USE RICHGEO/RICH/RANG
    N = rang_length

*   special material definition here:
    component O    A=16.00     Z=8    W=2
    component Si   A=28.09     Z=14   W=1
    mixture   quarz         DENs=2.64
      
    component O    A=16.00     Z=8    W=2
    component Si   A=28.09     Z=14   W=1
    mixture   opaco         DENS=2.64
*
    component C    A=12.01  Z=6   W=1
    component H    A=1      Z=1   W=4
    mixture   methane       Dens=0.000717
*
    Material  Carbon
    Material  Carbonio      Radl=0.376   Dens=2.265
*   Material  Carbonio      Radl=18.8    Dens=2.265
*
    component Cu      A=63.540  Z=29  w=1
    mixture   Anode   Radl=1.43 Dens=8.96
*      
    Material  Air
    Medium    Standard

* define RICH position using reconstructed Euler angles
* thanks to James Dunlop

    tx = degrad*rpos_tx
    ty = degrad*rpos_ty
    tz = degrad*rpos_tz

    rot(1,1) = -cos(tz)*cos(ty)
    rot(2,1) = -sin(tz)*cos(ty)
    rot(3,1) =  sin(ty)

    rot(1,2) = -cos(tz)*sin(ty)*sin(tx) + sin(tz)*cos(tx)
    rot(2,2) = -sin(tz)*sin(ty)*sin(tx) - cos(tz)*cos(tx)
    rot(3,2) = -cos(ty)*sin(tx)
  
    rot(1,3) =  cos(tz)*sin(ty)*cos(tx) + sin(tz)*sin(tx)
    rot(2,3) =  sin(tz)*sin(ty)*cos(tx) - cos(tz)*sin(tx)
    rot(3,3) =  cos(ty)*cos(tx)

    Create and Position RICH in CALB _
      x = rpos_rpos*cos(rpos_phi*degrad)-rpos_ypos*rot(1,2),
      y = rpos_rpos*sin(rpos_phi*degrad)-rpos_ypos*rot(2,2),
      z = rpos_zpos-rpos_ypos*rot(3,2),
      ThetaX=acos(rot(3,1))*raddeg  PhiX=atan2(rot(2,1),rot(1,1))*raddeg,
      ThetaY=acos(rot(3,2))*raddeg  PhiY=atan2(rot(2,2),rot(1,2))*raddeg,
      ThetaZ=acos(rot(3,3))*raddeg  PhiZ=atan2(rot(2,3),rot(1,3))*raddeg
                                            
*     AlphaZ=rpos_phi-90

    prin1  %x,%y,%z, (rot(i,2),i=1,3);
    (' Rich position =',3f10.4,',  orientation =',3f10.6)

* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
block RICH is just an aluminum box

*   outside dimensions                         mm       cm
*   tickness        104+25+41+22.5+2+32 =  226.5 /2 = 11.325
*   long dimension  146.3 /2 = 73.15  cm          
*   short dimension 989 /2   = 494.5  mm

    Material  Aluminium
    Attribute RICH seen=1   colo=5
    shape     BOX  dx=rich_dx  dy=rich_dy  dz=rich_dz      
    USE       RMAT  Mate='GRI'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)

    create and position  SRIC
endblock
*-----------------------------------------------------------------
block SRIC is sensitive part of the whole RICH ... full of Methane

* inside dimensions
* long dimension 146.3 - 2*70 = 132.3 / 2 = 66.15
* short dimension  989 - 2*70 =  84.9 / 2 = 42.45
*   +194.5+16    Aluminum frame                9.725
*   +194.5+16    Hole in Frame                 9.725
*   +194.5+2.25  board                         8.350
*   +194.5+0.25  csi center                    8.150
*   +192.5  4 millimeter volume gap center     7.925                       
*    (190.5 - 98 )/2=46.25+ 98 = 144.25 methane center  3.1
*   +95.5   center quartz                     -1.775 +1.275
*   +88     center freon                      -2.525 +1.275
*   +81     center neoceram                   -3.225 +1.275
*   +71     honeycomb ends   with alu cover   -4.225
*   +38     center honeycomb                  -7.525
*   +5      honeycomb starts with alu cover   -10.825
*   -11.325 is the bottom of the detector
*
*    position of the radiator is 433/2 = 21.65
*
*    four csi cathodes 640*393 with a frame of 21 
*    xpos  393/2 + 21/2 + 9.5 = 21.65
*    zpos  640/2 + 21/2 + 4.5 = 33.5

    material  Methane
    Attribute SRIC seen=1   colo=1
    shape     BOX  dx=42.45 dy=11.325  dz=66.15
    USE       RMAT  Mate='META'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)

    Create and Position ALUM              y=-10.825
    Create and Position HONE              y=-7.525
    Create and Position ALUM              y=-4.225
    Create and Position OQUA  x=-21.65    y=-3.225+1.275
    Create and Position OQUA  x=21.65     y=-3.225+1.275
    Create and Position OQUF  x=-21.65    y=-2.525+1.275
    Create and Position OQUF  x=21.65     y=-2.525+1.275
    Create and Position QUAR  x=-21.6     y=-1.775+1.275
    Create and Position QUAR  x=21.6      y=-1.775+1.275

*    It turns out that we are working in a methane volume in the
*    first place so no Methane here ..
*    Create and Position META              y= 3.100

    Create and Position rGAP              y= 7.550
    Create and Position ALMF              y= 9.350  
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block ALUM is an Aluminum sheet

* flush in sric
* thickness 0.5 mm ???

    Material  Aluminium
    Attribute ALUM  seen=1  colo=5 
    shape     BOX   dx=42.45 dy=0.025 dz=66.15
    USE       RMAT  Mate='GRI'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block ALMF is an Aluminum sheet

* flush in sric a four hole aluminum frame
* thickness 32mm

    Material  Aluminium
    Attribute ALMF  seen=1  colo=5 
    shape     BOX   dx=42.45 dy=1.6 dz=66.15
    USE       RMAT  Mate='GRI'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)

    Create and Position HOLE    x=-21.65  y= 0.0   z=-33.5
    Create and Position HOLE    x=21.65   y= 0.0   z=-33.5
    Create and Position HOLE    x=-21.65  y= 0.0   z=33.5
    Create and Position HOLE    x=21.65   y= 0.0   z=33.5
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
block HONE is a CARBONIO

*  flush in sric
*  thickness  104-33-5 = 66 / 2 = 3.3 but 0.188 is the equivalent to carbon

    material  Carbonio
    Attribute HONE  seen=1   colo=6
    shape     BOX   dx=42.45  dy=0.188  dz=66.15
*   use methane properties intead of Carbon ?
    USE       RMAT  Mate='META'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
block OQUA e me scelto per labellare il quarzo opaco

* 1330 /2 = 66.5 bigger than inside box 
* 413  /2 = 20.65
* thickness 4 / 2 = 0.2
* position width 433

    material  opaco  
    medium    Opaco_quartz    Isvol=Isopt  
    Attribute OQUA   seen=1   colo=2
    shape     BOX    dx=20.65 dy=0.2   dz=66.5
    USE       RMAT   Mate='QRZO'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)

    if (isopt>0) then
    HITS  OQUA x:.01:   y:.01:   z:.01:,
               cx:10:   cy:10:   cz:10:,
               Slen:.1:(0,500)   Lptot:18:(-3,2),
               Tof:16:(0,1.e-6)  LGAM:16:(-2,2),
               Step:16:(0,10)    Eloss:16:(0,0.001) 
    endif  
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block QUAR da me scelto per labellare il quarzo

*   A big piece consisting out of 3 tiles connected with opaco glue
*   actual tile size is
*   443.2 * 3 = 1329.6 that's 0.4 less than the 133, 2 gaps of 0.2/2=0.1
*     gaps are barr, positioned at 443.2/2+0.1 away from center 221.7
*   413   /2 =  20.65

    material  quarz
    medium    quartz          Isvol=Isopt  
    Attribute QUAR  seen=1    colo=2
    shape     BOX   dx=20.65  dy=0.25  dz=66.5
    USE       RMAT  mate='QARZ'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)

    Create and Position BARR     z =-21.7
    Create and Position BARR     z = 21.7
    if (isopt>0) then
    HITS  QUAR x:.01:   y:.01:   z:.01:,
               cx:10:   cy:10:   cz:10:,
               Slen:.1:(0,500)   Lptot:18:(-3,2),
               Tof:16:(0,1.e-6)  LGAM:16:(-2,2),
               Step:16:(0,10)    Eloss:16:(0,0.001) 
    endif  
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block BARR e Barrette quarzo opaco

* width in z  0.2/2=0.1
* 413   /2 =  20.65

    material  opaco
    Attribute BARR  seen=1   colo=2
    shape     BOX   dx=20.65  dy=0.25  dz=0.1
    USE       RMAT   Mate='QRZO'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - -
*block META is METANO
* long dimension 146.3 - 2*70 = 132.3 / 2 = 66.15
* short dimension  989 - 2*70 =  84.9 / 2 = 42.45
* thickness is 92.5/2 =46.25
*
*    material  Methane
*    Attribute META  seen=1   colo=4
*    shape     BOX   dx=42.45  dy=4.625   dz=66.15 
*    USE       RMAT  Mate='META'
*    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)
*endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - -
block rGAP is METANOL gap

* long dimension 146.3 - 2*70 = 132.3 / 2 = 66.15
* short dimension  989 - 2*70 =  84.9 / 2 = 42.45

    material  Methane       
    Medium    Methane_Gap    Isvol=1
    Attribute RGAP  seen=1   colo=4
    shape     BOX   dx=42.45  dy=0.2   dz=66.15
*   use methane properties instead of methanol ?
    USE       RMAT  Mate='META'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)

    HITS rGAP  x:.01:   y:.01:   z:.01:,
               cx:10:   cy:10:   cz:10:,
               Slen:.1:(0,500)   Lptot:18:(-3,2),
               Tof:16:(0,1.e-6)  LGAM:16:(-2,2),
               Step:16:(0,10)    Eloss:16:(0,0.001) 
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
block HOLE is an oppening in the aluminum frame 
    material  Air
    Attribute HOLE seen=1   colo=4
    shape     BOX  dx=19.65 dy=1.6  dz=32.00
*   use methane properties instead of air ?
    USE       RMAT  Mate='META'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)

    Create and Position BORD    x=0  y= -1.35  z=0
    Create and Position rCSI    x=0  y= -1.575  z=0
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block BORD is carbon 
    Material  Carbonio 
    Attribute BORD seen=1   colo=6 
    shape     BOX   dx=19.65  dy=0.2   dz=32.0
    USE       RMAT  Mate='GRI'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block rCSI is Cesium Iodide

* changed to four blocks
* frame size of holder is 72 each side
* frame along long dimension is 4.5
* frame along short dimension is 9.5
* 1463 - 72 - 72 - 21 = 1298 / 2 = 649 - 2*4.5 = 640
* long dimension  640 /2 = 32.0 this is 80 pads
* 1320 mm is 165 pads 5 empty in the center offset in z = 66.0
*
*  989  - 72 - 72 - 21 = 824 / 2 = 412 - 2*9.5 = 393
* short dimension (393-2*0.5) /2 = 19.60 this is 49 pads 
*  392*2+30 = 814 /2 =  (40.7 + 0.1) * 2 = 816 is 102 pads 3 empty offest 40.8

*   call  AgSSTEP(RGJKSTEP) 
    Material  Carbonio
    Medium    CSI   Isvol=1
    Attribute RCSI  seen=1   colo=6 
    shape     BOX   dx=19.60  dy=0.025   dz=32.0
*   refraction index same as in methane ?
    USE       RMAT  Mate='CSI'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)

    HITS rCSI  x:.005:   y:.01:   z:.005:,
               cx:10:    cy:10:   cz:10:,
               Slen:.1:(0,500)    Lptot:18:(-3,2),
               Tof:16:(0,1.e-6)   LGAM:16:(-2,2),
               Step:16:(0,10)     USER:32:(-0.01,0.01) 
endblock
*- - - - - - - - - - - - - - - - - - - - - - - - - - -
block OQUF is FRAME QUARZO OPACO

* 1330 /2 = 66.5 bigger than inside box 
* 413  /2 = 20.65
* thickness 10 / 2 = 0.5
* positioning width 433

    material  opaco 
    Attribute OQUF  seen=1    colo=2
    shape     BOX   dx=20.65  dy=0.5   dz=66.5
    USE       RMAT  Mate='QRZO'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)

    Create and Position FREO
endblock 
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
block FREO is FREON

* inside dimensions
* 1310 /2 = 65.5
* 413 of the frame - 2*0.5 = 403  /2 = 20.15

    component C     A=12  Z=6  W=6
    component F     A=19  Z=9  W=14
    mixture   freon DENS=1.7   Isvol=Isopt

    attribute FREO  seen=1    colo=3
    shape     BOX   dx=20.15  dy=0.5   dz=65.5
    USE       RMAT  mate='FREO'
    CALL GSCKOV(%Imed,N,rang_PPCKOV,Rmat_ABSCO,Rmat_EFFIC,Rmat_RINDEX)

    do i = 1,9
       Create and Position SPAC  x=+6.7 z=(5-i)*14.4 ort=zxy
       Create and Position SPAC  x=-6.7 z=(5-i)*14.4 ort=zxy
    enddo

    if (Isopt>0) then
    HITS FREO  x:.01:   y:.01:   z:.01:,
               cx:10:   cy:10:   cz:10:,
               Slen:.1:(0,500)   Lptot:18:(-3,2),
               Tof:16:(0,1.e-6)  LGAM:16:(-2,2),
               Step:16:(0,10)    Eloss:16:(0,0.001) 
    endif  
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
Block SPAC e Spacers (quarz cylinders)

* 9 spacers center is z=0
* spaced 144
* distance from the middle 134.0
* 1 cm cylinders 

    material  QUARZ
    attribute SPAC seen=1 colo=2
    shape TUBE rmin=0 Rmax=.5 dz=0.5
    USE   RMAT mate='QARZ'
    CALL  GSCKOV(%Imed,N,rang_PPCKOV,rmat_ABSCO,rmat_EFFIC,rmat_RINDEX)
endblock
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*Block awire 
* inside dimensions
* long dimension 146.3 - 2*70 = 132.3 / 2 = 66.15
* anode wires 20 microns 4 millimeter pitch
*
*    material  anode
*    attribute SPAC seen=1 colo=2
*    shape TUBE rmin=0 Rmax=0.0020 dy= 66.15
*endblock

end

* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

      subroutine RICHSTEP
* discard cerenkov photon absorption hits everywhere except for CSI
+CDE,TYPING,GCBANK,GCONST,GCUNIT,GCTMED,GCTRAK,GCKINE,GCSETS,AGCSTEP.
      character  Cmed*8

      check ISVOL>0 
      CALL UHTOC(NATMED,4,Cmed,8)
      check Cmed!='RICH_CSI'

* cerenkov photons are seen in CSI only
* secondaries are not interesting ???
      check Ipart==50  "| Istak>0"
           
* discard hit:
      {Step,dEstep,aStep,AdEstep} = 0; 
*
      end


      subroutine RCSISTEP(JJ,HIT)
+CDE,TYPING,GCBANK,GCONST,GCUNIT,GCTRAK,GCKINE,GCSETS,AGCSTEP.
      Integer JJ
      Real    HIT

      if Ipart != 50 { hit=AdEStep; return }
      hit = -AdEStep
      end 
      
 
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%OLD STUFF - NOT USED NOW%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
      subroutine  rgjkstep
+CDE,gckine,gctrak,gctmed,agcstep.

      common /gjk/ptot_old,prime_part,prime_pt
      
      real ptot_old 
      real prime_part
      real prime_pt
      real prime_vert
            
*     if(numed.ne.21)return
      if(numed.ne.35)return
      
      
      
c     print*,'----------------------------------------------------'
c     print*,'<RGJKSTEP>',nstep,ipart,numed,destep,ilosl
c     print*,'destep',destep
      
      if(ipart.eq.8.or.ipart.eq.11)then
         prime_part = ipart
         prime_pt   = sqrt(pvert(1)**2+pvert(2)**2)
         prime_vert = sqrt(vert(1)**2+vert(2)**2+vert(3)**2) 
         
c     print*,'primary',id,pvert(1),pvert(2),pvert(3)
c     write(42,*)ipart,pvert(1),pvert(2),pvert(3)
c     write(42,*)prime_part,prime_pt
         
         ptot_old=vect(7)      
      endif
      
c     print*,'<<RGJKSTEP>>  ',
c     +     vect(1),vect(2),vect(3),' id ',ipart
      
      
      if(ipart.eq.50.and.(ptot_old.ne.vect(7).or.destep.gt.0))then
         
c     write(42,*)prime_part,prime_pt,prime_vert,
c     +           vect(1),vect(2),vect(3),destep

         
         if(destep.gt.0)then
            call hfill(400,vect(3)+66.0,vect(1)+40.8,1.0)
            write(42,*)ipart,vert(1),vert(2),vert(3),
     +           pvert(1),pvert(2),pvert(3),
     +           vect(4)*vect(7),vect(5)*vect(7),vect(6)*vect(7)
            id=50
         endif
         if(destep.eq.0)then
            call hfill(401,vect(3)+66.0,vect(1)+40.8,1.0)
            id=51
         endif
c         write(43,*)id,vert(1),vert(2),vert(3),
c     +           pvert(1),pvert(2),pvert(3),
c     +           vect(4)*vect(7),vect(5)*vect(7),vect(6)*vect(7)     
      endif
      
      ptot_old = vect(7)
      
c     print*,'----------------------------------------------------'
      end
      
      subroutine  RGAPSTEP
      +CDE,gckine,gctrak,gctmed,agcstep.
      
      common/coordimpat/vect_new(2),prime_part,psi,prime_mom
      
      
c     print*,'<RGAP>',nstep,ipart,numed,destep,ilosl
c     print*,'destep',destep
      
      vect_new(1) = vect(3)     ! long dimension is x in geant z the beam axis
      vect_new(2) = vect(1)     ! short dimensions is y in geant x the bending
      
      prime_part = ipart
      prime_mom = vect(7)
      
c     print*,'<<RGAPSTEP>> ',
c     +     vect(1),vect(2),vect(3)
      
      if((abs(vect(3)).le.2.).and.(abs(vect(1)).le.1.5))then ! this is the inactive
                                !  area in the center
      else
         if(ipart.gt.1.and.ipart.lt.50)then
c     print*,'<<RGAPSTEP>> minimum ionizing charged particle'
            call integration
         endif
         
         
         if(ipart.ge.1.and.ipart.le.20)then
c            print*,vert(1),vert(2),vert(3)
            if(vert(1)*vert(1)+vert(2)*vert(2)+vert(3)*vert(3).le.1)then
               id=200+ipart     ! primary
c               print*,'<RGAP> primary',ipart,vect(3)+66.0,vect(1)+40.8
            else
               id=300+ipart     ! secondary
c               print*,'<RGAP> secondary',ipart,vect(3)+66.0,vect(1)+40.8
            endif
            write(42,*)id,vert(1),vert(2),vert(3),
     +           pvert(1),pvert(2),pvert(3),
     +           vect(4)*vect(7),vect(5)*vect(7),vect(6)*vect(7)       
c            print*,'<RGAP>',id,ipart,vect(3)+66.0,vect(1)+40.8
            call hfill(id,vect(3)+66.0,vect(1)+40.8,1.0)
         endif
         
      endif
      end

      subroutine OQUASTEP
+CDE,gckine,gctrak,gctmed,agcstep.
      character*75 fname
      fname='/home/star/horsley/C_programs/Analytical/' //
     +      'analytical_classes/data/track.data'

c       character*50 fname
c       fname='/home/star/horsley/C_programs/Pid/' //
c     +      '/data/track.data'


      
c      if(ignext.eq.1.and.ipart.ge.8.and.ipart.le.20
c     +   .and.vert(1).eq.0.0.and.vert(2).eq.0.and.vert(3).eq.0.0 )then
      if(ignext.eq.1.and.ipart.gt.7) then
         open(unit=11,file=fname)
         print*,'just about to write out file 11'
         print*,ipart,vect(4)*vect(7),vect(5)*vect(7),
     +         vect(6)*vect(7),vect(1),vect(3)     
         write(11,*) ipart,vect(4)*vect(7),vect(5)*vect(7),
     +               vect(6)*vect(7),vect(1),vect(3) 
         close(11)
      endif
      end
      

      
      subroutine  RCSISTEP
+CDE,gckine,gctrak,gctmed,agcstep.
      common/coordimpat/vect_new(2),prime_part,psi,prime_mom
      character*76 fname1
      fname1='/home/star/horsley/C_programs/Analytical/' //
     +      'analytical_classes/data/photon.data'

c      character*51 fname1
c      fname1='/home/star/horsley/C_programs/Pid/' //
c     +      '/data/photon.data'

      
c     print*,'<RCSI>',nstep,ipart,numed,destep,ilosl
c     print*,'destep',destep
      
      vect_new(1) = vect(3)     ! long dimension is x in geant z the beam axis
      vect_new(2) = vect(1)     ! short dimensions is y in geant x the bending
      prime_part = ipart
      prime_mom = vect(7)
      

      
      
      if(ipart.eq.50)then
c         print*,'<<RCSISTEP>> cherenkov light'
         
c        write out list of photons impact points         
         if(ipart.eq.50.and.destep.gt.0)then
            open(unit=17,file=fname1,status='unknown',access='append')
c            open(unit=18,file='/data/matt/quartz/q.ntp',
c     +           status='unknown',access='append')

            opx = vect(4)*vect(7)
            opy = vect(5)*vect(7)
            opz = vect(6)*vect(7)
c            print*, vect(3),vect(1),
c     +                  1240./((sqrt(opy*opy+opx*opx+opz*opz))*(10.**9))

       
            write(17,*) vect(3),vect(1),
     +                  1240./((sqrt(opy*opy+opx*opx+opz*opz))*(10.**9))
            
            print*, vert(2)
c            write(18,*) vert(1),vert(2),vert(3),
c     +                 1240./((sqrt(opy*opy+opx*opx+opz*opz))*(10.**9)),
c     +                 45
c     print*,'just wrote out photons z,x, wavelenght to ftn26'
            close(17)
            close(18)
         endif
         
         call integration
         
      endif
      if(ipart.eq.1)then
         print*,'<<RCSISTEP>> gamma light'
         call integration
      endif
      
      end
      
c-------------------------------------------------------
      SUBROUTINE INTEGRATION
+CDE,gckine,gctrak,gctmed,agcstep.
      
      
      common/coordimpat/vect_new(2),prime_part,psi,prime_mom
      
      common / paw_pads / x_pd,y_pd,qp,ipp,psii,ptot
      common / npad / ntpad
      
      common/coord/x_pad(1000),y_pad(1000),np,alcs 
      common/anodi/y_anodo(1000)
      common/xfyf/xf,yf
      parameter (sigma = 0.15)
      common/seed/seed
      integer * 4 seed
      
      integer ifl               ! gjk
      data    ifl /0/
      
c     print*,'<<INTEGRATION>> for particle',ipart
      
      alcs = 132.0              ! longest dimension of the RICH module
      
      if(ipart.eq.50.or.ipart.eq.1)call get_charge(qtot)
      if(ipart.ne.50)call get_charge_mip(qtot)
      
      if(ifl.eq.0)then
         call pad               ! initialize pads
         ifl = 1
c         print*,'initialize pad'
      endif
      
      x0 = vect_new(1) + 66.0   ! no offset here
      y0 = vect_new(2) + 40.8 
      
c     x0 = vect_new(1)
c     y0 = vect_new(2)
      
      
      call fili_anodici(y0a)
      
      do ix=1,np-1
         do iy=1,np-1
            xi_1=(x_pad(ix+1)-x0)/sigma ! lim. di integrazione in x
            xi=(x_pad(ix)-x0)/sigma
            
            yi_1=(y_pad(iy+1)-y0a)/sigma ! lim. di integrazione in y
            yi=(y_pad(iy)-y0a)/sigma
            
            if(xi.gt.-10.and.xi.lt.10.and.yi.gt.-10.and.yi.lt.10.)then
c     print*,'erf(xi_1),erf(xi),erf(yi_1),erf(yi)'
c     print*,xi_1,xi,yi_1,yi
c     print*,erf(xi_1),erf(xi),erf(yi_1),erf(yi)
               qp= qtot/4.*(erf(xi_1)-erf(xi))*(erf(yi_1)-erf(yi)) ! ris.integrale 
c     print*,'qtot,qp',qtot,qp
               
               if(qp.gt.0.0)then
c     print*,'<<hfill 100>>',x_pad(ix),y_pad(iy),qp
                  call hfill(100,x_pad(ix),y_pad(iy),qp) !  histo
                  id=100+ipart
               endif
               
                             
               if(ipart.eq.50.or.ipart.eq.1)then
                  if(qp.gt.0.05)ntpad = ntpad + 1
               endif
               
c     x_pd = x_pad(ix)
c     y_pd = y_pad(iy)
c     ipp = prime_part
c     ptot = prime_mom
c     psii = psi
c     print*,'hfnt(20)'
c     call hfnt(20)
            endif
            
         enddo
      enddo
      
      return
      end
c-----------------------------------------------------
      

C------------------------------------------------------
      SUBROUTINE get_charge(qtot)
      
      common/seed/seed
      integer * 4 seed
      
      real random_array(2)      ! gjk
      
      
c      print*,'<<get_charge(qtot)>>'
      
      call grndm(random_array,1) ! gjk
      
      prob= random_array(1)     ! gjk
      qtot=-(1.*(alog(1.-prob))) ! gjk
      
c     qtot=-(1.*(alog(1.-ran(seed))))  ! gjk
      
c      print*,'<<get_charge>>',qtot
      
      return
      end
      
C------------------------------------------------------
      SUBROUTINE get_charge_mip(qtot)
+CDE,gckine,gctrak,gctmed,agcstep.     
      
      integer nmip/0/
      
      real random_array(2)      ! gjk 
      
      
c      print*,'<<get_charge_mip(qtot)>>'
      
      nmip=nmip+1
c      print*,'-----------------------------------------------------------'
c      print*,' '
c      print*,' '
c      print*,' '
c      print*,' '
c      print*,'Mip # ',nmip
c      print*,' '
c      print*,' '
c      print*,' '
c      print*,' '
c      print*,'-----------------------------------------------------------'
      
      n = nint(destep * 1.e9 / 30.) ! number of electrons
      
c      print*,'<get_charge_mip>',destep,n
      
      qtot = 0.
      do i = 1,n	
         call grndm(random_array,1) ! gjk
         
         prob= random_array(1)  ! gjk
         qtot=-(1.*(alog(1.-prob))) ! gjk
         
c     qtot = qtot - (1.*(alog(1.-ran(seed)))) ! gjk
      enddo
      
c      print*,'<<get_charge_mip>>',qtot
      
      return
      end
      
c--------------------------------------------------
      SUBROUTINE PAD
      
      COMMON/anodi/y_anodo(1000)
      common/coord/x_pad(1000),y_pad(1000),np,alcs
      parameter (dx=0.8,dy=0.8) ! .. dx e dy dimensioni pad
      parameter (ansp=0.4)      ! ansp=interspazio anodico
      
c      print*,'<<PAD>>'
      
      x_pad(1) = 0.+dx/2.
      y_pad(1) = 0.+dx/2.
      np=int(alcs/dx)
      
c      print*,'Number of x pads is ',np
      
      do i=1,np
         x_pad(i)=(i-1)*dx+dx/2.
         y_pad(i)=(i-1)*dx+dy/2.
c      print *,'  x_pad(i),y_pad(i) ',i,x_pad(i),y_pad(i)
      enddo
      
      do i=1,2*np+1
         y_anodo(i)=(i-1)*ansp+ansp/2.
      enddo
      
      return
      end
      
c------------------------------------------------------
      SUBROUTINE FILI_ANODICI(y0a)
      
      COMMON/anodi/y_anodo(1000)
      common/coordimpat/vect_new(2),prime_part,psi,prime_mom
      
c     print*,'<<FILI_ANODICI(y0a)>>'
      
      
      y0 = vect_new(2) + 40.8
c     y0 = vect_new(2) 

      if(y0.le.y_anodo(1))then

      y0a=y_anodo(1)                ! catch the first wire

      else
         
      do i=1,205
         if(y0.gt.y_anodo(i).and.y0.le.y_anodo(i+1))then
	    ass_i1=abs(y_anodo(i+1)-y0)
	    ass_i=abs(y_anodo(i)-y0)
	    if(ass_i1.le.ass_i)y0a = y_anodo(i+1)
	    if(ass_i1.gt.ass_i)y0a = y_anodo(i)
	    goto 999
         endif
      enddo
      
      print*,'<FILI_ANODICI> could not find wire ',i,y0       

      endif
      
 999  continue
c     print*,'<<FILI_ANODICI>>',y0a
      
      
      return
      end
      
c-----------------------------------------------------------------------------
     
