// ************************************************************************** //
//
//  STeCa2:    StressTextureCalculator ver. 2
//
//! @file      core_curve.cpp
//!
//! @homepage  http://apps.jcns.fz-juelich.de/steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016
//! @authors   Scientific Computing Group at MLZ Garching
//! @authors   Rebecca Brydon, Jan Burle,  Antti Soininen
//! @authors   Based on the original STeCa by Christian Randau
//
// ************************************************************************** //

#include "mainwin.h"
#include "actions.h"
#include "../manifest.h"
#include "output/output_diagrams.h"
#include "output/output_diffractograms.h"
#include "output/output_polefigures.h"
#include "panels/panel_dataset.h"
#include "panels/panel_diffractogram.h"
#include "panels/panel_file.h"
#include "panels/panel_fitting.h"
#include "panels/panel_fitting.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDate>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QMenuBar>
#include <QMessageBox>
#include <QSplitter>
#include <QStatusBar>
#include <QUrl>
#include <QtMultimedia/QSound>

namespace gui {
//------------------------------------------------------------------------------

class SplitImage : public BoxWidget {
  SUPER(SplitImage, BoxWidget)
public:
  SplitImage(TheHub&);
};

SplitImage::SplitImage(TheHub& hub) : super(Qt::Horizontal) {
  auto* options1 = new panel::DatasetOptions1(hub);
  auto* options2 = new panel::DatasetOptions2(hub);
  auto* dataset  = new panel::Dataset(hub);

  connect(options2, &panel::DatasetOptions2::imageScale,
          dataset,  &panel::Dataset::setImageScale);
  box_->addWidget(options1);
  box_->addWidget(options2);
  box_->addWidget(dataset);
  box_->setStretch(2, 1);
}

//------------------------------------------------------------------------------

class SplitFitting : public BoxWidget {
  SUPER(SplitFitting, BoxWidget)
public:
  SplitFitting(TheHub&);
};

SplitFitting::SplitFitting(TheHub& hub) : super(Qt::Vertical) {
  box_->addWidget(new panel::Fitting(hub));
}

//------------------------------------------------------------------------------

class SplitDiffractogram : public BoxWidget {
  SUPER(SplitDiffractogram, BoxWidget)
public:
  SplitDiffractogram(TheHub&);
};

SplitDiffractogram::SplitDiffractogram(TheHub& hub) : super(Qt::Horizontal) {
  auto diffractogram = new panel::Diffractogram(hub);
  diffractogram->setHorizontalStretch(1);
  box_->addWidget(diffractogram);
}

//------------------------------------------------------------------------------

MainWin::MainWin() : hub_(), acts_(hub_.actions) {
  setWindowIcon(QIcon(":/icon/retroStier"));
  QDir::setCurrent(QDir::homePath());

  initMenus();
  initLayout();
  initStatusBar();
  connectActions();

  readSettings();
}

void MainWin::initMenus() {
  auto separator = [this]() {
    auto act = new QAction(this);
    act->setSeparator(true);
    return act;
  };

  auto* mbar = menuBar();
  mbar->setNativeMenuBar(true);

  menuFile_     = mbar->addMenu("&File");
  menuView_     = mbar->addMenu("&View");
  menuDetector_ = mbar->addMenu("&Detector");
  menuImage_    = mbar->addMenu("&Image");
  menuDgram_    = mbar->addMenu("Di&ffractogram");
  menuOutput_   = mbar->addMenu("&Output");
  menuHelp_     = mbar->addMenu("&Help");

  menuFile_->addActions({
      acts_.addFiles, acts_.remFile,
      separator(),
      acts_.enableCorr, acts_.remCorr,
      separator(),
      acts_.loadSession, acts_.saveSession,
  });

  menuFile_->addActions({
    #ifndef Q_OS_OSX  // Mac puts Quit into the Apple menu
      separator(),
    #endif
      acts_.quit,
  });

  menuView_->addActions({
      acts_.viewFiles, acts_.viewDatasets, acts_.viewDatasetInfo,
      separator(),
    #ifndef Q_OS_OSX
      acts_.fullScreen,
    #endif
      acts_.viewStatusbar,
      separator(),
      acts_.viewReset,
  });

  menuDetector_->addActions({
      acts_.hasBeamOffset,
  });

  menuImage_->addActions({
      acts_.rotateImage,
      acts_.mirrorImage,
      acts_.fixedIntenImageScale,
      acts_.showOverlay,
      acts_.linkCuts,
  });

  menuDgram_->addActions({
      acts_.fitRegions,
      acts_.fitBgClear,
      acts_.fitBgShow,
      separator(),
      acts_.addReflection, acts_.remReflection,
      separator(),
      acts_.combinedDgram,
      acts_.fixedIntenDgramScale,
  });

  menuOutput_->addActions({
      acts_.outputPolefigures, acts_.outputDiagrams, acts_.outputDiffractograms,
  });

  menuHelp_->addActions({
    acts_.about,
  #ifndef Q_OS_OSX
    separator(),  // Mac puts About into the Apple menu
  #endif
    acts_.online,
  });
}

void MainWin::initLayout() {
  addDockWidget(Qt::LeftDockWidgetArea,
                (dockFiles_ = new panel::DockFiles(hub_)));
  addDockWidget(Qt::LeftDockWidgetArea,
                (dockDatasets_ = new panel::DockDatasets(hub_)));
  addDockWidget(Qt::RightDockWidgetArea,
                (dockDatasetInfo_ = new panel::DockDatasetInfo(hub_)));

  auto splMain = new QSplitter(Qt::Vertical);
  splMain->setChildrenCollapsible(false);
  setCentralWidget(splMain);

  auto splImages = new QSplitter(Qt::Horizontal);
  splImages->setChildrenCollapsible(false);

  auto splReflections = new QSplitter(Qt::Horizontal);
  splReflections->setChildrenCollapsible(false);

  splMain->addWidget(splImages);
  splMain->addWidget(splReflections);

  splImages->addWidget(new SplitImage(hub_));

  splReflections->addWidget(new SplitFitting(hub_));
  splReflections->addWidget(new SplitDiffractogram(hub_));
  splReflections->setStretchFactor(1, 1);
}

void MainWin::initStatusBar() {
  statusBar();
}

void MainWin::connectActions() {
  auto onTrigger = [this](QAction* action, void (thisClass::*fun)()) {
    QObject::connect(action, &QAction::triggered, this, fun);
  };

  auto onToggle = [this](QAction* action, void (thisClass::*fun)(bool)) {
    QObject::connect(action, &QAction::toggled, this, fun);
  };

  onTrigger(acts_.loadSession, &thisClass::loadSession);
  onTrigger(acts_.saveSession, &thisClass::saveSession);

  onTrigger(acts_.addFiles, &thisClass::addFiles);
  onTrigger(acts_.enableCorr, &thisClass::enableCorr);

  onTrigger(acts_.quit, &thisClass::close);

  onTrigger(acts_.outputPolefigures,    &thisClass::outputPoleFigures);
  onTrigger(acts_.outputDiagrams,       &thisClass::outputDiagrams);
  onTrigger(acts_.outputDiffractograms, &thisClass::outputDiffractograms);

  onTrigger(acts_.about,  &thisClass::about);
  onTrigger(acts_.online, &thisClass::online);

  onToggle(acts_.viewStatusbar, &thisClass::viewStatusbar);
#ifndef Q_OS_OSX
  onToggle(acts_.fullScreen, &thisClass::viewFullScreen);
#endif

  onToggle(acts_.viewFiles, &thisClass::viewFiles);
  onToggle(acts_.viewDatasets, &thisClass::viewDatasets);
  onToggle(acts_.viewDatasetInfo, &thisClass::viewDatasetInfo);

  onTrigger(acts_.viewReset, &thisClass::viewReset);
}

class AboutBox : public QMessageBox {
  SUPER(AboutBox, QMessageBox)
public:
  using super::super;

protected:
  void mouseDoubleClickEvent(QMouseEvent*);
};

void AboutBox::mouseDoubleClickEvent(QMouseEvent*) {
  QSound::play(":/HAL/good_evening");
}

void MainWin::about() {
  str appName = qApp->applicationDisplayName();
  str version = qApp->applicationVersion();

  str title = str("About %1").arg(appName);
  str text  = str("<h4>%1 ver. %2</h4>").arg(appName, version);
  str info  = str("<p>StressTextureCalculator</p>"
                  "<p>Copyright: Forschungszentrum Jülich GmbH %1</p>"
                  "<p><a href='%2'>%2</a></p>")
                  .arg(QDate::currentDate().toString("yyyy"))
                  .arg(STECA2_URL);

  auto box = new AboutBox(QMessageBox::NoIcon,
                title, text, QMessageBox::Close, this);

  box->setInformativeText(info);
  auto pm = QPixmap(":/icon/retroStier")
            .scaled(120, 120, Qt::KeepAspectRatio, Qt::SmoothTransformation);
  box->setIconPixmap(pm);
  box->exec();
}

void MainWin::online() {
  QDesktopServices::openUrl(QUrl(STECA2_URL));
}

void MainWin::show() {
  super::show();
  onShow();
}

void MainWin::close() {
  super::close();
}

void MainWin::addFiles() {
  str_lst fileNames = QFileDialog::getOpenFileNames(
      this, "Add files", QDir::current().absolutePath(),
      "Data files (*.dat);;All files (*.*)");

  if (!fileNames.isEmpty()) {
    hub_.addFiles(fileNames);
    QDir::setCurrent(QFileInfo(fileNames.at(0)).absolutePath());
  }
}

void MainWin::enableCorr() {
  str fileName;
  if (!hub_.hasCorrFile())
    fileName = QFileDialog::getOpenFileName(
        this, "Set correction file", QDir::current().absolutePath(),
        "Data files (*.dat);;All files (*.*)");

  if (!fileName.isEmpty()) {
    hub_.setCorrFile(fileName);
    QDir::setCurrent(QFileInfo(fileName).absolutePath());
  }
}

static str const STE(".ste");

void MainWin::loadSession() {
  str fileName = QFileDialog::getOpenFileName(
      this, "Load session", QDir::current().absolutePath(),
      "Session files (*" % STE % ");;All files (*.*)");

  if (fileName.isEmpty()) return;

  hub_.loadSession(QFileInfo(fileName));
}

void MainWin::saveSession() {
  QFileDialog dlg(this, "Save session", QDir::current().absolutePath(),
                  "Session files (*" % STE % ");;All files (*.*)");
  dlg.setFileMode(QFileDialog::AnyFile);
  dlg.setAcceptMode(QFileDialog::AcceptSave);

  if (!dlg.exec()) return;

  auto files = dlg.selectedFiles();
  if (files.isEmpty()) return;

  str fileName = files.first();
  if (!fileName.endsWith(STE)) fileName += STE;

  hub_.saveSession(QFileInfo(fileName));
}

void MainWin::outputPoleFigures() {
  auto popup = new output::PoleFiguresFrame(hub_, "Pole Figures", this);
  popup->show();
}

void MainWin::outputDiagrams() {
  auto popup = new output::DiagramsFrame(hub_, "Diagrams", this);
  popup->show();
}

void MainWin::outputDiffractograms() {
  auto popup = new output::DiffractogramsFrame(hub_, "Diffractograms", this);
  popup->show();
}

void MainWin::closeEvent(QCloseEvent* event) {
  onClose();
  event->accept();
}

void MainWin::onShow() {
  checkActions();

#if defined(DEVELOPMENT_REBECCA) || defined(DEVELOPMENT_JAN)
  auto safeLoad = [this](rcstr fileName) {
    QFileInfo info(QDir::homePath() % fileName);
    if (info.exists())
      hub_.loadSession(info);
  };
#endif

#ifdef DEVELOPMENT_REBECCA
  safeLoad("/SCG/STeCa-Data/1.ste");
#endif

#ifdef DEVELOPMENT_JAN
  safeLoad("/P53/+scg/0.ste");
//  hub_.actions.about->trigger();
#endif
}

void MainWin::onClose() {
  saveSettings();
}

static str const GROUP_MAINWIN("MainWin");
static str const KEY_GEOMETRY("geometry");
static str const KEY_STATE("state");

void MainWin::readSettings() {
  if (initialState_.isEmpty()) initialState_ = saveState();

  Settings s(GROUP_MAINWIN);
  restoreGeometry(s.value(KEY_GEOMETRY).toByteArray());
  restoreState(s.value(KEY_STATE).toByteArray());
}

void MainWin::saveSettings() {
  Settings s(GROUP_MAINWIN);
  s.setValue(KEY_GEOMETRY, saveGeometry());
  s.setValue(KEY_STATE, saveState());
}

void MainWin::checkActions() {
  acts_.viewStatusbar->setChecked(statusBar()->isVisible());

#ifndef Q_OS_OSX
  acts_.fullScreen->setChecked(isFullScreen());
#endif

  acts_.viewFiles->setChecked(dockFiles_->isVisible());
  acts_.viewDatasets->setChecked(dockDatasets_->isVisible());
  acts_.viewDatasetInfo->setChecked(dockDatasetInfo_->isVisible());
}

void MainWin::viewStatusbar(bool on) {
  statusBar()->setVisible(on);
  acts_.viewStatusbar->setChecked(on);
}

void MainWin::viewFullScreen(bool on) {
  if (on)
    showFullScreen();
  else
    showNormal();

#ifndef Q_OS_OSX
  acts_.fullScreen->setChecked(on);
#endif
}

void MainWin::viewFiles(bool on) {
  dockFiles_->setVisible(on);
  acts_.viewFiles->setChecked(on);
}

void MainWin::viewDatasets(bool on) {
  dockDatasets_->setVisible(on);
  acts_.viewDatasets->setChecked(on);
}

void MainWin::viewDatasetInfo(bool on) {
  dockDatasetInfo_->setVisible(on);
  acts_.viewDatasetInfo->setChecked(on);
}

void MainWin::viewReset() {
  restoreState(initialState_);
  viewStatusbar(true);
  viewFullScreen(false);
  viewFiles(true);
  viewDatasets(true);
  viewDatasetInfo(true);
}

//------------------------------------------------------------------------------
}
// eof
