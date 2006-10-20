* $Id: fstdgeo.g,v 1.11 2006/10/20 19:24:47 potekhin Exp $
* $Log: fstdgeo.g,v $
* Revision 1.11  2006/10/20 19:24:47  potekhin
* Korrekt a tipo
*
* Revision 1.10  2006/10/20 19:15:18  potekhin
* Added the 'MANY' option to the positioning (necessary due
* to overlaps in the upgrade geometries) and made the mother
* volume visible (to aid in resolution of same)
*
* Revision 1.9  2005/02/11 20:26:48  nieuwhzs
* Moved the silicon sensors to their proper positions, i.e. flush with the
* front faces of the hybrids.
* Adjusted the size of the readout chip 'strip' to be more realistic.
* Added second cooling channel on the backside of the modules and adjusted
* mother volume size to accomodate this second cooling channel. This make the
* modules symmetric.
* Adjusted the size of the FST mother volume so that the 4 disks can be placed
* in their proper position in the 'CAVE' volume. The 'center' of the disks is
* assumed to be exactly halway between the silicon sensors.
* +++Gerrit van Nieuwenhuizen, 01/11/05
*
* Revision 1.6  2005/02/02 22:07:28  potekhin
* Changed the sensor color as per Gerrit's request
*
* Revision 1.5  2005/01/26 01:12:46  potekhin
* Removed active and passive layers, corrected nesting of
* the water duct volume
*
* Revision 1.4  2005/01/25 22:43:36  potekhin
* Added the water manifold (duct). Corrected a small
* error in the size of AlN plate. Added HITS (to be further devloped)
*
* Revision 1.3  2005/01/06 02:01:09  potekhin
* Substantial reorganization, better AlN description
*
* Revision 1.2  2005/01/04 23:47:48  potekhin
* A waypoint check-in:
* implemented a full fledged double wafer in each layer,
* and the AlN thermal plate.
*
* Revision 1.1  2004/12/07 00:41:00  potekhin
* We need a directory subtree for the Forward Tracker.
* It has been planned for a long time.
*
*
******************************************************************************
Module FSTDGEO is the geometry of the forward silicon tracker pixel detector
  Created  12/06/04
  Author   Maxim Potekhin
******************************************************************************
+CDE,AGECOM,GCUNIT.

      real center,      depth
      real WedgeOffset, WedgeDx1, WedgeDx2, WedgeLength
      real PlateOffset, PlateDx1, PlateDx2, Offhack
      real SensorOffset,SensorDx1,SensorDx2,SensorLength
      real raddeg,start,angle,    TanHalfAngle

      integer nl, ly

      Content   FSMO, FDMO, FDMS, FDMW, FDSW, FDTP, FDSC, FDWD, FDWW

      Structure FSTG {Layer,    Nsec,      Spacing,
                      RminS,    RmaxS,     Rmin,      Rmax,       Zmin,      Zmax,
                      WedgeThk, SensAThk,  SensorThk, Z(4)}

      Structure FSAN {Version,  Thk,       Length}

      Structure FSCG {Nummer,   W,         H,         Thk}

      Structure FDWG {Version,  Rmin,Rmax, Thk,       Len,        WallThk}
*
* -----------------------------------------------------------------------------
*
   Fill FSTG                   ! Forward Silicon Tracker Geometry data
      Layer      =  1          ! layer index (reserved)
      Nsec       = 21          ! number of sectors in a wheel
      Spacing    =  0.1686     ! spacing between the wafers

      RminS      =  7.0        ! inner radius of sensitive wedge
      RmaxS      = 19.0        ! outer radius of sensitive wedge

      Rmin       =  7.0        ! inner radius of all of the detector
      Rmax       = 22.5        ! outer radius of all of the detector

