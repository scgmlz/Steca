// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/io/io_io.h
//! @brief     Defines loader functions in namespace io.
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef IO_IO_H
#define IO_IO_H

#include "def/macros.h"
#include "typ/str.h"
#include <QSharedPointer>

class Datafile;
class QFileInfo;

namespace io {

//! load a file; file type will be sensed
QSharedPointer<Datafile const> loadDatafile(rcstr filePath) THROWS;

str loadComment(QFileInfo const& info);

} // namespace io

#endif // IO_IO_H
