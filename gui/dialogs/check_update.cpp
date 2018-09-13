//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/check_update.cpp
//! @brief     Implements class CheckUpdate
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/check_update.h"
#include "manifest.h"
#include <QApplication>
#include <QMessageBox>
#include <QNetworkReply>
#include <QStringBuilder> // for ".." % ..

//! Queries whether there is a new version of this software, and shows message box.

CheckUpdate::CheckUpdate(QWidget* parent)
{
    QNetworkRequest req;
    QString ver = qApp->applicationVersion();
    QString qry = ver % "\t| " % QSysInfo::prettyProductName();
    req.setUrl(QUrl(QString(STECA2_VERSION_URL) % "?" % qry));
    auto* manager = new QNetworkAccessManager;
    QObject::connect(
        manager, &QNetworkAccessManager::finished, [parent,manager](QNetworkReply* reply){
            if (QNetworkReply::NoError != reply->error()) {
                qWarning() << "Network Error: " << reply->errorString();
                return;
            }
            QString ver = qApp->applicationVersion();
            QString lastVer = reply->readAll().trimmed();
            QString name = qApp->applicationName();
            QString result;
            if (ver != lastVer)
                result = QString(
                    "<p>The latest released %1 version is %2. You have "
                    "version %3.</p>"
                    "<p><a href='%4'>Open download location in external browser</a></p>")
                    .arg(name, lastVer, ver, STECA2_DOWNLOAD_URL);
            else
                result = QString(
                    "<p>You have the latest released %1 version (%2).</p>").arg(name).arg(ver);
            QMessageBox::information(parent, QString("%1 update").arg(name), result);
            reply->deleteLater();
            manager->deleteLater();
        });
    manager->get(req);
}
