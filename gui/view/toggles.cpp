//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/toggles.cpp
//! @brief     Implements class Toggles
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/view/toggles.h"
#include "core/session.h"
#include "gui/mainwin.h"
//#include "qcr/base/debug.h"
#include <QStatusBar>

Toggles::Toggles()
    : combinedDfgram {"dfg.all", &gSession->params.showAvgeDfgram,
        "All measurements", ":/icon/all"}
    , enableCorr {"enableCorr", &gSession->corrset.enabled,
        "Enable correction file", ":/icon/useCorrection"}
    , linkCuts {"linkCuts", &gSession->params.imageCut.linked,
              "Link the four cut settings", ":/icon/link"}
{
    QObject::connect(&viewStatusbar, &QAction::toggled, [](bool on){
            gGui->statusBar()->setVisible(on); });
#ifndef Q_OS_OSX
    QObject::connect(&fullScreen, &QAction::toggled, [](bool on){
            if (on)
                gGui->showFullScreen();
            else
                gGui->showNormal(); });
#endif
    QObject::connect(&viewFiles, &QAction::toggled, [](bool on){
            gGui->dockFiles_->setVisible(on); });
    QObject::connect(&viewClusters, &QAction::toggled, [](bool on){
            gGui->dockClusters_->setVisible(on); });
    QObject::connect(&viewMetadata, &QAction::toggled, [](bool on){
            gGui->dockMetadata_->setVisible(on); });
}
