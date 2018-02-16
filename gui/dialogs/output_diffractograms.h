// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/dialogs/output_diffractograms.h
//! @brief     Defines class DiffractogramsFrame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef OUTPUT_DIFFRACTOGRAMS_H
#define OUTPUT_DIFFRACTOGRAMS_H

#include "frame.h"
#include <QWidget>

//! The modal dialog for saving diffractograms.

class DiffractogramsFrame : public QDialog, private CModal {
public:
    DiffractogramsFrame();
    ~DiffractogramsFrame();

private:
    QAction *actClose_, *actCalculate_, *actInterpolate_;
    QToolButton *btnClose_, *btnCalculate_, *btnInterpolate_;
    QProgressBar* progressBar_;
    class PanelPeak* panelPeak;
    class PanelGammaSlices* panelGammaSlices;
    class PanelGammaRange* panelGammaRange;
    class PanelPoints* panelPoints;
    QTabWidget* tabs_;
    vec<PeakInfos> calcPoints_;
    void calculate();

    QWidget tab;
    class TabDiffractogramsSave* tabSave_;

    void save();
    void saveCurrent();
    void saveAll(bool oneFile);
};

#endif // OUTPUT_DIFFRACTOGRAMS_H
