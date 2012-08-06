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

OTHER_FILES += \
    android/res/values-rs/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/drawable-mdpi/icon.png \
    android/res/drawable-hdpi/icon.png \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/layout/splash.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/drawable-ldpi/icon.png \
    android/res/values-fr/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-fa/strings.xml \
    android/AndroidManifest.xml \
    android/version.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/src/org/kde/necessitas/origo/QtActivity.java

