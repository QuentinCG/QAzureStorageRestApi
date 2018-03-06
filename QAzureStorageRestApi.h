/*
 * \brief Send/Receive/List files (blobs in container) from Azure storage
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 5 March 2018
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2018 Quentin Comte-Gaz
 * \version 1.0
 */

#ifndef QAZURESTORAGERESTAPI_H
#define QAZURESTORAGERESTAPI_H

#include <QObject>
#include <QtNetwork>

class QAzureStorageRestApi : public QObject
{
  Q_OBJECT

public:
  /*!
   * \brief QAzureStorageRestApi Send/Receive/List files from Azure storage
   * \param accountName Account name
   * \param accountKey Account key
   * \param parent (optional) QObject parent
   */
  QAzureStorageRestApi(const QString& accountName, const QString& accountKey, QObject* parent = 0);

  /*!
   * \brief updateCredentials Update the account name and account key if changed
   * \param accountName Account name
   * \param accountKey Account key
   */
  void updateCredentials(const QString&accountName, const QString& accountKey);

  /*!
   * \brief listFiles List files in an azure storage container
   *
   * \param container Container to check
   *
   * \return Reply from Azure (XML encoded file list if QNetworkReply::isFinished() is
   *         triggered with QNetworkReply::error() ==  QNetworkReply::NetworkError::NoError)
   *         Return value can be nullptr if invalid request
   *         It is possible to decode the reply from Azure with \s QAzureStorageRestApi::parseFileList
   */
  QNetworkReply* listFiles(const QString& container);

  /*!
   * \brief uploadFile Upload a file from azure storage (remote path: \s container/\s blobName)
   *
   * \param filePath Absolute path of the local file to upload
   * \param container Container to put the file into
   * \param blobName Name of the file (blob) to create
   * \param blobType (optional) Type of blob to create
   *
   * \return Reply from Azure (Uploaded with success if QNetworkReply::isFinished() is
   *         triggered with QNetworkReply::error() ==  QNetworkReply::NetworkError::NoError)
   *         Return value can be nullptr if invalid request
   */
  QNetworkReply* uploadFile(const QString& filePath, const QString& container, const QString& blobName, const QString& blobType = "BlockBlob");

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

  /*!
   * \brief parseFileList Helper to convert XML file list received from Azure into Qt compatible format
   *
   * \param xmlFileList XML file list received using \s QAzureStorageRestApi::listFiles
   *
   * \return List of files with all available information on files (name, type, md5, ...)
   */
  static QList< QMap<QString,QString> > parseFileList(const QByteArray& xmlFileList);

private:
  QString generateCurrentTimeUTC();
  QString generateHeader(const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&,
                         const QString&, const QString&, const QString&, const QString&, const QString&, const QString&, const QString&);
  QString generateAutorizationHeader(const QString& httpVerb, const QString& container, const QString& blobName,
                                     const QString& currentDateTime, const long& contentLength,
                                     const QStringList additionnalCanonicalHeaders = QStringList(),
                                     const QStringList additionnalCanonicalRessources = QStringList());
  QString generateUrl(const QString& container, const QString& blobName = QString(), const QString& additionnalParameters = QString());

private:
  QString m_version = "2009-09-19";
  QString m_accountName;
  QString m_accountKey;
  QNetworkAccessManager* m_manager;

};

#endif // QAZURESTORAGERESTAPI_H
