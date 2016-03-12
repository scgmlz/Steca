#ifndef CORE_LENS_H
#define CORE_LENS_H

#include "core_types.h"
#include "core_image_cut.h"
#include "core_image_transform.h"
#include "core_priority_chain.h"

namespace core {
//------------------------------------------------------------------------------

class Lens;

using shp_LensSystem = QSharedPointer<Lens>;

//------------------------------------------------------------------------------

class Lens : public ChainLink<Lens> {
public:
  virtual intens_t getIntensity(uint x, uint y) const = 0;
  virtual QSize getSize() const = 0;

protected:
  void nextChangedImpl() override;
};

//------------------------------------------------------------------------------

class Image;

class PlainLens final : public Lens {
public:
  const static uint PRIORITY = 2;

  PlainLens(Image const& image);

  uint getPriority() const override;

  intens_t getIntensity(uint x, uint y) const override;
  QSize getSize() const override;

private:
  Image const* rawImage;
};

//------------------------------------------------------------------------------

class TransformationLens final : public Lens {
public:
  const static uint PRIORITY = 1;

  TransformationLens(ImageTransform const& transformation);

  uint getPriority() const override;

  intens_t getIntensity(uint x, uint y) const override;
  QSize getSize() const override;

private:
  ImageTransform transform;
};

//------------------------------------------------------------------------------

class ROILens final : public Lens {
public:
  const static uint PRIORITY = 0;

  ROILens(ImageCut const& imageCut);

  uint getPriority() const override;

  intens_t getIntensity(uint x, uint y) const override;
  QSize getSize() const override;

private:
  ImageCut cut;
};

//------------------------------------------------------------------------------

shp_LensSystem makeLensSystem(Image const& image);

//------------------------------------------------------------------------------
}
#endif