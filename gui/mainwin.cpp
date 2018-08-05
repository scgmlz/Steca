//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/mainwin.cpp
//! @brief     Implements class MainWin
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/mainwin.h"
#include "core/algo/collect_intensities.h"
#include "core/algo/fitting.h"
#include "core/algo/interpolate_polefig.h"
#include "core/aux/async.h"
#include "core/aux/exception.h"
#include "core/aux/settings.h"
#include "core/session.h"
#include "gui/actions/image_trafo_actions.h"
#include "gui/actions/menus.h"
#include "gui/actions/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/dialogs/file_dialog.h"
#include "gui/panels/mainframe.h"
#include "gui/panels/subframe_clusters.h"
#include "gui/panels/subframe_dfgram.h"
#include "gui/panels/subframe_files.h"
#include "gui/panels/subframe_metadata.h"
#include "gui/panels/subframe_setup.h"
#include "gui/state.h"
#include "qcr/base/debug.h"
#include <QSplitter>
#include <QStatusBar>
#include <QStringBuilder> // for ".." % ..
#include <iostream> // debug

MainWin* gGui; //!< global pointer to _the_ main window

namespace {
const QString dataFormats {"Data files (*.dat *.yaml *.mar*);;All files (*.*)"};
}

//  ***********************************************************************************************
//! @class MainWin

MainWin::MainWin()
{
    gGui = this;
    gRoot = this;

    triggers = new Triggers;
    toggles = new Toggles;
    state = new GuiState;
    imageTrafoActions = new ImageTrafoActions;
    menus_ = new Menus(menuBar());

    setWindowIcon(QIcon(":/icon/retroStier"));
    setTabPosition(Qt::AllDockWidgetAreas, QTabWidget::North);
    setAttribute(Qt::WA_DeleteOnClose, true);

    // layout
    setContentsMargins(5,5,5,5);

    addDockWidget(Qt::LeftDockWidgetArea, (dockFiles_    = new SubframeFiles()));
    addDockWidget(Qt::LeftDockWidgetArea, (dockClusters_ = new SubframeClusters()));
    addDockWidget(Qt::LeftDockWidgetArea, (dockMetadata_ = new SubframeMetadata()));

    auto* splTop = new QSplitter {Qt::Horizontal};
    splTop->setChildrenCollapsible(false);
    splTop->addWidget(frameSetup_ = new SubframeSetup());
    splTop->addWidget(mainframe_  = new Mainframe());
    splTop->setStretchFactor(1, 1);

    auto* splMain = new QSplitter {Qt::Vertical};
    splMain->setChildrenCollapsible(false);
    splMain->addWidget(splTop);
    splMain->addWidget(frameDfgram_ = new SubframeDfgram());
    splMain->setStretchFactor(1, 1);
    setCentralWidget(splMain);

    statusBar()->addWidget(&progressBar);

    // initialize state
    readSettings();

    // TODO move this elsewhere
    triggers->removeFile.setRemake([=]() {
            triggers->removeFile.setEnabled(gSession->dataset.countFiles()); });
    triggers->corrFile.setRemake([=]() {
            bool hasCorr = gSession->hasCorrFile();
            triggers->corrFile.setIcon(QIcon(hasCorr ? ":/icon/rem" : ":/icon/add"));
            QString text = QString(hasCorr ? "Remove" : "Add") + " correction file";
            triggers->corrFile.setText(text);
            triggers->corrFile.setToolTip(text.toLower()); });
    toggles->enableCorr.setRemake([=]() {
            toggles->enableCorr.setEnabled(gSession->hasCorrFile()); });

    // how to refresh
    setRemake( [=]() { refresh(); } );
}

MainWin::~MainWin()
{
    saveSettings();
    // the following deletions are obligatory to prevent a crash upon closing this MainWin:
    delete imageTrafoActions;
    delete triggers;
    delete toggles;
    // whereas all the following only reduces the number of perfectly inconsequential leaks:
    delete menus_;
    gGui = nullptr;
}

//  ***********************************************************************************************
//   reset / load / save
//  ***********************************************************************************************

void MainWin::viewReset()
{
    restoreState(initialState_);
#ifndef Q_OS_OSX
    toggles->fullScreen.programaticallySetValue(false);
#endif
    toggles->viewStatusbar.programaticallySetValue(true);
    toggles->viewClusters.programaticallySetValue(true);
    toggles->viewFiles.programaticallySetValue(true);
    toggles->viewMetadata.programaticallySetValue(true);
}

