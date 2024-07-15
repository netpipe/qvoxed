QT += core gui widgets

CONFIG += c++17

TEMPLATE = app
TARGET = VoxelEditor

SOURCES += main.cpp

# Include paths
INCLUDEPATH += /usr/include/OGRE
INCLUDEPATH += /usr/include/SDL2

# Library paths
LIBS += -L/usr/lib -lOgreMain -lSDL2

# Additional libraries
LIBS += -lSDL2 -lSDL2main -lSDL2_image
LIBS += -lOgreMain

# Qt modules
QT += widgets

# Add debug and release configurations
CONFIG(debug, debug|release) {
    TARGET = VoxelEditor_debug
    LIBS += -lOgreMain_d
} else {
    TARGET = VoxelEditor
}

# Ogre Plugins (assuming they are installed in standard directories)
RESOURCES += OgrePlugins.cfg OgreResources.cfg

# Definitions
DEFINES += OGRE_STATIC_LIB

# Ensure Qt is aware of the main.moc include
HEADERS += main.moc

