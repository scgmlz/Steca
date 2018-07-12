//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/actions/triggers.h
//! @brief     Defines class Triggers
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#ifndef TRIGGERS_H
#define TRIGGERS_H

#include "qcr/widgets/controls.h"

//! Collection of trigger actions, for use as member of MainWin.

class Triggers : private QObject {
public:
    Triggers();
    QcrTrigger about {"about", "About Steca"};
    QcrTrigger addFiles {"addFiles", "Add files...", ":/icon/add", Qt::CTRL | Qt::Key_O};
    QcrTrigger addPeak {"addPeak", "Add peak", ":/icon/add"};
    QcrTrigger checkUpdate {"checkUpdate", "Check for update"};
    QcrTrigger clearBackground {"clearBackground", "Clear baseline ranges", ":/icon/clear"};
    QcrTrigger clearPeaks {"clearPeaks", "Clear peaks", ":/icon/clear"};
    QcrTrigger clearSession {"clearSession", "Clear session"};
    QcrTrigger corrFile {"loadCorr", "Add correction file...", ":/icon/add",
            Qt::SHIFT | Qt::CTRL | Qt::Key_O};
    QcrTrigger exportDfgram {"exportDfgram", "Export diffractogram(s)...", ":/icon/filesave" };
    QcrTrigger exportPolefig {"exportPolefig", "Export pole figure...", ":/icon/filesave" };
    QcrTrigger exportBigtable {"exportBigtable", "Export fit result table...", ":/icon/filesave" };
    QcrTrigger exportDiagram {"exportDiagram", "Export diagram...", ":/icon/filesave" };
    QcrTrigger spawnTable {"spawnTable", "Spawn table...", ":/icon/window" };
    QcrTrigger spawnDiagram {"spawnDiagram", "Spawn diagram...", ":/icon/window" };
    QcrTrigger spawnPolefig {"spawnPolefig", "Spawn pole figure...", ":/icon/window" };
    QcrTrigger loadSession {"loadSession", "Load session..."};
    QcrTrigger online  {"online", "Open docs in external browser"};
    QcrTrigger quit {"quit", "Quit", "", QKeySequence::Quit};
    QcrTrigger removeFile {"removeFile", "Remove highlighted file", ":/icon/rem",
            QKeySequence::Delete};
    QcrTrigger removePeak {"removePeak", "Remove peak", ":/icon/rem"};
    QcrTrigger removeBaserange {"removeBaserange", "Remove baseline range", ":/icon/rem"};
    QcrTrigger saveSession {"saveSession", "Save session..."};
    QcrTrigger viewReset {"viewReset", "Reset"};
};

#endif // TRIGGERS_H
