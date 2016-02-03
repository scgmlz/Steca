#ifndef SESSION_H
#define SESSION_H

#include "defs.h"
#include "core_session.h"
#include <QAbstractListModel>
#include <QAbstractTableModel>

#include "panels/datasetinfo.h" // TODO this is quite brutal in order to get InfoItems; refactor?

// As core::Session, with models and signals

class Session: public QObject, public core::Session {
  SUPER(Session,core::Session) Q_OBJECT
public:
  Session();
 ~Session();

  void load(QFileInfo const&)       THROWS;
  void load(QByteArray const& json) THROWS;
  QByteArray save() const;

  void addFile(rcstr filePath)      THROWS;
  void addFiles(str_lst filePaths)  THROWS;

  void remFile(uint i);

  void loadCorrFile(rcstr filePath);

  void setSelectedFile(core::shp_File);
  void setSelectedDataset(core::shp_Dataset);

  void setImageCut(bool topLeft, bool linked, core::ImageCut const&);

  QAction *actImageRotate, *actImageMirror;

  void setImageMirror(bool);
  void setImageRotate(core::Image::Transform);
  void nextImageRotate();

private:
  core::shp_File    selectedFile;
  core::shp_Dataset selectedDataset;

signals:
  void sessionLoaded();
  void filesChanged();
  void fileSelected(core::shp_File);
  void datasetSelected(core::shp_Dataset);
  void imageCutChanged();

public:
  class FileViewModel: public QAbstractListModel {
    SUPER(FileViewModel,QAbstractListModel)
  public:
    FileViewModel(Session&);

    enum { GetFileRole = Qt::UserRole, IsCorrectionFileRole };

    int rowCount(QModelIndex const&)      const;
    QVariant data(QModelIndex const&,int) const;

    Session &session;
  };

  FileViewModel fileViewModel;

public:
  class DatasetViewModel: public QAbstractTableModel {
    SUPER(DatasetViewModel,QAbstractTableModel)
  public:
    DatasetViewModel();

    enum { GetDatasetRole = Qt::UserRole };

    int columnCount(QModelIndex const&)   const;
    int rowCount(QModelIndex const&)      const;
    QVariant data(QModelIndex const&,int) const;
    QVariant headerData(int,Qt::Orientation,int) const;

    void setCoreFile(core::shp_File);
    void setInfoItems(panel::DatasetInfo::InfoItems const*);

  private:
    core::shp_Dataset const& getDataset(int row) const;

  private:
    core::shp_File coreFile;
    panel::DatasetInfo::InfoItems const* infoItems; // TODO make better
    QVector<int> attributeNums;
  };

  DatasetViewModel datasetViewModel;
};

#endif
