// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      core/typ/typ_image.h
//! @brief     Defines ...
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef TYP_IMAGE_H
#define TYP_IMAGE_H

#include "typ/typ_array2d.h"
#include "typ/typ_types.h"
#include <QSharedPointer>

namespace typ {

typedef Array2D<inten_t> inten_arr;

class Image final {
  CLASS(Image)
public:
  // Image as vector of intensities, filled with 0 or given intensities.
  Image(size2d::rc = size2d(0, 0));
  Image(inten_arr::rc);

  size2d::rc size() const { return intens_.size(); }

  void clear() { intens_.clear(); }

  bool isEmpty() const { return intens_.isEmpty(); }

  void fill(inten_t val, size2d::rc size) { intens_.fill(val, size); }

  inten_t inten(uint i) const { return intens_.at(i); }

  inten_t inten(uint i, uint j) const { return intens_.at(i, j); }

  void setInten(uint i, inten_t val) { intens_.setAt(i, val); }

  void setInten(uint i, uint j, inten_t val) { intens_.setAt(i, j, val); }

  void addInten(uint i, uint j, inten_t val) { intens_.refAt(i, j) += val; }

  // Sum all intensities with new ones.
  void addIntens(rc) THROWS;

  inten_rge::rc rgeInten() const { return rgeInten_; }

private:
  inten_arr intens_;
  inten_rge rgeInten_;
};

typedef QSharedPointer<Image> shp_Image;
}
#endif  // TYP_IMAGE_H
