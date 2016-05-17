// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
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

namespace core {
//------------------------------------------------------------------------------

struct IJ; typedef IJ const& rcIJ;
struct XY; typedef XY const& rcXY;

struct Range;  typedef Range  const& rcRange;
class  Ranges; typedef Ranges const& rcRanges;

class  Curve;  typedef Curve  const& rcCurve;

struct vector3f;
struct vector3d;
struct matrix3d;

class  Image;   typedef Image const&  rcImage;
struct ImageTransform;

typedef float inten_t;    ///< intensity if in a vector; float should suffice

class JsonObj; typedef JsonObj const& rcJsonObj;
class JsonArr; typedef JsonArr const& rcJsonArr;

class Session;  typedef Session const&  rcSession;
class File;     typedef File const&     rcFile;

struct Metadata; typedef Metadata const& rcMetadata;
class  Dataset;  typedef Dataset  const& rcDataset;
class  Datasets; typedef Datasets const& rcDatasets;

class ImageLens; class Lens;
class AngleMap;

class Reflection;      typedef Reflection      const& rcReflection;
class ReflectionInfo;  typedef ReflectionInfo  const& rcReflectionInfo;
class ReflectionInfos; typedef ReflectionInfos const& rcReflectionInfos;

typedef QSharedPointer<File>        shp_File;
typedef QSharedPointer<Metadata>    shp_Metadata;
typedef QSharedPointer<Dataset>     shp_Dataset;
typedef QSharedPointer<ImageLens>   shp_ImageLens;
typedef QSharedPointer<Lens>        shp_Lens;
typedef QSharedPointer<AngleMap>    shp_AngleMap;
typedef QSharedPointer<Reflection>  shp_Reflection;

typedef QVector<shp_Reflection>     Reflections;
typedef Reflections const& rcReflections;

enum class eNorm {
  NONE,
  DELTA_MONITOR_COUNT, DELTA_TIME, BACKGROUND,
};

enum class ePeakType {
  RAW, GAUSSIAN, LORENTZIAN, PSEUDOVOIGT1, PSEUDOVOIGT2,
  NUM_TYPES
};

namespace fit {
class Function;
class PeakFunction;
}

//------------------------------------------------------------------------------
}
#endif // CORE_TYPES_FWD_H
