#include "thehub.h"
#include "core_session.h"

//------------------------------------------------------------------------------

Action::Action(rcstr text, rcstr tip, rcstr iconFile, QObject* parent): super(text,parent) {
  setToolTip(tip);

  if (!iconFile.isEmpty())
    setIcon(QIcon(iconFile));
}

//------------------------------------------------------------------------------

PushAction::PushAction(rcstr text, rcstr tip, rcstr iconFile, QObject* parent)
: super(text,tip,iconFile,parent) {
}

//------------------------------------------------------------------------------

ToggleAction::ToggleAction(rcstr text, rcstr tip, rcstr iconFile, QObject* parent)
: super(text,tip,iconFile,parent) {
  setCheckable(true);
}

//------------------------------------------------------------------------------

TheHub::TheHub(): session(new core::Session())
, globalNorm(false)
, fileViewModel(*this), datasetViewModel(*this), reflectionViewModel(*this) {
}

TheHub::~TheHub() {
  delete session;
}

#define PUSH_ACTION(act,text,tip,icon) \
  lastAction = act = new PushAction(text,tip,icon,this);
#define TOGL_ACTION(act,text,tip,icon) \
  lastAction = act  = new PushAction(text,tip,icon,this);
#define ACTION_KEY(key) \
  lastAction->setShortcut(key);

