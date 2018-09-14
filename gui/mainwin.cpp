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
#include "core/misc/async.h"
#include "core/misc/settings.h"
#include "core/session.h"
#include "gui/actions/image_trafo_actions.h"
#include "gui/actions/menus.h"
#include "gui/view/toggles.h"
#include "gui/actions/triggers.h"
#include "gui/panels/mainframe.h"
#include "gui/panels/subframe_clusters.h"
#include "gui/panels/subframe_dfgram.h"
#include "gui/panels/subframe_files.h"
#include "gui/panels/subframe_metadata.h"
#include "gui/panels/subframe_setup.h"
//#include "qcr/base/debug.h"
#include <QProgressBar>
#include <QSplitter>
#include <QStatusBar>

MainWin* gGui; //!< global pointer to _the_ main window

//  ***********************************************************************************************
//! @class MainWin

MainWin::MainWin()
{
    gGui = this;

    triggers = new Triggers;
    toggles = new Toggles;
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
    splTop->addWidget(new SubframeSetup());
    splTop->addWidget(new Mainframe());
    splTop->setStretchFactor(1, 1);

    auto* splMain = new QSplitter {Qt::Vertical};
    splMain->setChildrenCollapsible(false);
    splMain->addWidget(splTop);
    splMain->addWidget(new SubframeDfgram());
    splMain->setStretchFactor(1, 1);
    setCentralWidget(splMain);

    auto* progressBar = new QProgressBar{this};
    statusBar()->addWidget(progressBar);
    TakesLongTime::registerProgressBar(progressBar);

    // connect toggles
    toggles->viewStatusbar.setHook([this](bool on){statusBar()  ->setVisible(on);});
    toggles->viewFiles    .setHook([this](bool on){dockFiles_   ->setVisible(on);});
    toggles->viewClusters .setHook([this](bool on){dockClusters_->setVisible(on);});
    toggles->viewMetadata .setHook([this](bool on){dockMetadata_->setVisible(on);});
#ifndef Q_OS_OSX
    toggles->fullScreen   .setHook([this](bool on){
            if (on) showFullScreen(); else showNormal();});
#endif

    // initialize state
    readSettings();

    setRemake( [=]() { refresh(); } );
    show(); // must be called before initial remakeAll because remakeAll depends on visibility
    remakeAll();
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

void MainWin::refresh()
{
    bool hasData = gSession->hasData();
    bool hasPeak = gSession->peaks.size();
    bool hasBase = gSession->baseline.ranges.size();
    toggles->enableCorr.setEnabled(gSession->hasCorrFile());
    triggers->exportDfgram.setEnabled(hasData);
    triggers->exportBigtable.setEnabled(hasData && hasPeak);
    triggers->exportDiagram.setEnabled(hasData && hasPeak);
    triggers->baserangeAdd   .setEnabled(hasData);
    triggers->baserangeRemove.setEnabled(hasBase);
    triggers->baserangesClear.setEnabled(hasBase);
    triggers->peakAdd   .setEnabled(hasData);
    triggers->peakRemove.setEnabled(hasPeak);
    triggers->peaksClear.setEnabled(hasPeak);
    triggers->removeFile.setEnabled(hasData);
    menus_->export_->setEnabled(hasData);
    menus_->image_->setEnabled(hasData);
    menus_->dgram_->setEnabled(hasData);
}

void MainWin::resetViews()
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
