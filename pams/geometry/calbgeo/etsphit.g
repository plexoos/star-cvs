******************************************************************************
* $Id: etsphit.g,v 1.1 2004/01/19 21:16:53 potekhin Exp $
* $Log: etsphit.g,v $
* Revision 1.1  2004/01/19 21:16:53  potekhin
* Need to make this into a separate compilation unit
* due to necessary code changes in the calbgeo
*
                Subroutine  etsphit(j,Hit)
*
+CDE,Typing,GCBANK,GCVOLU,GCKINE,GCTRAK,AgCSTEP.
*
      integer j,iplane,ishape
      integer nlev_r,nlev_m,mycell,jmycell,jmypar
      character cname*4,mname*4
      data mname/'CSDA'/
      Real      Hit,xyzhm(3),xyzh(3),dxhw,dyhw,dzhw
      save nlev_r,nlev_m,dxhw,dyhw,dzhw
*
      nlev_r=nlevel-2
      mycell=lvolum(nlev_r) 
      jmycell=LQ(jvolum-mycell)
      call uhtoc(IQ(jmycell-4),4,cname,4)
      if(cname==mname) then
        ishape=Q(jmycell+2)
        if(ishape==1) then
          jmypar=LQ(jgpar-nlev_r)
          dxhw=Q(jmypar+1)
          dyhw=Q(jmypar+2)
          dzhw=Q(jmypar+3)
        endif
      else
        print *,'There are some problem with level volume in the CSDA'        
      endif
      nlev_m=nlevel
      call gdtom(xloc(1),xyzhm(1),1)
      hits(1)=hit
      iplane=hit
      nlevel=nlev_r
      call gmtod(xyzhm(1),xyzh(1),1)
      nlevel=nlev_m
      if(iplane.le.2) then
        hit=xyzh(3)+dzhw
      else
        hit=xyzh(2)+dyhw
      endif
*
      end

