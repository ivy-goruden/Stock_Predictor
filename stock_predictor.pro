QT += widgets
SOURCES += main.cpp
# Header files (optional, but helps IDEs and qmake detect dependencies)
HEADERS += $$files(classes/*.hpp)
HEADERS +=$$files(include/*.hpp, true)

# .ui files from Qt Designer (optional)
# FORMS += mainwindow.ui
TARGET = main