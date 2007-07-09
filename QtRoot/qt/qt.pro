######################################################################
# Automatically generated by qmake (1.07a) Tue Jun 14 13:51:05 2005
######################################################################

QT += qt3support

TEMPLATE = lib 
CONFIG += thread dll
CONFIG += create_prl

DEPENDPATH += inc src

QT_VERSION=$$[QT_VERSION]
contains( QT_VERSION, "^4.*" ) {
#  DEFINES += QT_VERSION=0x40000
#   INCLUDEPATH +=$(QTDIR)/include  $(QTDIR)/include/ActiveQt  $(QTDIR)/include/Qt  $(QTDIR)/include/Qt3Support $(QTDIR)/include/QtAssistant  $(QTDIR)/include/QtCore  $(QTDIR)/include/QtGui  $(QTDIR)/include/QtNetwork   $(QTDIR)/include/QtOpenGL   $(QTDIR)/include/QtXml
#   LIBS += -L$(QTDIR)/lib 
}
win32 {
  DEFINES +=  GDK_WIN32
}


exists ($(ROOTSYS)/include){
   DEPENDPATH += $(ROOTSYS)/include
}

unix:  TARGET = GQt
win32: TARGET = libGQt

QTROOTSYSPATHINSTALL = $(QTROOTSYSDIR)

isEmpty(DESTDIR) {
  DESTDIR=..
}
isEmpty(QTROOTSYSPATHINSTALL) {
  QTROOTSYSPATHINSTALL = $$DESTDIR
}

#!isEmpty(QTROOTSYSPATHINSTALL) {
#  DESTDIR=$$QTROOTSYSDIR
#}
#isEmpty(DESTDIR) {
#  DESTDIR=..
#}

#QTROOTSYSDIR=$$DESTDIR

GQTDIRI = inc
GQTDIRS = src

INCLUDEPATH += $$GQTDIRS $$GQTDIRI $(ROOTSYS)/include

#   List of the package classes the ROOT dictinary should be created for
#
GQTH1  =  $$GQTDIRI/TGQt.h            $$GQTDIRI/TQtApplication.h  $$GQTDIRI/TQtTimer.h    \
          $$GQTDIRI/TQtBrush.h        $$GQTDIRI/TQMimeTypes.h $$GQTDIRI/TQtClientFilter.h \
          $$GQTDIRI/TQtClientWidget.h $$GQTDIRI/TQtWidget.h   $$GQTDIRI/TQtMarker.h       \
          $$GQTDIRI/TQtRootSlot.h
          

CREATE_ROOT_DICT_FOR_CLASSES  = $$GQTH1 $$GQTDIRI/LinkDef.h

    !exists ($$GQTDIRI/rootcintrule.pri){
        message "The rootcintrule.pri was not found"
    }
    exists ($$GQTDIRI/rootcintrule.pri){
       include ($$GQTDIRI/rootcintrule.pri)
    }

win32 {
#  copy from rootlibs.pri for GQt project
   LIBS	+=                                                                                                \
      -include:_G__cpp_setupG__Hist       -include:_G__cpp_setupG__Graf1  -include:_G__cpp_setupG__G3D     \
      -include:_G__cpp_setupG__GPad       -include:_G__cpp_setupG__Tree   -include:_G__cpp_setupG__Rint    \
      -include:_G__cpp_setupG__PostScript -include:_G__cpp_setupG__Matrix -include:_G__cpp_setupG__Physics \
      -include:_G__cpp_setupG__Gui1       -include:_G__cpp_setupG__Geom1   
    
   
   LIBS	+=                                                                                         \
    $(ROOTSYS)/lib/libCore.lib   $(ROOTSYS)/lib/libCint.lib     $(ROOTSYS)/lib/libHist.lib         \
    $(ROOTSYS)/lib/libGraf.lib   $(ROOTSYS)/lib/libGraf3d.lib   $(ROOTSYS)/lib/libGpad.lib         \
    $(ROOTSYS)/lib/libTree.lib   $(ROOTSYS)/lib/libRint.lib     $(ROOTSYS)/lib/libPostscript.lib   \
    $(ROOTSYS)/lib/libMatrix.lib $(ROOTSYS)/lib/libPhysics.lib  $(ROOTSYS)/lib/libGui.lib          \
    $(ROOTSYS)/lib/libGeom.lib    

    LIBS+=comctl32.lib
}
unix {
   LIBS += -L$(ROOTSYS)/lib -lGui  -lGpad -lRint
}
mac {
    !exists ($$GQTDIRI/rootlibs.pri){
        message "The rootlibs.pri was not found"
    }
    exists ($$GQTDIRI/rootlibs.pri){
       include ($$GQTDIRI/rootlibs.pri)
    }
#   LIBS += -L$(ROOTSYS)/lib -lGpad -lRint
}
unix {
    LIBS -= -lGQt
    LIBS -= -lQtRootGui
}
    
