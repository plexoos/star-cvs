*************************************************************************
                SUBROUTINE    A G D U M M Y
*  description - a routine to force loading of library routines 	*
*                frequently used in some users codes                    *
Replace[REF#,#;] with [{IF}[EXIST 1]{Call #1;Call CsExt('D',#1);REF #2,;}]
*************************************************************************
Replace[LIST #;] with [ #1 _
 "Geant  "   GfKine,GfVert,GfPart,GfPath,GfHead,GDtoM,GMtoD,GlVolu,Grun,
             GpKine,GLast,GsHEAD,GsCKOV,GpIons,Grndm,Granor,Gfnhit,GpRung,
 "vector "   vdist,vscale,vadd,vmod,vfill,Ublank,Ufill,Uzero,IuLast,
 "interp "   fint,polint,lfit,divdif,lfitw,lvsimx,lvmaxa,lzlong,
 "sorting"   sortzv,zsorvh,intsor,
 "minuit "   mnseti,mninit,mnstat,mnexcm,mnpout,mnparm,
 "matrix "   Rsinv,Dsinv,Rinv,Dinv,Dmmlt,assndx,
 "ffread "   ffinit,ffset,ffkey,ffget,ffgo,
 "random "   poissn,norran,rndm,rnorml,dircos,rnpssn,
 "random "   binomi,rngama,gauss,proxim,rnhpre,rnhran,
 "comis  "   csjcal,csaddr,jumpad,Jbyt,msbit,
 "zebra  "   mzvolm,mzdred,zverif,fzrun,fzlimi,rzvers,
 "hbook  "   hplfun,hpagsz,hnoent,hnform,hbpro,hspfun,hplcap,
 "somesg "   rm48,rm48in,rsfact,rsfinv,rsfeqn,dsfact,rfft,cfft,
 " math  "   sortrq,dgmlt1,dgmlt2,eisrs1,dsfeqn,mxuty,rseqn,
             DMCPY,DVSET,DMMPA,DFACT,DFEQN,DFINV,DMBIL,
 "spare  "   ie3tos,ie3fos,pkbyt,upkbyt,ucocop,vbias,vline,vdist2,vmaxa,
 " epio  "   epinit,epread,epsetw,epdefu,tzinit,tzfind,cfperm,cfsize,
 "muonbx "   icloc,funlux,dzerox,dsfinv,radapt,etime,getnum,ncocti
 "tr-prop    trprfn,trprop,trscsp,trspsc,trscsd,trsdsc,Ssmt5t,Xmm55 "
 ]
+CDE,GCFLAG.
  Integer  SystemF,Ix/0/;  Real x/0.0/;  Double Precision d/0.D0/
  list external;
* make sure that real calls will never be done even if this routine is called
  Ix = Ix+1;  if (Ix<=0) Return;
  Ix = Ix+1;  if (Ix>=0) Return;
  ix = SystemF(' ')+JattF(ix);
  x  = sind(x)+asind(x)+cosd(x)+acosd(x)+tand(x)+atand(x)+atan2d(x,x)
  d  = dsind(d)+dasind(d)+dcosd(d)+dacosd(d)+dtand(d)+datand(d)+datan2d(d,d)
  x  = mod(Ix,ix+1)+amod(x,x+1)+dmod(d,d+1.)
  x=ifromc(' '); c=cfromi(0)

* now fake calls to library - no need for arguments
  list ref;
*
  END

  subroutine message(line,i,j)
  character  Line*(*)
  print *,%L(line)
  entry msg_enable
  entry msg_enabled
  end
