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

class FileViewModel: public TableModel {
  SUPER(FileViewModel,TableModel)
public:
  FileViewModel(TheHub&);

  int columnCount(rcIndex = ANY_INDEX) const;
  int rowCount(rcIndex    = ANY_INDEX) const;

  QVariant data(rcIndex,int) const;

public:
  enum { GetFileRole = Qt::UserRole, IsCorrFileRole };

  void remFile(uint i);
};

//------------------------------------------------------------------------------

class DatasetViewModel: public TableModel {
  SUPER(DatasetViewModel,TableModel)
public:
  DatasetViewModel(TheHub&);

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

  void setFile(core::shp_File);
  void showMetaInfo(checkedinfo_vec const&);

private:
  core::shp_File file;              ///< the file with datasets
  checkedinfo_vec const* metaInfo;  ///< metadata items
  uint_vec metaInfoNums;            ///< selected metadata items to show
};

//------------------------------------------------------------------------------

class ReflectionViewModel: public TableModel {
  SUPER(ReflectionViewModel,TableModel)
public:
  ReflectionViewModel(TheHub&);

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
