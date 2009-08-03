######################################################################
# Automatically generated by qmake (1.07a) Thu Jun 16 17:07:54 2005
######################################################################

QT += qt3support

CONFIG += ordered
TEMPLATE = subdirs
win32: DEFINES +=  _CRT_SECURE_NO_WARNINGS
#  qt4ged
QTPACKAGES = qt qtgui qtgl qtthread qtimage 
unix: QTPACKAGES +=  qtroot
QT_VERSION=$$[QT_VERSION]

QTROOTSYS = $$(QTROOTSYSDIR)

contains( QT_VERSION, "^4.*" ) {
  QTPACKAGES += qt4ged
}
!contains( QT_VERSION, "^4.*" ) {
  QTPACKAGES += qtged
}
QTPACKAGES -= $$REMOVEPACKAGES
#qtExamples
SUBDIRS = $$QTPACKAGES
message (excluding $$EXCLUDE)
# qtx3d qtExamples qtetc

TARGET = QtRoot
!isEmpty(QTROOTSYS) 
{
   QTROOTSYSPATHINSTALL = $$QTROOTSYS
}

isEmpty(QTROOTSYSPATHINSTALL) {
  QTROOTSYSPATHINSTALL = $$DESTDIR
  message("Warning:  Did you forget to set QTROOTSYSDIR environment variable? $$QTROOTSYSDIR")
  message(" ")
  message(" Installing <$$QTPACKAGES> of $$TARGET system")
  message(" To remove some packages from the build, start the qmake utility as follows:")
  message(" ")
  message(" > qmake "REMOVEPACKAGES=<list of unwanted packages>"")
  message(" ")
  message("Usage: ")
  message("----- ")
  message(">       cvs -d :pserver:cvsuser@cvs.bnl.gov:/data01/CVS  login")
  message(">       password: cvsuser")
  message(">       cvs -d :pserver:cvsuser@cvs.bnl.gov:/data01/CVS co -d QtRoot root")
  message(">       cd QtRoot")
  message(" ")
  message("    To install QtRoot into your current ROOT area do")
  message(" ")
  message(">       setenv QTROOTSYSDIR $ROOTSYS")
  message("   QTROOTSYS variable must define an absolute path ")  
  message(">       qmake qtRoot.pro")
  message(">       make install")
  message(" ")
  message(" clean up ")
  message(" ")
  message(">       make distclean")
  message(" ")
  message("--------------------- http://root.bnl.gov ----------------------------")
  message(" Adjust your .rootrc or system.rootrc file")
  message(" See: http://root.bnl.gov/QtRoot/root/root.diff/config/unix.rootrc for details")
  unix:message(" setenv LD_LIBRARY_PATH $QTROOTSYSDIR/lib:$LD_LIBRARY_PATH")
  win32:message(" PATH %QTROOTSYSDIR%/bin;%PATH%")
  message("-----------------------------------------------------------------------")
  message(" ")
}

!isEmpty(QTROOTSYSPATHINSTALL) {
   message(" Installing <$$QTPACKAGES> of QtRoot system")
   message(" To remove some packages from the build, start the qmake utility as follows:")
   message(" ")
   message(" > qmake "QTPACKAGES -= <list of unwanted packages>"")
   message(" ")
   message ("type <make install> - to install $$QTPACKAGES of $$TARGET into <$$QTROOTSYSPATHINSTALL> directory")
   message(" ")
}
