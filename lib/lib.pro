QT += core
QT += network

QT -= gui

DEFINES += QAZURESTORAGERESTAPI_LIBRARY

CONFIG += c++11
QMAKE_CXXFLAGS += -std=c++11

CONFIG(release, debug|release): TARGET = QAzureStorageRestApi
else:CONFIG(debug, debug|release): TARGET = QAzureStorageRestApid

TEMPLATE = lib

SOURCES += \
           src/QAzureStorageRestApi.cpp

HEADERS += \
           include/QAzureStorageRestApi.h \
           include/QAzureStorageRestApi_global.h

INCLUDEPATH += \
           include/

# -- Test coverage (if requested only) --
TEST_COVERAGE {
  QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
  LIBS += -lgcov
}
