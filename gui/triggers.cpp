// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/triggers.cpp
//! @brief     Implements class Triggers
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#include "triggers.h"
#include "../manifest.h"
#include "core/session.h"
#include "gui/mainwin.h"
#include "gui/output/output_diagrams.h"
#include "gui/output/output_diffractograms.h"
#include "gui/output/output_polefigures.h"
#include "gui/popup/about.h"
#include <QDesktopServices>

Triggers::Triggers() {

#define AT &QAction::triggered
    QObject::connect(&about, AT, [](){ AboutBox().exec(); });
    QObject::connect(&addFiles, AT, []() { gGui->addFiles(); });
    QObject::connect(&checkUpdate, AT, []() { gGui->checkUpdate(); });
    QObject::connect(&clearBackground, AT, []() { gSession->baseline().setRanges({}); });
    QObject::connect(&clearSession, AT, []() { gSession->clear(); });
    QObject::connect(&corrFile, AT, []() { gGui->loadCorrFile(); });
    QObject::connect(&loadSession, AT, []() { gGui->loadSession(); });
    QObject::connect(&online, AT, []() { QDesktopServices::openUrl(QUrl(STECA2_PAGES_URL)); });
    QObject::connect(&outputDiagrams, AT, [](){ DiagramsFrame().exec(); });
    QObject::connect(&outputDiffractograms, AT, [](){ DiffractogramsFrame().exec(); });
    QObject::connect(&outputPolefigures, AT, []() { PoleFiguresFrame().exec(); });
    QObject::connect(&quit, AT, []() { gGui->close(); });
    QObject::connect(&removeFile, AT, []() { gSession->dataset().removeFile(); });
    QObject::connect(&rotateImage, AT, []() { gGui->setImageRotate(
                gSession->imageTransform().nextRotate()); });
    QObject::connect(&saveSession, AT, []() { gGui->saveSession(); });
    QObject::connect(&viewReset, AT, []() { gGui->viewReset(); });
}
