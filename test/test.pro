QT += core
QT += network

QT -= gui

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

CONFIG(release, debug|release): TARGET = test
else:CONFIG(debug, debug|release): TARGET = testd

CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
           main.cpp

HEADERS += \
           externals/catch.hpp \

INCLUDEPATH += \
           $$PWD/../lib/include \
           $$PWD/externals/

DEPENDPATH += \
           $$PWD/../lib

win32: {
  CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/release/ -lQAzureStorageRestApi
  else:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/debug/ -lQAzureStorageRestApid
}
else:unix: {
  CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../lib/ -lQAzureStorageRestApi
  else:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../lib/ -lQAzureStorageRestApid
}
