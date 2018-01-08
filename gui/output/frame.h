// ************************************************************************** //
//
//  Steca: stress and texture calculator
//
//! @file      gui/output/frame.h
//! @brief     Defines class Frame
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
// ************************************************************************** //

#ifndef FRAME_H
#define FRAME_H

#include "core/calc/calc_reflection_info.h"
#include <QDialog>

class QBoxLayout;
class QProgressBar;
class QTabWidget;
class QToolButton;

//! A rich modal dialog, for the polefig, diagram, and diffractogram outputs.
class Frame : public QDialog {
public:
    Frame(rcstr title, class Params*, QWidget*);
protected:
    QAction *actClose_, *actCalculate_, *actInterpolate_;
    QToolButton *btnClose_, *btnCalculate_, *btnInterpolate_;
    QProgressBar* pb_;
    QBoxLayout* box_;
    class Params* params_;
    QTabWidget* tabs_;
    vec<ReflectionInfos> calcPoints_, interpPoints_;
    class Table* table_;
    void calculate();
    void interpolate();
    virtual void displayReflection(uint reflIndex, bool interpolated);
    uint getReflIndex() const;
    bool getInterpolated() const;
};

#endif // FRAME_H
