//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/data/export.h
//! @brief     Defines
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include <QString>

namespace data_export {

//! Returns templatedName with '%d' replaced by string representation of num.

//!  The string representation of num has leading zeros, and its number of
//!  digits is determined by the maximum value maxNum.

QString numberedFileName(const QString& templatedName, int num, int maxNum);

} // namespace data_export
