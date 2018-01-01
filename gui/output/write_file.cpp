// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/output/write_file.cpp
//! @brief     Implements class WriteFile
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "typ/exception.h"
#include "write_file.h"
#include <QMessageBox>
#include <QStringBuilder> // for ".." % ..

WriteFile::WriteFile(rcstr path) THROWS : QFile(path) {
    if (QFile::exists()) {
        if (QMessageBox::Yes
            != QMessageBox::question(nullptr, "File exists", "Overwrite " % path % " ?"))
            THROW_SILENT();
    }

    RUNTIME_CHECK(
        QFile::open(QIODevice::WriteOnly | QIODevice::Text),
        "Cannot open file for writing: " % path);
}
