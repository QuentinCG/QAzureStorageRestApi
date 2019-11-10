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

int main(int argc, char* argv[])
{
    QCoreApplication a(argc, argv);

    qDebug() << QString("Launching test using catch version %1.%2 - %3").arg(CATCH_VERSION_MAJOR).arg(CATCH_VERSION_MINOR).arg(CATCH_VERSION_PATCH);

    int result = Catch::Session().run(argc, argv);
    return result;
}
