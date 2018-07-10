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

namespace {
double safeReal(double val) { return qIsFinite(val) ? val : 0.0; }
}

RangeControl::RangeControl(const QString& _name, const std::function<Range*()>& _selectRange)
    : QcrWidget(_name)
    , spinRangeMin_ {name()+"Min", 6, 0., 89.9}
    , spinRangeMax_ {name()+"Max", 6, 0., 90.}
    , selectRange_(_selectRange)
{
    spinRangeMin_.setSingleStep(.1);
    spinRangeMax_.setSingleStep(.1);

    // outbound connections
    connect(&spinRangeMin_, &QcrDoubleSpinBox::valueReleased, [this](double val) {
            double antival = qMax(spinRangeMax_.getValue(), val);
            selectRange_()->set(val, antival); });
    connect(&spinRangeMax_, &QcrDoubleSpinBox::valueReleased, [this](double val) {
            double antival = qMin(spinRangeMin_.getValue(), val);
            selectRange_()->set(antival, val); });

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
