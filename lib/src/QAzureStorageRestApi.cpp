/*
 * \brief Send/Receive/List files (blob in container) from Azure storage
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 08 November 2023
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2023 Quentin Comte-Gaz
 * \version 3.2
 */

#include "QAzureStorageRestApi.h"

QAzureStorageRestApi::QAzureStorageRestApi(const QString& accountName, const QString& accountKeyOrSasCredentials, QObject* parent, const bool isAccountKey) :
  QObject(parent)
{
  updateCredentials(accountName, accountKeyOrSasCredentials, isAccountKey);
  m_manager = new QNetworkAccessManager(this);
}

void QAzureStorageRestApi::updateCredentials(const QString&accountName, const QString& accountKeyOrSasCredentials, const bool isAccountKey)
{
  m_accountName = accountName;
  m_accountKey = isAccountKey ? accountKeyOrSasCredentials : QString();
  m_sasKey = isAccountKey ? QString() : accountKeyOrSasCredentials;
}

QNetworkReply* QAzureStorageRestApi::listContainers(const QString& marker)
{
  QNetworkRequest request;

  // --- Prepare the URL ---
  QString additionalUrlParams = "comp=list";

  // Adding SAS authentication if SAS enabled
  if (!m_sasKey.isEmpty())
  {
      additionalUrlParams += "&" + m_sasKey;
  }
  QString url = generateUrl("", "", additionalUrlParams, marker);
  request.setUrl(QUrl(url));
  // ------------------------

  // --- Adding Account key authentication if Account key enabled ---
  QString currentDateTime = generateCurrentTimeUTC();
  if (!m_accountKey.isEmpty())
  {
      QStringList additionnalCanonicalRessources;
      additionnalCanonicalRessources.append("comp:list");

      if (!marker.isEmpty())
      {
          additionnalCanonicalRessources.append("marker:"+marker);
      }

      QString authorization = generateAutorizationHeader("GET", "", "", currentDateTime, 0, QStringList(), additionnalCanonicalRessources);
      request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
  }
  // ------------------------

  // --- Adding common header info ---
  request.setRawHeader(QByteArray("x-ms-date"), QByteArray(currentDateTime.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-version"), QByteArray(m_version.toStdString().c_str()));
  request.setRawHeader(QByteArray("Content-Length"), QByteArray("0"));
  // ------------------------

  // Sending the request
  return m_manager->get(request);
}

QList< QMap<QString,QString> > QAzureStorageRestApi::parseObjectList(const char* tag, const QByteArray& data, QString* NextMarker)
{
  QList< QMap<QString,QString> > objs;
  QXmlStreamReader xmlReader(data);

  while(!xmlReader.atEnd() && !xmlReader.hasError())
  {
    QXmlStreamReader::TokenType token = xmlReader.readNext();

    if(token == QXmlStreamReader::StartElement)
    {
      // Enter in a object
      if (xmlReader.name().toString().toStdString() == tag)
      {
        QMap<QString, QString> obj;

        // Get all data in the object
        while(!(xmlReader.tokenType() == QXmlStreamReader::EndElement &&
                xmlReader.name().toString().toStdString() == tag) &&
              xmlReader.tokenType() != QXmlStreamReader::TokenType::Invalid)
        {
          xmlReader.readNext();

          if (xmlReader.tokenType() == QXmlStreamReader::StartElement)
          {
            if (xmlReader.name().toString().toStdString() != "Properties")
            {
              QString key = xmlReader.name().toString();
              QString content;

              xmlReader.readNext();

              while (xmlReader.tokenType() == QXmlStreamReader::StartElement)
              {
                key = xmlReader.name().toString();
                xmlReader.readNext();
              }

              if (xmlReader.tokenType() == QXmlStreamReader::Characters)
              {
                content = xmlReader.text().toString();
              }
              else if (xmlReader.tokenType() != QXmlStreamReader::EndElement)
              {
                return QList< QMap<QString,QString> >();
              }

              obj.insert(key, content);
            }
          }
        }

        objs.append(obj);
      }
      else if (NextMarker && xmlReader.name().toString().toStdString() == "NextMarker")
      {
        while(!(xmlReader.tokenType() == QXmlStreamReader::EndElement &&
                xmlReader.name().toString().toStdString() == "NextMarker") &&
              xmlReader.tokenType() != QXmlStreamReader::TokenType::Invalid)
        {
          xmlReader.readNext();

          if (xmlReader.tokenType() == QXmlStreamReader::Characters)
          {
            *NextMarker = xmlReader.text().toString();
          }
        }
      }
    }
  }

  return objs;
}

QList< QMap<QString,QString> > QAzureStorageRestApi::parseContainerList(const QByteArray& xmlContainerList,
                                                                        QString* NextMarker)
{
  return parseObjectList("Container", xmlContainerList, NextMarker);
}

QList< QMap<QString,QString> > QAzureStorageRestApi::parseFileList(const QByteArray& xmlFileList,
                                                                   QString* NextMarker)
{
  return parseObjectList("Blob", xmlFileList, NextMarker);
}

QNetworkReply* QAzureStorageRestApi::listFiles(const QString& container, const QString& marker)
{
  QNetworkRequest request;

  // --- Prepare the URL ---
  QString additionalUrlParams = "restype=container&comp=list";

  // Adding SAS authentication if SAS enabled
  if (!m_sasKey.isEmpty())
  {
    additionalUrlParams += "&" + m_sasKey;
  }
  QString url = generateUrl(container, "", additionalUrlParams, marker);
  request.setUrl(QUrl(url));
  // ------------------------

  // --- Adding Account key authentication if Account key enabled ---
  QString currentDateTime = generateCurrentTimeUTC();
  if (!m_accountKey.isEmpty())
  {
    QStringList additionnalCanonicalRessources;
    additionnalCanonicalRessources.append("comp:list");

    if (!marker.isEmpty())
    {
      additionnalCanonicalRessources.append("marker:"+marker);
    }
    additionnalCanonicalRessources.append("restype:container");

    QString authorization = generateAutorizationHeader("GET", "", "", currentDateTime, 0, QStringList(), additionnalCanonicalRessources);
    request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
  }
  // ------------------------

  // --- Adding common header info ---
  request.setRawHeader(QByteArray("x-ms-date"), QByteArray(currentDateTime.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-version"), QByteArray(m_version.toStdString().c_str()));
  request.setRawHeader(QByteArray("Content-Length"), QByteArray("0"));
  // ------------------------

  // Sending the request
  return m_manager->get(request);
}

QNetworkReply* QAzureStorageRestApi::downloadFile(const QString& container, const QString& blobName)
{
  QNetworkRequest request;

  // --- Prepare the URL ---
  QString additionalUrlParams = QString();

  // Adding SAS authentication if SAS enabled
  if (!m_sasKey.isEmpty())
  {
    additionalUrlParams += "&" + m_sasKey;
  }
  QString url = generateUrl(container, blobName, additionalUrlParams);
  request.setUrl(QUrl(url));
  // ------------------------

  // --- Adding Account key authentication if Account key enabled ---
  QString currentDateTime = generateCurrentTimeUTC();
  if (!m_accountKey.isEmpty())
  {
    QString authorization = generateAutorizationHeader("GET", container, blobName, currentDateTime, 0);
    request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
  }
  // ------------------------

  // --- Adding common header info ---
  request.setRawHeader(QByteArray("x-ms-date"), QByteArray(currentDateTime.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-version"), QByteArray(m_version.toStdString().c_str()));
  request.setRawHeader(QByteArray("Content-Length"), QByteArray("0"));
  // ------------------------

  // Sending the request
  return m_manager->get(request);
}

QNetworkReply* QAzureStorageRestApi::uploadFileQByteArray(const QByteArray& fileContent, const QString& container, const QString& blobName, const QString& blobType)
{
  QNetworkRequest request;

  // --- Prepare the URL ---
  QString additionalUrlParams = QString();

  // Adding SAS authentication if SAS enabled
  if (!m_sasKey.isEmpty())
  {
    additionalUrlParams += "&" + m_sasKey;
  }
  QString url = generateUrl(container, blobName, additionalUrlParams);
  request.setUrl(QUrl(url));
  // ------------------------


  // --- Adding Account key authentication if Account key enabled ---
  QString currentDateTime = generateCurrentTimeUTC();
  int contentLength = fileContent.size();

  if (!m_accountKey.isEmpty())
  {
    QStringList additionalCanonicalHeaders;
    additionalCanonicalHeaders.append(QString("x-ms-blob-type:%1").arg(blobType));

    QString authorization = generateAutorizationHeader("PUT", container, blobName, currentDateTime, contentLength, additionalCanonicalHeaders, QStringList());
    request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
  }
  // ------------------------

  // --- Adding file size header info ---
  request.setRawHeader(QByteArray("Content-Length"),QByteArray(QString::number(contentLength).toStdString().c_str()));
  // ------------------------

  // --- Adding common header info ---
  request.setRawHeader(QByteArray("x-ms-date"),QByteArray(currentDateTime.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-version"),QByteArray(m_version.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-blob-type"),QByteArray(blobType.toStdString().c_str()));
  // ------------------------

  // Sending the request
  return m_manager->put(request, fileContent);
}

QNetworkReply* QAzureStorageRestApi::uploadFile(const QString& filePath, const QString& container, const QString& blobName, const QString& blobType)
{
  // --- Getting file content ---
  QByteArray fileContent;
  QFile file(filePath);

  if (file.open(QIODevice::ReadOnly))
  {
    fileContent = file.readAll();
  }
  else
  {
    return nullptr;
  }
  // ------------------------

  // Returning the upload result
  return uploadFileQByteArray(fileContent, container, blobName, blobType);
}

QNetworkReply* QAzureStorageRestApi::deleteFile(const QString& container, const QString& blobName)
{
  QNetworkRequest request;

  // --- Prepare the URL ---
  QString additionalUrlParams = QString();

  // Adding SAS authentication if SAS enabled
  if (!m_sasKey.isEmpty())
  {
    additionalUrlParams += "&" + m_sasKey;
  }
  QString url = generateUrl(container, blobName, additionalUrlParams);
  request.setUrl(QUrl(url));
  // ------------------------

  // --- Adding Account key authentication if Account key enabled ---
  QString currentDateTime = generateCurrentTimeUTC();
  if (!m_accountKey.isEmpty())
  {
    QString authorization = generateAutorizationHeader("DELETE", container, blobName, currentDateTime, 0);
    request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
  }
  // ------------------------

  // --- Adding common header info ---
  request.setRawHeader(QByteArray("x-ms-date"),QByteArray(currentDateTime.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-version"),QByteArray(m_version.toStdString().c_str()));
  // ------------------------

  // Sending the request
  return m_manager->deleteResource(request);
}

QString QAzureStorageRestApi::generateCurrentTimeUTC()
{
  return QLocale(QLocale::English).toString(QDateTime::currentDateTimeUtc(), "ddd, dd MMM yyyy hh:mm:ss").append(" GMT");
}

QString QAzureStorageRestApi::generateHeader(
    const QString& httpVerb, const QString& contentEncoding, const QString& contentLanguage, const QString& contentLength,
    const QString& contentMd5, const QString& contentType, const QString& date, const QString& ifModifiedSince,
    const QString& ifMatch, const QString& ifNoneMatch, const QString& ifUnmodifiedSince, const QString& range,
    const QString& canonicalizedHeaders, const QString& canonicalizedResource)
{
  QString result = "";

  result = result + httpVerb + "\n";
  result = result + contentEncoding + "\n";
  result = result + contentLanguage + "\n";
  result = result + contentLength + "\n";
  result = result + contentMd5 + "\n";
  result = result + contentType + "\n";
  result = result + date + "\n";
  result = result + ifModifiedSince + "\n";
  result = result + ifMatch + "\n";
  result = result + ifNoneMatch + "\n";
  result = result + ifUnmodifiedSince + "\n";
  result = result + range + "\n";
  result = result + canonicalizedHeaders + "\n";
  result = result + canonicalizedResource;

  return result;
}

QString QAzureStorageRestApi::generateAutorizationHeader(const QString& httpVerb, const QString& container,
                                                         const QString& blobName, const QString& currentDateTime,
                                                         const long& contentLength, const QStringList additionnalCanonicalHeaders,
                                                         const QStringList additionnalCanonicalRessources)
{
  // Create canonicalized header
  QString canonicalizedHeaders;
  for (const QString& additionnalCanonicalHeader : additionnalCanonicalHeaders)
  {
    canonicalizedHeaders.append(additionnalCanonicalHeader+"\n");
  }
  canonicalizedHeaders.append(QString("x-ms-date:%1\nx-ms-version:%2").arg(currentDateTime, m_version));

  // Create canonicalized ressource
  QString canonicalizedResource;
  if (blobName.isEmpty())
  {
    canonicalizedResource = QString("/%1/%2").arg(m_accountName, container);
  }
  else
  {
    canonicalizedResource = QString("/%1/%2/%3").arg(m_accountName, container,
                                                     QUrl::toPercentEncoding(blobName,"/"));
  }

  for (const QString& additionnalCanonicalRessource : additionnalCanonicalRessources)
  {
    canonicalizedResource.append("\n"+additionnalCanonicalRessource);
  }

  // Create signature
  QString signature = generateHeader(httpVerb, "", "", (contentLength==0 ? "" : QString::number(contentLength)),
                                     "", "", "", "",
                                     "", "", "", "", canonicalizedHeaders, canonicalizedResource);

  // Create authorization header
  QByteArray authorizationHeader = QMessageAuthenticationCode::hash(
        QByteArray(signature.toUtf8()),
        QByteArray(QByteArray::fromBase64(m_accountKey.toStdString().c_str())),
        QCryptographicHash::Sha256);
  authorizationHeader = authorizationHeader.toBase64();

  return QString("SharedKey %1:%2").arg(m_accountName, QString(authorizationHeader));
}

QString QAzureStorageRestApi::generateUrl(const QString& container, const QString& blobName, const QString& additionnalParameters,
                                          const QString& marker)
{
  QString blobEndPoint = QString("https://%1.blob.core.windows.net/").arg(m_accountName);
  QString url = blobEndPoint + container;
  if (!blobName.isEmpty())
  {
    url.append("/" + QUrl::toPercentEncoding(blobName,"/"));
  }

  if (!additionnalParameters.isEmpty())
  {
    url.append("?"+additionnalParameters);
    if (!marker.isEmpty())
    {
      url.append("&marker="+marker);
    }
  }

  return url;
}
