/*
 * \brief Example on how to Send/Receive/List files (blob in container) from Azure storage
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 17 August 2022
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2022 Quentin Comte-Gaz
 * \version 3.0
 */

#include <QCoreApplication>
#include <QDebug>

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

  // ---- Instantiate the Azure storage ----
  QAzureStorageRestApi* azure = new QAzureStorageRestApi(accountName, accountKey, &a);

  // ---- List files available in $container ----
  if (true)
  {
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
          if (code == QNetworkReply::NetworkError::NoError)
          {
            qDebug() << "Received list of files in container " + container;
            qDebug() << "List of files in the container:";
            QList< QMap<QString,QString> > files = QAzureStorageRestApi::parseFileList(listFilesReply->readAll().data());
            // Use this to list also containers: QList< QMap<QString,QString> > containers = QAzureStorageRestApi::parseContainerList(listFilesReply->readAll().data());
            for (QMap<QString,QString>& file : files)
            {
              qDebug() << "----File----";
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
  }

  // ---- Upload $localFileToUpload into $container/$azureFilenameForUpload ----
  if (true)
  {
    QNetworkReply* uploadFileReply = azure->uploadFile(localFileToUpload, container, azureFilenameForUpload);
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
          if (uploadFileReply->error() == QNetworkReply::NetworkError::NoError)
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
  }

  // ---- Download $container/$azureFilenameToDownload ----
  if (true)
  {
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
          if (downloadFileReply->error() == QNetworkReply::NetworkError::NoError)
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
  }

  return a.exec();
}
