******************************************************************************
* $Id: calbpar.g,v 1.1 2004/01/19 21:15:50 potekhin Exp $
* $Log: calbpar.g,v $
* Revision 1.1  2004/01/19 21:15:50  potekhin
* We need to make this sub a separate compilation unit due
* to necessary restructuring of the calb geometry
*
      subroutine  CALBPAR(imed,medium)
*
      integer   imed
      character medium*(*)
*
      if      ( medium=='ABSORBER' ) then    
* --- cuts for EMC absorber and scintillator 
        Call GSTPAR (imed,'CUTGAM',0.00008)
        Call GSTPAR (imed,'CUTELE',0.001)
        Call GSTPAR (imed,'BCUTE' ,0.0001)
      else if ( medium=='SENSITIVE' ) then   
* --- cuts for SMD light material and gas
        Call GSTPAR (imed,'CUTGAM',0.00001)
        Call GSTPAR (imed,'CUTELE',0.00001)    
      endif

*--- common cuts for hadrons and muons in EMC&SMD

      Call GSTPAR (imed,'CUTNEU',0.001)
      Call GSTPAR (imed,'CUTHAD',0.001)
      Call GSTPAR (imed,'CUTMUO',0.001)
*
      end
