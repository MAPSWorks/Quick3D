#-------------------------------------------------
#
# Project created by QtCreator 2016-01-22T20:41:20
#
#-------------------------------------------------

QT += core gui network opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Quick3DTest
TEMPLATE = app
INCLUDEPATH += $$PWD/../Quick3D/Source
DEPENDPATH += $$PWD/../Quick3D

# C++ Flags
QMAKE_CXXFLAGS += -Wno-invalid-offsetof
QMAKE_CXXFLAGS += -Wno-unused-parameter
QMAKE_CXXFLAGS += -Wno-reorder

# Libraries
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../Quick3D/release/ -lQuick3D
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../Quick3D/debug/ -lQuick3D
else:unix: LIBS += -L$$OUT_PWD/../Quick3D/ -lQuick3D

# Code
SOURCES += \
    Quick3DTest.cpp \
    main.cpp

HEADERS  += \
    Quick3DTest.h

DISTFILES +=

RESOURCES += \
    Quick3DTest.qrc

FORMS += \
    ExportDialog.ui \
    Quick3DTest.ui
