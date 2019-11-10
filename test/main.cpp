/*
 * \brief Test the Azure storage rest api library
 *
 * \author Quentin Comte-Gaz <quentin@comte-gaz.com>
 * \date 10 November 2019
 * \license MIT License (contact me if too restrictive)
 * \copyright Copyright (c) 2019 Quentin Comte-Gaz
 * \version 2.0
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

TEST_CASE("List files (invalid response)")
{
    QString username("fakeUser");
    QString pass("fakePass");
    QString container = "invalidContainer";

    QAzureStorageRestApi api(username, pass);

    QNetworkReply* reply = api.listFiles(container);
    REQUIRE(reply != nullptr);
    REQUIRE(reply->error() == QNetworkReply::NetworkError::NoError);
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

TEST_CASE("Upload file (invalid response)")
{
    QString username("fakeUser");
    QString pass("fakePass");

    QString path = "dummyFile.txt";
    QFile file(path);
    REQUIRE(file.open(QIODevice::WriteOnly));
    {
        QTextStream stream(&file);
        stream << "Dummy information" << endl;
    }
    file.close();

    QString container = "invalidContainer";
    QString blob = "invalidBlolName";

    QAzureStorageRestApi api(username, pass);

    QNetworkReply* reply = api.uploadFile(path, container, blob);
    REQUIRE(reply != nullptr);
    REQUIRE(reply->error() == QNetworkReply::NetworkError::NoError);
}

TEST_CASE("Download file (invalid response)")
{
    QString username("fakeUser");
    QString pass("fakePass");
    QString container = "invalidContainer";
    QString blob = "invalidBlolName";

    QAzureStorageRestApi api(username, pass);

    QNetworkReply* reply = api.downloadFile(container, blob);
    REQUIRE(reply != nullptr);
    REQUIRE(reply->error() == QNetworkReply::NetworkError::NoError);
}

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << QString("Launching test using catch version %1.%2 - %3").arg(CATCH_VERSION_MAJOR).arg(CATCH_VERSION_MINOR).arg(CATCH_VERSION_PATCH);

    int result = Catch::Session().run(argc, argv);
    return result;
}
