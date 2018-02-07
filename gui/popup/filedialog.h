// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/popup/filedialog.h
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

//! Functions for opening files. TODO: clarify relation to newQ::OutputFile

namespace file_dialog {

QFile* OutputFile(
    const QString& name, QWidget* parent, const QString& path, bool check_overwrite=true);

QStringList openFileNames(QWidget*, const QString& caption, const QString& dir,
                          const QString& filter, bool plural=true);
QString openFileName(QWidget*, const QString& caption, const QString& dir, const QString& filter);

QString saveFileName(QWidget*, const QString& caption, const QString& dir, const QString& filter);
QString saveDirName(QWidget*, const QString& caption, const QString& dir);

} // namespace file_dialog


#endif // FILEDIALOG_H
