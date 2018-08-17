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
#include "qcr/base/debug.h"
#include <cmath>

namespace {
const double STEP = 0.1;
double myRound(double val, double step=STEP) { return step * std::round(val/step); }
}

RangeControl::RangeControl(
    const QString& _name,
    const std::function<const Range*()>& _getRange,
    const std::function<void(double,bool)>& _setOne)
{
    auto* cellMin = new QcrCell<double>{0.}; // will not be deleted on shutdown
    auto* cellMax = new QcrCell<double>{0.};
    auto* spinMin = new QcrDoubleSpinBox{_name+"Min", cellMin, 5, 2, 0., 89.9};
    auto* spinMax = new QcrDoubleSpinBox{_name+"Max", cellMax, 5, 2, 0., 90.};

    spinMin->setSingleStep(STEP);
    spinMax->setSingleStep(STEP);

    cellMin->setHook([cellMax, _setOne](double& val){
            qDebug() << "cellMin hook" << val;
            val = myRound(qMin(val, myRound(cellMax->val())-STEP));
            qDebug() << ".. rounded" << val;
            _setOne(val, false); });
    cellMax->setHook([cellMin, _setOne](double& val){
            qDebug() << "cellMax hook" << val;
            val = myRound(qMax(val, myRound(cellMin->val())+STEP));
            qDebug() << ".. rounded" << val;
            _setOne(val, true); });

    // layout
    auto hb = new QHBoxLayout();
    hb->addWidget(new QLabel("range"));
    hb->addWidget(spinMin);
    hb->addWidget(new QLabel(".."));
    hb->addWidget(spinMax);
    hb->addWidget(new QLabel("deg"));
    hb->addStretch();
    setLayout(hb);

    setRemake([cellMin, cellMax, spinMin, spinMax, _getRange, this](){
            qDebug() << "RangeControl::remake";
            const Range* range = _getRange();
            setEnabled(range!=nullptr);
            if (!range)
                return;
            qDebug() << "RangeControl::remake ctd with " << range->to_s();
            cellMax->setVal(90);
            cellMin->setVal(0);
            spinMin->programaticallySetValue(range->min);
            spinMax->programaticallySetValue(range->max);
            qDebug() << "RangeControl::remake done";
        });
}
