//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/actions/toggles.cpp
//! @brief     Implements class Toggles
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "toggles.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "qcr/engine/debug.h"
#include <QDockWidget>
#include <QStatusBar>

Toggles::Toggles()
{
    linkCuts = new QcrToggle {"linkCuts", &gSession->imageCut().linked, "Link cuts", ":/icon/link"};

#define AT &QAction::toggled

    connect(&enableCorr, AT, [](bool on) { gSession->corrset().tryEnable(on); });

    connect(&fixedIntenImage, AT, [](bool on) {
            EMITS("Toggle::fixedIntenImage", gSession->sigImage());
            EMITS("Toggle::fixedIntenImage", gSession->sigDfgram());
        });
    connect(&fixedIntenDgram, AT, [](bool on) {
            EMITS("Toggles::fixedIntenDgram", gSession->sigImage());
            EMITS("Toggles::fixedIntenDgram", gSession->sigDfgram());
        });
    connect(&combinedDgram, AT, [](bool on) {
            EMITS("Toggles::combinedDgram", gSession->sigImage());
            EMITS("Toggles::combinedDgram", gSession->sigDfgram());
        });
    connect(&showBackground, AT, [](bool on) {
            EMITS("Toggles::showBackground", gSession->sigDfgram());
        });

    connect(&viewStatusbar, AT, [](bool on) { gGui->statusBar()->setVisible(on); });
#ifndef Q_OS_OSX
    connect(&fullScreen, AT, [](bool on) {
            if (on)
                gGui->showFullScreen();
            else
                gGui->showNormal(); });
#endif
    connect(&viewFiles, AT, [](bool on) { gGui->dockFiles_->setVisible(on); });
    connect(&viewClusters, AT, [](bool on) { gGui->dockClusters_->setVisible(on); });
    connect(&viewMetadata, AT, [](bool on) { gGui->dockMetadata_->setVisible(on); });
}

Toggles::~Toggles()
{
    delete linkCuts;
}
