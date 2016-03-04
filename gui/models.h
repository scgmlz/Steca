/** \files
 * Data models.
 */

#ifndef MODELS_H
#define MODELS_H

#include "core_defs.h"
#include "panel/panel.h"  // TODO remove ?
#include "core_file.h"
#include <QAbstractListModel>
#include <QAbstractTableModel>

class TheHub;

namespace model {
//------------------------------------------------------------------------------

class ModelBase {
public:
  ModelBase(TheHub&);
protected:
  TheHub &theHub;
};

class FileViewModel: public QAbstractListModel, public ModelBase {
  SUPER(FileViewModel,QAbstractListModel)
public:
  FileViewModel(TheHub&);

  uint numFiles(bool withCorr=false);
  void remFile(uint i);
  void setSelectedFile(core::shp_File);

  enum { GetFileRole = Qt::UserRole, IsCorrectionFileRole };

  int rowCount(QModelIndex const& = QModelIndex())      const;
  QVariant data(QModelIndex const&,int) const;

  void signalReset();
};

class DatasetViewModel: public QAbstractTableModel, public ModelBase {
  SUPER(DatasetViewModel,QAbstractTableModel)
public:
  DatasetViewModel(TheHub&);

  enum { GetDatasetRole = Qt::UserRole };

  int columnCount(QModelIndex const& = QModelIndex())   const;
  int rowCount(QModelIndex const& = QModelIndex())      const;
  QVariant data(QModelIndex const&,int) const;
  QVariant headerData(int,Qt::Orientation,int) const;

  void setCoreFile(core::shp_File);
  void setInfoItems(panel::InfoItems const*);

private:
  core::shp_Dataset const& getDataset(int row) const;

private:
  core::shp_File coreFile;
  panel::InfoItems const* infoItems; // TODO make better; remove #include panel.h then
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

  int columnCount(QModelIndex const& = QModelIndex()) const;
  int rowCount(QModelIndex const& = QModelIndex())    const;
  Qt::ItemFlags flags(QModelIndex const&)             const;
  QVariant headerData(int,Qt::Orientation,int)        const;

  QVariant data(QModelIndex const&,int) const;
  bool setData(QModelIndex const&, QVariant const&, int role = Qt::EditRole);

  void addReflection();
  void remReflection(uint);

  void signalReset();

private:
  QVector<int> reflections;
};

//------------------------------------------------------------------------------
}
#endif
