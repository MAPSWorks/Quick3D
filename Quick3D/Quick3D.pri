
HEADERS += \
    Source/Animation/*.h \
    Source/Base/*.h \
    Source/Components/*.h \
    Source/Input/*.h \
    Source/Math/*.h \
    Source/Mesh/*.h \
    Source/Render/*.h \
    Source/Terrain/*.h \
    Source/Utils/*.h

SOURCES += \
    Source/Animation/*.cpp \
    Source/Base/*.cpp \
    Source/Components/*.cpp \
    Source/Input/*.cpp \
    Source/Math/*.cpp \
    Source/Mesh/*.cpp \
    Source/Render/*.cpp \
    Source/Terrain/*.cpp \
    Source/Utils/*.cpp

RESOURCES += \
    Quick3DShaders.qrc

win32 {
    HEADERS += \
        Source/Zip/*.h \
        ../COTS/zlib/crc32.h \
        ../COTS/zlib/deflate.h \
        ../COTS/zlib/gzguts.h \
        ../COTS/zlib/inffast.h \
        ../COTS/zlib/inffixed.h \
        ../COTS/zlib/inflate.h \
        ../COTS/zlib/inftrees.h \
        ../COTS/zlib/trees.h \
        ../COTS/zlib/zconf.h \
        ../COTS/zlib/zlib.h \
        ../COTS/zlib/zutil.h \
        ../COTS/unzip11/crypt.h \
        ../COTS/unzip11/ioapi.h \
        ../COTS/unzip11/iowin32.h \
        ../COTS/unzip11/mztools.h \
        ../COTS/unzip11/unzip.h \
        ../COTS/unzip11/zip.h \
        ../COTS/SFML-1.6/src/SFML/Window/GlContext.hpp \
        ../COTS/SFML-1.6/src/SFML/Window/InputImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/Window/JoystickImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/Window/JoystickManager.hpp \
        ../COTS/SFML-1.6/src/SFML/Window/VideoModeImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/Window/WindowImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/Window/Win32/InputImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/Window/Win32/JoystickImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/Window/Win32/WglContext.hpp \
        ../COTS/SFML-1.6/src/SFML/Window/Win32/WindowImplWin32.hpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/ClockImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/MutexImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/SleepImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadImpl.hpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadLocalImpl.hpp

    SOURCES += \
        Source/Zip/*.cpp \
        ../COTS/zlib/adler32.c \
        ../COTS/zlib/compress.c \
        ../COTS/zlib/crc32.c \
        ../COTS/zlib/deflate.c \
        ../COTS/zlib/gzclose.c \
        ../COTS/zlib/gzlib.c \
        ../COTS/zlib/gzread.c \
        ../COTS/zlib/gzwrite.c \
        ../COTS/zlib/infback.c \
        ../COTS/zlib/inffast.c \
        ../COTS/zlib/inflate.c \
        ../COTS/zlib/inftrees.c \
        ../COTS/zlib/trees.c \
        ../COTS/zlib/uncompr.c \
        ../COTS/zlib/zutil.c \
        ../COTS/unzip11/ioapi.c \
        ../COTS/unzip11/iowin32.c \
        ../COTS/unzip11/mztools.c \
        ../COTS/unzip11/unzip.c \
        ../COTS/unzip11/zip.c \
        ../COTS/SFML-1.6/src/SFML/System/Clock.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Err.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Lock.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Mutex.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Sleep.cpp \
        ../COTS/SFML-1.6/src/SFML/System/String.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Thread.cpp \
        ../COTS/SFML-1.6/src/SFML/System/ThreadLocal.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Time.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Context.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/GlContext.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/GlResource.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Joystick.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/JoystickManager.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Keyboard.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Mouse.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/VideoMode.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Window.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/WindowImpl.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Win32/InputImpl.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Win32/JoystickImpl.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Win32/VideoModeImpl.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Win32/WglContext.cpp \
        ../COTS/SFML-1.6/src/SFML/Window/Win32/WindowImplWin32.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/ClockImpl.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/MutexImpl.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/SleepImpl.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadImpl.cpp \
        ../COTS/SFML-1.6/src/SFML/System/Win32/ThreadLocalImpl.cpp
}
