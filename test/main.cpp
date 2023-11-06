/*
 * \brief Test the Azure storage rest api library
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 17 August 2022
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2022 Quentin Comte-Gaz
 * \version 3.0
 */
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <QCoreApplication>
#include <QDebug>

#include <QAzureStorageRestApi.h>

TEST_CASE("Create instance")
{
    QString username("fakeUser");
    QString pass("fakePass");

    QAzureStorageRestApi api(username, pass);
}

TEST_CASE("Update credentials")
{
    QString username("fakeUser");
    QString pass("fakePass");

    QAzureStorageRestApi api(username, pass);

    username = "fakeUser2";
    pass = "fakePass2";
    api.updateCredentials(username, pass);
}

TEST_CASE("Upload file (invalid path)")
{
    QString username("fakeUser");
    QString pass("fakePass");
    QString path = "invalidPath";
    QString container = "invalidContainer";
    QString blob = "invalidBlolName";

    QAzureStorageRestApi api(username, pass);

    QNetworkReply* reply = api.uploadFile(path, container, blob);
    REQUIRE(reply == nullptr);
}

TEST_CASE("List files")
{
    QString username("fakeUser");
    QString pass("fakePass");
    QString container = "invalidContainer";

    QAzureStorageRestApi api(username, pass);

    QNetworkReply* reply = api.listFiles(container);
    REQUIRE(reply != nullptr);
    qDebug() << "Error response: " << reply->error();
}

TEST_CASE("Upload file")
{
    QString username("fakeUser");
    QString pass("fakePass");

    QString path = "dummyFile.txt";
    QFile file(path);
    REQUIRE(file.open(QIODevice::WriteOnly));
    {
        QTextStream stream(&file);
        stream << "Dummy information\n";
    }
    file.close();

    QString container = "invalidContainer";
    QString blob = "invalidBlolName";

    QAzureStorageRestApi api(username, pass);

    QNetworkReply* reply = api.uploadFile(path, container, blob);
    REQUIRE(reply != nullptr);
    qDebug() << "Error response: " << reply->error();
}

TEST_CASE("Download file")
{
    QString username("fakeUser");
    QString pass("fakePass");
    QString container = "invalidContainer";
    QString blob = "invalidBlolName";

    QAzureStorageRestApi api(username, pass);

    QNetworkReply* reply = api.downloadFile(container, blob);
    REQUIRE(reply != nullptr);
    qDebug() << "Error response: " << reply->error();
}

TEST_CASE("Parse file list")
{
    QByteArray fileList;

    QList< QMap<QString,QString> > res = QAzureStorageRestApi::parseFileList(fileList);
    qDebug() << "List: " << res;
    REQUIRE(res.isEmpty());

    fileList = QByteArray("<?xml version=\"1.0\" encoding=\"utf-8\"?>\
                            <EnumerationResults ServiceEndpoint=\"http://myaccount.blob.core.windows.net/\"  ContainerName=\"mycontainer\">\
                              <Prefix>string-value</Prefix>\
                              <Marker>string-value</Marker>\
                              <MaxResults>int-value</MaxResults>\
                              <Delimiter>string-value</Delimiter>\
                              <Blobs>\
                                <Blob>\
                                  <Name>blob-name</Name>\
                                  <Deleted>true</Deleted>\
                                  <Snapshot>date-time-value</Snapshot>\
                                  <Properties>\
                                    <Creation-Time>date-time-value</Creation-Time>\
                                    <Last-Modified>date-time-value</Last-Modified>\
                                    <Etag>etag</Etag>\
                                    <Content-Length>size-in-bytes</Content-Length>\
                                    <Content-Type>blob-content-type</Content-Type>\
                                    <Content-Encoding />\
                                    <Content-Language />\
                                    <Content-MD5 />\
                                    <Cache-Control />\
                                    <BlobType>BlockBlob|PageBlob|AppendBlob</BlobType>\
                                    <AccessTier>tier</AccessTier>\
                                    <LeaseStatus>locked|unlocked</LeaseStatus>\
                                    <LeaseState>available | leased | expired | breaking | broken</LeaseState>\
                                    <LeaseDuration>infinite | fixed</LeaseDuration>\
                                    <CopyId>id</CopyId>\
                                    <CopyStatus>pending | success | aborted | failed </CopyStatus>\
                                    <CopySource>source url</CopySource>\
                                    <CopyProgress>bytes copied/bytes total</CopyProgress>\
                                    <CopyCompletionTime>datetime</CopyCompletionTime>\
                                    <CopyStatusDescription>error string</CopyStatusDescription>\
                                    <ServerEncrypted>true</ServerEncrypted>\
                                    <IncrementalCopy>true</IncrementalCopy>\
                                    <AccessTierInferred>true</AccessTierInferred>\
                                    <AccessTierChangeTime>datetime</AccessTierChangeTime>\
                                    <DeletedTime>datetime</DeletedTime>\
                                    <RemainingRetentionDays>no-of-days</RemainingRetentionDays>\
                                  </Properties>\
                                  <Metadata>\
                                    <Name>value</Name>\
                                  </Metadata>\
                                </Blob>\
                                <Blob>\
                                  <Name>blob-name-2</Name>\
                                  <FakeKey>FakeValue</FakeKey>\
                                  <FakeKey2>FakeValue2</FakeKey2>\
                                </Blob>\
                                <BlobPrefix>\
                                  <Name>blob-prefix</Name>\
                                </BlobPrefix>\
                              </Blobs>\
                              <NextMarker />\
                            </EnumerationResults>");

    res = QAzureStorageRestApi::parseFileList(fileList);
    qDebug() << "Extracted blobl list: " << res;
    REQUIRE(!res.isEmpty());
    REQUIRE(res.count() == 2);
    REQUIRE(res[1] == QMap<QString,QString>({{"FakeKey", "FakeValue"}, {"FakeKey2", "FakeValue2"}, {"Name", "blob-name-2"}}));
}

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << QString("Launching test using catch version %1.%2 - %3").arg(CATCH_VERSION_MAJOR).arg(CATCH_VERSION_MINOR).arg(CATCH_VERSION_PATCH);

    int result = Catch::Session().run(argc, argv);
    return result;
}
