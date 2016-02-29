/** \files
 * Data models.
 */

#ifndef MODELS_H
#define MODELS_H

#include "core_defs.h"
#include "panel/panel.h"  // TODO remove
#include "core_file.h"
#include <QAbstractListModel>
#include <QAbstractTableModel>

class Session;

namespace model {
//------------------------------------------------------------------------------

class SessionModel {
public:
  SessionModel(Session&);

  Session const& getSession() const { return session ; }
  void setSelectedDataset(core::shp_Dataset);

protected:
  Session &session;
};

class FileViewModel: public QAbstractListModel, public SessionModel {
  SUPER(FileViewModel,QAbstractListModel)
public:
  FileViewModel(Session&);

  uint numFiles(bool withCorr=false);
  void remFile(uint i);
  void setSelectedFile(core::shp_File);

  enum { GetFileRole = Qt::UserRole, IsCorrectionFileRole };

  int rowCount(QModelIndex const&)      const;
  QVariant data(QModelIndex const&,int) const;

  void signalReset();
};

class DatasetViewModel: public QAbstractTableModel, public SessionModel {
  SUPER(DatasetViewModel,QAbstractTableModel)
public:
  DatasetViewModel(Session&);

  enum { GetDatasetRole = Qt::UserRole };

  int columnCount(QModelIndex const&)   const;
  int rowCount(QModelIndex const&)      const;
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

class ReflectionViewModel: public QAbstractTableModel, public SessionModel {
  SUPER(ReflectionViewModel,QAbstractTableModel)
public:
  ReflectionViewModel(Session&);

  int columnCount(QModelIndex const&)   const;
  int rowCount(QModelIndex const&)      const;
  QVariant data(QModelIndex const&,int) const;
  QVariant headerData(int,Qt::Orientation,int) const;

  void addReflection();

private:
  QVector<int> reflections;
};

//------------------------------------------------------------------------------
}
#endif
