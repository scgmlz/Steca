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
#include <cmath>

namespace {
double safeReal(double val) { return qIsFinite(val) ? val : 0.0; }
double myRound(double val, double step=0.05) { return step * std::round(val/step); }
}

RangeControl::RangeControl(const QString& _name, const std::function<Range*()>& _selectRange)
    : QcrWidget(_name)
    , spinRangeMin_ {name()+"Min", 5, 2, 0., 89.9}
    , spinRangeMax_ {name()+"Max", 5, 2, 0., 90.}
    , selectRange_(_selectRange)
{
    spinRangeMin_.setSingleStep(.1);
    spinRangeMax_.setSingleStep(.1);

    spinRangeMin_.cell()->setHook([=](double val){
            qDebug() << "MinHook " << val;
            double antival = spinRangeMax_.getValue();
            double newantival = myRound(antival);
            double newval = qMin(myRound(val), antival-.1);
            qDebug() << "1--> " << val << antival;
            if (newval!=val || newantival!=antival) {
                spinRangeMin_.programaticallySetValue(newval);
                spinRangeMax_.programaticallySetValue(newantival);
                qDebug() << "1z";
                //return;
            }
            selectRange_()->set(newval, newantival);
            qDebug() << "2--> " << spinRangeMin_.getValue() << spinRangeMax_.getValue();
            gSession->onBaseline(); // TODO do this via setRange
            qDebug() << "3--> " << spinRangeMin_.getValue() << spinRangeMax_.getValue();
            gRoot->remakeAll("RangeControl/min");
            qDebug() << "4--> " << spinRangeMin_.getValue() << spinRangeMax_.getValue();
        });
    spinRangeMax_.cell()->setHook([=](double val){
            double antival = myRound(spinRangeMin_.getValue());
            val = qMax(myRound(val), antival+.1);
            selectRange_()->set(antival, val);
            gSession->onBaseline(); // TODO do this via setRange
            gRoot->remakeAll("RangeControl/max");
        });

    // layout
    auto hb = new QHBoxLayout();
    hb->addWidget(new QLabel("range"));
    hb->addWidget(&spinRangeMin_);
    hb->addWidget(new QLabel(".."));
    hb->addWidget(&spinRangeMax_);
    hb->addWidget(new QLabel("deg"));
    hb->addStretch();
    setLayout(hb);

    setRemake([this](){
            const Range* range = selectRange_();
            setEnabled(range);
            if (!range)
                return;
            spinRangeMin_.programaticallySetValue(safeReal(range->min));
            spinRangeMax_.programaticallySetValue(safeReal(range->max));
        });
}
