// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      actions.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
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

Action& Action::text(rcstr text, bool alsoTip) {
  setText(text);
  if (alsoTip)
    tip(text);
  return *this;
}

Action& Action::tip(rcstr tip) {
  setToolTip(tip);
  return *this;
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

TriggerAction::TriggerAction(rcstr text, QObject* parent)
: Cls(text, "", parent) {
}

TriggerAction::TriggerAction(rcstr text, rcstr tip, QObject* parent)
: super(text,tip,parent)
{
}

//------------------------------------------------------------------------------

ToggleAction::ToggleAction(rcstr text, QObject* parent)
: Cls(text, "", parent) {
}

ToggleAction::ToggleAction(rcstr text, rcstr tip, QObject* parent)
: super(text,tip,parent), text1_(text), tip1_(tip.isEmpty() ? text : tip)
{
  setCheckable(true);
}

Action& ToggleAction::alt(rcstr text2, rcstr tip2) {
  text2_ = text2; tip2_ = tip2.isEmpty() ? text2 : tip2;
  connect(this,&Cls::toggled,[this](bool on) {
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
  trg(online,
      "Online documentation...");
  trg(quit,
      "Quit");

  tgl(viewStatusbar,
      "Statusbar");
  tgl(viewFiles,
      "Files");
  tgl(viewDatasets,
      "Datasets");
  tgl(viewDatasetInfo,
      "Metadata");
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
      .icon(":/icon/mirrorHorz");
  tgl(linkCuts,
      "Link cut", "Use the same value for all cuts")
      .icon(":/icon/link");
  tgl(showOverlay,
      "Overlay", "Show cut")
      .icon(":/icon/crop");
  tgl(hasBeamOffset,
      "Beam centre offset", "Enable beam center offset (for X-ray instruments)")
      .alt("Beam centre offset", "Disable beam center offset")
      .icon(":/icon/beamOffset");

  tgl(fixedIntenImageScale,
      "Fixed image scale", "Display image using a fixed intensity scale")
      .alt("Variable image scale", "Display image using normalised intensity scale")
      .icon(":/icon/scale");
  tgl(fixedIntenDgramScale,
      "Fixed diffractogram scale", "Display diffractogram using a fixed intensity scale")
      .alt("Variable diffractogram scale", "Display diffractogram using normalised intensity scale");

  tgl(combinedDgram,
      "Combined diffractogram", "Show diffractogram of all datasets")
      .alt("Single diffractogram", "Show diffractogram of a single dataset");

  tgl(fitRegions, "Select regions").icon(":/icon/bgRegion");
  trg(fitBgClear,
      "Clear background regions", "Clear regions for background fitting")
      .icon(":/icon/clear");
  tgl(fitBgShow, "Show background", "Show fitted background")
      .icon(":/icon/showBackground");

  trg(addReflection,
      "Add reflection")
      .icon(":/icon/add");
  trg(remReflection,
      "Remove reflection")
      .icon(":/icon/rem");

  trg(outputPolefigures,
      "Pole figures...");
  trg(outputDiagrams,
      "Diagrams...");
  trg(outputDiffractograms,
      "Diffractograms...");

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

  rotateImage     -> key(Qt::CTRL|Qt::Key_R);

  // handle signals

  onSigFilesSelected([this]() {
    remFile->setEnabled(!hub_.collectedFromFiles().isEmpty());
  });

  onSigCorrFile([this](data::shp_File file) {
    remCorr->setEnabled(!file.isNull());
  });

  onSigCorrEnabled([this](bool on) {
    enableCorr->setChecked(on);
  });

  auto deselect = [this]() {
    fixedIntenImageScale->setChecked(false);
    fixedIntenDgramScale->setChecked(false);
    combinedDgram->setChecked(false);
  };

  onSigGeometryChanged([deselect]() {
    deselect();
  });

  onSigDatasetsChanged([this,deselect]() {
    deselect();
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
