* $Id: ffpdstep.g,v 1.2 2004/03/31 16:34:10 potekhin Exp $
* $Log: ffpdstep.g,v $
* Revision 1.2  2004/03/31 16:34:10  potekhin
* Factored out two methods due to code
* reorganization.
*

      subroutine FFPDSTEP
* discard cerenkov photon absorption hits everywhere except for
* photocathode
+CDE,TYPING,GCBANK,GCONST,GCUNIT,GCTMED,GCTRAK,GCKINE,GCSETS,AGCSTEP.
      character  Cmed*13

*      check ISVOL>0 
*      CALL UHTOC(NATMED,4,Cmed,8)
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
      Real HIT
      if(Ipart == 50 & Istop ==2 & deStep>0.0) then
        hit=1.0
        return
      endif
      hit = 0.0
      return
      end 
