// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2
//
//! @file      models.h
//! @brief     Data models.
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#ifndef MODELS_H
#define MODELS_H

#include "types/type_models.h"

namespace models {
//------------------------------------------------------------------------------

class FilesViewModel: public TableModel {
  SUPER(FilesViewModel,TableModel)
public:
  FilesViewModel(gui::TheHub&);

  int columnCount(rcIndex = ANY_INDEX) const;
  int rowCount(rcIndex    = ANY_INDEX) const;

  QVariant data(rcIndex,int) const;

public:
  enum { GetFileRole = Qt::UserRole };

  void remFile(uint i);
};

//------------------------------------------------------------------------------

class DatasetViewModel: public TableModel {
  SUPER(DatasetViewModel,TableModel)
public:
  DatasetViewModel(gui::TheHub&);

  int columnCount(rcIndex = ANY_INDEX) const;
  int rowCount(rcIndex    = ANY_INDEX) const;

  QVariant data(rcIndex,int) const;
  QVariant headerData(int,Qt::Orientation,int) const;

  enum {
    COL_NUMBER = DCOL,
    COL_ATTRS
  };

public:
  enum { GetDatasetRole = Qt::UserRole };

  void showMetaInfo(checkedinfo_vec const&);

private:
  core::rcDatasets datasets_;        ///< the selected datasets
  checkedinfo_vec const* metaInfo_;  ///< metadata items
  uint_vec metaInfoNums_;            ///< selected metadata items to show
};

//------------------------------------------------------------------------------

class ReflectionViewModel: public TableModel {
  SUPER(ReflectionViewModel,TableModel)
public:
  ReflectionViewModel(gui::TheHub&);

  int columnCount(rcIndex = ANY_INDEX) const;
  int rowCount(rcIndex    = ANY_INDEX) const;

  QVariant data(rcIndex,int) const;
  QVariant headerData(int,Qt::Orientation,int) const;

  enum {
    COL_ID = DCOL,
    COL_TYPE,
    NUM_COLUMNS
  };

public:
  enum { GetDatasetRole = Qt::UserRole };

  void addReflection(core::ePeakType);
  void remReflection(uint);
};

//------------------------------------------------------------------------------
}
#endif // MODELS_H
