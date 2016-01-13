TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
QMAKE_CXXFLAGS += -std=c++11 -Wall -Wextra -Weffc++

SOURCES += \
    main.cpp \
    BranchEvent.cpp \
    CommandLineProcessor.cpp \
    Log.cpp \
    MbRandom.cpp \
    Node.cpp \
    Settings.cpp \
    SettingsParameter.cpp \
    SimTree.cpp \
    SimTreeEngine.cpp

HEADERS += \
    BranchEvent.h \
    CommandLineProcessor.h \
    Log.h \
    MatchPathSeparator.h \
    MbRandom.h \
    Node.h \
    Settings.h \
    SettingsParameter.h \
    SimTree.h \
    SimTreeEngine.h

