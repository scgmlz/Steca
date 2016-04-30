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
#define DEFINE_HUB_SIGNAL_HANDLER(name,signal)        \
  template <typename Lambda>                          \
  void name(Lambda slot) {                            \
    onHubSignal(&TheHubSignallingBase::signal, slot); \
  }

  DEFINE_HUB_SIGNAL_HANDLER(ON_FILES_CHANGED,     sigFilesChanged)
  DEFINE_HUB_SIGNAL_HANDLER(ON_FILES_SELECTED,    sigFilesSelected)

  DEFINE_HUB_SIGNAL_HANDLER(ON_DATASETS_CHANGED,  sigDatasetsChanged)
  DEFINE_HUB_SIGNAL_HANDLER(ON_DATASET_SELECTED,  sigDatasetSelected)

  DEFINE_HUB_SIGNAL_HANDLER(ON_CORR_ENABLED,      sigCorrEnabled)
  DEFINE_HUB_SIGNAL_HANDLER(ON_CORR_FILENAME,     sigCorrFileName)

  DEFINE_HUB_SIGNAL_HANDLER(ON_FACTORY_SETTINGS,  sigFactorySettings)

  DEFINE_HUB_SIGNAL_HANDLER(ON_REFL_CHANGED,      sigReflectionsChanged)
  DEFINE_HUB_SIGNAL_HANDLER(ON_REFL_SELECTED,     sigReflectionSelected)
  DEFINE_HUB_SIGNAL_HANDLER(ON_REFL_DATA,         sigReflectionData)
  DEFINE_HUB_SIGNAL_HANDLER(ON_REFL_VALUES,       sigReflectionValues)

  DEFINE_HUB_SIGNAL_HANDLER(ON_DISPLAY_CHANGED,   sigDisplayChanged)
  DEFINE_HUB_SIGNAL_HANDLER(ON_GEOMETRY_CHANGED,  sigGeometryChanged)

  DEFINE_HUB_SIGNAL_HANDLER(ON_NORM_CHANGED,      sigNormChanged)
  DEFINE_HUB_SIGNAL_HANDLER(ON_BG_POLY_DEGREE,    sigBgPolynomialDegree)

  DEFINE_HUB_SIGNAL_HANDLER(ON_FITTING_TAB,       sigFittingTab)

#undef DEFINE_HUB_SIGNAL_HANDLER
};

//------------------------------------------------------------------------------
}
#endif // REFHUB_H
