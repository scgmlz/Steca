//  ***********************************************************************************************
//
//  Steca: stress and texture calculator
//
//! @file      gui/view/range_control.cpp
//! @brief     Implements class RangeControl
//!
//! @homepage  https://github.com/scgmlz/Steca
//! @license   GNU General Public License v3 or higher (see COPYING)
//! @copyright Forschungszentrum Jülich GmbH 2016-2018
//! @authors   Scientific Computing Group at MLZ (see CITATION, MAINTAINER)
//
//  ***********************************************************************************************

#include "gui/view/range_control.h"
#include "core/session.h"
#include "qcr/widgets/controls.h"
#include <cmath>

namespace {
const double STEP = 0.1;
//double safeReal(double val) { return qIsFinite(val) ? val : 0.0; }
double myRound(double val, double step=STEP) { return step * std::round(val/step); }
}

RangeControl::RangeControl(const QString& _name, const std::function<Range*()>& _selectRange)
    : selectRange_(_selectRange)
{
    auto* cellMin = new QcrCell<double>{0.};
    auto* cellMax = new QcrCell<double>{0.};
    auto* spinMin = new QcrDoubleSpinBox{_name+"Min", cellMin, 5, 2, 0., 89.9};
    auto* spinMax = new QcrDoubleSpinBox{_name+"Max", cellMax, 5, 2, 0., 90.};

    spinMin->setSingleStep(STEP);
    spinMax->setSingleStep(STEP);

    cellMin->setHook([cellMax, this](double& val){
            val = myRound(qMin(val, myRound(cellMax->val())-STEP));
            gSession->onBaseline(); // TODO do this via setRange
            selectRange_()->setOne(val, false);
        });
    cellMax->setHook([cellMin, this](double& val){
            val = myRound(qMax(val, myRound(cellMin->val())+STEP));
            gSession->onBaseline(); // TODO do this via setRange
            selectRange_()->setOne(val, true);
        });

    // layout
    auto hb = new QHBoxLayout();
    hb->addWidget(new QLabel("range"));
    hb->addWidget(spinMin);
    hb->addWidget(new QLabel(".."));
    hb->addWidget(spinMax);
    hb->addWidget(new QLabel("deg"));
    hb->addStretch();
    setLayout(hb);

    setRemake([spinMin, spinMax, this](){
            const Range* range = selectRange_();
            setEnabled(range);
            if (!range)
                return;
            spinMin->programaticallySetValue(range->min);
            spinMax->programaticallySetValue(range->max);
        });
}
