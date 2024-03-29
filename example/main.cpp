/*
 * \brief Example on how to Send/Receive/List files (blob in container) from Azure storage
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 08 November 2023
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2023 Quentin Comte-Gaz
 * \version 3.2
 */

#include <QCoreApplication>
#include <QDebug>
#include <QtNetwork>

#include "QAzureStorageRestApi.h"

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);

  // ---- Configuration ----
  QString accountName = "AZURE_STORAGE_ACCOUNT_NAME_HERE";
  QString accountKey = "AZURE_STORAGE_ACCOUNT_KEY_HERE";

  QString container = "CONTAINER_NAME_HERE";
  QString localFileToUpload = "/PATH/TO/LOCAL/FILE.txt";
  QString azureFilenameForUpload = "test.txt";
  QString azureFilenameToDownload = "test.txt";
  QString azureOptionalSasCredentialToGenerateUserUrl = "sv=2022-11-02&sr=b&sig=.......";
  QString azureFilenameToDelete = "test.txt";

  // ---- Instantiate the Azure storage ----
  // Initialize connection using Storage account key
  QAzureStorageRestApi* azure = new QAzureStorageRestApi(accountName, accountKey, &a, true);
  // OR
  // Initialize connection using SAS key (SAS key must grant access to Read/Add/Write/Delete/List depending on which requests you need to execute with this library)
  // QAzureStorageRestApi* azure = new QAzureStorageRestApi(accountName, "sv=2022-11-02&sr=b&sig=.......", &a, false);

  // ---- List containers available ----
  if (true)
  {
    // Synchronous list containers
    QList< QMap<QString,QString> > foundListOfContainers;
    QNetworkReply::NetworkError codeSynchronous = azure->listContainersSynchronous(foundListOfContainers);
    if (QAzureStorageRestApi::isErrorCodeSuccess(codeSynchronous))
    {
        qDebug() << "Received list of containers.";
        qDebug() << "List of containers:";
        for (QMap<QString,QString>& foundContainer : foundListOfContainers)
        {
            QMap<QString, QString>::iterator it;
            for (it = foundContainer.begin(); it != foundContainer.end(); ++it)
            {
                qDebug() << QString("%1 : %2").arg(it.key()).arg(it.value());
            }
        }
    }
    else
    {
        qWarning() << "Error listing containers (error code " + QString::number(codeSynchronous) + ")";
    }

    // OR ASYNCHRONOUS:
    /*
    QNetworkReply* listContainersReply = azure->listContainers();
    if (listContainersReply != nullptr)
    {
      QObject::connect(listContainersReply, &QNetworkReply::finished,
        [listContainersReply]()
        {
          if (listContainersReply == nullptr) {
              qWarning() << "listContainersReply is null but signal sent from it !";
              return;
          }

          QNetworkReply::NetworkError code = listContainersReply->error();
          if (QAzureStorageRestApi::isErrorCodeSuccess(code))
          {
            qDebug() << "Received list of containers";
            qDebug() << "List of containers:";
            QList< QMap<QString,QString> > containers = QAzureStorageRestApi::parseContainerList(listContainersReply->readAll().data());
            for (QMap<QString,QString>& container : containers)
            {
              QMap<QString, QString>::iterator it;
              for (it = container.begin(); it != container.end(); ++it)
              {
                qDebug() << QString("%1 : %2").arg(it.key()).arg(it.value());
              }
            }
          }
          else
          {
            qWarning() << "Error while trying to list containers (error code " + QString::number(code) + ")";
          }

          listContainersReply->deleteLater();
      });
    }
    */
  }

  // ---- List files available in $container ----
  if (true)
  {
    // Synchronous list files
    QList< QMap<QString,QString> > foundListOfFiles;
    QNetworkReply::NetworkError codeSynchronous = azure->listFilesSynchronous(container, foundListOfFiles);
    if (QAzureStorageRestApi::isErrorCodeSuccess(codeSynchronous))
    {
        qDebug() << "Received list of files in container " + container;
        qDebug() << "List of files in the container:";
        for (QMap<QString,QString>& file : foundListOfFiles)
        {
            QMap<QString, QString>::iterator it;
            for (it = file.begin(); it != file.end(); ++it)
            {
                qDebug() << QString("%1 : %2").arg(it.key()).arg(it.value());
            }
        }
    }
    else
    {
        qWarning() << "Error listing files in container " + container + " (error code " + QString::number(codeSynchronous) + ")";
    }

    // OR ASYNCHRONOUS:
    /*
    QNetworkReply* listFilesReply = azure->listFiles(container);
    if (listFilesReply != nullptr)
    {
      QObject::connect(listFilesReply, &QNetworkReply::finished,
        [listFilesReply, container]()
        {
          if (listFilesReply == nullptr) {
              qWarning() << "listFilesReply is null but signal sent from it !";
              return;
          }

          QNetworkReply::NetworkError code = listFilesReply->error();
          if (QAzureStorageRestApi::isErrorCodeSuccess(code))
          {
            qDebug() << "Received list of files in container " + container;
            qDebug() << "List of files in the container:";
            QList< QMap<QString,QString> > files = QAzureStorageRestApi::parseFileList(listFilesReply->readAll().data());
            for (QMap<QString,QString>& file : files)
            {
              QMap<QString, QString>::iterator it;
              for (it = file.begin(); it != file.end(); ++it)
              {
                qDebug() << QString("%1 : %2").arg(it.key()).arg(it.value());
              }
            }
          }
          else
          {
            qWarning() << "Error while trying to list files in container " + container + " (error code " + QString::number(code) + ")";
          }

          listFilesReply->deleteLater();
      });
    }
    */
  }

  // ---- Upload $localFileToUpload into $container/$azureFilenameForUpload ----
  if (true)
  {
    // Synchronous file upload
    // (Use azure->uploadFileQByteArraySynchronous if you have the data in memory)
    QNetworkReply::NetworkError codeSynchronous = azure->uploadFileSynchronous(localFileToUpload, container, azureFilenameForUpload);
    if (QAzureStorageRestApi::isErrorCodeSuccess(codeSynchronous))
    {
      qDebug() << "File " + localFileToUpload + " uploaded with success into " + container + "/" + azureFilenameForUpload;
    }
    else
    {
      qWarning() << "Error upload file " + localFileToUpload + " in " + container + "/" + azureFilenameForUpload + " (error code " + QString::number(codeSynchronous) + ")";
    }

    // OR ASYNCHRONOUS:
    /*
    QNetworkReply* uploadFileReply = azure->uploadFile(localFileToUpload, container, azureFilenameForUpload);
    // (Use azure->uploadFileQByteArray if you have the data in memory)
    if (uploadFileReply != nullptr)
    {
      QObject::connect(uploadFileReply, &QNetworkReply::finished,
        [uploadFileReply, container, localFileToUpload, azureFilenameForUpload]()
        {
          if (uploadFileReply == nullptr) {
              qWarning() << "uploadFileReply is null but signal sent from it !";
              return;
          }

          QNetworkReply::NetworkError code = uploadFileReply->error();
          if (QAzureStorageRestApi::isErrorCodeSuccess(code))
          {
            qDebug() << "File " + localFileToUpload + " uploaded with success into " + container + "/" + azureFilenameForUpload;
          }
          else
          {
            qWarning() << "Error while trying to upload file " + localFileToUpload + " in " + container + "/" + azureFilenameForUpload + " (error code " + QString::number(code) + ")";
          }

          uploadFileReply->deleteLater();
        }
      );
    }
    */
  }

  // ---- Download $container/$azureFilenameToDownload ----
  if (true)
  {
    // Synchronous file download
    QByteArray downloadedFile;
    QNetworkReply::NetworkError codeSynchronous = azure->downloadFileSynchronous(container, azureFilenameToDownload, downloadedFile);
    if (QAzureStorageRestApi::isErrorCodeSuccess(codeSynchronous))
    {
      qDebug() << "File " + container + "/" + azureFilenameToDownload + " downloaded with success";
      qDebug() << "File content : " << QString(downloadedFile);
    }
    else
    {
      qWarning() << "Error download file from " + container + "/" + azureFilenameToDownload + " (error code " + QString::number(codeSynchronous) + ")";
    }

    // OR ASYNCHRONOUS:
    /*
    QNetworkReply* downloadFileReply = azure->downloadFile(container, azureFilenameToDownload);
    if (downloadFileReply != nullptr)
    {
      QObject::connect(downloadFileReply, &QNetworkReply::finished,
        [downloadFileReply, container, azureFilenameToDownload]() {
          if (downloadFileReply == nullptr) {
              qWarning() << "downloadFileReply is null but signal sent from it !";
              return;
          }

          QNetworkReply::NetworkError code = downloadFileReply->error();
          if (QAzureStorageRestApi::isErrorCodeSuccess(code))
          {
            qDebug() << "File " + container + "/" + azureFilenameToDownload + " downloaded with success";
            qDebug() << "File content : " << QString(downloadFileReply->readAll().data());
          }
          else
          {
            qWarning() << "Error while trying to download file from " + container + "/" + azureFilenameToDownload + " (error code " + QString::number(code) + ")";
            qWarning() << "Error content : " << QString(downloadFileReply->readAll().data());
          }

          downloadFileReply->deleteLater();
      });
    }
    */
  }

  // ---- Delete $container/$azureFilenameToDelete ----
  if (true)
  {
    // Synchronous file delete
    QNetworkReply::NetworkError codeSynchronous = azure->deleteFileSynchronous(container, azureFilenameToDelete);
    if (QAzureStorageRestApi::isErrorCodeSuccess(codeSynchronous))
    {
      qDebug() << "File " + localFileToUpload + " deleted with success from " + container + "/" + azureFilenameToDelete;
    }
    else
    {
      qWarning() << "Error deleting file from " + container + "/" + azureFilenameToDelete + " (error code " + QString::number(codeSynchronous) + ")";
    }

    // OR ASYNCHRONOUS:
    /*
    QNetworkReply* deleteFileReply = azure->deleteFile(container, azureFilenameToDelete);
    if (deleteFileReply != nullptr)
    {
      QObject::connect(deleteFileReply, &QNetworkReply::finished,
        [deleteFileReply, container, azureFilenameToDelete]() {
          if (deleteFileReply == nullptr) {
              qWarning() << "deleteFileReply is null but signal sent from it !";
              return;
          }

          QNetworkReply::NetworkError code = deleteFileReply->error();
          if (QAzureStorageRestApi::isErrorCodeSuccess(code))
          {
            qDebug() << "File " + container + "/" + azureFilenameToDelete + " deleted with success";
          }
          else
          {
            qWarning() << "Error while trying to delete file from " + container + "/" + azureFilenameToDelete + " (error code " + QString::number(code) + ")";
          }

          deleteFileReply->deleteLater();
      });
    }
    */
  }

  // --- Create/Delete container ---
  // Same logic:
  if (true)
  {
    //azure->createContainer(container);
    azure->createContainerSynchronous(container);
    //azure->deleteContainer(container);
    azure->deleteContainerSynchronous(container);
  }

  // --- GENERATE URL TO PROVIDE TO USER (SAS TOKEN TO PROVIDE, CHECK ANNEX OF README FOR PROCEDURE) ---
  qDebug() << "URL to provide to user to download file if SAS token provided with read access to container: '" + azure->generateUrl(container, azureFilenameToDownload, azureOptionalSasCredentialToGenerateUserUrl) + "'";

  return a.exec();
}
