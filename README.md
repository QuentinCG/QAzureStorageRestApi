# Qt5+ Azure Storage REST API class
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/59f51d86f3ac401d8b11bb59c3cba523)](https://www.codacy.com/manual/QuentinCG/QAzureStorageRestApi?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=QuentinCG/QAzureStorageRestApi&amp;utm_campaign=Badge_Grade) [![License: MIT](https://img.shields.io/badge/License-MIT-brightgreen.svg)](https://github.com/QuentinCG/QAzureStorageRestApi/blob/master/LICENSE)
 
## What is it

This library (with a basic example) is designed to be integrated in projects using Azure storage which can't use Azure communication libraries provided by Microsoft.

This Qt class is able to <b>download/upload/list files (any kind of blob)</b> from/to a container in Azure storage using an account name and an account key.

This class was fully tested with <a href="https://download.qt.io/archive/qt/5.2/5.2.1/">Qt 5.2.1</a> (only required libraries: QtNetwork and QtCore)

<b>Important note: This project only support account credentials and therefore does not support SAS credentials.</b>

<img src="azure.png" width="300">

## How to install

1) Download <a target="_blank" href="https://github.com/QuentinCG/QAzureStorageRestApi/releases/download/1.0.0/QAzureStorageRestApi_v1_0_0.zip">latest release</a>

2) Open the <a href="https://github.com/QuentinCG/QAzureStorageRestApi/blob/master/QAzureStorageRestApi.pro">QAzureStorageRestApi.pro</a> file from <a href="https://download.qt.io/archive/qt/">QtCreator</a> (or add the <a href="https://github.com/QuentinCG/QAzureStorageRestApi/blob/master/QAzureStorageRestApi.h">QAzureStorageRestApi.h</a> and <a href="https://github.com/QuentinCG/QAzureStorageRestApi/blob/master/QAzureStorageRestApi.cpp">QAzureStorageRestApi.cpp</a> files into your project and add `QT += core` and `QT += network` in your pro file)

3) Edit the configuration in the <a href="https://github.com/QuentinCG/QAzureStorageRestApi/blob/master/main.cpp">main.cpp</a> (or use the class in your project as done in the example)

4) Build and launch the project to see the result in the command line

## License

This project is under MIT license. This means you can use it as you want (just don't delete the library header).

## Contribute

If you want to add more examples or improve the library, just create a pull request with proper commit message and right wrapping.