*GvN First disk is sitting at 280mm, last disk is sitting at 370mm
      Zmin       = 27.4619     ! Z-start  of the barrel comprising the three pancakes
      Zmax       = 37.5381     ! Z-finish of the barrel comprising the three pancakes


      WedgeThk   =  1.0762     ! Includes sensor assembly and water pipes
      SensAThk   =  0.3686     ! Sensor assembly thk: includes two layers of Si, AlN plates and chips
      SensorThk  =  0.030      ! Total   silicon thickness, includes passive and active

      Z          =  {0.0, 3.0, 6.0, 9.0} ! nominal Zs of the faces

   EndFill

   Fill FSAN                   ! Aluminum Nitride Thermal Plate
      Version    =   1         ! May have a few
      Thk        =  0.0762     ! AlN Thickness
      Length     =  3.0        ! AlN length along radius
   EndFill

   Fill FSAN                   ! Aluminum Nitride Thermal Plate
      Version    =   2         ! May have a few
      Thk        =  0.0762     ! AlN Thickness
      Length     =  2.0        ! AlN length along radius
   EndFill


   Fill FSCG                   ! Forward Silicon readout Chip Geometry, first approx.
      Nummer     =   1         ! We can have a few different chips
      W          =   5.4       ! Width
      H          =   0.82      ! Height, APV25 size keeps changing!
      Thk        =   0.07      ! Thickness
   EndFill

   Fill FDWG                   ! Water duct geometry
      Version    =   1         ! Version
      Rmin       =   21.0      ! Inner radius of the duct
      Rmax       =   22.0      ! Outer radius of the duct
      Thk        =    0.5      ! Thickness in Z
      Len        =    6.3      ! Length
      WallThk    =    0.1      ! Wall thickness
   EndFill
*
******************************************************

      USE FSTG
      USE FSAN Version=1
      USE FSCG
*
      raddeg=3.14159265/180.0

      angle=360.0/FSTG_Nsec ! opening angle of the sector
      TanHalfAngle=tan(angle*raddeg/2.0)

      SensorLength= FSTG_RmaxS - FSTG_RminS

      SensorDx1   = FSTG_RminS*TanHalfAngle;    SensorDx2    = FSTG_RmaxS*TanHalfAngle
      WedgeLength = SensorLength+FSAN_Length;   SensorOffset = -0.5*(WedgeLength-SensorLength)

      WedgeDx1    = FSTG_Rmin*TanHalfAngle;     WedgeDx2     = (FSTG_RmaxS+FSAN_Length)*TanHalfAngle
      WedgeOffset = FSTG_Rmin + 0.5*WedgeLength



* Top sector has to be at 12 o'clock: May need Angular Offset --
* Keep this blank for now
      start=0.0

      center=(FSTG_Zmax+FSTG_Zmin)/2.0
      depth =(FSTG_Zmax-FSTG_Zmin)

      Create FSMO
      Position FSMO in CAVE z=+center kOnly='MANY'
* -----------------------------------------------------------------------------
Block FSMO is the mother of one endcap of FSTD
      Material  Air
      Attribute FSMO  Seen=1  colo=6

      Shape     TUBE Rmin=FSTG_Rmin _
                     Rmax=FSTG_Rmax _
                     Dz=depth/2.0

*     Place the 4 disks in the mother volume
      Create    FDMO
      do nl=1,4
       Position FDMO z=-depth/2.0+FSTG_WedgeThk/2.0+FSTG_Z(nl)
      enddo

endblock

* -----------------------------------------------------------------------------
Block FDMO is the mother of an individual two-layer disk assembly (wafers and cooling)
      Material  Air
      Attribute FDMO  Seen=0  colo=6

      Shape TUBE Rmin=FSTG_Rmin _
                 Rmax=FSTG_Rmax _
                 Dz=FSTG_WedgeThk/2.0

      Create FDMS
endblock

*
* -----------------------------------------------------------------------------
Block FDMS is a division within an individual disk

      Shape  Division   Iaxis=2   Ndiv=21 c0=start

* -------------------------------------
* sensor assembly mother
      Create   FDMW
      Position FDMW x=WedgeOffset _
                    y=0.0 _
                    z=0.0 _
                    ORT=YZX

* -------------------------------------
* Water manifold (duct)
      Create   FDWD

      Position FDWD x=FDWG_Rmin+0.5*(FDWG_Rmax-FDWG_Rmin) _
                    y=0 _
                    z=-0.5*(FDWG_Thk+FSAN_Thk) _
                    ORT=YZX
      Position FDWD x=FDWG_Rmin+0.5*(FDWG_Rmax-FDWG_Rmin) _
                    y=0 _
                    z=+0.5*(FDWG_Thk+FSAN_Thk) _
                    ORT=YZX

endblock
* -----------------------------------------------------------------------------
Block FDMW is the mother wedge, housing plate, sensor  and chips
      Attribute FDMW  Seen=0  colo=4

      Shape TRD1 dx1=WedgeDx1 _
                 dx2=WedgeDx2 _
                 dy=FSTG_SensAThk/2.0 _
                 dz=WedgeLength/2.0

