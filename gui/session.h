#ifndef SESSION_H
#define SESSION_H

#include "defs.h"
#include "core.h"
#include <QAbstractListModel>
#include <QAbstractTableModel>

#include "panels/datasetinfo.h" // TODO this is quite brutal in order to get InfoItems; refactor?

// A proxy for access to core::Session
// Models and signals are also here

class Session: public QObject {
  Q_OBJECT
  QSharedPointer<core::Session> coreSession;
public:
  Session();
 ~Session();

  void load(QFileInfo const&)       THROWS;
  void load(QByteArray const& json) THROWS;
  QByteArray save() const;

  void addFile(rcstr filePath)      THROWS;
  void addFiles(str_lst filePaths)  THROWS;
  void remFile(uint i);
  uint numFiles(bool withCorr=false);
  core::File const& getFile(uint i);
  str  fileName(uint i);
  bool hasCorrFile();
  void setCorrFile(rcstr filePath);

  void setSelectedFile(pcCoreFile);
  void setSelectedDataset(pcCoreDataset);

  core::Dataset const& getDataset(uint i);

  struct imagecut_t {
    imagecut_t(int top = 0, int bottom = 0, int left = 0, int right = 0);
    int top, bottom, left, right;
  };

  imagecut_t const& getImageCut() const;

  void setImageCut(bool topLeft, bool linked, imagecut_t const&);
  void setImageCut(bool topLeft, bool linked, int top, int bottom, int left, int right);

  struct detector_t {
    detector_t();
    qreal   distance, pixelSize;
    bool    isBeamOffset;
    QPointF beamOffset;
  };

private:
  pcCoreFile    selectedFile;
  pcCoreDataset selectedDataset;

  imagecut_t    imageCut;

public:
  detector_t    detector;

signals:
  void sessionLoaded();
  void filesChanged();
  void fileSelected(pcCoreFile);
  void datasetSelected(pcCoreDataset);
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

    void setCoreFile(pcCoreFile);
    void setInfoItems(panel::DatasetInfo::InfoItems const*);

  private:
    core::Dataset& getDataset(int row)    const;

  private:
    pcCoreFile coreFile;
    panel::DatasetInfo::InfoItems const* infoItems; // TODO make better
    QVector<int> attributeNums;
  };

  DatasetViewModel datasetViewModel;
};

#endif
