// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      refhub.h
//! @brief     Reference to The Hub.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef REFHUB_H
#define REFHUB_H

#include "core_defs.h"
#include "types/core_types_fwd.h"

namespace gui {
//------------------------------------------------------------------------------
/// The base class of the hub that signalls.

class TheHub;

class TheHubSignallingBase: public QObject {
  SUPER(TheHubSignallingBase,QObject)
  Q_OBJECT
  friend class RefHub;
private:
  TheHub& asHub();

protected:
  // emit signals
  void tellFilesSelectedDatasetsChanged();
  void tellSelectedDataset(core::shp_Dataset);
  void tellSelectedReflection(core::shp_Reflection);
  void tellReflectionData(core::shp_Reflection);
  void tellReflectionValues(core::rcRange,core::rcXY,qreal,bool);

signals:
  void sigFilesChanged();
  void sigFilesSelected();

  void sigDatasetsChanged();
  void sigDatasetSelected(core::shp_Dataset);

  void sigCorrEnabled(bool);
  void sigCorrFileName(QString const&);

  void sigFactorySettings();

  void sigReflectionsChanged();
  void sigReflectionSelected(core::shp_Reflection);
  void sigReflectionData(core::shp_Reflection);
  void sigReflectionValues(core::Range const&, core::XY const&, qreal, bool);

  void sigDisplayChanged();
  void sigGeometryChanged();

  void sigNormChanged();
  void sigBgPolynomialDegree(uint);

  void sigFittingTab(int);
};

//------------------------------------------------------------------------------
/// The base class that refers to the hub + support methods.

class RefHub {
public:
  RefHub(TheHub&);
protected:
  TheHub &hub_;

private:
  template <typename Signal, typename Lambda>
  void onHubSignal(Signal signal, Lambda slot) {
    QObject::connect(&hub_,signal,slot);
  }

// send signals (through the hub)
protected:
  void tellFilesSelectedDatasetsChanged();
  void tellSelectedDataset(core::shp_Dataset);
  void tellSelectedReflection(core::shp_Reflection);
  void tellReflectionData(core::shp_Reflection);
  void tellReflectionValues(core::rcRange,core::rcXY,qreal,bool);

// handle same signals
protected:
#define DEFINE_HUB_SIGNAL_HANDLER(name)                   \
  template <typename Lambda>                              \
  void onSig##name(Lambda slot) {                         \
    onHubSignal(&TheHubSignallingBase::sig##name, slot);  \
  }

  DEFINE_HUB_SIGNAL_HANDLER(FilesChanged)
  DEFINE_HUB_SIGNAL_HANDLER(FilesSelected)

  DEFINE_HUB_SIGNAL_HANDLER(DatasetsChanged)
  DEFINE_HUB_SIGNAL_HANDLER(DatasetSelected)

  DEFINE_HUB_SIGNAL_HANDLER(CorrEnabled)
  DEFINE_HUB_SIGNAL_HANDLER(CorrFileName)

  DEFINE_HUB_SIGNAL_HANDLER(FactorySettings)

  DEFINE_HUB_SIGNAL_HANDLER(ReflectionsChanged)
  DEFINE_HUB_SIGNAL_HANDLER(ReflectionSelected)
  DEFINE_HUB_SIGNAL_HANDLER(ReflectionData)
  DEFINE_HUB_SIGNAL_HANDLER(ReflectionValues)

  DEFINE_HUB_SIGNAL_HANDLER(DisplayChanged)
  DEFINE_HUB_SIGNAL_HANDLER(GeometryChanged)

  DEFINE_HUB_SIGNAL_HANDLER(NormChanged)
  DEFINE_HUB_SIGNAL_HANDLER(BgPolynomialDegree)

  DEFINE_HUB_SIGNAL_HANDLER(FittingTab)

#undef DEFINE_HUB_SIGNAL_HANDLER
};

//------------------------------------------------------------------------------
}
#endif // REFHUB_H