void TheHub::initActions() {
  typedef QKeySequence QKey; QAction *lastAction;

  PUSH_ACTION(actAddFiles,    "Add files...","Add files",":/icon/add")                        ACTION_KEY(Qt::CTRL|Qt::Key_O)
  PUSH_ACTION(actRemoveFile,  "Remove selected file","Remove selected file",":/icon/rem")     ACTION_KEY(QKey::Delete);
  actRemoveFile->setEnabled(false);

  PUSH_ACTION(actLoadCorrectionFile,"Load correction file...", "Load correction file", "")    ACTION_KEY(Qt::SHIFT|Qt::CTRL|Qt::Key_O);
  TOGL_ACTION(actImagesEnableCorr,  "Enable correction file",  "Enable correction by correction file", ":/icon/useCorrection")
  actImagesEnableCorr->setEnabled(false);
  PUSH_ACTION(actLoadSession,"Load session...","","")
  PUSH_ACTION(actSaveSession,"Save session...","","")

  PUSH_ACTION(actExportDiffractogramCurrent,"Current only...","","")
  PUSH_ACTION(actExportDiffractogramAllSeparateFiles,"All to separate files...","","")
  PUSH_ACTION(actExportDiffractogramSingleFile,"All to a single file...","","")

  PUSH_ACTION(actExportImagesWithMargins,"With margins...","","")
  PUSH_ACTION(actExportImagesWithoutMargins,"Without margins...","","")

  PUSH_ACTION(actQuit ,"Quit", "",  "") ACTION_KEY(QKey::Quit)

  PUSH_ACTION(actUndo ,"Undo",  "", "") ACTION_KEY(QKey::Undo)
  PUSH_ACTION(actRedo ,"Redo",  "", "") ACTION_KEY(QKey::Redo)
  PUSH_ACTION(actCut  ,"Cut",   "", "") ACTION_KEY(QKey::Cut)
  PUSH_ACTION(actCopy ,"Copy",  "", "") ACTION_KEY(QKey::Copy)
  PUSH_ACTION(actPaste,"Paste", "", "") ACTION_KEY(QKey::Paste)

  TOGL_ACTION(actViewStatusbar ,"Statusbar",  "", "") ACTION_KEY(Qt::Key_F12)
#ifndef Q_OS_OSX
  TOGL_ACTION(actFullscreen    ,"Fullscreen", "", "") ACTION_KEY(Qt::Key_F11)
#endif
  PUSH_ACTION(actViewReset    ,"Reset", "","")

  PUSH_ACTION(actPreferences         ,"Preferences...","","")
  PUSH_ACTION(actFitErrorParameters  ,"Fit error parameters...","","")

  PUSH_ACTION(actAbout     ,"About...","","")

  TOGL_ACTION(actSelectPeak           ,"Select Peak", "Select Peak", ":/icon/selectPeak")
  PUSH_ACTION(actReflectionPeak      ,"Peak",  "Set reflection peak",  ":/icon/selectHight")
  PUSH_ACTION(actReflectionWidth     ,"Width", "Set reflection width", ":/icon/selectWidth")
  PUSH_ACTION(actReflectionAdd       ,"Add",   "Add reflection",       ":/icon/add")
  PUSH_ACTION(actReflectionRemove    ,"Remove","Remove reflection",    ":/icon/rem")
  actReflectionRemove->setEnabled(false);
  PUSH_ACTION(actReflectionSelectRegion,"Select reflection fit region","","")

  PUSH_ACTION(actCalculatePolefigures,"Calculate polefigures...","","") // TODO polefigure or pole figure?
  PUSH_ACTION(actCalculateHistograms ,"Calculate histograms..","","")

  TOGL_ACTION(actImagesLink           ,"Link",         "Use the same value for all cuts", ":/icon/linkNew")
  TOGL_ACTION(actImageOverlay         ,"overlay",      "Show cut", ":/icon/imageCrop")
  TOGL_ACTION(actImagesGlobalNorm     ,"global norm.", "Display data using a fixed intensity scale", ":/icon/scale")
  PUSH_ACTION(actImageRotate         ,"Rotate",       "Rotate 90° clockwise", ":/icon/rotate0") ACTION_KEY(Qt::CTRL|Qt::Key_R)
  TOGL_ACTION(actImageMirror          ,"Mirror",       "Mirror image", ":/icon/mirror_horz")

  PUSH_ACTION(actBackgroundClear     ,"Clear background fit regions",    "Clear regions for background fitting", ":/icon/clearBackground")
  TOGL_ACTION(actBackgroundBackground ,"Select background fit regions",    "Select regions for background fitting", ":/icon/pekBackground")
  TOGL_ACTION(actBackgroundShowFit    ,"BackgroundEye", "Show background fit", ":/icon/showBackground")

  TOGL_ACTION(actHasBeamOffset        ,"Beam centre offset", "Enable beam center offset (for X-ray instruments)", ":/icon/beam")

  PUSH_ACTION(actNormalizationDisable    ,"Disable","","")
  PUSH_ACTION(actNormalizationMeasureTime,"Measurement Time","","")
  PUSH_ACTION(actNormalizationMonitor    ,"Monitor counts","","")
      PUSH_ACTION(actNormalizationBackground ,"Background level","","")
}

uint TheHub::numFiles(bool withCorr) const {
  return session->numFiles(withCorr);
}

bool TheHub::hasCorrFile() const {
  return session->hasCorrFile();
}

bool TheHub::isCorrFile(uint index) const {
  return hasCorrFile() && numFiles(true) == index+1;
}

str TheHub::fileName(uint index) const {
  return session->getFile(index)->getName();
}

str TheHub::filePath(uint index) const {
  return session->getFile(index)->getInfo().absoluteFilePath();
}

core::shp_File TheHub::getFile(uint index) const {
  return session->getFile(index);
}

void TheHub::remFile(uint i) {
  if (hasCorrFile() && numFiles(true) == i+1) {
    super::remCorrFile();
    emit correctionEnabled(super::isCorrEnabled());
    emit filesChanged();
  } else {
    super::remFile(i);
    emit filesChanged();
  }

  if (0==numFiles(true)) {
    setSelectedDataset(core::shp_Dataset());
    setImageCut(true,false,ImageCut());
  }
}

void TheHub::setSelectedFile(core::shp_File file) {
  emit fileSelected(file);
}

void TheHub::setSelectedDataset(core::shp_Dataset dataset) {
  emit datasetSelected(dataset);
}

//------------------------------------------------------------------------------
// eof
