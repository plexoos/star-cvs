######################################################################
# Automatically generated by qmake (2.00a) Mon Jun 13 19:09:35 2005
######################################################################

TEMPLATE = lib 
QT += webkit
CONFIG += thread dll
CONFIG += create_prl
DEPENDPATH += inc src
# DEFINES += QT_QTCOLORPICKER_EXPORT
TARGET = QtRootGui

QMAKE_RPATH=

GQTDIRI   = ../qt/inc

QTROOTSYSPATHINSTALL = $(QTROOTSYSDIR)
QT_VERSION=$$[QT_VERSION]
contains( QT_VERSION, "^4.*" ) {
    QTROOTSYSPATHINSTALL = $$(QTROOTSYSDIR)
}

contains( QT_VERSION, "^4.*" ) {
}

isEmpty(DESTDIR) {
  DESTDIR=..
}
isEmpty(QTROOTSYSPATHINSTALL) {
  QTROOTSYSPATHINSTALL = $$DESTDIR
}

DEFINES += R__QT
MODDIRI = inc

INCLUDEPATH +=  $$MODDIRI

QTGUIH1        = $$MODDIRI/TQtGuiFactory.h     $$MODDIRI/TBrowserCustom.h  $$MODDIRI/TQtColorSelect.h  \
                 $$MODDIRI/TQtPatternSelect.h  $$MODDIRI/TEmbeddedPad.h    $$MODDIRI/TQtTabValidator.h \
                 $$MODDIRI/TQtZoomPadWidget.h  $$MODDIRI/TQGsiRootCanvas.h $$MODDIRI/TQtPad2Html.h     \
                 $$MODDIRI/TQtCanvas2Html.h    $$MODDIRI/TQtPixmapBox.h    $$MODDIRI/TQtMarkerSelect.h



CREATE_ROOT_DICT_FOR_CLASSES  = $$QTGUIH1 $$MODDIRI/LinkDef.h

unix {
  LIBS += -L..
}

INCLUDEPATH  *= "$$GQTDIRI"
ROOTCINTFOUND =

    exists ($$GQTDIRI/rootcintrule.pri){
         ROOTCINTFOUND ="$$GQTDIRI/rootcintrule.pri"
         INCLUDEPATH  *= "$$GQTDIRI"
         DEPENDPATH   *= "$$GQTDIRI"
    }
    isEmpty(ROOTCINTFOUND) {
       exists ($(ROOTSYS)/include/rootcintrule.pri){
           ROOTCINTFOUND ="$(ROOTSYS)/include/rootcintrule.pri"
       }
    }

    isEmpty(ROOTCINTFOUND) {
        message ("qtgui: The rootcintrule.pri was not found")
     }

include ("$$ROOTCINTFOUND")
    
ROOTCINTFOUND =

    exists ($$GQTDIRI/rootlibs.pri){
         ROOTCINTFOUND ="$$GQTDIRI/rootlibs.pri"
    }
    isEmpty(ROOTCINTFOUND) {
       exists ($(ROOTSYS)/include/rootlibs.pri){
           ROOTCINTFOUND ="$(ROOTSYS)/include/rootlibs.pri"
       }
    }

    isEmpty(ROOTCINTFOUND) {
        message ("$$TARGET: The rootlibs.pri was not found")
     }

#include ("$$ROOTCINTFOUND")
LIBS        += $$system(${ROOTSYS}/bin/root-config --glibs)
# remove the posible refs to itself
LIBS -= -lQtRootGui

LIBS += -L$(QTROOTSYSDIR)/lib -lGQt

exists ($(ROOTSYS)/include){
   DEPENDPATH += $(ROOTSYS)/include
}

FORMS += TQtRootCommand.ui

