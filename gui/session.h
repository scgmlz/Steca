/** \file
 */

#ifndef SESSION_H
#define SESSION_H

#include "defs.h"
#include "core_session.h"
#include "models.h"

/// As core::Session, with models and signals
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
  void setImageRotate(core::ImageTransform);
  void nextImageRotate();

signals:
  void sessionLoaded();
  void filesChanged();
  void fileSelected(core::shp_File);
  void datasetSelected(core::shp_Dataset);
  void imageCutChanged();

public:
  model::FileViewModel    fileViewModel;
  model::DatasetViewModel datasetViewModel;
};

#endif
