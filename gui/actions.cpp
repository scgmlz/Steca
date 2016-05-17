// ************************************************************************** //
//
//  STeCa2:    StressTexCalculator ver. 2 REVIEW
//
//! @file      actions.cpp
//!
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Original version: Christian Randau
//! @authors   Version 2: Antti Soininen, Jan Burle, Rebecca Brydon
//
// ************************************************************************** //

#include "actions.h"
#include "app.h"
#include "thehub.h"

namespace gui {
//------------------------------------------------------------------------------

Action::Action(rcstr text, rcstr tip, QObject* parent): super(text,parent) {
  setToolTip(tip.isEmpty() ? text : tip);
}

Action& Action::key(QKeySequence key) {
  setShortcut(key);
  return *this;
}

Action& Action::icon(rcstr iconFile) {
  setIcon(QIcon(iconFile));
  return *this;
}

Action& Action::alt(rcstr /*text2*/, rcstr /*tip2*/) {
  return *this;
}

//------------------------------------------------------------------------------

TriggerAction::TriggerAction(rcstr text, rcstr tip, QObject* parent)
: super(text,tip,parent) {
}

//------------------------------------------------------------------------------

ToggleAction::ToggleAction(rcstr text, rcstr tip, QObject* parent)
  : super(text,tip,parent), text1_(text), tip1_(tip.isEmpty() ? text : tip) {
  setCheckable(true);
}

Action& ToggleAction::alt(rcstr text2, rcstr tip2) {
  text2_ = text2; tip2_ = tip2.isEmpty() ? text2 : tip2;
  connect(this,&thisClass::toggled,[this](bool on) {
    setText(on ? text2_ : text1_);
    setToolTip(on ? tip2_ : tip1_);
  });

  return super::alt(text2, tip2);
}

//------------------------------------------------------------------------------

Actions::Actions(TheHub& hub): super(hub) {
  using QKey = QKeySequence;

  // create actions

  trg(about,
      "About...", "About " + App::applicationName());
  trg(quit,
      "Quit");

  tgl(viewStatusbar,
      "Statusbar");
  tgl(viewFiles,
      "Files");
  tgl(viewDatasets,
      "Datasets");
  tgl(viewDatasetInfo,
      "Dataset info");
  trg(viewReset,
      "Reset", "Reset views");
#ifndef Q_OS_OSX
  tgl(fullScreen,
      "FullScreen");
#endif

  trg(loadSession,
      "Load session...");
  trg(saveSession,
      "Save session...");

  trg(addFiles,
      "Add files...")
      .icon(":/icon/add");
  trg(remFile,
      "Remove selected file(s)")
      .icon(":/icon/rem");
  tgl(enableCorr,
      "Enable correction file...", "Enable correction by correction file")
      .alt("Disable correction file", "Disable correction by correction file")
      .icon(":/icon/useCorrection");
  trg(remCorr, "Remove correction file")
      .icon(":/icon/clear");

  trg(rotateImage,
      "Rotate", "Rotate by 90° clockwise")
      .icon(":/icon/rotate0");
  tgl(mirrorImage,
      "Mirror", "Mirror image")
      .icon(":/icon/mirror_horz");
  tgl(linkCuts,
      "Link", "Use the same value for all cuts")
      .alt("Unlink", "Use different values for cuts")
      .icon(":/icon/linkNew");
  tgl(showCut,
      "overlay", "Show cut")
      .alt("overlay", "Hide cut")
      .icon(":/icon/imageCrop");
  tgl(hasBeamOffset,
      "Beam centre offset", "Enable beam center offset (for X-ray instruments)")
      .alt("Beam centre offset", "Disable beam center offset")
      .icon(":/icon/beam");

  tgl(fixedIntenImageScale,
      "Fixed image scale", "Display image using a fixed intensity scale")
      .alt("Variable image scale", "Display image using normalized intensity scale")
      .icon(":/icon/scale");
  tgl(fixedIntenDgramScale,
      "Fixed diffractogram scale", "Display diffractogram using a fixed intensity scale")
      .alt("Variable diffractogram scale", "Display diffractogram using normalized intensity scale");

  // TODO make combined, *not* average
  tgl(avgCurveDgram,
      "Combined diffractogram", "Show diffractogram of all datasets")
      .alt("Single diffractogram", "Show diffractogram of a single dataset");

  tgl(fitRegions,
      "Select regions", "Select regions for fitting")
      .icon(":/icon/peakBackground"); // RENAME icon file
  trg(fitBgClear,
      "Clear background regions", "Clear regions for background fitting")
      .icon(":/icon/clear");
  tgl(fitBgShow, "Show background", "Show fitted background")
      .icon(":/icon/showBackground");

  trg(addReflection,
      "Add", "Add reflection")
      .icon(":/icon/add");
  trg(remReflection,
      "Remove", "Remove reflection")
      .icon(":/icon/rem");

  trg(outputPolefigures,
      "Pole figures...");
  trg(outputHistograms,
      "Histograms...");


  // key shortcuts

  quit            -> key(QKey::Quit);

  viewStatusbar   -> key(Qt::Key_F12);
  viewFiles       -> key(Qt::Key_F8);
  viewDatasets    -> key(Qt::Key_F9);
  viewDatasetInfo -> key(Qt::Key_F10);

#ifndef Q_OS_OSX
  fullScreen      -> key(Qt::Key_F11);
#endif

  addFiles        -> key(Qt::CTRL|Qt::Key_O);
  remFile         -> key(QKey::Delete);
  enableCorr      -> key(Qt::SHIFT|Qt::CTRL|Qt::Key_C);
  remCorr         -> key(QKey::Delete);

  rotateImage     -> key(Qt::CTRL|Qt::Key_R);

  // handle signals

  onSigFilesSelected([this]() {
    remFile->setEnabled(!hub_.collectedFromFiles().isEmpty());
  });

  onSigCorrFile([this](core::shp_File file) {
    remCorr->setEnabled(!file.isNull());
  });

  onSigCorrEnabled([this](bool on) {
    enableCorr->setChecked(on);
  });

  onSigDatasetsChanged([this]() {
    fixedIntenImageScale->setChecked(false);
    fixedIntenDgramScale->setChecked(false);
    avgCurveDgram->setChecked(false);
  });
}

Action& Actions::trg(Action* &action, rcstr text, rcstr tip) {
  return *(action = new TriggerAction(text,tip,&hub_));
}

Action& Actions::tgl(Action* &action, rcstr text, rcstr tip) {
  return *(action = new ToggleAction(text,tip,&hub_));
}

//------------------------------------------------------------------------------
}
// eof
