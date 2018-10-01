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
#include "gui/dialogs/io_session.h"
#include "gui/dialogs/load_data.h"
#include "gui/dialogs/popup_bigtable.h"
#include "gui/dialogs/popup_diagram.h"
#include "gui/dialogs/popup_polefig.h"
#include "gui/mainwin.h"
#include <QDesktopServices>

Triggers::Triggers()
{
    about          .setTriggerHook([](){ AboutBox().exec(); });
    baserangeAdd   .setTriggerHook([](){ AddRangeBox("baseline").exec(); });
    baserangesClear.setTriggerHook([](){ gSession->baseline.clear();          });
    baserangeRemove.setTriggerHook([](){ gSession->baseline.removeSelected(); });
    peakAdd        .setTriggerHook([](){ AddRangeBox("peak").exec(); });
    addFiles       .setTriggerHook([](){ loadData::addFiles(gGui); });
    checkUpdate    .setTriggerHook([](){ CheckUpdate _(gGui); });
    clearSession   .setTriggerHook([](){ gSession->clear(); });
    corrFile       .setTriggerHook([](){ loadData::loadCorrFile(gGui); });
    exportDfgram   .setTriggerHook([](){ ExportDfgram().exec(); });
    exportPolefig  .setTriggerHook([](){ ExportPolefig().exec(); });
    exportBigtable .setTriggerHook([](){ ExportBigtable().exec(); });
    exportDiagram  .setTriggerHook([](){ ExportDiagram().exec(); });
    loadSession    .setTriggerHook([](){ ioSession::load(gGui); });
    online         .setTriggerHook([](){ QDesktopServices::openUrl(QUrl(STECA2_PAGES_URL)); });
    peakRemove     .setTriggerHook([](){ gSession->peaks.removeSelected(); });
    peaksClear     .setTriggerHook([](){ gSession->peaks.clear();          });
    quit           .setTriggerHook([](){ gGui->deleteLater(); });
    removeFile     .setTriggerHook([](){ gSession->dataset.removeFile(); });
    saveSession    .setTriggerHook([](){ ioSession::save(gGui); });
    spawnDiagram   .setTriggerHook([](){ new PopupDiagram(); });
    spawnTable     .setTriggerHook([](){ new PopupBigtable(); });
    spawnPolefig   .setTriggerHook([](){ new PopupPolefig(); });
    viewsReset     .setTriggerHook([](){ gGui->resetViews(); });

    // Remakes (others are set more conveniently in Mainwindow::refresh):
    corrFile.setRemake([this]() {
            bool hasCorr = gSession->hasCorrFile();
            corrFile.setIcon(QIcon(hasCorr ? ":/icon/rem" : ":/icon/add"));
            QString text = QString(hasCorr ? "Remove" : "Add") + " correction file";
            corrFile.setText(text);
            corrFile.setToolTip(text.toLower()); });
}
