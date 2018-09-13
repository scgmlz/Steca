//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/io_session.cpp
//! @brief     Implements functions in namespace ioSession
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/io_session.h"
#include "core/session.h"
#include "core/misc/async.h"
#include "core/misc/exception.h"
#include "gui/dialogs/file_dialog.h"
#include "qcr/base/debug.h" // warning
#include <QDir>
#include <QStringBuilder> // for ".." % ..

namespace {
QDir sessionDir_ {QDir::homePath()};
} // namespace

void ioSession::load(QWidget* parent)
{
    QString fileName = file_dialog::queryImportFileName(
        parent, "Load session", sessionDir_, "Session files (*.ste)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!(file.open(QIODevice::ReadOnly | QIODevice::Text))) {
        qWarning() << ("Cannot open file for reading: " % fileName);
        return;
    }
    try {
        TakesLongTime __("loadSession");
        gSession->sessionFromJson(file.readAll());
    } catch(Exception& ex) {
        qWarning() << "Could not load session from file " << fileName << ":\n"
                   << ex.msg() << "\n"
                   << "The application may now be in an inconsistent state.\n"
                   << "Please consider to quit the application, and start afresh.\n";
        gSession->clear();
    }
}

void ioSession::save(QWidget* parent)
{
    QString fileName = file_dialog::queryExportFileName(
        parent, "Save session", sessionDir_, "Session files (*.ste)");
    if (!fileName.endsWith(".ste"))
        fileName += ".ste";
    QFileInfo fileInfo(fileName);
    QFile* file = file_dialog::openFileConfirmOverwrite("file", parent, fileInfo.filePath());
    if (!file)
        return;
    const int result = file->write(gSession->serializeSession());
    delete file;
    if (!(result >= 0))
        qWarning() << "Could not write session";
}
