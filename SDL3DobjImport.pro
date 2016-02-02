TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp

LIBS += -lGLU
LIBS += -lGL
LIBS += -lSDL2
LIBS += -lSDL2_image
