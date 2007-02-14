# Module.mk for qtcoin module
# Copyright (c) 2006 [BNL] Valeri Fine
# 
# Author: Valeri Fine, 11/11/2002

QMAKE        := $(QTDIR)/bin/qmake

MODDIR       := qtgl
MODDIRS      := $(MODDIR)/qtcoin/src
MODDIRI      := $(MODDIR)/qtcoin/inc

QTCOIN3DDIR      := $(MODDIR)/qtcoin
QTCOIN3DDIRS     := $(QTCOIN3DDIR)/src
QTCOIN3DDIRI     := $(QTCOIN3DDIR)/inc

ifeq ($(ARCH),win32)
QTCOIN3DMAKE   := nmake
else
QTCOIN3DMAKE   := make
endif

# remeber the current directory

CURRENTROOT = $(shell pwd)

##### libRQTCOIN3D #####
QTCOIN3DL       := $(QTCOIN3DDIRI)/LinkDef.h
QTCOIN3DDS      := $(QTCOIN3DDIRS)/G__QTCOIN3D.cxx
QTCOIN3DDO      := $(QTCOIN3DDS:.cxx=.o)
QTCOIN3DDH      := $(QTCOIN3DDS:.cxx=.h)

QTCOIN3DH1      := $(QTCOIN3DDIRI)/TQtCoinViewerImp.h     $(QTCOIN3DDIRI)/TQtCoinWidget.h         \
                   $(QTCOIN3DDIRI)/TQtRootCoinViewer3D.h  $(QTCOIN3DDIRI)/TQtCoin3DDefInterface.h \
                   $(QTCOIN3DDIRI)/TGeoTranslationC.h


QTCOIN3DH       := $(filter-out $(QTCOIN3DDIRI)/LinkDef%,$(wildcard $(QTCOIN3DDIRI)/*.h))
               
COINFLAGS     += -I. -I$(QTINCDIR) -DQT_DLL -DQT_THREAD_SUPPORT

QTCOININCDIR   := $(IVROOT)/include $(IVROOT)/include/annex

COINFLAGS     += -DSOQT_DLL -DCOIN_DLL  -DSMALLCHANGE_DLL -DSIMAGE_DLL
#  COINLIBS      := $(OPENIVLIBDIR) $(filter-out %Xt,$(OPENIVLIB))
 
QTCOIN3DMOC     := $(subst $(QTCOIN3DDIRI)/,$(QTCOIN3DDIRS)/moc_,$(patsubst %.h,%.cxx,$(QTCOIN3DH)))
QTCOIN3DMOCO    := $(QTCOIN3DMOC:.cxx=.o)

QTCOIN3DS       += TObjectCoinViewFactory.cxx  TQtCoinWidget.cxx      TSimageMovie.cxx          \
                   TCoinShapeBuilder.cxx       TQtCoinViewerImp.cxx   TQtRootCoinViewer3D.cxx   \
                   TQtCoin3DDefInterface.cxx
                    
QTCOIN3DS       := $(patsubst %,$(QTCOIN3DDIRS)/%,$(QTCOIN3DS))
QTCOIN3DLIB     := $(LPATH)/libRQIVTGL.$(SOEXT)
QTCOIN3DO       := $(QTCOIN3DS:.cxx=.o)
QTCOIN3DDEP     := $(QTCOIN3DO:.o=.d) $(QTCOIN3DDO:.o=.d)

# used in the main Makefile
ALLHDRS     += $(patsubst $(QTCOIN3DDIRI)/%.h,include/%.h,$(QTCOIN3DH))
ALLHDRS     += $(patsubst $(QTCOIN3DDIRI)/%.h,include/%.h,$(QTCOINH))
ALLLIBS     += $(QTCOIN3DLIB) $(QTCOINLIB)
# include all dependency files
INCLUDEFILES += $(QTCOIN3DDEP) 

##### local rules #####
include/%.h:    $(QTCOIN3DDIRI)/%.h
		cp $< $@
		
		
$(QTCOIN3DLIB):       $(QTCOIN3DO) $(QTCOIN3DDO)  $(QTCOIN3DMOCO) $(MAINLIBS) $(QTCOIN3DLIBDEP)
		@$(MAKELIB) $(PLATFORM) $(LD) "$(LDFLAGS)" \
		   "$(SOFLAGS)" libRQTCOIN3D.$(SOEXT) $@ "$(QTCOIN3DO) $(QTCOIN3DDO)  $(QTCOIN3DMOCO)" \
		   " $(QTCOIN3DLIBEXTRA) $(QTCOIN3DLIB:.dll=.lib) "

$(QTCOIN3DDS):    $(QTCOIN3DH1) $(QTCOIN3DL) $(ROOTCINTTMPEXE)
		@echo "Generating dictionary $@..."
		$(ROOTCINTTMP) -f $@ -c $(QTCOIN3DH1) $(QTCOIN3DL)

$(QTCOIN3DDO): $(QTCOIN3DDS)
		$(CXX) $(OPT) $(CXXFLAGS) $(COINFLAGS) $(GQTCXXFLAGS)  $(CXXOUT)$@ -c $<


all-qtcoin:         $(QTCOIN3DLIB) $(QTCOIN3DMOC)

map-qtcoin:       $(RLIBMAP)
		$(RLIBMAP) -r $(ROOTMAP) -l $(QTCOIN3DLIB) \
                  -d $(QTCOIN3DLIBDEP) -c $(QTCOIN3DL)

map::           map-qtcoin

clean-qtcoin:
		@rm -f $(QTCOIN3DO)  $(QTCOIN3DDO) $(QTCOIN3DMOC) $(QTCOIN3DMOCO)

clean::   clean-qtcoin

distclean-qtcoin:   clean-qtcoin
		@rm -f $(QTCOIN3DDEP) $(QTCOIN3DLIB)

distclean::     distclean-qtcoin

##### extra rules ######
ifneq ($(OIVHOME),)
QTCOINCXXFLAGS += -DR__OPENINVENTOR -I$(IVINCDIR)
else
QTCOINCXXFLAGS += -I$(QGLVIEWERDIR) $(COINFLAGS)
endif

$(sort $(QTCOIN3DMOCO) $(QTCOIN3DO)) : %.o: %.cxx
	$(CXX) $(OPT) $(CXXFLAGS) $(QTCOINCXXFLAGS) -Iqt/src $(QTCOININCDIR:%=-I%)  \
	    $(CXXOUT)$@ -c $<
##### extra rules ######

$(sort $(QTCOIN3DMOC) ): $(QTCOIN3DDIRS)/moc_%.cxx: $(QTCOIN3DDIRI)/%.h
	$(QTMOCEXE) $< -o $@
