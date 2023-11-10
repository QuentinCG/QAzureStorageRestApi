/*
 * \brief Send/Receive/List files (blobs in container) from Azure storage
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 08 November 2023
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2023 Quentin Comte-Gaz
 * \version 3.2
 */

#ifndef QAZURESTORAGERESTAPI_H
#define QAZURESTORAGERESTAPI_H

#include <QObject>
#include <QtNetwork>

#include "QAzureStorageRestApi_global.h"

class QAZURESTORAGERESTAPISHARED_EXPORT QAzureStorageRestApi : public QObject
{
  Q_OBJECT

public:
  /*!
   * \brief QAzureStorageRestApi Send/Receive/List files from Azure storage
   * \param accountName Account name
   * \param accountKeyOrSasCredentials Account key OR SAS credentials
   * \param parent (optional) QObject parent
   * \param isAccountKey (optional) Is \p accountKeyOrSasCredentials an account key or a sas credential ?
   */
  QAzureStorageRestApi(const QString& accountName, const QString& accountKeyOrSasCredentials, QObject* parent = nullptr, const bool isAccountKey = true);

  /*!
   * \brief updateCredentials Update the account name and account key if changed
   * \param accountName Account name
   * \param accountKeyOrSasCredentials Account key OR SAS credentials
   * \param isAccountKey (optional) Is \p accountKeyOrSasCredentials an account key or a sas credential ?
   */
  void updateCredentials(const QString&accountName, const QString& accountKeyOrSasCredentials, const bool isAccountKey = true);

  /*!
   * \brief listContainers List containers in an azure storage account
   *
   * \param marker (optional) Marker to list specific informations only
   *
   * \return Reply from Azure (XML encoded file list if QNetworkReply::isFinished() is
   *         triggered with QNetworkReply::error() ==  QNetworkReply::NetworkError::NoError)
   *         Return value can be nullptr if invalid request
   *         It is possible to decode the reply from Azure with \s QAzureStorageRestApi::parseContainerList
   */
  QNetworkReply* listContainers(const QString& marker = QString());

  /*!
   * \brief listFiles List files in an azure storage container
   *
   * \param container Container to check
   * \param marker (optional) Marker to check specific informations only
   *
   * \return Reply from Azure (XML encoded file list if QNetworkReply::isFinished() is
   *         triggered with QNetworkReply::error() ==  QNetworkReply::NetworkError::NoError)
   *         Return value can be nullptr if invalid request
   *         It is possible to decode the reply from Azure with \s QAzureStorageRestApi::parseFileList
   */
  QNetworkReply* listFiles(const QString& container, const QString& marker = QString());

  /*!
   * \brief uploadFile Upload a file from local directory into azure storage (remote path: \s container/\s blobName)
   *
   * \param filePath Absolute path of the local file to upload
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to create
   * \param blobType (optional) Type of blob to create
   *
   * \return Reply from Azure (Uploaded with success if QNetworkReply::isFinished() is
   *         triggered with QNetworkReply::error() ==  QNetworkReply::NetworkError::NoError)
   *         Return value can be nullptr if invalid request or filePath is invalid
   */
  QNetworkReply* uploadFile(const QString& filePath, const QString& container, const QString& blobName, const QString& blobType = "BlockBlob");

  /*!
   * \brief uploadFileSynchronous Synchronous method to upload a file from local directory into azure storage (remote path: \s container/\s blobName)
   *
   * \param filePath Absolute path of the local file to upload
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to create
   * \param blobType (optional) Type of blob to create
   *
   * \return True if uploaded successfully on time
   */
  bool uploadFileSynchronous(const QString& filePath, const QString& container, const QString& blobName, const QString& blobType = "BlockBlob", const int& timeoutInMs = 5000);

  /*!
   * \brief uploadFileQByteArray Upload a file from QByteArray into azure storage (remote path: \s container/\s blobName)
   *
   * \param fileContent Content of the file to upload
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to create
   * \param blobType (optional) Type of blob to create
   *
   * \return Reply from Azure (Uploaded with success if QNetworkReply::isFinished() is
   *         triggered with QNetworkReply::error() ==  QNetworkReply::NetworkError::NoError)
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* uploadFileQByteArray(const QByteArray& fileContent, const QString& container, const QString& blobName, const QString& blobType = "BlockBlob");

  /*!
   * \brief uploadFileQByteArraySynchronous Synchronously method to upload a file from QByteArray into azure storage (remote path: \s container/\s blobName)
   *
   * \param fileContent Content of the file to upload
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to create
   * \param blobType (optional) Type of blob to create
   *
   * \return True if uploaded successfully on time
   */
  bool uploadFileQByteArraySynchronous(const QByteArray& fileContent, const QString& container, const QString& blobName, const QString& blobType = "BlockBlob", const int& timeoutInMs = 5000);

  /*!
   * \brief deleteFile Delete a file from azure storage (remote path: \s container/\s blobName)
   *
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to delete
   *
   * \return Reply from Azure (Uploaded with success if QNetworkReply::isFinished() is
   *         triggered with QNetworkReply::error() ==  QNetworkReply::NetworkError::NoError)
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* deleteFile(const QString& container, const QString& blobName);

  /*!
   * \brief downloadFile Download a file from azure storage (remote path: \s container/\s blobName)
   *
   * \param container Container to take the file from
   * \param blobName File (any kind of blob) to download
   *
   * \return Reply from Azure (File will be available when QNetworkReply::isFinished() will
   *         be triggered with QNetworkReply::error() ==  QNetworkReply::NetworkError::NoError)
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* downloadFile(const QString& container, const QString& blobName);

  QString generateUrl(const QString& container, const QString& blobName = QString(), const QString& additionnalParameters = QString(),
                      const QString& marker = QString());

  /*!
   * \brief parseContainerList Helper to convert XML file list received from Azure into Qt compatible format
   *
   * \param xmlFileList XML file list received using \s QAzureStorageRestApi::listContainers
   * \param NextMarker (optional) Marker to list specific informations only
   *
   * \return List of containers with all available information on containers (name, type, md5, ...)
   */
  static QList< QMap<QString,QString> > parseContainerList(const QByteArray& xmlContainerList, QString* NextMarker = nullptr);

  /*!
   * \brief parseFileList Helper to convert XML file list received from Azure into Qt compatible format
   *
   * \param xmlFileList XML file list received using \s QAzureStorageRestApi::listFiles
   * \param NextMarker (optional) Marker to list specific informations only
   *
   * \return List of files with all available information on files (name, type, md5, ...)
   */
  static QList< QMap<QString,QString> > parseFileList(const QByteArray& xmlFileList, QString* NextMarker = nullptr);

private:
  QString generateCurrentTimeUTC();
  QString generateHeader(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&,
                         const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&);
  QString generateAutorizationHeader(const QString& httpVerb, const QString& container, const QString& blobName,
                                     const QString& currentDateTime, const long& contentLength,
                                     const QStringList additionnalCanonicalHeaders = QStringList(),
                                     const QStringList additionnalCanonicalRessources = QStringList());
  void updateRequestToAddAuthentication(QNetworkRequest* request);
  static QList< QMap<QString,QString> > parseObjectList(const char* tag, const QByteArray& xml, QString* NextMarker);

private:
  QString m_version = "2021-04-10"; //!< Azure Storage API currently used by this library
  QString m_accountName;
  QString m_accountKey;
  QString m_sasKey;
  QNetworkAccessManager* m_manager;
};

#endif // QAZURESTORAGERESTAPI_H
