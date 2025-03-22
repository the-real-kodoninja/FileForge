QT += core gui widgets
TARGET = FileForge
TEMPLATE = app

SOURCES += src/main.cpp \
           src/FileForgeWindow.cpp \
           src/FileConverter.cpp \
           src/ConversionThread.cpp

HEADERS += include/FileForgeWindow.hpp \
           include/FileConverter.hpp \
           include/ConversionThread.hpp

INCLUDEPATH += include
LIBS += -larchive  # For libarchive
# Add -lavcodec -lavformat -lavutil for ffmpeg if installed