* $Id: g2rmain.g,v 1.2 2000/01/28 00:47:13 nevsky Exp $
* $Log: g2rmain.g,v $
* Revision 1.2  2000/01/28 00:47:13  nevsky
* rich hits may contain up to 5 detectors: RCSI,RGAP,QUAR,FREO,OQUA
*
* Revision 1.1  2000/01/24 14:48:43  nevski
*  combined g2t+g2r version
*
* Revision 1.4  2000/01/12 00:08:51  nevski
* clean-up: control skip headers, g2t/r tpc corrections speed-up
*
* Revision 1.3  2000/01/11 17:29:57  nevski
* a separate g2t_field function provided to extract nominal field
*
*
**:>-----------------------------------------------------------------------
module    G2Rmain  is g2r converter
author    Pavel Nevski
created   22 april 98
*
* description: create and fill two new directories in the CWD:
*              Run   - with tables mapped to DETM family banks
*              Event - with track/vertex and hit tables
*              One level is assumed, names can be modified by DETP command
* Modifications 
* PN, 98/09/23: leave NTRACK in IQUEST(100) to allow analysis in Kuip
**:<-----------------------------------------------------------------------
#include "geant321/gcbank.inc"
#include "geant321/gcflag.inc"
#include "geant321/gcunit.inc"
#include "geant321/gcnum.inc"
#include "g2t_event.inc"
#include "g2t_vertex.inc"
#include "g2t_track.inc"
#include "g2t_hits.inc"
+cde,quest.
*
      structure GTTC  { int version, int nsys , char edir(3), char rdir(3)}
      structure dete  { onoff, char ctab, char spec, char csys, char cdet }
      structure deten { onoff, char ctab, char spec, char csys, char cdet }
      Integer        G2R_MAIN,DUI_CDIR,AgHITSET
      Integer        TDM_NEW_TABLE,AMI_CALL,G2T_NEW_TABLE
      External       TDM_NEW_TABLE,AMI_CALL,G2T_NEW_TABLE
      Integer        i,j,ld,nhits,nnhits,Iprin
      Character*16   ctab,ctabo,hitter,names(3)
      Character      o*1,cdir*20,edir*20
*
  entry  g2r 
  entry  g2r_start

* show Ntrack in Kuip
  IQUEST(100)=0

 begin 
 G2R_MAIN=0 
 if JVOLUM<=0 { print *,' G2R: no geometry loaded YET'; return; }
*
 If (First) Then
   first = .false.
   fill GTTC(1)           ! g2r control
     version = 1                  ! version number
     nsys    = 21                 ! number of subsystems
     edir    = {'Even','t',' '}   ! event output directory name 
     rdir    = {'Run ',' ',' '}   ! run output directory name 

   fill dete       ! star subsystem
     onoff = 1            ! usage flag
     ctab  = 'svt'        ! table name
     spec  = 'svt'        ! specification type
     csys  = 'SVTT'       ! Geant Subsystem
     cdet  = 'SVTD'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'svt'        ! table name
     spec  = 'svt'        ! specification type
     csys  = 'SVTT'       ! Geant Subsystem
     cdet  = 'SFSD'       ! Sensitive detector

   fill dete       ! star subsystem
     ctab  = 'tpc'        ! table name
     spec  = 'tpc'        ! specification type
     csys  = 'TPCE'       ! Geant Subsystem
     cdet  = 'TPAD'       ! Sensitive detector
   fill dete       ! star subsystem
     cdet  = 'TPAI'       ! Sensitive detector
   fill dete       ! star subsystem
     cdet  = 'TPAO'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'mwc'        ! table name
     spec  = 'mwc'        ! specification type
     csys  = 'TPCE'       ! Geant Subsystem
     cdet  = 'TMSE'       ! Sensitive detector

   fill dete       ! star subsystem
     ctab  = 'ftp'        ! table name
     spec  = 'ftp'        ! specification type
     csys  = 'FTPC'       ! Geant Subsystem
     cdet  = 'FSEN'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'ftp'        ! table name
     spec  = 'ftp'        ! specification type
     csys  = 'FTPC'       ! Geant Subsystem
     cdet  = 'FSEC'       ! Sensitive detector

   fill dete       ! star subsystem
     ctab  = 'ctb'        ! table name
     spec  = 'ctf'        ! specification type
     csys  = 'BTOF'       ! Geant Subsystem
     cdet  = 'BXSA'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'tof'        ! table name
     spec  = 'ctf'        ! specification type
     csys  = 'BTOF'       ! Geant Subsystem
     cdet  = 'BCSB'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'ctb'        ! table name
     spec  = 'ctf'        ! specification type
     csys  = 'BTOF'       ! Geant Subsystem
     cdet  = 'BCSA'       ! Sensitive detector

   fill dete       ! star subsystem
     ctab  = 'rch'        ! table name
     spec  = 'ctf'        ! specification type
     csys  = 'RICH'       ! Geant Subsystem
     cdet  = 'RCSI'       ! Sensitive detector
   fill dete       ! star subsystem
     cdet  = 'RGAP'       ! Sensitive detector
   fill dete       ! star subsystem
     cdet  = 'QUAR'       ! Sensitive detector
   fill dete       ! star subsystem
     cdet  = 'FREO'       ! Sensitive detector
   fill dete       ! star subsystem
     cdet  = 'OQUA'       ! Sensitive detector

   fill dete       ! star subsystem
     ctab  = 'emc'        ! table name
     spec  = 'emc'        ! specification type
     csys  = 'CALB'       ! Geant Subsystem
     cdet  = 'CSUP'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'smd'        ! table name
     spec  = 'emc'        ! specification type
     csys  = 'CALB'       ! Geant Subsystem
     cdet  = 'CSDA'       ! Sensitive detector

   fill dete       ! star subsystem
     ctab  = 'eem'        ! table name
     spec  = 'emc'        ! specification type
     csys  = 'ECAL'       ! Geant Subsystem
     cdet  = 'ESCI'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'esm'        ! table name
     spec  = 'emc'        ! specification type
     csys  = 'ECAL'       ! Geant Subsystem
     cdet  = 'EXSE'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'zdc'        ! table name
     spec  = 'emc'        ! specification type
     csys  = 'ZCAL'       ! Geant Subsystem
     cdet  = 'QSCI'       ! Sensitive detector

   fill dete       ! star subsystem
     ctab  = 'vpd'        ! table name
     spec  = 'vpd'        ! specification type
     csys  = 'VPDD'       ! Geant Subsystem
     cdet  = 'VRAD'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'psc'        ! table name
     spec  = '   '        ! specification type
     csys  = 'PMDD'       ! Geant Subsystem
     cdet  = 'VRAD'       ! Sensitive detector
   fill dete       ! star subsystem
     ctab  = 'pgc'        ! table name
   fill dete       ! star subsystem
     ctab  = '    '       ! table name
     spec  = '    '       ! specification type
     csys  = '    '       ! Geant Subsystem
     cdet  = '    '       ! Sensitive detector
   endfill
