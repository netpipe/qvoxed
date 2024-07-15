QT += core gui widgets

CONFIG += c++11

TARGET = VoxelEditor
TEMPLATE = app

SOURCES += main.cpp \
                     VoxelNode.cpp

HEADERS +=            VoxelNode.h

# Include paths for Irrlicht
INCLUDEPATH += /path/to/irrlicht/include

# Library paths for Irrlicht
LIBS += -L/path/to/irrlicht/lib -lIrrlicht

# Add any other necessary Qt or system libraries here
