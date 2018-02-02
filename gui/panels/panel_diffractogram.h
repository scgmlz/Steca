// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/panels/panel_diffractogram.h
//! @brief     Defines class Diffractogram
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PANEL_DIFFRACTOGRAM_H
#define PANEL_DIFFRACTOGRAM_H

#include "gui/thehub.h" // for eFittingTab
#include "gui/base/new_q.h"

//! A diffractogram display, with associated controls, for use in SubframeDiffractogram.

class Diffractogram final : public QWidget {
public:
    Diffractogram();

    void render();

    QBoxLayout* box() const { return box_; }

    void calcDgram();
    void calcBackground();
    void calcPeaks();

    void setCurrReflNewRange(const Range&);
    Range currReflRange() const;

private:
    QBoxLayout* box_;
    void onNormChanged();
    void onFittingTab(eFittingTab tab);

    void onHighlight();

    class DiffractogramPlot* plot_;

    Curve dgram_, dgramBgFitted_, bg_;
    curve_vec refls_;

    int currReflIndex_;

    QComboBox* comboNormType_;
    QRadioButton *intenSum_, *intenAvg_;
    QDoubleSpinBox* intenScale_;
    QToolButton* enableZoom_;
    QAction* actZoom_;
};

#endif // PANEL_DIFFRACTOGRAM_H