*
 endif
*   
      G2R_MAIN = 0
      Use GTTC
*     map detm family banks:
      o    = CHAR(0)
      Cdir = gttc_rdir(1)//gttc_rdir(2)//gttc_rdir(3)
      Ld=0; Do i=1,12 { if (Cdir(i:i)==' '|Cdir(i:i)==o) Break; Ld=i; }
      If (Ld>0&Cdir!='none')  Call AGSTRUT (' ',Cdir(1:ld))
*
      Cdir = gttc_edir(1)//gttc_edir(2)//gttc_edir(3)
      Ld=0; Do i=1,12 { if (Cdir(i:i)==' '|Cdir(i:i)==o) Break; Ld=i; }
      edir = Cdir(1:ld)//o;  if (ld>0) Call TDM_CLEAR_ALL(edir)

      IQUEST(1)=IEOTRI+1
      if (.not. ( NTRACK>0 & NVERTX>0 & IEOTRI==0) ) then
         If (NTRACK+NVERTX>0) <w>;(' G2R: something wrong !')
         If (NTRACK+NVERTX+IEOTRI>0) <w> NTRACK,NVERTX,IEOTRI
            (' G2R: quit with NTRACK,NVERTX,IEOTRI=',3i7)
         return
      endif
*     map ghea_* headers and  hepe_* particle tables:
      call agstrut('/evnt/@HEPE',Edir)
      call agstrut('/evnt/@GHEA',Edir)

      if (ld>0) i = DUI_CDIR (edir)
      i = TDM_NEW_TABLE('g2t_event'//o, G2T_EVENT_SPEC//o, 1)
      i = TDM_NEW_TABLE('g2t_vertex'//o,G2T_VERTEX_SPEC//o,NVERTX)
      i = TDM_NEW_TABLE('g2t_track'//o, G2T_TRACK_SPEC//o, NTRACK)

*  NOTE:
*  order of calls IS importrant to proper fill vertex-track-hit references:
*  STAF ami-calls return 1 as OK flag, 0 as ERROR

      names(1)='g2t_vertex'//o
      names(2)='g2t_track'//o

      i = AMI_CALL ('g2t_get_kine'//o,2,names)
      prin5 i; (' g2tmain: get_kine done with i=',i6)

      Use GTTC

      nnhits = 0
      do j=1,GTTC_Nsys

        use dete (j) 

        call GFNHIT (dete_Csys(1:3)//'H',dete_Cdet,Nhits)
        ctab='g2t_'//dete_ctab(1:3)//'_hit'//o
        prin4 ctab,dete_Csys,dete_Cdet,nhits
        (' in G2Tmain: found ',3(1x,a),'   Nhits = ',i6)
        nnhits = nnhits + Nhits

        use deten(j+1) 
        check dete_ctab != deten_ctab & nnHits>0

        i = G2T_NEW_TABLE (ctab, dete_spec, nnhits)
        prin3 i,nnhits; (' g2rmain ===> tdm table for g2t_hits = ',2i6)

        names(1) = 'g2t_track'//o
        names(2) =  ctab
        hitter   = 'g2t_'//dete_Ctab(1:3)//o
        i        =  AMI_CALL (hitter,2,names)
        prin5 hitter,i; (' g2rmain ===> ami_call ',a7,' = ',i6)
        nnhits   = 0

      enddo
*
      i = AMI_CALL ('g2t_get_event'//o,1,'g2t_event'//o)
      prin5 i; (' g2rmain: get_event done with i=',i6)
*
      if (ld>0) i = DUI_CDIR('..'//o)
      IQUEST(1)   = IEOTRI
      IQUEST(100) = NTRACK
      G2R_MAIN = i
      END

