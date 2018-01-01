// ************************************************************************** //
//
//  Steca2: stress and texture calculator
//
//! @file      gui/panels/panel_diffractogram.h
//! @brief     Defines class Diffractogram
//!
//! @homepage  https://github.com/scgmlz/Steca2
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2017
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef PANEL_DIFFRACTOGRAM_H
#define PANEL_DIFFRACTOGRAM_H

#include "signalling.h" // for eFittingTab

class QAction;
class QBoxLayout;
class QComboBox;
class QRadioButton;
class QDoubleSpinBox;
class QToolButton;
class QAction;

namespace gui {
namespace panel {

class Diffractogram : public QWidget {
public:
    Diffractogram();

    void render();

    data::shp_Dataset dataset() const { return dataset_; }
    QBoxLayout* box() const { return box_; }

private:
    QBoxLayout* box_;
    void onNormChanged();
    void onFittingTab(eFittingTab tab);

    void setDataset(data::shp_Dataset);

    data::shp_Dataset dataset_;

    class DiffractogramPlot* plot_;

    typ::Curve dgram_, dgramBgFitted_, bg_;
    typ::curve_vec refls_;

    uint currReflIndex_;
    calc::shp_Reflection currentReflection_;

    QComboBox* comboNormType_;
    QRadioButton *intenSum_, *intenAvg_;
    QDoubleSpinBox* intenScale_;
    QToolButton* enableZoom_;
    QAction* actZoom_;

public:
    void calcDgram();
    void calcBackground();
    void calcReflections();

    void setCurrReflNewRange(typ::Range const&);
    typ::Range currReflRange() const;
};

} // namespace panel
} // namespace gui

#endif // PANEL_DIFFRACTOGRAM_H
