//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/load_data.cpp
//! @brief     Implements functions in namespace loadData
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/dialogs/load_data.h"
#include "core/session.h"
#include "core/misc/async.h"
#include "core/misc/exception.h"
#include "gui/dialogs/file_dialog.h"
#include "qcr/base/debug.h" // warning
#include <QDir>
#include <QStringBuilder> // for ".." % ..

namespace {
QDir dataDir_ {QDir::homePath()};
const QString dataFormats {"Data files (*.dat *.yaml *.mar*);;All files (*.*)"};
} // namespace

void loadData::addFiles(QWidget* parent)
{
    QStringList fileNames
        = file_dialog::queryImportFileNames(parent, "Add files", dataDir_, dataFormats);
    // gGui->repaint();
    if (fileNames.isEmpty())
        return;
    TakesLongTime __("addFiles");
    try {
        gSession->dataset.addGivenFiles(fileNames);
    } catch (const Exception& ex) {
        qWarning() << ex.msg();
    }
}

void loadData::loadCorrFile(QWidget* parent)
{
    // TODO catch THROW's
    if (gSession->corrset.hasFile()) {
        gSession->corrset.removeFile();
    } else {
        QString fileName = file_dialog::queryImportFileName(
            parent, "Set correction file", dataDir_, dataFormats);
        if (fileName.isEmpty())
            return;
        try {
            gSession->corrset.loadFile(fileName);
        } catch (const Exception& ex) {
            qWarning() << ex.msg();
        }
    }
}
