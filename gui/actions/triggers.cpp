// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/actions/triggers.cpp
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
#include "gui/dialogs/about.h"
#include "gui/dialogs/check_update.h"
#include "gui/dialogs/output_diagrams.h"
#include "gui/dialogs/export_dfgram.h"
#include "gui/dialogs/output_polefigures.h"
#include <QDesktopServices>

Triggers::Triggers()
{
#define AT &QAction::triggered
    connect(&about, AT, [](){ AboutBox().exec(); });
    connect(&addFiles, AT, []() { gGui->addFiles(); });
    connect(&checkUpdate, AT, []() { CheckUpdate _(gGui); });
    connect(&clearBackground, AT, []() { gSession->baseline().setRanges({}); });
    connect(&clearSession, AT, []() { gSession->clear(); });
    connect(&corrFile, AT, []() { gGui->loadCorrFile(); });
    connect(&loadSession, AT, []() { gGui->loadSession(); });
    connect(&online, AT, []() { QDesktopServices::openUrl(QUrl(STECA2_PAGES_URL)); });
    connect(&outputDiagrams, AT, [](){ DiagramsFrame().exec(); });
    connect(&exportDfgram, AT, [](){ DiffractogramsFrame().exec(); });
    //connect(&exportDfgram, AT, [](){ ExportDfgram().exec(); });
    connect(&exportTable, AT, [](){ gSession->exportTable(); });
    connect(&exportXY, AT, [](){ gSession->exportXY(); });
    connect(&outputPolefigures, AT, []() { PoleFiguresFrame().exec(); });
    connect(&quit, AT, []() { gGui->close(); });
    connect(&removeFile, AT, []() { gSession->dataset().removeFile(); });
    connect(&saveSession, AT, []() { gGui->saveSession(); });
    connect(&viewReset, AT, []() { gGui->viewReset(); });
}
