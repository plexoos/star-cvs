* $Id: geometry.g,v 1.36 2000/11/22 17:51:41 nevski Exp $
* $Log: geometry.g,v $
* Revision 1.36  2000/11/22 17:51:41  nevski
* tof geometry versions 1 and 2 preserved in btofgeo1, version 3 goes in btofgeo2
*
***************************************************************************
   subroutine geometry
*                                                                         *
*  author  Pavel Nevski                                                   *
*  Created August 1998                                                    *
*                                                                         *
*  Update history:                                                        *
*  08/19/98, PN: tof is not part of year_2a                               *
*  12/04/98, PN: rich  + upstream part + zero degree calo                 *
*  09/26/99, E.Cains: 1H geometry added - one svt ladder at layer 3       *
*  01/27/99, PN: rich in 1H geometry is simulated with hits is quartz & fr*
***************************************************************************
   Implicit   none
   Logical    cave,pipe,svtt,tpce,ftpc,btof,vpdd,magp,calb,ecal,upst,rich,
              zcal,mfld,mwc,pse,tof,t25,t1,four,ems,alpipe,
              on/.true./,off/.false./
   real       Par(1000),field,dcay(5),shift(2),wdm
   Integer    LENOCC,LL,IPRIN,Nsi,i,j,l,nmod(2),Nleft,Mleft,Rv,Wfr,Itof
   character  Commands*4000
* - - - - - - - - - - - - - - - - -
+CDE,GCBANK,GCUNIT,GCPHYS,GCCUTS,GCFLAG,AGCKINE,QUEST.
*  temporarely until GCTLIT is not part of GCTMED:
   Integer        Thrind   ,Jmin,ItCkov,ImCkov,NpCkov
   common/GCTLIT/ Thrind(4),Jmin,ItCkov,ImCkov,NpCkov
