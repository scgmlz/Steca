// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/write_file.cpp
//! @brief     Declares class WriteFile
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef WRITEFILE_H
#define WRITEFILE_H

#include "def/macros.h"
#include <QFile>

class WriteFile : public QFile {
private:
public:
    WriteFile(rcstr path) THROWS;
};

#endif // WRITEFILE_H
