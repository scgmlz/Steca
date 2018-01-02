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
#include <QWidget>

class QBoxLayout;
class QComboBox;
class QRadioButton;
class QDoubleSpinBox;
class QToolButton;

namespace gui {
namespace panel {

class Diffractogram : public QWidget {
public:
    Diffractogram();

    void render();

    QSharedPointer<Suite> suite() const { return suite_; }
    QBoxLayout* box() const { return box_; }

private:
    QBoxLayout* box_;
    void onNormChanged();
    void onFittingTab(eFittingTab tab);

    void setSuite(QSharedPointer<Suite>);

    QSharedPointer<Suite> suite_;

    class DiffractogramPlot* plot_;

    Curve dgram_, dgramBgFitted_, bg_;
    curve_vec refls_;

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

    void setCurrReflNewRange(Range const&);
    Range currReflRange() const;
};

} // namespace panel
} // namespace gui

#endif // PANEL_DIFFRACTOGRAM_H
