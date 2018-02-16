// ************************************************************************** //
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
// ************************************************************************** //

#include "toggles.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "gui/dialogs/about.h"
#include <QDockWidget>
#include <QStatusBar>

Toggles::Toggles() {

#define AT &QAction::toggled

    QObject::connect(&enableCorr, AT, [this](bool on) {
            gSession->corrset().tryEnable(on); });
    QObject::connect(&mirrorImage, AT, [this](bool on) { gGui->setImageMirror(on); });

    // TODO rm state variables
    QObject::connect(&fixedIntenImage, AT, [this](bool on) {
        gGui->isFixedIntenImageScale_ = on;
        emit gSession->sigImage();
        emit gSession->sigDiffractogram();
        });
    QObject::connect(&fixedIntenDgram, AT, [this](bool on) {
        gGui->isFixedIntenDgramScale_ = on;
        emit gSession->sigImage();
        emit gSession->sigDiffractogram();
        });
    QObject::connect(&combinedDgram, AT, [this](bool on) {
        gGui->isCombinedDgram_ = on;
        emit gSession->sigImage();
        emit gSession->sigDiffractogram();
        });

    QObject::connect(&viewStatusbar, AT, [](bool on) { gGui->statusBar()->setVisible(on); });
#ifndef Q_OS_OSX
    QObject::connect(&fullScreen, AT, [](bool on) {
            if (on)
                gGui->showFullScreen();
            else
                gGui->showNormal(); });
#endif
    QObject::connect(&viewFiles, AT, [](bool on) { gGui->dockFiles_->setVisible(on); });
    QObject::connect(&viewClusters, AT, [](bool on) { gGui->dockClusters_->setVisible(on); });
    QObject::connect(&viewMetadata, AT, [](bool on) { gGui->dockMetadata_->setVisible(on); });
}
