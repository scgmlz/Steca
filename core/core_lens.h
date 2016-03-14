#ifndef CORE_LENS_H
#define CORE_LENS_H

#include "core_types.h"
#include "core_priority_chain.h"
#include "core_angle_map_array.h"

namespace core {
//------------------------------------------------------------------------------

class Lens : public ChainLink<Lens> {
public:
  virtual DiffractionAngles getAngles(uint x, uint y) const = 0;
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

  PlainLens(Image const& image, AngleMapArray const& angleMapArray);

  uint getPriority() const override;

  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  QSize getSize() const override;

private:
  AngleMapArray const* angleMap;
  Image const* rawImage;
};

//------------------------------------------------------------------------------

class ImageTransform;

class TransformationLens final : public Lens {
public:
  const static uint PRIORITY = 1;

  TransformationLens(ImageTransform const& transformation);

  uint getPriority() const override;

  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  QSize getSize() const override;

private:
  ImageTransform const* transform;
};

//------------------------------------------------------------------------------

class ImageCut;

class ROILens final : public Lens {
public:
  const static uint PRIORITY = 0;

  ROILens(ImageCut const& imageCut);

  uint getPriority() const override;

  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  QSize getSize() const override;

private:
  ImageCut const* cut;
};

//------------------------------------------------------------------------------

class Dataset;

shp_LensSystem makeLensSystem(Dataset const& dataset,
                              AngleMapArray const& angleMap);

//------------------------------------------------------------------------------
}
#endif