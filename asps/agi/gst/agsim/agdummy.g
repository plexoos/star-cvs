*************************************************************************
                SUBROUTINE    A G D U M M Y
*  description - a routine to force loading of library routines 	*
*                frequently used in some users codes                    *
Replace[REF#,#;] with [{IF}[EXIST 1]{Call #1;Call CsExt('D',#1);REF #2,;}]
*************************************************************************
Replace[LIST #;] with [ #1 _
 "Geant  "   GfKine,GfVert,GfPart,GfPath,GfHead,GDtoM,GMtoD,GlVolu,
             GpKine,GLast,GsHEAD,GsCKOV,GpIons,Grndm,Granor,Gfnhit,GpRung,
 "vector "   vdist,vscale,vadd,vmod,sortzv,vfill,Ublank,Ufill,Uzero,IuLast,
 "tr-prop"   trprfn,trprop,trscsp,trspsc,trscsd,trsdsc,
 "minuit "   mnseti,mninit,mnstat,mnexcm,mnpout,mnparm,
 "matrix "   Rsinv,Dsinv,Rinv,Dinv,Ssmt5t,Xmm55,Dmmlt,
 "ffread "   ffinit,ffset,ffkey,ffget,ffgo,
 "random "   poissn,norran,rndm,rnorml,dircos,
 "random "   binomi,rngama,gauss,proxim,
 "comis  "   csjcal,csaddr,jumpad,Jbyt,
 "interp "   fint,polint,lfit,divdif,lfitw,lvsimx,
 "zebra  "   mzvolm,mzdred,zverif,
 "hbook  "   hplfun,hpagsz,hnoent,hnform,
 "somesg "   rm48,rm48in,rsfact,rsfinv,rsfeqn,dsfact,rfft,cfft,
 " math   "  sortrq,dgmlt1,dgmlt2
 " sind,cosd "
 ]
+CDE,GCFLAG.
  Integer  SystemF,Ix/0/
  list external;
* make sure that real calls will never be done even if this routine is called
  Ix = Ix+1;  if (Ix<=0) Return;
  Ix = Ix+1;  if (Ix>=0) Return;
  ix=SystemF(' ')
* now fake calls to library - no need for arguments
  list ref;
*
  END
 
 
 
