// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      core_dataset.h
//! @brief     Dataset = a collection of images + metadata.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef CORE_DATASET_H
#define CORE_DATASET_H

#include "types/core_type_image.h"

namespace core {
//------------------------------------------------------------------------------

class Datasets;

/// Dataset = image + metadata
class Dataset final {
  friend class Datasets;
public:
  // attribute list - will be dynamic
  static uint  numAttributes();
  static rcstr attributeTag(uint);

public:
  Dataset(rcstr date, rcstr comment,
          qreal motorXT,  qreal motorYT,  qreal motorZT,
          qreal motorOmg, qreal motorTth, qreal motorPhi, qreal motorChi,
          qreal motorPST, qreal motorSST, qreal motorOMGM,
          qreal deltaMonitorCount, qreal deltaTime,
          QSize const& size, inten_t const* intens);

  Datasets const& datasets() const;

  str attributeStrValue(uint) const;

  qreal midTth()              const;
  qreal deltaMonitorCount()   const;
  qreal deltaTime()           const;

  qreal omg() const { return _motorOmg; }
  qreal phi() const { return _motorPhi; }
  qreal chi() const { return _motorChi; }

  QSize   imageSize()          const;
  inten_t inten(uint i,uint j) const;

  /// used for correction files if there is more than one image
  void addIntens(rcDataset);

private:
  Datasets *_datasets;  // here it belongs
  str _date, _comment;

  // all stored angles in degrees
  qreal _motorXT,  _motorYT,  _motorZT,  _motorOmg, _motorTth,
        _motorPhi, _motorChi, _motorPST, _motorSST, _motorOMGM;

  qreal _deltaMonitorCount, _deltaTime;

  Image _image;
};

/// A groupd of Dataset(s)
class Datasets: public QVector<shp_Dataset> {
  SUPER(Datasets,QVector<shp_Dataset>)
public:
  Datasets();

  void append(shp_Dataset);
  void fold(); ///< collapse datasets into one (for correction files)

  // in all datasets
  QSize imageSize()       const;
//  rcRange rgeInten() const;
  qreal avgDeltaMonitorCount() const;
  qreal avgDeltaTime()    const;

private:
  void invalidateMutables();
  // computed on demand
  mutable Range _rgeInten; //>>>
  mutable qreal _avgMonitorCount, _avgDeltaTime;
};

//------------------------------------------------------------------------------
}

Q_DECLARE_METATYPE(core::shp_Dataset)

#endif // CORE_DATASET_H
