// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/file_dialog.h
//! @brief     Defines functions queryImportFileName(s), queryExportFileName, queryDirectory in namespace file_dialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef FILE_DIALOG_H
#define FILE_DIALOG_H

//! Functions for opening files.

namespace file_dialog {

QFile* openFileConfirmOverwrite(const QString&, QWidget*, const QString&);
QStringList queryImportFileNames(QWidget*, const QString&, QDir&, const QString&, bool plural=true);
QString queryImportFileName(QWidget*, const QString&, QDir&, const QString&);
QString queryExportFileName(QWidget*, const QString&, QDir&, const QString&);
QString queryDirectory(QWidget*, const QString&, QDir&);

} // namespace file_dialog

#endif // FILE_DIALOG_H
