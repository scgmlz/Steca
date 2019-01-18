//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/file_dialog.h
//! @brief     Defines functions in namespace file_dialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

#include <QDir>
#include <QFile>

//! GUI dialog functions for opening files.

namespace file_dialog {

enum class eFileOverwritePolicy {
    PROMPT = 0, // promps a confirmation dialog.
    PANIC = 1, // throws an exception
    SILENT_OVERWRITE = 2, // silently overrides the files
};

void setFileOverwritePolicy(eFileOverwritePolicy);

bool confirmOverwrite(const QString& name, QObject* parent, const QString& path);
QFile* openFileConfirmOverwrite(const QString&, QObject*, const QString&);
QStringList queryImportFileNames(QObject*, const QString&, QDir&, const QString&, bool plural=true);
QString queryImportFileName(QObject*, const QString&, QDir&, const QString&);
QString queryExportFileName(QObject*, const QString&, QDir&, const QString&);
QString queryDirectory(QObject*, const QString&, const QString&);

} // namespace file_dialog

#endif // FILE_DIALOG_H
