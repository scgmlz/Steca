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
//#include "qcr/base/debug.h"
#include <cmath>

const double RangeControl::STEP {0.1};

RangeControl::RangeControl(
    const QString& _name,
    const std::function<const Range*()>& _getRange,
    const std::function<void(double,bool)>& _setOne)
{
    const Range* range = _getRange();
    auto* cellMin = new QcrCell<double>{range->min}; // will not be deleted on shutdown
    auto* cellMax = new QcrCell<double>{range->max};
    auto* spinMin = new QcrDoubleSpinBox{_name+"Min", cellMin, 5, 2, 0., 90.};
    auto* spinMax = new QcrDoubleSpinBox{_name+"Max", cellMax, 5, 2, 0., 90.};

    auto hb = new QHBoxLayout();
    hb->addWidget(new QLabel("range"));
    hb->addWidget(spinMin);
    hb->addWidget(new QLabel(".."));
    hb->addWidget(spinMax);
    hb->addWidget(new QLabel("deg"));
    hb->addStretch();
    setLayout(hb);

    spinMin->setSingleStep(STEP);
    spinMax->setSingleStep(STEP);

    cellMin->setHook([_setOne](const double val){_setOne(val, false); });
    cellMax->setHook([_setOne](const double val){_setOne(val, true ); });

    setRemake([cellMin, cellMax, spinMin, spinMax, _getRange, this](){
            const Range* range = _getRange();
            setEnabled(range!=nullptr);
            if (!range)
                return;
            cellMin->pureSetVal(range->min);
            cellMax->pureSetVal(range->max);
            spinMin->setMaximum(range->max-STEP);
            spinMax->setMinimum(range->min+STEP);
        });
}
