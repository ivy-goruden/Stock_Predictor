QT += widgets printsupport
SOURCES += main.cpp \
		   Gui/gui.cpp \
		   Gui/graphControls.cpp \
		   Gui/graphDrawer.cpp \
		   include/qcustomplot.cpp
# Header files (optional, but helps IDEs and qmake detect dependencies)
HEADERS += $$files(classes/*.hpp)
HEADERS +=$$files(include/*.hpp, true)
HEADERS += Gui/gui.hpp \
		   Gui/graphControls.hpp \
		   Gui/graphDrawer.hpp \
		   include/qcustomplot.h

# .ui files from Qt Designer (optional)
# FORMS += mainwindow.ui
TARGET = main