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
  // ------------------------------------- CONSTRUCTOR & INIT -------------------------------------
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

  // ------------------------------------- PUBLIC HELPER -------------------------------------
  /*!
   * \brief generateUrl Generate URL for user to download a file
   *
   * \param container Container to add to URL
   * \param blobName Name of the file to add to URL
   * \param additionnalParameters Additional parameters
   * \param marker (optional) Marker to add to URL
   *
   * \return Full URL
   */
  QString generateUrl(const QString& container, const QString& blobName = QString(), const QString& additionnalParameters = QString(),
                      const QString& marker = QString(), const int& timeoutInSec = -1, const QString& sasKey = QString());

  static bool isErrorCodeSuccess(const QNetworkReply::NetworkError& errorCode);

  // ------------------------------------- PUBLIC ASYNCHRONOUS -------------------------------------

  /*!
   * \brief listContainers List containers in an azure storage account
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/list-blobs?tabs=microsoft-entra-id
   *
   * \param marker (optional) Marker to list specific informations only
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return Reply from Azure (XML encoded file list if QNetworkReply::isFinished() is
   *         triggered with isErrorCodeSuccess(QNetworkReply::error())
   *         Return value can be nullptr if invalid request
   *         It is possible to decode the reply from Azure with \s QAzureStorageRestApi::parseContainerList
   */
  QNetworkReply* listContainers(const QString& marker = QString(), const int& timeoutInSec = -1);

  /*!
   * \brief listFiles List files in an azure storage container
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/list-blobs?tabs=microsoft-entra-id
   *
   * \param container Container to check
   * \param marker (optional) Marker to check specific informations only
   * \param prefix (optional) Prefix to filter results
   * \param maxResults (optional, default: default Azure REST API number of results) Max number of elements
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return Reply from Azure (XML encoded file list if QNetworkReply::isFinished() is
   *         triggered with isErrorCodeSuccess(QNetworkReply::error())
   *         Return value can be nullptr if invalid request
   *         It is possible to decode the reply from Azure with \s QAzureStorageRestApi::parseFileList
   */
  QNetworkReply* listFiles(const QString& container, const QString& marker = QString(), const QString& prefix = QString(), const int& maxResults = -1, const int& timeoutInSec = -1);

  /*!
   * \brief uploadFile Upload a file from local directory into azure storage (remote path: \s container/\s blobName)
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/put-blob?tabs=microsoft-entra-id
   *
   * \param filePath Absolute path of the local file to upload
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to create
   * \param blobType (optional) Type of blob to create
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return Reply from Azure (Uploaded with success if QNetworkReply::isFinished() is
   *         triggered with isErrorCodeSuccess(QNetworkReply::error())
   *         Return value can be nullptr if invalid request or filePath is invalid
   */
  QNetworkReply* uploadFile(const QString& filePath, const QString& container, const QString& blobName, const QString& blobType = "BlockBlob", const int& timeoutInSec = -1);

  /*!
   * \brief uploadFileQByteArray Upload a file from QByteArray into azure storage (remote path: \s container/\s blobName)
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/put-blob?tabs=microsoft-entra-id
   *
   * \param fileContent Content of the file to upload
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to create
   * \param blobType (optional) Type of blob to create
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return Reply from Azure (Uploaded with success if QNetworkReply::isFinished() is
   *         triggered with isErrorCodeSuccess(QNetworkReply::error())
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* uploadFileQByteArray(const QByteArray& fileContent, const QString& container, const QString& blobName, const QString& blobType = "BlockBlob", const int& timeoutInSec = -1);

  /*!
   * \brief deleteFile Delete a file from azure storage (remote path: \s container/\s blobName)
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/delete-blob?tabs=microsoft-entra-id
   *
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to delete
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return Reply from Azure (Uploaded with success if QNetworkReply::isFinished() is
   *         triggered with isErrorCodeSuccess(QNetworkReply::error())
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* deleteFile(const QString& container, const QString& blobName, const int& timeoutInSec = -1);

  /*!
   * \brief downloadFile Download a file from azure storage (remote path: \s container/\s blobName)
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/get-blob?tabs=microsoft-entra-id
   *
   * \param container Container to take the file from
   * \param blobName File (any kind of blob) to download
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return Reply from Azure (File will be available when QNetworkReply::isFinished() will
   *         triggered with isErrorCodeSuccess(QNetworkReply::error())
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* downloadFile(const QString& container, const QString& blobName, const int& timeoutInSec = -1);

  /*!
   * \brief startCopyFile START a copy of a file from azure storage account into a file in the same azure storage account
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/copy-blob?tabs=microsoft-entra-id
   *
   * \param containerFrom Container of the file to copy
   * \param blobNameFrom File name (with potential path) of the file to copy
   * \param containerTo Container of the new file
   * \param blobNameTo File name (with potential path) of the new file
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return Reply from Azure (XML encoded file list if QNetworkReply::isFinished() is
   *         triggered with isErrorCodeSuccess(QNetworkReply::error())
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* startCopyFile(const QString& containerFrom, const QString& blobNameFrom, const QString& containerTo, const QString& blobNameTo, const int& timeoutInSec = -1);

  /*!
   * \brief createContainer Create a container
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/create-container?tabs=microsoft-entra-id
   *
   * \param container Container to create
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return Reply from Azure (Container created when QNetworkReply::isFinished() will
   *         triggered with isErrorCodeSuccess(QNetworkReply::error())
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* createContainer(const QString& container, const int& timeoutInSec = -1);

  /*!
   * \brief deleteContainer Delete a container
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/delete-container?tabs=microsoft-entra-id
   *
   * \param container Container to delete
   * \param leaseId (optional) Active lease to delete (check microsoft link for more details)
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return Reply from Azure (Container deleted when QNetworkReply::isFinished() will
   *         triggered with isErrorCodeSuccess(QNetworkReply::error())
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* deleteContainer(const QString& container, const QString& leaseId = QString(), const int& timeoutInSec = -1);

  // ------------------------------------- PUBLIC SYNCHRONOUS -------------------------------------
  /*!
   * \brief listContainers List containers in an azure storage account
   *
   * \param[out] foundListOfContainers List of containers retrieved from Azure API (if no error)
   * \param marker (optional) Marker to list specific informations only
   * \param timeoutInSec (optional) Max time to wait answer (in sec)
   *
   * \return isErrorCodeSuccess(QNetworkReply::NetworkError) if list retrieved successfully on time
   */
  QNetworkReply::NetworkError listContainersSynchronous(QList< QMap<QString,QString> >& foundListOfContainers, const QString& marker = QString(), const int& timeoutInSec = 30, const bool& forceTimeoutOnApi = false);

  /*!
   * \brief listFiles List files in an azure storage container
   *
   * \param container Container to check
   * \param[out] foundListOfFiles List of files retrieved from Azure API (if no error)
   * \param marker (optional) Marker to check specific informations only
   * \param prefix (optional) Prefix to filter results
   * \param maxResults (optional, default: default Azure REST API number of results) Max number of elements
   * \param timeoutInSec (optional) Max time to wait answer (in sec)
   *
   * \return isErrorCodeSuccess(QNetworkReply::NetworkError) if list retrieved successfully on time
   */
  QNetworkReply::NetworkError listFilesSynchronous(const QString& container, QList< QMap<QString,QString> >& foundListOfFiles, const QString& marker = QString(), const QString& prefix = QString(), const int& maxResults = -1, const int& timeoutInSec = 30, const bool& forceTimeoutOnApi = false);

  /*!
   * \brief uploadFileSynchronous Synchronous method to upload a file from local directory into azure storage (remote path: \s container/\s blobName)
   *
   * \param filePath Absolute path of the local file to upload
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to create
   * \param blobType (optional) Type of blob to create
   * \param timeoutInSec (optional) Max time to wait answer (in sec)
   *
   * \return isErrorCodeSuccess(QNetworkReply::NetworkError) if uploaded successfully on time
   */
  QNetworkReply::NetworkError uploadFileSynchronous(const QString& filePath, const QString& container, const QString& blobName, const QString& blobType = "BlockBlob", const int& timeoutInSec = 30, const bool& forceTimeoutOnApi = false);

  /*!
   * \brief uploadFileQByteArraySynchronous Synchronous method to upload a file from QByteArray into azure storage (remote path: \s container/\s blobName)
   *
   * \param fileContent Content of the file to upload
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to create
   * \param blobType (optional) Type of blob to create
   * \param timeoutInSec (optional) Max time to wait answer (in sec)
   *
   * \return isErrorCodeSuccess(QNetworkReply::NetworkError) if uploaded successfully on time
   */
  QNetworkReply::NetworkError uploadFileQByteArraySynchronous(const QByteArray& fileContent, const QString& container, const QString& blobName, const QString& blobType = "BlockBlob", const int& timeoutInSec = 30, const bool& forceTimeoutOnApi = false);

  /*!
   * \brief deleteFileSynchronous Synchronous method to delete a file from azure storage (remote path: \s container/\s blobName)
   *
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to delete
   * \param timeoutInSec (optional) Max time to wait answer (in sec)
   *
   * \return isErrorCodeSuccess(QNetworkReply::NetworkError) if deleted successfully on time
   */
  QNetworkReply::NetworkError deleteFileSynchronous(const QString& container, const QString& blobName, const int& timeoutInSec = 30, const bool& forceTimeoutOnApi = false);

  /*!
   * \brief downloadFile Synchronous method to download a file from azure storage (remote path: \s container/\s blobName)
   *
   * \param container Container to take the file from
   * \param blobName File (any kind of blob) to download
   * \param[out] downloadedFile Downloaded file from Azure API (if no error)
   * \param timeoutInSec (optional) Max time to wait answer (in sec)
   *
   * \return isErrorCodeSuccess(QNetworkReply::NetworkError) if downloaded successfully on time
   */
  QNetworkReply::NetworkError downloadFileSynchronous(const QString& container, const QString& blobName, QByteArray& downloadedFile, const int& timeoutInSec = 30, const bool& forceTimeoutOnApi = false);

  /*!
   * \brief startCopyFile Synchronous START a copy of a file from azure storage account into a file in the same azure storage account
   *
   * WARNING: IT WILL NOT WAIT FULL COPY TO BE DONE, DO NOT DELETE SOURCE FILE DIRECTLY AFTER ELSE COPY WILL FAIL (CHECK API FOR MORE DETAIL)
   *
   * Full details: https://learn.microsoft.com/en-us/rest/api/storageservices/copy-blob?tabs=microsoft-entra-id
   *
   * \param containerFrom Container of the file to copy
   * \param blobNameFrom File name (with potential path) of the file to copy
   * \param containerTo Container of the new file
   * \param blobNameTo File name (with potential path) of the new file
   * \param timeoutInSec (optional) Max time specified to Azure REST API to wait answer (in sec)
   *
   * \return isErrorCodeSuccess(QNetworkReply::NetworkError) if copy STARTED correctly on time
   */
  QNetworkReply::NetworkError startCopyFileSynchronous(const QString& containerFrom, const QString& blobNameFrom, const QString& containerTo, const QString& blobNameTo, const int& timeoutInSec = 30, const bool& forceTimeoutOnApi = false);

  /*!
   * \brief createContainer Create a container
   *
   * \param container Container to create
   * \param timeoutInSec (optional) Max time to wait answer (in sec)
   *
   * \return isErrorCodeSuccess(QNetworkReply::NetworkError) if container created on time
   */
  QNetworkReply::NetworkError createContainerSynchronous(const QString& container, const int& timeoutInSec = 30, const bool& forceTimeoutOnApi = false);

  /*!
   * \brief createContainer Delete a container
   *
   * \param container Container to delete
   * \param leaseId (optional) Active lease to delete (check microsoft link for more details)
   * \param timeoutInSec (optional) Max time to wait answer (in sec)
   *
   * \return isErrorCodeSuccess(QNetworkReply::NetworkError) if container deleted on time
   */
  QNetworkReply::NetworkError deleteContainerSynchronous(const QString& container, const QString& leaseId = QString(), const int& timeoutInSec = 30, const bool& forceTimeoutOnApi = false);

  // ------------------------------------- PUBLIC STATIC -------------------------------------
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
