   subroutine gstar_input
+CDE,typing,gcnum.
   print *,' Gstar User Input for TEXT, TXOLD and FZ formats activated '
   print *,' To activate XDF format readout do " gexec gstar_readxdf " '
   if (Npart<=100) call GUPART
   end

*************************************************************************
   subroutine agusopen(file)
*
* Description: open a TXT, EGZ(FZ) or XDF event generator data file.    *
*              Data type should corresponds to the filename extension   *
*              File types are saved in Ccommand - one letter per file   *
*************************************************************************
   Implicit   None   
   Integer    LENOCC,CSADDR,i,J,ier,L,N
   Character  file*(*),C*1
*
+CDE,AGCKINE.
*
    L=LENOCC(file);  Check L>0;
    C=' '
    Do i=1,L-1  {  if (file(i:i)='.') C=file(i+1:i+1) }
    Call CLTOU(C)
*
    N=LENOCC(CCOMMAND)+1
    print *,' AgUsOpen: openning file ',file,' mode ',C 
*
    if      C=='E'                       " egz format "
    {  Call AgzOPEN('PZ',file,'EK',0,0)
       Call AgZREAD('P',ier)
       if (Ier!=0)  goto :e:
    }
    else if C=='X'                       " xdf format "
    {  J=Csaddr('xdf_open')
       If ( J==0 )  goto :e:
       call CsJCAL(J,2,file)
    }
    else                                 "  any text  "
    {  Call AgFOPEN(21-N,file,ier) 
       if (ier!=0)  goto :e:
    } 
    CCOMMAND(N:N)=C
    return
:e: print *,' gstar_AgUsOPEN error openning file ',file(1:L)
   end

*************************************************************************
   Subroutine  AgUsRead(ier)
*                                                                       *
* Description: loop over all files opened by USER/INPUT                 *
*              and call the corresponding readout routine               *
*************************************************************************
   Implicit   None
   Integer    LENOCC,CSADDR,LOCF,Igate,Ier,J,I
   Character  C*1
*
+CDE,GCBANK,SCLINK,RBBANK,AgCKINE,GCNUM.
*
   Call HEPEVNT

   Do i=1,LENOCC(Ccommand)
     C=CCOMMAND(i:i); Igate=i
     if     C=='E' { Call AGZREAD('P',ier); call gstar_ReadEGZ(Igate)     }
     elseif C=='X' { IrbDIV=IxDIV;          LKARP2=LkEvnt
                     J=CsADDR('xdf_read'); If (J!=0) call CsJCAL(J,1,Igate) }
     else          {                          call gstar_ReadTXT(Igate)     }
     If Igate<=0   { Ier=1; return }
     print *,' AgUsREAD mode ',C,': # particles in GEANT=',Ntrack,
                                 '  # vertices=',Nvertx
   enddo

*  To generate more header information:
   call HEPEHeader

*  To map hepe_gent table:
   call agstrut('/evnt/gene/gent@HEPE','Event')

   End
 
*************************************************************************
   Subroutine    gstar_ReadTXT(Igate)
