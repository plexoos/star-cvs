* $Id: igtdgeo.g,v 1.6 2007/11/13 21:30:34 perev Exp $
* $Log: igtdgeo.g,v $
* Revision 1.6  2007/11/13 21:30:34  perev
* material ALKAP fixed
*
* Revision 1.5  2006/11/14 00:18:52  potekhin
* Allow for a modified set of parameters as per recent change
* in the IGT geometry (Ross, Bernd)
*
* Revision 1.4  2006/10/21 18:16:35  potekhin
* Add 'MANY' option to ensure correct material
* taken into account when working with the new support structure
*
* Revision 1.3  2006/10/06 23:07:02  potekhin
* An extraneous Denstity=1.7 in one of the G10
* references caused and extr material to be created,
* which is in fact a bug. Fixed that.
*
* Revision 1.2  2006/06/26 17:51:33  potekhin
* Added a comment to clarify the configuration of the detector:
* there are currently a fwe GEM devices described, so we need
* to add some clarity to their denomination
*
* Revision 1.1  2006/05/05 17:22:12  potekhin
* The first cut of the forward GEM disks ("smaller
* aperture design as compared to fgt") by Doug Hasell.
* Redundant code removed, and other cosmetic changes
* by Maxim.
*
*
* IGTDGEO - Inner GEM Tracking Detector Geometry
*
* Describes four disks of triple GEM detectors for a possible STAR upgrade
* Distinct from, and alternative to, the previosuly proposed
* LAGRGE GEM surfaces
*
* Inner and outer radii as well as position along the Z axis are variable
* parameters as well as the geometry, foil thicknesses, and spacing of the 
* triple GEM detectors.
*
* First design 2006/05/01 - D.K. Hasell
*
******************************************************************************
Module IGTDGEO is the geometry of the inner GEM tracking detector
  Created  5/1/06
  Author   Douglas Hasell
******************************************************************************
+CDE,AGECOM,GCUNIT.

* Declare variables used in the code.

      real center                  ! centre of complete assembly along Z axis
      real length                  ! length of complete assembly along Z axis
      real thick                   ! thickness in Z of a single GEM disk
      real rmn                     ! used to determine the minimum radius
      real rmx                     ! used to determine the maximum radius
      real rin                     ! inner radius for each GEM detector
      real rout                    ! outer radius for each GEM detector
      real zsum                    ! accumulates Z position for layers in GEM
      real thk                     ! half thickness of layer in each GEM

      integer disk                 ! index for counting over the GEM disks
      integer layer                ! index fro counting over the layers in GEM

* Declare the blocks to be defined in the code.
      Content   IGMO, IGDO, IGFO, IGIS, IGOS, IGRL, IGAL

      Structure IGTV {version,  int Config}

* Declare the structure IGTG which defines the detector geometry.
      Structure IGTG { Config, RI(4), RO(4), Zstart, Z(4), FThk(4), SThk(4), SR, RThk }

* -----------------------------------------------------------------------------

   Fill IGTV    !  IGT geometry version
      version    =  1    ! geometry version - dummy
      Config =  1    ! config
   EndFill

   Fill IGTG                   ! Inner GEM Tracker Geometry data
      Config  =  1                            ! Version
      RI      = { 15.0, 20.0, 25.0, 30.0 }    ! inner radii for each GEM
      RO      = { 45.0, 45.0, 45.0, 45.0 }    ! outer radii for each GEM
      Zstart  = 55.0                          ! starting position along Z axis
      Z       = { 0.0, 30.0, 60.0, 90.0}      ! Z positions for GEM front face
      FThk    = { 0.05, 0.05, 0.05, 0.05 }    ! foil thicknesses inside GEM
      SThk    = { 0.4, 0.3, 0.3, 0.4 }        ! support/spacing thicknesses
      SR      = 1.0                           ! radial size for support
      RThk    = 0.3                           ! readout plane thickness
   EndFill

   Fill IGTG                   ! Inner GEM Tracker Geometry data
      Config  =  2                            ! Version
      RI      = { 15.0, 20.0, 25.0, 30.0 }    ! inner radii for each GEM. 
      RO      = { 43.0, 43.0, 43.0, 43.0 }    ! outer radii for each GEM
      Zstart  = 58.0                          ! starting position along Z axis. rcc 55.0 originally
      Z       = { 0.0, 30.0, 60.0, 90.0}      ! Z positions for GEM front face
      FThk    = { 0.05, 0.05, 0.05, 0.05 }    ! foil thicknesses inside GEM
      SThk    = { 0.4, 0.3, 0.3, 0.4 }        ! support/spacing thicknesses
      SR      = 1.0                           ! radial size for support
      RThk    = 0.3                           ! readout plane thickness
   EndFill

*******************************************************************************

* Calculate some parameters from the input variables.

      USE IGTV
      USE IGTG  config=IGTV_Config;
      write(*,*) 'IGTD Version:',IGTV_Config

* use aluminized mylar mixture instead of kapton
      Component C5  A=12    Z=6  W=5
      Component H4  A=1     Z=1  W=4
      Component O2  A=16    Z=8  W=2
      Component Al  A=27    Z=13 W=0.2302
      Mixture  ALKAP  Dens=1.432
*     G10 is about 60% SiO2 and 40% epoxy
      Component Si  A=28.08  Z=14   W=0.6*1*28./60.
      Component O   A=16     Z=8    W=0.6*2*16./60.
      Component C   A=12     Z=6    W=0.4*8*12./174.
      Component H   A=1      Z=1    W=0.4*14*1./174.
      Component O   A=16     Z=8    W=0.4*4*16./174.
      Mixture   G10    Dens=1.7


* Loop over the layers in a GEM detector and accumulate the total thickness.

      thick = 0.0

      do layer = 1, 4
         thick = thick + IGTG_FThk(layer) + IGTG_SThk(layer)
      enddo

      thick = thick + IGTG_RThk

* Loop over the GEM disks and determine the min and max radius and max Z.

      rmn = 10000.0
      rmx = - rmn
      length = 0.0

      do disk = 1, 4

         if( rmn .gt. IGTG_RI(disk) ) then
            rmn = IGTG_RI(disk)
         endif

         if( rmx .lt. IGTG_RO(disk) ) then
            rmx = IGTG_RO(disk)
         endif

         if( length .lt. IGTG_Z(disk) ) then
            length = IGTG_Z(disk)
         endif
      enddo

      length = length + thick

* Calculate centre of assembly.

      center = IGTG_Zstart + length / 2.0

      Create   IGMO
      Position IGMO in CAVE z=+center kOnly='MANY'

* -----------------------------------------------------------------------------
Block IGMO is the mother volume for the IGTD
      Material  Air
      Attribute IGMO  Seen=1  colo=6

      Shape     TUBE Rmin=rmn Rmax=rmx Dz=length/2.0

* Loop over the four GEM disks.

      do disk=1,4
         rin  = IGTG_RI(disk)
         rout = IGTG_RO(disk)
         Create and Position IGDO z=-length/2.0 + thick/2.0 + IGTG_Z(disk)
      enddo

endblock

* -----------------------------------------------------------------------------
Block IGDO is the mother volume of the individual GEM disks
      Component Ar A=39.95   Z=18.   W=0.9
      Component C  A=12.01   Z=6.    W=0.1*1*12.01/44.01
      Component O  A=16.     Z=8.    W=0.1*2*16./44.01
      Mixture   Ar_mix  Dens=0.0018015

      Attribute IGDO  Seen=0  colo=6

      Shape TUBE Rmin=rin Rmax=rout Dz=thick/2.0

* Loop over the layers inside the GEM.

      zsum = -thick/2.0                 ! Start at the front face.

      do layer = 1, 4

* Create GEM foil.

         thk = IGTG_FThk(layer)/2.0     ! half thickness of foil
         zsum = zsum + thk              ! position of foil centre

         Create and Position IGFO z=zsum 

         zsum = zsum + thk              ! position of foil rear face

* Create inner and outer spacer/support.

         thk = IGTG_SThk(layer)/2.0     ! half thickness of spacer
         zsum = zsum + thk              ! position of spacer centre

         Create and Position IGIS z=zsum 
         Create and Position IGOS z=zsum 

* First gap is active volume -> create it.

         if( layer .eq. 1 ) then
            Create and Position IGAL z=zsum
         endif

         zsum = zsum + thk              ! position of spacer rear face

      enddo

* Create readout layer as last layer.

      thk = IGTG_RThk/2.0               ! half thickness of readout layer
      zsum = zsum + thk                 ! position of readout layer centre

      Create and Position IGRL z=zsum

endblock

* -----------------------------------------------------------------------------
Block IGFO describes the GEM foils
      Material ALKAP
      Attribute IGFO  Seen=1 colo=4

      Shape TUBE Rmin=rin Rmax=rout Dz=thk

endblock

* -----------------------------------------------------------------------------
Block IGIS describes the inner support or spacer
      Material G10
      Attribute IGIS  Seen=1  colo=3

      Shape TUBE Rmin=rin Rmax=rin+IGTG_SR Dz=thk

endblock

* -----------------------------------------------------------------------------
Block IGOS describes the outer support or spacer
      Material G10
      Attribute IGOS  Seen=1  colo=3

      Shape TUBE Rmin=rout-IGTG_SR Rmax=rout Dz=thk

endblock

* -----------------------------------------------------------------------------
Block IGRL describes the readout layer
      Material G10
      Attribute IGRL  Seen=1  colo=3

      Shape TUBE Rmin=rin Rmax=rout Dz=thk

endblock

* -----------------------------------------------------------------------------
Block IGAL describes the active area
      Component Ar A=39.95   Z=18.   W=0.9
      Component C  A=12.01   Z=6.    W=0.1*1*12.01/44.01
      Component O  A=16.     Z=8.    W=0.1*2*16./44.01
      Mixture   Ar_mix  Dens=0.0018015 Isvol=1

      Material  Sensitive  Isvol=1

      Attribute IGAL  Seen=1 colo=6

      Shape TUBE Rmin=rin+IGTG_SR Rmax=rout-IGTG_SR Dz=IGTG_SThk(1)/2.0

      HITS    IGAL   Z:.001:S  Y:.001:   X:.001:     Ptot:16:(0,100),
                     cx:10:    cy:10:    cz:10:      Sleng:16:(0,500),
                     ToF:16:(0,1.e-6)    Step:.01:   Eloss:16:(0,0.001) 

endblock

* -----------------------------------------------------------------------------

      END

