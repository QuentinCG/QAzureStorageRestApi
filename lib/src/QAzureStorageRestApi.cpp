/*
 * \brief Send/Receive/List files (blob in container) from Azure storage
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 10 November 2019
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2019 Quentin Comte-Gaz
 * \version 2.0
 */

#include "QAzureStorageRestApi.h"

QAzureStorageRestApi::QAzureStorageRestApi(const QString& accountName, const QString& accountKey, QObject* parent) :
  QObject(parent),
  m_accountName(accountName),
  m_accountKey(accountKey)
{
  m_manager = new QNetworkAccessManager(this);
}

void QAzureStorageRestApi::updateCredentials(const QString&accountName, const QString& accountKey)
{
  m_accountName = accountName;
  m_accountKey = accountKey;
}

QList< QMap<QString,QString> > QAzureStorageRestApi::parseFileList(const QByteArray& data)
{
  QList< QMap<QString,QString> > files;
  QXmlStreamReader xmlReader(data);

  while(!xmlReader.atEnd() && !xmlReader.hasError())
  {
    QXmlStreamReader::TokenType token = xmlReader.readNext();

    if(token == QXmlStreamReader::StartElement)
    {
      // Enter in a blob
      if (xmlReader.name() == "Blob")
      {
        QMap<QString, QString> file;

        // Get all data in the blob
        while(!(xmlReader.tokenType() == QXmlStreamReader::EndElement &&
            xmlReader.name() == "Blob"))
        {
          xmlReader.readNext();

          if (xmlReader.tokenType() == QXmlStreamReader::StartElement)
          {
            if (xmlReader.name() != "Properties")
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

              file.insert(key, content);
            }
          }
        }

        files.append(file);
      }
    }
  }

  return files;
}

QNetworkReply* QAzureStorageRestApi::listFiles(const QString& container)
{
  QString currentDateTime = generateCurrentTimeUTC();

  QString url = generateUrl(container, "", "restype=container&comp=list");

  QStringList additionnalCanonicalRessources;
  additionnalCanonicalRessources.append("comp:list");
  additionnalCanonicalRessources.append("restype:container");

  QString authorization = generateAutorizationHeader("GET", container, "", currentDateTime, 0, QStringList(), additionnalCanonicalRessources);

  QNetworkRequest request;

  request.setUrl(QUrl(url));
  request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-date"), QByteArray(currentDateTime.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-version"), QByteArray(m_version.toStdString().c_str()));
  request.setRawHeader(QByteArray("Content-Length"), QByteArray("0"));

  return m_manager->get(request);
}

QNetworkReply* QAzureStorageRestApi::downloadFile(const QString& container, const QString& blobName)
{
  QString currentDateTime = generateCurrentTimeUTC();

  QString url = generateUrl(container, blobName);

  QString authorization = generateAutorizationHeader("GET", container, blobName, currentDateTime, 0);

  QNetworkRequest request;

  request.setUrl(QUrl(url));
  request.setRawHeader(QByteArray("Authorization"), QByteArray(authorization.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-date"), QByteArray(currentDateTime.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-version"), QByteArray(m_version.toStdString().c_str()));
  request.setRawHeader(QByteArray("Content-Length"), QByteArray("0"));

  return m_manager->get(request);
}

QNetworkReply* QAzureStorageRestApi::uploadFile(const QString& filePath, const QString& container, const QString& blobName, const QString& blobType)
{
  QByteArray data;
  QFile file(filePath);

  if (file.open(QIODevice::ReadOnly))
  {
    data = file.readAll();
  }
  else
  {
    return nullptr;
  }

  QString currentDateTime = generateCurrentTimeUTC();
  int contentLength = data.size();

  QStringList additionalCanonicalHeaders;
  additionalCanonicalHeaders.append(QString("x-ms-blob-type:%1").arg(blobType));

  QString url = generateUrl(container, blobName);
  QString authorization = generateAutorizationHeader("PUT", container, blobName, currentDateTime, data.size(), additionalCanonicalHeaders, QStringList());

  QNetworkRequest request;

  request.setUrl(QUrl(url));
  request.setRawHeader(QByteArray("Authorization"),QByteArray(authorization.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-date"),QByteArray(currentDateTime.toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-version"),QByteArray(m_version.toStdString().c_str()));
  request.setRawHeader(QByteArray("Content-Length"),QByteArray(QString::number(contentLength).toStdString().c_str()));
  request.setRawHeader(QByteArray("x-ms-blob-type"),QByteArray(blobType.toStdString().c_str()));

  return m_manager->put(request, data);
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
    canonicalizedResource = QString("/%1/%2/%3").arg(m_accountName, container, blobName);
  }

  for (const QString& additionnalCanonicalRessource : additionnalCanonicalRessources)
  {
    canonicalizedResource.append("\n"+additionnalCanonicalRessource);
  }

  // Create signature
  QString signature = generateHeader(httpVerb, "", "", QString::number(contentLength), "", "", "", "",
                                     "", "", "", "", canonicalizedHeaders, canonicalizedResource);

  // Create authorization header
  QByteArray authorizationHeader = QMessageAuthenticationCode::hash(
        QByteArray(signature.toUtf8()),
        QByteArray(QByteArray::fromBase64(m_accountKey.toStdString().c_str())),
        QCryptographicHash::Sha256);
  authorizationHeader = authorizationHeader.toBase64();

  return QString("SharedKey %1:%2").arg(m_accountName, QString(authorizationHeader));
}

QString QAzureStorageRestApi::generateUrl(const QString& container, const QString& blobName, const QString& additionnalParameters)
{
  QString blobEndPoint = QString("https://%1.blob.core.windows.net/").arg(m_accountName);
  QString url = blobEndPoint + container;
  if (!blobName.isEmpty())
  {
    url.append("/" + blobName);
  }

  if (!additionnalParameters.isEmpty())
  {
    url.append("?"+additionnalParameters);
  }

  return url;
}
