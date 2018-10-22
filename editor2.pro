######################################################################
# Automatically generated by qmake (3.1) Mon Oct 22 14:36:38 2018
######################################################################

SDLINCLUDE
TEMPLATE = app
TARGET = editor2

DEFINES += _OFED

INCLUDEPATH += .
INCLUDEPATH += Source
INCLUDEPATH += Dependencies/OpenFodder/Source
INCLUDEPATH += /usr/include/SDL2

LIBS += -lSDL2 -lSDL2_mixer
QT += widgets
QMAKE_CXXFLAGS += -g

# The following define makes your compiler warn you if you use any
# feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Input
HEADERS += Source/OFED.hpp \
           Source/stdafx_ofed.hpp \
           Source/WindowQT.hpp \
           Source/Dialogs/CampaignDialog.hpp \
           Source/Dialogs/NewMapDialog.hpp \
           Source/Dialogs/ToolboxSprites.hpp \
           Source/Dialogs/ToolboxTiles.hpp \
           Source/Models/CampaignModel.hpp \
           Source/Models/MissionModel.hpp \
           Dependencies/OpenFodder/Source/Campaign.hpp \
           Dependencies/OpenFodder/Source/CopyProtection.hpp \
           Dependencies/OpenFodder/Source/Dimension.hpp \
           Dependencies/OpenFodder/Source/Event.hpp \
           Dependencies/OpenFodder/Source/Fodder.hpp \
           Dependencies/OpenFodder/Source/FontData.hpp \
           Dependencies/OpenFodder/Source/GameData.hpp \
           Dependencies/OpenFodder/Source/Graphics.hpp \
           Dependencies/OpenFodder/Source/GUI_Element.hpp \
           Dependencies/OpenFodder/Source/IntroData.hpp \
           Dependencies/OpenFodder/Source/Map.hpp \
           Dependencies/OpenFodder/Source/md5.hpp \
           Dependencies/OpenFodder/Source/Position.hpp \
           Dependencies/OpenFodder/Source/Recruits.hpp \
           Dependencies/OpenFodder/Source/Resources.hpp \
           Dependencies/OpenFodder/Source/Singleton.hpp \
           Dependencies/OpenFodder/Source/Sound.hpp \
           Dependencies/OpenFodder/Source/Sprites.hpp \
           Dependencies/OpenFodder/Source/SpriteSheet.hpp \
           Dependencies/OpenFodder/Source/stdafx.hpp \
           Dependencies/OpenFodder/Source/Surface.hpp \
           Dependencies/OpenFodder/Source/Tiles.hpp \
           Dependencies/OpenFodder/Source/Types.hpp \
           Dependencies/OpenFodder/Source/Versions.hpp \
           Dependencies/OpenFodder/Source/Window.hpp \
           Dependencies/OpenFodder/Projects/VS2017/resource.h \
           Dependencies/OpenFodder/Source/Amiga/audiostream.hpp \
           Dependencies/OpenFodder/Source/Amiga/dernc.hpp \
           Dependencies/OpenFodder/Source/Amiga/Graphics_Amiga.hpp \
           Dependencies/OpenFodder/Source/Amiga/paula.hpp \
           Dependencies/OpenFodder/Source/Amiga/Resource_Amiga_File.hpp \
           Dependencies/OpenFodder/Source/Amiga/rjp1.hpp \
           Dependencies/OpenFodder/Source/Amiga/Sound_Amiga.hpp \
           Dependencies/OpenFodder/Source/Amiga/SpriteData_Amiga.hpp \
           Dependencies/OpenFodder/Source/PC/Graphics_PC.hpp \
           Dependencies/OpenFodder/Source/PC/Resource_PC_CD.hpp \
           Dependencies/OpenFodder/Source/PC/Sound_PC.hpp \
           Dependencies/OpenFodder/Source/PC/Sound_PC2.hpp \
           Dependencies/OpenFodder/Source/PC/SpriteData_PC.hpp \
           Dependencies/OpenFodder/Source/PC/VocTable.hpp \
           Dependencies/OpenFodder/Source/Structures/Barracks.hpp \
           Dependencies/OpenFodder/Source/Utils/diamondsquare.hpp \
           Dependencies/OpenFodder/Source/Utils/json.hpp
FORMS += Project/VS2017/ofed.ui \
         Source/Dialogs/CampaignDialog.ui \
         Source/Dialogs/NewMapDialog.ui \
         Source/Dialogs/ToolboxSprites.ui \
         Source/Dialogs/ToolboxTiles.ui
SOURCES += Source/main.cpp \
           Source/OFED.cpp \
           Source/WindowQT.cpp \
           Source/Dialogs/CampaignDialog.cpp \
           Source/Dialogs/NewMapDialog.cpp \
           Source/Dialogs/ToolboxSprites.cpp \
           Source/Dialogs/ToolboxTiles.cpp \
           Source/Models/CampaignModel.cpp \
           Source/Models/MissionModel.cpp \
           Dependencies/OpenFodder/Source/Campaign.cpp \
           Dependencies/OpenFodder/Source/CopyProtection.cpp \
           Dependencies/OpenFodder/Source/Event.cpp \
           Dependencies/OpenFodder/Source/Fodder.cpp \
           Dependencies/OpenFodder/Source/FontData.cpp \
           Dependencies/OpenFodder/Source/GameData.cpp \
           Dependencies/OpenFodder/Source/Graphics.cpp \
           Dependencies/OpenFodder/Source/GUI_Element.cpp \
           Dependencies/OpenFodder/Source/md5.cpp \
           Dependencies/OpenFodder/Source/Recruits.cpp \
           Dependencies/OpenFodder/Source/Resources.cpp \
           Dependencies/OpenFodder/Source/Sound.cpp \
           Dependencies/OpenFodder/Source/Sprites.cpp \
           Dependencies/OpenFodder/Source/SpriteSheet.cpp \
           Dependencies/OpenFodder/Source/stdafx.cpp \
           Dependencies/OpenFodder/Source/Surface.cpp \
           Dependencies/OpenFodder/Source/Tiles.cpp \
           Dependencies/OpenFodder/Source/Versions.cpp \
           Dependencies/OpenFodder/Source/Window.cpp \
           Dependencies/OpenFodder/Source/Amiga/dernc.cpp \
           Dependencies/OpenFodder/Source/Amiga/Graphics_Amiga.cpp \
           Dependencies/OpenFodder/Source/Amiga/IntroData_Amiga.cpp \
           Dependencies/OpenFodder/Source/Amiga/paula.cpp \
           Dependencies/OpenFodder/Source/Amiga/Resource_Amiga_File.cpp \
           Dependencies/OpenFodder/Source/Amiga/rjp1.cpp \
           Dependencies/OpenFodder/Source/Amiga/Sound_Amiga.cpp \
           Dependencies/OpenFodder/Source/PC/Graphics_PC.cpp \
           Dependencies/OpenFodder/Source/PC/IntroData_PC.cpp \
           Dependencies/OpenFodder/Source/PC/Resource_PC_CD.cpp \
           Dependencies/OpenFodder/Source/PC/Sound_PC.cpp \
           Dependencies/OpenFodder/Source/PC/Sound_PC2.cpp \
           Dependencies/OpenFodder/Source/Structures/Barracks.cpp
RESOURCES += Project/VS2017/ofed.qrc
