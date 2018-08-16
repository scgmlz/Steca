//  ***********************************************************************************************
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
//  ***********************************************************************************************

#include "gui/actions/triggers.h"
#include "manifest.h"
#include "core/session.h"
#include "gui/dialogs/message_boxes.h"
#include "gui/dialogs/check_update.h"
#include "gui/dialogs/export_bigtable.h"
#include "gui/dialogs/export_dfgram.h"
#include "gui/dialogs/export_diagram.h"
#include "gui/dialogs/export_polefig.h"
#include "gui/dialogs/popup_bigtable.h"
#include "gui/dialogs/popup_diagram.h"
#include "gui/dialogs/popup_polefig.h"
#include "gui/mainwin.h"
#include <QDesktopServices>

Triggers::Triggers()
{
#define AT &QAction::triggered
    QObject::connect(&about, AT, [](){ AboutBox().exec(); });
    QObject::connect(&baserangeAdd, &QAction::triggered, [](){ AddRangeBox("baseline").exec(); });
    QObject::connect(&peakAdd, &QAction::triggered, [](){ AddRangeBox("peak").exec(); });
    QObject::connect(&addFiles, AT, []() { gGui->addFiles(); });
    QObject::connect(&checkUpdate, AT, []() { CheckUpdate _(gGui); });
    QObject::connect(&clearSession, AT, []() { gSession->clear(); });
    QObject::connect(&corrFile, AT, []() { gGui->loadCorrFile(); });
    QObject::connect(&exportDfgram, AT, [](){ ExportDfgram().exec(); });
    QObject::connect(&exportPolefig, AT, [](){ ExportPolefig().exec(); });
    QObject::connect(&exportBigtable, AT, [](){ ExportBigtable().exec(); });
    QObject::connect(&exportDiagram, AT, [](){ ExportDiagram().exec(); });
    QObject::connect(&loadSession, AT, []() { gGui->loadSession(); });
    QObject::connect(&online, AT, []() { QDesktopServices::openUrl(QUrl(STECA2_PAGES_URL)); });
    QObject::connect(&quit, AT, []() { gGui->deleteLater(); });
    QObject::connect(&removeFile, AT, []() { gSession->dataset.removeFile(); });
    QObject::connect(&saveSession, AT, []() { gGui->saveSession(); });
    QObject::connect(&spawnDiagram, AT, [](){ new PopupDiagram(); });
    QObject::connect(&spawnTable, AT, [](){ new PopupBigtable(); });
    QObject::connect(&spawnPolefig, AT, [](){ new PopupPolefig(); });
    QObject::connect(&viewReset, AT, []() { gGui->viewReset(); });

    // Remakes (others are done more conveniently through Mainwindow::refresh):
    corrFile.setRemake([this]() {
            bool hasCorr = gSession->hasCorrFile();
            corrFile.setIcon(QIcon(hasCorr ? ":/icon/rem" : ":/icon/add"));
            QString text = QString(hasCorr ? "Remove" : "Add") + " correction file";
            corrFile.setText(text);
            corrFile.setToolTip(text.toLower()); });

    // Hooks:
    peakRemove.setHook([](){
            gSession->peaks.removeSelected();
            gSession->onPeaks(); });
    peaksClear.setHook([](){
            gSession->peaks.clear();
            gSession->onPeaks(); });
}
