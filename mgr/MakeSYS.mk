#  $Id: MakeSYS.mk,v 1.4 1998/03/25 16:13:53 nevski Exp $
#  $Log: MakeSYS.mk,v $
#  Revision 1.4  1998/03/25 16:13:53  nevski
#  old fashion gstar setup
#
#  Revision 1.3  1998/03/23 02:31:43  fisyak
#  move staff in group_dir
#
#  Revision 1.2  1998/03/09 14:36:31  fisyak
#  Switch varibales
#
#  Revision 1.1  1998/03/09 13:31:50  fisyak
#  Remove environment Variables
#
#  Revision 1.6  1998/02/22 02:07:10  fisyak
#  Add DATA
#
#  Revision 1.5  1998/02/17 18:06:48  fisyak
#  Add dropit for PATH
#
#  Revision 1.4  1998/02/13 14:18:20  fisyak
#  Simplify Makefile, reduce SLibrary
#
#  Revision 1.3  1998/02/12 13:35:09  fisyak
#  Add versioning, new Makefile with domain/package libraries
#
#  Revision 1.2  1998/02/10 00:06:10  fisyak
#  SL98a second version
#
#  Revision 1.1  1998/01/31 23:32:52  fisyak
#  New Environment variables
#
#  Revision 1.2  1998/01/30 12:42:16  fisyak
#  Save changes before moving to SL97b
#
#  Revision 1.1.1.1  1997/12/31 14:35:23  fisyak
#
#             Last modification $Date: 1998/03/25 16:13:53 $ 
ALL_DEPS    = $^
FIRST_DEP   = $<
FIRSTF      = $(<D)/$(<F)
ALL_TAGS    = $@
STEM        = $*
STEMF       = $(*D)/$(*F)
STIC        = stic
KUIPC      := kuipc
KUIPC_FLAGS+=
EMPTY      :=
FOUR       :=4
TWO        :=2
#
CC         := gcc
CXX        := gcc
ARFLAGS    := rvu
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),rs_aix31 rs_aix32 rs_aix41))
CXX        := xlC
FFLAGS     +=-O -qextname -qcharlen=6000 -WF,-D
F_EXTENDED :=-e
LDFLAGS    +=-bnoentry -bE:$*.exp import.map -bh:8 -T512 -H512
LD_LIBS    :=-lld -lxlf90 -lxlf -lm -lc
OPSYS      := AIX
endif
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),alpha_osf32c))
CXX        := cxx
CFLAGS     += -w
FFLAGS     += -pic  -static -fpe2
F_EXTENDED := -extend_source
LDFLAGS    += -shared -expect_unresolved \*\
LD_LIBS    := lUfor -lfor -lFutil -lm -lm_4sqrt -lots -lc
OPSYS      := OSF1V32
endif
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),hp700_ux90))
CXX        :=      CC
CXXFLAGS   += -w -O +a1
CFLAGS     +=  +z  -Aa -D_HPUX_SOURCE
FFLAGS     += +ppu +z +O2
LDFLAGS    += -b +a1 -z
LD_LIBS    := /opt/fortran/lib/libU77.a
CC_LIBS    := -L/opt/CC/lib -lC.ansi -lcxx -lcl -lc
endif
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),hp_ux102 hp_ux102_aCC))
CXX        := /opt/aCC/bin/aCC
#CXXFLAGS   +=  -z +Z -w                              # from ROOT
CXXFLAGS   += -w -z +Z +DAportable                   # P.Nevski
CC         := cc
#CFLAGS     += +DAportable +Z  -Wl,+s,+b${STAR_LIB} -Ae    # from DSPACK
CFLAGS     +=  +z -Aa +DAportable -D_HPUX_SOURCE            # P.Nevski 
FC         := /opt/fortran/bin/fort77
FFLAGS     += +DAportable +U77 +ppu +B +Z                    # from DSPACK
F_EXTENDED := +es
LDFLAGS    += -b                                     # from DSPACK 
LD         := ld                                     # from DSPACK
#LD_LIBS    := -L/opt/fortran/lib -lU77 -lV3 -lm -lf -lC
#LD         := aCC                                    # P.Nevski
#LDFLAGS    += -w -z +Z -g +DAportable -Wl,-E         # P.Nevski
LD_LIBS    := -lCsup -lstream                        # P.Nevski
CERN_LIBS  := 
endif
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),sgi_52 sgi_53))
FFLAGS     +=-Nn20000 -static -trapuv -KPIC
#                                                  -u
F_EXTENDED :=-extend_source
CC         := cc
CFLAGS     +=   -KPIC -kpicopt -w
CXX        := CC
CXXFLAGS   +=  -32 ${CFLAGS} -xansi -w
ARFLAGS    :=     slrvu
LD         := CC
LDFLAGS    += -32 -shared
        LD_LIBS    := -lsun -lftn -lm -lc -lPW -lmalloc