*                                                                       *
* Description: read both NEW and OLD gstar event generator text formats *
Replace [READ[DIGIT](#)#;] with [READ(#2,ERR=:E:)#3;IF(Idebug>=#1)<W>#3;] 
*************************************************************************
   implicit      none
   character*8   tit    ", eg_name, frame "
   character*120 line
   integer       LENOCC,Index,li,Ieven,Ntrac,Nvert,itr,ivt,nv,nt,Igate,
                 LabelTr,LabelVx,ge_pid,eg_pid,StartVx,StopVx,i,
                 eg_proc,parent
   Real          version,east_z,east_a,west_z,west_a,sqrts,b_max,
                 PP(3),vert(4),UBUF(10),a,b
*
+CDE,GCUNIT,GCFLAG.
*
   Li=21-Igate
 { Ntrac,Nvert } = 999;
 { itr, ivt    } =  0;
*
 WHILE itr<Ntrac | ivt<Nvert
 { line='end-of-file';   read5 (li,'(a)') Line; (1x,a)

   If Line(1:5)=='GENER'
   {  
     read1 (line(11:),*) version,east_z,east_a,west_z,west_a,sqrts,b_max
                         (' gstar_ReadNew: GENER :',F8.3,4F8.0,F8.1,F8.3)
   }
   else If Line(1:5)=='EVENT'
   { 
     read2 (line(7:),*,end=:a:)  Ieven,Ntrac,Nvert,a,b
                         (' gstar_ReadNew: EVENT :',3i8,2f8.3)
     :a: if (Ieven<=-999) goto :e: " end of data "
   }
   else If Line(1:5)='TRACK'
   { 
     read5 (line(7:),*)  ge_pid,PP,LabelTr,StartVx,StopVx,eg_pid  
                         (16x,'TRACK :',i6,3F9.3,4i6)

     Call VZERO(Vert,4); Call AgSVERT(vert,-StartVx,-Igate,Ubuf,0,nv)  
     Itr += 1;           call AgSKINE(PP,ge_pid,nv,Ubuf,0,nt)
   }
   else If Line(1:6)=='VERTEX'
   { 
     read5 (line(8:),*)  Vert,LabelVx,eg_proc,parent
                         (16x,'VERTEX:',4F10.6,3i6)
     ivt += 1;           call AgSVERT(vert,-LabelVx,-Igate,Ubuf,0,nv) 
   }
   else If Index(Line,'event')>0 & itr+ivt==0      " old format "
   { i=Index(Line,'event');  line(i:i+6)='  ';
     read1 (line,*) Ntrac,Ieven; (' gstar_ReadOld: ',i8,' event# ',i6)
     call VZERO(vert,4); call AgSVERT(vert,-1,-Igate,Ubuf,0,nv)
     do itr=1,Ntrac
        read5 (li,*) ge_pid,PP; (16x,i6,3F8.3)
        call AgSKINE(PP,ge_pid,nv,Ubuf,0,nt)
     enddo  
     break
   }
   else If LENOCC(Line)>0
   { <w> line(1:LENOCC(Line)); (' unknown line : ',a); }
 } return
*
:e:<w> line; (' gstar_readTXT error in line '/1x,a);  Igate=-1 
   end

*************************************************************************
   subroutine gstar_ReadEGZ (Igate)
*                                                                       *
* Description: copy event generator data from ZEBRA banks (CODE/EVEN)   *
*              Banks are read in a standard way on LKRUNT/LKEVNT links  *
*************************************************************************
   Implicit   None
   Character  Cname*4,Gname*8,Gnamo*8/' '/
   Real       evtver,zproj,aproj,ztarg,atarg,sqrts,bmin,bmax,bimevt,
              plab(4),UBUF(100),VERT(4)/4*0/
   Integer    Iprin,L,i,nptls,nptarg,nntarg,npproj,nnproj,ntry,IdPtl,
              IRC,Ipart,Nvtx,Nt,Igate,Part_Size/6/
*
+CDE,GCBANK,GCUNIT,SCLINK,GCFLAG.
*
      Iprin=Idebug
      prin1; (' gstar_EGZ called to decode RUNT/EVNT banks ')
*
*     generator run header
      check   LKRUNT>0;    call UHTOC(IQ(LKRUNT-4),4,CNAME,4)
      check  "bank length" IQ(LKRUNT-1)>=10 & "name" CNAME=='CODE'
*
      call UHTOC(IQ(LKRUNT+1),4,Gname,8)
      evtver  = Q(LKRUNT+3)
      zproj   = Q(LKRUNT+4)
      aproj   = Q(LKRUNT+5)
      ztarg   = Q(LKRUNT+6)
      atarg   = Q(LKRUNT+7)
      sqrts   = Q(LKRUNT+8)
      bmin    = Q(LKRUNT+9)
      bmax    = Q(LKRUNT+10)
*
      If Gname!=Gnamo
      { Prin1 Gname,evtver,zproj,aproj,ztarg,atarg,sqrts,bmin,bmax
        (' Gstar_EGZ: generator ',a8,' version ',f8.3/10x,
        ' beam z/a',2f5.0,' target a/z',2f5.0,' sqrt(s)=',f6.1,' b=',2f8.3)
          Gnamo=Gname
      }
*     Normal event follows
      Check   LKEVNT>0;    Call UHTOC(IQ(LKEVNT-4),4,CNAME,4)
      check  "bank length" IQ(LKEVNT-1)>=7 & "name" CNAME=='EVEN'
*
*     Do leading part of bank
      nptls   = IQ(LKEVNT+1)
      nptarg  = IQ(LKEVNT+2)
      nntarg  = IQ(LKEVNT+3)
      npproj  = IQ(LKEVNT+4)
      nnproj  = IQ(LKEVNT+5)
      ntry    = IQ(LKEVNT+6)
      bimevt  =  q(LKEVNT+7)

      prin3 nptls,npproj,nnproj,nptarg,nntarg,ntry,bimevt
      (' Gstar_EGZ: setting GEANT vertex with Npart=',i6,' (before filter)'/,
        10x,' beam p/n=',2i5,'  targ p/n=',2i5,'  ntry=',i6,'  b=',f8.3)
      Call aGSVERT(VERT,-1,-Igate,Ubuf,0,Nvtx)
*
*     Do trailing part of bank:  ID, 4-momentum, daughter/parent information
      do i=1,nptls
         L=LKEVNT+7+(i-1)*part_size
         Ipart    =  0
         Idptl    = IQ(L+1)
         Call UCOPY (Q(L+2),plab,4)
         If (Gname='VENUS'  | Gname='HIJET')    Call ISA_to_GEANT (IdPtl,Ipart)
         If (Gname='HIJING')                    Call a PDG 2 GEA  (IdPtl,Ipart)
         If (Cname='FRITIOF'| Gname='FRTIOF17') Call LUND_to_GEANT(IdPtl,Ipart)
         if (Ipart<=0) then
           prin1 Idptl,Gname
           (' Gstar_EGZ: unknown particle ',i6,' from ',a)
           next
         endif
         Call AgGZUFLT('EGZ',1,I,Idptl,Vert,Plab,IRC);  Check IRC==0
         CALL AgSKINE (Plab,Ipart,Nvtx,Ubuf,0,Nt)
      enddo
*
   END

*************************************************************************
   module  HepeHeader defines event header
   author  Pavel Nevski
   created sometime ago
*************************************************************************
+CDE,GCBANK,GCTIME,SCLINK.
*
      STRUCTURE EGRN {               " Event generator run structure  "_
        INT   generator              " event generator identification ",
        CHAR  eg_name(8)             " event generator name           ",
        REAL  eg_version             " version of event generator     ",
        INT   eg_run                 " generator run number           ",
        INT   eg_rndm(2)             " generator random numbers       ",
        REAL  sqrts                  " center of mass energy          ",
        INT   is_minbias             " minimum bias flag              ",
        REAL  b_min                  " minimum impact parameter       ",
        REAL  b_max                  " maximum impact parameter       ",
        INT   east_a                 " projectile 1 mass number       ",
        INT   east_z                 " projectile 1 charge            ",
        INT   west_a                 " projectile 2 mass number       ",
        INT   west_z                 " projectile 2 charge            ",
        INT   polarizatn(10)         " to be defined                  "_
      }
*
      STRUCTURE EGEV {               " Event generator event structure"_
        INT   n_event                " eg event number                ",
        REAL  b_impact               " actual impact parameter        ",
        REAL  phi_impact             " reaction plane                 ",
        INT   event_type             " trigger, minbias bkgd, cosmic  ",
        INT   polarizatn(10)         " to be defined                  ",
        INT   n_proteast             " number of participant protons  ",
        INT   n_neuteast             " number of participant neutrons ",
        INT   n_protwest             " number of participant protons  ",
        INT   n_neutwest             " number of participant neutrons ",
        INT   n_track                " # tracks                       ",
        INT   n_vertex               " # vertices                     ",
        INT   n_fstrack              " # final state tracks           ",
        INT   n_nfstrack             " # non-final state tracks       ",
        INT   n_prvertex             " # primary vertices             ",
        INT   n_fsvertex             " # non-final state vertices     ",
        INT   pr_vertex              " pointer to ll of primary vertices",
        INT   fs_vertex              " pointer to ll of final state vert"_
      }
*
   structure uevn {                      _
        char author(20), char machine(20), 
        int date, int time, version,
        int ge_run,
        int rndmrun(2),
        int ordered,
        int rndmevt(2),
        int n_event,
        int cav(12),
        int n_shtk_evt,
        int mx_shtk,
        int nw_shtk,
        int firstvert,
        int firsttrack,  
        int equals  _
    }
*
   integer iprin,Iadr
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*

   fill /EVNT/UEVN(1) ! user event buffer
      author     = { 'mea','mea' }  ! author
      machine    = { 'ibm','bla' }  ! machine
      date       = igdate           ! date
      time       = igtime           ! time
      version    = gversn           ! geant version
      equals     = 99999            ! end label
   endfill
*
   IF (LKRUNT>0) then
    FILL /EVNT/UEVN/EGRN(1)          !  Event generator run structure 
      generator  = 0                ! event generator identification
      eg_name    = { 'a','b','c' }  ! event generator name
      eg_version = Q(LKRUNT+3)      ! version of event generator
      eg_run     = 0                ! generator run number
      eg_rndm    = { 0,0 }          ! generator random numbers
      sqrts      = Q(LKRUNT+8)      ! center of mass energy
      is_minbias = 0                ! minimum bias flag
      b_min      = Q(LKRUNT+9)      ! minimum impact parameter
      b_max      = Q(LKRUNT+10)     ! maximum impact parameter
      east_a     = Q(LKRUNT+5)      ! projectile 1 mass number
      east_z     = Q(LKRUNT+4)      ! projectile 1 charge
      west_a     = Q(LKRUNT+7)      ! projectile 2 mass number
      west_z     = Q(LKRUNT+6)      ! projectile 2 charge
      polarizatn = { 0,0,0,0,0 }    ! to be defined
    endfill
   endif
*
   If (LKEVNT>0) then
    FILL /EVNT/UEVN/EGEV(1)     !  Event generator event structure 
      n_event    = 0                ! eg event number
      b_impact   = Q (LKEVNT+7)     ! actual impact parameter
      phi_impact = 0                ! reaction plane
      event_type = 0                ! trigger, minbias bkgd, cosmic, etc.
      polarizatn = { 0,0,0,0,0 }    ! to be defined
      n_proteast = IQ(LKEVNT+4)     ! number of participant protons
      n_neuteast = IQ(LKEVNT+5)     ! number of participant neutrons
      n_protwest = IQ(LKEVNT+2)     ! number of participant protons
      n_neutwest = IQ(LKEVNT+3)     ! number of participant neutrons
      n_track    = IQ(LKEVNT+1)     ! number of tracks
      n_vertex   = 0                ! number of vertices
      n_fstrack  = 0                ! number of final state tracks
      n_nfstrack = 0                ! number of non-final state tracks
      n_prvertex = 0                ! number of primary vertices
      n_fsvertex = 0                ! number of non-final state vertices
      pr_vertex  = 0                ! pointer to ll of primary vertices
      fs_vertex  = 0                ! pointer to ll of final state vert.
    endfill
   endif
*
   Iadr = 0
   call GsHEAD (len_egrn-2, egrn_generator, iadr)
   call GsHEAD (len_egev-2, egev_N_event,   iadr)
   call GsHEAD (len_uevn-2, uevn_author,    iadr)
   print *,' HepeHeader: geant header set with length =',iadr
*
   end

*****************************************************************************
 module  HEPEVNT  is the HEPEVNT event header
 author  P.Nevski
 created at night
*****************************************************************************
+CDE,AGECOM,GCBANK,SCLINK,RBBANK.
 real x
 Old Structure PASS {int SYS1, int SYS2, int SYS3, int PJID,int GJID,int EVID}

 Old Structure GENE {int SYS1, int SYS2, int SYS3, int GRUN,int GEVT,
                     Char GNAM, VRTX, VRTY, VRTZ, VRTT, int WTFL, WEIG }

 Old Structure GENT {int STAT, int PDGC, int MOT1,int MOT2, int DAU1,int DAU2,
                     PX, PY, PZ, ENER, MASS, VERTX, VERTY, VERTZ, TIME }

     If (LkEvnt==0) Call MZBOOK(IxDIV,LKAR P2,LkEvnt, 1,'EVNT',2,2,7,2,0)

     IrbDIV=IxDIV;         LKARP2=LkEvnt
     Fill /EVNT/PASS(1)  ! Pass Record Bank
        SYS1 = 1         !  Format flag
        SYS2 = 1         !  Member system word = 100000*NG+NM NGEN 
        SYS3 = 300003    !  Modularity system word = 100000*NW+NT
        PJID = 1         !  Pass Job ID (GJID for latest PASS bank)
        GJID = 1         !  Generator Job ID.
        EVID = 1         !  ZEBRA IDN of event read in or generated
     endfill

     IrbDIV=IxDIV;         LKARP2=LkEvnt
     Fill /EVNT/GENE(1)  ! GENZ Event Bank  
       SYS1 =     1      !  Format flag = 1
       SYS2 =     0      !  Member system word = 100000*NG+NM 
       SYS3 =     0      !  Modularity system word = 100000*NW+NT
       GRUN =     0      !  Generator run number
       GEVT =     1      !  Generator event number
       GNAM = 'VENUS'    !  Generator name
       VRTX =   0.0      !  Interaction vertex position in metres
       VRTY =   0.0      !  idem
       VRTZ =   0.0      !  idem
       VRTT =   0.0      !  Interaction vertex time in seconds
       WTFL =     1      !  Interaction weight flag
       WEIG =  1.00      !  Interaction weight  
     endfill
 
     IrbDIV=IxDIV;             LKARP2=LkEvnt
     Fill /EVNT/GENE/GENT(1) ! HEPEVT parton level data 
       STAT  =   0       !  Status flag
       PDGC  =   0       !  PDG particle code
       MOT1  =   0       !  First mother
       MOT2  =   0       !  Second mother. If -ve, mothers give a range
       DAU1  =   0       !  First daughter. If<0, reference to GEANT
       DAU2  =   0       !  Last daughter.  If<0, reference to GEANT
       PX    = 0.0       !  x-Momentum of particle in GeV/c
       PY    = 0.0       !  idem in y
       PZ    = 0.0       !  idem in z
       ENER  = 0.0       !  particle Total Energy 
       MASS  = 0.0       !  Mass in GeV/c. Not necessarily the on-shell m
       VERTX = 0.0       !  particle origin in x
       VERTY = 0.0       !  idem in y
       VERTZ = 0.0       !  idem in z
       TIME  = 0.0       !  Start time of particle relative to the interact
     endfill

 end






