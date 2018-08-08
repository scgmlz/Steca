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

#include "range_control.h"
#include "core/session.h"
#include "qcr/widgets/controls.h"
#include <cmath>

namespace {
double safeReal(double val) { return qIsFinite(val) ? val : 0.0; }
double myRound(double val, double step=0.05) { return step * std::round(val/step); }
}

RangeControl::RangeControl(const QString& _name, const std::function<Range*()>& _selectRange)
    : QcrWidget(_name)
    , selectRange_(_selectRange)
{
    QcrCell<double> cellMin {0};
    QcrCell<double> cellMax {0};
    auto* spinMin = new QcrDoubleSpinBox{name()+"Min", &cellMin, 5, 2, 0., 89.9};
    auto* spinMax = new QcrDoubleSpinBox{name()+"Max", &cellMax, 5, 2, 0., 90.};

    spinMin->setSingleStep(.1);
    spinMax->setSingleStep(.1);

    cellMin.setHook([=](double val){
            qDebug() << "MinHook " << val;
            double antival = spinMax->getValue();
            double newantival = myRound(antival);
            double newval = qMin(myRound(val), antival-.1);
            qDebug() << "1--> " << val << antival;
            if (newval!=val || newantival!=antival) {
                spinMin->programaticallySetValue(newval);
                spinMax->programaticallySetValue(newantival);
                qDebug() << "1z";
                //return;
            }
            selectRange_()->set(newval, newantival);
            qDebug() << "2--> " << spinMin->getValue() << spinMax->getValue();
            gSession->onBaseline(); // TODO do this via setRange
            qDebug() << "3--> " << spinMin->getValue() << spinMax->getValue();
            gRoot->remakeAll("RangeControl/min");
            qDebug() << "4--> " << spinMin->getValue() << spinMax->getValue();
        });
    cellMax.setHook([=](double val){
            double antival = myRound(spinMin->getValue());
            val = qMax(myRound(val), antival+.1);
            selectRange_()->set(antival, val);
            gSession->onBaseline(); // TODO do this via setRange
            gRoot->remakeAll("RangeControl/max");
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

    setRemake([this,spinMin,spinMax](){
            const Range* range = selectRange_();
            setEnabled(range);
            if (!range)
                return;
            spinMin->programaticallySetValue(safeReal(range->min));
            spinMax->programaticallySetValue(safeReal(range->max));
        });
}
