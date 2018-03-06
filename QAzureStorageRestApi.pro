QT += core
QT += network

QT -= gui

CONFIG += c++11

TARGET = QAzureStorageRestApi
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += \
           main.cpp \
           QAzureStorageRestApi.cpp

HEADERS += \
           QAzureStorageRestApi.h
