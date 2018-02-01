// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/write_file.cpp
//! @brief     Implements class WriteFile
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "write_file.h"
#include "core/typ/exception.h"
#include <QDebug>
#include <QMessageBox>
#include <QStringBuilder> // for ".." % ..

WriteFile::WriteFile(const QString& path) THROWS : QFile(path) {
    if (QFile::exists()) {
        if (QMessageBox::question(nullptr, "File exists", "Overwrite " % path % " ?") !=
            QMessageBox::Yes)
            THROW("");
    }

    if (!QFile::open(QIODevice::WriteOnly | QIODevice::Text))
        qWarning() << "Cannot open file for writing: " << path;
}
