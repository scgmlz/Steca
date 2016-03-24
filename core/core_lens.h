#ifndef CORE_LENS_H
#define CORE_LENS_H

#include "core_types.h"
#include "core_priority_chain.h"
#include "core_angle_map_array.h"

namespace core {
//------------------------------------------------------------------------------

class Lens : public ChainLink<Lens> {
  SUPER(Lens, ChainLink<Lens>)
public:
  enum {
    PRIORITY_NORMALIZATION,
    PRIORITY_INTENSITY_RANGE,
    PRIORITY_ROI,
    PRIORITY_TRANSFORMATION,
    PRIORITY_SENSITIVITY_CORRECTION,
    PRIORITY_PLAIN
  };

public:
  virtual DiffractionAngles getAngles(uint x, uint y) const = 0;
  virtual intens_t getIntensity(uint x, uint y) const = 0;
  virtual Range getIntensityRange() const = 0;
  virtual QSize getSize() const = 0;

protected:
  void nextChangedImpl() override;
};

//------------------------------------------------------------------------------

class Image;

class PlainLens final : public Lens {
  SUPER(PlainLens, Lens)
public:
  PlainLens(Image const& image, AngleMapArray const& angleMapArray);

  uint getPriority() const override;

  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  Range getIntensityRange() const override;
  QSize getSize() const override;

private:
  AngleMapArray const* angleMap;
  Range const* intensityRange;
  Image const* rawImage;
};

//------------------------------------------------------------------------------

class ImageTransform;

class TransformationLens final : public Lens {
  SUPER(TransformationLens, Lens)
public:
  TransformationLens(ImageTransform const& transformation);

  uint getPriority() const override;

  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  Range getIntensityRange() const override;
  QSize getSize() const override;

private:
  ImageTransform const* transform;
};

//------------------------------------------------------------------------------

class ImageCut;

class ROILens final : public Lens {
  SUPER(ROILens, Lens)
public:
  ROILens(ImageCut const& imageCut);

  uint getPriority() const override;

  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  Range getIntensityRange() const override;
  QSize getSize() const override;

private:
  ImageCut const* cut;
};

//------------------------------------------------------------------------------

class SensitivityCorrectionLens final : public Lens {
  SUPER(SensitivityCorrectionLens, Lens)
public:
  SensitivityCorrectionLens(Array2D<qreal> const& sensitivityCorrection);

  uint getPriority() const override;

  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  Range getIntensityRange() const override;
  QSize getSize() const override;

private:
  Array2D<qreal> const* correction;
};

//------------------------------------------------------------------------------

class IntensityRangeLens final : public Lens {
  SUPER(IntensityRangeLens, Lens)
public:
  IntensityRangeLens();

  uint getPriority() const override;

  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  Range getIntensityRange() const override;
  QSize getSize() const override;

protected:
  void nextChangedImpl() override;

private:
  Range range;

  void updateRange();
};

//------------------------------------------------------------------------------

class GlobalIntensityRangeLens final : public Lens {
  SUPER(GlobalIntensityRangeLens, Lens)
public:
  GlobalIntensityRangeLens(Range const& intensityRange);

  uint getPriority() const override;

  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  Range getIntensityRange() const override;
  QSize getSize() const override;

private:
  Range const* range;
};
//------------------------------------------------------------------------------
class NormalizationLens final : public Lens {
  SUPER(NormalizationLens, Lens)
public:
  NormalizationLens(qreal normVal_);
  
  uint getPriority() const override;
  
  DiffractionAngles getAngles(uint x, uint y) const override;
  intens_t getIntensity(uint x, uint y) const override;
  Range getIntensityRange() const override;
  QSize getSize() const override;
  
private:
  qreal normVal;
};

//------------------------------------------------------------------------------

class Dataset;

shp_LensSystem makeLensSystem(Dataset const& dataset,
                              AngleMapArray const& angleMap);

//------------------------------------------------------------------------------
}

#endif
