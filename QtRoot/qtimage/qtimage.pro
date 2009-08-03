######################################################################
# Automatically generated by qmake (1.07a) Thu Jun 16 20:43:06 2005
######################################################################

TEMPLATE = lib

CONFIG += thread dll
CONFIG += create_prl

GQTDIRI   = ../qt/inc

QMAKE_RPATH=

TARGET = QtImage

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

unix {
  LIBS += -L..
}
 
QTIMAGEDIRS  = src
QTIMAGEDIRI  = inc
 
DEPENDPATH  += $$QTIMAGEDIRI $$QTIMAGEDIRS
INCLUDEPATH += $$QTIMAGEDIRI $$QTIMAGEDIRS

QTIMAGEL     = $$QTIMAGEDIRI/LinkDef.h

# Input
HEADERS += $$QTIMAGEDIRI/TQtImage.h              \
           $$QTIMAGEDIRI/TQtPaletteEditor.h
           
SOURCES += $$QTIMAGEDIRS/TQtImage.cxx \
           $$QTIMAGEDIRS/TQtPaletteEditor.cxx

CREATE_ROOT_DICT_FOR_CLASSES  = $$HEADERS $$QTIMAGEL

    !exists ($$GQTDIRI/rootcintrule.pri){
        message "The rootcintrule.pri was not found"
    }
    exists ($$GQTDIRI/rootcintrule.pri){
       include ($$GQTDIRI/rootcintrule.pri)
        INCLUDEPATH *= $$GQTDIRI
        DEPENDPATH  *= $$GQTDIRI
    }
mac {
    !exists ($$GQTDIRI/rootlibs.pri){
        message "The rootlibs.pri was not found"
    }
    exists ($$GQTDIRI/rootlibs.pri){
       include ($$GQTDIRI/rootlibs.pri)
    }
}

headerfiles.path   = $$QTROOTSYSPATHINSTALL/include/
headerfiles.files  = $$QTIMAGEDIRI/*.*
headerfiles.files -= $$QTIMAGEDIRI/LinkDef.h
target.path = $$QTROOTSYSPATHINSTALL/lib/

INSTALLS += headerfiles target

DISTFILES += LICENSE.QPL Module.mk

unix {
#  -- working directory 
  
  ROOTCINT_DIR = .rootcint
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj

}
#The following line was inserted by qt3to4
QT +=  qt3support 