headerfiles.path  = $$QTROOTSYSPATHINSTALL/include/
headerfiles.files = $$GQTDIRI/*.h $$GQTDIRI/*.cw $$GQTDIRI/*.pri  $$GQTDIRI/TVirtualX.interface.h
headerfiles.files -= $$GQTDIRI/LinkDef.h
unix:  target.path = $$QTROOTSYSPATHINSTALL/lib
win32: target.path = $$QTROOTSYSPATHINSTALL/bin

INSTALLS += headerfiles target 

mac {
 sharedsolink.files = $${DESTDIR}/$$sharedso.target
 sharedsolink.path  = $$QTROOTSYSPATHINSTALL/lib
 sharedsolink.commands = (ln -sf "$$sharedso.target" "$$sharedsolink.path" )
#  INSTALLS += sharedsolink
 message ( PLEASE  make the symbolic link  $$sharedsolink.commands by hand!!!  )
}

# Input
HEADERS += $$GQTDIRI/TGQt.h \
           $$GQTDIRI/TObjectExecute.h \
           $$GQTDIRI/TQMimeTypes.h \
           $$GQTDIRI/TQtApplication.h \
           $$GQTDIRI/TQtBrush.h \
           $$GQTDIRI/TQtClientFilter.h \
           $$GQTDIRI/TQtClientGuard.h \
           $$GQTDIRI/TQtClientWidget.h \
           $$GQTDIRI/TQtEvent.h \
           $$GQTDIRI/TQtEventQueue.h \
           $$GQTDIRI/TQtMarker.h \
           $$GQTDIRI/TQtRConfig.h \
           $$GQTDIRI/TQtRootApplication.h \
           $$GQTDIRI/TQtSymbolCodec.h \
           $$GQTDIRI/TQtTimer.h \
           $$GQTDIRI/TQtUtil.h \
           $$GQTDIRI/TQtWidget.h \
           $$GQTDIRI/TQUserEvent.h \
           $$GQTDIRI/TVirtualX.interface.h  \
           $$GQTDIRI/TWaitCondition.h       \
           $$GQTDIRI/TQtLock.h              \
           $$GQTDIRI/TQtEmitter.h           \
           $$GQTDIRI/TQtLockGuard.h         \
           $$GQTDIRI/TQtRootSlot.h
           
SOURCES += $$GQTDIRS/GQtGUI.cxx             \
           $$GQTDIRS/TGQt.cxx               \
           $$GQTDIRS/TGQtDummy.cxx          \
           $$GQTDIRS/TQMimeTypes.cxx        \
           $$GQTDIRS/TQtApplication.cxx     \
           $$GQTDIRS/TQtBrush.cxx           \
           $$GQTDIRS/TQtClientFilter.cxx    \
           $$GQTDIRS/TQtClientGuard.cxx     \
           $$GQTDIRS/TQtClientWidget.cxx    \
           $$GQTDIRS/TQtEvent.cxx           \
           $$GQTDIRS/TQtEventQueue.cxx      \
           $$GQTDIRS/TQtMarker.cxx          \
           $$GQTDIRS/TQtRootApplication.cxx \
           $$GQTDIRS/TQtSymbolCodec.cxx     \
           $$GQTDIRS/TQtTimer.cxx           \
           $$GQTDIRS/TQtWidget.cxx          \
           $$GQTDIRS/TQtRootSlot.cxx
           
DISTFILES += LICENSE.QPL  Module.mk  Module.mk.unix  Module.mk.win32

unix {
#  -- working directory 
  
  ROOTCINT_DIR = .rootcint
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj

}
#The following line was inserted by qt3to4
QT +=  
