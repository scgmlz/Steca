//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      core/loaders/load_caress.h
//! @brief     Declares function load::loadCaress
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

class Rawfile;
class QString;

namespace load {

Rawfile loadCaress(const QString& filePath);
QString loadCaressComment(const QString& filePath);

}
