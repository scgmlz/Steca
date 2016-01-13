#ifndef SESSION_H
#define SESSION_H

#include "defs.h"
#include "core.h"
#include <QAbstractListModel>

// A proxy for access to core::Session
// Models and signals also here

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

  void setImageCut(bool topLeft, imagecut_t const&);
  void setImageCut(bool topLeft, int top, int bottom, int left, int right);

private:
  pcCoreFile    selectedFile;
  pcCoreDataset selectedDataset;

  imagecut_t    imageCut;

signals:
  void filesChanged();
  void fileSelected(pcCoreFile);
  void datasetSelected(pcCoreDataset);
  void imageCutChanged();

public:
  class FileListModel: public QAbstractListModel {
    SUPER(FileListModel,QAbstractListModel)
  public:
    FileListModel(Session&);

    enum { GetFileRole = Qt::UserRole, IsCorrectionFileRole };

    int rowCount(QModelIndex const&)      const;
    QVariant data(QModelIndex const&,int) const;

    Session &session;
  };

  FileListModel fileListModel;

public:
  class DatasetListModel: public QAbstractListModel {
    SUPER(DatasetListModel,QAbstractListModel)
  public:
    DatasetListModel(Session&);

    enum { GetFileRole = Qt::UserRole };

    int rowCount(QModelIndex const&)      const;
    QVariant data(QModelIndex const&,int) const;

    Session &session;
  };

  DatasetListModel datasetListModel;
};

#endif
