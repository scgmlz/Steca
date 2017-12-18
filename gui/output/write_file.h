// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/thehub.cpp
//! @brief     Declares class WriteFile
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //


#include "def/macros.h"
#include "typ/str.h"
#include <QFile>

class WriteFile : public QFile {
private:
    using super = QFile;
public:
    WriteFile(rcstr path) THROWS;
};
