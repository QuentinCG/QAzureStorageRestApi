# Qt5+ Azure Storage REST API class

## What is it?

This library (with a basic example) is designed to be integrated in projects using Azure storage which can't use Azure communication libraries provided by Microsoft.

This Qt class is able to <b>download/upload/list files (any kind of blob)</b> from/to a container in Azure storage using an account name and an account key.

This class was fully tested with Qt 5.2.1 (only required libraries: QtNetwork and QtCore)


<b>Important note: This project only support account credentials and therefore does not support SAS credentials.</b>


<img src="azure.png" width="300">


## How to install

1) Download <a target="_blank" href="https://github.com/QuentinCG/QAzureStorageRestApi/releases/download/1.0.0/QAzureStorageRestApi_v1_0_0.zip">latest release</a>

2) Open the QAzureStorageRestApi.pro file from QtCreator (or add the QAzureStorageRestApi.h and QAzureStorageRestApi.cpp files into your project + `QT += core` and `QT += network` in your pro file)

3) Edit the configuration in the main.cpp (or use the class in your project as done in the example)

4) Build and launch the project to see the result in the command line


## License

This project is under MIT license. This means you can use it as you want (just don't delete the library header).


## Contribute

If you want to add more examples or improve the library, just create a pull request with proper commit message and right wrapping.
