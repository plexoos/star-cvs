######################################################################
# Automatically generated by qmake (1.07a) Thu Jun 30 15:19:04 2005
######################################################################

TEMPLATE = lib  thread
CONFIG *= dll

# Get the "lib" prefix to match the Unix default
win32 : TARGET = libHelloCint

# Input
HEADERS += TMyQButton.h
SOURCES += TMyQButton.cxx

CREATE_ROOT_DICT_FOR_CLASSES  = $$HEADERS LinkDef.h

includeFile = $(QTROOTSYSDIR)/include
exists ($$includeFile) {
  include ($$includeFile/rootcint.pri)
}

!exists ($$includeFile) {
  includeFile = $(ROOTSYS)/include/rootcint.pri
  exists ($$includeFile) {
    include ($$includeFile)
  }
}

#  -- Check for the obsolete ROOT version :(
!exists ($$inlcudeFile) {
    message (" ")
    message ("WARNING:  The $$inlcudeFile was not found !!!")
    message ("          The example $$TARGET can not be built")
    message ("          Please update your Qt layer version")
    message (" ")
    LIBS += $$system(root-config --glibs) -lGQt
    INCLUDEPATH += $(ROOTSYS)/include
}

#The following line was inserted by qt3to4
QT +=  qt3support