* -------------------------------------
* The Silicon Wafer
      Create   FDSW
      Position FDSW x=0.0 _
                    y=-FSTG_Spacing/2.0-FSTG_SensorThk/2.0 _
                    z=SensorOffset
      Position FDSW x=0.0 _
                    y=+FSTG_Spacing/2.0+FSTG_SensorThk/2.0 _
                    z=SensorOffset AlphaZ=180

* -------------------------------------
* ALN thermal plate
      USE FSAN Version=1 ! the center plate

      PlateDx1    = FSTG_RmaxS*TanHalfAngle;    PlateDx2     = (FSTG_RmaxS+FSAN_Length)*TanHalfAngle

      Create   FDTP
      PlateOffset = 0.5*(SensorLength) !   WedgeLength = SensorLength+FSAN_Length
      Position FDTP z=PlateOffset

      Create   FDSC
      Position FDSC x=0.0 _
                    y=+0.5*FSCG_Thk+1.5*FSAN_Thk _
                    z=(0.5*WedgeLength - FSAN_Length+FSCG_H/2.0)
      Position FDSC x=0.0 _
                    y=-0.5*FSCG_Thk-1.5*FSAN_Thk _
                    z=(0.5*WedgeLength - FSAN_Length+FSCG_H/2.0)

* -------------------------------------
* ALN substrate
      USE FSAN Version=2 ! the substrate plate

      PlateDx1    = FSTG_RmaxS*TanHalfAngle;    PlateDx2     = (FSTG_RmaxS+FSAN_Length)*TanHalfAngle

      Create   FDTP
      Offhack=0.5
      PlateOffset    = PlateOffset-Offhack ! Encapsulated hack
      Position FDTP y=+FSAN_Thk z=PlateOffset
      Position FDTP y=-FSAN_Thk z=PlateOffset

endblock
* -----------------------------------------------------------------------------
Block FDSW is the Silicon Wafer (all active)
      Material  Silicon  
      Material  Sensitive  Isvol=1

      Attribute FDSW  Seen=1  colo=4

      Shape TRD1 dx1=SensorDx1 dx2=SensorDx2 dy=FSTG_SensorThk/2.0 dz=SensorLength/2.0

      HITS    FDSW   Z:.001:S  Y:.001:   X:.001:     Ptot:16:(0,100),
                     cx:10:    cy:10:    cz:10:      Sleng:16:(0,500),
                     ToF:16:(0,1.e-6)    Step:.01:   Eloss:16:(0,0.001) 


* I'll leave this for reference, but we no longer have
* passive and active layers in this sensor. It's all active
*      Create and Position FDSA y=-(FSTG_SensorThk-FSTG_ActiveThk)/2.0
*      Create and Position FDSP y=+(FSTG_SensorThk-FSTG_PassiveThk)/2.0

endblock
* -----------------------------------------------------------------------------
Block FDTP is the AlN Thermal Plate
      Attribute FDTP Seen=1  colo=6

      Component Al   A=27  Z=13  W=1
      Component N    A=14  Z=7   W=1
      Mixture   AlN  Dens=3.30

      Shape TRD1 dx1=PlateDx1 _
                 dx2=PlateDx2 _
                 dy=FSAN_Thk/2.0 _
                 dz=FSAN_Length/2.0

endblock
* -----------------------------------------------------------------------------
Block FDSC is the readout Chip
      Material  Silicon  
      Attribute FDSC  Seen=1  colo=1

      Shape BOX dx=FSCG_W/2.0 _
                dy=FSCG_Thk/2.0 _
                dz=FSCG_H/2.0

endblock
* -----------------------------------------------------------------------------
Block FDWD is the water duct made of carbon composite
      Material  Carbon
      Attribute FDWD  Seen=1  colo=1

      Shape BOX dx=FDWG_Len/2.0 _
                dy=FDWG_Thk/2.0 _
                dz=(FDWG_Rmax-FDWG_Rmin)/2.0
      Create and Position FDWW

endblock
* -----------------------------------------------------------------------------
Block FDWW is the water inside the carbon duct
*     Pellegrino:
      Component H2     A=1   Z=1   W=2
      Component O      A=16  Z=8   W=1
      Mixture   Water  Dens=1.0

      Attribute FDWD  Seen=1  colo=3

      Shape BOX dx=FDWG_Len/2.0 _
                dy=(FDWG_Thk-2.0*FDWG_WallThk)/2.0 _
                dz=(FDWG_Rmax-FDWG_Rmin-2.0*FDWG_WallThk)/2.0

endblock
* -----------------------------------------------------------------------------


      END