OPSYS      := IRIX53
endif
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),sgi_63 sgi_64))
FFLAGS     +=-32 -Nn20000 -static -trapuv -KPIC
#                                                       -u
F_EXTENDED :=-extend_source
CC         := cc
CFLAGS     +=   -32 -ansi -KPIC -kpicopt -w
CXX        :=  CC
CXXFLAGS   +=  ${CFLAGS} -xansi -use_cfront -w
LD         := CC
LDFLAGS    += -32 -shared
LD_LIBS    := -lXext -lm
OPSYS      := IRIX64_32
else
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),sgi_64_n32))
CXX        :=  CC
CXXFLAGS   += -n32
CFLAGS     += -n32
FFLAGS     += -n32 -Nn20000 -O2
F_EXTENDED := -extend_source
LDFLAGS    += -n32 -shared
LD_LIBS    := -lXext -lm
OPSYS      := IRIX64_n32
endif
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),sgi_64_64))
CXX        :=  CC
CXXFLAGS   += -64
CFLAGS     += -64
FFLAGS     += -64 -Nn20000 -O2
F_EXTENDED := -extend_source
LDFLAGS    += -64 -shared
LD_LIBS    := -lXext -lm
OPSYS      := IRIX64_64
endif
endif
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),i386_linux2))
#  ====================
FFLAGS     +=-w -O2 
F_EXTENDED :=-e 
LDFLAGS    += -shared
LD_LIBS    := -ldl -L/usr/X11R6/lib/ -lX11 -lXt
OPSYS      := Linux
CPPFLAGS   += -Dlynx
ifdef PGI
FC         := /usr/pgi/linux86/bin/pgf77
F_EXTENDED := -Mextend
LD_LIBS    := -ldl -L/usr/X11R6/lib/ -lX11 -lXt -L/usr/local/lib/ -lF77 -lI77  -L/usr/pgi/linux86/lib/ -lpgftnrtl -lpgc
#CC         := /usr/pgi/linux86/bin/pgcc -g77libs 
#CXX        := /usr/pgi/linux86/bin/pgCC
endif
endif
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),sun4m_54 sun4m_55 sun4x_55 sun4x_56))
CPPFLAGS   := -DSUN -DSOLARIS -Dsun
FFLAGS     +=-xl -PIC -w
F_EXTENDED :=-e
CC         := /opt/SUNWspro/bin/cc         # V.P.
CFLAGS     += -KPIC                        # V.P.
CXX        := /opt/SUNWspro/bin/CC         # V.P.
CXXFLAGS   += -KPIC                        # V.P.
#CC         := gcc
#CFLAGS     += -ansi -fpic -fPIC -w
#CXX        := g++
#CXXFLAGS   += -w
#LD         := $(CXX)                      # V.P.
LDFLAGS    += -G
LD_LIBS    := -L/opt/SUNWspro/lib -L/opt/SUNWspro/SC4.2/lib -lM77 -lF77 -lm -lc -L/usr/ucblib -R/usr/ucblib -lucb
CC_LIBS    := -L/usr/ucblib -R/usr/ucblib -lm -ldl -lform -lmalloc
endif 
ifneq ($(EMPTY),$(findstring $(STAR_HOST_SYS),sunx86_55))
CXX        :=  /opt/SUNWspro/bin/CC
CC         :=  /opt/SUNWspro/bin/cc
CXXFLAGS   +=  -KPIC +w2                      # V.P.
CFLAGS     +=  -KPIC +w2
FFLAGS     +=-KPIC                         # V.P. -xlX[
F_EXTENDED :=-e
LDFLAGS    += -G -t -z muldefs
LD_LIBS    :=-L/opt/SUNWspro/lib -L/opt/SUNWspro/SC4.2/lib -lM77 -lF77 -lm -lc -L/usr/ucblib -R/usr/ucblib -lucb -lsunmath
OPSYS      := sun4os5pc
endif
# defaul flags
ifeq ($(EMPTY),$(CFLAGS))
CFLAGS     += -fpic -w
endif
ifeq ($(EMPTY),$(CXXFLAGS))
CXXFLAGS   += -fpic -w
endif





