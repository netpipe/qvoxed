# Define the project
TEMPLATE = app
TARGET = YourAppName
QT += core gui

# Define your source and header files
SOURCES += main.cpp \
    mainwindow.cpp \
    osg_widget.cpp

HEADERS += \
    mainwindow.h \
    mainwindow.h \
    osg_utils.h \
    osg_widget.h

FORMS += \
    mainwindow.ui \
    mainwindow.ui

# Include OpenSceneGraph
INCLUDEPATH += /usr/local/include/osg
INCLUDEPATH += /usr/local/include/osgViewer
INCLUDEPATH += /usr/local/include/osgDB
INCLUDEPATH += /usr/local/include/osgGA
INCLUDEPATH += /usr/local/include/osgUtil
INCLUDEPATH += /usr/local/include/osgText
INCLUDEPATH += /usr/local/include/osgSim
INCLUDEPATH += /usr/local/include/osgParticle
INCLUDEPATH += /usr/local/include/osgFX
INCLUDEPATH += /usr/local/include/osgQt

LIBS += -L/usr/local/lib \
    -losg \
    -losgViewer \
    -losgDB \
    -losgGA \
    -losgUtil \
    -losgText \
    -losgSim \
    -losgParticle \
    -losgFX \

# Include Qt modules
QT += widgets

# Include path to your Qt installation
INCLUDEPATH += /usr/include/qt
INCLUDEPATH += /usr/include/qt/QtWidgets
INCLUDEPATH += /usr/include/qt/QtGui
INCLUDEPATH += /usr/include/qt/QtCore

# Additional compiler options
QMAKE_CXXFLAGS += -std=c++11

# Add any necessary DEFINES
DEFINES += OSG_LIBRARY_STATIC