* - - - - - - - - - - - - - - - - -
replace[;ON#{#;] with [
  IF Index(Commands,'#1')>0 
  { j=Index(Commands,'#1');  l=j+Lenocc('#1')-1;  Commands(j:l)=' ';
    <W>; (' #1: #2');
]
*
   call ASLGETBA ('GEOM','DETP',1000,LL,Par)
   If (JVOLUM>0) call AGDROP ('*')
   IPRIN    = IDEBUG
   NtrSubEv = 1000     " automatic !"
*
* -------------------- set GSTAR absolute default ------------------------
* Set only flags for the main configuration (everthing on, except for tof),
* but no actual parameters (CUTS,Processes,MODES) are set or modified here. 
* If an empty or no DETP GEOM was issued, geometry is defined externally.
*
   {cave,pipe,svtt,tpce,ftpc,btof,vpdd,calb,ecal,magp,mfld,upst,zcal} = on;
   {mwc,four,pse}=on      "MultiWire Chambers, 4th Si layer, pseudopadrows"   
   {tof,t25,t1,ems,rich,alpipe}=off   "TimeOfFlight, EM calorimeter Sector"
   field=5; Nsi=7; Rv=2; Wfr=0; Itof=2; Wdm=6.0;       "defaults constants"
   Commands=' '
*
* -------------------- select USERS configuration ------------------------
* On a non-empty DETP GEOM every keyword makes an action and is erased.
* Actions consist here of selecting the appropriate parameteres and flags.
* This flags are used in the next section to create subsystems and 
* to communicate DETP commands with parameters to them.
* 
If LL>1   
{ Call AGSFLAG  ('GEOM',1)
  CALL UHTOC(PAR(2),4,Commands,LL*4-4);  Call CLTOU(Commands);

  * set geant processes and cuts only if any detp geometry was issued:
   
  {CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM,DCUTE,DCUTM,PPCUTM} =.001;
  {IDCAY,IANNI,IBREM,ICOMP,IHADR,IMUNU,IPAIR,IPHOT,ILOSS,IDRAY,IMULS} = 1;
  {IRAYL,ISTRA} = 0;
  TOFMAX        = 1.e-4 
*
  for(j=1;j>0;) { j=0;
  on HELP       { you may select the following keywords: ;
                  <W>;('---------------:----------------------------- ');
                  <W>;('Configurations : complete,tpc_only,field_only ');
                  <W>;('               : year_1a,s,b,h,c;  year_2a    ');
                  <W>;('Geant Physics  : Hadr_on, Hadr_off            ');
                  <W>;('Geant Physics  : Phys_off, Decay_Only         ');
                  <W>;('Geometry Detail: mwc_off, pse_off, 4th_off    ');
                  <W>;('Magnetic Field : Field_on/off, field=value    ');
                  <W>;('Auxillary keys : Debug_on/off, Split_on/off   ');
                  <W>;('--------------------------------------------- ');
                  <W>;('Default: complete STAR with hadr_on,auto-split');
                  <W>;('--------------------------------------------- ');
                }  
  on COMPLETE   { Complete STAR geometry;                     tof=on; Itof=2; }
  on YEAR_1S    { starting in summer: TPC, CONE, AL pipe;          alpipe=on;
                                         {ftpc,vpdd,calb,ecal}=off;    Nsi=0; }
  on YEAR_1A    { poor approximation to year1: TPC+CTB+FTPC;      
                                              {vpdd,calb,ecal}=off;    Nsi=0; }
  on YEAR_1B    { better year1: TPC+CTB+FTPC+calo patch+RICH, no svt; 
                        {vpdd,ecal}=off;  {rich,ems,t1}=on;
                        Itof=1;  Rv=1;  Nsi=0;                                }
  on YEAR_1C    { not a year1:  TPC+CTB+FTPC+calo;  {vpdd,ecal}=off;   Nsi=0; }
  on YEAR_1H    { even better y1:  TPC+CTB+FTPC+RICH+caloPatch+svtLadder;  
                        {vpdd,ecal}=off;  {rich,ems,t1}=on; 
                        Itof=1;  Rv=2;  Wdm=6.0;  Nsi=-3;                     }
  on YEAR_1E    { even better y1:  TPC+CTB+RICH+caloPatch+svtLadder(4);  
*    HELEN:       one ladder at R=10.16cm with 7 wafers at the 12 O'Clock...
                        {vpdd,ecal,ftpc}=off; {rich,ems,t1}=on;
                        Itof=1;  Rv=2;  Wfr=7;  Wdm=6.0;  Nsi=-3;             }
  on YEAR_2A    { asymptotic STAR;                                            }
  on YEAR_2B    { 2001 geometry first guess - TPC+CTB+FTPC+RICH+CaloPatch+SVT;
                        {vpdd,ecal}=off;  {rich,ems,t1}=on;  
                        Itof=2;  Rv=2;  Wfr=7;  Wdm=6.305;                    }
  on HADR_ON    { all Geant Physics On;                                       }
  on HADR_OFF   { all Geant Physics on, except for hadronic interactions; 
                                                                       IHADR=0}
  on PHYS_OFF   { No Physics: only energy loss;
      {IDCAY,IANNI,IBREM,ICOMP,IHADR,IMUNU,IPAIR,IPHOT,IDRAY,IMULS}=0; Iloss=2}
  on DECAY_ONLY { Some Physics: decays, mult.scat and energy loss;
                  {IANNI,IBREM,ICOMP,IHADR,IMUNU,IPAIR,IPHOT,IDRAY}=0; Iloss=2}
  on TPC_ONLY   { Minimal geometry - only TPC;
                     {pipe,svtt,ftpc,btof,vpdd,calb,ecal,magp,upst,zcal}=off; }
  on FIELD_ONLY { No geometry - only magnetic field;              NtrSubEv=0;
      {cave,pipe,svtt,tpce,ftpc,btof,vpdd,magp,calb,ecal,rich,upst,zcal}=off; }
  on FIELD_OFF  { no magnetic field;                field=0;                  }
  on FIELD_ON   { Standard (5 KGs) field on;        field=5;                  }
  i=Index(Commands,'FIELD=')
  if i>0        { j=i/4+3; field=Par(1+j);  Commands(i:j*4)=' ';
                  <W> field; (' Modified field value =',F6.2,' KGS');         }
  on MWC_OFF    { Trigger Multy-wire readout off;   mwc=off;                  }
  on PSE_OFF    { No TPC pseudo-padrow generated;   pse=off;                  }
  on 4TH_OFF    { SVT fourth layer off;             Nsi=min(Nsi,6);           }
  on SPLIT_OFF  { events will not be split into subevents;     NtrSubEv=0;    }
  on SPLIT_ON   { events will be split into subevents;         NtrSubEv=1000; }
  on DEBUG_ON   { verbose mode, some graphics; Idebug=max(Idebug,1); Itest=1; }
  on DEBUG_OFF  { standard debug mode;         {Idebug,Itest}=0;              }
  }
* sanity check - if something left in commands (unknown keyword), we stop!
  l=LENOCC(commands); if l>0
  {  print *,' Unknown command left => ', commands(1:l), ' <= ',l
     if (IPRIN==0) stop 'You better stop here to avoid problems'     
  }
}
* -------------------- setup selected configuration ------------------------
* Now when all parameters and flags are ready, make gstar work as usually
* ie put a MODE or/and DETP command and executing them for selected systems.
*
* - to save secondaries AFTER all decays:      DETP TRAC DCAY 210 210 0.1 0.01
   dcay={210,210,0.1,0.01}
   If LL>1 { call AgDETP new ('Trac'); call AgDETP add ('TracDCAY',dcay,4) }
*
   if (rich) ItCKOV = 1
   if (rich & Rv>1) call AgDETP new ('Rich')
   if (rich & Rv>1) call AgDETP add ('Rich.Version=',Rv,1)
   if (cave)        Call cavegeo
   If (LL>1)        call AgDETP new ('PIPE')
   if (alpipe)      call AgDETP add ('pipg.BeLeng=', 0, 1)
   if (alpipe)      call AgDETP add ('pipg.S1Leng=',230,1)
   if (pipe)        Call pipegeo
   if (upst)        Call upstgeo

   Call AGSFLAG('SIMU',2)
* - to switch off the fourth svt layer:        DETP SVTT SVTG.nlayer=6 
   If (LL>1) call AgDETP new ('SVTT')
   if (svtt & Nsi < 7) call AgDETP add ('svtg.nlayer=',   Nsi,1)
   if (svtt & Wfr > 0) call AgDETP add ('svtl(3).nwafer=',wfr,1)
   if (svtt & wdm > 0) call AgDETP add ('swca.WaferWid=', wdm,1)
   if (svtt & wdm > 0) call AgDETP add ('swca.WaferLen=', wdm,1)
   if (svtt) Call svttgeo
 
* - MWC or pseudo padrows needed ? DETP TPCE TPCG(1).MWCread=0 TPRS(1).super=1
*  CRAY does not accept construction: IF (mwc==off) ... I do it differntly:

   If (LL>1) call AgDETP new ('TPCE')
   If (tpce &.not.mwc) call AgDETP add ('tpcg(1).MWCread=',0,1)
   If (tpce &.not.pse) call AgDETP add ('tprs(1).super='  ,1,1) 
   if (tpce) Call tpcegeo
   if (ftpc) Call ftpcgeo
   if (ftpc) Call supogeo

* - tof system should be on (for year 2):      DETP BTOF BTOG.choice=2
   If (LL>1) call AgDETP new ('BTOF')
   if (tof)  call AgDETP add ('btog.choice=',2,1)
   if (t25)  call AgDETP add ('btog.choice=',3,1)
   if (t1)   call AgDETP add ('btog.choice=',4,1)
   if btof { If Itof==1 { call btofgeo1 } else { call btofgeo2 }}
     
   Call AGSFLAG('SIMU',1)
   if (vpdd) Call vpddgeo

*  - barrel calorimeter may be a patch of 12 modules at the left side
   nmod={12,0};  shift={87,0}
   If (LL>1) call AgDETP new ('CALB')
   if (ems)  call AgDETP add ('calg.nmodule=',Nmod,2)
   if (ems)  call AgDETP add ('calg.shift=',shift,2)
   if (calb) Call calbgeo
   if (rich) Call richgeo
   if (ecal) Call ecalgeo
   if (zcal) Call zcalgeo
   if (magp) Call magpgeo
*
* - reset magnetic field value (default is 5): DETP MFLD MFLG.Bfield=5
   If (LL>1) call AgDETP new ('MFLD')
   if (mfld & field!=5) call AgDETP add ('MFLG(1).Bfield=',field,1)
   if (mfld) Call mfldgeo
*
   if JVOLUM>0 
   { Call ggclos
     If IDEBUG>0 { CALL ICLRWK(0,1); Call GDRAWC('CAVE',1,.2,10.,10.,.03,.03)}
   }
   IDEBUG = IPRIN
   ITEST  = min(IPRIN,1)
   Call agphysi
*                      automatic subevent size selection
   If NtrSubev > 0
   { Call MZNEED(IXDIV,1000,'G')
     NLEFT    = max(10,IQUEST(11)/1200)
     MLEFT    = 10**Int(Alog10(Float(Nleft))-1)
     NtrSubEv = MLEFT*(NLEFT/MLEFT)
     Prin1 NtrSubEv; (' Ntrack per subevent = ',i6)
   } 
*
   end


