/** \files
 * Data models.
 */

#ifndef MODELS_H
#define MODELS_H

#include "defs.h"
#include "panel/panel.h"  // TODO remove
#include "core_file.h"
#include <QAbstractListModel>
#include <QAbstractTableModel>

class Session;

namespace model {

class SessionModel {
public:
  SessionModel(Session&);

  uint numFiles(bool withCorr=false);
  void remFile(uint i);

  void setSelectedFile(core::shp_File);
  void setSelectedDataset(core::shp_Dataset);

  Session const& getSession() const { return session ; }

protected:
  Session &session;
};

class FileViewModel: public QAbstractListModel, public SessionModel {
  SUPER(FileViewModel,QAbstractListModel)
public:
  FileViewModel(Session&);

  enum { GetFileRole = Qt::UserRole, IsCorrectionFileRole };

  int rowCount(QModelIndex const&)      const;
  QVariant data(QModelIndex const&,int) const;
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

}

#endif
