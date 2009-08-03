######################################################################
# Automatically generated by qmake (1.07a) Tue Jun 14 13:51:05 2005
######################################################################

QT += qt3support

QMAKE_RPATH=


TEMPLATE = lib 
CONFIG += thread dll
CONFIG += create_prl

DEPENDPATH += inc src

exists ($(ROOTSYS)/include){
   DEPENDPATH += $(ROOTSYS)/include
}
TARGET = QtThread

GQTDIRI   = ../qt/inc

QTROOTSYSPATHINSTALL = $(QTROOTSYSDIR)
QT_VERSION=$$[QT_VERSION]
contains( QT_VERSION, "^4.*" ) {
    QTROOTSYSPATHINSTALL = $$(QTROOTSYSDIR)
}

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

QTTHREADDIRI = inc
QTTHREADDIRS = src

INCLUDEPATH += $$QTTHREADDIRS $$QTTHREADDIRI $(ROOTSYS)/include

#   List of the package classes the ROOT dictinary should be created for
#
QTTHREADH1  = $$QTTHREADDIRI/TQtCondition.h     \
              $$QTTHREADDIRI/TQtMutex.h         \
              $$QTTHREADDIRI/TQtThreadFactory.h \
              $$QTTHREADDIRI/TQtThreadImp.h     \
              $$QTTHREADDIRI/LinkDef.h

unix {
  LIBS += -L.. 
}
CREATE_ROOT_DICT_FOR_CLASSES  = $$QTTHREADH1

    !exists ($$GQTDIRI/rootcintrule.pri){
        message ("The rootcintrule.pri was not found")
    }
    exists ($$GQTDIRI/rootcintrule.pri){
       include ($$GQTDIRI/rootcintrule.pri)
    }

mac {
    !exists (../qt/inc/rootlibs.pri){
        message "The rootlibs.pri was not found"
    }
    exists ($$GQTDIRI/rootlibs.pri){
       include ($$GQTDIRI/rootlibs.pri)
    }
}

mac: unix: LIBS += -lThread

headerfiles.path  = $$QTROOTSYSPATHINSTALL/include/
headerfiles.files = $$QTTHREADDIRI/*.*
headerfiles.files -= $$QTTHREADDIRI/LinkDef.h
target.path = $$QTROOTSYSPATHINSTALL/lib/

INSTALLS += headerfiles target

# Input
HEADERS += $$QTTHREADDIRI/TQtCondition.h     \
           $$QTTHREADDIRI/TQtMutex.h         \
           $$QTTHREADDIRI/TQtThreadFactory.h \
           $$QTTHREADDIRI/TQtThreadImp.h
           
SOURCES += $$QTTHREADDIRS/TQtCondition.cxx     \
           $$QTTHREADDIRS/TQtMutex.cxx         \
           $$QTTHREADDIRS/TQtThreadFactory.cxx \
           $$QTTHREADDIRS/TQtThreadImp.cxx

           
DISTFILES += LICENSE.QPL  Module.mk  Module.mk.unix  Module.mk.win32

unix {
#  -- working directory 
  
  ROOTCINT_DIR = .rootcint
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj

}
