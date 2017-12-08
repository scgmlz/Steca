// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/filedialog.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef FILEDIALOG_H
#define FILEDIALOG_H

#include "typ/str.h"
#include "typ/strlst.h"

class QWidget;

namespace gui {
namespace file_dialog {

str openFileName(QWidget*, rcstr caption, rcstr dir, rcstr filter);
str_lst openFileNames(QWidget*, rcstr caption, rcstr dir, rcstr filter);

str saveFileName(QWidget*, rcstr caption, rcstr dir, rcstr filter);
str saveDirName(QWidget*, rcstr caption, rcstr dir);
}
}
#endif // FILEDIALOG_H
