QT += svg
QT += webkit

HEADERS += \
    Common/map.h \
    Common/mapfloor.h \
    Common/maparea.h \
    Common/mapbase.h \
    Ariadne/mainwindow.h \
    Ariadne/dialogmapsize.h \
    Common/geometry.h \
    Common/mapdoor.h \
    Common/mapselection.h \
    Common/grapharc.h \
    Common/graph.h \
    Common/graphnode.h \
    Ariadne/dialogfloorchoice.h \
    Common/mapvertical.h \
    Common/global.h \
    Ariadne/dialogmapproperties.h

SOURCES += \
    Common/map.cpp \
    Common/mapfloor.cpp \
    Common/maparea.cpp \
    Common/mapbase.cpp \
    Ariadne/main.cpp \
    Ariadne/mainwindow.cpp \
    Ariadne/dialogmapsize.cpp \
    Common/geometry.cpp \
    Common/mapdoor.cpp \
    Common/mapselection.cpp \
    Common/grapharc.cpp \
    Common/graph.cpp \
    Common/graphnode.cpp \
    Ariadne/dialogfloorchoice.cpp \
    Common/mapvertical.cpp \
    Common/global.cpp \
    Ariadne/dialogmapproperties.cpp

RESOURCES += \
    MapIcons.qrc

RC_FILE = AppIcon.rc
