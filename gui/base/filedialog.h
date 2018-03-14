// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/base/filedialog.h
//! @brief     Defines functions openFileName(s), saveFileName, saveDirName in namespace file_dialog
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

//! Functions for opening files.

namespace file_dialog {

QFile* OutputFile(const QString&, QWidget*, const QString&, bool check_overwrite=true);

QStringList openFileNames(QWidget*, const QString&, QDir&, const QString&, bool plural=true);
QString openFileName(QWidget*, const QString&, QDir&, const QString&);
QString saveFileName(QWidget*, const QString&, QDir&, const QString&);
QString saveDirName(QWidget*, const QString&, QDir&);

} // namespace file_dialog

#endif // FILEDIALOG_H
