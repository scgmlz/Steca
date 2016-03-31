/** \files
 * Data models.
 */

#ifndef MODELS_H
#define MODELS_H

#include "core_defs.h"
#include "core_file.h"
#include "core_reflection.h"
#include <QAbstractListModel>
#include <QAbstractTableModel>

class TheHub;
class QCheckBox;
class QLineEdit;

namespace model {
//------------------------------------------------------------------------------

struct InfoItem {
  str tag; QCheckBox *cb; QLineEdit *text;
};

typedef QVector<InfoItem> infoitem_vec;

//------------------------------------------------------------------------------

class ModelBase {
public:
  ModelBase(TheHub&);
protected:
  TheHub &theHub;
};

class FileViewModel: public QAbstractTableModel, public ModelBase {
  SUPER(FileViewModel,QAbstractTableModel)
public:
  FileViewModel(TheHub&);

  enum { GetFileRole = Qt::UserRole, IsCorrectionFileRole };

  uint numFiles(bool withCorr=false);
  void remFile(uint i);

  int columnCount(QModelIndex const& = QModelIndex())   const;
  int rowCount(QModelIndex const& = QModelIndex())      const;
  QVariant data(QModelIndex const&,int)                 const;

  void signalReset();
};

class DatasetViewModel: public QAbstractTableModel, public ModelBase {
  SUPER(DatasetViewModel,QAbstractTableModel)
public:
  DatasetViewModel(TheHub&);

  enum { GetDatasetRole = Qt::UserRole };

  int columnCount(QModelIndex const& = QModelIndex())   const;
  int rowCount(QModelIndex const& = QModelIndex())      const;
  QVariant data(QModelIndex const&,int)                 const;
  QVariant headerData(int,Qt::Orientation,int)          const;

  void setCoreFile(core::shp_File);
  void setInfoItems(infoitem_vec const*);

private:
  core::shp_Dataset const& getDataset(int row) const;

private:
  core::shp_File coreFile;
  infoitem_vec const* infoItems;
  QVector<int> attributeNums;
};

class ReflectionViewModel: public QAbstractTableModel, public ModelBase {
  SUPER(ReflectionViewModel,QAbstractTableModel)
public:
  enum {
    COLUMN_0,
    COLUMN_ID,
    COLUMN_TYPE,
    NUM_COLUMNS
  };

  ReflectionViewModel(TheHub&);

  enum { GetDatasetRole = Qt::UserRole };

  int columnCount(QModelIndex const& = QModelIndex()) const;
  int rowCount(QModelIndex const& = QModelIndex())    const;
  QVariant data(QModelIndex const&,int)               const;
  QVariant headerData(int,Qt::Orientation,int)        const;

  void addReflection(core::Reflection::eType);
  void remReflection(uint);

  void signalReset();
};

//------------------------------------------------------------------------------
}
#endif