# Input
HEADERS += inc/TBrowserCustom.h       \
           inc/TEmbeddedPad.h         \
           inc/TCanvasImp.interface.h \
           inc/TQtBrowserImp.h        \
           inc/TQtCanvasImp.h         \
           inc/TQtCanvasWidget.h      \
           inc/TQtColorSelect.h       \
           inc/TQtColorSelectButton.h \
           inc/TQtContextMenuImp.h    \
           inc/TQtControlBarImp.h     \
           inc/TQtFloatSpinBox.h      \
           inc/TQtGui.h               \
           inc/TQtGuiFactory.h        \
           inc/TQtIconBrowserImp.h    \
           inc/TQtInspectImp.h        \
           inc/TQtObjectDialog.h      \
           inc/TQtObjectListItem.h    \
           inc/TQtPatternSelect.h     \
           inc/TQtPatternSelectButton.h \
           inc/TQtRootAction.h        \
           inc/TQtRootBrowserAction.h \
           inc/TQtRootBrowserImp.h    \
           inc/TQtTabValidator.h      \
           inc/TQtUpdateViewFlag.h    \
           inc/TQtZoomPadWidget.h     \
           inc/TQGsiRootCanvas.h      \
           inc/TQtPad2Html.h          \
           inc/TQtCanvas2Html.h       \
           inc/TQtToolBar.h           \
           inc/TQtStyleComboBox.h     \
           inc/TQtLockWidget.h        \
           inc/TQtPixmapBox.h         \
           inc/TQtMarkerSelect.h      \
           inc/TQtMarkerSelectButton.h\
           inc/TQtFloatSlider.h       \
           inc/TQtRootCommandCombo.h  \
           inc/qtcolorpicker.h        \
           inc/qtmmlwidget.h          \
           inc/QtMmlWidget            \
           inc/QtMmlDocument          \
           inc/TQtMmlTextProxy.h      \
           inc/TQtColorPickerHelper.h \
           inc/TQtCommandPlugin.h     \
           inc/TQtCustomizeCanvasMenu.h\
           inc/TQtTextEditor.h          \
           inc/TRedirectGuardInterface.h 
           


HEADERS -= inc/LinkDef.h

SOURCES += src/TQtMarkerSelect.cxx      \
           src/TQtMarkerSelectButton.cxx\
           src/TQtPixmapBox.cxx         \
           src/TBrowserCustom.cxx       \
           src/TEmbeddedPad.cxx         \
           src/TQtBrowserImp.cxx        \
           src/TQtCanvasImp.cxx         \
           src/TQtCanvasWidget.cxx      \
           src/TQtColorSelect.cxx       \
           src/TQtColorSelectButton.cxx \
           src/TQtContextMenuImp.cxx    \
           src/TQtControlBarImp.cxx     \
           src/TQtFloatSpinBox.cxx      \
           src/TQtGuiFactory.cxx        \
           src/TQtIconBrowserImp.cxx    \
           src/TQtInspectImp.cxx        \
           src/TQtObjectDialog.cxx      \
           src/TQtPatternSelect.cxx     \
           src/TQtPatternSelectButton.cxx \
           src/TQtRootAction.cxx        \
           src/TQtRootBrowserAction.cxx \
           src/TQtRootBrowserImp.cxx    \
           src/TQtCustomizeCanvasMenu.cxx \
           src/TQtTabValidator.cxx      \
           src/TQtZoomPadWidget.cxx     \
           src/TQRootCanvas.cxx         \
           src/TQtPad2Html.cxx          \
           src/TQtCanvas2Html.cxx       \
           src/TQtToolBar.cxx           \
           src/TQtStyleComboBox.cxx     \
           src/TQtGui.cxx               \
           src/TQtFloatSlider.cxx       \
           src/TQtRootCommandCombo.cxx  \
           src/qtcolorpicker.cxx        \
           src/qtmmlwidget.cxx          \
           src/TQtMmlTextProxy.cxx      \
           src/TQtCommandPlugin.cxx     \
           src/TQtTextEditor.cxx        \
           src/TRedirectGuardInterface.cxx \
           src/TQtObjectListItemInterface.cxx
           
           
DISTFILES += LICENSE.QPL Module.mk  Module.mk.unix  Module.mk.win32 

headerfiles.path   = $$QTROOTSYSPATHINSTALL/include/
headerfiles.files  = $$HEADERS inc/*.cw
headerfiles.files -= $$MODDIRI/LinkDef.h

plugins.path   = $$QTROOTSYSPATHINSTALL/etc/plugins/TGuiFactory/
plugins.files  = plugins/TGuiFactory/*.C

target.path = $$QTROOTSYSPATHINSTALL/lib/

INSTALLS += headerfiles target plugins

unix {
#  -- working directory 
!contains( QT_VERSION, "^4.*" ) {
#
#  generate the link to the old QtGui name
#
  QtGuiOldName.path   = $$QTROOTSYSPATHINSTALL/lib  
  QtGuiOldName.target   = libQtGui.so
  QtGuiOldName.files += $$QtGuiOldName.target
  QtGuiOldName.commands = @cd $$QtGuiOldName.path; rm -rf libQtGui.so; ln -s $(TARGET) $$QtGuiOldName.target
  #QMAKE_EXTRA_UNIX_TARGETS += QtGuiOldName 
  #PRE_TARGETDEPS  += $$QtGuiOldName.target
  #QMAKE_CLEAN     += $$QtGuiOldName.target
  
  INSTALLS += QtGuiOldName
}
#  -- working directory 
  ROOTCINT_DIR = .rootcint
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj

}
#The following line was inserted by qt3to4
QT +=  qt3support 

QT +=


