// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_types_fwd.h
//! @brief     Forward declaration of core types
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_TYPES_FWD_H
#define CORE_TYPES_FWD_H

#include "core_defs.h"
#include "core_types_fwd.h"

namespace core {
//------------------------------------------------------------------------------

struct XY;

class  Range;
class  Ranges;

class  Curve;

struct vector3d;
struct matrix3d;

class  Image;
struct ImageTransform;
struct DiffractionAngles;

typedef float intens_t;   ///< intensity; float should suffice

class JsonObj;            /// for saving / loading
typedef JsonObj const &rcJsonObj;

class Lens;       typedef QSharedPointer<Lens>        shp_LensSystem; // RENAME to shp_LensSystem
class File;       typedef QSharedPointer<File>        shp_File;
class Dataset;    typedef QSharedPointer<Dataset>     shp_Dataset;
class Reflection; typedef QSharedPointer<Reflection>  shp_Reflection;

namespace fit {

class Function;
class PeakFunction;

}

//------------------------------------------------------------------------------
}
#endif // CORE_TYPES_FWD_H
