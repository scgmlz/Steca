#ifndef SESSION_H
#define SESSION_H

#include "defs.h"
#include "core.h"
#include <QAbstractListModel>

// A proxy for access to core::Session
// Models and signals also here

class Session: public QObject {
  Q_OBJECT
  core::Session *coreSession;
public:
  Session();
 ~Session();

  void addFile(rcstr filePath);
  void addFiles(str_lst filePaths);
  bool hasFile(rcstr filePath);
  void remFile(uint i);
  uint numFiles(bool withCorr=false);
  core::File const& getFile(uint i);
  str  fileName(uint i);
  bool hasCorrFile();
  void setCorrFile(rcstr filePath);
  str  corrFileName();

  void emitSelectedFile(pcCoreFile);

signals:
  void filesChanged();
  void selectedFile(pcCoreFile);

public:
  class FileListModel: public QAbstractListModel {
    SUPER(FileListModel,QAbstractListModel)
  public:
    FileListModel(Session&);

    enum { IsCorrectionFileRole = Qt::UserRole, GetFileRole };

    int rowCount(QModelIndex const&)      const;
    QVariant data(QModelIndex const&,int) const;

    Session &session;
  };

  FileListModel fileListModel;
};

#endif