//! Stores native defaults as initialState_, then reads from config file.
void MainWin::readSettings()
{
    if (initialState_.isEmpty())
        initialState_ = saveState();
    XSettings s("MainWin");
    restoreGeometry(s.value("geometry").toByteArray());
    restoreState(s.value("state").toByteArray());
}

void MainWin::saveSettings() const
{
    XSettings s("MainWin");
    s.setValue("geometry", saveGeometry()); // this mainwindow's widget geometry
    s.setValue("state", saveState()); // state of this mainwindow's toolbars and dockwidgets
}

void MainWin::loadSession()
{
    QString fileName = file_dialog::queryImportFileName(
        this, "Load session", sessionDir_, "Session files (*.ste)");
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!(file.open(QIODevice::ReadOnly | QIODevice::Text))) {
        qWarning() << ("Cannot open file for reading: " % fileName);
        return;
    }
    try {
        TakesLongTime __("loadSession");
        gSession->sessionFromJson(file.readAll());
    } catch(Exception& ex) {
        qWarning() << "Could not load session from file " << fileName << ":\n"
                   << ex.msg() << "\n"
                   << "The application may now be in an inconsistent state.\n"
                   << "Please consider to quit the application, and start afresh.\n";
        gSession->clear();
    }
}

void MainWin::saveSession()
{
    QString fileName = file_dialog::queryExportFileName(
        this, "Save session", sessionDir_, "Session files (*.ste)");
    if (!fileName.endsWith(".ste"))
        fileName += ".ste";
    QFileInfo fileInfo(fileName);
    QFile* file = file_dialog::openFileConfirmOverwrite("file", this, fileInfo.filePath());
    if (!file)
        return;
    const int result = file->write(gSession->serializeSession());
    delete file;
    if (!(result >= 0))
        qWarning() << "Could not write session";
}

//  ***********************************************************************************************
//   modal dialogs
//  ***********************************************************************************************

void MainWin::addFiles()
{
    QStringList fileNames
        = file_dialog::queryImportFileNames(this, "Add files", dataDir_, dataFormats);
    repaint();
    if (fileNames.isEmpty())
        return;
    TakesLongTime __("addFiles");
    try {
        gSession->dataset.addGivenFiles(fileNames);
    } catch (Exception ex) {
        qWarning() << ex.msg();
    }
    gRoot->remakeAll("MainWin::addFiles");
}

void MainWin::loadCorrFile()
{
    // TODO catch THROW's
    if (gSession->corrset.hasFile()) {
        gSession->corrset.removeFile();
    } else {
        QString fileName = file_dialog::queryImportFileName(
            this, "Set correction file", dataDir_, dataFormats);
        if (fileName.isEmpty())
            return;
        try {
            gSession->corrset.loadFile(fileName);
        } catch (Exception ex) {
            qWarning() << ex.msg();
        }
    }
    gRoot->remakeAll("MainWin::loadCorrFile");
}

//  ***********************************************************************************************
//   remake / compute
//  ***********************************************************************************************

void MainWin::refresh()
{
    bool hasData = gSession->hasData();
    bool hasPeak = gSession->peaks.count();
    bool hasBase = gSession->baseline.ranges.count();
    triggers->exportDfgram.setEnabled(hasData);
    triggers->exportBigtable.setEnabled(hasData && hasPeak);
    triggers->exportDiagram.setEnabled(hasData && hasPeak);
    triggers->baserangeAdd   .setEnabled(hasData);
    triggers->baserangeRemove.setEnabled(hasBase);
    triggers->baserangesClear.setEnabled(hasBase);
    triggers->peakAdd   .setEnabled(hasData);
    triggers->peakRemove.setEnabled(hasPeak);
    triggers->peaksClear.setEnabled(hasPeak);
    menus_->export_->setEnabled(hasData);
    menus_->image_->setEnabled(hasData);
    menus_->dgram_->setEnabled(hasData);
}

/*
void MainWin::runFits()
{
    if (!gSession->peaks.count()) {
        gSession->setDirectPeakInfos({});
        gSession->setInterpolatedPeakInfos({});
        return;
    }
    algo::rawFits(&gGui->progressBar);
    // algo::interpolateInfos(&gGui->progressBar);
}

void MainWin::runInterpolation()
{
    algo::interpolateInfos(&gGui->progressBar);
}
*/
