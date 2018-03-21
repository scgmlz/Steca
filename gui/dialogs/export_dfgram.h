// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/export_dfgram.h
//! @brief     Defines class ExportDfgram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef EXPORT_DFGRAM_H
#define EXPORT_DFGRAM_H

#include "frame.h"
#include <QWidget>

//! The modal dialog for saving diffractograms.

class ExportDfgram : public QDialog, private CModal {
public:
    ExportDfgram();
    ~ExportDfgram();
    void onCommand(const QStringList&);

private:
    QProgressBar* progressBar_;
    class PanelGammaSlices* panelGammaSlices;
    class PanelGammaRange* panelGammaRange;
    class TabDiffractogramsSave* tabSave_;

    void save();
    void saveCurrent();
    void saveAll(bool oneFile);
};

#endif // EXPORT_DFGRAM_H
