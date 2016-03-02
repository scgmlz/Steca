/** \file
 */

#ifndef SESSION_H
#define SESSION_H

#include "core_defs.h"
#include "core_session.h"

//------------------------------------------------------------------------------
/// As core::Session, with models and signals

class Session: public QObject, public core::Session {
  SUPER(Session,core::Session) Q_OBJECT

//private:
//  MainWin &mainWin;

public:
  void load(QFileInfo const&)       THROWS;
  void load(QByteArray const& json) THROWS;
  QByteArray save() const;

  void addFile(rcstr filePath)      THROWS;
  void addFiles(str_lst filePaths)  THROWS;


  void loadCorrFile(rcstr filePath);
  void enableCorrection(bool);


  void setImageCut(bool topLeft, bool linked, ImageCut const&);
  void setGeometry(qreal sampleDetectorSpan, qreal pixSpan, bool hasBeamOffset, QPoint const& middlePixOffset);

  void setImageMirror(bool);
  void setImageRotate(ImageTransform);
  void nextImageRotate();

  void setBackgroundPolynomialDegree(uint);

private:
public:
  void setGlobalNorm(bool);
  bool isGlobalNorm() const { return globalNorm; }

// Session emits most signals
signals:
  void geometryChanged();             // detector geometry, image cut, image rotation

  void correctionEnabled(bool);
  void displayChange();               // TODO this signal

  void fileSelected(core::shp_File);  // may be null
  void filesChanged();                // emited after corrFileSet, fileAdded, fileRemoved

  void datasetSelected(core::shp_Dataset);  // may be null

  void saveSettings();  // TODO for now; eventually move things to save to Session
  void readSettings();  // TODO for now; eventually move things to read to Session

  void backgroundPolynomialDegree(uint);

public:

  void doSaveSettings(); // TODO temp.
  void doReadSettings(); // TODO temp.
};

//------------------------------------------------------------------------------
#endif
