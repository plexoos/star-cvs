***************************************************************************
* $Id: hpdtgeo.g,v 1.3 2006/10/21 18:38:03 potekhin Exp $
* $Log: hpdtgeo.g,v $
* Revision 1.3  2006/10/21 18:38:03  potekhin
* Added a small safety margin on Rin, used 'ONLY' option to keep hits
*
* Revision 1.2  2006/10/11 17:52:05  potekhin
* Corrected the angular offset, 60 instead of 70
*
* Revision 1.1  2006/09/15 19:55:10  potekhin
* New pixle detector requires new code.
*
*
* Modified 09/08/06
* the cell size and the support structure with the request of N.Smirnov 
******************************************************************************
Module HPDTGEO is the geometry of the STAR pixel detector
  Created  09/01/06
  Author   Sevil Salur
******************************************************************************
+CDE,AGECOM,GCUNIT.
*
      real angle,anglePos,angleCorr,raddeg, anglesep
      integer nLadder,nSector, nExtraLadder, nWafer, nCell

      Content  YPXM, YPSC, YPLM, YPLP, YPLA, YPWM, YPWP, YPWR
*
      Structure YPXG {Rin, Rout, TotalLength,AcLength, 
*                      WaferLen, PWaferLen,GuardLen, 
                      CellLen, PCellLen, CellGuardLen ,
                      LadderWidth,PassLadderW,LadderThk,CarbonThk,
                      PassiveThk,ActiveThk, ReadOutThk, 
                      r,a,pOffset,aOffset,numberladder,numbercell}
*
* -----------------------------------------------------------------------------
*
   Fill YPXG                   ! Pixel detector data
      Rin        =  8.61       ! Inner radius in x or y 
      Rout       =  11.        ! Outer radius in x or y 
      TotalLength=  49.5       ! Overal length of the detector z direction
      AcLength   =  28.288     ! Active length of the detector z direction
*      WaferLen   =  7.072      ! Active silicon Wafer Length 
*      PWaferLen  =  0.001      ! Spacing between two Wafers 
*      GuardLen =    0.112      ! The space around the Wafer (Frame) 
      CellLen   =  1.36      ! Active silicon Cell Length 
      PCellLen  =  0.001      ! Spacing between two Cells
      CellGuardLen =    0.112      ! The space around the Cells (Frame) 


*
      LadderWidth=  1.28       ! Ladder Width
      PassLadderW=  1.68       ! Passive Ladder Width 
*
      LadderThk  =  0.1780     ! Total ladder Thickness
      PassiveThk =  0.1380     ! Passive mylar Thickness 
      CarbonThk  =  0.056     ! Active  carbon Thickness
      ActiveThk  =  0.0200     ! Active  silicon Thickness
      ReadOutThk =  0.0200     ! Inactive silicon for Readout 
     
*
      r           =  9.1        ! 1st ladder nominal radius
      a           =  3.75       ! 1st ladder nominal position angle
      aOffset     = 60.0       ! Angular offset      
      numberladder= 48         ! Number of ladders   
      numbercell  = 20           ! Number of cells 
*
      pOffset    =  0.0        ! Position offset (shift)
   EndFill


******************************************************
      USE      YPXG  
*
      raddeg=3.14159265/180.0
      angle = YPXG_a
      anglesep= 360/YPXG_numberladder 
      
      Create   YPXM  ! create the object for the position in the cave for the detector
      Position YPXM in CAVE Konly='ONLY'

* -----------------------------------------------------------------------------
Block YPXM is the mother of the pixel detector volumes  !!define each object
      Material  Air
      Attribute YPXM  Seen=1  colo=2

      Shape TUBE Rmin=YPXG_Rin Rmax=YPXG_Rout Dz=YPXG_TotalLength/2.0

      do nLadder=1,YPXG_numberladder  ! Inner loop, create ladders inside the sector
      anglePos = angle*raddeg +  anglesep*(nLadder-1)*raddeg             !  +angleCorr  see above comment

      Create and Position YPLM x=YPXG_r*cos(anglePos) y=YPXG_r*sin(anglePos), 
      z=0.0 AlphaZ=YPXG_aOffset+angle+anglesep*(nLadder-1)
  
   enddo  !!loop ended for the nladder

endblock
* -----------------------------------------------------------------------------
Block YPLM is the mother of the pixel ladder
      Material  Air
      Attribute YPLM   Seen=1  Colo=4
      Shape BOX dX=YPXG_PassLadderW/2.0 dY=YPXG_LadderThk/2.0 Dz=YPXG_TotalLength/2.0
       Create and position YPLP y=-YPXG_LadderThk/2.0+YPXG_PassiveThk/2
       Create and position YPWP y=YPXG_LadderThk/2.0-YPXG_ActiveThk/2-YPXG_ReadOutThk/2
endblock
*-----------------------------------------------------------------------------
Block YPLP is the mother of the support
      Material  Mylar
      Attribute YPLP   Seen=1  Colo=4
      Shape BOX dX=YPXG_PassLadderW/2.0 dY=YPXG_PassiveThk/2.0 Dz=YPXG_TotalLength/2.0
      Create and position YPWR y=YPXG_CarbonThk/2.0-YPXG_PassiveThk/2
endblock

*---------------------------------------------------------------------------------
Block YPWR is the other support
      Material  Carbon
      Attribute YPWR   Seen=1 Colo=4
      Shape BOX dX=YPXG_PassLadderW/2.0 dY=YPXG_CarbonThk/2.0 Dz=YPXG_TotalLength/2.0
  
endblock

*---------------------------------------------------------------------------------
Block YPWP is the all silicon layer active and readout  
      Material  Silicon
      Attribute YPWP   Seen=1  Colo=2
      Shape BOX Dx=YPXG_LadderWidth/2.0+YPXG_CellGuardLen/2_
      Dy=YPXG_ActiveThk/2.0+YPXG_ReadOutThk/2.0 Dz=YPXG_TotalLength/2.0
      
      do nCell= 1, YPXG_numbercell ! Wafer loop, creat wafers inside the ladders 
      Create and Position YPLA x=0 y=YPXG_ActiveThk/2.0,
      z=(YPXG_CellLen+YPXG_CellGuardLen+YPXG_PCellLen)*((nCell-1)-(YPXG_numbercell-1)*0.5)

      enddo   !!loop ended for the nCell

endblock
* -----------------------------------------------------------------------------
Block YPLA is the active layer of the pixel
      Material  Silicon
      Material  Sensitive  Isvol=1
      Attribute YPLA  Seen=1  Colo=4

      Shape BOX dX=YPXG_LadderWidth/2.0 dY=YPXG_ActiveThk/2.0 Dz=YPXG_CellLen/2.0

      call      GSTPAR (%Imed,'STRA',1.)

      HITS    YPLA   Z:.0001:S  Y:.0001:   X:.0001:     Ptot:16:(0,100),
                     cx:10:    cy:10:    cz:10:    Sleng:16:(0,500),    
                     ToF:16:(0,1.e-6)  Step:.01: Eloss:32:(0,0.001),                     
                     zz:.0001:  yy:.0001:   xx:.0001:
endblock

* -----------------------------------------------------------------------------
      END



